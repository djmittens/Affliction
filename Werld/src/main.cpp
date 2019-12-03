#include <VulkanExperiment/experiment.hpp>
#include <VulkanExperiment/logging.hpp>
#include <iostream>

int main() {
  // HelloTriangleApplication app;
  auto app = vke::platform::createApplication();
  try {
    // BOOST_LOG_TRIVIAL(info) << "Starting the Werld app... Word...";
    // vke::log::info() << "Starting the Werld app... "
    //                  << "Word "
    //                  << "...";
    auto logger = vke::log::LazyLogger(vke::log::crapLogger());
    logger.info("Starting the Werld app and you are all invited !");
    
    app->run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}