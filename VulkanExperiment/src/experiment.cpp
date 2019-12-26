#include <VulkanExperiment/experiment.hpp>
#include <VulkanExperiment/logging.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <algorithm>
#include <array>
// #include <boost/bind.hpp>
#include <cstdint>
#include <cstdlib>
#include <fmt/format.h>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>


// Files and shit
#include <fstream>

// #include <boost/log/trivial.hpp>

/*
  Some serious TODO list for me to tackle at some point on this project.

  * Add Doxygen documentation, to the Vk Layer (maybe even create the layer)
  * Move the build to CMakeLists
  * Upload to GitHub.
  * Stop using Exceptions darn it ! Use actual response types.
  * Create a simple module for logging, maybe experiment with macros for that ?

 */
// In pixels or something
const int WIDTH = 800;
const int HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;

#define UNUSED(x) (void)x;

// because this function is an extension function, it is not automatically
// loaded. We have to look up its address ourselves using vkGetInstanceProcAddr
VkResult CreateDebugUtilsMessangerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto funk = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (funk != nullptr) {
    return funk(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

// This will cleanup the left behind garbage for the callback.
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
  auto funk = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (funk != nullptr) {
    funk(instance, debugMessenger, pAllocator);
  }
}

namespace vke::platform {
IApplication::~IApplication() {}

class HelloTriangleApplication : public IApplication {
public:
  HelloTriangleApplication() {
    // TODO i need to get rid of this thing.
    // UNUSED(p_logger);
    //auto logger = vke::log::LazyLogger(vke::log::fileLogger("logs/vulkan.log"));
    // m_logger = std::make_unique<vke::log::LazyLogger>(std::move(logger));
    m_logger =  std::unique_ptr<vke::log::LazyLogger>(new vke::log::LazyLogger(vke::log::fileLogger("logs/vulkan.log")));
  }
  void run(std::vector<std::string> args) final override  {
    UNUSED(args);
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  // Useful junk for logging to somewhere
  // Window junk i dunno
  GLFWwindow *window = nullptr;

  // Vk stuff specific to the window
  VkInstance instance = VK_NULL_HANDLE;
  VkDevice device = VK_NULL_HANDLE;
  VkQueue graphicsQueue = VK_NULL_HANDLE;
  VkQueue presentationQueue = VK_NULL_HANDLE;
  VkSurfaceKHR surface = VK_NULL_HANDLE;

  VkCommandPool commandPool = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> commandBuffers;

  VkSwapchainKHR swapChain = VK_NULL_HANDLE;
  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;
  VkFormat swapChainImageFormat = {};
  VkExtent2D swapChainExtent = {};
  VkRenderPass renderPass = VK_NULL_HANDLE;

  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkPipeline graphicsPipeline;

  VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

  size_t currentFrame = 0;

  const std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  const std::vector<const char *> validationLayers = {
      // "VK_LAYER_KHRONOS_validation"
      // aparantly this is a new type of validation layer that comes with the lunarg implementation? but why is the other one not working?
      /// thats also hilarious
      "VK_LAYER_LUNARG_standard_validation"
      };

  /**
   * my spiffy little logging implementation.
   */
  std::unique_ptr<vke::log::LazyLogger> m_logger;

#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
#endif

  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }

  void initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFrameBuffers();
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();

    {
      uint32_t extensionCount = 0;
      vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

      std::vector<VkExtensionProperties> extensions(extensionCount);

      vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                             extensions.data());

      m_logger->debug("available extensions:");

      for (const auto &extension : extensions) {
        m_logger->debug(vke::log::TAB + std::string(extension.extensionName));
      }
    }
  }

  void createInstance() {

    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error(
          "validation layers requested, but not available ");
    }

    // Create App

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    // Create instance

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Create debug messenger

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
      populateDebugMessengerCreateInfo(debugCreateInfo);
      createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
      createInfo.enabledLayerCount = 0;
      createInfo.pNext = nullptr;
    }

    if (VK_SUCCESS != vkCreateInstance(&createInfo, nullptr, &instance)) {

      throw std::runtime_error("failed to create a vulkan instance!");
    } else {
      m_logger->debug("Successfully created a Vulkan instance !!!!");
    }
  }

  // Extensions and Layers
  void setupDebugMessenger() {
    if (!enableValidationLayers)
      return;

    m_logger->debug("setting up the debug messenger.");

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    populateDebugMessengerCreateInfo(createInfo);
    if (VK_SUCCESS != CreateDebugUtilsMessangerEXT(instance, &createInfo,
                                                   nullptr, &debugMessenger)) {
      throw std::runtime_error("failed to set up debug messenger !");
    }
  }

  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        // Leaving this one out because its the verbose debugging information,
        // but would be fun to turn it on some day.
        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = debugCallback;
    // boost::bind(debugCallback, m_logger, _1, _2, _3, _4);
    createInfo.pUserData = nullptr;
  }

  bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers) {
      bool layerFound = false;
      for (const auto &layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }
      if (!layerFound) {
        return false;
      }
    }

    return true;
  }

  /// This function will get the extensions we specify and return them for the
  /// instance.
  std::vector<const char *> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
  }

  // Surfaces (eg. stuff you draw to render to the screen)
  void createSurface() {
    if (VK_SUCCESS !=
        glfwCreateWindowSurface(instance, window, nullptr, &surface)) {
      throw std::runtime_error("failed to create window surface!~");
    }
    /// This is the manual way of doing the same thing but only for windows.
    // VkWin32SurfaceCreateInfoKHR createInfo = {};
    // createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    // createInfo.hwnd = glfwGetWin32Window(window);
    // createInfo.hinstance = GetModuleHandle(nullptr);
    // if (VK_SUCCESS != vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr,
    // &surface)) { 	throw std::runtime_error("Failed to create the window
    // surface!");
    //}
  }

  // Physical devices

  /// In order for us to do anything we need to access a physical device.
  /// This function will pick the best device available based on a ranking
  /// system that operates over some desired features useful for drawing, such
  /// as support for geometry shaders, queue families, and other junk.
  void pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount < 1) {
      throw std::runtime_error("Failed to find GPU's with Vulkan support!");
    } else {
      std::vector<VkPhysicalDevice> devices(deviceCount);
      vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

      std::multimap<int, VkPhysicalDevice> candidates;
      rateDevices(devices, candidates);

      auto [rating, d] = *candidates.rbegin();

      if (rating > 0) {
        m_logger->debug(fmt::format(
            "Found a suitable physical device i can  use! with a score of: {}",
            rating));

        physicalDevice = d;
      } else {
        throw std::runtime_error("failed to find a suitable GPU!");
      }
    }

    // In case we failed to initialize the physical device, panic!
    if (physicalDevice == VK_NULL_HANDLE) {
      throw std::runtime_error("failed to find a suitable GPU!");
    }
  }

  void rateDevices(std::vector<VkPhysicalDevice> devices,
                   std::multimap<int, VkPhysicalDevice> &output) {
    for (const auto &d : devices) {
      int score = getDeviceRating(d);
      output.insert(std::make_pair(score, d));
    }
  }

  int getDeviceRating(VkPhysicalDevice p_device) {
    int score = 0;

    // Device should be a discrete graphics card, those are usually the most
    // potent.
    {
      VkPhysicalDeviceProperties deviceProperties;
      vkGetPhysicalDeviceProperties(p_device, &deviceProperties);
      if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
      }
    }

    // The device must support geometry shading, otherwise no point in
    // rendering.
    {
      VkPhysicalDeviceFeatures deviceFeatures;
      vkGetPhysicalDeviceFeatures(p_device, &deviceFeatures);

      // This is a deal breaker. 😭
      // Also do we actually use geometry shaders?
      // if (!deviceFeatures.geometryShader) {
      //   std::cout << "geometryShader feature was not found on a device" <<
      //   ENDL; return 0;
      // }
    }

    // Very important to have complete queue family set.
    {
      QueueFamilyIndicies indices = findQueueFamilies(p_device);
      if (indices.isComplete()) {
        score += 500;
      }
    }

    // Check required extensions on the device, if there was some that were not
    // found, we will fail.
    {
      uint32_t extensionCount;
      vkEnumerateDeviceExtensionProperties(p_device, nullptr, &extensionCount,
                                           nullptr);

      std::vector<VkExtensionProperties> availableExtensions(extensionCount);
      vkEnumerateDeviceExtensionProperties(p_device, nullptr, &extensionCount,
                                           availableExtensions.data());

      std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                               deviceExtensions.end());

      for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
      }

      if (!requiredExtensions.empty()) {
        m_logger->error("required extensions were not found on a device");
        return 0;
      }
    }

    // Querying for swapchain support
    // A swap chain is sufficient if there is at least one presentation
    {
      SwapChainSupportDetails swapChainSupport =
          querySwapChainSupport(p_device);
      if (swapChainSupport.formats.empty() &&
          swapChainSupport.presentModes.empty()) {
        return 0;
      }
    }

    return score;
  }

  // SwapChain Fun

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities = {};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice p_device) {

    SwapChainSupportDetails details;

    // Basic Surface capabilities
    {
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_device, surface,
                                                &details.capabilities);
    }

    // Supported surface formats
    {
      uint32_t formatCount;
      vkGetPhysicalDeviceSurfaceFormatsKHR(p_device, surface, &formatCount,
                                           nullptr);
      if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(p_device, surface, &formatCount,
                                             details.formats.data());
      }
    }

    // Querying supported presentation modes
    {
      uint32_t modeCount;
      vkGetPhysicalDeviceSurfacePresentModesKHR(p_device, surface, &modeCount,
                                                nullptr);
      if (modeCount != 0) {
        details.presentModes.resize(modeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(p_device, surface, &modeCount,
                                                  details.presentModes.data());
      }
    }

    return details;
  }

  // Queue family fun

  struct QueueFamilyIndicies {
    // Device support for draw commands.
    std::optional<uint32_t> graphicsFamily = {};
    // Device support for presentation commands
    std::optional<uint32_t> presentFamily = {};

    bool isComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

  /// This will find different types of queue families supported by the device.
  /// Queue families can be used to schedule command buffers, for the hardware
  /// drive to perform the sort of real operations that usually perform.
  QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice p_device) {
    QueueFamilyIndicies indicies = {};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(p_device, &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(p_device, &queueFamilyCount,
                                             queueFamilies.data());

    int i = 0;

    for (const auto &queueFamily : queueFamilies) {
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(p_device, i, surface,
                                           &presentSupport);

      if (queueFamily.queueCount > 0 &&
          queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indicies.graphicsFamily = i;
      }

      if (queueFamily.queueCount > 0 && presentSupport) {
        indicies.presentFamily = i;
      }

      i++;
    }

    return indicies;
  }

  // Logical Devices go here, section
  /// A Logical Device is basically an interface (or a handle) to interacting a
  /// physical device. This is the thing you actually draw to, and therefore it
  /// requires a bit more configuration. On the other hand it gets us close !
  void createLogicalDevice() {
    // TODO: this seems unnecessary, figure out why we must query for families
    // multiple times.

    QueueFamilyIndicies indices = findQueueFamilies(physicalDevice);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    {
      // TODO this doesn't seem very safe...
      // FIXME this function assumes that the graphics family will contain both.
      std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                                indices.presentFamily.value()};

      const float queuePriority = 1.0f;
      for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
      }
    }

    // TODO: come back here for more interesting features later on.
    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // TODO: we will need to figure out how to shave off some precision here to
    // avoid overflows.
    createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount =
        static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    /// This is an optional deprecated, backwards compatibility for older SDK's,
    /// that differentiate, between device and instance based layers
    if (enableValidationLayers) {
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }

    if (VK_SUCCESS !=
        vkCreateDevice(physicalDevice, &createInfo, nullptr, &device)) {
      throw std::runtime_error("failed to create logical device!");
    }

    // TODO: This seems unsafe because graphics family is an optional value,
    // clearly there has to be a better way
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0,
                     &presentationQueue);

    // std::cout << "created a logical device !" << ENDL;
    // BOOST_LOG_TRIVIAL(debug) << "created a logical device !";
    // vke::log::debug() << "created a logical device !";
    m_logger->debug("created a logical device !");
    // m_logger->info("created a logical device !");
  }

  void createSwapChain() {
    // Create the actual swap chain
    {
      SwapChainSupportDetails swapChainSupport =
          querySwapChainSupport(physicalDevice);
      VkSurfaceFormatKHR surfaceFormat =
          chooseSwapSurfaceFormat(swapChainSupport.formats);
      VkPresentModeKHR presentMode =
          chooseSwapPresentMode(swapChainSupport.presentModes);
      VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

      // One more than the minimum, just to make sure we get a dedicated one.
      uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

      if (swapChainSupport.capabilities.maxImageCount > 0 &&
          imageCount > swapChainSupport.capabilities.maxImageCount) {

        imageCount = swapChainSupport.capabilities.maxImageCount;
      }
      VkSwapchainCreateInfoKHR createInfo = {};
      createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      createInfo.surface = surface;
      // details of swap chain images
      createInfo.minImageCount = imageCount;
      createInfo.imageFormat = surfaceFormat.format;
      createInfo.imageColorSpace = surfaceFormat.colorSpace;
      createInfo.imageExtent = extent;
      createInfo.imageArrayLayers = 1;
      createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

      QueueFamilyIndicies indices = findQueueFamilies(physicalDevice);
      uint32_t queueFamilyIndicies[] = {indices.graphicsFamily.value(),
                                        indices.presentFamily.value()};

      if (indices.graphicsFamily != indices.presentFamily) {
        // std::cout << "creating the slow VK_SHARING_MODE_CONCURRENT mode "
        //           << ENDL;
        // BOOST_LOG_TRIVIAL(debug)
        //     << "creating the slow VK_SHARING_MODE_CONCURRENT mode ";
        // vke::log::debug()
        //     << "creating the slow VK_SHARING_MODE_CONCURRENT mode ";
        m_logger->debug("creating the slow VK_SHARING_MODE_CONCURRENT mode ");
        // m_logger->info(
        //     std::string("creating the slow VK_SHARING_MODE_CONCURRENT
        //     mode"));
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndicies;
      } else {
        // std::cout << "creating the fast VK_SHARING_MODE_EXCLUSIVE mode "
        //           << ENDL;
        // BOOST_LOG_TRIVIAL(debug)
        //     << "creating the fast VK_SHARING_MODE_EXCLUSIVE mode";
        // vke::log::debug() << "creating the fast VK_SHARING_MODE_EXCLUSIVE
        // mode";
        m_logger->debug("creating the fast VK_SHARING_MODE_EXCLUSIVE mode");
        // m_logger->info("creating the fast VK_SHARING_MODE_EXCLUSIVE mode ");

        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
      }

      createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
      createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
      createInfo.presentMode = presentMode;
      createInfo.clipped = VK_TRUE;
      createInfo.oldSwapchain = VK_NULL_HANDLE;

      if (VK_SUCCESS !=
          vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain)) {
        throw std::runtime_error("failed to create swap chain!");
      }

      swapChainImageFormat = surfaceFormat.format;
      swapChainExtent = extent;
    }

    // Get the images.
    {
      uint32_t imageCount = 0;
      vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
      swapChainImages.resize(imageCount);
      vkGetSwapchainImagesKHR(device, swapChain, &imageCount,
                              swapChainImages.data());
    }
  }

  /// Surface format is basically color depth.
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
      if (availableFormat.format == VK_FORMAT_B8G8R8_UNORM &&
          availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return availableFormat;
      }
    }
    return availableFormats[0];
  }

  /// Presentation mode, is the most important setting.  It represents the
  /// actual conditions for showing  images to the screen. There are 4 modes
  /// that can be supported
  ///
  // VK_PRESENT_MODE_IMMEDIATE_KHR: Images submitted by your application are
  // transferred to the screen right away, which may result in tearing.
  // VK_PRESENT_MODE_FIFO_KHR : The swap chain is a queue where the display
  // takes an image from the front of the queue when the display is refreshed
  // and the program inserts rendered images at the back of the queue.If the
  // queue is full then the program has to wait.This is most similar to vertical
  // sync as found in modern games.The moment that the display is refreshed is
  // known as "vertical blank". VK_PRESENT_MODE_FIFO_RELAXED_KHR : This mode
  // only differs from the previous one if the application is late and the queue
  // was empty at the last vertical blank.Instead of waiting for the next
  // vertical blank, the image is transferred right away when it finally
  // arrives.This may result in visible tearing. VK_PRESENT_MODE_MAILBOX_KHR :
  // This is another variation of the second mode.Instead of blocking the
  // application when the queue is full, the images that are already queued are
  // simply replaced with the newer ones.This mode can be used to implement
  // triple buffering, which allows you to avoid tearing with significantly less
  // latency issues than standard vertical sync that uses double buffering.
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        return availablePresentMode;
      }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  /// This is the resolution of a swap buffer.
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
      return capabilities.currentExtent;
    } else {
      VkExtent2D actualExtent = {WIDTH, HEIGHT};
      actualExtent.width = std::max(
          capabilities.minImageExtent.width,
          std::min(capabilities.maxImageExtent.width, actualExtent.width));
      return actualExtent;
    }
  }

  void createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) {
      VkImageViewCreateInfo createInfo = {};
      createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      createInfo.image = swapChainImages[i];
      // Specifies how the image data will be interpreted.
      createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

      createInfo.format = swapChainImageFormat;
      createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

      createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      createInfo.subresourceRange.baseMipLevel = 0;
      createInfo.subresourceRange.levelCount = 1;
      createInfo.subresourceRange.baseArrayLayer = 0;
      createInfo.subresourceRange.layerCount = 1;

      if (VK_SUCCESS != vkCreateImageView(device, &createInfo, nullptr,
                                          &swapChainImageViews[i])) {
        throw std::runtime_error("failed to create image views!");
      }
    }
  }

  // Rendering n STUFF
  void createRenderPass() {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;

    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                               VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (VK_SUCCESS !=
        vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass)) {
      throw std::runtime_error("failed to create render pass!");
    }
  }

  // Graphics pipelines and shit
  void createGraphicsPipeline() {
    auto vertShaderModule =
        createShaderModule(readFile("shaders/helloworld.vert.spv"));
    auto fragShaderModule =
        createShaderModule(readFile("shaders/helloworld.frag.spv"));

    // Pipeline layout
    {

      VkPipelineVertexInputStateCreateInfo vertInputInfo = {};
      vertInputInfo.sType =
          VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      vertInputInfo.vertexBindingDescriptionCount = 0;
      vertInputInfo.pVertexBindingDescriptions = nullptr;
      vertInputInfo.vertexAttributeDescriptionCount = 0;
      vertInputInfo.pVertexAttributeDescriptions = nullptr;

      VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
      inputAssembly.sType =
          VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      inputAssembly.primitiveRestartEnable = VK_FALSE;

      // viewport doesnt have to follow swapchain dimensions, but we are going
      // to do anyways.
      VkViewport viewport = {};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = (float)swapChainExtent.width;
      viewport.height = (float)swapChainExtent.height;
      viewport.minDepth = 1.0f;
      viewport.maxDepth = 1.0f;

      // Draw the entire rectangle.
      VkRect2D scissor = {};
      scissor.offset = {0, 0};
      scissor.extent = swapChainExtent;

      // Some graphics cards can create more than one, but a certain extensions
      // will need to be enabled. For our use case we only create a single
      // viewport. basically we just combine the two, into one state.
      VkPipelineViewportStateCreateInfo viewportState = {};
      viewportState.sType =
          VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewportState.viewportCount = 1;
      viewportState.pViewports = &viewport;
      viewportState.scissorCount = 1;
      viewportState.pScissors = &scissor;

      // Rasterizer

      VkPipelineRasterizationStateCreateInfo rasterizer = {};
      rasterizer.sType =
          VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      rasterizer.depthClampEnable = VK_FALSE;

      // If this were enabled its data would never pass through the rasterizer
      // stage to the viewport.
      rasterizer.rasterizerDiscardEnable = VK_FALSE;
      rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
      rasterizer.lineWidth = 1.0f;
      rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
      rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

      /// instead of clipping, vertices's that fall outside of the clipping
      /// space, are instead clamped to it. apparently useful for shadows, but
      /// we need an extension loaded.
      rasterizer.depthBiasEnable = VK_FALSE;
      rasterizer.depthBiasConstantFactor = 0.0f;
      rasterizer.depthBiasClamp = 0.0f;
      rasterizer.depthBiasSlopeFactor = 0.0f;

      // Multi sampling
      // Basically this allows you to do antialiasing, but we disable it for
      // now, to revisit later.
      VkPipelineMultisampleStateCreateInfo multisampling = {};
      multisampling.sType =
          VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      multisampling.sampleShadingEnable = VK_FALSE;
      multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
      multisampling.minSampleShading = 1.0f;
      multisampling.pSampleMask = nullptr;
      multisampling.alphaToCoverageEnable = VK_FALSE;
      multisampling.alphaToOneEnable = VK_FALSE;

      // Depth and stencils
      // we don't have any yet but we could use
      // `VkPipelineDepthStencilStateCreateInfo` to create some

      // Color Blending
      // there are two ways to configure, this, one is globally the other is per
      // framebuffer. in our case we only have one framebuffer, so we did it
      // that way.
      VkPipelineColorBlendAttachmentState colorBlend = {};
      colorBlend.colorWriteMask =
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
      colorBlend.blendEnable = VK_FALSE;
      // colorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
      // colorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
      // colorBlend.colorBlendOp = VK_BLEND_OP_ADD;
      // colorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      // colorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
      // colorBlend.alphaBlendOp = VK_BLEND_OP_ADD;

      colorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
      colorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      colorBlend.colorBlendOp = VK_BLEND_OP_ADD;
      colorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      colorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
      colorBlend.alphaBlendOp = VK_BLEND_OP_ADD;

      VkPipelineColorBlendStateCreateInfo blendState = {};
      blendState.sType =
          VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      blendState.logicOpEnable = VK_FALSE;
      blendState.logicOp = VK_LOGIC_OP_COPY;
      blendState.attachmentCount = 1;
      blendState.pAttachments = &colorBlend;
      blendState.blendConstants[0] = 0.0f;
      blendState.blendConstants[1] = 0.0f;
      blendState.blendConstants[2] = 0.0f;
      blendState.blendConstants[3] = 0.0f;

      // Dynamic state
      VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                        VK_DYNAMIC_STATE_LINE_WIDTH};

      VkPipelineDynamicStateCreateInfo dynamicState = {};
      dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamicState.dynamicStateCount = 2;
      dynamicState.pDynamicStates = dynamicStates;

      // Finally the final pipeline layout

      VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
      pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutInfo.setLayoutCount = 0;
      pipelineLayoutInfo.pSetLayouts = nullptr;
      pipelineLayoutInfo.pushConstantRangeCount = 0;
      pipelineLayoutInfo.pPushConstantRanges = nullptr;

      if (VK_SUCCESS != vkCreatePipelineLayout(device, &pipelineLayoutInfo,
                                               nullptr, &pipelineLayout)) {
        throw std::runtime_error("failed to create pipeline layout!");
      }

      // Graphics Pipeline
      // shaders n stuff
      // Also whats up with that craaaaazy lambda? its pretty friggin cool.
      // std::vector<VkPipelineShaderStageCreateInfo> shaderStages = [&]() ->
      // auto { 	VkPipelineShaderStageCreateInfo vertShaderStageInfo =
      // {};
      //	vertShaderStageInfo.sType =
      // VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      //	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
      //	vertShaderStageInfo.module = vertShaderModule;
      //	vertShaderStageInfo.pName = "main";

      //	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
      //	fragShaderStageInfo.sType =
      // VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      //	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      //	fragShaderStageInfo.module = fragShaderModule;
      //	fragShaderStageInfo.pName = "main";
      //	return std::vector{ vertShaderStageInfo, fragShaderStageInfo };
      //}();
      VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
      vertShaderStageInfo.sType =
          VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
      vertShaderStageInfo.module = vertShaderModule;
      vertShaderStageInfo.pName = "main";

      VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
      fragShaderStageInfo.sType =
          VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      fragShaderStageInfo.module = fragShaderModule;
      fragShaderStageInfo.pName = "main";

      std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{
          vertShaderStageInfo, fragShaderStageInfo};

      VkGraphicsPipelineCreateInfo pipelineInfo = {};
      pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipelineInfo.stageCount = 2;
      pipelineInfo.pStages = shaderStages.data();

      // Woof we are combining everything together finally.
      pipelineInfo.pVertexInputState = &vertInputInfo;
      pipelineInfo.pInputAssemblyState = &inputAssembly;
      pipelineInfo.pViewportState = &viewportState;
      pipelineInfo.pRasterizationState = &rasterizer;
      pipelineInfo.pMultisampleState = &multisampling;
      pipelineInfo.pDepthStencilState = nullptr;
      pipelineInfo.pColorBlendState = &blendState;
      pipelineInfo.pDynamicState = nullptr;

      pipelineInfo.layout = pipelineLayout;

      pipelineInfo.renderPass = renderPass;
      pipelineInfo.subpass = 0;

      pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
      pipelineInfo.basePipelineIndex = -1;

      if (VK_SUCCESS != vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
                                                  &pipelineInfo, nullptr,
                                                  &graphicsPipeline)) {
        throw std::runtime_error("failed to create graphics pipeline!");
      }
    }

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
  }

  /// FrameBuffer fun
  void createFrameBuffers() {
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
      VkImageView attachments[] = {swapChainImageViews[i]};

      VkFramebufferCreateInfo framebufferInfo = {};
      framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferInfo.renderPass = renderPass;
      framebufferInfo.attachmentCount = 1;
      framebufferInfo.pAttachments = attachments;
      framebufferInfo.width = swapChainExtent.width;
      framebufferInfo.height = swapChainExtent.height;
      framebufferInfo.layers = 1;

      if (VK_SUCCESS != vkCreateFramebuffer(device, &framebufferInfo, nullptr,
                                            &swapChainFramebuffers[i])) {
        throw std::runtime_error("failed to create a framebuffer!");
      }
    }
  }

  void createCommandPool() {
    QueueFamilyIndicies queueFamilyIndices = findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = 0;

    if (VK_SUCCESS !=
        vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool)) {
      throw std::runtime_error("failed to create command pool!");
    }
  }

  void createCommandBuffers() {
    commandBuffers.resize(swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (VK_SUCCESS !=
        vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data())) {
      throw std::runtime_error("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < commandBuffers.size(); i++) {
      VkCommandBufferBeginInfo beginInfo = {};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      beginInfo.flags = 0;
      beginInfo.pInheritanceInfo = nullptr;

      if (VK_SUCCESS != vkBeginCommandBuffer(commandBuffers[i], &beginInfo)) {
        throw std::runtime_error("failed to begin recording command buffer!");
      }

      VkRenderPassBeginInfo renderPassInfo = {};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassInfo.renderPass = renderPass;
      renderPassInfo.framebuffer = swapChainFramebuffers[i];

      renderPassInfo.renderArea.offset = {0, 0};
      renderPassInfo.renderArea.extent = swapChainExtent;

      VkClearValue clearColor = {{{0.5f, 0.5f, 0.5f, 1.0f}}};
      renderPassInfo.clearValueCount = 1;
      renderPassInfo.pClearValues = &clearColor;

      /// Actual drawing going on up in here.
      vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                           VK_SUBPASS_CONTENTS_INLINE);
      vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                        graphicsPipeline);

      vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

      vkCmdEndRenderPass(commandBuffers[i]);

      if (VK_SUCCESS != vkEndCommandBuffer(commandBuffers[i])) {
        throw std::runtime_error("failed to record command buffer!");
      }
    }
  }

  void createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      if (VK_SUCCESS != vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                                          &imageAvailableSemaphores[i]) ||
          VK_SUCCESS != vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                                          &renderFinishedSemaphores[i]) ||
          VK_SUCCESS !=
              vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i])) {
        throw std::runtime_error("failed to create image available semaphore");
      }
    }
  }

  VkShaderModule createShaderModule(const std::vector<char> &code) {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    // This reinterpret cast is funny, because it turned a char array into a
    // pointer, to raw bytes.
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
    VkShaderModule shaderModule;
    if (VK_SUCCESS !=
        vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule)) {
      throw std::runtime_error("failed to create shader module!");
    }
    return shaderModule;
  }

  // This gets called on every debug message
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {
    UNUSED(messageType);
    UNUSED(pUserData);
    // TODO: figure how to bind a logger to this callback :()
    // vke::log::debug() << "validation layer::" << pCallbackData->pMessage;

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
      // An important message that we would show.
    }
    return VK_FALSE;
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
      drawFrame();
      // vkQueueWaitIdle(presentationQueue);
    }
    vkDeviceWaitIdle(device);
  }

  void drawFrame() {
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE,
                    UINT64_MAX);
    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    uint32_t imageIndex = 0;
    vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
                          imageAvailableSemaphores[currentFrame],
                          VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr;

    if (VK_SUCCESS != vkQueueSubmit(graphicsQueue, 1, &submitInfo,
                                    inFlightFences[currentFrame])) {
      throw std::runtime_error("failed to submit draw command buffer!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    vkQueuePresentKHR(presentationQueue, &presentInfo);
    vkQueueWaitIdle(presentationQueue);
  }

  void cleanup() {
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
      vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
      vkDestroyFence(device, inFlightFences[i], nullptr);
    }
    vkDestroyCommandPool(device, commandPool, nullptr);
    for (auto framebuffer : swapChainFramebuffers) {
      vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    for (auto imageView : swapChainImageViews) {
      vkDestroyImageView(device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(device, swapChain, nullptr);
    vkDestroyDevice(device, nullptr);
    if (enableValidationLayers) {
      DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  // Read the file in as bytes.
  static std::vector<char> readFile(const std::string &filename) {
    // ate means that we are reading from the end, which allows us to
    // preallocate the array, given that we know the offset.
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
      // TODO needs much more error handling.
      throw std::runtime_error("failed to open file !");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    // read the file back to the beginning.
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
  }
};

std::unique_ptr<IApplication> createApplication() {
  // auto logger = std::shared_ptr<vke::platform::logging::NoOpLogger>(
  //     new vke::platform::logging::NoOpLogger());
  return std::unique_ptr<IApplication>(new HelloTriangleApplication());
}
} // namespace vke::platform