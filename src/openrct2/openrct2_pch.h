#pragma once

// Include some expensive headers here to improve compilation speed

// Standard library headers that are frequently used and expensive to compile
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

// Core project headers with high compile cost and wide usage
#include "core/DataSerialiser.h"
#include "core/DataSerialiserTraits.h"
#include "core/FileSystem.hpp"
#include "core/StringTypes.h"

// Key project headers used across many files
#include "Context.h"
#include "GameState.h"
#include "object/Object.h"
#include "rct2/RCT2.h"
#include "ride/Ride.h"
