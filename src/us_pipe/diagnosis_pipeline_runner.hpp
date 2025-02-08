#ifndef __US_PIPE_DIAGNOSIS_PIPELINE_RUNNER_HPP__
#define __US_PIPE_DIAGNOSIS_PIPELINE_RUNNER_HPP__

#include "logger/logger.hpp"
#include "pipe_types.hpp"
#include "utils/template_utils.hpp"
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>

using TimePoint = std::chrono::_V2::system_clock::time_point;

namespace us_pipe {

template <class CallBackFuncT, class PipelineTypeT>
class DiagnosisPipelineRunner {
private:
  using PipelineDetectFuncType = decltype(&PipelineTypeT::detect_single_frame);
  using VectorOfDetectResultT =
      std::invoke_result_t<PipelineDetectFuncType, PipelineTypeT,
                           const Frame &>;
  // 为了兼容 pipeline_new 返回 DetResult
  using DetectResultT =
      typename utils::tpl::get_vector_element_type<VectorOfDetectResultT>::type;

  // 帧, 回调函数, 数据输入的时间
  std::queue<std::tuple<std::shared_ptr<Frame>, CallBackFuncT, TimePoint>>
      _frame_queue;
  // 帧, 回调函数, 数据输入的时间, 检测结果, 开始做检测的时间
  std::queue<std::tuple<std::shared_ptr<Frame>, CallBackFuncT, TimePoint,
                        std::unique_ptr<std::vector<DetectResultT>>, TimePoint>>
      _message_queue;

  using PipelineProcessFuncType =
      decltype(&PipelineTypeT::process_single_frame);
  using VectorOfDisplayResultT =
      std::invoke_result_t<PipelineProcessFuncType, PipelineTypeT,
                           const Frame &>;
  using DisplayResultT = typename utils::tpl::get_vector_element_type<
      VectorOfDisplayResultT>::type;
  std::vector<DisplayResultT> _last_display_results;

  using VectorOfOutterDisplayResultT = std::decay_t<
      typename utils::tpl::get_first_arg_type<CallBackFuncT>::type>;
  using OutterDisplayResultT = typename utils::tpl::get_vector_element_type<
      VectorOfOutterDisplayResultT>::type;
  std::vector<OutterDisplayResultT> _last_outter_display_results;

  std::mutex _frame_queue_mutex;
  std::mutex _message_queue_mutex;

  std::condition_variable _frame_queue_condition;
  std::condition_variable _message_queue_condition;

  std::thread _detect_thread;
  std::thread _diagnose_thread;

  std::shared_ptr<PipelineTypeT> _diagnosis_pipeline;

  bool _stop_processing = false;
  bool _adaptive_stride; // 是否进行跳帧操作;

  int _det_skip_num = 0;
  double _det_delta_time_sum = 0.0;
  int _diag_skip_num = 0;
  double _diag_delta_time_sum = 0.0;

  static const int TARGET_FPS = 30;
  static constexpr double TARGET_LATENCY = 1000. / TARGET_FPS;

public:
  DiagnosisPipelineRunner(const std::shared_ptr<PipelineTypeT> &ppl,
                          bool adaptive_stride)
      : _diagnosis_pipeline(ppl), _adaptive_stride(adaptive_stride) {
    _detect_thread = std::thread(&DiagnosisPipelineRunner::detect, this);
    _diagnose_thread = std::thread(&DiagnosisPipelineRunner::diagnose, this);
  }

  ~DiagnosisPipelineRunner() {
    _stop_processing = true;

    _frame_queue_condition.notify_all();
    _message_queue_condition.notify_all();

    if (_detect_thread.joinable())
      _detect_thread.join();
    if (_diagnose_thread.joinable())
      _diagnose_thread.join();

    // std::queue 没有 clear() 方法, 置空即可
    _frame_queue = std::queue<
        std::tuple<std::shared_ptr<Frame>, CallBackFuncT, TimePoint>>();
    _message_queue = std::queue<
        std::tuple<std::shared_ptr<Frame>, CallBackFuncT, TimePoint,
                   std::unique_ptr<std::vector<DetectResultT>>, TimePoint>>();

    _last_display_results.clear();
    _last_outter_display_results.clear();
  }

