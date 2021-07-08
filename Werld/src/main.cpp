#include <VulkanExperiment/experiment.hpp>
#include <VulkanExperiment/logging.hpp>
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char** argv) {
  // HelloTriangleApplication app;
  auto app = vke::platform::createApplication();
  auto logger = vke::log::LazyLogger(vke::log::crapLogger());
  try {
    
    // BOOST_LOG_TRIVIAL(info) << "Starting the Werld app... Word...";
    // vke::log::info() << "Starting the Werld app... "
    //                  << "Word "
    //                  << "...";
    logger.info("Starting the Werld app and you are all invited !");
    
    std::vector<std::string> args;

    /// Standard pointers can also act as iterators apparantly, which is somehow cool??? but its mostly magic.
    args.assign(argv, argv + argc);
    app->run(args);

  } catch (const std::exception &e) {
    // std::cerr << e.what() << std::endl;
    logger.error(e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}