#pragma once

// On windows by default, nothing is exported,
// thats why we need to add something like this.
#define VKE_EXPORT __declspec(dllexport)