#include <memory>


class IVulkanApplication {
public:
//This is a convention that makes sure this function is abstract
	// this patter is called pure void
  virtual ~IVulkanApplication () = 0;
  virtual void run() = 0;
};

std::unique_ptr<IVulkanApplication> newVulkanApplication(const int p_width, const int p_height);