  // 1230版:开放是否跳帧接口;
  void set_skip_op(bool is_skip) {
    std::lock_guard<std::mutex> guard{_frame_queue_mutex};
    this->_adaptive_stride = is_skip;
    LOGGER_INFO("set skip operation = {}", is_skip);
  }

  void enqueue_data(const std::shared_ptr<Frame> &frame,
                    const CallBackFuncT &func) {
    std::lock_guard<std::mutex> guard{_frame_queue_mutex};

    TimePoint in_queue_time = std::chrono::high_resolution_clock::now();
    _frame_queue.emplace(std::make_tuple(frame, func, in_queue_time));
    _frame_queue_condition.notify_one();
  }

  void reset() { _diagnosis_pipeline->reset(); }

  std::vector<OutterDisplayResultT> get_outter_display_result() {
    return _last_outter_display_results;
  }

private:
  void detect() {
    for (;;) {
      std::tuple<std::shared_ptr<Frame>, CallBackFuncT, TimePoint> frame_data;
      {
        std::unique_lock<std::mutex> guard{_frame_queue_mutex};
        _frame_queue_condition.wait(guard, [this]() {
          return (!_frame_queue.empty()) || _stop_processing;
        });

        if (_stop_processing && _frame_queue.empty())
          return;

        frame_data = std::move(_frame_queue.front());
        _frame_queue.pop();
      }

      const std::shared_ptr<Frame> &p_frame = std::get<0>(frame_data);
      CallBackFuncT func = std::get<1>(frame_data);
      TimePoint in_queue_time = std::get<2>(frame_data);

      const auto frame = *p_frame;
      std::unique_ptr<std::vector<DetectResultT>> p_det_results = nullptr;

      double calculation_time = 0.0, delay_time = 0.0;
      auto det_start_time = std::chrono::high_resolution_clock::now();

      // 跳帧的逻辑
      if (_adaptive_stride && _det_skip_num > 0) {
        _det_skip_num -= 1;

        auto det_end_time = std::chrono::high_resolution_clock::now();
        delay_time = std::chrono::duration<double, std::milli>(det_end_time -
                                                               in_queue_time)
                         .count();
        calculation_time = std::chrono::duration<double, std::milli>(
                               det_end_time - det_start_time)
                               .count();

        // !_adaptive_stride 不跳帧, 或 _skip_num == 0 不需要跳帧
      } else {
        auto det_results = _diagnosis_pipeline->detect_single_frame(frame);
        p_det_results = std::make_unique<std::vector<DetectResultT>>(
            std::move(det_results));

        auto det_end_time = std::chrono::high_resolution_clock::now();
        auto det_latency = std::chrono::duration<double, std::milli>(
                               det_end_time - in_queue_time)
                               .count();

        delay_time = det_latency;
        calculation_time = std::chrono::duration<double, std::milli>(
                               det_end_time - det_start_time)
                               .count();

        _det_delta_time_sum += (det_latency - TARGET_LATENCY);
        _det_delta_time_sum = std::max(0.0, _det_delta_time_sum);

        _det_skip_num = std::floor(_det_delta_time_sum / TARGET_LATENCY);
        _det_delta_time_sum -= _det_skip_num * TARGET_LATENCY;

        if (_det_delta_time_sum > TARGET_LATENCY / 2) {
          _det_skip_num += 1;
          _det_delta_time_sum = 0.0;
        }
      }

      {
        std::lock_guard<std::mutex> guard{_message_queue_mutex};

        auto det_end_time = std::chrono::high_resolution_clock::now();
        auto message = std::make_tuple(std::move(p_frame), func, in_queue_time,
                                       std::move(p_det_results), det_end_time);

        _message_queue.emplace(std::move(message));
        _message_queue_condition.notify_one();
      }
      LOGGER_INFO("[Detect] Frame {}: delay: {}ms, cal: {}ms, queue len: {}, "
                  "skip: {}, _delta_time_sum: {}",
                  frame.index, delay_time, calculation_time,
                  _frame_queue.size(), _det_skip_num, _det_delta_time_sum);
    }
  }

