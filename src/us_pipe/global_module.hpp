#ifndef US_PIPE_GLOBAL_MODULES_HPP
#define US_PIPE_GLOBAL_MODULES_HPP

#include "status_pipe.hpp"
#include "utils/thread_pool.hpp"
#include <memory>

namespace us_pipe {
struct GlobalModules {
  std::unique_ptr<StatusPipeline> videoStatusPipe;
  std::unique_ptr<utils::thread_pool> threadPool;

  void init(const std::string &modelRoot) {
    threadPool = std::make_unique<utils::thread_pool>();
    threadPool->start(1);
    videoStatusPipe = std::make_unique<StatusPipeline>(modelRoot, 0);
  }

  void release() {
    threadPool.reset();
    videoStatusPipe.reset();
  }
};
} // namespace us_pipe
#endif
