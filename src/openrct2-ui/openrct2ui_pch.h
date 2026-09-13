#pragma once

// Precompiled header for libopenrct2ui project

// Standard library headers that are expensive to compile
#include <algorithm>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>

// Project core headers (from libopenrct2) that are used in UI
#include "../openrct2/Context.h"
#include "../openrct2/GameState.h"
#include "../openrct2/core/StringTypes.h"

// Interface headers that are widely used in UI
// Note: These must be in dependency order
#include "../openrct2/interface/Viewport.h"
#include "../openrct2/interface/Window.h"
#include "interface/Viewport.h"
#include "interface/Widget.h"
#include "interface/Window.h"
