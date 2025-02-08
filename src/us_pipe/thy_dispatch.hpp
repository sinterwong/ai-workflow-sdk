#ifndef US_PIPE_THYROID_DISPATCH_HPP
#define US_PIPE_THYROID_DISPATCH_HPP

#include "diagnosis_pipeline_runner.hpp"
#include "global_module.hpp"
#include "thy_pipe.hpp"
#include "thy_types.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <opencv2/core.hpp>
#include <vector>

namespace us_pipe {

class ThyroidDispatch {
public:
  using ThyCBK = std::function<void(std::vector<ThyroidInsu> &, int,
                                    const cv::Rect2i &, bool)>;

  using SummaryCallback = std::function<void(VideoRepr &)>;

  ThyroidDispatch();
  ~ThyroidDispatch();

  ThyroidDispatch(const ThyroidDispatch &) = delete;
  ThyroidDispatch &operator=(const ThyroidDispatch &) = delete;

  void init(const ThyDispatchConfig &);
  void reset();
  void release();

  void registerInsuranceCallback(const ThyCBK &callback);
  void registerSummaryCallback(const SummaryCallback &callback);

  void processFrame(const std::shared_ptr<Frame> &frame);
  cv::Rect getCurrentRoi();
  void summary();

private:
  void dispatchCalc(const std::shared_ptr<Frame> &frame);
  void processInsuranceDetection(const std::shared_ptr<Frame> &frame);
  void calculateROI(const std::shared_ptr<Frame> &frame);

  std::mutex mtx;

  GlobalModules gblModule;

  std::shared_ptr<ThyroidInsurancePipeline> thyPipe;
  std::unique_ptr<DiagnosisPipelineRunner<ThyCBK, ThyroidInsurancePipeline>>
      pplRunner;

  cv::Rect curDetRoi;
  ThyCBK insuCb;
  SummaryCallback summaryCb;
};

} // namespace us_pipe

#endif // US_PIPE_THYROID_DISPATCH_HPP