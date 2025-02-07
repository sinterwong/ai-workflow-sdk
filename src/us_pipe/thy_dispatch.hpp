#ifndef US_PIPE_THYROID_DISPATCH_HPP
#define US_PIPE_THYROID_DISPATCH_HPP

#include "status_pipe.hpp"
#include "thy_pipe.hpp"
#include "thy_types.hpp" // 包含 Frame、ThyroidInsu、VideoRepr 等定义
#include "utils/thread_pool.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <opencv2/core.hpp>
#include <string>
#include <vector>

namespace us_pipe {

struct GlobalModules {

  std::unique_ptr<StatusPipeline> video_status_ppl;
  std::unique_ptr<utils::thread_pool> thread_pool;

  void init(const std::string &model_folder) {
    thread_pool = std::make_unique<utils::thread_pool>();
    thread_pool->start(1);
    video_status_ppl = std::make_unique<StatusPipeline>(model_folder, 0);
  }

  void release() {
    thread_pool.reset();
    video_status_ppl.reset();
  }
};

struct ThyroidInsuranceModules {
  std::shared_ptr<ThyroidInsurancePipeline> thyroid_insu_ppl;
  //   std::unique_ptr<DiagnosisPipelineRunner<ThyroidInsuranceCallBack,
  //                                           ThyroidInsurancePipeline>>
  //       thyroid_insu_ppl_runner;

  void init(const std::string &model_folder, const bool &adaptivate_stride) {
    // TODO: complete pipeline config
    ThyroidInsurancePipelineConfig config;
    thyroid_insu_ppl = std::make_shared<ThyroidInsurancePipeline>(config);
    // thyroid_insu_ppl_runner =
    //     std::make_unique<DiagnosisPipelineRunner<ThyroidInsuranceCallBack,
    //                                              ThyroidInsurancePipeline>>(
    //         thyroid_insu_ppl, adaptivate_stride);
  }

  void release() {
    // thyroid_insu_ppl_runner.reset();
    thyroid_insu_ppl.reset();
  }
};

class ThyroidDispatch {
public:
  using InsuranceCallback = std::function<void(std::vector<ThyroidInsu> &, int,
                                               const cv::Rect2i &, bool)>;
  using SummaryCallback = std::function<void(VideoRepr &)>;

  ThyroidDispatch();
  ~ThyroidDispatch();

  ThyroidDispatch(const ThyroidDispatch &) = delete;
  ThyroidDispatch &operator=(const ThyroidDispatch &) = delete;

  void init(const std::string &model_folder, bool adaptivate_stride = true);

  void reset();

  void release();

  void registerInsuranceCallback(const InsuranceCallback &callback);

  void registerSummaryCallback(const SummaryCallback &callback);

  void processFrame(const std::shared_ptr<Frame> &frame);

  cv::Rect getCurrentRoi();

  void summary();

private:
  void dispatchCalc(const std::shared_ptr<Frame> &frame);
  void processInsuranceDetection(const std::shared_ptr<Frame> &frame);
  void calculateROI(const std::shared_ptr<Frame> &frame);

  std::mutex _mutex;

  GlobalModules _global_modules;
  ThyroidInsuranceModules _thyroid_insu_modules;

  cv::Rect _det_roi;

  InsuranceCallback _insurance_cb;
  SummaryCallback _summary_cb;
};

} // namespace us_pipe

#endif // US_PIPE_THYROID_DISPATCH_HPP