  void diagnose() {
    for (;;) {
      std::tuple<std::shared_ptr<Frame>, CallBackFuncT, TimePoint,
                 std::unique_ptr<std::vector<DetectResultT>>, TimePoint>
          message_data;
      {
        std::unique_lock<std::mutex> guard{_message_queue_mutex};
        _message_queue_condition.wait(guard, [this]() {
          return (_message_queue.size() > 0) || _stop_processing;
        });

        if (_stop_processing && _message_queue.empty())
          return;

        message_data = std::move(_message_queue.front());
        _message_queue.pop();
      }

      const auto &[p_frame, cbk_func, in_queue_time, p_det_results,
                   det_end_time] = message_data;
      const auto frame = *p_frame;

      double calculation_time = 0.0, delay_time = 0.0;
      auto diag_start_time = std::chrono::high_resolution_clock::now();

      // 因为检测跳帧而跳帧, 或者检测完发现需要跳帧
      if (_adaptive_stride &&
          (p_det_results == nullptr || _diag_skip_num > 0)) {
        cbk_func(_last_outter_display_results, p_frame->index, p_frame->roi,
                 true);
        auto diag_end_time = std::chrono::high_resolution_clock::now();
        delay_time = std::chrono::duration<double, std::milli>(diag_end_time -
                                                               in_queue_time)
                         .count();
        calculation_time = std::chrono::duration<double, std::milli>(
                               diag_end_time - diag_start_time)
                               .count();

        if (_diag_skip_num > 0)
          _diag_skip_num -= 1;

        // !_adaptive_stride 不跳帧, 或 _skip_num == 0 不需要跳帧
      } else {
        // 可以确保这个指针不为空
        auto det_results = *p_det_results;
        _last_display_results =
            _diagnosis_pipeline->diagnose_single_frame(frame, det_results);

        _last_outter_display_results.clear();
        for (const auto &display_result : _last_display_results) {
          // if constexpr (std::is_same<PipelineTypeT,
          // ThyroidDiagnosisPipeline>::value) {
          //     std::cout<<"runner-debug, index="<<p_frame->index<<
          //     ",
          //     c_tirads="<<static_cast<int>(display_result.diag_tirads->c_type_tirads)<<std::endl;
          // }

          _last_outter_display_results.push_back(
              OutterDisplayResultT::convert_from_display_result(
                  display_result));
        }

        cbk_func(_last_outter_display_results, p_frame->index, p_frame->roi,
                 false);

        auto diag_end_time = std::chrono::high_resolution_clock::now();
        auto diag_latency =
            std::chrono::duration<double>(diag_end_time - det_end_time)
                .count() *
            1000;

        delay_time = std::chrono::duration<double, std::milli>(diag_end_time -
                                                               in_queue_time)
                         .count();
        calculation_time = std::chrono::duration<double, std::milli>(
                               diag_end_time - diag_start_time)
                               .count();

        _diag_delta_time_sum += (diag_latency - TARGET_LATENCY);
        _diag_delta_time_sum = std::max(0.0, _diag_delta_time_sum);

        _diag_skip_num = std::floor(_diag_delta_time_sum / TARGET_LATENCY);
        _diag_delta_time_sum -= _diag_skip_num * TARGET_LATENCY;

        if (_diag_delta_time_sum > TARGET_LATENCY / 2) {
          _diag_skip_num += 1;
          _diag_delta_time_sum = 0.0;
        }
      }
      LOGGER_INFO("[Diagnose] Frame {}: delay: {}ms, cal: {}ms, queue len: {}, "
                  "skip: {}, _delta_time_sum: {}",
                  frame.index, delay_time, calculation_time,
                  _message_queue.size(), _diag_skip_num, _diag_delta_time_sum);
    }
  }
};

} // namespace us_pipe

#endif