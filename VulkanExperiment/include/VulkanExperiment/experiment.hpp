#pragma once

#include "logging.hpp"
#include <memory>

namespace vke::platform {
/**
 * @brief This is main configuration for a running application, that will allow
 * you to configure different aspects of stuff.
 */
class IApplication {
public:
  // This is a convention that makes sure this function is abstract
  // this patter is called pure void
  virtual ~IApplication() = 0;
  virtual void run() = 0;
};

/**
 * @brief Create a Application object
 *
 * @param p_width
 * @param p_height
 * @return std::unique_ptr<IApplication>
 */
std::unique_ptr<IApplication> createApplication();

} // namespace vke::platform