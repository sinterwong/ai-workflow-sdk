#include "inference_node.hpp"
#include "utils/mexception.hpp" // For MEXCEPTION
#include <vector>

namespace ai_pipe {

InferenceNode::InferenceNode(const std::string& name) : NodeBase(name) {}

void InferenceNode::process(const PortDataMap& inputs, PortDataMap& outputs,
                               std::shared_ptr<PipelineContext> context) {
    if (inputs.find("raw_image_data") == inputs.end()) {
        throw MEXCEPTION("InferenceNode: Missing 'raw_image_data' input.");
    }

    const auto& input_data_packet = inputs.at("raw_image_data");
    if (!input_data_packet->has<RawImageData>()) {
        throw MEXCEPTION("InferenceNode: 'raw_image_data' input is not of type RawImageData.");
    }

    const RawImageData& raw_image = input_data_packet->get<RawImageData>();

    // Simulate object detection
    InferenceResult result;
    if (!raw_image.image.empty()) {
        // Simulate detecting two objects
        result.boxes.push_back({50, 50, 100, 100, 0.9f, 1}); // x, y, w, h, score, label
        result.boxes.push_back({200, 100, 150, 120, 0.8f, 2});
    }

    auto inference_result_data = std::make_shared<PortData>();
    inference_result_data->set(result);
    outputs["inference_result"] = inference_result_data;
}

std::vector<std::string> InferenceNode::getExpectedInputPorts() const {
    return {"raw_image_data"};
}

std::vector<std::string> InferenceNode::getExpectedOutputPorts() const {
    return {"inference_result"};
}

} // namespace ai_pipe
