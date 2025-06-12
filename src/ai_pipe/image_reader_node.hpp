#ifndef __IMAGE_READER_NODE_HPP__
#define __IMAGE_READER_NODE_HPP__

#include "ai_pipe/node_base.hpp"
#include "ai_pipe/pipe_types.hpp" // For RawImageData
#include <opencv2/opencv.hpp>     // For cv::Mat, cv::imread

namespace ai_pipe {

class ImageReaderNode : public NodeBase {
public:
    ImageReaderNode(const std::string& name);
    ~ImageReaderNode() override = default;

    void process(const PortDataMap& inputs, PortDataMap& outputs,
                 std::shared_ptr<PipelineContext> context = nullptr) override;

    std::vector<std::string> getExpectedInputPorts() const override;
    std::vector<std::string> getExpectedOutputPorts() const override;
};

} // namespace ai_pipe

#endif // __IMAGE_READER_NODE_HPP__
