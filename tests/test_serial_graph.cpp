#include "ai_pipe/serial_graph.hpp"
#include "logger/logger.hpp"
#include <gtest/gtest.h>

namespace testing_serial_graph {
class DataSourceNode : public ai_pipe::NodeBase {
public:
  DataSourceNode(const std::string &name, int initialValue)
      : NodeBase(name), value_(initialValue) {}

  void process(const ai_pipe::PortDataMap &inputs,
               ai_pipe::PortDataMap &outputs) override {
    // mark it as not in use to avoid compiler BB
    (void)inputs;

    LOG_INFOS << " [" << getName()
              << "] DataSourceNode::process, Generated data's value: "
              << value_;
    outputs.params["output_data"] = value_++;
  }

private:
  int value_;
};

class AddNode : public ai_pipe::NodeBase {
public:
  AddNode(const std::string &name, int addValue)
      : NodeBase(name), addValue_(addValue) {}

  void process(const ai_pipe::PortDataMap &inputs,
               ai_pipe::PortDataMap &outputs) override {
    if (inputs.params.find("input_a") != inputs.params.end()) {
      try {
        int inputData = std::any_cast<int>(inputs.params.at("input_a"));
        int result = inputData + addValue_;
        LOG_INFOS << " [" << getName()
                  << "] AddNode::process, input: " << inputData
                  << ", addValue: " << addValue_ << ", result: " << result;
        outputs.params["result"] = result;
      } catch (const std::bad_any_cast &e) {
        LOG_ERRORS << " [" << getName()
                   << "] AddNode::process, bad_any_cast: " << e.what();
        throw;
      }
    } else {
      LOG_ERRORS << " [" << getName()
                 << "] AddNode::process, input_a not found";
      throw std::runtime_error("input_a not found");
    }
  }

private:
  int addValue_;
};

class IntToStringNode : public ai_pipe::NodeBase {
public:
  IntToStringNode(const std::string &name) : NodeBase(name) {}
  void process(const ai_pipe::PortDataMap &inputs,
               ai_pipe::PortDataMap &outputs) override {
    if (inputs.params.find("int_input") != inputs.params.end()) {
      try {
        int inputData = std::any_cast<int>(inputs.params.at("int_input"));
        std::string result = std::to_string(inputData);
        LOG_INFOS << " [" << getName()
                  << "] IntToStringNode::process, input: " << inputData
                  << ", result: " << result;
        outputs.params["string_output"] = result;
      } catch (const std::bad_any_cast &e) {
        LOG_ERRORS << " [" << getName()
                   << "] IntToStringNode::process, bad_any_cast: " << e.what();
        throw;
      }
    } else {
      LOG_ERRORS << " [" << getName()
                 << "] IntToStringNode::process, int_input not found";
      throw std::runtime_error("int_input not found");
    }
  }
};

class SinkNode : public ai_pipe::NodeBase {
public:
  SinkNode(const std::string &name) : NodeBase(name) {}
  void process(const ai_pipe::PortDataMap &inputs,
               ai_pipe::PortDataMap &outputs) override {
    // mark it as not in use to avoid compiler BB
    (void)outputs;
    LOG_INFOS << " [" << getName() << "] Receiving final data: ";
    for (const auto &pair : inputs.params) {
      std::cout << "    Input Port '" << pair.first << "': ";
      try {
        if (pair.second.type() == typeid(int)) {
          std::cout << "(int) " << std::any_cast<int>(pair.second);
        } else if (pair.second.type() == typeid(std::string)) {
          std::cout << "(string) \"" << std::any_cast<std::string>(pair.second)
                    << "\"";
        } else {
          std::cout << "(type: " << pair.second.type().name()
                    << ") - Cannot display value directly";
        }
      } catch (const std::bad_any_cast &e) {
        std::cout << "(Error retrieving value: " << e.what() << ")";
      }
      std::cout << std::endl;
    }
  }
};

class SerialGraphTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(SerialGraphTest, Normal) {
  ai_pipe::SerialGraph pipeline;

  auto source1 = std::make_shared<DataSourceNode>("Source1", 100);
  auto adder1 = std::make_shared<AddNode>("Adder1", 5);
  auto toString1 = std::make_shared<IntToStringNode>("ToString1");
  auto sink1 = std::make_shared<SinkNode>("Sink1");

  auto adder2 = std::make_shared<AddNode>("Adder2", -20);
  auto sink2 = std::make_shared<SinkNode>("Sink2");

  pipeline.addNode(source1);
  pipeline.addNode(adder1);
  pipeline.addNode(toString1);
  pipeline.addNode(sink1);

  pipeline.addNode(adder2);
  pipeline.addNode(sink2);

  // build pipeline
  try {
    // source1 -> adder1
    pipeline.addEdge(source1, "output_data", adder1, "input_a");
    // adder1 -> toString1
    pipeline.addEdge(adder1, "result", toString1, "int_input");
    // toString1 -> sink1
    pipeline.addEdge(toString1, "string_output", sink1, "final_string");

    // adder2 -> sink2
    pipeline.addEdge(source1, "output_data", adder2, "input_a");
    pipeline.addEdge(adder2, "result", sink2, "input_data");

    pipeline.addEdge(adder1, "result", sink1, "raw_int_result");
  } catch (const std::runtime_error &e) {
    LOG_ERRORS << "Error adding edge: " << e.what();
    throw;
  }

  try {
    pipeline.run();
  } catch (const std::runtime_error &e) {
    LOG_ERRORS << "Error running pipeline: " << e.what();
    throw;
  }
}
} // namespace testing_serial_graph