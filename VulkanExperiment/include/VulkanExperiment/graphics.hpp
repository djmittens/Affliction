#pragma once

#include "logging.hpp"

namespace vke::platform {
/**
 * @brief This is a pretty awesome interface to allow you to interact with the
 * graphics hardware.  Pretty rudamentary stuff.
 *
 */
class IWindowGraphics {

public:
  /**
   * @brief prepare a surface for drawing.
   */
  virtual void initializeSurface() = 0;
};

IWindowGraphics createWindow();

struct SCreateWindow {
  int height = 0;
  int width = 0;

  SCreateWindow &withHeight(int p_height) {
    height = p_height;
    return *this;
  };
};

} // namespace vke::platform