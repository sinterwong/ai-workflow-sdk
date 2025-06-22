#include "vision_inference_node.hpp"
#include "core/infer_types.hpp"
#include "logger/logger.hpp"
#include "types/pipe_data_types.hpp"
#include "utils/mexception.hpp"

#include <vector>

namespace ai_pipe {
using namespace utils::exception;
VisionInferenceNode::VisionInferenceNode(
    const std::string &name, const VisionInferenceNodeParams &params)
    : NodeBase(name), params_(params) {

  if (params_.modelName.empty()) {
    LOG_ERRORS << "VisionInferenceNode: Missing 'model_name' parameter.";
    throw InvalidValueException(
        "VisionInferenceNode: Missing 'model_name' parameter.");
  }
}

void VisionInferenceNode::process(const PortDataMap &inputs,
                                  PortDataMap &outputs,
                                  std::shared_ptr<PipelineContext> context) {
  const std::string inputPortName = getExpectedInputPorts()[0];
  const std::string ouputPortName = getExpectedOutputPorts()[0];

  if (inputs.find(inputPortName) == inputs.end()) {
    LOG_ERRORS << "VisionInferenceNode: Missing '" << inputPortName
               << "' input.";
    throw InvalidValueException("VisionInferenceNode: Missing '" +
                                inputPortName + "' input.");
  }

  if (!context->isValid()) {
    LOG_ERRORS << "VisionInferenceNode: Pipeline context is invalid.";
    throw InvalidValueException(
        "VisionInferenceNode: Pipeline context is invalid.");
  }

  const auto &algoManager = context->getAlgoManager();
  if (!algoManager) {
    LOG_ERRORS
        << "VisionInferenceNode: AlgoManager is not set in pipeline context.";
    throw InvalidValueException(
        "VisionInferenceNode: AlgoManager is not set in pipeline context.");
  }

  if (!algoManager->hasAlgo(params_.modelName)) {
    LOG_ERRORS << "VisionInferenceNode: Model '" << params_.modelName
               << "' not registered with AlgoManager.";
    throw InvalidValueException("VisionInferenceNode: Model '" +
                                params_.modelName +
                                "' not registered with AlgoManager.");
  }

  const auto &inputDataPacket = inputs.at(inputPortName);
  if (!inputDataPacket->has<ImageFramePtr>("image_data")) {
    LOG_ERRORS << "VisionInferenceNode: '" << inputPortName
               << "' input is not of type ImageFrame.";
    throw InvalidValueException("VisionInferenceNode: '" + inputPortName +
                                "' input is not of type ImageFrame.");
  }

  const ImageFramePtr &imageData =
      inputDataPacket->getParam<ImageFramePtr>("image_data");

  // make input data
  // TODO: maybe a dedicated node can be set up later to complete this step
  infer::AlgoInput algoInput;
  infer::FrameInput frameInput;
  frameInput.image = imageData->data;
  frameInput.args.originShape = {imageData->data.cols, imageData->data.rows};
  frameInput.args.roi = {0, 0, imageData->data.cols, imageData->data.rows};
  frameInput.args.isEqualScale = true;
  frameInput.args.pad = {0, 0, 0};
  frameInput.args.meanVals = {0, 0, 0};
  frameInput.args.normVals = {255.f, 255.f, 255.f};
  algoInput.setParams(frameInput);

  infer::AlgoOutput result;
  infer::InferErrorCode inferRet =
      algoManager->infer(params_.modelName, algoInput, result);
  if (inferRet != infer::InferErrorCode::SUCCESS) {
    LOG_ERRORS << "VisionInferenceNode: Inference failed for model '"
               << params_.modelName << "'. Error: " << (int)inferRet;
    throw InferenceException(
        "VisionInferenceNode: Inference failed for model '" +
        params_.modelName + "'.");
  }

  auto inference_result_data_packet = std::make_shared<PortData>();
  inference_result_data_packet->setParam<infer::AlgoOutput>("infer_result",
                                                            result);
  outputs[ouputPortName] = inference_result_data_packet;
}

std::vector<std::string> VisionInferenceNode::getExpectedInputPorts() const {
  return {"raw_image_input"};
}

std::vector<std::string> VisionInferenceNode::getExpectedOutputPorts() const {
  return {"inference_output_result"};
}
} // namespace ai_pipe
