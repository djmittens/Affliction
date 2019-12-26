#include <vector>
#include <string>

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
  virtual void run(std::vector<std::string> args) = 0;
};

/// I am going to attempt to implement a bare bones ECS interface here, but i am feeling rather uncomfortable with all the different aspects
/// of it at the moment

class ISystem {
    virtual ~ISystem() = 0;
};

}