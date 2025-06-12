#include "visualization_node.hpp"
#include "utils/mexception.hpp" // For MEXCEPTION
#include <opencv2/imgproc.hpp> // For cv::rectangle, cv::putText
#include <opencv2/imgcodecs.hpp> // For cv::imwrite
#include <filesystem> // Required for std::filesystem::path

namespace ai_pipe {

VisualizationNode::VisualizationNode(const std::string& name) : NodeBase(name) {}

void VisualizationNode::process(const PortDataMap& inputs, PortDataMap& outputs,
                                   std::shared_ptr<PipelineContext> context) {
    if (inputs.find("raw_image_data") == inputs.end() ||
        inputs.find("inference_result") == inputs.end()) {
        throw MEXCEPTION("VisualizationNode: Missing 'raw_image_data' or 'inference_result' input.");
    }

    const auto& raw_image_packet = inputs.at("raw_image_data");
    if (!raw_image_packet->has<RawImageData>()) {
        throw MEXCEPTION("VisualizationNode: 'raw_image_data' input is not of type RawImageData.");
    }
    const RawImageData& raw_image_data = raw_image_packet->get<RawImageData>();

    const auto& inference_result_packet = inputs.at("inference_result");
    if (!inference_result_packet->has<InferenceResult>()) {
        throw MEXCEPTION("VisualizationNode: 'inference_result' input is not of type InferenceResult.");
    }
    const InferenceResult& inference_result = inference_result_packet->get<InferenceResult>();

    cv::Mat visualized_image = raw_image_data.image.clone();

    for (const auto& box : inference_result.boxes) {
        cv::rectangle(visualized_image, cv::Point(box.x, box.y),
                      cv::Point(box.x + box.width, box.y + box.height),
                      cv::Scalar(0, 255, 0), 2); // Green bounding box
        std::string label_text = "Label: " + std::to_string(box.label) +
                                   " Score: " + std::to_string(box.score).substr(0,4);
        cv::putText(visualized_image, label_text, cv::Point(box.x, box.y - 10),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
    }

    // Determine output path
    std::filesystem::path original_path(raw_image_data.image_path);
    std::string output_filename = original_path.stem().string() + "_visualized" + original_path.extension().string();
    // For demo purposes, save in the same directory as the input.
    // In a real scenario, this might be a configurable output directory.
    std::string output_path_str = (original_path.parent_path() / output_filename).string();


    if (!cv::imwrite(output_path_str, visualized_image)) {
        throw MEXCEPTION("VisualizationNode: Failed to save visualized image to " + output_path_str);
    }

    auto visualized_data = std::make_shared<PortData>();
    visualized_data->set(VisualizedImageData{visualized_image, output_path_str});
    outputs["visualized_image_data"] = visualized_data;
}

std::vector<std::string> VisualizationNode::getExpectedInputPorts() const {
    return {"raw_image_data", "inference_result"};
}

std::vector<std::string> VisualizationNode::getExpectedOutputPorts() const {
    return {"visualized_image_data"};
}

} // namespace ai_pipe
