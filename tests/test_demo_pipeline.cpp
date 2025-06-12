#include "ai_pipe/pipeline.hpp"
#include "ai_pipe/pipeline_builder.hpp"
#include "ai_pipe/pipeline_context.hpp"
#include "ai_pipe/node_registrar.hpp" // To ensure nodes are registered
#include "ai_pipe/pipe_types.hpp"     // For PortData, RawImageData, VisualizedImageData
#include "gtest/gtest.h"
#include <filesystem> // For std::filesystem::exists, std::filesystem::remove
#include <fstream>    // For std::ofstream to create a dummy graph config

// Helper to create a dummy graph config file for the test
void create_dummy_graph_config(const std::string& config_path) {
    std::ofstream config_file(config_path);
    config_file << R"({
        "graph_name": "DemoPipelineTest",
        "nodes": [
            {
                "name": "ImageReader",
                "type": "ImageReaderNode",
                "params": {}
            },
            {
                "name": "Inference",
                "type": "InferenceNode",
                "params": {}
            },
            {
                "name": "ResultSaver",
                "type": "ResultSaverNode",
                "params": {}
            },
            {
                "name": "Visualization",
                "type": "VisualizationNode",
                "params": {}
            }
        ],
        "edges": [
            {
                "from_node": "ImageReader",
                "from_port": "raw_image_data",
                "to_node": "Inference",
                "to_port": "raw_image_data"
            },
            {
                "from_node": "ImageReader",
                "from_port": "raw_image_data",
                "to_node": "Visualization",
                "to_port": "raw_image_data"
            },
            {
                "from_node": "Inference",
                "from_port": "inference_result",
                "to_node": "ResultSaver",
                "to_port": "inference_result"
            },
            {
                "from_node": "Inference",
                "from_port": "inference_result",
                "to_node": "Visualization",
                "to_port": "inference_result"
            }
        ],
        "inputs": [
            {"node_name": "ImageReader", "port_name": "image_path"}
        ],
        "outputs": [
            {"node_name": "Visualization", "port_name": "visualized_image_data"}
        ]
    })";
    config_file.close();
}

TEST(DemoPipelineTest, RunPipeline) {
    // Ensure nodes are registered (accessing the registrar instance)
    [[maybe_unused]] auto& registrar = ai_pipe::NodeRegistrar::getInstance();

    const std::string test_image_path = "tests/data/yolov11/image.png";
    // For this test, we'll output the visualized image next to the executable.
    // A more robust test might use a temporary directory.
    std::filesystem::path input_fs_path(test_image_path);
    std::string expected_output_filename = input_fs_path.stem().string() + "_visualized" + input_fs_path.extension().string();
    std::string expected_output_path_str = (input_fs_path.parent_path() / expected_output_filename).string();


    // 0. Create a dummy graph config file
    const std::string graph_config_path = "test_demo_pipeline_config.json";
    create_dummy_graph_config(graph_config_path);

    // 1. Initialize Pipeline
    ai_pipe::Pipeline pipeline;
    auto context = std::make_shared<ai_pipe::PipelineContext>();
    // If an AlgoManager is needed by any node (not in this simple demo, but good practice):
    // auto algo_manager = std::make_shared<infer::dnn::AlgoManager>();
    // ASSERT_TRUE(algo_manager->initialize({})); // Initialize with default/empty config
    // context->setAlgoManager(algo_manager);

    ai_pipe::PipelineConfig pipeline_config;
    pipeline_config.graphConfigPath = graph_config_path; // Use the dummy config
    pipeline_config.numWorkers = 1; // Simple test, 1 worker is enough

    ASSERT_TRUE(pipeline.initialize(pipeline_config, context));
    ASSERT_EQ(pipeline.getState(), ai_pipe::PipelineState::IDLE);

    // 2. Prepare Input Data
    ai_pipe::PortDataMap inputs;
    auto image_path_data = std::make_shared<ai_pipe::PortData>();
    image_path_data->set(test_image_path);
    inputs["image_path"] = image_path_data; // Corresponds to "inputs" in graph_config.json

    // 3. Set up result and error callbacks (optional but good for debugging)
    bool result_received = false;
    bool error_occurred = false;
    std::string last_error_msg;

    pipeline.setPipelineResultCallback([&](const ai_pipe::PortDataMap& finalResults) {
        result_received = true;
        ASSERT_TRUE(finalResults.count("visualized_image_data"));
        ASSERT_TRUE(finalResults.at("visualized_image_data")->has<ai_pipe::VisualizedImageData>());
        const auto& viz_data = finalResults.at("visualized_image_data")->get<ai_pipe::VisualizedImageData>();
        ASSERT_FALSE(viz_data.image.empty());
        ASSERT_EQ(viz_data.output_path, expected_output_path_str);
    });

    pipeline.setPipelineErrorCallback([&](const std::string& errorMsg, const std::string& nodeName) {
        error_occurred = true;
        last_error_msg = "Pipeline error in node '" + nodeName + "': " + errorMsg;
        std::cerr << last_error_msg << std::endl;
    });

    // 4. Start and Feed Data
    ASSERT_TRUE(pipeline.start());
    ASSERT_EQ(pipeline.getState(), ai_pipe::PipelineState::RUNNING);

    // Use feedDataAndGetResultFuture for synchronous-like testing
    auto result_future = pipeline.feedDataAndGetResultFuture(inputs);

    // 5. Wait for completion (or timeout)
    std::future_status status = result_future.wait_for(std::chrono::seconds(10)); // 10-second timeout
    ASSERT_EQ(status, std::future_status::ready) << "Pipeline execution timed out.";

    // 6. Check result from future
    bool pipeline_success = result_future.get(); // Will rethrow exceptions from pipeline if any
    ASSERT_TRUE(pipeline_success);


    // 7. Verify Callbacks and Output
    ASSERT_TRUE(result_received) << "Pipeline result callback was not invoked.";
    ASSERT_FALSE(error_occurred) << "Pipeline error callback was invoked: " << last_error_msg;

    ASSERT_TRUE(std::filesystem::exists(expected_output_path_str))
        << "Expected output image file was not created: " << expected_output_path_str;

    // 8. Stop and Reset Pipeline
    ASSERT_TRUE(pipeline.stop());
    ASSERT_EQ(pipeline.getState(), ai_pipe::PipelineState::STOPPED);
    pipeline.reset();
    ASSERT_EQ(pipeline.getState(), ai_pipe::PipelineState::IDLE);

    // 9. Cleanup
    if (std::filesystem::exists(expected_output_path_str)) {
        std::filesystem::remove(expected_output_path_str);
    }
    if (std::filesystem::exists(graph_config_path)) {
        std::filesystem::remove(graph_config_path);
    }
}
