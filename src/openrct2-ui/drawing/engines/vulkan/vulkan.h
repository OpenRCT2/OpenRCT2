#ifndef VULKAN_H_
#define VULKAN_H_ 1

/*
** Copyright 2015-2022 The Khronos Group Inc.
**
** SPDX-License-Identifier: Apache-2.0
*/

#include "vk_platform.h"
#include "vulkan_core.h"

#ifdef VK_USE_PLATFORM_ANDROID_KHR
#    include "vulkan_android.h"
#endif

#ifdef VK_USE_PLATFORM_FUCHSIA
#    include "vulkan_fuchsia.h"

#    include <zircon/types.h>
#endif

#ifdef VK_USE_PLATFORM_IOS_MVK
#    include "vulkan_ios.h"
#endif

#ifdef VK_USE_PLATFORM_MACOS_MVK
#    include "vulkan_macos.h"
#endif

#ifdef VK_USE_PLATFORM_METAL_EXT
#    include "vulkan_metal.h"
#endif

#ifdef VK_USE_PLATFORM_VI_NN
#    include "vulkan_vi.h"
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
#    include "vulkan_wayland.h"

#    include <wayland-client.h>
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
#    include "vulkan_win32.h"

#    include <windows.h>
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
#    include "vulkan_xcb.h"

#    include <xcb/xcb.h>
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
#    include "vulkan_xlib.h"

#    include <X11/Xlib.h>
#endif

#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
#    include "vulkan_directfb.h"

#    include <directfb.h>
#endif

#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
#    include "vulkan_xlib_xrandr.h"

#    include <X11/Xlib.h>
#    include <X11/extensions/Xrandr.h>
#endif

#ifdef VK_USE_PLATFORM_GGP
#    include "vulkan_ggp.h"

#    include <ggp_c/vulkan_types.h>
#endif

#ifdef VK_USE_PLATFORM_SCREEN_QNX
#    include "vulkan_screen.h"

#    include <screen/screen.h>
#endif

#ifdef VK_ENABLE_BETA_EXTENSIONS
#    include "vulkan_beta.h"
#endif

#endif // VULKAN_H_
