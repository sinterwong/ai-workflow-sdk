#ifndef __INFERENCE_NODE_HPP__
#define __INFERENCE_NODE_HPP__

#include "ai_pipe/node_base.hpp"
#include "ai_pipe/pipe_types.hpp" // For RawImageData, InferenceResult

namespace ai_pipe {

class InferenceNode : public NodeBase {
public:
    InferenceNode(const std::string& name);
    ~InferenceNode() override = default;

    void process(const PortDataMap& inputs, PortDataMap& outputs,
                 std::shared_ptr<PipelineContext> context = nullptr) override;

    std::vector<std::string> getExpectedInputPorts() const override;
    std::vector<std::string> getExpectedOutputPorts() const override;
};

} // namespace ai_pipe

#endif // __INFERENCE_NODE_HPP__
