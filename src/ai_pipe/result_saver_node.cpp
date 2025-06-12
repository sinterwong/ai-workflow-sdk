#include "result_saver_node.hpp"
#include "utils/mexception.hpp" // For MEXCEPTION
#include <iostream> // For std::cout

namespace ai_pipe {

ResultSaverNode::ResultSaverNode(const std::string& name) : NodeBase(name) {}

void ResultSaverNode::process(const PortDataMap& inputs, PortDataMap& outputs,
                                 std::shared_ptr<PipelineContext> context) {
    if (inputs.find("inference_result") == inputs.end()) {
        throw MEXCEPTION("ResultSaverNode: Missing 'inference_result' input.");
    }

    const auto& input_data_packet = inputs.at("inference_result");
    if (!input_data_packet->has<InferenceResult>()) {
        throw MEXCEPTION("ResultSaverNode: 'inference_result' input is not of type InferenceResult.");
    }

    const InferenceResult& result = input_data_packet->get<InferenceResult>();

    std::cout << "Inference Results:" << std::endl;
    for (const auto& box : result.boxes) {
        std::cout << "  Label: " << box.label
                  << ", Score: " << box.score
                  << ", BBox: [" << box.x << ", " << box.y << ", "
                  << box.width << ", " << box.height << "]" << std::endl;
    }
    // This node doesn't add to 'outputs' as it's a sink for this data.
}

std::vector<std::string> ResultSaverNode::getExpectedInputPorts() const {
    return {"inference_result"};
}

} // namespace ai_pipe
