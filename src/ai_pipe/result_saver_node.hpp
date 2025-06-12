#ifndef __RESULT_SAVER_NODE_HPP__
#define __RESULT_SAVER_NODE_HPP__

#include "ai_pipe/node_base.hpp"
#include "ai_pipe/pipe_types.hpp" // For InferenceResult

namespace ai_pipe {

class ResultSaverNode : public NodeBase {
public:
    ResultSaverNode(const std::string& name);
    ~ResultSaverNode() override = default;

    void process(const PortDataMap& inputs, PortDataMap& outputs,
                 std::shared_ptr<PipelineContext> context = nullptr) override;

    std::vector<std::string> getExpectedInputPorts() const override;
    // This node does not produce output for other nodes, so output ports can be empty
    std::vector<std::string> getExpectedOutputPorts() const override { return {}; }
};

} // namespace ai_pipe

#endif // __RESULT_SAVER_NODE_HPP__
