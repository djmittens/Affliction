#pragma once

#include "vke.hpp"
#include <memory>
#include "platform/application.h"
#include <vector>

namespace vke::platform {


/**
 * @brief Create a Application object
 *
 * @param p_width
 * @param p_height
 * @return std::unique_ptr<IApplication>
 */
std::unique_ptr<IApplication> createApplication();

} // namespace vke::platform