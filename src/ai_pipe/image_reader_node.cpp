#include "image_reader_node.hpp"
#include "utils/mexception.hpp" // For MEXCEPTION
#include <opencv2/imgcodecs.hpp> // For cv::imread

namespace ai_pipe {

ImageReaderNode::ImageReaderNode(const std::string& name) : NodeBase(name) {}

void ImageReaderNode::process(const PortDataMap& inputs, PortDataMap& outputs,
                             std::shared_ptr<PipelineContext> context) {
    if (inputs.find("image_path") == inputs.end()) {
        throw MEXCEPTION("ImageReaderNode: Missing 'image_path' input.");
    }

    const auto& input_data_packet = inputs.at("image_path");
    if (!input_data_packet->has<std::string>()) {
        throw MEXCEPTION("ImageReaderNode: 'image_path' input is not a string.");
    }

    const std::string image_path = input_data_packet->get<std::string>();
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);

    if (image.empty()) {
        throw MEXCEPTION("ImageReaderNode: Failed to read image from path: " + image_path);
    }

    auto raw_image_data = std::make_shared<PortData>();
    raw_image_data->set(RawImageData{image, image_path});
    outputs["raw_image_data"] = raw_image_data;
}

std::vector<std::string> ImageReaderNode::getExpectedInputPorts() const {
    return {"image_path"};
}

std::vector<std::string> ImageReaderNode::getExpectedOutputPorts() const {
    return {"raw_image_data"};
}

} // namespace ai_pipe
