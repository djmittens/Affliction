#include <boost/log/trivial.hpp>
#include <VulkanExperiment/experiment.hpp>
#include <iostream>

int main() {
  // HelloTriangleApplication app;
  auto app = vke::platform::createApplication();
  try {
    BOOST_LOG_TRIVIAL(info) << "Starting the Werld app... Word...";
    app->run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}