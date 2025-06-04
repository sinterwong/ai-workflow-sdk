/**
 * @file demo_nodes.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-06-02
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __DEMO_NODES_HPP__
#define __DEMO_NODES_HPP__

#include "logger/logger.hpp"
#include "node_base.hpp"

namespace ai_pipe {
class DemoSourceNode : public NodeBase {
public:
  DemoSourceNode(const std::string &name, int initialValue)
      : NodeBase(name), valueData_(initialValue) {}

  void process(const PortDataMap &inputs, PortDataMap &outputs) override {
    if (!inputs.empty()) {
      LOG_ERRORS << "Source node " << name_ << " received unexpected input.";
      return;
    }

    PortData outputPacket0;
    PortData outputPacket1;

    outputPacket0.id = 100000;
    outputPacket0.setParam<int>("original_data", valueData_);
    LOG_INFOS << "[" << getName()
              << "] Emitting data 0: value = " << valueData_;

    outputPacket1.id = 100001;
    outputPacket1.setParam<int>("processed_data", ++valueData_);
    LOG_INFOS << "[" << getName()
              << "] Emitting data 1: value = " << valueData_;

    outputs["demo_source_output_0"] = std::make_shared<PortData>(outputPacket0);
    outputs["demo_source_output_1"] = std::make_shared<PortData>(outputPacket1);
  }

  std::vector<std::string> getExpectedOutputPorts() const override {
    return {"demo_source_output_0", "demo_source_output_1"};
  }

  std::vector<std::string> getExpectedInputPorts() const override { return {}; }

private:
  int valueData_;
};

class DemoProcessingNode : public NodeBase {
public:
  DemoProcessingNode(const std::string &name, int addValue)
      : NodeBase(name), valueToAdd_(addValue) {}

  std::vector<std::string> getExpectedInputPorts() const override {
    return {"demo_process_input"};
  }

  std::vector<std::string> getExpectedOutputPorts() const override {
    return {"demo_process_output"};
  }

  void process(const PortDataMap &inputs, PortDataMap &outputs) override {
    auto it = inputs.find("demo_process_input");
    if (it == inputs.end()) {
      LOG_ERRORS << "Processing node " << name_ << " missing input data.";
      return;
    }

    const auto inputPacket = it->second;
    PortData outputPacket;

    outputPacket.id = 10101010;

    try {
      auto data = inputPacket->getParam<int>("original_data");

      int processedValue = data + valueToAdd_;

      outputPacket.setParam<int>("processed_data", processedValue);
      outputPacket.setParam<uint64_t>("original_id", inputPacket->id);

      LOG_INFOS << "[" << getName()
                << "] Emitting data: value = " << processedValue;

      outputs["demo_process_output"] = std::make_shared<PortData>(outputPacket);
    } catch (const std::runtime_error &e) {
      LOG_ERRORS << "[" << getName()
                 << "] Failed to get 'original_data' from input: " << e.what();
      // Depending on desired behavior, you might output an empty packet,
      // an error packet, or simply return without producing output.
      // For this demo, we'll just log and return.
      throw std::runtime_error(
          "DemoProcessingNode:" + getName() +
          " node failed due to missing or invalid input data.");
    }
  }

private:
  int valueToAdd_;
};

class DemoSinkNode : public NodeBase {
public:
  DemoSinkNode(const std::string &name,
               const std::vector<std::string> &inputPorts)
      : NodeBase(name), inputPorts_(inputPorts) {}

  std::vector<std::string> getExpectedInputPorts() const override {
    return inputPorts_;
  }

  std::vector<std::string> getExpectedOutputPorts() const override {
    return {};
  }

  void process(const PortDataMap &inputs, PortDataMap &outputs) override {
    if (!outputs.empty()) {
      LOG_ERRORS << "Sink node " << name_ << " produced unexpected output.";
      return;
    }
    LOG_INFOS << "[" << getName() << "] Received data on ports:";
    for (const auto &portName : inputPorts_) {
      auto it = inputs.find(portName);
      if (it != inputs.end()) {
        const auto inputPacket = it->second;
        LOG_INFOS << "  Port: " << portName
                  << ", Packet ID: " << inputPacket->id;
        // Example of accessing data, assuming 'processed_data' exists
        try {
          auto processedData =
              inputPacket->getOptionalParam<int>("processed_data");
          if (processedData.has_value()) {
            LOG_INFOS << "    Processed Data: " << processedData.value();
          } else {
            LOG_INFOS << "    No 'processed_data' found.";
          }
        } catch (const std::runtime_error &e) {
          LOG_WARNINGS << "    Failed to access data on port " << portName
                       << ": " << e.what();
        }
      } else {
        LOG_WARNINGS << "  Port: " << portName << " - No data received.";
      }
    }
  }

private:
  std::vector<std::string> inputPorts_;
};

} // namespace ai_pipe
#endif