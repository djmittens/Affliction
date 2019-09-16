#pragma once

#include <memory>

namespace vke::platform {
class IApplication {
public:
  // This is a convention that makes sure this function is abstract
  // this patter is called pure void
  virtual ~IApplication() = 0;
  virtual void run() = 0;
};

std::unique_ptr<IApplication> createApplication(const int p_width,
                                                const int p_height);

} // namespace vke::platform