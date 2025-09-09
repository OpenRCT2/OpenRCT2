/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include "../Vehicle.h"
#include "SubpositionCommon.hpp"

namespace OpenRCT2::Subposition::Minigolf
{
    using enum VehiclePitch;
    using enum VehicleRoll;

#define MINI_GOLF_STATE(STATE) { kLocationNull, EnumValue(MiniGolfState::STATE), 0, 0, flat, unbanked }
#define MINI_GOLF_ANIMATION(VALUE)                                                                                             \
    { kLocationNull, EnumValue(MiniGolfState::Unk4), EnumValue(MiniGolfAnimation::VALUE), 0, flat, unbanked }

    // clang-format off
    CREATE_VEHICLE_INFO(Flat_0, {
        { 30, 16, 0, 0, flat, unbanked }, { 28, 16, 0, 0, flat, unbanked }, { 26, 16, 0, 0, flat, unbanked }, { 24, 16, 0, 0, flat, unbanked }, { 22, 16, 0, 0, flat, unbanked }, { 20, 16, 0, 0, flat, unbanked },
        { 18, 16, 0, 0, flat, unbanked }, { 16, 16, 0, 0, flat, unbanked }, { 14, 16, 0, 0, flat, unbanked }, { 12, 16, 0, 0, flat, unbanked }, { 10, 16, 0, 0, flat, unbanked }, {  8, 16, 0, 0, flat, unbanked },
        {  6, 16, 0, 0, flat, unbanked }, {  4, 16, 0, 0, flat, unbanked }, {  2, 16, 0, 0, flat, unbanked }, {  0, 16, 0, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Flat_1, {
        { 16,  0, 0, 8, flat, unbanked }, { 16,  2, 0, 8, flat, unbanked }, { 16,  4, 0, 8, flat, unbanked }, { 16,  6, 0, 8, flat, unbanked }, { 16,  8, 0, 8, flat, unbanked }, { 16, 10, 0, 8, flat, unbanked },
        { 16, 12, 0, 8, flat, unbanked }, { 16, 14, 0, 8, flat, unbanked }, { 16, 16, 0, 8, flat, unbanked }, { 16, 18, 0, 8, flat, unbanked }, { 16, 20, 0, 8, flat, unbanked }, { 16, 22, 0, 8, flat, unbanked },
        { 16, 24, 0, 8, flat, unbanked }, { 16, 26, 0, 8, flat, unbanked }, { 16, 28, 0, 8, flat, unbanked }, { 16, 30, 0, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Flat_2, {
        {  0, 16, 0, 16, flat, unbanked }, {  2, 16, 0, 16, flat, unbanked }, {  4, 16, 0, 16, flat, unbanked }, {  6, 16, 0, 16, flat, unbanked }, {  8, 16, 0, 16, flat, unbanked },
        { 10, 16, 0, 16, flat, unbanked }, { 12, 16, 0, 16, flat, unbanked }, { 14, 16, 0, 16, flat, unbanked }, { 16, 16, 0, 16, flat, unbanked }, { 18, 16, 0, 16, flat, unbanked },
        { 20, 16, 0, 16, flat, unbanked }, { 22, 16, 0, 16, flat, unbanked }, { 24, 16, 0, 16, flat, unbanked }, { 26, 16, 0, 16, flat, unbanked }, { 28, 16, 0, 16, flat, unbanked },
        { 30, 16, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Flat_3, {
        { 16, 30, 0, 24, flat, unbanked }, { 16, 28, 0, 24, flat, unbanked }, { 16, 26, 0, 24, flat, unbanked }, { 16, 24, 0, 24, flat, unbanked }, { 16, 22, 0, 24, flat, unbanked },
        { 16, 20, 0, 24, flat, unbanked }, { 16, 18, 0, 24, flat, unbanked }, { 16, 16, 0, 24, flat, unbanked }, { 16, 14, 0, 24, flat, unbanked }, { 16, 12, 0, 24, flat, unbanked },
        { 16, 10, 0, 24, flat, unbanked }, { 16,  8, 0, 24, flat, unbanked }, { 16,  6, 0, 24, flat, unbanked }, { 16,  4, 0, 24, flat, unbanked }, { 16,  2, 0, 24, flat, unbanked },
        { 16,  0, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25_0, {
        { 30, 16,  1, 0, flat, unbanked }, { 28, 16,  2, 0, flat, unbanked }, { 26, 16,  3, 0, flat, unbanked }, { 24, 16,  4, 0, flat, unbanked }, { 22, 16,  5, 0, flat, unbanked },
        { 20, 16,  6, 0, flat, unbanked }, { 18, 16,  7, 0, flat, unbanked }, { 16, 16,  8, 0, flat, unbanked }, { 14, 16,  9, 0, flat, unbanked }, { 12, 16, 10, 0, flat, unbanked },
        { 10, 16, 11, 0, flat, unbanked }, {  8, 16, 12, 0, flat, unbanked }, {  6, 16, 13, 0, flat, unbanked }, {  4, 16, 14, 0, flat, unbanked }, {  2, 16, 15, 0, flat, unbanked },
        {  0, 16, 16, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25_1, {
        { 16,  0,  1, 8, flat, unbanked }, { 16,  2,  2, 8, flat, unbanked }, { 16,  4,  3, 8, flat, unbanked }, { 16,  6,  4, 8, flat, unbanked }, { 16,  8,  5, 8, flat, unbanked },
        { 16, 10,  6, 8, flat, unbanked }, { 16, 12,  7, 8, flat, unbanked }, { 16, 14,  8, 8, flat, unbanked }, { 16, 16,  9, 8, flat, unbanked }, { 16, 18, 10, 8, flat, unbanked },
        { 16, 20, 11, 8, flat, unbanked }, { 16, 22, 12, 8, flat, unbanked }, { 16, 24, 13, 8, flat, unbanked }, { 16, 26, 14, 8, flat, unbanked }, { 16, 28, 15, 8, flat, unbanked },
        { 16, 30, 16, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25_2, {
        {  0, 16,  1, 16, flat, unbanked }, {  2, 16,  2, 16, flat, unbanked }, {  4, 16,  3, 16, flat, unbanked }, {  6, 16,  4, 16, flat, unbanked }, {  8, 16,  5, 16, flat, unbanked },
        { 10, 16,  6, 16, flat, unbanked }, { 12, 16,  7, 16, flat, unbanked }, { 14, 16,  8, 16, flat, unbanked }, { 16, 16,  9, 16, flat, unbanked }, { 18, 16, 10, 16, flat, unbanked },
        { 20, 16, 11, 16, flat, unbanked }, { 22, 16, 12, 16, flat, unbanked }, { 24, 16, 13, 16, flat, unbanked }, { 26, 16, 14, 16, flat, unbanked }, { 28, 16, 15, 16, flat, unbanked },
        { 30, 16, 16, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25_3, {
        { 16, 30,  1, 24, flat, unbanked }, { 16, 28,  2, 24, flat, unbanked }, { 16, 26,  3, 24, flat, unbanked }, { 16, 24,  4, 24, flat, unbanked }, { 16, 22,  5, 24, flat, unbanked },
        { 16, 20,  6, 24, flat, unbanked }, { 16, 18,  7, 24, flat, unbanked }, { 16, 16,  8, 24, flat, unbanked }, { 16, 14,  9, 24, flat, unbanked }, { 16, 12, 10, 24, flat, unbanked },
        { 16, 10, 11, 24, flat, unbanked }, { 16,  8, 12, 24, flat, unbanked }, { 16,  6, 13, 24, flat, unbanked }, { 16,  4, 14, 24, flat, unbanked }, { 16,  2, 15, 24, flat, unbanked },
        { 16,  0, 16, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToUp25_0, {
        { 30, 16, 0, 0, flat, unbanked }, { 28, 16, 0, 0, flat, unbanked }, { 26, 16, 0, 0, flat, unbanked }, { 24, 16, 0, 0, flat, unbanked }, { 22, 16, 0, 0, flat, unbanked }, { 20, 16, 0, 0, flat, unbanked },
        { 18, 16, 0, 0, flat, unbanked }, { 16, 16, 0, 0, flat, unbanked }, { 14, 16, 1, 0, flat, unbanked }, { 12, 16, 2, 0, flat, unbanked }, { 10, 16, 3, 0, flat, unbanked }, {  8, 16, 4, 0, flat, unbanked },
        {  6, 16, 5, 0, flat, unbanked }, {  4, 16, 6, 0, flat, unbanked }, {  2, 16, 7, 0, flat, unbanked }, {  0, 16, 8, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToUp25_1, {
        { 16,  0, 0, 8, flat, unbanked }, { 16,  2, 0, 8, flat, unbanked }, { 16,  4, 0, 8, flat, unbanked }, { 16,  6, 0, 8, flat, unbanked }, { 16,  8, 0, 8, flat, unbanked }, { 16, 10, 0, 8, flat, unbanked },
        { 16, 12, 0, 8, flat, unbanked }, { 16, 14, 0, 8, flat, unbanked }, { 16, 16, 1, 8, flat, unbanked }, { 16, 18, 2, 8, flat, unbanked }, { 16, 20, 3, 8, flat, unbanked }, { 16, 22, 4, 8, flat, unbanked },
        { 16, 24, 5, 8, flat, unbanked }, { 16, 26, 6, 8, flat, unbanked }, { 16, 28, 7, 8, flat, unbanked }, { 16, 30, 8, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToUp25_2, {
        {  0, 16, 0, 16, flat, unbanked }, {  2, 16, 0, 16, flat, unbanked }, {  4, 16, 0, 16, flat, unbanked }, {  6, 16, 0, 16, flat, unbanked }, {  8, 16, 0, 16, flat, unbanked },
        { 10, 16, 0, 16, flat, unbanked }, { 12, 16, 0, 16, flat, unbanked }, { 14, 16, 0, 16, flat, unbanked }, { 16, 16, 1, 16, flat, unbanked }, { 18, 16, 2, 16, flat, unbanked },
        { 20, 16, 3, 16, flat, unbanked }, { 22, 16, 4, 16, flat, unbanked }, { 24, 16, 5, 16, flat, unbanked }, { 26, 16, 6, 16, flat, unbanked }, { 28, 16, 7, 16, flat, unbanked },
        { 30, 16, 8, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToUp25_3, {
        { 16, 30, 0, 24, flat, unbanked }, { 16, 28, 0, 24, flat, unbanked }, { 16, 26, 0, 24, flat, unbanked }, { 16, 24, 0, 24, flat, unbanked }, { 16, 22, 0, 24, flat, unbanked },
        { 16, 20, 0, 24, flat, unbanked }, { 16, 18, 0, 24, flat, unbanked }, { 16, 16, 0, 24, flat, unbanked }, { 16, 14, 1, 24, flat, unbanked }, { 16, 12, 2, 24, flat, unbanked },
        { 16, 10, 3, 24, flat, unbanked }, { 16,  8, 4, 24, flat, unbanked }, { 16,  6, 5, 24, flat, unbanked }, { 16,  4, 6, 24, flat, unbanked }, { 16,  2, 7, 24, flat, unbanked },
        { 16,  0, 8, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25ToFlat_0, {
        { 30, 16, 1, 0, flat, unbanked }, { 28, 16, 2, 0, flat, unbanked }, { 26, 16, 3, 0, flat, unbanked }, { 24, 16, 4, 0, flat, unbanked }, { 22, 16, 5, 0, flat, unbanked }, { 20, 16, 6, 0, flat, unbanked },
        { 18, 16, 7, 0, flat, unbanked }, { 16, 16, 8, 0, flat, unbanked }, { 14, 16, 8, 0, flat, unbanked }, { 12, 16, 8, 0, flat, unbanked }, { 10, 16, 8, 0, flat, unbanked }, {  8, 16, 8, 0, flat, unbanked },
        {  6, 16, 8, 0, flat, unbanked }, {  4, 16, 8, 0, flat, unbanked }, {  2, 16, 8, 0, flat, unbanked }, {  0, 16, 8, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25ToFlat_1, {
        { 16,  0, 1, 8, flat, unbanked }, { 16,  2, 2, 8, flat, unbanked }, { 16,  4, 3, 8, flat, unbanked }, { 16,  6, 4, 8, flat, unbanked }, { 16,  8, 5, 8, flat, unbanked }, { 16, 10, 6, 8, flat, unbanked },
        { 16, 12, 7, 8, flat, unbanked }, { 16, 14, 8, 8, flat, unbanked }, { 16, 16, 8, 8, flat, unbanked }, { 16, 18, 8, 8, flat, unbanked }, { 16, 20, 8, 8, flat, unbanked }, { 16, 22, 8, 8, flat, unbanked },
        { 16, 24, 8, 8, flat, unbanked }, { 16, 26, 8, 8, flat, unbanked }, { 16, 28, 8, 8, flat, unbanked }, { 16, 30, 8, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25ToFlat_2, {
        {  0, 16, 1, 16, flat, unbanked }, {  2, 16, 2, 16, flat, unbanked }, {  4, 16, 3, 16, flat, unbanked }, {  6, 16, 4, 16, flat, unbanked }, {  8, 16, 5, 16, flat, unbanked },
        { 10, 16, 6, 16, flat, unbanked }, { 12, 16, 7, 16, flat, unbanked }, { 14, 16, 8, 16, flat, unbanked }, { 16, 16, 8, 16, flat, unbanked }, { 18, 16, 8, 16, flat, unbanked },
        { 20, 16, 8, 16, flat, unbanked }, { 22, 16, 8, 16, flat, unbanked }, { 24, 16, 8, 16, flat, unbanked }, { 26, 16, 8, 16, flat, unbanked }, { 28, 16, 8, 16, flat, unbanked },
        { 30, 16, 8, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25ToFlat_3, {
        { 16, 30, 1, 24, flat, unbanked }, { 16, 28, 2, 24, flat, unbanked }, { 16, 26, 3, 24, flat, unbanked }, { 16, 24, 4, 24, flat, unbanked }, { 16, 22, 5, 24, flat, unbanked },
        { 16, 20, 6, 24, flat, unbanked }, { 16, 18, 7, 24, flat, unbanked }, { 16, 16, 8, 24, flat, unbanked }, { 16, 14, 8, 24, flat, unbanked }, { 16, 12, 8, 24, flat, unbanked },
        { 16, 10, 8, 24, flat, unbanked }, { 16,  8, 8, 24, flat, unbanked }, { 16,  6, 8, 24, flat, unbanked }, { 16,  4, 8, 24, flat, unbanked }, { 16,  2, 8, 24, flat, unbanked },
        { 16,  0, 8, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25_0, {
        { 30, 16, 16, 0, flat, unbanked }, { 28, 16, 15, 0, flat, unbanked }, { 26, 16, 14, 0, flat, unbanked }, { 24, 16, 13, 0, flat, unbanked }, { 22, 16, 12, 0, flat, unbanked },
        { 20, 16, 11, 0, flat, unbanked }, { 18, 16, 10, 0, flat, unbanked }, { 16, 16,  9, 0, flat, unbanked }, { 14, 16,  8, 0, flat, unbanked }, { 12, 16,  7, 0, flat, unbanked },
        { 10, 16,  6, 0, flat, unbanked }, {  8, 16,  5, 0, flat, unbanked }, {  6, 16,  4, 0, flat, unbanked }, {  4, 16,  3, 0, flat, unbanked }, {  2, 16,  2, 0, flat, unbanked },
        {  0, 16,  1, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25_1, {
        { 16,  0, 16, 8, flat, unbanked }, { 16,  2, 15, 8, flat, unbanked }, { 16,  4, 14, 8, flat, unbanked }, { 16,  6, 13, 8, flat, unbanked }, { 16,  8, 12, 8, flat, unbanked },
        { 16, 10, 11, 8, flat, unbanked }, { 16, 12, 10, 8, flat, unbanked }, { 16, 14,  9, 8, flat, unbanked }, { 16, 16,  8, 8, flat, unbanked }, { 16, 18,  7, 8, flat, unbanked },
        { 16, 20,  6, 8, flat, unbanked }, { 16, 22,  5, 8, flat, unbanked }, { 16, 24,  4, 8, flat, unbanked }, { 16, 26,  3, 8, flat, unbanked }, { 16, 28,  2, 8, flat, unbanked },
        { 16, 30,  1, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25_2, {
        {  0, 16, 16, 16, flat, unbanked }, {  2, 16, 15, 16, flat, unbanked }, {  4, 16, 14, 16, flat, unbanked }, {  6, 16, 13, 16, flat, unbanked }, {  8, 16, 12, 16, flat, unbanked },
        { 10, 16, 11, 16, flat, unbanked }, { 12, 16, 10, 16, flat, unbanked }, { 14, 16,  9, 16, flat, unbanked }, { 16, 16,  8, 16, flat, unbanked }, { 18, 16,  7, 16, flat, unbanked },
        { 20, 16,  6, 16, flat, unbanked }, { 22, 16,  5, 16, flat, unbanked }, { 24, 16,  4, 16, flat, unbanked }, { 26, 16,  3, 16, flat, unbanked }, { 28, 16,  2, 16, flat, unbanked },
        { 30, 16,  1, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25_3, {
        { 16, 30, 16, 24, flat, unbanked }, { 16, 28, 15, 24, flat, unbanked }, { 16, 26, 14, 24, flat, unbanked }, { 16, 24, 13, 24, flat, unbanked }, { 16, 22, 12, 24, flat, unbanked },
        { 16, 20, 11, 24, flat, unbanked }, { 16, 18, 10, 24, flat, unbanked }, { 16, 16,  9, 24, flat, unbanked }, { 16, 14,  8, 24, flat, unbanked }, { 16, 12,  7, 24, flat, unbanked },
        { 16, 10,  6, 24, flat, unbanked }, { 16,  8,  5, 24, flat, unbanked }, { 16,  6,  4, 24, flat, unbanked }, { 16,  4,  3, 24, flat, unbanked }, { 16,  2,  2, 24, flat, unbanked },
        { 16,  0,  1, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToDown25_0, {
        { 30, 16, 8, 0, flat, unbanked }, { 28, 16, 8, 0, flat, unbanked }, { 26, 16, 8, 0, flat, unbanked }, { 24, 16, 8, 0, flat, unbanked }, { 22, 16, 8, 0, flat, unbanked }, { 20, 16, 8, 0, flat, unbanked },
        { 18, 16, 8, 0, flat, unbanked }, { 16, 16, 8, 0, flat, unbanked }, { 14, 16, 8, 0, flat, unbanked }, { 12, 16, 7, 0, flat, unbanked }, { 10, 16, 6, 0, flat, unbanked }, {  8, 16, 5, 0, flat, unbanked },
        {  6, 16, 4, 0, flat, unbanked }, {  4, 16, 3, 0, flat, unbanked }, {  2, 16, 2, 0, flat, unbanked }, {  0, 16, 1, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToDown25_1, {
        { 16,  0, 8, 8, flat, unbanked }, { 16,  2, 8, 8, flat, unbanked }, { 16,  4, 8, 8, flat, unbanked }, { 16,  6, 8, 8, flat, unbanked }, { 16,  8, 8, 8, flat, unbanked }, { 16, 10, 8, 8, flat, unbanked },
        { 16, 12, 8, 8, flat, unbanked }, { 16, 14, 8, 8, flat, unbanked }, { 16, 16, 8, 8, flat, unbanked }, { 16, 18, 7, 8, flat, unbanked }, { 16, 20, 6, 8, flat, unbanked }, { 16, 22, 5, 8, flat, unbanked },
        { 16, 24, 4, 8, flat, unbanked }, { 16, 26, 3, 8, flat, unbanked }, { 16, 28, 2, 8, flat, unbanked }, { 16, 30, 1, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToDown25_2, {
        {  0, 16, 8, 16, flat, unbanked }, {  2, 16, 8, 16, flat, unbanked }, {  4, 16, 8, 16, flat, unbanked }, {  6, 16, 8, 16, flat, unbanked }, {  8, 16, 8, 16, flat, unbanked },
        { 10, 16, 8, 16, flat, unbanked }, { 12, 16, 8, 16, flat, unbanked }, { 14, 16, 8, 16, flat, unbanked }, { 16, 16, 8, 16, flat, unbanked }, { 18, 16, 7, 16, flat, unbanked },
        { 20, 16, 6, 16, flat, unbanked }, { 22, 16, 5, 16, flat, unbanked }, { 24, 16, 4, 16, flat, unbanked }, { 26, 16, 3, 16, flat, unbanked }, { 28, 16, 2, 16, flat, unbanked },
        { 30, 16, 1, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToDown25_3, {
        { 16, 30, 8, 24, flat, unbanked }, { 16, 28, 8, 24, flat, unbanked }, { 16, 26, 8, 24, flat, unbanked }, { 16, 24, 8, 24, flat, unbanked }, { 16, 22, 8, 24, flat, unbanked },
        { 16, 20, 8, 24, flat, unbanked }, { 16, 18, 8, 24, flat, unbanked }, { 16, 16, 8, 24, flat, unbanked }, { 16, 14, 8, 24, flat, unbanked }, { 16, 12, 7, 24, flat, unbanked },
        { 16, 10, 6, 24, flat, unbanked }, { 16,  8, 5, 24, flat, unbanked }, { 16,  6, 4, 24, flat, unbanked }, { 16,  4, 3, 24, flat, unbanked }, { 16,  2, 2, 24, flat, unbanked },
        { 16,  0, 1, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25ToFlat_0, {
        { 30, 16, 8, 0, flat, unbanked }, { 28, 16, 7, 0, flat, unbanked }, { 26, 16, 6, 0, flat, unbanked }, { 24, 16, 5, 0, flat, unbanked }, { 22, 16, 4, 0, flat, unbanked }, { 20, 16, 3, 0, flat, unbanked },
        { 18, 16, 2, 0, flat, unbanked }, { 16, 16, 1, 0, flat, unbanked }, { 14, 16, 0, 0, flat, unbanked }, { 12, 16, 0, 0, flat, unbanked }, { 10, 16, 0, 0, flat, unbanked }, {  8, 16, 0, 0, flat, unbanked },
        {  6, 16, 0, 0, flat, unbanked }, {  4, 16, 0, 0, flat, unbanked }, {  2, 16, 0, 0, flat, unbanked }, {  0, 16, 0, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25ToFlat_1, {
        { 16,  0, 8, 8, flat, unbanked }, { 16,  2, 7, 8, flat, unbanked }, { 16,  4, 6, 8, flat, unbanked }, { 16,  6, 5, 8, flat, unbanked }, { 16,  8, 4, 8, flat, unbanked }, { 16, 10, 3, 8, flat, unbanked },
        { 16, 12, 2, 8, flat, unbanked }, { 16, 14, 1, 8, flat, unbanked }, { 16, 16, 0, 8, flat, unbanked }, { 16, 18, 0, 8, flat, unbanked }, { 16, 20, 0, 8, flat, unbanked }, { 16, 22, 0, 8, flat, unbanked },
        { 16, 24, 0, 8, flat, unbanked }, { 16, 26, 0, 8, flat, unbanked }, { 16, 28, 0, 8, flat, unbanked }, { 16, 30, 0, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25ToFlat_2, {
        {  0, 16, 8, 16, flat, unbanked }, {  2, 16, 7, 16, flat, unbanked }, {  4, 16, 6, 16, flat, unbanked }, {  6, 16, 5, 16, flat, unbanked }, {  8, 16, 4, 16, flat, unbanked },
        { 10, 16, 3, 16, flat, unbanked }, { 12, 16, 2, 16, flat, unbanked }, { 14, 16, 1, 16, flat, unbanked }, { 16, 16, 0, 16, flat, unbanked }, { 18, 16, 0, 16, flat, unbanked },
        { 20, 16, 0, 16, flat, unbanked }, { 22, 16, 0, 16, flat, unbanked }, { 24, 16, 0, 16, flat, unbanked }, { 26, 16, 0, 16, flat, unbanked }, { 28, 16, 0, 16, flat, unbanked },
        { 30, 16, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25ToFlat_3, {
        { 16, 30, 8, 24, flat, unbanked }, { 16, 28, 7, 24, flat, unbanked }, { 16, 26, 6, 24, flat, unbanked }, { 16, 24, 5, 24, flat, unbanked }, { 16, 22, 4, 24, flat, unbanked },
        { 16, 20, 3, 24, flat, unbanked }, { 16, 18, 2, 24, flat, unbanked }, { 16, 16, 1, 24, flat, unbanked }, { 16, 14, 0, 24, flat, unbanked }, { 16, 12, 0, 24, flat, unbanked },
        { 16, 10, 0, 24, flat, unbanked }, { 16,  8, 0, 24, flat, unbanked }, { 16,  6, 0, 24, flat, unbanked }, { 16,  4, 0, 24, flat, unbanked }, { 16,  2, 0, 24, flat, unbanked },
        { 16,  0, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn1Tile_0, {
        { 30, 16, 0,  0, flat, unbanked }, { 26, 16, 0,  0, flat, unbanked }, { 24, 16, 0,  0, flat, unbanked }, { 22, 16, 0,  0, flat, unbanked }, { 20, 16, 0,  0, flat, unbanked },
        { 18, 16, 0,  0, flat, unbanked }, { 16, 16, 0,  0, flat, unbanked }, { 16, 14, 0, 24, flat, unbanked }, { 16, 12, 0, 24, flat, unbanked }, { 16, 10, 0, 24, flat, unbanked },
        { 16,  8, 0, 24, flat, unbanked }, { 16,  6, 0, 24, flat, unbanked }, { 16,  4, 0, 24, flat, unbanked }, { 16,  2, 0, 24, flat, unbanked }, { 16,  0, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn1Tile_1, {
        { 16,  0, 0, 8, flat, unbanked }, { 16,  2, 0, 8, flat, unbanked }, { 16,  4, 0, 8, flat, unbanked }, { 16,  6, 0, 8, flat, unbanked }, { 16,  8, 0, 8, flat, unbanked }, { 16, 10, 0, 8, flat, unbanked },
        { 16, 12, 0, 8, flat, unbanked }, { 16, 14, 0, 8, flat, unbanked }, { 16, 16, 0, 8, flat, unbanked }, { 14, 16, 0, 0, flat, unbanked }, { 12, 16, 0, 0, flat, unbanked }, { 10, 16, 0, 0, flat, unbanked },
        {  8, 16, 0, 0, flat, unbanked }, {  6, 16, 0, 0, flat, unbanked }, {  4, 16, 0, 0, flat, unbanked }, {  2, 16, 0, 0, flat, unbanked }, {  0, 16, 0, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn1Tile_2, {
        {  0, 16, 0, 16, flat, unbanked }, {  2, 16, 0, 16, flat, unbanked }, {  4, 16, 0, 16, flat, unbanked }, {  6, 16, 0, 16, flat, unbanked }, {  8, 16, 0, 16, flat, unbanked },
        { 10, 16, 0, 16, flat, unbanked }, { 12, 16, 0, 16, flat, unbanked }, { 14, 16, 0, 16, flat, unbanked }, { 16, 16, 0, 16, flat, unbanked }, { 16, 18, 0,  8, flat, unbanked },
        { 16, 20, 0,  8, flat, unbanked }, { 16, 22, 0,  8, flat, unbanked }, { 16, 24, 0,  8, flat, unbanked }, { 16, 26, 0,  8, flat, unbanked }, { 16, 28, 0,  8, flat, unbanked },
        { 16, 30, 0,  8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn1Tile_3, {
        { 16, 30, 0, 24, flat, unbanked }, { 16, 28, 0, 24, flat, unbanked }, { 16, 26, 0, 24, flat, unbanked }, { 16, 24, 0, 24, flat, unbanked }, { 16, 22, 0, 24, flat, unbanked },
        { 16, 20, 0, 24, flat, unbanked }, { 16, 18, 0, 24, flat, unbanked }, { 16, 16, 0, 24, flat, unbanked }, { 18, 16, 0, 16, flat, unbanked }, { 20, 16, 0, 16, flat, unbanked },
        { 22, 16, 0, 16, flat, unbanked }, { 24, 16, 0, 16, flat, unbanked }, { 26, 16, 0, 16, flat, unbanked }, { 28, 16, 0, 16, flat, unbanked }, { 30, 16, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn1Tile_0, {
        { 30, 16, 0, 0, flat, unbanked }, { 28, 16, 0, 0, flat, unbanked }, { 26, 16, 0, 0, flat, unbanked }, { 24, 16, 0, 0, flat, unbanked }, { 22, 16, 0, 0, flat, unbanked }, { 20, 16, 0, 0, flat, unbanked },
        { 18, 16, 0, 0, flat, unbanked }, { 16, 16, 0, 0, flat, unbanked }, { 16, 18, 0, 8, flat, unbanked }, { 16, 20, 0, 8, flat, unbanked }, { 16, 22, 0, 8, flat, unbanked }, { 16, 24, 0, 8, flat, unbanked },
        { 16, 26, 0, 8, flat, unbanked }, { 16, 28, 0, 8, flat, unbanked }, { 16, 30, 0, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn1Tile_1, {
        { 16,  0, 0,  8, flat, unbanked }, { 16,  2, 0,  8, flat, unbanked }, { 16,  4, 0,  8, flat, unbanked }, { 16,  6, 0,  8, flat, unbanked }, { 16,  8, 0,  8, flat, unbanked },
        { 16, 10, 0,  8, flat, unbanked }, { 16, 12, 0,  8, flat, unbanked }, { 16, 14, 0,  8, flat, unbanked }, { 16, 16, 0,  8, flat, unbanked }, { 18, 16, 0, 16, flat, unbanked },
        { 20, 16, 0, 16, flat, unbanked }, { 22, 16, 0, 16, flat, unbanked }, { 24, 16, 0, 16, flat, unbanked }, { 26, 16, 0, 16, flat, unbanked }, { 28, 16, 0, 16, flat, unbanked },
        { 30, 16, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn1Tile_2, {
        {  0, 16, 0, 16, flat, unbanked }, {  2, 16, 0, 16, flat, unbanked }, {  4, 16, 0, 16, flat, unbanked }, {  6, 16, 0, 16, flat, unbanked }, {  8, 16, 0, 16, flat, unbanked },
        { 10, 16, 0, 16, flat, unbanked }, { 12, 16, 0, 16, flat, unbanked }, { 14, 16, 0, 16, flat, unbanked }, { 16, 16, 0, 16, flat, unbanked }, { 16, 14, 0, 24, flat, unbanked },
        { 16, 12, 0, 24, flat, unbanked }, { 16, 10, 0, 24, flat, unbanked }, { 16,  8, 0, 24, flat, unbanked }, { 16,  6, 0, 24, flat, unbanked }, { 16,  4, 0, 24, flat, unbanked },
        { 16,  2, 0, 24, flat, unbanked }, { 16,  0, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn1Tile_3, {
        { 16, 30, 0, 24, flat, unbanked }, { 16, 28, 0, 24, flat, unbanked }, { 16, 26, 0, 24, flat, unbanked }, { 16, 24, 0, 24, flat, unbanked }, { 16, 22, 0, 24, flat, unbanked },
        { 16, 20, 0, 24, flat, unbanked }, { 16, 18, 0, 24, flat, unbanked }, { 16, 16, 0, 24, flat, unbanked }, { 14, 16, 0,  0, flat, unbanked }, { 12, 16, 0,  0, flat, unbanked },
        { 10, 16, 0,  0, flat, unbanked }, {  8, 16, 0,  0, flat, unbanked }, {  6, 16, 0,  0, flat, unbanked }, {  4, 16, 0,  0, flat, unbanked }, {  2, 16, 0,  0, flat, unbanked },
        {  0, 16, 0,  0, flat, unbanked },
    })
    // clang-format on

    namespace PlayerPathA
    {
        // clang-format off
        CREATE_VEHICLE_INFO(MinigolfHoleA_0, {
                          MINI_GOLF_STATE(Unk0),  {  31, 16, 0,  0, flat, unbanked },  {  29, 16, 0,  0, flat, unbanked },  {  27, 16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  27, 16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  27, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  27, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  27, 16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  25, 16, 0,  0, flat, unbanked },  {  23, 16, 0,  0, flat, unbanked },  {  21, 16, 0,  0, flat, unbanked },
             {  19, 16, 0,  0, flat, unbanked },  {  17, 16, 0,  0, flat, unbanked },  {  15, 16, 0,  0, flat, unbanked },  {  13, 16, 0,  0, flat, unbanked },
             {  11, 16, 0,  0, flat, unbanked },  {   9, 16, 0,  0, flat, unbanked },  {   7, 16, 0,  0, flat, unbanked },  {   5, 16, 0,  0, flat, unbanked },
             {   3, 16, 0,  0, flat, unbanked },  {   1, 16, 0,  0, flat, unbanked },  {  -1, 16, 0,  0, flat, unbanked },  {  -3, 16, 0,  0, flat, unbanked },
             {  -5, 16, 0,  0, flat, unbanked },  {  -7, 16, 0,  0, flat, unbanked },  {  -9, 16, 0,  0, flat, unbanked },  { -11, 16, 0,  0, flat, unbanked },
             { -13, 16, 0,  0, flat, unbanked },  { -15, 16, 0,  0, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
             { -15, 16, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { -17, 16, 0,  0, flat, unbanked },
             { -19, 16, 0,  0, flat, unbanked },  { -21, 16, 0,  0, flat, unbanked },  { -23, 16, 0,  0, flat, unbanked },  { -25, 16, 0,  0, flat, unbanked },
             { -27, 16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { -29, 16, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_1, {
                          MINI_GOLF_STATE(Unk0),    { 16,  0, 0, 8, flat, unbanked },    { 16,  2, 0, 8, flat, unbanked },    { 16,  4, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),    { 16,  4, 0, 8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),    { 16,  4, 0, 8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),    { 16,  4, 0, 0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),    { 16,  4, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16,  4, 0, 0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),    { 16,  4, 0, 8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),    { 16,  6, 0, 8, flat, unbanked },    { 16,  8, 0, 8, flat, unbanked },    { 16, 10, 0, 8, flat, unbanked },
               { 16, 12, 0, 8, flat, unbanked },    { 16, 14, 0, 8, flat, unbanked },    { 16, 16, 0, 8, flat, unbanked },    { 16, 18, 0, 8, flat, unbanked },
               { 16, 20, 0, 8, flat, unbanked },    { 16, 22, 0, 8, flat, unbanked },    { 16, 24, 0, 8, flat, unbanked },    { 16, 26, 0, 8, flat, unbanked },
               { 16, 28, 0, 8, flat, unbanked },    { 16, 30, 0, 8, flat, unbanked },    { 16, 32, 0, 8, flat, unbanked },    { 16, 34, 0, 8, flat, unbanked },
               { 16, 36, 0, 8, flat, unbanked },    { 16, 38, 0, 8, flat, unbanked },    { 16, 40, 0, 8, flat, unbanked },    { 16, 42, 0, 8, flat, unbanked },
               { 16, 44, 0, 8, flat, unbanked },    { 16, 46, 0, 8, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
               { 16, 46, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),    { 16, 48, 0, 8, flat, unbanked },
               { 16, 50, 0, 8, flat, unbanked },    { 16, 52, 0, 8, flat, unbanked },    { 16, 54, 0, 8, flat, unbanked },    { 16, 56, 0, 8, flat, unbanked },
               { 16, 58, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),    { 16, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },   {  2, 15, 0, 16, flat, unbanked },   {  4, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   {  4, 15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   {  4, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   {  4, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  4, 15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {  6, 15, 0, 16, flat, unbanked },   {  8, 15, 0, 16, flat, unbanked },   { 10, 15, 0, 16, flat, unbanked },
              { 12, 15, 0, 16, flat, unbanked },   { 14, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 18, 15, 0, 16, flat, unbanked },
              { 20, 15, 0, 16, flat, unbanked },   { 22, 15, 0, 16, flat, unbanked },   { 24, 15, 0, 16, flat, unbanked },   { 26, 15, 0, 16, flat, unbanked },
              { 28, 15, 0, 16, flat, unbanked },   { 30, 15, 0, 16, flat, unbanked },   { 32, 15, 0, 16, flat, unbanked },   { 34, 15, 0, 16, flat, unbanked },
              { 36, 15, 0, 16, flat, unbanked },   { 38, 15, 0, 16, flat, unbanked },   { 40, 15, 0, 16, flat, unbanked },   { 42, 15, 0, 16, flat, unbanked },
              { 44, 15, 0, 16, flat, unbanked },   { 46, 15, 0, 16, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
              { 46, 15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),   { 48, 15, 0, 16, flat, unbanked },
              { 50, 15, 0, 16, flat, unbanked },   { 52, 15, 0, 16, flat, unbanked },   { 54, 15, 0, 16, flat, unbanked },   { 56, 15, 0, 16, flat, unbanked },
              { 58, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },  { 15,  29, 0, 24, flat, unbanked },  { 15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 15,  25, 0, 24, flat, unbanked },  { 15,  23, 0, 24, flat, unbanked },  { 15,  21, 0, 24, flat, unbanked },
             { 15,  19, 0, 24, flat, unbanked },  { 15,  17, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 15,  13, 0, 24, flat, unbanked },
             { 15,  11, 0, 24, flat, unbanked },  { 15,   9, 0, 24, flat, unbanked },  { 15,   7, 0, 24, flat, unbanked },  { 15,   5, 0, 24, flat, unbanked },
             { 15,   3, 0, 24, flat, unbanked },  { 15,   1, 0, 24, flat, unbanked },  { 15,  -1, 0, 24, flat, unbanked },  { 15,  -3, 0, 24, flat, unbanked },
             { 15,  -5, 0, 24, flat, unbanked },  { 15,  -7, 0, 24, flat, unbanked },  { 15,  -9, 0, 24, flat, unbanked },  { 15, -11, 0, 24, flat, unbanked },
             { 15, -13, 0, 24, flat, unbanked },  { 15, -15, 0, 24, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
             { 15, -15, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { 15, -17, 0, 24, flat, unbanked },
             { 15, -19, 0, 24, flat, unbanked },  { 15, -21, 0, 24, flat, unbanked },  { 15, -23, 0, 24, flat, unbanked },  { 15, -25, 0, 24, flat, unbanked },
             { 15, -27, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_0, {
                          MINI_GOLF_STATE(Unk0),  {  31, 16, 0,  0, flat, unbanked },  {  29, 16, 0,  0, flat, unbanked },  {  27, 16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  27, 16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  27, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  27, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  27, 16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  25, 16, 0,  0, flat, unbanked },  {  23, 16, 0,  0, flat, unbanked },  {  21, 16, 0,  0, flat, unbanked },
             {  19, 16, 0,  0, flat, unbanked },  {  17, 16, 0,  0, flat, unbanked },  {  15, 16, 0,  0, flat, unbanked },  {  13, 16, 0,  0, flat, unbanked },
             {  11, 16, 0,  0, flat, unbanked },  {   9, 16, 0,  0, flat, unbanked },  {   7, 16, 0,  0, flat, unbanked },  {   5, 16, 0,  0, flat, unbanked },
             {   3, 16, 0,  0, flat, unbanked },  {   1, 16, 0,  0, flat, unbanked },  {  -1, 16, 0,  0, flat, unbanked },  {  -3, 16, 0,  0, flat, unbanked },
             {  -5, 16, 0,  0, flat, unbanked },  {  -7, 16, 0,  0, flat, unbanked },  {  -9, 16, 0,  0, flat, unbanked },  { -11, 16, 0,  0, flat, unbanked },
             { -13, 16, 0,  0, flat, unbanked },  { -15, 16, 0,  0, flat, unbanked },  { -17, 16, 0,  0, flat, unbanked },  { -19, 16, 0,  0, flat, unbanked },
             { -21, 16, 0,  0, flat, unbanked },  { -23, 16, 0,  0, flat, unbanked },  { -25, 16, 0,  0, flat, unbanked },  { -27, 16, 0,  0, flat, unbanked },
                MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),  { -27, 16, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk6),  { -27, 16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { -29, 16, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_1, {
                          MINI_GOLF_STATE(Unk0),    { 16,  0, 0, 8, flat, unbanked },    { 16,  2, 0, 8, flat, unbanked },    { 16,  4, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),    { 16,  4, 0, 8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),    { 16,  4, 0, 8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),    { 16,  4, 0, 0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),    { 16,  4, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16,  4, 0, 0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),    { 16,  4, 0, 8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),    { 16,  6, 0, 8, flat, unbanked },    { 16,  8, 0, 8, flat, unbanked },    { 16, 10, 0, 8, flat, unbanked },
               { 16, 12, 0, 8, flat, unbanked },    { 16, 14, 0, 8, flat, unbanked },    { 16, 16, 0, 8, flat, unbanked },    { 16, 18, 0, 8, flat, unbanked },
               { 16, 20, 0, 8, flat, unbanked },    { 16, 22, 0, 8, flat, unbanked },    { 16, 24, 0, 8, flat, unbanked },    { 16, 26, 0, 8, flat, unbanked },
               { 16, 28, 0, 8, flat, unbanked },    { 16, 30, 0, 8, flat, unbanked },    { 16, 32, 0, 8, flat, unbanked },    { 16, 34, 0, 8, flat, unbanked },
               { 16, 36, 0, 8, flat, unbanked },    { 16, 38, 0, 8, flat, unbanked },    { 16, 40, 0, 8, flat, unbanked },    { 16, 42, 0, 8, flat, unbanked },
               { 16, 44, 0, 8, flat, unbanked },    { 16, 46, 0, 8, flat, unbanked },    { 16, 48, 0, 8, flat, unbanked },    { 16, 50, 0, 8, flat, unbanked },
               { 16, 52, 0, 8, flat, unbanked },    { 16, 54, 0, 8, flat, unbanked },    { 16, 56, 0, 8, flat, unbanked },    { 16, 58, 0, 8, flat, unbanked },
                MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),    { 16, 58, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk6),    { 16, 58, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),    { 16, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },   {  2, 15, 0, 16, flat, unbanked },   {  4, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   {  4, 15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   {  4, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   {  4, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  4, 15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {  6, 15, 0, 16, flat, unbanked },   {  8, 15, 0, 16, flat, unbanked },   { 10, 15, 0, 16, flat, unbanked },
              { 12, 15, 0, 16, flat, unbanked },   { 14, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 18, 15, 0, 16, flat, unbanked },
              { 20, 15, 0, 16, flat, unbanked },   { 22, 15, 0, 16, flat, unbanked },   { 24, 15, 0, 16, flat, unbanked },   { 26, 15, 0, 16, flat, unbanked },
              { 28, 15, 0, 16, flat, unbanked },   { 30, 15, 0, 16, flat, unbanked },   { 32, 15, 0, 16, flat, unbanked },   { 34, 15, 0, 16, flat, unbanked },
              { 36, 15, 0, 16, flat, unbanked },   { 38, 15, 0, 16, flat, unbanked },   { 40, 15, 0, 16, flat, unbanked },   { 42, 15, 0, 16, flat, unbanked },
              { 44, 15, 0, 16, flat, unbanked },   { 46, 15, 0, 16, flat, unbanked },   { 48, 15, 0, 16, flat, unbanked },   { 50, 15, 0, 16, flat, unbanked },
              { 52, 15, 0, 16, flat, unbanked },   { 54, 15, 0, 16, flat, unbanked },   { 56, 15, 0, 16, flat, unbanked },   { 58, 15, 0, 16, flat, unbanked },
                MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),   { 58, 15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk6),   { 58, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },  { 15,  29, 0, 24, flat, unbanked },  { 15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 15,  25, 0, 24, flat, unbanked },  { 15,  23, 0, 24, flat, unbanked },  { 15,  21, 0, 24, flat, unbanked },
             { 15,  19, 0, 24, flat, unbanked },  { 15,  17, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 15,  13, 0, 24, flat, unbanked },
             { 15,  11, 0, 24, flat, unbanked },  { 15,   9, 0, 24, flat, unbanked },  { 15,   7, 0, 24, flat, unbanked },  { 15,   5, 0, 24, flat, unbanked },
             { 15,   3, 0, 24, flat, unbanked },  { 15,   1, 0, 24, flat, unbanked },  { 15,  -1, 0, 24, flat, unbanked },  { 15,  -3, 0, 24, flat, unbanked },
             { 15,  -5, 0, 24, flat, unbanked },  { 15,  -7, 0, 24, flat, unbanked },  { 15,  -9, 0, 24, flat, unbanked },  { 15, -11, 0, 24, flat, unbanked },
             { 15, -13, 0, 24, flat, unbanked },  { 15, -15, 0, 24, flat, unbanked },  { 15, -17, 0, 24, flat, unbanked },  { 15, -19, 0, 24, flat, unbanked },
             { 15, -21, 0, 24, flat, unbanked },  { 15, -23, 0, 24, flat, unbanked },  { 15, -25, 0, 24, flat, unbanked },  { 15, -27, 0, 24, flat, unbanked },
                MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),  { 15, -27, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk6),  { 15, -27, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_0, {
                          MINI_GOLF_STATE(Unk0),  {  31, 16, 0,  0, flat, unbanked },  {  29, 16, 0,  0, flat, unbanked },  {  27, 16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  27, 16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  27, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  27, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  27, 16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  25, 16, 0,  0, flat, unbanked },  {  23, 16, 0,  0, flat, unbanked },  {  21, 16, 0,  0, flat, unbanked },
             {  19, 16, 0,  0, flat, unbanked },  {  17, 16, 0,  0, flat, unbanked },  {  15, 16, 0,  0, flat, unbanked },  {  13, 16, 0,  0, flat, unbanked },
             {  11, 16, 0,  0, flat, unbanked },  {   9, 16, 5,  0, flat, unbanked },  {   7, 16, 5,  0, flat, unbanked },  {   5, 16, 5,  0, flat, unbanked },
             {   3, 16, 5,  0, flat, unbanked },  {   1, 16, 5,  0, flat, unbanked },  {  -1, 16, 5,  0, flat, unbanked },  {  -3, 16, 5,  0, flat, unbanked },
             {  -5, 16, 5,  0, flat, unbanked },  {  -7, 16, 5,  0, flat, unbanked },  {  -9, 16, 0,  0, flat, unbanked },  { -11, 16, 0,  0, flat, unbanked },
             { -13, 16, 0,  0, flat, unbanked },  { -15, 16, 0,  0, flat, unbanked },  { -17, 16, 0,  0, flat, unbanked },  { -19, 16, 0,  0, flat, unbanked },
             { -21, 16, 0,  0, flat, unbanked },  { -23, 16, 0,  0, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
             { -23, 16, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { -23, 16, 0,  0, flat, unbanked },
             { -25, 16, 0,  0, flat, unbanked },  { -27, 16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { -29, 16, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_1, {
                          MINI_GOLF_STATE(Unk0),    { 16,  0, 0, 8, flat, unbanked },    { 16,  2, 0, 8, flat, unbanked },    { 16,  4, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),    { 16,  4, 0, 8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),    { 16,  4, 0, 8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),    { 16,  4, 0, 0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),    { 16,  4, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16,  4, 0, 0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),    { 16,  4, 0, 8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),    { 16,  6, 0, 8, flat, unbanked },    { 16,  8, 0, 8, flat, unbanked },    { 16, 10, 0, 8, flat, unbanked },
               { 16, 12, 0, 8, flat, unbanked },    { 16, 14, 0, 8, flat, unbanked },    { 16, 16, 0, 8, flat, unbanked },    { 16, 18, 0, 8, flat, unbanked },
               { 16, 20, 0, 8, flat, unbanked },    { 16, 22, 5, 8, flat, unbanked },    { 16, 24, 5, 8, flat, unbanked },    { 16, 26, 5, 8, flat, unbanked },
               { 16, 28, 5, 8, flat, unbanked },    { 16, 30, 5, 8, flat, unbanked },    { 16, 32, 5, 8, flat, unbanked },    { 16, 34, 5, 8, flat, unbanked },
               { 16, 36, 5, 8, flat, unbanked },    { 16, 38, 5, 8, flat, unbanked },    { 16, 40, 0, 8, flat, unbanked },    { 16, 42, 0, 8, flat, unbanked },
               { 16, 44, 0, 8, flat, unbanked },    { 16, 46, 0, 8, flat, unbanked },    { 16, 48, 0, 8, flat, unbanked },    { 16, 50, 0, 8, flat, unbanked },
               { 16, 52, 0, 8, flat, unbanked },    { 16, 54, 0, 8, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
               { 16, 54, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),    { 16, 54, 0, 8, flat, unbanked },
               { 16, 56, 0, 8, flat, unbanked },    { 16, 58, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),    { 16, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },   {  2, 15, 0, 16, flat, unbanked },   {  4, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   {  4, 15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   {  4, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   {  4, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  4, 15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {  6, 15, 0, 16, flat, unbanked },   {  8, 15, 0, 16, flat, unbanked },   { 10, 15, 0, 16, flat, unbanked },
              { 12, 15, 0, 16, flat, unbanked },   { 14, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 18, 15, 0, 16, flat, unbanked },
              { 20, 15, 0, 16, flat, unbanked },   { 22, 15, 5, 16, flat, unbanked },   { 24, 15, 5, 16, flat, unbanked },   { 26, 15, 5, 16, flat, unbanked },
              { 28, 15, 5, 16, flat, unbanked },   { 30, 15, 5, 16, flat, unbanked },   { 32, 15, 5, 16, flat, unbanked },   { 34, 15, 5, 16, flat, unbanked },
              { 36, 15, 5, 16, flat, unbanked },   { 38, 15, 5, 16, flat, unbanked },   { 40, 15, 0, 16, flat, unbanked },   { 42, 15, 0, 16, flat, unbanked },
              { 44, 15, 0, 16, flat, unbanked },   { 46, 15, 0, 16, flat, unbanked },   { 48, 15, 0, 16, flat, unbanked },   { 50, 15, 0, 16, flat, unbanked },
              { 52, 15, 0, 16, flat, unbanked },   { 54, 15, 0, 16, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
              { 54, 15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),   { 54, 15, 0, 16, flat, unbanked },
              { 56, 15, 0, 16, flat, unbanked },   { 58, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },  { 15,  29, 0, 24, flat, unbanked },  { 15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 15,  25, 0, 24, flat, unbanked },  { 15,  23, 0, 24, flat, unbanked },  { 15,  21, 0, 24, flat, unbanked },
             { 15,  19, 0, 24, flat, unbanked },  { 15,  17, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 15,  13, 0, 24, flat, unbanked },
             { 15,  11, 0, 24, flat, unbanked },  { 15,   9, 5, 24, flat, unbanked },  { 15,   7, 5, 24, flat, unbanked },  { 15,   5, 5, 24, flat, unbanked },
             { 15,   3, 5, 24, flat, unbanked },  { 15,   1, 5, 24, flat, unbanked },  { 15,  -1, 5, 24, flat, unbanked },  { 15,  -3, 5, 24, flat, unbanked },
             { 15,  -5, 5, 24, flat, unbanked },  { 15,  -7, 5, 24, flat, unbanked },  { 15,  -9, 0, 24, flat, unbanked },  { 15, -11, 0, 24, flat, unbanked },
             { 15, -13, 0, 24, flat, unbanked },  { 15, -15, 0, 24, flat, unbanked },  { 15, -17, 0, 24, flat, unbanked },  { 15, -19, 0, 24, flat, unbanked },
             { 15, -21, 0, 24, flat, unbanked },  { 15, -23, 0, 24, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
             { 15, -23, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { 15, -23, 0, 24, flat, unbanked },
             { 15, -25, 0, 24, flat, unbanked },  { 15, -27, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_0, {
                          MINI_GOLF_STATE(Unk0),  {  31, 16, 0,  0, flat, unbanked },  {  29, 16, 0,  0, flat, unbanked },  {  27, 16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  27, 16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  27, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  27, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  27, 16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  25, 16, 0,  0, flat, unbanked },  {  23, 16, 0,  0, flat, unbanked },  {  21, 16, 0,  0, flat, unbanked },
             {  19, 16, 0,  0, flat, unbanked },  {  17, 16, 0,  0, flat, unbanked },  {  15, 16, 0,  0, flat, unbanked },  {  13, 16, 0,  0, flat, unbanked },
             {  11, 16, 0,  0, flat, unbanked },  {   9, 16, 0,  0, flat, unbanked },  {   7, 16, 0,  0, flat, unbanked },  {   5, 16, 0,  0, flat, unbanked },
             {   3, 16, 0,  0, flat, unbanked },  {   1, 16, 0,  0, flat, unbanked },  {  -1, 16, 0,  0, flat, unbanked },  {  -3, 16, 0,  0, flat, unbanked },
             {  -3, 18, 0,  8, flat, unbanked },  {  -3, 20, 0,  8, flat, unbanked },  {  -3, 22, 0,  8, flat, unbanked },  {  -3, 24, 0,  8, flat, unbanked },
             {  -3, 26, 0,  8, flat, unbanked },  {  -3, 28, 0,  8, flat, unbanked },  {  -3, 30, 0,  8, flat, unbanked },  {  -3, 32, 0,  8, flat, unbanked },
             {  -3, 34, 0,  8, flat, unbanked },  {  -3, 36, 0,  8, flat, unbanked },  {  -3, 38, 0,  8, flat, unbanked },  {  -3, 40, 0,  8, flat, unbanked },
             {  -3, 42, 0,  8, flat, unbanked },  {  -3, 44, 0,  8, flat, unbanked },  {  -3, 46, 0,  8, flat, unbanked },  {  -3, 48, 0,  8, flat, unbanked },
             {  -5, 48, 0,  0, flat, unbanked },  {  -7, 48, 0,  0, flat, unbanked },  {  -9, 48, 0,  0, flat, unbanked },  { -11, 48, 0,  0, flat, unbanked },
             { -13, 48, 0,  0, flat, unbanked },  { -15, 48, 0,  0, flat, unbanked },  { -17, 48, 0,  0, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),  { -17, 49, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
             { -17, 48, 0,  8, flat, unbanked },  { -17, 50, 0,  8, flat, unbanked },  { -17, 52, 0,  8, flat, unbanked },  { -17, 54, 0,  8, flat, unbanked },
             { -17, 56, 0,  8, flat, unbanked },  { -17, 58, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { -17, 60, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_1, {
                          MINI_GOLF_STATE(Unk0),   { 16,  0, 0,  8, flat, unbanked },   { 16,  2, 0,  8, flat, unbanked },   { 16,  4, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 16,  4, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   { 16,  6, 0,  8, flat, unbanked },   { 16,  8, 0,  8, flat, unbanked },   { 16, 10, 0,  8, flat, unbanked },
              { 16, 12, 0,  8, flat, unbanked },   { 16, 14, 0,  8, flat, unbanked },   { 16, 16, 0,  8, flat, unbanked },   { 16, 18, 0,  8, flat, unbanked },
              { 16, 20, 0,  8, flat, unbanked },   { 16, 22, 0,  8, flat, unbanked },   { 16, 24, 0,  8, flat, unbanked },   { 16, 26, 0,  8, flat, unbanked },
              { 16, 28, 0,  8, flat, unbanked },   { 16, 30, 0,  8, flat, unbanked },   { 16, 32, 0,  8, flat, unbanked },   { 16, 34, 0,  8, flat, unbanked },
              { 18, 34, 0, 16, flat, unbanked },   { 20, 34, 0, 16, flat, unbanked },   { 22, 34, 0, 16, flat, unbanked },   { 24, 34, 0, 16, flat, unbanked },
              { 26, 34, 0, 16, flat, unbanked },   { 28, 34, 0, 16, flat, unbanked },   { 30, 34, 0, 16, flat, unbanked },   { 32, 34, 0, 16, flat, unbanked },
              { 34, 34, 0, 16, flat, unbanked },   { 36, 34, 0, 16, flat, unbanked },   { 38, 34, 0, 16, flat, unbanked },   { 40, 34, 0, 16, flat, unbanked },
              { 42, 34, 0, 16, flat, unbanked },   { 44, 34, 0, 16, flat, unbanked },   { 46, 34, 0, 16, flat, unbanked },   { 48, 34, 0, 16, flat, unbanked },
              { 48, 36, 0,  8, flat, unbanked },   { 48, 38, 0,  8, flat, unbanked },   { 48, 40, 0,  8, flat, unbanked },   { 48, 42, 0,  8, flat, unbanked },
              { 48, 44, 0,  8, flat, unbanked },   { 48, 46, 0,  8, flat, unbanked },   { 48, 48, 0,  8, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),   { 49, 48, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
              { 48, 48, 0, 16, flat, unbanked },   { 50, 48, 0, 16, flat, unbanked },   { 52, 48, 0, 16, flat, unbanked },   { 54, 48, 0, 16, flat, unbanked },
              { 56, 48, 0, 16, flat, unbanked },   { 58, 48, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   { 60, 48, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_2, {
                          MINI_GOLF_STATE(Unk0),  {  0,  15, 0, 16, flat, unbanked },  {  2,  15, 0, 16, flat, unbanked },  {  4,  15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  4,  15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  4,  15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  4,  15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  4,  15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  4,  15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),  {  4,  15, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  6,  15, 0, 16, flat, unbanked },  {  8,  15, 0, 16, flat, unbanked },  { 10,  15, 0, 16, flat, unbanked },
             { 12,  15, 0, 16, flat, unbanked },  { 14,  15, 0, 16, flat, unbanked },  { 16,  15, 0, 16, flat, unbanked },  { 18,  15, 0, 16, flat, unbanked },
             { 20,  15, 0, 16, flat, unbanked },  { 22,  15, 0, 16, flat, unbanked },  { 24,  15, 0, 16, flat, unbanked },  { 26,  15, 0, 16, flat, unbanked },
             { 28,  15, 0, 16, flat, unbanked },  { 30,  15, 0, 16, flat, unbanked },  { 32,  15, 0, 16, flat, unbanked },  { 34,  15, 0, 16, flat, unbanked },
             { 34,  13, 0, 24, flat, unbanked },  { 34,  11, 0, 24, flat, unbanked },  { 34,   9, 0, 24, flat, unbanked },  { 34,   7, 0, 24, flat, unbanked },
             { 34,   5, 0, 24, flat, unbanked },  { 34,   3, 0, 24, flat, unbanked },  { 34,   1, 0, 24, flat, unbanked },  { 34,  -1, 0, 24, flat, unbanked },
             { 34,  -3, 0, 24, flat, unbanked },  { 34,  -5, 0, 24, flat, unbanked },  { 34,  -7, 0, 24, flat, unbanked },  { 34,  -9, 0, 24, flat, unbanked },
             { 34, -11, 0, 24, flat, unbanked },  { 34, -13, 0, 24, flat, unbanked },  { 34, -15, 0, 24, flat, unbanked },  { 34, -17, 0, 24, flat, unbanked },
             { 36, -17, 0, 16, flat, unbanked },  { 38, -17, 0, 16, flat, unbanked },  { 40, -17, 0, 16, flat, unbanked },  { 42, -17, 0, 16, flat, unbanked },
             { 44, -17, 0, 16, flat, unbanked },  { 46, -17, 0, 16, flat, unbanked },  { 48, -17, 0, 16, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),  { 48, -18, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
             { 48, -17, 0, 24, flat, unbanked },  { 48, -19, 0, 24, flat, unbanked },  { 48, -21, 0, 24, flat, unbanked },  { 48, -23, 0, 24, flat, unbanked },
             { 48, -25, 0, 24, flat, unbanked },  { 48, -27, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 48, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_3, {
                          MINI_GOLF_STATE(Unk0), {  15,  31, 0, 24, flat, unbanked }, {  15,  29, 0, 24, flat, unbanked }, {  15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2), {  15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5), {  15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3), {  15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5), {  15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5), {  15,  27, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk), {  15,  25, 0, 24, flat, unbanked }, {  15,  23, 0, 24, flat, unbanked }, {  15,  21, 0, 24, flat, unbanked },
            {  15,  19, 0, 24, flat, unbanked }, {  15,  17, 0, 24, flat, unbanked }, {  15,  15, 0, 24, flat, unbanked }, {  15,  13, 0, 24, flat, unbanked },
            {  15,  11, 0, 24, flat, unbanked }, {  15,   9, 0, 24, flat, unbanked }, {  15,   7, 0, 24, flat, unbanked }, {  15,   5, 0, 24, flat, unbanked },
            {  15,   3, 0, 24, flat, unbanked }, {  15,   1, 0, 24, flat, unbanked }, {  15,  -1, 0, 24, flat, unbanked }, {  15,  -3, 0, 24, flat, unbanked },
            {  13,  -3, 0,  0, flat, unbanked }, {  11,  -3, 0,  0, flat, unbanked }, {   9,  -3, 0,  0, flat, unbanked }, {   7,  -3, 0,  0, flat, unbanked },
            {   5,  -3, 0,  0, flat, unbanked }, {   3,  -3, 0,  0, flat, unbanked }, {   1,  -3, 0,  0, flat, unbanked }, {  -1,  -3, 0,  0, flat, unbanked },
            {  -3,  -3, 0,  0, flat, unbanked }, {  -5,  -3, 0,  0, flat, unbanked }, {  -7,  -3, 0,  0, flat, unbanked }, {  -9,  -3, 0,  0, flat, unbanked },
            { -11,  -3, 0,  0, flat, unbanked }, { -13,  -3, 0,  0, flat, unbanked }, { -15,  -3, 0,  0, flat, unbanked }, { -17,  -3, 0,  0, flat, unbanked },
            { -17,  -5, 0, 24, flat, unbanked }, { -17,  -7, 0, 24, flat, unbanked }, { -17,  -9, 0, 24, flat, unbanked }, { -17, -11, 0, 24, flat, unbanked },
            { -17, -13, 0, 24, flat, unbanked }, { -17, -15, 0, 24, flat, unbanked }, { -17, -17, 0, 24, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5), { -18, -17, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
            { -17, -17, 0,  0, flat, unbanked }, { -19, -17, 0,  0, flat, unbanked }, { -21, -17, 0,  0, flat, unbanked }, { -23, -17, 0,  0, flat, unbanked },
            { -25, -17, 0,  0, flat, unbanked }, { -27, -17, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk3), { -29, -17, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_0, {
                          MINI_GOLF_STATE(Unk0), {  31,  16, 0,  0, flat, unbanked }, {  29,  16, 0,  0, flat, unbanked }, {  27,  16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2), {  27,  16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5), {  27,  16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3), {  27,  16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5), {  27,  16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  27,  16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5), {  27,  16, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk), {  25,  16, 0,  0, flat, unbanked }, {  23,  16, 0,  0, flat, unbanked }, {  21,  16, 0,  0, flat, unbanked },
            {  19,  16, 0,  0, flat, unbanked }, {  17,  16, 0,  0, flat, unbanked }, {  15,  16, 0,  0, flat, unbanked }, {  13,  16, 0,  0, flat, unbanked },
            {  11,  16, 0,  0, flat, unbanked }, {   9,  16, 0,  0, flat, unbanked }, {   7,  16, 0,  0, flat, unbanked }, {   5,  16, 0,  0, flat, unbanked },
            {   3,  16, 0,  0, flat, unbanked }, {   1,  16, 0,  0, flat, unbanked }, {  -1,  16, 0,  0, flat, unbanked }, {  -3,  16, 0,  0, flat, unbanked },
            {  -3,  14, 0, 24, flat, unbanked }, {  -3,  12, 0, 24, flat, unbanked }, {  -3,  10, 0, 24, flat, unbanked }, {  -3,   8, 0, 24, flat, unbanked },
            {  -3,   6, 0, 24, flat, unbanked }, {  -3,   4, 0, 24, flat, unbanked }, {  -3,   2, 0, 24, flat, unbanked }, {  -3,   0, 0, 24, flat, unbanked },
            {  -3,  -2, 0, 24, flat, unbanked }, {  -3,  -4, 0, 24, flat, unbanked }, {  -3,  -6, 0, 24, flat, unbanked }, {  -3,  -8, 0, 24, flat, unbanked },
            {  -3, -10, 0, 24, flat, unbanked }, {  -3, -12, 0, 24, flat, unbanked }, {  -3, -14, 0, 24, flat, unbanked }, {  -3, -16, 0, 24, flat, unbanked },
            {  -5, -16, 0,  0, flat, unbanked }, {  -7, -16, 0,  0, flat, unbanked }, {  -9, -16, 0,  0, flat, unbanked }, { -11, -16, 0,  0, flat, unbanked },
            { -13, -16, 0,  0, flat, unbanked }, { -15, -16, 0,  0, flat, unbanked }, { -17, -16, 0,  0, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5), { -17, -17, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
            { -17, -16, 0, 24, flat, unbanked }, { -17, -18, 0, 24, flat, unbanked }, { -17, -20, 0, 24, flat, unbanked }, { -17, -22, 0, 24, flat, unbanked },
            { -17, -24, 0, 24, flat, unbanked }, { -17, -26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3), { -17, -28, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_1, {
                          MINI_GOLF_STATE(Unk0),   {  16,  0, 0, 8, flat, unbanked },   {  16,  2, 0, 8, flat, unbanked },   {  16,  4, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   {  16,  4, 0, 8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   {  16,  4, 0, 8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   {  16,  4, 0, 0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   {  16,  4, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  16,  4, 0, 0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),   {  16,  4, 0, 8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {  16,  6, 0, 8, flat, unbanked },   {  16,  8, 0, 8, flat, unbanked },   {  16, 10, 0, 8, flat, unbanked },
              {  16, 12, 0, 8, flat, unbanked },   {  16, 14, 0, 8, flat, unbanked },   {  16, 16, 0, 8, flat, unbanked },   {  16, 18, 0, 8, flat, unbanked },
              {  16, 20, 0, 8, flat, unbanked },   {  16, 22, 0, 8, flat, unbanked },   {  16, 24, 0, 8, flat, unbanked },   {  16, 26, 0, 8, flat, unbanked },
              {  16, 28, 0, 8, flat, unbanked },   {  16, 30, 0, 8, flat, unbanked },   {  16, 32, 0, 8, flat, unbanked },   {  16, 34, 0, 8, flat, unbanked },
              {  14, 34, 0, 0, flat, unbanked },   {  12, 34, 0, 0, flat, unbanked },   {  10, 34, 0, 0, flat, unbanked },   {   8, 34, 0, 0, flat, unbanked },
              {   6, 34, 0, 0, flat, unbanked },   {   4, 34, 0, 0, flat, unbanked },   {   2, 34, 0, 0, flat, unbanked },   {   0, 34, 0, 0, flat, unbanked },
              {  -2, 34, 0, 0, flat, unbanked },   {  -4, 34, 0, 0, flat, unbanked },   {  -6, 34, 0, 0, flat, unbanked },   {  -8, 34, 0, 0, flat, unbanked },
              { -10, 34, 0, 0, flat, unbanked },   { -12, 34, 0, 0, flat, unbanked },   { -14, 34, 0, 0, flat, unbanked },   { -16, 34, 0, 0, flat, unbanked },
              { -16, 36, 0, 8, flat, unbanked },   { -16, 38, 0, 8, flat, unbanked },   { -16, 40, 0, 8, flat, unbanked },   { -16, 42, 0, 8, flat, unbanked },
              { -16, 44, 0, 8, flat, unbanked },   { -16, 46, 0, 8, flat, unbanked },   { -16, 48, 0, 8, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),   { -17, 48, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
              { -16, 48, 0, 0, flat, unbanked },   { -18, 48, 0, 0, flat, unbanked },   { -20, 48, 0, 0, flat, unbanked },   { -22, 48, 0, 0, flat, unbanked },
              { -24, 48, 0, 0, flat, unbanked },   { -26, 48, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk3),   { -28, 48, 0, 0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },   {  2, 15, 0, 16, flat, unbanked },   {  4, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   {  4, 15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   {  4, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   {  4, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  4, 15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {  6, 15, 0, 16, flat, unbanked },   {  8, 15, 0, 16, flat, unbanked },   { 10, 15, 0, 16, flat, unbanked },
              { 12, 15, 0, 16, flat, unbanked },   { 14, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 18, 15, 0, 16, flat, unbanked },
              { 20, 15, 0, 16, flat, unbanked },   { 22, 15, 0, 16, flat, unbanked },   { 24, 15, 0, 16, flat, unbanked },   { 26, 15, 0, 16, flat, unbanked },
              { 28, 15, 0, 16, flat, unbanked },   { 30, 15, 0, 16, flat, unbanked },   { 32, 15, 0, 16, flat, unbanked },   { 34, 15, 0, 16, flat, unbanked },
              { 34, 17, 0,  8, flat, unbanked },   { 34, 19, 0,  8, flat, unbanked },   { 34, 21, 0,  8, flat, unbanked },   { 34, 23, 0,  8, flat, unbanked },
              { 34, 25, 0,  8, flat, unbanked },   { 34, 27, 0,  8, flat, unbanked },   { 34, 29, 0,  8, flat, unbanked },   { 34, 31, 0,  8, flat, unbanked },
              { 34, 33, 0,  8, flat, unbanked },   { 34, 35, 0,  8, flat, unbanked },   { 34, 37, 0,  8, flat, unbanked },   { 34, 39, 0,  8, flat, unbanked },
              { 34, 41, 0,  8, flat, unbanked },   { 34, 43, 0,  8, flat, unbanked },   { 34, 45, 0,  8, flat, unbanked },   { 34, 47, 0,  8, flat, unbanked },
              { 36, 47, 0, 16, flat, unbanked },   { 38, 47, 0, 16, flat, unbanked },   { 40, 47, 0, 16, flat, unbanked },   { 42, 47, 0, 16, flat, unbanked },
              { 44, 47, 0, 16, flat, unbanked },   { 46, 47, 0, 16, flat, unbanked },   { 48, 47, 0, 16, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),   { 48, 48, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
              { 48, 47, 0,  8, flat, unbanked },   { 48, 49, 0,  8, flat, unbanked },   { 48, 51, 0,  8, flat, unbanked },   { 48, 53, 0,  8, flat, unbanked },
              { 48, 55, 0,  8, flat, unbanked },   { 48, 57, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk3),   { 48, 59, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },  { 15,  29, 0, 24, flat, unbanked },  { 15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),           MINI_GOLF_ANIMATION(Jump),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 15,  25, 0, 24, flat, unbanked },  { 15,  23, 0, 24, flat, unbanked },  { 15,  21, 0, 24, flat, unbanked },
             { 15,  19, 0, 24, flat, unbanked },  { 15,  17, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 15,  13, 0, 24, flat, unbanked },
             { 15,  11, 0, 24, flat, unbanked },  { 15,   9, 0, 24, flat, unbanked },  { 15,   7, 0, 24, flat, unbanked },  { 15,   5, 0, 24, flat, unbanked },
             { 15,   3, 0, 24, flat, unbanked },  { 15,   1, 0, 24, flat, unbanked },  { 15,  -1, 0, 24, flat, unbanked },  { 15,  -3, 0, 24, flat, unbanked },
             { 17,  -3, 0, 16, flat, unbanked },  { 19,  -3, 0, 16, flat, unbanked },  { 21,  -3, 0, 16, flat, unbanked },  { 23,  -3, 0, 16, flat, unbanked },
             { 25,  -3, 0, 16, flat, unbanked },  { 27,  -3, 0, 16, flat, unbanked },  { 29,  -3, 0, 16, flat, unbanked },  { 31,  -3, 0, 16, flat, unbanked },
             { 33,  -3, 0, 16, flat, unbanked },  { 35,  -3, 0, 16, flat, unbanked },  { 37,  -3, 0, 16, flat, unbanked },  { 39,  -3, 0, 16, flat, unbanked },
             { 41,  -3, 0, 16, flat, unbanked },  { 43,  -3, 0, 16, flat, unbanked },  { 45,  -3, 0, 16, flat, unbanked },  { 47,  -3, 0, 16, flat, unbanked },
             { 47,  -5, 0, 24, flat, unbanked },  { 47,  -7, 0, 24, flat, unbanked },  { 47,  -9, 0, 24, flat, unbanked },  { 47, -11, 0, 24, flat, unbanked },
             { 47, -13, 0, 24, flat, unbanked },  { 47, -15, 0, 24, flat, unbanked },  { 47, -17, 0, 24, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),  { 48, -17, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
             { 47, -17, 0, 16, flat, unbanked },  { 49, -17, 0, 16, flat, unbanked },  { 51, -17, 0, 16, flat, unbanked },  { 53, -17, 0, 16, flat, unbanked },
             { 55, -17, 0, 16, flat, unbanked },  { 57, -17, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 59, -17, 0, 16, flat, unbanked },
        })

        static constexpr const VehicleInfoList * PlayerPathA[] = {
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3,
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // EndStation
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // BeginStation
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // MiddleStation
            &Up25_0,                  &Up25_1,                  &Up25_2,                  &Up25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60
            &FlatToUp25_0,            &FlatToUp25_1,            &FlatToUp25_2,            &FlatToUp25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp25
            &Up25ToFlat_0,            &Up25ToFlat_1,            &Up25ToFlat_2,            &Up25ToFlat_3,
            &Down25_0,                &Down25_1,                &Down25_2,                &Down25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60
            &FlatToDown25_0,          &FlatToDown25_1,          &FlatToDown25_2,          &FlatToDown25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown25
            &Down25ToFlat_0,          &Down25ToFlat_1,          &Down25ToFlat_2,          &Down25ToFlat_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BankedLeftQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BankedRightQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendLeft
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendRight
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftVerticalLoop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightVerticalLoop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankedQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankedQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesDown25
            &LeftQuarterTurn1Tile_0,  &LeftQuarterTurn1Tile_1,  &LeftQuarterTurn1Tile_2,  &LeftQuarterTurn1Tile_3,
            &RightQuarterTurn1Tile_0, &RightQuarterTurn1Tile_1, &RightQuarterTurn1Tile_2, &RightQuarterTurn1Tile_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftTwistDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightTwistDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftTwistUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightTwistUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // TowerBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // TowerSection
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToUp60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToFlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToDown60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToFlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendLeftCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendRightCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixUpSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixUpSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixDownSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixDownSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixUpLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixUpLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixDownLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixDownLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn1TileUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn1TileUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn1TileDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn1TileDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Brakes
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Booster
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Maze
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterBankedHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterBankedHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterBankedHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterBankedHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25LeftBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25RightBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Waterfall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Rapids
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // OnRidePhoto
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25LeftBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25RightBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Watersplash
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp60LongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToFlatLongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Whirlpool
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToFlatLongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown60LongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // CableLiftHill
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // ReverseFreefallSlope
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // ReverseFreefallVertical
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down90ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up90ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BrakeForDrop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthBankToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthBankToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthBankToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthBankToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60ToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60ToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LogFlumeReverser
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SpinningTunnel
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBarrelRollUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBarrelRollUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBarrelRollDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBarrelRollDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToLeftQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToRightQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesDown25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesDown25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // PoweredLift
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftLargeHalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightLargeHalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftLargeHalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightLargeHalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerTwistUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerTwistUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerTwistDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerTwistDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlyerHalfLoopUninvertedUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlyerHalfLoopInvertedDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HeartLineTransferUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HeartLineTransferDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHeartLineRoll
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHeartLineRoll
            &MinigolfHoleA_0,         &MinigolfHoleA_1,         &MinigolfHoleA_2,         &MinigolfHoleA_3,
            &MinigolfHoleB_0,         &MinigolfHoleB_1,         &MinigolfHoleB_2,         &MinigolfHoleB_3,
            &MinigolfHoleC_0,         &MinigolfHoleC_1,         &MinigolfHoleC_2,         &MinigolfHoleC_3,
            &MinigolfHoleD_0,         &MinigolfHoleD_1,         &MinigolfHoleD_2,         &MinigolfHoleD_3,
            &MinigolfHoleE_0,         &MinigolfHoleE_1,         &MinigolfHoleE_2,         &MinigolfHoleE_3,
        };
        // clang-format on
        static_assert(std::size(PlayerPathA) == kVehicleTrackSubpositionSizeMinigolf);
    } // namespace PlayerPathA

    namespace BallPathA
    {
        // clang-format off
        CREATE_VEHICLE_INFO(MinigolfHoleA_0, {
                          MINI_GOLF_STATE(Unk0),   {  31, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  26, 16, 0, 0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  26, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  25, 16, 0, 0, flat, unbanked },
              {  22, 16, 0, 0, flat, unbanked },   {  19, 16, 0, 0, flat, unbanked },   {  16, 16, 0, 0, flat, unbanked },   {  13, 17, 0, 0, flat, unbanked },
              {  10, 17, 0, 0, flat, unbanked },   {   8, 17, 0, 0, flat, unbanked },   {   6, 17, 0, 0, flat, unbanked },   {   4, 17, 0, 0, flat, unbanked },
              {   2, 17, 0, 0, flat, unbanked },   {   0, 18, 0, 0, flat, unbanked },   {  -2, 18, 0, 0, flat, unbanked },   {  -4, 18, 0, 0, flat, unbanked },
              {  -6, 18, 0, 0, flat, unbanked },   {  -8, 18, 0, 0, flat, unbanked },   { -10, 18, 0, 0, flat, unbanked },   { -12, 18, 0, 0, flat, unbanked },
              { -14, 19, 0, 0, flat, unbanked },   { -16, 19, 0, 0, flat, unbanked },   { -18, 19, 0, 0, flat, unbanked },   { -20, 19, 0, 0, flat, unbanked },
              { -22, 19, 0, 0, flat, unbanked },   { -24, 19, 0, 0, flat, unbanked },   { -26, 19, 0, 0, flat, unbanked },   { -28, 20, 0, 0, flat, unbanked },
              { -30, 20, 0, 0, flat, unbanked },   { -29, 20, 0, 0, flat, unbanked },   { -28, 20, 0, 0, flat, unbanked },   { -27, 20, 0, 0, flat, unbanked },
              { -26, 19, 0, 0, flat, unbanked },   { -25, 19, 0, 0, flat, unbanked },   { -24, 19, 0, 0, flat, unbanked },   { -23, 18, 0, 0, flat, unbanked },
              { -22, 18, 0, 0, flat, unbanked },   { -21, 18, 0, 0, flat, unbanked },   { -20, 17, 0, 0, flat, unbanked },   { -19, 17, 0, 0, flat, unbanked },
              { -18, 17, 0, 0, flat, unbanked },   { -17, 16, 0, 0, flat, unbanked },   { -16, 16, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk2),   { -29, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
              { -29, 16, 0, 0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_1, {
                          MINI_GOLF_STATE(Unk0),    { 16,  0, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16,  5, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),    { 16,  5, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),    { 16,  6, 0, 8, flat, unbanked },
               { 16,  9, 0, 8, flat, unbanked },    { 16, 12, 0, 8, flat, unbanked },    { 16, 15, 0, 8, flat, unbanked },    { 17, 18, 0, 8, flat, unbanked },
               { 17, 21, 0, 8, flat, unbanked },    { 17, 23, 0, 8, flat, unbanked },    { 17, 25, 0, 8, flat, unbanked },    { 17, 27, 0, 8, flat, unbanked },
               { 17, 29, 0, 8, flat, unbanked },    { 18, 31, 0, 8, flat, unbanked },    { 18, 33, 0, 8, flat, unbanked },    { 18, 35, 0, 8, flat, unbanked },
               { 18, 37, 0, 8, flat, unbanked },    { 18, 39, 0, 8, flat, unbanked },    { 18, 41, 0, 8, flat, unbanked },    { 18, 43, 0, 8, flat, unbanked },
               { 19, 45, 0, 8, flat, unbanked },    { 19, 47, 0, 8, flat, unbanked },    { 19, 49, 0, 8, flat, unbanked },    { 19, 51, 0, 8, flat, unbanked },
               { 19, 53, 0, 8, flat, unbanked },    { 19, 55, 0, 8, flat, unbanked },    { 19, 57, 0, 8, flat, unbanked },    { 20, 59, 0, 8, flat, unbanked },
               { 20, 61, 0, 8, flat, unbanked },    { 20, 60, 0, 8, flat, unbanked },    { 20, 59, 0, 8, flat, unbanked },    { 20, 58, 0, 8, flat, unbanked },
               { 19, 57, 0, 8, flat, unbanked },    { 19, 56, 0, 8, flat, unbanked },    { 19, 55, 0, 8, flat, unbanked },    { 18, 54, 0, 8, flat, unbanked },
               { 18, 53, 0, 8, flat, unbanked },    { 18, 52, 0, 8, flat, unbanked },    { 17, 51, 0, 8, flat, unbanked },    { 17, 50, 0, 8, flat, unbanked },
               { 17, 49, 0, 8, flat, unbanked },    { 16, 48, 0, 8, flat, unbanked },    { 16, 47, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk2),    { 16, 60, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
               { 16, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  5, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  5, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  6, 15, 0, 16, flat, unbanked },
              {  9, 15, 0, 16, flat, unbanked },   { 12, 15, 0, 16, flat, unbanked },   { 15, 15, 0, 16, flat, unbanked },   { 18, 14, 0, 16, flat, unbanked },
              { 21, 14, 0, 16, flat, unbanked },   { 23, 14, 0, 16, flat, unbanked },   { 25, 14, 0, 16, flat, unbanked },   { 27, 14, 0, 16, flat, unbanked },
              { 29, 14, 0, 16, flat, unbanked },   { 31, 13, 0, 16, flat, unbanked },   { 33, 13, 0, 16, flat, unbanked },   { 35, 13, 0, 16, flat, unbanked },
              { 37, 13, 0, 16, flat, unbanked },   { 39, 13, 0, 16, flat, unbanked },   { 41, 13, 0, 16, flat, unbanked },   { 43, 13, 0, 16, flat, unbanked },
              { 45, 12, 0, 16, flat, unbanked },   { 47, 12, 0, 16, flat, unbanked },   { 49, 12, 0, 16, flat, unbanked },   { 51, 12, 0, 16, flat, unbanked },
              { 53, 12, 0, 16, flat, unbanked },   { 55, 12, 0, 16, flat, unbanked },   { 57, 12, 0, 16, flat, unbanked },   { 59, 11, 0, 16, flat, unbanked },
              { 61, 11, 0, 16, flat, unbanked },   { 60, 11, 0, 16, flat, unbanked },   { 59, 11, 0, 16, flat, unbanked },   { 58, 11, 0, 16, flat, unbanked },
              { 57, 12, 0, 16, flat, unbanked },   { 56, 12, 0, 16, flat, unbanked },   { 55, 12, 0, 16, flat, unbanked },   { 54, 13, 0, 16, flat, unbanked },
              { 53, 13, 0, 16, flat, unbanked },   { 52, 13, 0, 16, flat, unbanked },   { 51, 14, 0, 16, flat, unbanked },   { 50, 14, 0, 16, flat, unbanked },
              { 49, 14, 0, 16, flat, unbanked },   { 48, 15, 0, 16, flat, unbanked },   { 47, 15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk2),   { 60, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
              { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15,  25, 0, 24, flat, unbanked },
             { 15,  22, 0, 24, flat, unbanked },  { 15,  19, 0, 24, flat, unbanked },  { 15,  16, 0, 24, flat, unbanked },  { 14,  13, 0, 24, flat, unbanked },
             { 14,  10, 0, 24, flat, unbanked },  { 14,   8, 0, 24, flat, unbanked },  { 14,   6, 0, 24, flat, unbanked },  { 14,   4, 0, 24, flat, unbanked },
             { 14,   2, 0, 24, flat, unbanked },  { 13,   0, 0, 24, flat, unbanked },  { 13,  -2, 0, 24, flat, unbanked },  { 13,  -4, 0, 24, flat, unbanked },
             { 13,  -6, 0, 24, flat, unbanked },  { 13,  -8, 0, 24, flat, unbanked },  { 13, -10, 0, 24, flat, unbanked },  { 13, -12, 0, 24, flat, unbanked },
             { 12, -14, 0, 24, flat, unbanked },  { 12, -16, 0, 24, flat, unbanked },  { 12, -18, 0, 24, flat, unbanked },  { 12, -20, 0, 24, flat, unbanked },
             { 12, -22, 0, 24, flat, unbanked },  { 12, -24, 0, 24, flat, unbanked },  { 12, -26, 0, 24, flat, unbanked },  { 11, -28, 0, 24, flat, unbanked },
             { 11, -30, 0, 24, flat, unbanked },  { 11, -29, 0, 24, flat, unbanked },  { 11, -28, 0, 24, flat, unbanked },  { 11, -27, 0, 24, flat, unbanked },
             { 12, -26, 0, 24, flat, unbanked },  { 12, -25, 0, 24, flat, unbanked },  { 12, -24, 0, 24, flat, unbanked },  { 13, -23, 0, 24, flat, unbanked },
             { 13, -22, 0, 24, flat, unbanked },  { 13, -21, 0, 24, flat, unbanked },  { 14, -20, 0, 24, flat, unbanked },  { 14, -19, 0, 24, flat, unbanked },
             { 14, -18, 0, 24, flat, unbanked },  { 15, -17, 0, 24, flat, unbanked },  { 15, -16, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk2),  { 15, -29, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
             { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_0, {
                          MINI_GOLF_STATE(Unk0),   {  31, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  26, 16, 0, 0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  26, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  25, 16, 0, 0, flat, unbanked },
              {  22, 16, 0, 0, flat, unbanked },   {  19, 15, 0, 0, flat, unbanked },   {  16, 15, 0, 0, flat, unbanked },   {  13, 14, 0, 0, flat, unbanked },
              {  10, 14, 0, 0, flat, unbanked },   {   7, 13, 0, 0, flat, unbanked },   {   4, 13, 0, 0, flat, unbanked },   {   1, 12, 0, 0, flat, unbanked },
              {  -2, 12, 0, 0, flat, unbanked },   {  -2, 15, 0, 0, flat, unbanked },   {  -2, 18, 0, 0, flat, unbanked },   {  -2, 21, 0, 0, flat, unbanked },
              {  -2, 24, 0, 0, flat, unbanked },   {  -2, 26, 0, 0, flat, unbanked },   {  -4, 26, 0, 0, flat, unbanked },   {  -6, 26, 0, 0, flat, unbanked },
              {  -8, 26, 0, 0, flat, unbanked },   { -10, 26, 0, 0, flat, unbanked },   { -11, 25, 0, 0, flat, unbanked },   { -11, 23, 0, 0, flat, unbanked },
              { -11, 21, 0, 0, flat, unbanked },   { -11, 19, 0, 0, flat, unbanked },   { -10, 17, 0, 0, flat, unbanked },   { -10, 15, 0, 0, flat, unbanked },
              { -10, 13, 0, 0, flat, unbanked },   { -12, 13, 0, 0, flat, unbanked },   { -14, 13, 0, 0, flat, unbanked },   { -16, 13, 0, 0, flat, unbanked },
              { -18, 13, 0, 0, flat, unbanked },   { -20, 14, 0, 0, flat, unbanked },   { -22, 14, 0, 0, flat, unbanked },   { -23, 14, 0, 0, flat, unbanked },
              { -24, 14, 0, 0, flat, unbanked },   { -25, 14, 0, 0, flat, unbanked },   { -26, 14, 0, 0, flat, unbanked },   { -27, 14, 0, 0, flat, unbanked },
              { -28, 14, 0, 0, flat, unbanked },   { -29, 14, 0, 0, flat, unbanked },   { -30, 14, 0, 0, flat, unbanked },   { -31, 14, 0, 0, flat, unbanked },
              { -30, 14, 0, 0, flat, unbanked },   { -29, 14, 0, 0, flat, unbanked },   { -28, 15, 0, 0, flat, unbanked },   { -27, 15, 0, 0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { -29, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),   { -29, 16, 0, 0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_1, {
                          MINI_GOLF_STATE(Unk0),    { 16,  0, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16,  5, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),    { 16,  5, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),    { 16,  6, 0, 8, flat, unbanked },
               { 16,  9, 0, 8, flat, unbanked },    { 15, 12, 0, 8, flat, unbanked },    { 15, 15, 0, 8, flat, unbanked },    { 14, 18, 0, 8, flat, unbanked },
               { 14, 21, 0, 8, flat, unbanked },    { 13, 24, 0, 8, flat, unbanked },    { 13, 27, 0, 8, flat, unbanked },    { 12, 30, 0, 8, flat, unbanked },
               { 12, 33, 0, 8, flat, unbanked },    { 15, 33, 0, 8, flat, unbanked },    { 18, 33, 0, 8, flat, unbanked },    { 21, 33, 0, 8, flat, unbanked },
               { 24, 33, 0, 8, flat, unbanked },    { 26, 33, 0, 8, flat, unbanked },    { 26, 35, 0, 8, flat, unbanked },    { 26, 37, 0, 8, flat, unbanked },
               { 26, 39, 0, 8, flat, unbanked },    { 26, 41, 0, 8, flat, unbanked },    { 25, 42, 0, 8, flat, unbanked },    { 23, 42, 0, 8, flat, unbanked },
               { 21, 42, 0, 8, flat, unbanked },    { 19, 42, 0, 8, flat, unbanked },    { 17, 41, 0, 8, flat, unbanked },    { 15, 41, 0, 8, flat, unbanked },
               { 13, 41, 0, 8, flat, unbanked },    { 13, 43, 0, 8, flat, unbanked },    { 13, 45, 0, 8, flat, unbanked },    { 13, 47, 0, 8, flat, unbanked },
               { 13, 49, 0, 8, flat, unbanked },    { 14, 51, 0, 8, flat, unbanked },    { 14, 53, 0, 8, flat, unbanked },    { 14, 54, 0, 8, flat, unbanked },
               { 14, 55, 0, 8, flat, unbanked },    { 14, 56, 0, 8, flat, unbanked },    { 14, 57, 0, 8, flat, unbanked },    { 14, 58, 0, 8, flat, unbanked },
               { 14, 59, 0, 8, flat, unbanked },    { 14, 60, 0, 8, flat, unbanked },    { 14, 61, 0, 8, flat, unbanked },    { 14, 62, 0, 8, flat, unbanked },
               { 14, 61, 0, 8, flat, unbanked },    { 14, 60, 0, 8, flat, unbanked },    { 15, 59, 0, 8, flat, unbanked },    { 15, 58, 0, 8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),    { 16, 60, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),    { 16, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  5, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  5, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  6, 15, 0, 16, flat, unbanked },
              {  9, 15, 0, 16, flat, unbanked },   { 12, 16, 0, 16, flat, unbanked },   { 15, 16, 0, 16, flat, unbanked },   { 18, 17, 0, 16, flat, unbanked },
              { 21, 17, 0, 16, flat, unbanked },   { 24, 18, 0, 16, flat, unbanked },   { 27, 18, 0, 16, flat, unbanked },   { 30, 19, 0, 16, flat, unbanked },
              { 33, 19, 0, 16, flat, unbanked },   { 33, 16, 0, 16, flat, unbanked },   { 33, 13, 0, 16, flat, unbanked },   { 33, 10, 0, 16, flat, unbanked },
              { 33,  7, 0, 16, flat, unbanked },   { 33,  5, 0, 16, flat, unbanked },   { 35,  5, 0, 16, flat, unbanked },   { 37,  5, 0, 16, flat, unbanked },
              { 39,  5, 0, 16, flat, unbanked },   { 41,  5, 0, 16, flat, unbanked },   { 42,  6, 0, 16, flat, unbanked },   { 42,  8, 0, 16, flat, unbanked },
              { 42, 10, 0, 16, flat, unbanked },   { 42, 12, 0, 16, flat, unbanked },   { 41, 14, 0, 16, flat, unbanked },   { 41, 16, 0, 16, flat, unbanked },
              { 41, 18, 0, 16, flat, unbanked },   { 43, 18, 0, 16, flat, unbanked },   { 45, 18, 0, 16, flat, unbanked },   { 47, 18, 0, 16, flat, unbanked },
              { 49, 18, 0, 16, flat, unbanked },   { 51, 17, 0, 16, flat, unbanked },   { 53, 17, 0, 16, flat, unbanked },   { 54, 17, 0, 16, flat, unbanked },
              { 55, 17, 0, 16, flat, unbanked },   { 56, 17, 0, 16, flat, unbanked },   { 57, 17, 0, 16, flat, unbanked },   { 58, 17, 0, 16, flat, unbanked },
              { 59, 17, 0, 16, flat, unbanked },   { 60, 17, 0, 16, flat, unbanked },   { 61, 17, 0, 16, flat, unbanked },   { 62, 17, 0, 16, flat, unbanked },
              { 61, 17, 0, 16, flat, unbanked },   { 60, 17, 0, 16, flat, unbanked },   { 59, 16, 0, 16, flat, unbanked },   { 58, 16, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { 60, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),   { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15,  25, 0, 24, flat, unbanked },
             { 15,  22, 0, 24, flat, unbanked },  { 16,  19, 0, 24, flat, unbanked },  { 16,  16, 0, 24, flat, unbanked },  { 17,  13, 0, 24, flat, unbanked },
             { 17,  10, 0, 24, flat, unbanked },  { 18,   7, 0, 24, flat, unbanked },  { 18,   4, 0, 24, flat, unbanked },  { 19,   1, 0, 24, flat, unbanked },
             { 19,  -2, 0, 24, flat, unbanked },  { 16,  -2, 0, 24, flat, unbanked },  { 13,  -2, 0, 24, flat, unbanked },  { 10,  -2, 0, 24, flat, unbanked },
             {  7,  -2, 0, 24, flat, unbanked },  {  5,  -2, 0, 24, flat, unbanked },  {  5,  -4, 0, 24, flat, unbanked },  {  5,  -6, 0, 24, flat, unbanked },
             {  5,  -8, 0, 24, flat, unbanked },  {  5, -10, 0, 24, flat, unbanked },  {  6, -11, 0, 24, flat, unbanked },  {  8, -11, 0, 24, flat, unbanked },
             { 10, -11, 0, 24, flat, unbanked },  { 12, -11, 0, 24, flat, unbanked },  { 14, -10, 0, 24, flat, unbanked },  { 16, -10, 0, 24, flat, unbanked },
             { 18, -10, 0, 24, flat, unbanked },  { 18, -12, 0, 24, flat, unbanked },  { 18, -14, 0, 24, flat, unbanked },  { 18, -16, 0, 24, flat, unbanked },
             { 18, -18, 0, 24, flat, unbanked },  { 17, -20, 0, 24, flat, unbanked },  { 17, -22, 0, 24, flat, unbanked },  { 17, -23, 0, 24, flat, unbanked },
             { 17, -24, 0, 24, flat, unbanked },  { 17, -25, 0, 24, flat, unbanked },  { 17, -26, 0, 24, flat, unbanked },  { 17, -27, 0, 24, flat, unbanked },
             { 17, -28, 0, 24, flat, unbanked },  { 17, -29, 0, 24, flat, unbanked },  { 17, -30, 0, 24, flat, unbanked },  { 17, -31, 0, 24, flat, unbanked },
             { 17, -30, 0, 24, flat, unbanked },  { 17, -29, 0, 24, flat, unbanked },  { 16, -28, 0, 24, flat, unbanked },  { 16, -27, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),  { 15, -29, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),  { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_0, {
                          MINI_GOLF_STATE(Unk0),   {  31, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  26, 16, 0, 0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  26, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  24, 16, 0, 0, flat, unbanked },
              {  21, 17, 0, 0, flat, unbanked },   {  18, 18, 0, 0, flat, unbanked },   {  15, 19, 0, 0, flat, unbanked },   {  12, 19, 0, 0, flat, unbanked },
              {  10, 20, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   {   7, 20, 0, 0, flat, unbanked },   {   5, 19, 0, 0, flat, unbanked },
              {   3, 16, 0, 0, flat, unbanked },   {   1, 16, 0, 0, flat, unbanked },   {  -1, 17, 0, 0, flat, unbanked },   {  -3, 16, 0, 0, flat, unbanked },
              {  -5, 16, 0, 0, flat, unbanked },   {  -7, 16, 0, 0, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   {  -8, 16, 0, 0, flat, unbanked },
              { -10, 16, 0, 0, flat, unbanked },   { -12, 16, 0, 0, flat, unbanked },   { -14, 16, 0, 0, flat, unbanked },   { -15, 16, 0, 0, flat, unbanked },
              { -16, 16, 0, 0, flat, unbanked },   { -17, 16, 0, 0, flat, unbanked },   { -18, 16, 0, 0, flat, unbanked },   { -19, 16, 0, 0, flat, unbanked },
              { -20, 16, 0, 0, flat, unbanked },   { -21, 16, 0, 0, flat, unbanked },   { -22, 16, 0, 0, flat, unbanked },   { -23, 16, 0, 0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { -29, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),   { -29, 16, 0, 0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_1, {
                          MINI_GOLF_STATE(Unk0),    { 16,  0, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16,  5, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),    { 16,  5, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),    { 16,  7, 0, 8, flat, unbanked },
               { 17, 10, 0, 8, flat, unbanked },    { 18, 13, 0, 8, flat, unbanked },    { 19, 16, 0, 8, flat, unbanked },    { 19, 19, 0, 8, flat, unbanked },
               { 20, 21, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),    { 20, 24, 0, 8, flat, unbanked },    { 19, 26, 0, 8, flat, unbanked },
               { 16, 28, 0, 8, flat, unbanked },    { 16, 30, 0, 8, flat, unbanked },    { 17, 32, 0, 8, flat, unbanked },    { 16, 34, 0, 8, flat, unbanked },
               { 16, 36, 0, 8, flat, unbanked },    { 16, 38, 0, 8, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),    { 16, 39, 0, 8, flat, unbanked },
               { 16, 41, 0, 8, flat, unbanked },    { 16, 43, 0, 8, flat, unbanked },    { 16, 45, 0, 8, flat, unbanked },    { 16, 46, 0, 8, flat, unbanked },
               { 16, 47, 0, 8, flat, unbanked },    { 16, 48, 0, 8, flat, unbanked },    { 16, 49, 0, 8, flat, unbanked },    { 16, 50, 0, 8, flat, unbanked },
               { 16, 51, 0, 8, flat, unbanked },    { 16, 52, 0, 8, flat, unbanked },    { 16, 53, 0, 8, flat, unbanked },    { 16, 54, 0, 8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),    { 16, 60, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),    { 16, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  5, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  5, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  7, 15, 0, 16, flat, unbanked },
              { 10, 14, 0, 16, flat, unbanked },   { 13, 13, 0, 16, flat, unbanked },   { 16, 12, 0, 16, flat, unbanked },   { 19, 12, 0, 16, flat, unbanked },
              { 21, 11, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 24, 11, 0, 16, flat, unbanked },   { 26, 12, 0, 16, flat, unbanked },
              { 28, 15, 0, 16, flat, unbanked },   { 30, 15, 0, 16, flat, unbanked },   { 32, 14, 0, 16, flat, unbanked },   { 34, 15, 0, 16, flat, unbanked },
              { 36, 15, 0, 16, flat, unbanked },   { 38, 15, 0, 16, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { 39, 15, 0, 16, flat, unbanked },
              { 41, 15, 0, 16, flat, unbanked },   { 43, 15, 0, 16, flat, unbanked },   { 45, 15, 0, 16, flat, unbanked },   { 46, 15, 0, 16, flat, unbanked },
              { 47, 15, 0, 16, flat, unbanked },   { 48, 15, 0, 16, flat, unbanked },   { 49, 15, 0, 16, flat, unbanked },   { 50, 15, 0, 16, flat, unbanked },
              { 51, 15, 0, 16, flat, unbanked },   { 52, 15, 0, 16, flat, unbanked },   { 53, 15, 0, 16, flat, unbanked },   { 54, 15, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { 60, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),   { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15,  24, 0, 24, flat, unbanked },
             { 14,  21, 0, 24, flat, unbanked },  { 13,  18, 0, 24, flat, unbanked },  { 12,  15, 0, 24, flat, unbanked },  { 12,  12, 0, 24, flat, unbanked },
             { 11,  10, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 11,   7, 0, 24, flat, unbanked },  { 12,   5, 0, 24, flat, unbanked },
             { 15,   3, 0, 24, flat, unbanked },  { 15,   1, 0, 24, flat, unbanked },  { 14,  -1, 0, 24, flat, unbanked },  { 15,  -3, 0, 24, flat, unbanked },
             { 15,  -5, 0, 24, flat, unbanked },  { 15,  -7, 0, 24, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),  { 15,  -8, 0, 24, flat, unbanked },
             { 15, -10, 0, 24, flat, unbanked },  { 15, -12, 0, 24, flat, unbanked },  { 15, -14, 0, 24, flat, unbanked },  { 15, -15, 0, 24, flat, unbanked },
             { 15, -16, 0, 24, flat, unbanked },  { 15, -17, 0, 24, flat, unbanked },  { 15, -18, 0, 24, flat, unbanked },  { 15, -19, 0, 24, flat, unbanked },
             { 15, -20, 0, 24, flat, unbanked },  { 15, -21, 0, 24, flat, unbanked },  { 15, -22, 0, 24, flat, unbanked },  { 15, -23, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),  { 15, -29, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),  { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_0, {
                          MINI_GOLF_STATE(Unk0),   {  31, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  26, 16, 0, 0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  26, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  24, 16, 0, 0, flat, unbanked },
              {  21, 16, 0, 0, flat, unbanked },   {  18, 16, 0, 0, flat, unbanked },   {  15, 16, 0, 0, flat, unbanked },   {  12, 16, 0, 0, flat, unbanked },
              {   9, 16, 0, 0, flat, unbanked },   {   6, 16, 0, 0, flat, unbanked },   {   3, 16, 0, 0, flat, unbanked },   {   0, 16, 0, 0, flat, unbanked },
              {  -3, 16, 0, 0, flat, unbanked },   {  -6, 16, 0, 0, flat, unbanked },   {  -9, 16, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { -12, 16, 0, 0, flat, unbanked },   { -15, 16, 0, 0, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { -18, 16, 0, 0, flat, unbanked },
              { -21, 16, 0, 0, flat, unbanked },   { -24, 16, 0, 0, flat, unbanked },   { -25, 15, 0, 0, flat, unbanked },   { -26, 13, 0, 0, flat, unbanked },
              { -27, 11, 0, 0, flat, unbanked },   { -28,  9, 0, 0, flat, unbanked },   { -26,  7, 0, 0, flat, unbanked },   { -24,  5, 1, 0, flat, unbanked },
              { -22,  3, 1, 0, flat, unbanked },   { -20,  2, 1, 0, flat, unbanked },   { -18,  2, 1, 0, flat, unbanked },   { -16,  3, 2, 0, flat, unbanked },
              { -14,  3, 2, 0, flat, unbanked },   { -14,  5, 2, 0, flat, unbanked },   { -13,  7, 2, 0, flat, unbanked },   { -12,  9, 3, 0, flat, unbanked },
              { -12, 11, 3, 0, flat, unbanked },   { -11, 13, 3, 0, flat, unbanked },   { -12, 15, 3, 0, flat, unbanked },   { -12, 17, 3, 0, flat, unbanked },
              { -12, 19, 3, 0, flat, unbanked },   { -13, 21, 3, 0, flat, unbanked },   { -13, 23, 3, 0, flat, unbanked },   { -13, 25, 3, 0, flat, unbanked },
              { -14, 27, 3, 0, flat, unbanked },   { -14, 29, 3, 0, flat, unbanked },   { -14, 30, 3, 0, flat, unbanked },   { -14, 31, 3, 0, flat, unbanked },
              { -15, 32, 3, 0, flat, unbanked },   { -15, 33, 3, 0, flat, unbanked },   { -15, 34, 3, 0, flat, unbanked },   { -15, 35, 3, 0, flat, unbanked },
              { -15, 36, 3, 0, flat, unbanked },   { -15, 38, 0, 0, flat, unbanked },   { -16, 40, 0, 0, flat, unbanked },   { -16, 41, 0, 0, flat, unbanked },
              { -16, 42, 0, 0, flat, unbanked },   { -16, 43, 0, 0, flat, unbanked },   { -16, 44, 0, 0, flat, unbanked },   { -16, 45, 0, 0, flat, unbanked },
              { -17, 46, 0, 0, flat, unbanked },   { -17, 47, 0, 0, flat, unbanked },   { -17, 48, 0, 0, flat, unbanked },   { -17, 49, 0, 0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { -17, 60, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),   { -17, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_1, {
                          MINI_GOLF_STATE(Unk0),   { 16,  0, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 16,  5, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   { 16,  5, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk3),   { 16,  7, 0,  8, flat, unbanked },
              { 16, 10, 0,  8, flat, unbanked },   { 16, 13, 0,  8, flat, unbanked },   { 16, 16, 0,  8, flat, unbanked },   { 16, 19, 0,  8, flat, unbanked },
              { 16, 22, 0,  8, flat, unbanked },   { 16, 25, 0,  8, flat, unbanked },   { 16, 28, 0,  8, flat, unbanked },   { 16, 31, 0,  8, flat, unbanked },
              { 16, 34, 0,  8, flat, unbanked },   { 16, 37, 0,  8, flat, unbanked },   { 16, 40, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { 16, 43, 0,  8, flat, unbanked },   { 16, 46, 0,  8, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { 16, 49, 0,  8, flat, unbanked },
              { 16, 52, 0,  8, flat, unbanked },   { 16, 55, 0,  8, flat, unbanked },   { 15, 56, 0,  8, flat, unbanked },   { 13, 57, 0,  8, flat, unbanked },
              { 11, 58, 0,  8, flat, unbanked },   {  9, 59, 0,  8, flat, unbanked },   {  7, 57, 0,  8, flat, unbanked },   {  5, 55, 1,  8, flat, unbanked },
              {  3, 53, 1,  8, flat, unbanked },   {  2, 51, 1,  8, flat, unbanked },   {  2, 49, 1,  8, flat, unbanked },   {  3, 47, 2,  8, flat, unbanked },
              {  3, 45, 2,  8, flat, unbanked },   {  5, 45, 2,  8, flat, unbanked },   {  7, 44, 2,  8, flat, unbanked },   {  9, 43, 3,  8, flat, unbanked },
              { 11, 43, 3,  8, flat, unbanked },   { 13, 42, 3,  8, flat, unbanked },   { 15, 43, 3,  8, flat, unbanked },   { 17, 43, 3,  8, flat, unbanked },
              { 19, 43, 3,  8, flat, unbanked },   { 21, 44, 3,  8, flat, unbanked },   { 23, 44, 3,  8, flat, unbanked },   { 25, 44, 3,  8, flat, unbanked },
              { 27, 45, 3,  8, flat, unbanked },   { 29, 45, 3,  8, flat, unbanked },   { 30, 45, 3,  8, flat, unbanked },   { 31, 45, 3,  8, flat, unbanked },
              { 32, 46, 3,  8, flat, unbanked },   { 33, 46, 3,  8, flat, unbanked },   { 34, 46, 3,  8, flat, unbanked },   { 35, 46, 3,  8, flat, unbanked },
              { 36, 46, 3,  8, flat, unbanked },   { 38, 46, 0,  8, flat, unbanked },   { 40, 47, 0,  8, flat, unbanked },   { 41, 47, 0,  8, flat, unbanked },
              { 42, 47, 0,  8, flat, unbanked },   { 43, 47, 0,  8, flat, unbanked },   { 44, 47, 0,  8, flat, unbanked },   { 45, 47, 0,  8, flat, unbanked },
              { 46, 48, 0,  8, flat, unbanked },   { 47, 48, 0,  8, flat, unbanked },   { 48, 48, 0,  8, flat, unbanked },   { 49, 48, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { 60, 48, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),   { 60, 48, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_2, {
                          MINI_GOLF_STATE(Unk0),  {  0,  15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  5,  15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  {  5,  15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),  {  7,  15, 0, 16, flat, unbanked },
             { 10,  15, 0, 16, flat, unbanked },  { 13,  15, 0, 16, flat, unbanked },  { 16,  15, 0, 16, flat, unbanked },  { 19,  15, 0, 16, flat, unbanked },
             { 22,  15, 0, 16, flat, unbanked },  { 25,  15, 0, 16, flat, unbanked },  { 28,  15, 0, 16, flat, unbanked },  { 31,  15, 0, 16, flat, unbanked },
             { 34,  15, 0, 16, flat, unbanked },  { 37,  15, 0, 16, flat, unbanked },  { 40,  15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             { 43,  15, 0, 16, flat, unbanked },  { 46,  15, 0, 16, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),  { 49,  15, 0, 16, flat, unbanked },
             { 52,  15, 0, 16, flat, unbanked },  { 55,  15, 0, 16, flat, unbanked },  { 56,  16, 0, 16, flat, unbanked },  { 57,  18, 0, 16, flat, unbanked },
             { 58,  20, 0, 16, flat, unbanked },  { 59,  22, 0, 16, flat, unbanked },  { 57,  24, 0, 16, flat, unbanked },  { 55,  26, 1, 16, flat, unbanked },
             { 53,  28, 1, 16, flat, unbanked },  { 51,  29, 1, 16, flat, unbanked },  { 49,  29, 1, 16, flat, unbanked },  { 47,  28, 2, 16, flat, unbanked },
             { 45,  28, 2, 16, flat, unbanked },  { 45,  26, 2, 16, flat, unbanked },  { 44,  24, 2, 16, flat, unbanked },  { 43,  22, 3, 16, flat, unbanked },
             { 43,  20, 3, 16, flat, unbanked },  { 42,  18, 3, 16, flat, unbanked },  { 43,  16, 3, 16, flat, unbanked },  { 43,  14, 3, 16, flat, unbanked },
             { 43,  12, 3, 16, flat, unbanked },  { 44,  10, 3, 16, flat, unbanked },  { 44,   8, 3, 16, flat, unbanked },  { 44,   6, 3, 16, flat, unbanked },
             { 45,   4, 3, 16, flat, unbanked },  { 45,   2, 3, 16, flat, unbanked },  { 45,   1, 3, 16, flat, unbanked },  { 45,   0, 3, 16, flat, unbanked },
             { 46,  -1, 3, 16, flat, unbanked },  { 46,  -2, 3, 16, flat, unbanked },  { 46,  -3, 3, 16, flat, unbanked },  { 46,  -4, 3, 16, flat, unbanked },
             { 46,  -5, 3, 16, flat, unbanked },  { 46,  -7, 0, 16, flat, unbanked },  { 47,  -9, 0, 16, flat, unbanked },  { 47, -10, 0, 16, flat, unbanked },
             { 47, -11, 0, 16, flat, unbanked },  { 47, -12, 0, 16, flat, unbanked },  { 47, -13, 0, 16, flat, unbanked },  { 47, -14, 0, 16, flat, unbanked },
             { 48, -15, 0, 16, flat, unbanked },  { 48, -16, 0, 16, flat, unbanked },  { 48, -17, 0, 16, flat, unbanked },  { 48, -18, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),  { 48, -29, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),  { 48, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_3, {
                          MINI_GOLF_STATE(Unk0), {  15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown), {  15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3), {  15,  24, 0, 24, flat, unbanked },
            {  15,  21, 0, 24, flat, unbanked }, {  15,  18, 0, 24, flat, unbanked }, {  15,  15, 0, 24, flat, unbanked }, {  15,  12, 0, 24, flat, unbanked },
            {  15,   9, 0, 24, flat, unbanked }, {  15,   6, 0, 24, flat, unbanked }, {  15,   3, 0, 24, flat, unbanked }, {  15,   0, 0, 24, flat, unbanked },
            {  15,  -3, 0, 24, flat, unbanked }, {  15,  -6, 0, 24, flat, unbanked }, {  15,  -9, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
            {  15, -12, 0, 24, flat, unbanked }, {  15, -15, 0, 24, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown), {  15, -18, 0, 24, flat, unbanked },
            {  15, -21, 0, 24, flat, unbanked }, {  15, -24, 0, 24, flat, unbanked }, {  16, -25, 0, 24, flat, unbanked }, {  18, -26, 0, 24, flat, unbanked },
            {  20, -27, 0, 24, flat, unbanked }, {  22, -28, 0, 24, flat, unbanked }, {  24, -26, 0, 24, flat, unbanked }, {  26, -24, 1, 24, flat, unbanked },
            {  28, -22, 1, 24, flat, unbanked }, {  29, -20, 1, 24, flat, unbanked }, {  29, -18, 1, 24, flat, unbanked }, {  28, -16, 2, 24, flat, unbanked },
            {  28, -14, 2, 24, flat, unbanked }, {  26, -14, 2, 24, flat, unbanked }, {  24, -13, 2, 24, flat, unbanked }, {  22, -12, 3, 24, flat, unbanked },
            {  20, -12, 3, 24, flat, unbanked }, {  18, -11, 3, 24, flat, unbanked }, {  16, -12, 3, 24, flat, unbanked }, {  14, -12, 3, 24, flat, unbanked },
            {  12, -12, 3, 24, flat, unbanked }, {  10, -13, 3, 24, flat, unbanked }, {   8, -13, 3, 24, flat, unbanked }, {   6, -13, 3, 24, flat, unbanked },
            {   4, -14, 3, 24, flat, unbanked }, {   2, -14, 3, 24, flat, unbanked }, {   1, -14, 3, 24, flat, unbanked }, {   0, -14, 3, 24, flat, unbanked },
            {  -1, -15, 3, 24, flat, unbanked }, {  -2, -15, 3, 24, flat, unbanked }, {  -3, -15, 3, 24, flat, unbanked }, {  -4, -15, 3, 24, flat, unbanked },
            {  -5, -15, 3, 24, flat, unbanked }, {  -7, -15, 0, 24, flat, unbanked }, {  -9, -16, 0, 24, flat, unbanked }, { -10, -16, 0, 24, flat, unbanked },
            { -11, -16, 0, 24, flat, unbanked }, { -12, -16, 0, 24, flat, unbanked }, { -13, -16, 0, 24, flat, unbanked }, { -14, -16, 0, 24, flat, unbanked },
            { -15, -17, 0, 24, flat, unbanked }, { -16, -17, 0, 24, flat, unbanked }, { -17, -17, 0, 24, flat, unbanked }, { -18, -17, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2), { -29, -17, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3), { -29, -17, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_0, {
                          MINI_GOLF_STATE(Unk0), {  31,  16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  26,  16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown), {  26,  16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk3), {  24,  16, 0,  0, flat, unbanked },
            {  21,  16, 0,  0, flat, unbanked }, {  18,  16, 0,  0, flat, unbanked }, {  15,  16, 0,  0, flat, unbanked }, {  12,  16, 0,  0, flat, unbanked },
            {   9,  16, 0,  0, flat, unbanked }, {   6,  16, 0,  0, flat, unbanked }, {   3,  16, 0,  0, flat, unbanked }, {   0,  16, 0,  0, flat, unbanked },
            {  -3,  16, 0,  0, flat, unbanked }, {  -6,  16, 0,  0, flat, unbanked }, {  -9,  16, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
            { -12,  16, 0,  0, flat, unbanked }, { -15,  16, 0,  0, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown), { -18,  16, 0,  0, flat, unbanked },
            { -21,  16, 0,  0, flat, unbanked }, { -24,  16, 0,  0, flat, unbanked }, { -25,  17, 0,  0, flat, unbanked }, { -26,  19, 0,  0, flat, unbanked },
            { -27,  21, 0,  0, flat, unbanked }, { -28,  23, 0,  0, flat, unbanked }, { -26,  25, 0,  0, flat, unbanked }, { -24,  27, 1,  0, flat, unbanked },
            { -22,  29, 1,  0, flat, unbanked }, { -20,  30, 1,  0, flat, unbanked }, { -18,  30, 1,  0, flat, unbanked }, { -16,  29, 2,  0, flat, unbanked },
            { -14,  29, 2,  0, flat, unbanked }, { -14,  27, 2,  0, flat, unbanked }, { -13,  25, 2,  0, flat, unbanked }, { -12,  23, 3,  0, flat, unbanked },
            { -12,  21, 3,  0, flat, unbanked }, { -11,  19, 3,  0, flat, unbanked }, { -12,  17, 3,  0, flat, unbanked }, { -12,  15, 3,  0, flat, unbanked },
            { -12,  13, 3,  0, flat, unbanked }, { -13,  11, 3,  0, flat, unbanked }, { -13,   9, 3,  0, flat, unbanked }, { -13,   7, 3,  0, flat, unbanked },
            { -14,   5, 3,  0, flat, unbanked }, { -14,   3, 3,  0, flat, unbanked }, { -14,   2, 3,  0, flat, unbanked }, { -14,   1, 3,  0, flat, unbanked },
            { -15,   0, 3,  0, flat, unbanked }, { -15,  -1, 3,  0, flat, unbanked }, { -15,  -2, 3,  0, flat, unbanked }, { -15,  -3, 3,  0, flat, unbanked },
            { -15,  -4, 3,  0, flat, unbanked }, { -15,  -6, 0,  0, flat, unbanked }, { -16,  -8, 0,  0, flat, unbanked }, { -16,  -9, 0,  0, flat, unbanked },
            { -16, -10, 0,  0, flat, unbanked }, { -16, -11, 0,  0, flat, unbanked }, { -16, -12, 0,  0, flat, unbanked }, { -16, -13, 0,  0, flat, unbanked },
            { -17, -14, 0,  0, flat, unbanked }, { -17, -15, 0,  0, flat, unbanked }, { -17, -16, 0,  0, flat, unbanked }, { -17, -17, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2), { -17, -28, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3), { -17, -28, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_1, {
                          MINI_GOLF_STATE(Unk0),   {  16,  0, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  16,  5, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  16,  5, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  16,  7, 0, 8, flat, unbanked },
              {  16, 10, 0, 8, flat, unbanked },   {  16, 13, 0, 8, flat, unbanked },   {  16, 16, 0, 8, flat, unbanked },   {  16, 19, 0, 8, flat, unbanked },
              {  16, 22, 0, 8, flat, unbanked },   {  16, 25, 0, 8, flat, unbanked },   {  16, 28, 0, 8, flat, unbanked },   {  16, 31, 0, 8, flat, unbanked },
              {  16, 34, 0, 8, flat, unbanked },   {  16, 37, 0, 8, flat, unbanked },   {  16, 40, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              {  16, 43, 0, 8, flat, unbanked },   {  16, 46, 0, 8, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   {  16, 49, 0, 8, flat, unbanked },
              {  16, 52, 0, 8, flat, unbanked },   {  16, 55, 0, 8, flat, unbanked },   {  17, 56, 0, 8, flat, unbanked },   {  19, 57, 0, 8, flat, unbanked },
              {  21, 58, 0, 8, flat, unbanked },   {  23, 59, 0, 8, flat, unbanked },   {  25, 57, 0, 8, flat, unbanked },   {  27, 55, 1, 8, flat, unbanked },
              {  29, 53, 1, 8, flat, unbanked },   {  30, 51, 1, 8, flat, unbanked },   {  30, 49, 1, 8, flat, unbanked },   {  29, 47, 2, 8, flat, unbanked },
              {  29, 45, 2, 8, flat, unbanked },   {  27, 45, 2, 8, flat, unbanked },   {  25, 44, 2, 8, flat, unbanked },   {  23, 43, 3, 8, flat, unbanked },
              {  21, 43, 3, 8, flat, unbanked },   {  19, 42, 3, 8, flat, unbanked },   {  17, 43, 3, 8, flat, unbanked },   {  15, 43, 3, 8, flat, unbanked },
              {  13, 43, 3, 8, flat, unbanked },   {  11, 44, 3, 8, flat, unbanked },   {   9, 44, 3, 8, flat, unbanked },   {   7, 44, 3, 8, flat, unbanked },
              {   5, 45, 3, 8, flat, unbanked },   {   3, 45, 3, 8, flat, unbanked },   {   2, 45, 3, 8, flat, unbanked },   {   1, 45, 3, 8, flat, unbanked },
              {   0, 46, 3, 8, flat, unbanked },   {  -1, 46, 3, 8, flat, unbanked },   {  -2, 46, 3, 8, flat, unbanked },   {  -3, 46, 3, 8, flat, unbanked },
              {  -4, 46, 3, 8, flat, unbanked },   {  -6, 46, 0, 8, flat, unbanked },   {  -8, 47, 0, 8, flat, unbanked },   {  -9, 47, 0, 8, flat, unbanked },
              { -10, 47, 0, 8, flat, unbanked },   { -11, 47, 0, 8, flat, unbanked },   { -12, 47, 0, 8, flat, unbanked },   { -13, 47, 0, 8, flat, unbanked },
              { -14, 48, 0, 8, flat, unbanked },   { -15, 48, 0, 8, flat, unbanked },   { -16, 48, 0, 8, flat, unbanked },   { -17, 48, 0, 8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { -28, 48, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),   { -28, 48, 0, 0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  5, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  5, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  7, 15, 0, 16, flat, unbanked },
              { 10, 15, 0, 16, flat, unbanked },   { 13, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 19, 15, 0, 16, flat, unbanked },
              { 22, 15, 0, 16, flat, unbanked },   { 25, 15, 0, 16, flat, unbanked },   { 28, 15, 0, 16, flat, unbanked },   { 31, 15, 0, 16, flat, unbanked },
              { 34, 15, 0, 16, flat, unbanked },   { 37, 15, 0, 16, flat, unbanked },   { 40, 15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { 43, 15, 0, 16, flat, unbanked },   { 46, 15, 0, 16, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { 49, 15, 0, 16, flat, unbanked },
              { 52, 15, 0, 16, flat, unbanked },   { 55, 15, 0, 16, flat, unbanked },   { 56, 14, 0, 16, flat, unbanked },   { 57, 12, 0, 16, flat, unbanked },
              { 58, 10, 0, 16, flat, unbanked },   { 59,  8, 0, 16, flat, unbanked },   { 57,  6, 0, 16, flat, unbanked },   { 55,  4, 1, 16, flat, unbanked },
              { 53,  2, 1, 16, flat, unbanked },   { 51,  1, 1, 16, flat, unbanked },   { 49,  1, 1, 16, flat, unbanked },   { 47,  2, 2, 16, flat, unbanked },
              { 45,  2, 2, 16, flat, unbanked },   { 45,  4, 2, 16, flat, unbanked },   { 44,  6, 2, 16, flat, unbanked },   { 43,  8, 3, 16, flat, unbanked },
              { 43, 10, 3, 16, flat, unbanked },   { 42, 12, 3, 16, flat, unbanked },   { 43, 14, 3, 16, flat, unbanked },   { 43, 16, 3, 16, flat, unbanked },
              { 43, 18, 3, 16, flat, unbanked },   { 44, 20, 3, 16, flat, unbanked },   { 44, 22, 3, 16, flat, unbanked },   { 44, 24, 3, 16, flat, unbanked },
              { 45, 26, 3, 16, flat, unbanked },   { 45, 28, 3, 16, flat, unbanked },   { 45, 29, 3, 16, flat, unbanked },   { 45, 30, 3, 16, flat, unbanked },
              { 46, 31, 3, 16, flat, unbanked },   { 46, 32, 3, 16, flat, unbanked },   { 46, 33, 3, 16, flat, unbanked },   { 46, 34, 3, 16, flat, unbanked },
              { 46, 35, 3, 16, flat, unbanked },   { 46, 37, 0, 16, flat, unbanked },   { 47, 39, 0, 16, flat, unbanked },   { 47, 40, 0, 16, flat, unbanked },
              { 47, 41, 0, 16, flat, unbanked },   { 47, 42, 0, 16, flat, unbanked },   { 47, 43, 0, 16, flat, unbanked },   { 47, 44, 0, 16, flat, unbanked },
              { 48, 45, 0, 16, flat, unbanked },   { 48, 46, 0, 16, flat, unbanked },   { 48, 47, 0, 16, flat, unbanked },   { 48, 48, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { 48, 59, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),   { 48, 59, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15,  24, 0, 24, flat, unbanked },
             { 15,  21, 0, 24, flat, unbanked },  { 15,  18, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 15,  12, 0, 24, flat, unbanked },
             { 15,   9, 0, 24, flat, unbanked },  { 15,   6, 0, 24, flat, unbanked },  { 15,   3, 0, 24, flat, unbanked },  { 15,   0, 0, 24, flat, unbanked },
             { 15,  -3, 0, 24, flat, unbanked },  { 15,  -6, 0, 24, flat, unbanked },  { 15,  -9, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             { 15, -12, 0, 24, flat, unbanked },  { 15, -15, 0, 24, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),  { 15, -18, 0, 24, flat, unbanked },
             { 15, -21, 0, 24, flat, unbanked },  { 15, -24, 0, 24, flat, unbanked },  { 14, -25, 0, 24, flat, unbanked },  { 12, -26, 0, 24, flat, unbanked },
             { 10, -27, 0, 24, flat, unbanked },  {  8, -28, 0, 24, flat, unbanked },  {  6, -26, 0, 24, flat, unbanked },  {  4, -24, 1, 24, flat, unbanked },
             {  2, -22, 1, 24, flat, unbanked },  {  1, -20, 1, 24, flat, unbanked },  {  1, -18, 1, 24, flat, unbanked },  {  2, -16, 2, 24, flat, unbanked },
             {  2, -14, 2, 24, flat, unbanked },  {  4, -14, 2, 24, flat, unbanked },  {  6, -13, 2, 24, flat, unbanked },  {  8, -12, 3, 24, flat, unbanked },
             { 10, -12, 3, 24, flat, unbanked },  { 12, -11, 3, 24, flat, unbanked },  { 14, -12, 3, 24, flat, unbanked },  { 16, -12, 3, 24, flat, unbanked },
             { 18, -12, 3, 24, flat, unbanked },  { 20, -13, 3, 24, flat, unbanked },  { 22, -13, 3, 24, flat, unbanked },  { 24, -13, 3, 24, flat, unbanked },
             { 26, -14, 3, 24, flat, unbanked },  { 28, -14, 3, 24, flat, unbanked },  { 29, -14, 3, 24, flat, unbanked },  { 30, -14, 3, 24, flat, unbanked },
             { 31, -15, 3, 24, flat, unbanked },  { 32, -15, 3, 24, flat, unbanked },  { 33, -15, 3, 24, flat, unbanked },  { 34, -15, 3, 24, flat, unbanked },
             { 35, -15, 3, 24, flat, unbanked },  { 37, -15, 0, 24, flat, unbanked },  { 39, -16, 0, 24, flat, unbanked },  { 40, -16, 0, 24, flat, unbanked },
             { 41, -16, 0, 24, flat, unbanked },  { 42, -16, 0, 24, flat, unbanked },  { 43, -16, 0, 24, flat, unbanked },  { 44, -16, 0, 24, flat, unbanked },
             { 45, -17, 0, 24, flat, unbanked },  { 46, -17, 0, 24, flat, unbanked },  { 47, -17, 0, 24, flat, unbanked },  { 48, -17, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),  { 59, -17, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),  { 59, -17, 0, 16, flat, unbanked },
        })

        static constexpr const VehicleInfoList * BallPathA[] = {
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3,
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // EndStation
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // BeginStation
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // MiddleStation
            &Up25_0,                  &Up25_1,                  &Up25_2,                  &Up25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60
            &FlatToUp25_0,            &FlatToUp25_1,            &FlatToUp25_2,            &FlatToUp25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp25
            &Up25ToFlat_0,            &Up25ToFlat_1,            &Up25ToFlat_2,            &Up25ToFlat_3,
            &Down25_0,                &Down25_1,                &Down25_2,                &Down25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60
            &FlatToDown25_0,          &FlatToDown25_1,          &FlatToDown25_2,          &FlatToDown25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown25
            &Down25ToFlat_0,          &Down25ToFlat_1,          &Down25ToFlat_2,          &Down25ToFlat_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BankedLeftQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BankedRightQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendLeft
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendRight
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftVerticalLoop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightVerticalLoop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankedQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankedQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesDown25
            &LeftQuarterTurn1Tile_0,  &LeftQuarterTurn1Tile_1,  &LeftQuarterTurn1Tile_2,  &LeftQuarterTurn1Tile_3,
            &RightQuarterTurn1Tile_0, &RightQuarterTurn1Tile_1, &RightQuarterTurn1Tile_2, &RightQuarterTurn1Tile_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftTwistDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightTwistDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftTwistUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightTwistUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // TowerBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // TowerSection
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToUp60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToFlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToDown60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToFlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendLeftCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendRightCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixUpSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixUpSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixDownSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixDownSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixUpLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixUpLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixDownLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixDownLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn1TileUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn1TileUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn1TileDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn1TileDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Brakes
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Booster
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Maze
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterBankedHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterBankedHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterBankedHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterBankedHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25LeftBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25RightBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Waterfall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Rapids
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // OnRidePhoto
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25LeftBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25RightBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Watersplash
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp60LongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToFlatLongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Whirlpool
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToFlatLongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown60LongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // CableLiftHill
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // ReverseFreefallSlope
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // ReverseFreefallVertical
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down90ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up90ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BrakeForDrop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthBankToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthBankToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthBankToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthBankToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60ToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60ToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LogFlumeReverser
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SpinningTunnel
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBarrelRollUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBarrelRollUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBarrelRollDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBarrelRollDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToLeftQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToRightQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesDown25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesDown25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // PoweredLift
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftLargeHalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightLargeHalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftLargeHalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightLargeHalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerTwistUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerTwistUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerTwistDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerTwistDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlyerHalfLoopUninvertedUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlyerHalfLoopInvertedDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HeartLineTransferUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HeartLineTransferDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHeartLineRoll
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHeartLineRoll
            &MinigolfHoleA_0,         &MinigolfHoleA_1,         &MinigolfHoleA_2,         &MinigolfHoleA_3,
            &MinigolfHoleB_0,         &MinigolfHoleB_1,         &MinigolfHoleB_2,         &MinigolfHoleB_3,
            &MinigolfHoleC_0,         &MinigolfHoleC_1,         &MinigolfHoleC_2,         &MinigolfHoleC_3,
            &MinigolfHoleD_0,         &MinigolfHoleD_1,         &MinigolfHoleD_2,         &MinigolfHoleD_3,
            &MinigolfHoleE_0,         &MinigolfHoleE_1,         &MinigolfHoleE_2,         &MinigolfHoleE_3,
        };
        // clang-format on
        static_assert(std::size(BallPathA) == kVehicleTrackSubpositionSizeMinigolf);
    } // namespace BallPathA

    namespace PlayerPathB
    {
        // clang-format off
         CREATE_VEHICLE_INFO(MinigolfHoleA_0, {
                          MINI_GOLF_STATE(Unk0),  {  31, 16, 0,  0, flat, unbanked },  {  29, 16, 0,  0, flat, unbanked },  {  27, 16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  27, 16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  27, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  27, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  27, 16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  25, 16, 0,  0, flat, unbanked },  {  23, 16, 0,  0, flat, unbanked },  {  21, 16, 0,  0, flat, unbanked },
             {  19, 16, 0,  0, flat, unbanked },  {  18, 16, 0,  0, flat, unbanked },  {  15, 16, 0,  0, flat, unbanked },  {  13, 16, 0,  0, flat, unbanked },
             {  11, 16, 0,  0, flat, unbanked },  {   9, 16, 0,  0, flat, unbanked },  {   7, 16, 0,  0, flat, unbanked },  {   5, 16, 0,  0, flat, unbanked },
             {   3, 16, 0,  0, flat, unbanked },  {   3, 16, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),  {   3, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
             {   3, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {   3, 16, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             {   1, 16, 0,  0, flat, unbanked },  {  -1, 16, 0,  0, flat, unbanked },  {  -3, 16, 0,  0, flat, unbanked },  {  -5, 16, 0,  0, flat, unbanked },
             {  -7, 16, 0,  0, flat, unbanked },  {  -9, 16, 0,  0, flat, unbanked },  { -11, 16, 0,  0, flat, unbanked },  { -13, 16, 0,  0, flat, unbanked },
             { -15, 16, 0,  0, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),  { -15, 16, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { -17, 16, 0,  0, flat, unbanked },  { -19, 16, 0,  0, flat, unbanked },
             { -21, 16, 0,  0, flat, unbanked },  { -23, 16, 0,  0, flat, unbanked },  { -25, 16, 0,  0, flat, unbanked },  { -27, 16, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),  { -29, 16, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_1, {
                          MINI_GOLF_STATE(Unk0),    { 16,  0, 0, 8, flat, unbanked },    { 16,  2, 0, 8, flat, unbanked },    { 16,  4, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),    { 16,  4, 0, 8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),    { 16,  4, 0, 8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),    { 16,  4, 0, 0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),    { 16,  4, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16,  4, 0, 0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),    { 16,  6, 0, 8, flat, unbanked },    { 16,  8, 0, 8, flat, unbanked },    { 16, 10, 0, 8, flat, unbanked },
               { 16, 12, 0, 8, flat, unbanked },    { 16, 13, 0, 8, flat, unbanked },    { 16, 16, 0, 8, flat, unbanked },    { 16, 18, 0, 8, flat, unbanked },
               { 16, 20, 0, 8, flat, unbanked },    { 16, 22, 0, 8, flat, unbanked },    { 16, 24, 0, 8, flat, unbanked },    { 16, 26, 0, 8, flat, unbanked },
               { 16, 28, 0, 8, flat, unbanked },    { 16, 28, 0, 0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),    { 16, 28, 0, 0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
               { 16, 28, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16, 28, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
               { 16, 30, 0, 8, flat, unbanked },    { 16, 32, 0, 8, flat, unbanked },    { 16, 34, 0, 8, flat, unbanked },    { 16, 36, 0, 8, flat, unbanked },
               { 16, 38, 0, 8, flat, unbanked },    { 16, 40, 0, 8, flat, unbanked },    { 16, 42, 0, 8, flat, unbanked },    { 16, 44, 0, 8, flat, unbanked },
               { 16, 46, 0, 8, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),    { 16, 46, 0, 8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),    { 16, 48, 0, 8, flat, unbanked },    { 16, 50, 0, 8, flat, unbanked },
               { 16, 52, 0, 8, flat, unbanked },    { 16, 54, 0, 8, flat, unbanked },    { 16, 56, 0, 8, flat, unbanked },    { 16, 58, 0, 8, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),    { 16, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },   {  2, 15, 0, 16, flat, unbanked },   {  4, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   {  4, 15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   {  4, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   {  4, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  4, 15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {  6, 15, 0, 16, flat, unbanked },   {  8, 15, 0, 16, flat, unbanked },   { 10, 15, 0, 16, flat, unbanked },
              { 12, 15, 0, 16, flat, unbanked },   { 13, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 18, 15, 0, 16, flat, unbanked },
              { 20, 15, 0, 16, flat, unbanked },   { 22, 15, 0, 16, flat, unbanked },   { 24, 15, 0, 16, flat, unbanked },   { 26, 15, 0, 16, flat, unbanked },
              { 28, 15, 0, 16, flat, unbanked },   { 28, 15, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),   { 28, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
              { 28, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 28, 15, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { 30, 15, 0, 16, flat, unbanked },   { 32, 15, 0, 16, flat, unbanked },   { 34, 15, 0, 16, flat, unbanked },   { 36, 15, 0, 16, flat, unbanked },
              { 38, 15, 0, 16, flat, unbanked },   { 40, 15, 0, 16, flat, unbanked },   { 42, 15, 0, 16, flat, unbanked },   { 44, 15, 0, 16, flat, unbanked },
              { 46, 15, 0, 16, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),   { 46, 15, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),   { 48, 15, 0, 16, flat, unbanked },   { 50, 15, 0, 16, flat, unbanked },
              { 52, 15, 0, 16, flat, unbanked },   { 54, 15, 0, 16, flat, unbanked },   { 56, 15, 0, 16, flat, unbanked },   { 58, 15, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),   { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },  { 15,  29, 0, 24, flat, unbanked },  { 15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 15,  25, 0, 24, flat, unbanked },  { 15,  23, 0, 24, flat, unbanked },  { 15,  21, 0, 24, flat, unbanked },
             { 15,  19, 0, 24, flat, unbanked },  { 15,  18, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 15,  13, 0, 24, flat, unbanked },
             { 15,  11, 0, 24, flat, unbanked },  { 15,   9, 0, 24, flat, unbanked },  { 15,   7, 0, 24, flat, unbanked },  { 15,   5, 0, 24, flat, unbanked },
             { 15,   3, 0, 24, flat, unbanked },  { 15,   3, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),  { 15,   3, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
             { 15,   3, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,   3, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             { 15,   1, 0, 24, flat, unbanked },  { 15,  -1, 0, 24, flat, unbanked },  { 15,  -3, 0, 24, flat, unbanked },  { 15,  -5, 0, 24, flat, unbanked },
             { 15,  -7, 0, 24, flat, unbanked },  { 15,  -9, 0, 24, flat, unbanked },  { 15, -11, 0, 24, flat, unbanked },  { 15, -13, 0, 24, flat, unbanked },
             { 15, -15, 0, 24, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),  { 15, -15, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { 15, -17, 0, 24, flat, unbanked },  { 15, -19, 0, 24, flat, unbanked },
             { 15, -21, 0, 24, flat, unbanked },  { 15, -23, 0, 24, flat, unbanked },  { 15, -25, 0, 24, flat, unbanked },  { 15, -27, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),  { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_0, {
                          MINI_GOLF_STATE(Unk0),  {  31, 16, 0,  0, flat, unbanked },  {  29, 16, 0,  0, flat, unbanked },  {  27, 16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  27, 16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  27, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  27, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  27, 16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  25, 16, 0,  0, flat, unbanked },  {  23, 16, 0,  0, flat, unbanked },  {  21, 16, 0,  0, flat, unbanked },
             {  19, 16, 0,  0, flat, unbanked },  {  17, 16, 0,  0, flat, unbanked },  {  15, 16, 0,  0, flat, unbanked },  {  13, 16, 0,  0, flat, unbanked },
             {  11, 16, 0,  0, flat, unbanked },  {   9, 16, 0,  0, flat, unbanked },  {   7, 16, 0,  0, flat, unbanked },  {   5, 16, 0,  0, flat, unbanked },
             {   3, 16, 0,  0, flat, unbanked },  {   1, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),  {   2, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
             {   2, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {   2, 16, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             {  -1, 16, 0,  0, flat, unbanked },  {  -3, 16, 0,  0, flat, unbanked },  {  -5, 16, 0,  0, flat, unbanked },  {  -5, 18, 0,  8, flat, unbanked },
             {  -5, 20, 0,  8, flat, unbanked },  {  -5, 22, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),  {  -4, 22, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
             {  -4, 22, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  -4, 22, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             {  -5, 20, 0, 24, flat, unbanked },  {  -5, 18, 0, 24, flat, unbanked },  {  -5, 16, 0, 24, flat, unbanked },  {  -7, 16, 0,  0, flat, unbanked },
             {  -9, 16, 0,  0, flat, unbanked },  { -11, 16, 0,  0, flat, unbanked },  { -13, 16, 0,  0, flat, unbanked },  { -15, 16, 0,  0, flat, unbanked },
             { -17, 16, 0,  0, flat, unbanked },  { -19, 16, 0,  0, flat, unbanked },  { -21, 16, 0,  0, flat, unbanked },  { -23, 16, 0,  0, flat, unbanked },
             { -25, 16, 0,  0, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),  { -27, 16, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { -27, 16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk3),
             { -29, 16, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_1, {
                          MINI_GOLF_STATE(Unk0),   { 16,  0, 0,  8, flat, unbanked },   { 16,  2, 0,  8, flat, unbanked },   { 16,  4, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 16,  4, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   { 16,  6, 0,  8, flat, unbanked },   { 16,  8, 0,  8, flat, unbanked },   { 16, 10, 0,  8, flat, unbanked },
              { 16, 12, 0,  8, flat, unbanked },   { 16, 14, 0,  8, flat, unbanked },   { 16, 16, 0,  8, flat, unbanked },   { 16, 18, 0,  8, flat, unbanked },
              { 16, 20, 0,  8, flat, unbanked },   { 16, 22, 0,  8, flat, unbanked },   { 16, 24, 0,  8, flat, unbanked },   { 16, 26, 0,  8, flat, unbanked },
              { 16, 28, 0,  8, flat, unbanked },   { 16, 30, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),   { 16, 29, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
              { 16, 29, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 16, 29, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { 16, 32, 0,  8, flat, unbanked },   { 16, 34, 0,  8, flat, unbanked },   { 16, 36, 0,  8, flat, unbanked },   { 18, 36, 0, 16, flat, unbanked },
              { 20, 36, 0, 16, flat, unbanked },   { 22, 36, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),   { 22, 35, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
              { 22, 35, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 22, 35, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { 20, 36, 0,  0, flat, unbanked },   { 18, 36, 0,  0, flat, unbanked },   { 16, 36, 0,  0, flat, unbanked },   { 16, 38, 0,  8, flat, unbanked },
              { 16, 40, 0,  8, flat, unbanked },   { 16, 42, 0,  8, flat, unbanked },   { 16, 44, 0,  8, flat, unbanked },   { 16, 46, 0,  8, flat, unbanked },
              { 16, 48, 0,  8, flat, unbanked },   { 16, 50, 0,  8, flat, unbanked },   { 16, 52, 0,  8, flat, unbanked },   { 16, 54, 0,  8, flat, unbanked },
              { 16, 56, 0,  8, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),   { 16, 58, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),   { 16, 58, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk3),
              { 16, 60, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },   {  2, 15, 0, 16, flat, unbanked },   {  4, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   {  4, 15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   {  4, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   {  4, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  4, 15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {  6, 15, 0, 16, flat, unbanked },   {  8, 15, 0, 16, flat, unbanked },   { 10, 15, 0, 16, flat, unbanked },
              { 12, 15, 0, 16, flat, unbanked },   { 14, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 18, 15, 0, 16, flat, unbanked },
              { 20, 15, 0, 16, flat, unbanked },   { 22, 15, 0, 16, flat, unbanked },   { 24, 15, 0, 16, flat, unbanked },   { 26, 15, 0, 16, flat, unbanked },
              { 28, 15, 0, 16, flat, unbanked },   { 30, 15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),   { 29, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
              { 29, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 29, 15, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { 32, 15, 0, 16, flat, unbanked },   { 34, 15, 0, 16, flat, unbanked },   { 36, 15, 0, 16, flat, unbanked },   { 36, 13, 0, 24, flat, unbanked },
              { 36, 11, 0, 24, flat, unbanked },   { 36,  9, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),   { 35,  9, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
              { 35,  9, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 35,  9, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { 36, 11, 0,  8, flat, unbanked },   { 36, 13, 0,  8, flat, unbanked },   { 36, 15, 0,  8, flat, unbanked },   { 38, 15, 0, 16, flat, unbanked },
              { 40, 15, 0, 16, flat, unbanked },   { 42, 15, 0, 16, flat, unbanked },   { 44, 15, 0, 16, flat, unbanked },   { 46, 15, 0, 16, flat, unbanked },
              { 48, 15, 0, 16, flat, unbanked },   { 50, 15, 0, 16, flat, unbanked },   { 52, 15, 0, 16, flat, unbanked },   { 54, 15, 0, 16, flat, unbanked },
              { 56, 15, 0, 16, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),   { 58, 15, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),   { 58, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),
              { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },  { 15,  29, 0, 24, flat, unbanked },  { 15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 15,  25, 0, 24, flat, unbanked },  { 15,  23, 0, 24, flat, unbanked },  { 15,  21, 0, 24, flat, unbanked },
             { 15,  19, 0, 24, flat, unbanked },  { 15,  17, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 15,  13, 0, 24, flat, unbanked },
             { 15,  11, 0, 24, flat, unbanked },  { 15,   9, 0, 24, flat, unbanked },  { 15,   7, 0, 24, flat, unbanked },  { 15,   5, 0, 24, flat, unbanked },
             { 15,   3, 0, 24, flat, unbanked },  { 15,   1, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),  { 15,   2, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
             { 15,   2, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,   2, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             { 15,  -1, 0, 24, flat, unbanked },  { 15,  -3, 0, 24, flat, unbanked },  { 15,  -5, 0, 24, flat, unbanked },  { 13,  -5, 0,  0, flat, unbanked },
             { 11,  -5, 0,  0, flat, unbanked },  {  9,  -5, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),  {  9,  -4, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
             {  9,  -4, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  9,  -4, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             { 11,  -5, 0, 16, flat, unbanked },  { 13,  -5, 0, 16, flat, unbanked },  { 15,  -5, 0, 16, flat, unbanked },  { 15,  -7, 0, 24, flat, unbanked },
             { 15,  -9, 0, 24, flat, unbanked },  { 15, -11, 0, 24, flat, unbanked },  { 15, -13, 0, 24, flat, unbanked },  { 15, -15, 0, 24, flat, unbanked },
             { 15, -17, 0, 24, flat, unbanked },  { 15, -19, 0, 24, flat, unbanked },  { 15, -21, 0, 24, flat, unbanked },  { 15, -23, 0, 24, flat, unbanked },
             { 15, -25, 0, 24, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),  { 15, -27, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { 15, -27, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),
             { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_0, {
                          MINI_GOLF_STATE(Unk0),  {  31, 16, 0,  0, flat, unbanked },  {  29, 16, 0,  0, flat, unbanked },  {  27, 16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  27, 16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  27, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  27, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  27, 16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  25, 16, 0,  0, flat, unbanked },  {  23, 16, 0,  0, flat, unbanked },  {  21, 16, 0,  0, flat, unbanked },
             {  19, 16, 0,  0, flat, unbanked },  {  17, 16, 0,  0, flat, unbanked },  {  15, 16, 0,  0, flat, unbanked },  {  13, 16, 0,  0, flat, unbanked },
             {  11, 16, 0,  0, flat, unbanked },  {   9, 16, 5,  0, flat, unbanked },  {   7, 16, 5,  0, flat, unbanked },  {   5, 16, 5,  0, flat, unbanked },
             {   3, 16, 5,  0, flat, unbanked },  {   1, 16, 5,  0, flat, unbanked },  {  -1, 16, 5,  0, flat, unbanked },  {  -3, 16, 5,  0, flat, unbanked },
             {  -5, 16, 5,  0, flat, unbanked },  {  -7, 16, 5,  0, flat, unbanked },  {  -9, 16, 0,  0, flat, unbanked },  { -11, 16, 0,  0, flat, unbanked },
             { -13, 16, 0,  0, flat, unbanked },  { -15, 16, 0,  0, flat, unbanked },  { -17, 16, 0,  0, flat, unbanked },  { -19, 16, 0,  0, flat, unbanked },
             { -21, 16, 0,  0, flat, unbanked },  { -23, 16, 0,  0, flat, unbanked },  { -23, 14, 0, 24, flat, unbanked },  { -23, 12, 0, 24, flat, unbanked },
             { -23, 10, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             { -23, 10, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { -23, 10, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  { -23, 10, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { -23, 10, 0,  8, flat, unbanked },
             { -23, 12, 0,  8, flat, unbanked },  { -23, 14, 0,  8, flat, unbanked },  { -23, 16, 0,  8, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),  { -23, 16, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
             { -23, 16, 0,  0, flat, unbanked },  { -25, 16, 0,  0, flat, unbanked },  { -27, 16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk3),
             { -29, 16, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_1, {
                          MINI_GOLF_STATE(Unk0),   { 16,  0, 0,  8, flat, unbanked },   { 16,  2, 0,  8, flat, unbanked },   { 16,  4, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 16,  4, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   { 16,  6, 0,  8, flat, unbanked },   { 16,  8, 0,  8, flat, unbanked },   { 16, 10, 0,  8, flat, unbanked },
              { 16, 12, 0,  8, flat, unbanked },   { 16, 14, 0,  8, flat, unbanked },   { 16, 16, 0,  8, flat, unbanked },   { 16, 18, 0,  8, flat, unbanked },
              { 16, 20, 0,  8, flat, unbanked },   { 16, 22, 5,  8, flat, unbanked },   { 16, 24, 5,  8, flat, unbanked },   { 16, 26, 5,  8, flat, unbanked },
              { 16, 28, 5,  8, flat, unbanked },   { 16, 30, 5,  8, flat, unbanked },   { 16, 32, 5,  8, flat, unbanked },   { 16, 34, 5,  8, flat, unbanked },
              { 16, 36, 5,  8, flat, unbanked },   { 16, 38, 5,  8, flat, unbanked },   { 16, 40, 0,  8, flat, unbanked },   { 16, 42, 0,  8, flat, unbanked },
              { 16, 44, 0,  8, flat, unbanked },   { 16, 46, 0,  8, flat, unbanked },   { 16, 48, 0,  8, flat, unbanked },   { 16, 50, 0,  8, flat, unbanked },
              { 16, 52, 0,  8, flat, unbanked },   { 16, 54, 0,  8, flat, unbanked },   { 14, 54, 0,  0, flat, unbanked },   { 12, 54, 0,  0, flat, unbanked },
              { 10, 54, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
              { 10, 54, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 10, 54, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),   { 10, 54, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 10, 54, 0, 16, flat, unbanked },
              { 12, 54, 0, 16, flat, unbanked },   { 14, 54, 0, 16, flat, unbanked },   { 16, 54, 0, 16, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),   { 16, 54, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
              { 16, 54, 0,  8, flat, unbanked },   { 16, 56, 0,  8, flat, unbanked },   { 16, 58, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk3),
              { 16, 60, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },   {  2, 15, 0, 16, flat, unbanked },   {  4, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   {  4, 15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   {  4, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   {  4, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  4, 15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {  6, 15, 0, 16, flat, unbanked },   {  8, 15, 0, 16, flat, unbanked },   { 10, 15, 0, 16, flat, unbanked },
              { 12, 15, 0, 16, flat, unbanked },   { 14, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 18, 15, 0, 16, flat, unbanked },
              { 20, 15, 0, 16, flat, unbanked },   { 22, 15, 5, 16, flat, unbanked },   { 24, 15, 5, 16, flat, unbanked },   { 26, 15, 5, 16, flat, unbanked },
              { 28, 15, 5, 16, flat, unbanked },   { 30, 15, 5, 16, flat, unbanked },   { 32, 15, 5, 16, flat, unbanked },   { 34, 15, 5, 16, flat, unbanked },
              { 36, 15, 5, 16, flat, unbanked },   { 38, 15, 5, 16, flat, unbanked },   { 40, 15, 0, 16, flat, unbanked },   { 42, 15, 0, 16, flat, unbanked },
              { 44, 15, 0, 16, flat, unbanked },   { 46, 15, 0, 16, flat, unbanked },   { 48, 15, 0, 16, flat, unbanked },   { 50, 15, 0, 16, flat, unbanked },
              { 52, 15, 0, 16, flat, unbanked },   { 54, 15, 0, 16, flat, unbanked },   { 54, 17, 0,  8, flat, unbanked },   { 54, 19, 0,  8, flat, unbanked },
              { 54, 21, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
              { 54, 21, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 54, 21, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),   { 54, 21, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 54, 21, 0, 24, flat, unbanked },
              { 54, 19, 0, 24, flat, unbanked },   { 54, 17, 0, 24, flat, unbanked },   { 54, 15, 0, 24, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),   { 54, 15, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
              { 54, 15, 0, 16, flat, unbanked },   { 56, 15, 0, 16, flat, unbanked },   { 58, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),
              { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },  { 15,  29, 0, 24, flat, unbanked },  { 15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 15,  25, 0, 24, flat, unbanked },  { 15,  23, 0, 24, flat, unbanked },  { 15,  21, 0, 24, flat, unbanked },
             { 15,  19, 0, 24, flat, unbanked },  { 15,  17, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 15,  13, 0, 24, flat, unbanked },
             { 15,  11, 0, 24, flat, unbanked },  { 15,   9, 5, 24, flat, unbanked },  { 15,   7, 5, 24, flat, unbanked },  { 15,   5, 5, 24, flat, unbanked },
             { 15,   3, 5, 24, flat, unbanked },  { 15,   1, 5, 24, flat, unbanked },  { 15,  -1, 5, 24, flat, unbanked },  { 15,  -3, 5, 24, flat, unbanked },
             { 15,  -5, 5, 24, flat, unbanked },  { 15,  -7, 5, 24, flat, unbanked },  { 15,  -9, 0, 24, flat, unbanked },  { 15, -11, 0, 24, flat, unbanked },
             { 15, -13, 0, 24, flat, unbanked },  { 15, -15, 0, 24, flat, unbanked },  { 15, -17, 0, 24, flat, unbanked },  { 15, -19, 0, 24, flat, unbanked },
             { 15, -21, 0, 24, flat, unbanked },  { 15, -23, 0, 24, flat, unbanked },  { 17, -23, 0, 16, flat, unbanked },  { 19, -23, 0, 16, flat, unbanked },
             { 21, -23, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             { 21, -23, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { 21, -23, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  { 21, -23, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 21, -23, 0,  0, flat, unbanked },
             { 19, -23, 0,  0, flat, unbanked },  { 17, -23, 0,  0, flat, unbanked },  { 15, -23, 0,  0, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),  { 15, -23, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
             { 15, -23, 0, 24, flat, unbanked },  { 15, -25, 0, 24, flat, unbanked },  { 15, -27, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),
             { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_0, {
                          MINI_GOLF_STATE(Unk0),  {  31, 16, 0,  0, flat, unbanked },  {  29, 16, 0,  0, flat, unbanked },  {  27, 16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  27, 16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  27, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  27, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  27, 16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  25, 16, 0,  0, flat, unbanked },  {  23, 16, 0,  0, flat, unbanked },  {  21, 16, 0,  0, flat, unbanked },
             {  19, 16, 0,  0, flat, unbanked },  {  17, 16, 0,  0, flat, unbanked },  {  15, 16, 0,  0, flat, unbanked },  {  15, 18, 0,  8, flat, unbanked },
             {  15, 20, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             {  15, 20, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  {  15, 20, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  {  15, 20, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  {  13, 20, 0,  0, flat, unbanked },
             {  11, 20, 0,  0, flat, unbanked },  {   9, 20, 0,  0, flat, unbanked },  {   7, 20, 0,  0, flat, unbanked },  {   5, 20, 0,  0, flat, unbanked },
             {   3, 20, 0,  0, flat, unbanked },  {   1, 20, 0,  0, flat, unbanked },  {  -1, 20, 0,  0, flat, unbanked },  {  -3, 20, 0,  0, flat, unbanked },
             {  -3, 22, 0,  8, flat, unbanked },  {  -3, 24, 0,  8, flat, unbanked },  {  -3, 26, 0,  8, flat, unbanked },  {  -3, 28, 0,  8, flat, unbanked },
             {  -3, 30, 0,  8, flat, unbanked },  {  -3, 32, 0,  8, flat, unbanked },  {  -3, 34, 0,  8, flat, unbanked },  {  -3, 36, 0,  8, flat, unbanked },
             {  -3, 38, 0,  8, flat, unbanked },  {  -3, 40, 0,  8, flat, unbanked },  {  -3, 42, 0,  8, flat, unbanked },  {  -5, 42, 0,  0, flat, unbanked },
             {  -7, 42, 0,  0, flat, unbanked },  {  -9, 42, 0,  0, flat, unbanked },  { -11, 42, 0,  0, flat, unbanked },  { -13, 42, 0,  0, flat, unbanked },
             { -15, 42, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             { -16, 41, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { -16, 41, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  { -16, 41, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { -17, 43, 0,  8, flat, unbanked },
             { -17, 45, 0,  8, flat, unbanked },  { -17, 47, 0,  8, flat, unbanked },  { -17, 49, 0,  8, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),  { -17, 49, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
             { -17, 48, 0,  8, flat, unbanked },  { -17, 50, 0,  8, flat, unbanked },  { -17, 52, 0,  8, flat, unbanked },  { -17, 54, 0,  8, flat, unbanked },
             { -17, 56, 0,  8, flat, unbanked },  { -17, 58, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { -17, 60, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_1, {
                          MINI_GOLF_STATE(Unk0),   { 16,  0, 0,  8, flat, unbanked },   { 16,  2, 0,  8, flat, unbanked },   { 16,  4, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 16,  4, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   { 16,  6, 0,  8, flat, unbanked },   { 16,  8, 0,  8, flat, unbanked },   { 16, 10, 0,  8, flat, unbanked },
              { 16, 12, 0,  8, flat, unbanked },   { 16, 14, 0,  8, flat, unbanked },   { 16, 16, 0,  8, flat, unbanked },   { 18, 16, 0, 16, flat, unbanked },
              { 20, 16, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
              { 20, 16, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 20, 16, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),   { 20, 16, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 20, 18, 0,  8, flat, unbanked },
              { 20, 20, 0,  8, flat, unbanked },   { 20, 22, 0,  8, flat, unbanked },   { 20, 24, 0,  8, flat, unbanked },   { 20, 26, 0,  8, flat, unbanked },
              { 20, 28, 0,  8, flat, unbanked },   { 20, 30, 0,  8, flat, unbanked },   { 20, 32, 0,  8, flat, unbanked },   { 20, 34, 0,  8, flat, unbanked },
              { 22, 34, 0, 16, flat, unbanked },   { 24, 34, 0, 16, flat, unbanked },   { 26, 34, 0, 16, flat, unbanked },   { 28, 34, 0, 16, flat, unbanked },
              { 30, 34, 0, 16, flat, unbanked },   { 32, 34, 0, 16, flat, unbanked },   { 34, 34, 0, 16, flat, unbanked },   { 36, 34, 0, 16, flat, unbanked },
              { 38, 34, 0, 16, flat, unbanked },   { 40, 34, 0, 16, flat, unbanked },   { 42, 34, 0, 16, flat, unbanked },   { 42, 36, 0,  8, flat, unbanked },
              { 42, 38, 0,  8, flat, unbanked },   { 42, 40, 0,  8, flat, unbanked },   { 42, 42, 0,  8, flat, unbanked },   { 42, 44, 0,  8, flat, unbanked },
              { 42, 46, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
              { 41, 47, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 41, 47, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),   { 41, 47, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 43, 48, 0, 16, flat, unbanked },
              { 45, 48, 0, 16, flat, unbanked },   { 47, 48, 0, 16, flat, unbanked },   { 49, 48, 0, 16, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),   { 49, 48, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
              { 48, 48, 0, 16, flat, unbanked },   { 50, 48, 0, 16, flat, unbanked },   { 52, 48, 0, 16, flat, unbanked },   { 54, 48, 0, 16, flat, unbanked },
              { 56, 48, 0, 16, flat, unbanked },   { 58, 48, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   { 60, 48, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_2, {
                          MINI_GOLF_STATE(Unk0),  {  0,  15, 0, 16, flat, unbanked },  {  2,  15, 0, 16, flat, unbanked },  {  4,  15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  4,  15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  4,  15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  4,  15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  4,  15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  4,  15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  6,  15, 0, 16, flat, unbanked },  {  8,  15, 0, 16, flat, unbanked },  { 10,  15, 0, 16, flat, unbanked },
             { 12,  15, 0, 16, flat, unbanked },  { 14,  15, 0, 16, flat, unbanked },  { 16,  15, 0, 16, flat, unbanked },  { 16,  13, 0, 24, flat, unbanked },
             { 16,  11, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             { 16,  11, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { 16,  11, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  { 16,  11, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 18,  11, 0, 16, flat, unbanked },
             { 20,  11, 0, 16, flat, unbanked },  { 22,  11, 0, 16, flat, unbanked },  { 24,  11, 0, 16, flat, unbanked },  { 26,  11, 0, 16, flat, unbanked },
             { 28,  11, 0, 16, flat, unbanked },  { 30,  11, 0, 16, flat, unbanked },  { 32,  11, 0, 16, flat, unbanked },  { 34,  11, 0, 16, flat, unbanked },
             { 34,   9, 0, 24, flat, unbanked },  { 34,   7, 0, 24, flat, unbanked },  { 34,   5, 0, 24, flat, unbanked },  { 34,   3, 0, 24, flat, unbanked },
             { 34,   1, 0, 24, flat, unbanked },  { 34,  -1, 0, 24, flat, unbanked },  { 34,  -3, 0, 24, flat, unbanked },  { 34,  -5, 0, 24, flat, unbanked },
             { 34,  -7, 0, 24, flat, unbanked },  { 34,  -9, 0, 24, flat, unbanked },  { 34, -11, 0, 24, flat, unbanked },  { 36, -11, 0, 16, flat, unbanked },
             { 38, -11, 0, 16, flat, unbanked },  { 40, -11, 0, 16, flat, unbanked },  { 42, -11, 0, 16, flat, unbanked },  { 44, -11, 0, 16, flat, unbanked },
             { 46, -11, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             { 47, -10, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { 47, -10, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  { 47, -10, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 48, -12, 0, 24, flat, unbanked },
             { 48, -14, 0, 24, flat, unbanked },  { 48, -16, 0, 24, flat, unbanked },  { 48, -18, 0, 24, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),  { 48, -18, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
             { 48, -17, 0, 24, flat, unbanked },  { 48, -19, 0, 24, flat, unbanked },  { 48, -21, 0, 24, flat, unbanked },  { 48, -23, 0, 24, flat, unbanked },
             { 48, -25, 0, 24, flat, unbanked },  { 48, -27, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 48, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_3, {
                          MINI_GOLF_STATE(Unk0), {  15,  31, 0, 24, flat, unbanked }, {  15,  29, 0, 24, flat, unbanked }, {  15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2), {  15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5), {  15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3), {  15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5), {  15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk), {  15,  25, 0, 24, flat, unbanked }, {  15,  23, 0, 24, flat, unbanked }, {  15,  21, 0, 24, flat, unbanked },
            {  15,  19, 0, 24, flat, unbanked }, {  15,  17, 0, 24, flat, unbanked }, {  15,  15, 0, 24, flat, unbanked }, {  13,  15, 0,  0, flat, unbanked },
            {  11,  15, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
            {  11,  15, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5), {  11,  15, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2), {  11,  15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk), {  11,  13, 0, 24, flat, unbanked },
            {  11,  11, 0, 24, flat, unbanked }, {  11,   9, 0, 24, flat, unbanked }, {  11,   7, 0, 24, flat, unbanked }, {  11,   5, 0, 24, flat, unbanked },
            {  11,   3, 0, 24, flat, unbanked }, {  11,   1, 0, 24, flat, unbanked }, {  11,  -1, 0, 24, flat, unbanked }, {  11,  -3, 0, 24, flat, unbanked },
            {   9,  -3, 0,  0, flat, unbanked }, {   7,  -3, 0,  0, flat, unbanked }, {   5,  -3, 0,  0, flat, unbanked }, {   3,  -3, 0,  0, flat, unbanked },
            {   1,  -3, 0,  0, flat, unbanked }, {  -1,  -3, 0,  0, flat, unbanked }, {  -3,  -3, 0,  0, flat, unbanked }, {  -5,  -3, 0,  0, flat, unbanked },
            {  -7,  -3, 0,  0, flat, unbanked }, {  -9,  -3, 0,  0, flat, unbanked }, { -11,  -3, 0,  0, flat, unbanked }, { -11,  -5, 0, 24, flat, unbanked },
            { -11,  -7, 0, 24, flat, unbanked }, { -11,  -9, 0, 24, flat, unbanked }, { -11, -11, 0, 24, flat, unbanked }, { -11, -13, 0, 24, flat, unbanked },
            { -11, -15, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
            { -10, -16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5), { -10, -16, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2), { -10, -16, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk), { -12, -17, 0,  0, flat, unbanked },
            { -14, -17, 0,  0, flat, unbanked }, { -16, -17, 0,  0, flat, unbanked }, { -18, -17, 0,  0, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5), { -18, -17, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
            { -17, -17, 0,  0, flat, unbanked }, { -19, -17, 0,  0, flat, unbanked }, { -21, -17, 0,  0, flat, unbanked }, { -23, -17, 0,  0, flat, unbanked },
            { -25, -17, 0,  0, flat, unbanked }, { -27, -17, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk3), { -29, -17, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_0, {
                          MINI_GOLF_STATE(Unk0), {  31,  16, 0,  0, flat, unbanked }, {  29,  16, 0,  0, flat, unbanked }, {  27,  16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2), {  27,  16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5), {  27,  16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3), {  27,  16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5), {  27,  16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  27,  16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk), {  25,  16, 0,  0, flat, unbanked }, {  23,  16, 0,  0, flat, unbanked }, {  21,  16, 0,  0, flat, unbanked },
            {  19,  16, 0,  0, flat, unbanked }, {  17,  16, 0,  0, flat, unbanked }, {  15,  16, 0,  0, flat, unbanked }, {  15,  14, 0,  8, flat, unbanked },
            {  15,  12, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
            {  15,  12, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5), {  15,  12, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2), {  15,  12, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk), {  13,  12, 0,  0, flat, unbanked },
            {  11,  12, 0,  0, flat, unbanked }, {   9,  12, 0,  0, flat, unbanked }, {   7,  12, 0,  0, flat, unbanked }, {   5,  12, 0,  0, flat, unbanked },
            {   3,  12, 0,  0, flat, unbanked }, {   1,  12, 0,  0, flat, unbanked }, {  -1,  12, 0,  0, flat, unbanked }, {  -3,  12, 0,  0, flat, unbanked },
            {  -3,  10, 0, 24, flat, unbanked }, {  -3,   8, 0, 24, flat, unbanked }, {  -3,   6, 0, 24, flat, unbanked }, {  -3,   4, 0, 24, flat, unbanked },
            {  -3,   2, 0, 24, flat, unbanked }, {  -3,   0, 0, 24, flat, unbanked }, {  -3,  -2, 0, 24, flat, unbanked }, {  -3,  -4, 0, 24, flat, unbanked },
            {  -3,  -6, 0, 24, flat, unbanked }, {  -3,  -8, 0, 24, flat, unbanked }, {  -3, -10, 0, 24, flat, unbanked }, {  -5, -10, 0,  0, flat, unbanked },
            {  -7, -10, 0,  0, flat, unbanked }, {  -9, -10, 0,  0, flat, unbanked }, { -11, -10, 0,  0, flat, unbanked }, { -13, -10, 0,  0, flat, unbanked },
            { -15, -10, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
            { -16,  -9, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5), { -16,  -9, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2), { -16,  -9, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk), { -17, -11, 0, 24, flat, unbanked },
            { -17, -13, 0, 24, flat, unbanked }, { -17, -15, 0, 24, flat, unbanked }, { -17, -17, 0, 24, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5), { -17, -17, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
            { -17, -16, 0, 24, flat, unbanked }, { -17, -18, 0, 24, flat, unbanked }, { -17, -20, 0, 24, flat, unbanked }, { -17, -22, 0, 24, flat, unbanked },
            { -17, -24, 0, 24, flat, unbanked }, { -17, -26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3), { -17, -28, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_1, {
                          MINI_GOLF_STATE(Unk0),  {  16,  0, 0,  8, flat, unbanked },  {  16,  2, 0,  8, flat, unbanked },  {  16,  4, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  16,  4, 0,  8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  16,  4, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  16,  4, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  16,  4, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  16,  4, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  16,  6, 0,  8, flat, unbanked },  {  16,  8, 0,  8, flat, unbanked },  {  16, 10, 0,  8, flat, unbanked },
             {  16, 12, 0,  8, flat, unbanked },  {  16, 14, 0,  8, flat, unbanked },  {  16, 16, 0,  8, flat, unbanked },  {  14, 16, 0, 16, flat, unbanked },
             {  12, 16, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             {  12, 16, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  {  12, 16, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  {  12, 16, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  {  12, 18, 0,  8, flat, unbanked },
             {  12, 20, 0,  8, flat, unbanked },  {  12, 22, 0,  8, flat, unbanked },  {  12, 24, 0,  8, flat, unbanked },  {  12, 26, 0,  8, flat, unbanked },
             {  12, 28, 0,  8, flat, unbanked },  {  12, 30, 0,  8, flat, unbanked },  {  12, 32, 0,  8, flat, unbanked },  {  12, 34, 0,  8, flat, unbanked },
             {  10, 34, 0,  0, flat, unbanked },  {   8, 34, 0,  0, flat, unbanked },  {   6, 34, 0,  0, flat, unbanked },  {   4, 34, 0,  0, flat, unbanked },
             {   2, 34, 0,  0, flat, unbanked },  {   0, 34, 0,  0, flat, unbanked },  {  -2, 34, 0,  0, flat, unbanked },  {  -4, 34, 0,  0, flat, unbanked },
             {  -6, 34, 0,  0, flat, unbanked },  {  -8, 34, 0,  0, flat, unbanked },  { -10, 34, 0,  0, flat, unbanked },  { -10, 36, 0,  8, flat, unbanked },
             { -10, 38, 0,  8, flat, unbanked },  { -10, 40, 0,  8, flat, unbanked },  { -10, 42, 0,  8, flat, unbanked },  { -10, 44, 0,  8, flat, unbanked },
             { -10, 46, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             {  -9, 47, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  {  -9, 47, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  {  -9, 47, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { -11, 48, 0,  0, flat, unbanked },
             { -13, 48, 0,  0, flat, unbanked },  { -15, 48, 0,  0, flat, unbanked },  { -17, 48, 0,  0, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),  { -17, 48, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
             { -16, 48, 0,  0, flat, unbanked },  { -18, 48, 0,  0, flat, unbanked },  { -20, 48, 0,  0, flat, unbanked },  { -22, 48, 0,  0, flat, unbanked },
             { -24, 48, 0,  0, flat, unbanked },  { -26, 48, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { -28, 48, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },   {  2, 15, 0, 16, flat, unbanked },   {  4, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   {  4, 15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   {  4, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   {  4, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  4, 15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {  6, 15, 0, 16, flat, unbanked },   {  8, 15, 0, 16, flat, unbanked },   { 10, 15, 0, 16, flat, unbanked },
              { 12, 15, 0, 16, flat, unbanked },   { 14, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 16, 17, 0, 24, flat, unbanked },
              { 16, 19, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
              { 16, 19, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 16, 19, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),   { 16, 19, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 18, 19, 0, 16, flat, unbanked },
              { 20, 19, 0, 16, flat, unbanked },   { 22, 19, 0, 16, flat, unbanked },   { 24, 19, 0, 16, flat, unbanked },   { 26, 19, 0, 16, flat, unbanked },
              { 28, 19, 0, 16, flat, unbanked },   { 30, 19, 0, 16, flat, unbanked },   { 32, 19, 0, 16, flat, unbanked },   { 34, 19, 0, 16, flat, unbanked },
              { 34, 21, 0,  8, flat, unbanked },   { 34, 23, 0,  8, flat, unbanked },   { 34, 25, 0,  8, flat, unbanked },   { 34, 27, 0,  8, flat, unbanked },
              { 34, 29, 0,  8, flat, unbanked },   { 34, 31, 0,  8, flat, unbanked },   { 34, 33, 0,  8, flat, unbanked },   { 34, 35, 0,  8, flat, unbanked },
              { 34, 37, 0,  8, flat, unbanked },   { 34, 39, 0,  8, flat, unbanked },   { 34, 41, 0,  8, flat, unbanked },   { 36, 41, 0, 16, flat, unbanked },
              { 38, 41, 0, 16, flat, unbanked },   { 40, 41, 0, 16, flat, unbanked },   { 42, 41, 0, 16, flat, unbanked },   { 44, 41, 0, 16, flat, unbanked },
              { 46, 41, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
              { 47, 40, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 47, 40, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),   { 47, 40, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 48, 42, 0,  8, flat, unbanked },
              { 48, 44, 0,  8, flat, unbanked },   { 48, 46, 0,  8, flat, unbanked },   { 48, 48, 0,  8, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),   { 48, 48, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
              { 48, 47, 0,  8, flat, unbanked },   { 48, 49, 0,  8, flat, unbanked },   { 48, 51, 0,  8, flat, unbanked },   { 48, 53, 0,  8, flat, unbanked },
              { 48, 55, 0,  8, flat, unbanked },   { 48, 57, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk3),   { 48, 59, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },  { 15,  29, 0, 24, flat, unbanked },  { 15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 15,  25, 0, 24, flat, unbanked },  { 15,  23, 0, 24, flat, unbanked },  { 15,  21, 0, 24, flat, unbanked },
             { 15,  19, 0, 24, flat, unbanked },  { 15,  17, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 17,  15, 0,  0, flat, unbanked },
             { 19,  15, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             { 19,  15, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { 19,  15, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  { 19,  15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 19,  13, 0, 24, flat, unbanked },
             { 19,  11, 0, 24, flat, unbanked },  { 19,   9, 0, 24, flat, unbanked },  { 19,   7, 0, 24, flat, unbanked },  { 19,   5, 0, 24, flat, unbanked },
             { 19,   3, 0, 24, flat, unbanked },  { 19,   1, 0, 24, flat, unbanked },  { 19,  -1, 0, 24, flat, unbanked },  { 19,  -3, 0, 24, flat, unbanked },
             { 21,  -3, 0, 16, flat, unbanked },  { 23,  -3, 0, 16, flat, unbanked },  { 25,  -3, 0, 16, flat, unbanked },  { 27,  -3, 0, 16, flat, unbanked },
             { 29,  -3, 0, 16, flat, unbanked },  { 31,  -3, 0, 16, flat, unbanked },  { 33,  -3, 0, 16, flat, unbanked },  { 35,  -3, 0, 16, flat, unbanked },
             { 37,  -3, 0, 16, flat, unbanked },  { 39,  -3, 0, 16, flat, unbanked },  { 41,  -3, 0, 16, flat, unbanked },  { 41,  -5, 0, 24, flat, unbanked },
             { 41,  -7, 0, 24, flat, unbanked },  { 41,  -9, 0, 24, flat, unbanked },  { 41, -11, 0, 24, flat, unbanked },  { 41, -13, 0, 24, flat, unbanked },
             { 41, -15, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             { 40, -16, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { 40, -16, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  { 40, -16, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 42, -17, 0, 16, flat, unbanked },
             { 44, -17, 0, 16, flat, unbanked },  { 46, -17, 0, 16, flat, unbanked },  { 48, -17, 0, 16, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),
                          MINI_GOLF_STATE(Unk5),  { 48, -17, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),
             { 47, -17, 0, 16, flat, unbanked },  { 49, -17, 0, 16, flat, unbanked },  { 51, -17, 0, 16, flat, unbanked },  { 53, -17, 0, 16, flat, unbanked },
             { 55, -17, 0, 16, flat, unbanked },  { 57, -17, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 59, -17, 0, 16, flat, unbanked },
        })

        static constexpr const VehicleInfoList * PlayerPathB[] = {
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3,
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // EndStation
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // BeginStation
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // MiddleStation
            &Up25_0,                  &Up25_1,                  &Up25_2,                  &Up25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60
            &FlatToUp25_0,            &FlatToUp25_1,            &FlatToUp25_2,            &FlatToUp25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp25
            &Up25ToFlat_0,            &Up25ToFlat_1,            &Up25ToFlat_2,            &Up25ToFlat_3,
            &Down25_0,                &Down25_1,                &Down25_2,                &Down25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60
            &FlatToDown25_0,          &FlatToDown25_1,          &FlatToDown25_2,          &FlatToDown25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown25
            &Down25ToFlat_0,          &Down25ToFlat_1,          &Down25ToFlat_2,          &Down25ToFlat_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BankedLeftQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BankedRightQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendLeft
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendRight
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftVerticalLoop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightVerticalLoop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankedQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankedQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesDown25
            &LeftQuarterTurn1Tile_0,  &LeftQuarterTurn1Tile_1,  &LeftQuarterTurn1Tile_2,  &LeftQuarterTurn1Tile_3,
            &RightQuarterTurn1Tile_0, &RightQuarterTurn1Tile_1, &RightQuarterTurn1Tile_2, &RightQuarterTurn1Tile_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftTwistDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightTwistDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftTwistUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightTwistUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // TowerBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // TowerSection
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToUp60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToFlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToDown60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToFlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendLeftCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendRightCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixUpSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixUpSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixDownSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixDownSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixUpLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixUpLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixDownLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixDownLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn1TileUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn1TileUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn1TileDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn1TileDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Brakes
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Booster
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Maze
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterBankedHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterBankedHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterBankedHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterBankedHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25LeftBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25RightBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Waterfall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Rapids
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // OnRidePhoto
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25LeftBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25RightBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Watersplash
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp60LongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToFlatLongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Whirlpool
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToFlatLongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown60LongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // CableLiftHill
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // ReverseFreefallSlope
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // ReverseFreefallVertical
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down90ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up90ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BrakeForDrop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthBankToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthBankToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthBankToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthBankToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60ToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60ToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LogFlumeReverser
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SpinningTunnel
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBarrelRollUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBarrelRollUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBarrelRollDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBarrelRollDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToLeftQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToRightQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesDown25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesDown25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // PoweredLift
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftLargeHalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightLargeHalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftLargeHalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightLargeHalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerTwistUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerTwistUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerTwistDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerTwistDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlyerHalfLoopUninvertedUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlyerHalfLoopInvertedDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HeartLineTransferUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HeartLineTransferDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHeartLineRoll
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHeartLineRoll
            &MinigolfHoleA_0,         &MinigolfHoleA_1,         &MinigolfHoleA_2,         &MinigolfHoleA_3,
            &MinigolfHoleB_0,         &MinigolfHoleB_1,         &MinigolfHoleB_2,         &MinigolfHoleB_3,
            &MinigolfHoleC_0,         &MinigolfHoleC_1,         &MinigolfHoleC_2,         &MinigolfHoleC_3,
            &MinigolfHoleD_0,         &MinigolfHoleD_1,         &MinigolfHoleD_2,         &MinigolfHoleD_3,
            &MinigolfHoleE_0,         &MinigolfHoleE_1,         &MinigolfHoleE_2,         &MinigolfHoleE_3,
        };
        // clang-format on
        static_assert(std::size(PlayerPathB) == kVehicleTrackSubpositionSizeMinigolf);
    } // namespace PlayerPathB

    namespace BallPathB
    {
        // clang-format off
        CREATE_VEHICLE_INFO(MinigolfHoleA_0, {
                          MINI_GOLF_STATE(Unk0),   {  31, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  26, 16, 0, 0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  26, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  25, 16, 0, 0, flat, unbanked },
              {  23, 16, 0, 0, flat, unbanked },   {  21, 16, 0, 0, flat, unbanked },   {  19, 16, 0, 0, flat, unbanked },   {  18, 16, 0, 0, flat, unbanked },
              {  17, 16, 0, 0, flat, unbanked },   {  16, 16, 0, 0, flat, unbanked },   {  15, 16, 0, 0, flat, unbanked },   {  14, 16, 0, 0, flat, unbanked },
              {  13, 16, 0, 0, flat, unbanked },   {  12, 16, 0, 0, flat, unbanked },   {  11, 16, 0, 0, flat, unbanked },   {  10, 16, 0, 0, flat, unbanked },
              {   9, 16, 0, 0, flat, unbanked },   {   8, 16, 0, 0, flat, unbanked },   {   7, 16, 0, 0, flat, unbanked },   {   6, 16, 0, 0, flat, unbanked },
              {   5, 16, 0, 0, flat, unbanked },   {   4, 16, 0, 0, flat, unbanked },   {   3, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   {   2, 16, 0, 0, flat, unbanked },   {   1, 16, 0, 0, flat, unbanked },   {  -1, 16, 0, 0, flat, unbanked },
              {  -3, 16, 0, 0, flat, unbanked },   {  -4, 16, 0, 0, flat, unbanked },   {  -5, 16, 0, 0, flat, unbanked },   {  -6, 16, 0, 0, flat, unbanked },
              {  -7, 16, 0, 0, flat, unbanked },   {  -8, 16, 0, 0, flat, unbanked },   {  -9, 16, 0, 0, flat, unbanked },   { -10, 16, 0, 0, flat, unbanked },
              { -11, 16, 0, 0, flat, unbanked },   { -12, 16, 0, 0, flat, unbanked },   { -13, 16, 0, 0, flat, unbanked },   { -14, 16, 0, 0, flat, unbanked },
              { -15, 16, 0, 0, flat, unbanked },   { -16, 16, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
              { -29, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),   { -29, 16, 0, 0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_1, {
                          MINI_GOLF_STATE(Unk0),    { 16,  0, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16,  5, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),    { 16,  5, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),    { 16,  6, 0, 8, flat, unbanked },
               { 16,  8, 0, 8, flat, unbanked },    { 16, 10, 0, 8, flat, unbanked },    { 16, 12, 0, 8, flat, unbanked },    { 16, 13, 0, 8, flat, unbanked },
               { 16, 14, 0, 8, flat, unbanked },    { 16, 15, 0, 8, flat, unbanked },    { 16, 16, 0, 8, flat, unbanked },    { 16, 17, 0, 8, flat, unbanked },
               { 16, 18, 0, 8, flat, unbanked },    { 16, 19, 0, 8, flat, unbanked },    { 16, 20, 0, 8, flat, unbanked },    { 16, 21, 0, 8, flat, unbanked },
               { 16, 22, 0, 8, flat, unbanked },    { 16, 23, 0, 8, flat, unbanked },    { 16, 24, 0, 8, flat, unbanked },    { 16, 25, 0, 8, flat, unbanked },
               { 16, 26, 0, 8, flat, unbanked },    { 16, 27, 0, 8, flat, unbanked },    { 16, 28, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),    { 16, 29, 0, 8, flat, unbanked },    { 16, 30, 0, 8, flat, unbanked },    { 16, 32, 0, 8, flat, unbanked },
               { 16, 34, 0, 8, flat, unbanked },    { 16, 35, 0, 8, flat, unbanked },    { 16, 36, 0, 8, flat, unbanked },    { 16, 37, 0, 8, flat, unbanked },
               { 16, 38, 0, 8, flat, unbanked },    { 16, 39, 0, 8, flat, unbanked },    { 16, 40, 0, 8, flat, unbanked },    { 16, 41, 0, 8, flat, unbanked },
               { 16, 42, 0, 8, flat, unbanked },    { 16, 43, 0, 8, flat, unbanked },    { 16, 44, 0, 8, flat, unbanked },    { 16, 45, 0, 8, flat, unbanked },
               { 16, 46, 0, 8, flat, unbanked },    { 16, 47, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
               { 16, 60, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),    { 16, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  5, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  5, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  6, 15, 0, 16, flat, unbanked },
              {  8, 15, 0, 16, flat, unbanked },   { 10, 15, 0, 16, flat, unbanked },   { 12, 15, 0, 16, flat, unbanked },   { 13, 15, 0, 16, flat, unbanked },
              { 14, 15, 0, 16, flat, unbanked },   { 15, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 17, 15, 0, 16, flat, unbanked },
              { 18, 15, 0, 16, flat, unbanked },   { 19, 15, 0, 16, flat, unbanked },   { 20, 15, 0, 16, flat, unbanked },   { 21, 15, 0, 16, flat, unbanked },
              { 22, 15, 0, 16, flat, unbanked },   { 23, 15, 0, 16, flat, unbanked },   { 24, 15, 0, 16, flat, unbanked },   { 25, 15, 0, 16, flat, unbanked },
              { 26, 15, 0, 16, flat, unbanked },   { 27, 15, 0, 16, flat, unbanked },   { 28, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   { 29, 15, 0, 16, flat, unbanked },   { 30, 15, 0, 16, flat, unbanked },   { 32, 15, 0, 16, flat, unbanked },
              { 34, 15, 0, 16, flat, unbanked },   { 35, 15, 0, 16, flat, unbanked },   { 36, 15, 0, 16, flat, unbanked },   { 37, 15, 0, 16, flat, unbanked },
              { 38, 15, 0, 16, flat, unbanked },   { 39, 15, 0, 16, flat, unbanked },   { 40, 15, 0, 16, flat, unbanked },   { 41, 15, 0, 16, flat, unbanked },
              { 42, 15, 0, 16, flat, unbanked },   { 43, 15, 0, 16, flat, unbanked },   { 44, 15, 0, 16, flat, unbanked },   { 45, 15, 0, 16, flat, unbanked },
              { 46, 15, 0, 16, flat, unbanked },   { 47, 15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
              { 60, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),   { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15,  25, 0, 24, flat, unbanked },
             { 15,  23, 0, 24, flat, unbanked },  { 15,  21, 0, 24, flat, unbanked },  { 15,  19, 0, 24, flat, unbanked },  { 15,  18, 0, 24, flat, unbanked },
             { 15,  17, 0, 24, flat, unbanked },  { 15,  16, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 15,  14, 0, 24, flat, unbanked },
             { 15,  13, 0, 24, flat, unbanked },  { 15,  12, 0, 24, flat, unbanked },  { 15,  11, 0, 24, flat, unbanked },  { 15,  10, 0, 24, flat, unbanked },
             { 15,   9, 0, 24, flat, unbanked },  { 15,   8, 0, 24, flat, unbanked },  { 15,   7, 0, 24, flat, unbanked },  { 15,   6, 0, 24, flat, unbanked },
             { 15,   5, 0, 24, flat, unbanked },  { 15,   4, 0, 24, flat, unbanked },  { 15,   3, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),  { 15,   2, 0, 24, flat, unbanked },  { 15,   1, 0, 24, flat, unbanked },  { 15,  -1, 0, 24, flat, unbanked },
             { 15,  -3, 0, 24, flat, unbanked },  { 15,  -4, 0, 24, flat, unbanked },  { 15,  -5, 0, 24, flat, unbanked },  { 15,  -6, 0, 24, flat, unbanked },
             { 15,  -7, 0, 24, flat, unbanked },  { 15,  -8, 0, 24, flat, unbanked },  { 15,  -9, 0, 24, flat, unbanked },  { 15, -10, 0, 24, flat, unbanked },
             { 15, -11, 0, 24, flat, unbanked },  { 15, -12, 0, 24, flat, unbanked },  { 15, -13, 0, 24, flat, unbanked },  { 15, -14, 0, 24, flat, unbanked },
             { 15, -15, 0, 24, flat, unbanked },  { 15, -16, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
             { 15, -29, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),  { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_0, {
                          MINI_GOLF_STATE(Unk0),   {  31, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  26, 16, 0, 0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  26, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  25, 16, 0, 0, flat, unbanked },
              {  22, 15, 0, 0, flat, unbanked },   {  20, 15, 0, 0, flat, unbanked },   {  18, 15, 0, 0, flat, unbanked },   {  16, 14, 0, 0, flat, unbanked },
              {  14, 14, 0, 0, flat, unbanked },   {  12, 13, 0, 0, flat, unbanked },   {  10, 13, 0, 0, flat, unbanked },   {   8, 13, 0, 0, flat, unbanked },
              {   6, 12, 0, 0, flat, unbanked },   {   4, 12, 0, 0, flat, unbanked },   {   2, 11, 0, 0, flat, unbanked },   {   1, 11, 0, 0, flat, unbanked },
              {   0, 11, 0, 0, flat, unbanked },   {   0, 12, 0, 0, flat, unbanked },   {   0, 13, 0, 0, flat, unbanked },   {   0, 14, 0, 0, flat, unbanked },
              {   1, 15, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   {   1, 16, 0, 0, flat, unbanked },
              {  -1, 16, 0, 0, flat, unbanked },   {  -3, 16, 0, 0, flat, unbanked },   {  -4, 16, 0, 0, flat, unbanked },   {  -5, 16, 0, 0, flat, unbanked },
              {  -5, 17, 0, 0, flat, unbanked },   {  -5, 18, 0, 0, flat, unbanked },   {  -5, 19, 0, 0, flat, unbanked },   {  -5, 20, 0, 0, flat, unbanked },
              {  -5, 21, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   {  -5, 22, 0, 0, flat, unbanked },
              {  -7, 22, 0, 0, flat, unbanked },   {  -9, 21, 0, 0, flat, unbanked },   { -11, 21, 0, 0, flat, unbanked },   { -13, 20, 0, 0, flat, unbanked },
              { -15, 20, 0, 0, flat, unbanked },   { -16, 20, 0, 0, flat, unbanked },   { -17, 19, 0, 0, flat, unbanked },   { -18, 19, 0, 0, flat, unbanked },
              { -19, 18, 0, 0, flat, unbanked },   { -20, 18, 0, 0, flat, unbanked },   { -21, 18, 0, 0, flat, unbanked },   { -22, 17, 0, 0, flat, unbanked },
              { -23, 17, 0, 0, flat, unbanked },   { -24, 17, 0, 0, flat, unbanked },   { -25, 16, 0, 0, flat, unbanked },   { -26, 16, 0, 0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { -29, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),   { -29, 16, 0, 0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_1, {
                          MINI_GOLF_STATE(Unk0),    { 16,  0, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16,  5, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),    { 16,  5, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),    { 16,  6, 0, 8, flat, unbanked },
               { 15,  9, 0, 8, flat, unbanked },    { 15, 11, 0, 8, flat, unbanked },    { 15, 13, 0, 8, flat, unbanked },    { 14, 15, 0, 8, flat, unbanked },
               { 14, 17, 0, 8, flat, unbanked },    { 13, 19, 0, 8, flat, unbanked },    { 13, 21, 0, 8, flat, unbanked },    { 13, 23, 0, 8, flat, unbanked },
               { 12, 25, 0, 8, flat, unbanked },    { 12, 27, 0, 8, flat, unbanked },    { 11, 29, 0, 8, flat, unbanked },    { 11, 30, 0, 8, flat, unbanked },
               { 11, 31, 0, 8, flat, unbanked },    { 12, 31, 0, 8, flat, unbanked },    { 13, 31, 0, 8, flat, unbanked },    { 14, 31, 0, 8, flat, unbanked },
               { 15, 30, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),    { 16, 30, 0, 8, flat, unbanked },
               { 16, 32, 0, 8, flat, unbanked },    { 16, 34, 0, 8, flat, unbanked },    { 16, 35, 0, 8, flat, unbanked },    { 16, 36, 0, 8, flat, unbanked },
               { 17, 36, 0, 8, flat, unbanked },    { 18, 36, 0, 8, flat, unbanked },    { 19, 36, 0, 8, flat, unbanked },    { 20, 36, 0, 8, flat, unbanked },
               { 21, 36, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),    { 22, 36, 0, 8, flat, unbanked },
               { 22, 38, 0, 8, flat, unbanked },    { 21, 40, 0, 8, flat, unbanked },    { 21, 42, 0, 8, flat, unbanked },    { 20, 44, 0, 8, flat, unbanked },
               { 20, 46, 0, 8, flat, unbanked },    { 20, 47, 0, 8, flat, unbanked },    { 19, 48, 0, 8, flat, unbanked },    { 19, 49, 0, 8, flat, unbanked },
               { 18, 50, 0, 8, flat, unbanked },    { 18, 51, 0, 8, flat, unbanked },    { 18, 52, 0, 8, flat, unbanked },    { 17, 53, 0, 8, flat, unbanked },
               { 17, 54, 0, 8, flat, unbanked },    { 17, 55, 0, 8, flat, unbanked },    { 16, 56, 0, 8, flat, unbanked },    { 16, 57, 0, 8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),    { 16, 60, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),    { 16, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  5, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  5, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  6, 15, 0, 16, flat, unbanked },
              {  9, 16, 0, 16, flat, unbanked },   { 11, 16, 0, 16, flat, unbanked },   { 13, 16, 0, 16, flat, unbanked },   { 15, 17, 0, 16, flat, unbanked },
              { 17, 17, 0, 16, flat, unbanked },   { 19, 18, 0, 16, flat, unbanked },   { 21, 18, 0, 16, flat, unbanked },   { 23, 18, 0, 16, flat, unbanked },
              { 25, 19, 0, 16, flat, unbanked },   { 27, 19, 0, 16, flat, unbanked },   { 29, 20, 0, 16, flat, unbanked },   { 30, 20, 0, 16, flat, unbanked },
              { 31, 20, 0, 16, flat, unbanked },   { 31, 19, 0, 16, flat, unbanked },   { 31, 18, 0, 16, flat, unbanked },   { 31, 17, 0, 16, flat, unbanked },
              { 30, 16, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   { 30, 15, 0, 16, flat, unbanked },
              { 32, 15, 0, 16, flat, unbanked },   { 34, 15, 0, 16, flat, unbanked },   { 35, 15, 0, 16, flat, unbanked },   { 36, 15, 0, 16, flat, unbanked },
              { 36, 14, 0, 16, flat, unbanked },   { 36, 13, 0, 16, flat, unbanked },   { 36, 12, 0, 16, flat, unbanked },   { 36, 11, 0, 16, flat, unbanked },
              { 36, 10, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   { 36,  9, 0, 16, flat, unbanked },
              { 38,  9, 0, 16, flat, unbanked },   { 40, 10, 0, 16, flat, unbanked },   { 42, 10, 0, 16, flat, unbanked },   { 44, 11, 0, 16, flat, unbanked },
              { 46, 11, 0, 16, flat, unbanked },   { 47, 11, 0, 16, flat, unbanked },   { 48, 12, 0, 16, flat, unbanked },   { 49, 12, 0, 16, flat, unbanked },
              { 50, 13, 0, 16, flat, unbanked },   { 51, 13, 0, 16, flat, unbanked },   { 52, 13, 0, 16, flat, unbanked },   { 53, 14, 0, 16, flat, unbanked },
              { 54, 14, 0, 16, flat, unbanked },   { 55, 14, 0, 16, flat, unbanked },   { 56, 15, 0, 16, flat, unbanked },   { 57, 15, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { 60, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),   { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15,  25, 0, 24, flat, unbanked },
             { 16,  22, 0, 24, flat, unbanked },  { 16,  20, 0, 24, flat, unbanked },  { 16,  18, 0, 24, flat, unbanked },  { 17,  16, 0, 24, flat, unbanked },
             { 17,  14, 0, 24, flat, unbanked },  { 18,  12, 0, 24, flat, unbanked },  { 18,  10, 0, 24, flat, unbanked },  { 18,   8, 0, 24, flat, unbanked },
             { 19,   6, 0, 24, flat, unbanked },  { 19,   4, 0, 24, flat, unbanked },  { 20,   2, 0, 24, flat, unbanked },  { 20,   1, 0, 24, flat, unbanked },
             { 20,   0, 0, 24, flat, unbanked },  { 19,   0, 0, 24, flat, unbanked },  { 18,   0, 0, 24, flat, unbanked },  { 17,   0, 0, 24, flat, unbanked },
             { 16,   1, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),  { 15,   1, 0, 24, flat, unbanked },
             { 15,  -1, 0, 24, flat, unbanked },  { 15,  -3, 0, 24, flat, unbanked },  { 15,  -4, 0, 24, flat, unbanked },  { 15,  -5, 0, 24, flat, unbanked },
             { 14,  -5, 0, 24, flat, unbanked },  { 13,  -5, 0, 24, flat, unbanked },  { 12,  -5, 0, 24, flat, unbanked },  { 11,  -5, 0, 24, flat, unbanked },
             { 10,  -5, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),  {  9,  -5, 0, 24, flat, unbanked },
             {  9,  -7, 0, 24, flat, unbanked },  { 10,  -9, 0, 24, flat, unbanked },  { 10, -11, 0, 24, flat, unbanked },  { 11, -13, 0, 24, flat, unbanked },
             { 11, -15, 0, 24, flat, unbanked },  { 11, -16, 0, 24, flat, unbanked },  { 12, -17, 0, 24, flat, unbanked },  { 12, -18, 0, 24, flat, unbanked },
             { 13, -19, 0, 24, flat, unbanked },  { 13, -20, 0, 24, flat, unbanked },  { 13, -21, 0, 24, flat, unbanked },  { 14, -22, 0, 24, flat, unbanked },
             { 14, -23, 0, 24, flat, unbanked },  { 14, -24, 0, 24, flat, unbanked },  { 15, -25, 0, 24, flat, unbanked },  { 15, -26, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),  { 15, -29, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),  { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_0, {
                          MINI_GOLF_STATE(Unk0),   {  31, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  26, 16, 0, 0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  26, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  24, 16, 0, 0, flat, unbanked },
              {  21, 16, 0, 0, flat, unbanked },   {  18, 16, 0, 0, flat, unbanked },   {  15, 16, 0, 0, flat, unbanked },   {  13, 16, 0, 0, flat, unbanked },
              {  11, 16, 0, 0, flat, unbanked },   {   9, 16, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   {   7, 15, 0, 0, flat, unbanked },
              {   5, 15, 0, 0, flat, unbanked },   {   3, 14, 0, 0, flat, unbanked },   {   1, 13, 0, 0, flat, unbanked },   {  -1, 13, 0, 0, flat, unbanked },
              {  -3, 12, 0, 0, flat, unbanked },   {  -5, 12, 0, 0, flat, unbanked },   {  -7, 11, 0, 0, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),
              {  -8, 11, 0, 0, flat, unbanked },   {  -9, 11, 0, 0, flat, unbanked },   { -10, 11, 0, 0, flat, unbanked },   { -11, 11, 0, 0, flat, unbanked },
              { -12, 11, 0, 0, flat, unbanked },   { -13, 11, 0, 0, flat, unbanked },   { -14, 11, 0, 0, flat, unbanked },   { -15, 11, 0, 0, flat, unbanked },
              { -16, 11, 0, 0, flat, unbanked },   { -17, 11, 0, 0, flat, unbanked },   { -18, 11, 0, 0, flat, unbanked },   { -19, 11, 0, 0, flat, unbanked },
              { -20, 11, 0, 0, flat, unbanked },   { -21, 11, 0, 0, flat, unbanked },   { -22, 11, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   { -23, 11, 0, 0, flat, unbanked },   { -23, 12, 0, 0, flat, unbanked },   { -23, 13, 0, 0, flat, unbanked },
              { -23, 14, 0, 0, flat, unbanked },   { -23, 15, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
              { -29, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_1, {
                          MINI_GOLF_STATE(Unk0),    { 16,  0, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16,  5, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),    { 16,  5, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),    { 16,  7, 0, 8, flat, unbanked },
               { 16, 10, 0, 8, flat, unbanked },    { 16, 13, 0, 8, flat, unbanked },    { 16, 16, 0, 8, flat, unbanked },    { 16, 18, 0, 8, flat, unbanked },
               { 16, 20, 0, 8, flat, unbanked },    { 16, 22, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),    { 15, 24, 0, 8, flat, unbanked },
               { 15, 26, 0, 8, flat, unbanked },    { 14, 28, 0, 8, flat, unbanked },    { 13, 30, 0, 8, flat, unbanked },    { 13, 32, 0, 8, flat, unbanked },
               { 12, 34, 0, 8, flat, unbanked },    { 12, 36, 0, 8, flat, unbanked },    { 11, 38, 0, 8, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),
               { 11, 39, 0, 8, flat, unbanked },    { 11, 40, 0, 8, flat, unbanked },    { 11, 41, 0, 8, flat, unbanked },    { 11, 42, 0, 8, flat, unbanked },
               { 11, 43, 0, 8, flat, unbanked },    { 11, 44, 0, 8, flat, unbanked },    { 11, 45, 0, 8, flat, unbanked },    { 11, 46, 0, 8, flat, unbanked },
               { 11, 47, 0, 8, flat, unbanked },    { 11, 48, 0, 8, flat, unbanked },    { 11, 49, 0, 8, flat, unbanked },    { 11, 50, 0, 8, flat, unbanked },
               { 11, 51, 0, 8, flat, unbanked },    { 11, 52, 0, 8, flat, unbanked },    { 11, 53, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),    { 11, 54, 0, 8, flat, unbanked },    { 12, 54, 0, 8, flat, unbanked },    { 13, 54, 0, 8, flat, unbanked },
               { 14, 54, 0, 8, flat, unbanked },    { 15, 54, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
               { 16, 60, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  5, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  5, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  7, 15, 0, 16, flat, unbanked },
              { 10, 15, 0, 16, flat, unbanked },   { 13, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 18, 15, 0, 16, flat, unbanked },
              { 20, 15, 0, 16, flat, unbanked },   { 22, 15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 24, 16, 0, 16, flat, unbanked },
              { 26, 16, 0, 16, flat, unbanked },   { 28, 17, 0, 16, flat, unbanked },   { 30, 18, 0, 16, flat, unbanked },   { 32, 18, 0, 16, flat, unbanked },
              { 34, 19, 0, 16, flat, unbanked },   { 36, 19, 0, 16, flat, unbanked },   { 38, 20, 0, 16, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),
              { 39, 20, 0, 16, flat, unbanked },   { 40, 20, 0, 16, flat, unbanked },   { 41, 20, 0, 16, flat, unbanked },   { 42, 20, 0, 16, flat, unbanked },
              { 43, 20, 0, 16, flat, unbanked },   { 44, 20, 0, 16, flat, unbanked },   { 45, 20, 0, 16, flat, unbanked },   { 46, 20, 0, 16, flat, unbanked },
              { 47, 20, 0, 16, flat, unbanked },   { 48, 20, 0, 16, flat, unbanked },   { 49, 20, 0, 16, flat, unbanked },   { 50, 20, 0, 16, flat, unbanked },
              { 51, 20, 0, 16, flat, unbanked },   { 52, 20, 0, 16, flat, unbanked },   { 53, 20, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   { 54, 20, 0, 16, flat, unbanked },   { 54, 19, 0, 16, flat, unbanked },   { 54, 18, 0, 16, flat, unbanked },
              { 54, 17, 0, 16, flat, unbanked },   { 54, 16, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
              { 60, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15,  24, 0, 24, flat, unbanked },
             { 15,  21, 0, 24, flat, unbanked },  { 15,  18, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 15,  13, 0, 24, flat, unbanked },
             { 15,  11, 0, 24, flat, unbanked },  { 15,   9, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 16,   7, 0, 24, flat, unbanked },
             { 16,   5, 0, 24, flat, unbanked },  { 17,   3, 0, 24, flat, unbanked },  { 18,   1, 0, 24, flat, unbanked },  { 18,  -1, 0, 24, flat, unbanked },
             { 19,  -3, 0, 24, flat, unbanked },  { 19,  -5, 0, 24, flat, unbanked },  { 20,  -7, 0, 24, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),
             { 20,  -8, 0, 24, flat, unbanked },  { 20,  -9, 0, 24, flat, unbanked },  { 20, -10, 0, 24, flat, unbanked },  { 20, -11, 0, 24, flat, unbanked },
             { 20, -12, 0, 24, flat, unbanked },  { 20, -13, 0, 24, flat, unbanked },  { 20, -14, 0, 24, flat, unbanked },  { 20, -15, 0, 24, flat, unbanked },
             { 20, -16, 0, 24, flat, unbanked },  { 20, -17, 0, 24, flat, unbanked },  { 20, -18, 0, 24, flat, unbanked },  { 20, -19, 0, 24, flat, unbanked },
             { 20, -20, 0, 24, flat, unbanked },  { 20, -21, 0, 24, flat, unbanked },  { 20, -22, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),  { 20, -23, 0, 24, flat, unbanked },  { 19, -23, 0, 24, flat, unbanked },  { 18, -23, 0, 24, flat, unbanked },
             { 17, -23, 0, 24, flat, unbanked },  { 16, -23, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
             { 15, -29, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_0, {
                          MINI_GOLF_STATE(Unk0),   {  31, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  26, 16, 0, 0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  26, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  24, 16, 0, 0, flat, unbanked },
              {  21, 17, 0, 0, flat, unbanked },   {  19, 17, 0, 0, flat, unbanked },   {  17, 17, 0, 0, flat, unbanked },   {  15, 16, 0, 0, flat, unbanked },
              {  13, 18, 0, 0, flat, unbanked },   {  11, 18, 0, 0, flat, unbanked },   {   9, 18, 0, 0, flat, unbanked },   {   7, 18, 0, 0, flat, unbanked },
              {   5, 18, 0, 0, flat, unbanked },   {   4, 18, 0, 0, flat, unbanked },   {   3, 19, 0, 0, flat, unbanked },   {   2, 19, 0, 0, flat, unbanked },
              {   1, 19, 0, 0, flat, unbanked },   {   0, 19, 0, 0, flat, unbanked },   {  -1, 19, 0, 0, flat, unbanked },   {  -2, 19, 0, 0, flat, unbanked },
              {  -1, 19, 0, 0, flat, unbanked },   {   0, 19, 0, 0, flat, unbanked },   {   1, 19, 0, 0, flat, unbanked },   {   2, 19, 0, 0, flat, unbanked },
              {   3, 19, 0, 0, flat, unbanked },   {   4, 19, 0, 0, flat, unbanked },   {   5, 19, 0, 0, flat, unbanked },   {   6, 20, 0, 0, flat, unbanked },
              {   7, 20, 0, 0, flat, unbanked },   {   8, 20, 0, 0, flat, unbanked },   {   9, 20, 0, 0, flat, unbanked },   {  10, 20, 0, 0, flat, unbanked },
              {  11, 20, 0, 0, flat, unbanked },   {  12, 20, 0, 0, flat, unbanked },   {  13, 20, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   {  14, 20, 0, 0, flat, unbanked },   {  12, 20, 0, 0, flat, unbanked },   {   9, 20, 0, 0, flat, unbanked },
              {   6, 20, 0, 0, flat, unbanked },   {   3, 20, 0, 0, flat, unbanked },   {   0, 20, 0, 0, flat, unbanked },   {  -3, 20, 0, 0, flat, unbanked },
              {  -6, 20, 0, 0, flat, unbanked },   {  -9, 20, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { -12, 20, 0, 0, flat, unbanked },
              { -15, 20, 0, 0, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { -18, 16, 0, 0, flat, unbanked },   { -21, 16, 0, 0, flat, unbanked },
              { -24, 16, 0, 0, flat, unbanked },   { -25, 15, 0, 0, flat, unbanked },   { -26, 13, 0, 0, flat, unbanked },   { -27, 11, 0, 0, flat, unbanked },
              { -28,  9, 0, 0, flat, unbanked },   { -26,  7, 0, 0, flat, unbanked },   { -24,  5, 1, 0, flat, unbanked },   { -22,  3, 1, 0, flat, unbanked },
              { -20,  2, 1, 0, flat, unbanked },   { -18,  2, 1, 0, flat, unbanked },   { -16,  3, 2, 0, flat, unbanked },   { -14,  3, 2, 0, flat, unbanked },
              { -14,  5, 2, 0, flat, unbanked },   { -13,  7, 2, 0, flat, unbanked },   { -12,  9, 3, 0, flat, unbanked },   { -12, 11, 3, 0, flat, unbanked },
              { -11, 13, 3, 0, flat, unbanked },   { -12, 15, 3, 0, flat, unbanked },   { -12, 17, 3, 0, flat, unbanked },   { -12, 19, 3, 0, flat, unbanked },
              { -13, 21, 3, 0, flat, unbanked },   { -13, 23, 3, 0, flat, unbanked },   { -13, 25, 3, 0, flat, unbanked },   { -14, 27, 3, 0, flat, unbanked },
              { -14, 29, 3, 0, flat, unbanked },   { -14, 30, 3, 0, flat, unbanked },   { -14, 31, 3, 0, flat, unbanked },   { -15, 32, 3, 0, flat, unbanked },
              { -15, 33, 3, 0, flat, unbanked },   { -15, 34, 3, 0, flat, unbanked },   { -15, 35, 3, 0, flat, unbanked },   { -15, 36, 3, 0, flat, unbanked },
              { -15, 38, 0, 0, flat, unbanked },   { -16, 40, 0, 0, flat, unbanked },   { -16, 41, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   { -16, 42, 0, 0, flat, unbanked },   { -16, 43, 0, 0, flat, unbanked },   { -16, 44, 0, 0, flat, unbanked },
              { -16, 45, 0, 0, flat, unbanked },   { -17, 46, 0, 0, flat, unbanked },   { -17, 47, 0, 0, flat, unbanked },   { -17, 48, 0, 0, flat, unbanked },
              { -17, 49, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { -17, 60, 0, 8, flat, unbanked },
                          MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),   { -17, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_1, {
                          MINI_GOLF_STATE(Unk0),   { 16,  0, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 16,  5, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   { 16,  5, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk3),   { 16,  7, 0,  8, flat, unbanked },
              { 17, 10, 0,  8, flat, unbanked },   { 17, 12, 0,  8, flat, unbanked },   { 17, 14, 0,  8, flat, unbanked },   { 16, 16, 0,  8, flat, unbanked },
              { 18, 18, 0,  8, flat, unbanked },   { 18, 20, 0,  8, flat, unbanked },   { 18, 22, 0,  8, flat, unbanked },   { 18, 24, 0,  8, flat, unbanked },
              { 18, 26, 0,  8, flat, unbanked },   { 18, 27, 0,  8, flat, unbanked },   { 19, 28, 0,  8, flat, unbanked },   { 19, 29, 0,  8, flat, unbanked },
              { 19, 30, 0,  8, flat, unbanked },   { 19, 31, 0,  8, flat, unbanked },   { 19, 32, 0,  8, flat, unbanked },   { 19, 33, 0,  8, flat, unbanked },
              { 19, 32, 0,  8, flat, unbanked },   { 19, 31, 0,  8, flat, unbanked },   { 19, 30, 0,  8, flat, unbanked },   { 19, 29, 0,  8, flat, unbanked },
              { 19, 28, 0,  8, flat, unbanked },   { 19, 27, 0,  8, flat, unbanked },   { 19, 26, 0,  8, flat, unbanked },   { 20, 25, 0,  8, flat, unbanked },
              { 20, 24, 0,  8, flat, unbanked },   { 20, 23, 0,  8, flat, unbanked },   { 20, 22, 0,  8, flat, unbanked },   { 20, 21, 0,  8, flat, unbanked },
              { 20, 20, 0,  8, flat, unbanked },   { 20, 19, 0,  8, flat, unbanked },   { 20, 18, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   { 20, 17, 0,  8, flat, unbanked },   { 20, 19, 0,  8, flat, unbanked },   { 20, 22, 0,  8, flat, unbanked },
              { 20, 25, 0,  8, flat, unbanked },   { 20, 28, 0,  8, flat, unbanked },   { 20, 31, 0,  8, flat, unbanked },   { 20, 34, 0,  8, flat, unbanked },
              { 20, 37, 0,  8, flat, unbanked },   { 20, 40, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 20, 43, 0,  8, flat, unbanked },
              { 20, 46, 0,  8, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { 16, 49, 0,  8, flat, unbanked },   { 16, 52, 0,  8, flat, unbanked },
              { 16, 55, 0,  8, flat, unbanked },   { 15, 56, 0,  8, flat, unbanked },   { 13, 57, 0,  8, flat, unbanked },   { 11, 58, 0,  8, flat, unbanked },
              {  9, 59, 0,  8, flat, unbanked },   {  7, 57, 0,  8, flat, unbanked },   {  5, 55, 1,  8, flat, unbanked },   {  3, 53, 1,  8, flat, unbanked },
              {  2, 51, 1,  8, flat, unbanked },   {  2, 49, 1,  8, flat, unbanked },   {  3, 47, 2,  8, flat, unbanked },   {  3, 45, 2,  8, flat, unbanked },
              {  5, 45, 2,  8, flat, unbanked },   {  7, 44, 2,  8, flat, unbanked },   {  9, 43, 3,  8, flat, unbanked },   { 11, 43, 3,  8, flat, unbanked },
              { 13, 42, 3,  8, flat, unbanked },   { 15, 43, 3,  8, flat, unbanked },   { 17, 43, 3,  8, flat, unbanked },   { 19, 43, 3,  8, flat, unbanked },
              { 21, 44, 3,  8, flat, unbanked },   { 23, 44, 3,  8, flat, unbanked },   { 25, 44, 3,  8, flat, unbanked },   { 27, 45, 3,  8, flat, unbanked },
              { 29, 45, 3,  8, flat, unbanked },   { 30, 45, 3,  8, flat, unbanked },   { 31, 45, 3,  8, flat, unbanked },   { 32, 46, 3,  8, flat, unbanked },
              { 33, 46, 3,  8, flat, unbanked },   { 34, 46, 3,  8, flat, unbanked },   { 35, 46, 3,  8, flat, unbanked },   { 36, 46, 3,  8, flat, unbanked },
              { 38, 46, 0,  8, flat, unbanked },   { 40, 47, 0,  8, flat, unbanked },   { 41, 47, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   { 42, 47, 0,  8, flat, unbanked },   { 43, 47, 0,  8, flat, unbanked },   { 44, 47, 0,  8, flat, unbanked },
              { 45, 47, 0,  8, flat, unbanked },   { 46, 48, 0,  8, flat, unbanked },   { 47, 48, 0,  8, flat, unbanked },   { 48, 48, 0,  8, flat, unbanked },
              { 49, 48, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { 60, 48, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),   { 60, 48, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_2, {
                          MINI_GOLF_STATE(Unk0),  {  0,  15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  5,  15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  {  5,  15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),  {  7,  15, 0, 16, flat, unbanked },
             { 10,  14, 0, 16, flat, unbanked },  { 12,  14, 0, 16, flat, unbanked },  { 14,  14, 0, 16, flat, unbanked },  { 16,  15, 0, 16, flat, unbanked },
             { 18,  13, 0, 16, flat, unbanked },  { 20,  13, 0, 16, flat, unbanked },  { 22,  13, 0, 16, flat, unbanked },  { 24,  13, 0, 16, flat, unbanked },
             { 26,  13, 0, 16, flat, unbanked },  { 27,  13, 0, 16, flat, unbanked },  { 28,  12, 0, 16, flat, unbanked },  { 29,  12, 0, 16, flat, unbanked },
             { 30,  12, 0, 16, flat, unbanked },  { 31,  12, 0, 16, flat, unbanked },  { 32,  12, 0, 16, flat, unbanked },  { 33,  12, 0, 16, flat, unbanked },
             { 32,  12, 0, 16, flat, unbanked },  { 31,  12, 0, 16, flat, unbanked },  { 30,  12, 0, 16, flat, unbanked },  { 29,  12, 0, 16, flat, unbanked },
             { 28,  12, 0, 16, flat, unbanked },  { 27,  12, 0, 16, flat, unbanked },  { 26,  12, 0, 16, flat, unbanked },  { 25,  11, 0, 16, flat, unbanked },
             { 24,  11, 0, 16, flat, unbanked },  { 23,  11, 0, 16, flat, unbanked },  { 22,  11, 0, 16, flat, unbanked },  { 21,  11, 0, 16, flat, unbanked },
             { 20,  11, 0, 16, flat, unbanked },  { 19,  11, 0, 16, flat, unbanked },  { 18,  11, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),  { 17,  11, 0, 16, flat, unbanked },  { 19,  11, 0, 16, flat, unbanked },  { 22,  11, 0, 16, flat, unbanked },
             { 25,  11, 0, 16, flat, unbanked },  { 28,  11, 0, 16, flat, unbanked },  { 31,  11, 0, 16, flat, unbanked },  { 34,  11, 0, 16, flat, unbanked },
             { 37,  11, 0, 16, flat, unbanked },  { 40,  11, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 43,  11, 0, 16, flat, unbanked },
             { 46,  11, 0, 16, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),  { 49,  15, 0, 16, flat, unbanked },  { 52,  15, 0, 16, flat, unbanked },
             { 55,  15, 0, 16, flat, unbanked },  { 56,  16, 0, 16, flat, unbanked },  { 57,  18, 0, 16, flat, unbanked },  { 58,  20, 0, 16, flat, unbanked },
             { 59,  22, 0, 16, flat, unbanked },  { 57,  24, 0, 16, flat, unbanked },  { 55,  26, 1, 16, flat, unbanked },  { 53,  28, 1, 16, flat, unbanked },
             { 51,  29, 1, 16, flat, unbanked },  { 49,  29, 1, 16, flat, unbanked },  { 47,  28, 2, 16, flat, unbanked },  { 45,  28, 2, 16, flat, unbanked },
             { 45,  26, 2, 16, flat, unbanked },  { 44,  24, 2, 16, flat, unbanked },  { 43,  22, 3, 16, flat, unbanked },  { 43,  20, 3, 16, flat, unbanked },
             { 42,  18, 3, 16, flat, unbanked },  { 43,  16, 3, 16, flat, unbanked },  { 43,  14, 3, 16, flat, unbanked },  { 43,  12, 3, 16, flat, unbanked },
             { 44,  10, 3, 16, flat, unbanked },  { 44,   8, 3, 16, flat, unbanked },  { 44,   6, 3, 16, flat, unbanked },  { 45,   4, 3, 16, flat, unbanked },
             { 45,   2, 3, 16, flat, unbanked },  { 45,   1, 3, 16, flat, unbanked },  { 45,   0, 3, 16, flat, unbanked },  { 46,  -1, 3, 16, flat, unbanked },
             { 46,  -2, 3, 16, flat, unbanked },  { 46,  -3, 3, 16, flat, unbanked },  { 46,  -4, 3, 16, flat, unbanked },  { 46,  -5, 3, 16, flat, unbanked },
             { 46,  -7, 0, 16, flat, unbanked },  { 47,  -9, 0, 16, flat, unbanked },  { 47, -10, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),  { 47, -11, 0, 16, flat, unbanked },  { 47, -12, 0, 16, flat, unbanked },  { 47, -13, 0, 16, flat, unbanked },
             { 47, -14, 0, 16, flat, unbanked },  { 48, -15, 0, 16, flat, unbanked },  { 48, -16, 0, 16, flat, unbanked },  { 48, -17, 0, 16, flat, unbanked },
             { 48, -18, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),  { 48, -29, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),  { 48, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_3, {
                          MINI_GOLF_STATE(Unk0), {  15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown), {  15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3), {  15,  24, 0, 24, flat, unbanked },
            {  14,  21, 0, 24, flat, unbanked }, {  14,  19, 0, 24, flat, unbanked }, {  14,  17, 0, 24, flat, unbanked }, {  15,  15, 0, 24, flat, unbanked },
            {  13,  13, 0, 24, flat, unbanked }, {  13,  11, 0, 24, flat, unbanked }, {  13,   9, 0, 24, flat, unbanked }, {  13,   7, 0, 24, flat, unbanked },
            {  13,   5, 0, 24, flat, unbanked }, {  13,   4, 0, 24, flat, unbanked }, {  12,   3, 0, 24, flat, unbanked }, {  12,   2, 0, 24, flat, unbanked },
            {  12,   1, 0, 24, flat, unbanked }, {  12,   0, 0, 24, flat, unbanked }, {  12,  -1, 0, 24, flat, unbanked }, {  12,  -2, 0, 24, flat, unbanked },
            {  12,  -1, 0, 24, flat, unbanked }, {  12,   0, 0, 24, flat, unbanked }, {  12,   1, 0, 24, flat, unbanked }, {  12,   2, 0, 24, flat, unbanked },
            {  12,   3, 0, 24, flat, unbanked }, {  12,   4, 0, 24, flat, unbanked }, {  12,   5, 0, 24, flat, unbanked }, {  11,   6, 0, 24, flat, unbanked },
            {  11,   7, 0, 24, flat, unbanked }, {  11,   8, 0, 24, flat, unbanked }, {  11,   9, 0, 24, flat, unbanked }, {  11,  10, 0, 24, flat, unbanked },
            {  11,  11, 0, 24, flat, unbanked }, {  11,  12, 0, 24, flat, unbanked }, {  11,  13, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3), {  11,  14, 0, 24, flat, unbanked }, {  11,  12, 0, 24, flat, unbanked }, {  11,   9, 0, 24, flat, unbanked },
            {  11,   6, 0, 24, flat, unbanked }, {  11,   3, 0, 24, flat, unbanked }, {  11,   0, 0, 24, flat, unbanked }, {  11,  -3, 0, 24, flat, unbanked },
            {  11,  -6, 0, 24, flat, unbanked }, {  11,  -9, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk), {  11, -12, 0, 24, flat, unbanked },
            {  11, -15, 0, 24, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown), {  15, -18, 0, 24, flat, unbanked }, {  15, -21, 0, 24, flat, unbanked },
            {  15, -24, 0, 24, flat, unbanked }, {  16, -25, 0, 24, flat, unbanked }, {  18, -26, 0, 24, flat, unbanked }, {  20, -27, 0, 24, flat, unbanked },
            {  22, -28, 0, 24, flat, unbanked }, {  24, -26, 0, 24, flat, unbanked }, {  26, -24, 1, 24, flat, unbanked }, {  28, -22, 1, 24, flat, unbanked },
            {  29, -20, 1, 24, flat, unbanked }, {  29, -18, 1, 24, flat, unbanked }, {  28, -16, 2, 24, flat, unbanked }, {  28, -14, 2, 24, flat, unbanked },
            {  26, -14, 2, 24, flat, unbanked }, {  24, -13, 2, 24, flat, unbanked }, {  22, -12, 3, 24, flat, unbanked }, {  20, -12, 3, 24, flat, unbanked },
            {  18, -11, 3, 24, flat, unbanked }, {  16, -12, 3, 24, flat, unbanked }, {  14, -12, 3, 24, flat, unbanked }, {  12, -12, 3, 24, flat, unbanked },
            {  10, -13, 3, 24, flat, unbanked }, {   8, -13, 3, 24, flat, unbanked }, {   6, -13, 3, 24, flat, unbanked }, {   4, -14, 3, 24, flat, unbanked },
            {   2, -14, 3, 24, flat, unbanked }, {   1, -14, 3, 24, flat, unbanked }, {   0, -14, 3, 24, flat, unbanked }, {  -1, -15, 3, 24, flat, unbanked },
            {  -2, -15, 3, 24, flat, unbanked }, {  -3, -15, 3, 24, flat, unbanked }, {  -4, -15, 3, 24, flat, unbanked }, {  -5, -15, 3, 24, flat, unbanked },
            {  -7, -15, 0, 24, flat, unbanked }, {  -9, -16, 0, 24, flat, unbanked }, { -10, -16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3), { -11, -16, 0, 24, flat, unbanked }, { -12, -16, 0, 24, flat, unbanked }, { -13, -16, 0, 24, flat, unbanked },
            { -14, -16, 0, 24, flat, unbanked }, { -15, -17, 0, 24, flat, unbanked }, { -16, -17, 0, 24, flat, unbanked }, { -17, -17, 0, 24, flat, unbanked },
            { -18, -17, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2), { -29, -17, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3), { -29, -17, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_0, {
                          MINI_GOLF_STATE(Unk0), {  31,  16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  26,  16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown), {  26,  16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk3), {  24,  16, 0,  0, flat, unbanked },
            {  21,  15, 0,  0, flat, unbanked }, {  19,  15, 0,  0, flat, unbanked }, {  17,  15, 0,  0, flat, unbanked }, {  15,  16, 0,  0, flat, unbanked },
            {  13,  14, 0,  0, flat, unbanked }, {  11,  14, 0,  0, flat, unbanked }, {   9,  14, 0,  0, flat, unbanked }, {   7,  14, 0,  0, flat, unbanked },
            {   5,  14, 0,  0, flat, unbanked }, {   4,  14, 0,  0, flat, unbanked }, {   3,  13, 0,  0, flat, unbanked }, {   2,  13, 0,  0, flat, unbanked },
            {   1,  13, 0,  0, flat, unbanked }, {   0,  13, 0,  0, flat, unbanked }, {  -1,  13, 0,  0, flat, unbanked }, {  -2,  13, 0,  0, flat, unbanked },
            {  -1,  13, 0,  0, flat, unbanked }, {   0,  13, 0,  0, flat, unbanked }, {   1,  13, 0,  0, flat, unbanked }, {   2,  13, 0,  0, flat, unbanked },
            {   3,  13, 0,  0, flat, unbanked }, {   4,  13, 0,  0, flat, unbanked }, {   5,  13, 0,  0, flat, unbanked }, {   6,  12, 0,  0, flat, unbanked },
            {   7,  12, 0,  0, flat, unbanked }, {   8,  12, 0,  0, flat, unbanked }, {   9,  12, 0,  0, flat, unbanked }, {  10,  12, 0,  0, flat, unbanked },
            {  11,  12, 0,  0, flat, unbanked }, {  12,  12, 0,  0, flat, unbanked }, {  13,  12, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3), {  14,  12, 0,  0, flat, unbanked }, {  12,  12, 0,  0, flat, unbanked }, {   9,  12, 0,  0, flat, unbanked },
            {   6,  12, 0,  0, flat, unbanked }, {   3,  12, 0,  0, flat, unbanked }, {   0,  12, 0,  0, flat, unbanked }, {  -3,  12, 0,  0, flat, unbanked },
            {  -6,  12, 0,  0, flat, unbanked }, {  -9,  12, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk), { -12,  12, 0,  0, flat, unbanked },
            { -15,  12, 0,  0, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown), { -18,  16, 0,  0, flat, unbanked }, { -21,  16, 0,  0, flat, unbanked },
            { -24,  16, 0,  0, flat, unbanked }, { -25,  17, 0,  0, flat, unbanked }, { -26,  19, 0,  0, flat, unbanked }, { -27,  21, 0,  0, flat, unbanked },
            { -28,  23, 0,  0, flat, unbanked }, { -26,  25, 0,  0, flat, unbanked }, { -24,  27, 1,  0, flat, unbanked }, { -22,  29, 1,  0, flat, unbanked },
            { -20,  30, 1,  0, flat, unbanked }, { -18,  30, 1,  0, flat, unbanked }, { -16,  29, 2,  0, flat, unbanked }, { -14,  29, 2,  0, flat, unbanked },
            { -14,  27, 2,  0, flat, unbanked }, { -13,  25, 2,  0, flat, unbanked }, { -12,  23, 3,  0, flat, unbanked }, { -12,  21, 3,  0, flat, unbanked },
            { -11,  19, 3,  0, flat, unbanked }, { -12,  17, 3,  0, flat, unbanked }, { -12,  15, 3,  0, flat, unbanked }, { -12,  13, 3,  0, flat, unbanked },
            { -13,  11, 3,  0, flat, unbanked }, { -13,   9, 3,  0, flat, unbanked }, { -13,   7, 3,  0, flat, unbanked }, { -14,   5, 3,  0, flat, unbanked },
            { -14,   3, 3,  0, flat, unbanked }, { -14,   2, 3,  0, flat, unbanked }, { -14,   1, 3,  0, flat, unbanked }, { -15,   0, 3,  0, flat, unbanked },
            { -15,  -1, 3,  0, flat, unbanked }, { -15,  -2, 3,  0, flat, unbanked }, { -15,  -3, 3,  0, flat, unbanked }, { -15,  -4, 3,  0, flat, unbanked },
            { -15,  -6, 0,  0, flat, unbanked }, { -16,  -8, 0,  0, flat, unbanked }, { -16,  -9, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3), { -16, -10, 0,  0, flat, unbanked }, { -16, -11, 0,  0, flat, unbanked }, { -16, -12, 0,  0, flat, unbanked },
            { -16, -13, 0,  0, flat, unbanked }, { -17, -14, 0,  0, flat, unbanked }, { -17, -15, 0,  0, flat, unbanked }, { -17, -16, 0,  0, flat, unbanked },
            { -17, -17, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2), { -17, -28, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3), { -17, -28, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_1, {
                          MINI_GOLF_STATE(Unk0),   {  16,  0, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  16,  5, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  16,  5, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  16,  7, 0, 8, flat, unbanked },
              {  15, 10, 0, 8, flat, unbanked },   {  15, 12, 0, 8, flat, unbanked },   {  15, 14, 0, 8, flat, unbanked },   {  16, 16, 0, 8, flat, unbanked },
              {  14, 18, 0, 8, flat, unbanked },   {  14, 20, 0, 8, flat, unbanked },   {  14, 22, 0, 8, flat, unbanked },   {  14, 24, 0, 8, flat, unbanked },
              {  14, 26, 0, 8, flat, unbanked },   {  14, 27, 0, 8, flat, unbanked },   {  13, 28, 0, 8, flat, unbanked },   {  13, 29, 0, 8, flat, unbanked },
              {  13, 30, 0, 8, flat, unbanked },   {  13, 31, 0, 8, flat, unbanked },   {  13, 32, 0, 8, flat, unbanked },   {  13, 33, 0, 8, flat, unbanked },
              {  13, 32, 0, 8, flat, unbanked },   {  13, 31, 0, 8, flat, unbanked },   {  13, 30, 0, 8, flat, unbanked },   {  13, 29, 0, 8, flat, unbanked },
              {  13, 28, 0, 8, flat, unbanked },   {  13, 27, 0, 8, flat, unbanked },   {  13, 26, 0, 8, flat, unbanked },   {  12, 25, 0, 8, flat, unbanked },
              {  12, 24, 0, 8, flat, unbanked },   {  12, 23, 0, 8, flat, unbanked },   {  12, 22, 0, 8, flat, unbanked },   {  12, 21, 0, 8, flat, unbanked },
              {  12, 20, 0, 8, flat, unbanked },   {  12, 19, 0, 8, flat, unbanked },   {  12, 18, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   {  12, 17, 0, 8, flat, unbanked },   {  12, 19, 0, 8, flat, unbanked },   {  12, 22, 0, 8, flat, unbanked },
              {  12, 25, 0, 8, flat, unbanked },   {  12, 28, 0, 8, flat, unbanked },   {  12, 31, 0, 8, flat, unbanked },   {  12, 34, 0, 8, flat, unbanked },
              {  12, 37, 0, 8, flat, unbanked },   {  12, 40, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   {  12, 43, 0, 8, flat, unbanked },
              {  12, 46, 0, 8, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   {  16, 49, 0, 8, flat, unbanked },   {  16, 52, 0, 8, flat, unbanked },
              {  16, 55, 0, 8, flat, unbanked },   {  17, 56, 0, 8, flat, unbanked },   {  19, 57, 0, 8, flat, unbanked },   {  21, 58, 0, 8, flat, unbanked },
              {  23, 59, 0, 8, flat, unbanked },   {  25, 57, 0, 8, flat, unbanked },   {  27, 55, 1, 8, flat, unbanked },   {  29, 53, 1, 8, flat, unbanked },
              {  30, 51, 1, 8, flat, unbanked },   {  30, 49, 1, 8, flat, unbanked },   {  29, 47, 2, 8, flat, unbanked },   {  29, 45, 2, 8, flat, unbanked },
              {  27, 45, 2, 8, flat, unbanked },   {  25, 44, 2, 8, flat, unbanked },   {  23, 43, 3, 8, flat, unbanked },   {  21, 43, 3, 8, flat, unbanked },
              {  19, 42, 3, 8, flat, unbanked },   {  17, 43, 3, 8, flat, unbanked },   {  15, 43, 3, 8, flat, unbanked },   {  13, 43, 3, 8, flat, unbanked },
              {  11, 44, 3, 8, flat, unbanked },   {   9, 44, 3, 8, flat, unbanked },   {   7, 44, 3, 8, flat, unbanked },   {   5, 45, 3, 8, flat, unbanked },
              {   3, 45, 3, 8, flat, unbanked },   {   2, 45, 3, 8, flat, unbanked },   {   1, 45, 3, 8, flat, unbanked },   {   0, 46, 3, 8, flat, unbanked },
              {  -1, 46, 3, 8, flat, unbanked },   {  -2, 46, 3, 8, flat, unbanked },   {  -3, 46, 3, 8, flat, unbanked },   {  -4, 46, 3, 8, flat, unbanked },
              {  -6, 46, 0, 8, flat, unbanked },   {  -8, 47, 0, 8, flat, unbanked },   {  -9, 47, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   { -10, 47, 0, 8, flat, unbanked },   { -11, 47, 0, 8, flat, unbanked },   { -12, 47, 0, 8, flat, unbanked },
              { -13, 47, 0, 8, flat, unbanked },   { -14, 48, 0, 8, flat, unbanked },   { -15, 48, 0, 8, flat, unbanked },   { -16, 48, 0, 8, flat, unbanked },
              { -17, 48, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { -28, 48, 0, 0, flat, unbanked },
                          MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),   { -28, 48, 0, 0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  5, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  5, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  7, 15, 0, 16, flat, unbanked },
              { 10, 16, 0, 16, flat, unbanked },   { 12, 16, 0, 16, flat, unbanked },   { 14, 16, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },
              { 18, 17, 0, 16, flat, unbanked },   { 20, 17, 0, 16, flat, unbanked },   { 22, 17, 0, 16, flat, unbanked },   { 24, 17, 0, 16, flat, unbanked },
              { 26, 17, 0, 16, flat, unbanked },   { 27, 17, 0, 16, flat, unbanked },   { 28, 18, 0, 16, flat, unbanked },   { 29, 18, 0, 16, flat, unbanked },
              { 30, 18, 0, 16, flat, unbanked },   { 31, 18, 0, 16, flat, unbanked },   { 32, 18, 0, 16, flat, unbanked },   { 33, 18, 0, 16, flat, unbanked },
              { 32, 18, 0, 16, flat, unbanked },   { 31, 18, 0, 16, flat, unbanked },   { 30, 18, 0, 16, flat, unbanked },   { 29, 18, 0, 16, flat, unbanked },
              { 28, 18, 0, 16, flat, unbanked },   { 27, 18, 0, 16, flat, unbanked },   { 26, 18, 0, 16, flat, unbanked },   { 25, 19, 0, 16, flat, unbanked },
              { 24, 19, 0, 16, flat, unbanked },   { 23, 19, 0, 16, flat, unbanked },   { 22, 19, 0, 16, flat, unbanked },   { 21, 19, 0, 16, flat, unbanked },
              { 20, 19, 0, 16, flat, unbanked },   { 19, 19, 0, 16, flat, unbanked },   { 18, 19, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   { 17, 19, 0, 16, flat, unbanked },   { 19, 19, 0, 16, flat, unbanked },   { 22, 19, 0, 16, flat, unbanked },
              { 25, 19, 0, 16, flat, unbanked },   { 28, 19, 0, 16, flat, unbanked },   { 31, 19, 0, 16, flat, unbanked },   { 34, 19, 0, 16, flat, unbanked },
              { 37, 19, 0, 16, flat, unbanked },   { 40, 19, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 43, 19, 0, 16, flat, unbanked },
              { 46, 19, 0, 16, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { 49, 15, 0, 16, flat, unbanked },   { 52, 15, 0, 16, flat, unbanked },
              { 55, 15, 0, 16, flat, unbanked },   { 56, 14, 0, 16, flat, unbanked },   { 57, 12, 0, 16, flat, unbanked },   { 58, 10, 0, 16, flat, unbanked },
              { 59,  8, 0, 16, flat, unbanked },   { 57,  6, 0, 16, flat, unbanked },   { 55,  4, 1, 16, flat, unbanked },   { 53,  2, 1, 16, flat, unbanked },
              { 51,  1, 1, 16, flat, unbanked },   { 49,  1, 1, 16, flat, unbanked },   { 47,  2, 2, 16, flat, unbanked },   { 45,  2, 2, 16, flat, unbanked },
              { 45,  4, 2, 16, flat, unbanked },   { 44,  6, 2, 16, flat, unbanked },   { 43,  8, 3, 16, flat, unbanked },   { 43, 10, 3, 16, flat, unbanked },
              { 42, 12, 3, 16, flat, unbanked },   { 43, 14, 3, 16, flat, unbanked },   { 43, 16, 3, 16, flat, unbanked },   { 43, 18, 3, 16, flat, unbanked },
              { 44, 20, 3, 16, flat, unbanked },   { 44, 22, 3, 16, flat, unbanked },   { 44, 24, 3, 16, flat, unbanked },   { 45, 26, 3, 16, flat, unbanked },
              { 45, 28, 3, 16, flat, unbanked },   { 45, 29, 3, 16, flat, unbanked },   { 45, 30, 3, 16, flat, unbanked },   { 46, 31, 3, 16, flat, unbanked },
              { 46, 32, 3, 16, flat, unbanked },   { 46, 33, 3, 16, flat, unbanked },   { 46, 34, 3, 16, flat, unbanked },   { 46, 35, 3, 16, flat, unbanked },
              { 46, 37, 0, 16, flat, unbanked },   { 47, 39, 0, 16, flat, unbanked },   { 47, 40, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   { 47, 41, 0, 16, flat, unbanked },   { 47, 42, 0, 16, flat, unbanked },   { 47, 43, 0, 16, flat, unbanked },
              { 47, 44, 0, 16, flat, unbanked },   { 48, 45, 0, 16, flat, unbanked },   { 48, 46, 0, 16, flat, unbanked },   { 48, 47, 0, 16, flat, unbanked },
              { 48, 48, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { 48, 59, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),   { 48, 59, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15,  24, 0, 24, flat, unbanked },
             { 16,  21, 0, 24, flat, unbanked },  { 16,  19, 0, 24, flat, unbanked },  { 16,  17, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },
             { 17,  13, 0, 24, flat, unbanked },  { 17,  11, 0, 24, flat, unbanked },  { 17,   9, 0, 24, flat, unbanked },  { 17,   7, 0, 24, flat, unbanked },
             { 17,   5, 0, 24, flat, unbanked },  { 17,   4, 0, 24, flat, unbanked },  { 18,   3, 0, 24, flat, unbanked },  { 18,   2, 0, 24, flat, unbanked },
             { 18,   1, 0, 24, flat, unbanked },  { 18,   0, 0, 24, flat, unbanked },  { 18,  -1, 0, 24, flat, unbanked },  { 18,  -2, 0, 24, flat, unbanked },
             { 18,  -1, 0, 24, flat, unbanked },  { 18,   0, 0, 24, flat, unbanked },  { 18,   1, 0, 24, flat, unbanked },  { 18,   2, 0, 24, flat, unbanked },
             { 18,   3, 0, 24, flat, unbanked },  { 18,   4, 0, 24, flat, unbanked },  { 18,   5, 0, 24, flat, unbanked },  { 19,   6, 0, 24, flat, unbanked },
             { 19,   7, 0, 24, flat, unbanked },  { 19,   8, 0, 24, flat, unbanked },  { 19,   9, 0, 24, flat, unbanked },  { 19,  10, 0, 24, flat, unbanked },
             { 19,  11, 0, 24, flat, unbanked },  { 19,  12, 0, 24, flat, unbanked },  { 19,  13, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),  { 19,  14, 0, 24, flat, unbanked },  { 19,  12, 0, 24, flat, unbanked },  { 19,   9, 0, 24, flat, unbanked },
             { 19,   6, 0, 24, flat, unbanked },  { 19,   3, 0, 24, flat, unbanked },  { 19,   0, 0, 24, flat, unbanked },  { 19,  -3, 0, 24, flat, unbanked },
             { 19,  -6, 0, 24, flat, unbanked },  { 19,  -9, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 19, -12, 0, 24, flat, unbanked },
             { 19, -15, 0, 24, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),  { 15, -18, 0, 24, flat, unbanked },  { 15, -21, 0, 24, flat, unbanked },
             { 15, -24, 0, 24, flat, unbanked },  { 14, -25, 0, 24, flat, unbanked },  { 12, -26, 0, 24, flat, unbanked },  { 10, -27, 0, 24, flat, unbanked },
             {  8, -28, 0, 24, flat, unbanked },  {  6, -26, 0, 24, flat, unbanked },  {  4, -24, 1, 24, flat, unbanked },  {  2, -22, 1, 24, flat, unbanked },
             {  1, -20, 1, 24, flat, unbanked },  {  1, -18, 1, 24, flat, unbanked },  {  2, -16, 2, 24, flat, unbanked },  {  2, -14, 2, 24, flat, unbanked },
             {  4, -14, 2, 24, flat, unbanked },  {  6, -13, 2, 24, flat, unbanked },  {  8, -12, 3, 24, flat, unbanked },  { 10, -12, 3, 24, flat, unbanked },
             { 12, -11, 3, 24, flat, unbanked },  { 14, -12, 3, 24, flat, unbanked },  { 16, -12, 3, 24, flat, unbanked },  { 18, -12, 3, 24, flat, unbanked },
             { 20, -13, 3, 24, flat, unbanked },  { 22, -13, 3, 24, flat, unbanked },  { 24, -13, 3, 24, flat, unbanked },  { 26, -14, 3, 24, flat, unbanked },
             { 28, -14, 3, 24, flat, unbanked },  { 29, -14, 3, 24, flat, unbanked },  { 30, -14, 3, 24, flat, unbanked },  { 31, -15, 3, 24, flat, unbanked },
             { 32, -15, 3, 24, flat, unbanked },  { 33, -15, 3, 24, flat, unbanked },  { 34, -15, 3, 24, flat, unbanked },  { 35, -15, 3, 24, flat, unbanked },
             { 37, -15, 0, 24, flat, unbanked },  { 39, -16, 0, 24, flat, unbanked },  { 40, -16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),  { 41, -16, 0, 24, flat, unbanked },  { 42, -16, 0, 24, flat, unbanked },  { 43, -16, 0, 24, flat, unbanked },
             { 44, -16, 0, 24, flat, unbanked },  { 45, -17, 0, 24, flat, unbanked },  { 46, -17, 0, 24, flat, unbanked },  { 47, -17, 0, 24, flat, unbanked },
             { 48, -17, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),  { 59, -17, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),  { 59, -17, 0, 16, flat, unbanked },
        })

        static constexpr const VehicleInfoList * BallPathB[] = {
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3,
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // EndStation
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // BeginStation
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // MiddleStation
            &Up25_0,                  &Up25_1,                  &Up25_2,                  &Up25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60
            &FlatToUp25_0,            &FlatToUp25_1,            &FlatToUp25_2,            &FlatToUp25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp25
            &Up25ToFlat_0,            &Up25ToFlat_1,            &Up25ToFlat_2,            &Up25ToFlat_3,
            &Down25_0,                &Down25_1,                &Down25_2,                &Down25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60
            &FlatToDown25_0,          &FlatToDown25_1,          &FlatToDown25_2,          &FlatToDown25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown25
            &Down25ToFlat_0,          &Down25ToFlat_1,          &Down25ToFlat_2,          &Down25ToFlat_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BankedLeftQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BankedRightQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendLeft
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendRight
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftVerticalLoop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightVerticalLoop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankedQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankedQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesDown25
            &LeftQuarterTurn1Tile_0,  &LeftQuarterTurn1Tile_1,  &LeftQuarterTurn1Tile_2,  &LeftQuarterTurn1Tile_3,
            &RightQuarterTurn1Tile_0, &RightQuarterTurn1Tile_1, &RightQuarterTurn1Tile_2, &RightQuarterTurn1Tile_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftTwistDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightTwistDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftTwistUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightTwistUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // TowerBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // TowerSection
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToUp60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToFlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToDown60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToFlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendLeftCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendRightCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixUpSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixUpSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixDownSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixDownSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixUpLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixUpLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixDownLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixDownLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn1TileUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn1TileUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn1TileDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn1TileDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Brakes
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Booster
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Maze
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterBankedHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterBankedHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterBankedHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterBankedHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25LeftBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25RightBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Waterfall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Rapids
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // OnRidePhoto
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25LeftBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25RightBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Watersplash
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp60LongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToFlatLongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Whirlpool
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToFlatLongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown60LongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // CableLiftHill
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // ReverseFreefallSlope
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // ReverseFreefallVertical
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down90ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up90ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BrakeForDrop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthBankToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthBankToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthBankToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthBankToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60ToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60ToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LogFlumeReverser
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SpinningTunnel
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBarrelRollUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBarrelRollUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBarrelRollDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBarrelRollDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToLeftQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToRightQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesDown25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesDown25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // PoweredLift
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftLargeHalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightLargeHalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftLargeHalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightLargeHalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerTwistUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerTwistUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerTwistDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerTwistDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlyerHalfLoopUninvertedUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlyerHalfLoopInvertedDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HeartLineTransferUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HeartLineTransferDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHeartLineRoll
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHeartLineRoll
            &MinigolfHoleA_0,         &MinigolfHoleA_1,         &MinigolfHoleA_2,         &MinigolfHoleA_3,
            &MinigolfHoleB_0,         &MinigolfHoleB_1,         &MinigolfHoleB_2,         &MinigolfHoleB_3,
            &MinigolfHoleC_0,         &MinigolfHoleC_1,         &MinigolfHoleC_2,         &MinigolfHoleC_3,
            &MinigolfHoleD_0,         &MinigolfHoleD_1,         &MinigolfHoleD_2,         &MinigolfHoleD_3,
            &MinigolfHoleE_0,         &MinigolfHoleE_1,         &MinigolfHoleE_2,         &MinigolfHoleE_3,
        };
        // clang-format on
        static_assert(std::size(BallPathB) == kVehicleTrackSubpositionSizeMinigolf);
    } // namespace BallPathB

    namespace PlayerPathC
    {
        // clang-format off
        CREATE_VEHICLE_INFO(MinigolfHoleA_0, {
                          MINI_GOLF_STATE(Unk0),  {  31, 16, 0,  0, flat, unbanked },  {  29, 16, 0,  0, flat, unbanked },  {  27, 16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  27, 16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  27, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  27, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  27, 16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  25, 16, 0,  0, flat, unbanked },  {  23, 16, 0,  0, flat, unbanked },  {  21, 16, 0,  0, flat, unbanked },
             {  19, 16, 0,  0, flat, unbanked },  {  18, 16, 0,  0, flat, unbanked },  {  15, 16, 0,  0, flat, unbanked },  {  13, 16, 0,  0, flat, unbanked },
             {  11, 16, 0,  0, flat, unbanked },  {   9, 16, 0,  0, flat, unbanked },  {   7, 16, 0,  0, flat, unbanked },  {   5, 16, 0,  0, flat, unbanked },
             {   3, 16, 0,  0, flat, unbanked },  {   1, 16, 0,  0, flat, unbanked },  {  -1, 16, 0,  0, flat, unbanked },  {  -3, 16, 0,  0, flat, unbanked },
             {  -5, 16, 0,  0, flat, unbanked },  {  -7, 16, 0,  0, flat, unbanked },  {  -9, 16, 0,  0, flat, unbanked },  {  -9, 14, 0, 24, flat, unbanked },
             {  -9, 12, 0, 24, flat, unbanked },  {  -9, 11, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),  {  -9, 11, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
             {  -9, 11, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  -9, 11, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             {  -9, 12, 0,  0, flat, unbanked },  { -11, 12, 0,  0, flat, unbanked },  { -13, 12, 0,  0, flat, unbanked },  { -15, 12, 0,  0, flat, unbanked },
             { -17, 12, 0,  0, flat, unbanked },  { -19, 12, 0,  0, flat, unbanked },  { -21, 12, 0,  0, flat, unbanked },  { -23, 12, 0,  0, flat, unbanked },
             { -25, 12, 0,  0, flat, unbanked },  { -25, 14, 0,  8, flat, unbanked },  { -25, 16, 0,  8, flat, unbanked },  { -25, 18, 0,  8, flat, unbanked },
                 MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { -25, 18, 0,  8, flat, unbanked },
                  MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { -25, 18, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),
             { -25, 18, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { -23, 18, 0, 16, flat, unbanked },  { -21, 18, 0, 16, flat, unbanked },
             { -19, 18, 0, 16, flat, unbanked },  { -17, 18, 0, 16, flat, unbanked },  { -15, 18, 0, 16, flat, unbanked },  { -15, 16, 0,  8, flat, unbanked },
                MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),  { -15, 16, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk6),  { -17, 16, 0,  0, flat, unbanked },  { -19, 16, 0,  0, flat, unbanked },  { -21, 16, 0,  0, flat, unbanked },
             { -23, 16, 0,  0, flat, unbanked },  { -25, 16, 0,  0, flat, unbanked },  { -27, 16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk3),
             { -29, 16, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_1, {
                          MINI_GOLF_STATE(Unk0),   { 16,  0, 0,  8, flat, unbanked },   { 16,  2, 0,  8, flat, unbanked },   { 16,  4, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 16,  4, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   { 16,  6, 0,  8, flat, unbanked },   { 16,  8, 0,  8, flat, unbanked },   { 16, 10, 0,  8, flat, unbanked },
              { 16, 12, 0,  8, flat, unbanked },   { 16, 13, 0,  8, flat, unbanked },   { 16, 16, 0,  8, flat, unbanked },   { 16, 18, 0,  8, flat, unbanked },
              { 16, 20, 0,  8, flat, unbanked },   { 16, 22, 0,  8, flat, unbanked },   { 16, 24, 0,  8, flat, unbanked },   { 16, 26, 0,  8, flat, unbanked },
              { 16, 28, 0,  8, flat, unbanked },   { 16, 30, 0,  8, flat, unbanked },   { 16, 32, 0,  8, flat, unbanked },   { 16, 34, 0,  8, flat, unbanked },
              { 16, 36, 0,  8, flat, unbanked },   { 16, 38, 0,  8, flat, unbanked },   { 16, 40, 0,  8, flat, unbanked },   { 14, 40, 0,  0, flat, unbanked },
              { 12, 40, 0,  0, flat, unbanked },   { 11, 40, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),   { 11, 40, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
              { 11, 40, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 11, 40, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { 12, 40, 0,  8, flat, unbanked },   { 12, 42, 0,  8, flat, unbanked },   { 12, 44, 0,  8, flat, unbanked },   { 12, 46, 0,  8, flat, unbanked },
              { 12, 48, 0,  8, flat, unbanked },   { 12, 50, 0,  8, flat, unbanked },   { 12, 52, 0,  8, flat, unbanked },   { 12, 54, 0,  8, flat, unbanked },
              { 12, 56, 0,  8, flat, unbanked },   { 14, 56, 0, 16, flat, unbanked },   { 16, 56, 0, 16, flat, unbanked },   { 18, 56, 0, 16, flat, unbanked },
                 MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 18, 56, 0, 16, flat, unbanked },
                  MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 18, 56, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),
              { 18, 56, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 18, 54, 0, 24, flat, unbanked },   { 18, 52, 0, 24, flat, unbanked },
              { 18, 50, 0, 24, flat, unbanked },   { 18, 48, 0, 24, flat, unbanked },   { 18, 46, 0, 24, flat, unbanked },   { 16, 46, 0, 16, flat, unbanked },
                MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),   { 16, 46, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk6),   { 16, 48, 0,  8, flat, unbanked },   { 16, 50, 0,  8, flat, unbanked },   { 16, 52, 0,  8, flat, unbanked },
              { 16, 54, 0,  8, flat, unbanked },   { 16, 56, 0,  8, flat, unbanked },   { 16, 58, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk3),
              { 16, 60, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },   {  2, 15, 0, 16, flat, unbanked },   {  4, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   {  4, 15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   {  4, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   {  4, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  4, 15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {  6, 15, 0, 16, flat, unbanked },   {  8, 15, 0, 16, flat, unbanked },   { 10, 15, 0, 16, flat, unbanked },
              { 12, 15, 0, 16, flat, unbanked },   { 13, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 18, 15, 0, 16, flat, unbanked },
              { 20, 15, 0, 16, flat, unbanked },   { 22, 15, 0, 16, flat, unbanked },   { 24, 15, 0, 16, flat, unbanked },   { 26, 15, 0, 16, flat, unbanked },
              { 28, 15, 0, 16, flat, unbanked },   { 30, 15, 0, 16, flat, unbanked },   { 32, 15, 0, 16, flat, unbanked },   { 34, 15, 0, 16, flat, unbanked },
              { 36, 15, 0, 16, flat, unbanked },   { 38, 15, 0, 16, flat, unbanked },   { 40, 15, 0, 16, flat, unbanked },   { 40, 17, 0,  8, flat, unbanked },
              { 40, 19, 0,  8, flat, unbanked },   { 40, 20, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),   { 40, 20, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
              { 40, 20, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 40, 20, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { 40, 19, 0, 16, flat, unbanked },   { 42, 19, 0, 16, flat, unbanked },   { 44, 19, 0, 16, flat, unbanked },   { 46, 19, 0, 16, flat, unbanked },
              { 48, 19, 0, 16, flat, unbanked },   { 50, 19, 0, 16, flat, unbanked },   { 52, 19, 0, 16, flat, unbanked },   { 54, 19, 0, 16, flat, unbanked },
              { 56, 19, 0, 16, flat, unbanked },   { 56, 17, 0, 24, flat, unbanked },   { 56, 15, 0, 24, flat, unbanked },   { 56, 13, 0, 24, flat, unbanked },
                 MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 56, 13, 0, 24, flat, unbanked },
                  MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 56, 13, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),
              { 56, 13, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 54, 13, 0,  0, flat, unbanked },   { 52, 13, 0,  0, flat, unbanked },
              { 50, 13, 0,  0, flat, unbanked },   { 48, 13, 0,  0, flat, unbanked },   { 46, 13, 0,  0, flat, unbanked },   { 46, 15, 0, 24, flat, unbanked },
                MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),   { 46, 15, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk6),   { 48, 15, 0, 16, flat, unbanked },   { 50, 15, 0, 16, flat, unbanked },   { 52, 15, 0, 16, flat, unbanked },
              { 54, 15, 0, 16, flat, unbanked },   { 56, 15, 0, 16, flat, unbanked },   { 58, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),
              { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },  { 15,  29, 0, 24, flat, unbanked },  { 15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 15,  25, 0, 24, flat, unbanked },  { 15,  23, 0, 24, flat, unbanked },  { 15,  21, 0, 24, flat, unbanked },
             { 15,  19, 0, 24, flat, unbanked },  { 15,  18, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 15,  13, 0, 24, flat, unbanked },
             { 15,  11, 0, 24, flat, unbanked },  { 15,   9, 0, 24, flat, unbanked },  { 15,   7, 0, 24, flat, unbanked },  { 15,   5, 0, 24, flat, unbanked },
             { 15,   3, 0, 24, flat, unbanked },  { 15,   1, 0, 24, flat, unbanked },  { 15,  -1, 0, 24, flat, unbanked },  { 15,  -3, 0, 24, flat, unbanked },
             { 15,  -5, 0, 24, flat, unbanked },  { 15,  -7, 0, 24, flat, unbanked },  { 15,  -9, 0, 24, flat, unbanked },  { 17,  -9, 0, 16, flat, unbanked },
             { 19,  -9, 0, 16, flat, unbanked },  { 20,  -9, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),  { 20,  -9, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
             { 20,  -9, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 20,  -9, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             { 19,  -9, 0, 24, flat, unbanked },  { 19, -11, 0, 24, flat, unbanked },  { 19, -13, 0, 24, flat, unbanked },  { 19, -15, 0, 24, flat, unbanked },
             { 19, -17, 0, 24, flat, unbanked },  { 19, -19, 0, 24, flat, unbanked },  { 19, -21, 0, 24, flat, unbanked },  { 19, -23, 0, 24, flat, unbanked },
             { 19, -25, 0, 24, flat, unbanked },  { 17, -25, 0,  0, flat, unbanked },  { 15, -25, 0,  0, flat, unbanked },  { 13, -25, 0,  0, flat, unbanked },
                 MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 13, -25, 0,  0, flat, unbanked },
                  MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { 13, -25, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),
             { 13, -25, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 13, -23, 0,  8, flat, unbanked },  { 13, -21, 0,  8, flat, unbanked },
             { 13, -19, 0,  8, flat, unbanked },  { 13, -17, 0,  8, flat, unbanked },  { 13, -15, 0,  8, flat, unbanked },  { 15, -15, 0,  0, flat, unbanked },
                MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),  { 15, -15, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk6),  { 15, -17, 0, 24, flat, unbanked },  { 15, -19, 0, 24, flat, unbanked },  { 15, -21, 0, 24, flat, unbanked },
             { 15, -23, 0, 24, flat, unbanked },  { 15, -25, 0, 24, flat, unbanked },  { 15, -27, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),
             { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_0, {
                          MINI_GOLF_STATE(Unk0),  {  31, 16, 0,  0, flat, unbanked },  {  29, 16, 0,  0, flat, unbanked },  {  27, 16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  27, 16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  27, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  27, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  27, 16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  25, 16, 0,  0, flat, unbanked },  {  23, 16, 0,  0, flat, unbanked },  {  21, 16, 0,  0, flat, unbanked },
             {  19, 16, 0,  0, flat, unbanked },  {  17, 16, 0,  0, flat, unbanked },  {  15, 16, 0,  0, flat, unbanked },  {  13, 16, 0,  0, flat, unbanked },
             {  11, 16, 0,  0, flat, unbanked },  {   9, 16, 0,  0, flat, unbanked },  {   7, 16, 0,  0, flat, unbanked },  {   5, 16, 0,  0, flat, unbanked },
             {   3, 16, 0,  0, flat, unbanked },  {   1, 16, 0,  0, flat, unbanked },  {  -1, 16, 0,  0, flat, unbanked },  {  -3, 16, 0,  0, flat, unbanked },
             {  -5, 16, 0,  0, flat, unbanked },  {  -7, 16, 0,  0, flat, unbanked },  {  -9, 16, 0,  0, flat, unbanked },  { -11, 16, 0,  0, flat, unbanked },
             { -13, 16, 0,  0, flat, unbanked },  { -15, 16, 0,  0, flat, unbanked },  { -17, 16, 0,  0, flat, unbanked },  { -19, 16, 0,  0, flat, unbanked },
             { -19, 14, 0, 24, flat, unbanked },  { -19, 12, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),  { -19, 12, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
             { -19, 12, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { -19, 12, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             { -19, 14, 0,  8, flat, unbanked },  { -19, 16, 0,  8, flat, unbanked },  { -21, 16, 0,  0, flat, unbanked },  { -23, 16, 0,  0, flat, unbanked },
             { -25, 16, 0,  0, flat, unbanked },  { -27, 16, 0,  0, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
             { -27, 16, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { -27, 16, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),  { -29, 16, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_1, {
                          MINI_GOLF_STATE(Unk0),   { 16,  0, 0,  8, flat, unbanked },   { 16,  2, 0,  8, flat, unbanked },   { 16,  4, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 16,  4, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   { 16,  6, 0,  8, flat, unbanked },   { 16,  8, 0,  8, flat, unbanked },   { 16, 10, 0,  8, flat, unbanked },
              { 16, 12, 0,  8, flat, unbanked },   { 16, 14, 0,  8, flat, unbanked },   { 16, 16, 0,  8, flat, unbanked },   { 16, 18, 0,  8, flat, unbanked },
              { 16, 20, 0,  8, flat, unbanked },   { 16, 22, 0,  8, flat, unbanked },   { 16, 24, 0,  8, flat, unbanked },   { 16, 26, 0,  8, flat, unbanked },
              { 16, 28, 0,  8, flat, unbanked },   { 16, 30, 0,  8, flat, unbanked },   { 16, 32, 0,  8, flat, unbanked },   { 16, 34, 0,  8, flat, unbanked },
              { 16, 36, 0,  8, flat, unbanked },   { 16, 38, 0,  8, flat, unbanked },   { 16, 40, 0,  8, flat, unbanked },   { 16, 42, 0,  8, flat, unbanked },
              { 16, 44, 0,  8, flat, unbanked },   { 16, 46, 0,  8, flat, unbanked },   { 16, 48, 0,  8, flat, unbanked },   { 16, 50, 0,  8, flat, unbanked },
              { 14, 50, 0,  0, flat, unbanked },   { 12, 50, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),   { 12, 50, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
              { 12, 50, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 12, 50, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { 14, 50, 0, 16, flat, unbanked },   { 16, 50, 0, 16, flat, unbanked },   { 16, 52, 0,  8, flat, unbanked },   { 16, 54, 0,  8, flat, unbanked },
              { 16, 56, 0,  8, flat, unbanked },   { 16, 58, 0,  8, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
              { 16, 58, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),   { 16, 58, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),   { 16, 60, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },   {  2, 15, 0, 16, flat, unbanked },   {  4, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   {  4, 15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   {  4, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   {  4, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  4, 15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {  6, 15, 0, 16, flat, unbanked },   {  8, 15, 0, 16, flat, unbanked },   { 10, 15, 0, 16, flat, unbanked },
              { 12, 15, 0, 16, flat, unbanked },   { 14, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 18, 15, 0, 16, flat, unbanked },
              { 20, 15, 0, 16, flat, unbanked },   { 22, 15, 0, 16, flat, unbanked },   { 24, 15, 0, 16, flat, unbanked },   { 26, 15, 0, 16, flat, unbanked },
              { 28, 15, 0, 16, flat, unbanked },   { 30, 15, 0, 16, flat, unbanked },   { 32, 15, 0, 16, flat, unbanked },   { 34, 15, 0, 16, flat, unbanked },
              { 36, 15, 0, 16, flat, unbanked },   { 38, 15, 0, 16, flat, unbanked },   { 40, 15, 0, 16, flat, unbanked },   { 42, 15, 0, 16, flat, unbanked },
              { 44, 15, 0, 16, flat, unbanked },   { 46, 15, 0, 16, flat, unbanked },   { 48, 15, 0, 16, flat, unbanked },   { 50, 15, 0, 16, flat, unbanked },
              { 50, 17, 0,  8, flat, unbanked },   { 50, 19, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),   { 50, 19, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
              { 50, 19, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 50, 19, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { 50, 17, 0, 24, flat, unbanked },   { 50, 15, 0, 24, flat, unbanked },   { 52, 15, 0, 16, flat, unbanked },   { 54, 15, 0, 16, flat, unbanked },
              { 56, 15, 0, 16, flat, unbanked },   { 58, 15, 0, 16, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
              { 58, 15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),   { 58, 15, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),   { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },  { 15,  29, 0, 24, flat, unbanked },  { 15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 15,  25, 0, 24, flat, unbanked },  { 15,  23, 0, 24, flat, unbanked },  { 15,  21, 0, 24, flat, unbanked },
             { 15,  19, 0, 24, flat, unbanked },  { 15,  17, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 15,  13, 0, 24, flat, unbanked },
             { 15,  11, 0, 24, flat, unbanked },  { 15,   9, 0, 24, flat, unbanked },  { 15,   7, 0, 24, flat, unbanked },  { 15,   5, 0, 24, flat, unbanked },
             { 15,   3, 0, 24, flat, unbanked },  { 15,   1, 0, 24, flat, unbanked },  { 15,  -1, 0, 24, flat, unbanked },  { 15,  -3, 0, 24, flat, unbanked },
             { 15,  -5, 0, 24, flat, unbanked },  { 15,  -7, 0, 24, flat, unbanked },  { 15,  -9, 0, 24, flat, unbanked },  { 15, -11, 0, 24, flat, unbanked },
             { 15, -13, 0, 24, flat, unbanked },  { 15, -15, 0, 24, flat, unbanked },  { 15, -17, 0, 24, flat, unbanked },  { 15, -19, 0, 24, flat, unbanked },
             { 17, -19, 0, 16, flat, unbanked },  { 19, -19, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),
                          MINI_GOLF_STATE(Unk3),  { 19, -19, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),
             { 19, -19, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 19, -19, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             { 17, -19, 0,  0, flat, unbanked },  { 15, -19, 0,  0, flat, unbanked },  { 15, -21, 0, 24, flat, unbanked },  { 15, -23, 0, 24, flat, unbanked },
             { 15, -25, 0, 24, flat, unbanked },  { 15, -27, 0, 24, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
             { 15, -27, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { 15, -27, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),  { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_0, {
                          MINI_GOLF_STATE(Unk0),  {  31, 16, 0,  0, flat, unbanked },  {  29, 16, 0,  0, flat, unbanked },  {  27, 16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  27, 16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  27, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  27, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  27, 16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  25, 16, 0,  0, flat, unbanked },  {  23, 16, 0,  0, flat, unbanked },  {  21, 16, 0,  0, flat, unbanked },
             {  20, 16, 0,  0, flat, unbanked },  {  20, 14, 0, 24, flat, unbanked },  {  20, 12, 0, 24, flat, unbanked },  {  20, 11, 0, 24, flat, unbanked },
                 MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  20, 11, 0, 24, flat, unbanked },
                  MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  {  20, 11, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),
             {  20, 11, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  {  19, 12, 0,  8, flat, unbanked },  {  19, 14, 0,  8, flat, unbanked },
             {  19, 16, 0,  0, flat, unbanked },  {  19, 16, 0,  0, flat, unbanked },  {  17, 16, 0,  0, flat, unbanked },  {  15, 16, 0,  0, flat, unbanked },
             {  13, 16, 0,  0, flat, unbanked },  {  11, 16, 0,  0, flat, unbanked },  {   9, 16, 5,  0, flat, unbanked },  {   7, 16, 5,  0, flat, unbanked },
             {   5, 16, 5,  0, flat, unbanked },  {   3, 16, 5,  0, flat, unbanked },  {   1, 16, 5,  0, flat, unbanked },  {  -1, 16, 5,  0, flat, unbanked },
             {  -3, 16, 5,  0, flat, unbanked },  {  -5, 16, 5,  0, flat, unbanked },  {  -7, 16, 5,  0, flat, unbanked },  {  -9, 16, 0,  0, flat, unbanked },
             { -11, 16, 0,  0, flat, unbanked },  { -13, 16, 0,  0, flat, unbanked },  { -15, 16, 0,  0, flat, unbanked },  { -17, 16, 0,  0, flat, unbanked },
             { -19, 16, 0,  0, flat, unbanked },  { -21, 16, 0,  0, flat, unbanked },  { -23, 16, 0,  0, flat, unbanked },  { -23, 18, 0,  8, flat, unbanked },
             { -23, 20, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             { -23, 21, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { -23, 21, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  { -23, 21, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { -23, 20, 0, 24, flat, unbanked },
             { -23, 18, 0, 24, flat, unbanked },  { -23, 16, 0, 24, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
             { -23, 16, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { -23, 16, 0,  0, flat, unbanked },
             { -25, 16, 0,  0, flat, unbanked },  { -27, 16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { -29, 16, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_1, {
                          MINI_GOLF_STATE(Unk0),   { 16,  0, 0,  8, flat, unbanked },   { 16,  2, 0,  8, flat, unbanked },   { 16,  4, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 16,  4, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   { 16,  6, 0,  8, flat, unbanked },   { 16,  8, 0,  8, flat, unbanked },   { 16, 10, 0,  8, flat, unbanked },
              { 16, 11, 0,  8, flat, unbanked },   { 14, 11, 0,  0, flat, unbanked },   { 12, 11, 0,  0, flat, unbanked },   { 11, 11, 0,  0, flat, unbanked },
                 MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 11, 11, 0,  0, flat, unbanked },
                  MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 11, 11, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),
              { 11, 11, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 12, 12, 0, 16, flat, unbanked },   { 14, 12, 0, 16, flat, unbanked },
              { 16, 12, 0,  8, flat, unbanked },   { 16, 12, 0,  8, flat, unbanked },   { 16, 14, 0,  8, flat, unbanked },   { 16, 16, 0,  8, flat, unbanked },
              { 16, 18, 0,  8, flat, unbanked },   { 16, 20, 0,  8, flat, unbanked },   { 16, 22, 5,  8, flat, unbanked },   { 16, 24, 5,  8, flat, unbanked },
              { 16, 26, 5,  8, flat, unbanked },   { 16, 28, 5,  8, flat, unbanked },   { 16, 30, 5,  8, flat, unbanked },   { 16, 32, 5,  8, flat, unbanked },
              { 16, 34, 5,  8, flat, unbanked },   { 16, 36, 5,  8, flat, unbanked },   { 16, 38, 5,  8, flat, unbanked },   { 16, 40, 0,  8, flat, unbanked },
              { 16, 42, 0,  8, flat, unbanked },   { 16, 44, 0,  8, flat, unbanked },   { 16, 46, 0,  8, flat, unbanked },   { 16, 48, 0,  8, flat, unbanked },
              { 16, 50, 0,  8, flat, unbanked },   { 16, 52, 0,  8, flat, unbanked },   { 16, 54, 0,  8, flat, unbanked },   { 18, 54, 0, 16, flat, unbanked },
              { 20, 54, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
              { 21, 54, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 21, 54, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),   { 21, 54, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 20, 54, 0,  0, flat, unbanked },
              { 18, 54, 0,  0, flat, unbanked },   { 16, 54, 0,  0, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
              { 16, 54, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),   { 16, 54, 0,  8, flat, unbanked },
              { 16, 56, 0,  8, flat, unbanked },   { 16, 58, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk3),   { 16, 60, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },   {  2, 15, 0, 16, flat, unbanked },   {  4, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   {  4, 15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   {  4, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   {  4, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  4, 15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {  6, 15, 0, 16, flat, unbanked },   {  8, 15, 0, 16, flat, unbanked },   { 10, 15, 0, 16, flat, unbanked },
              { 11, 15, 0, 16, flat, unbanked },   { 11, 17, 0,  8, flat, unbanked },   { 11, 19, 0,  8, flat, unbanked },   { 11, 20, 0,  8, flat, unbanked },
                 MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 11, 20, 0,  8, flat, unbanked },
                  MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 11, 20, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),
              { 11, 20, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 12, 19, 0, 24, flat, unbanked },   { 12, 17, 0, 24, flat, unbanked },
              { 12, 15, 0, 16, flat, unbanked },   { 12, 15, 0, 16, flat, unbanked },   { 14, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },
              { 18, 15, 0, 16, flat, unbanked },   { 20, 15, 0, 16, flat, unbanked },   { 22, 15, 5, 16, flat, unbanked },   { 24, 15, 5, 16, flat, unbanked },
              { 26, 15, 5, 16, flat, unbanked },   { 28, 15, 5, 16, flat, unbanked },   { 30, 15, 5, 16, flat, unbanked },   { 32, 15, 5, 16, flat, unbanked },
              { 34, 15, 5, 16, flat, unbanked },   { 36, 15, 5, 16, flat, unbanked },   { 38, 15, 5, 16, flat, unbanked },   { 40, 15, 0, 16, flat, unbanked },
              { 42, 15, 0, 16, flat, unbanked },   { 44, 15, 0, 16, flat, unbanked },   { 46, 15, 0, 16, flat, unbanked },   { 48, 15, 0, 16, flat, unbanked },
              { 50, 15, 0, 16, flat, unbanked },   { 52, 15, 0, 16, flat, unbanked },   { 54, 15, 0, 16, flat, unbanked },   { 54, 13, 0, 24, flat, unbanked },
              { 54, 11, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
              { 54, 10, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 54, 10, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),   { 54, 10, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 54, 11, 0,  8, flat, unbanked },
              { 54, 13, 0,  8, flat, unbanked },   { 54, 15, 0,  8, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
              { 54, 15, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),   { 54, 15, 0, 16, flat, unbanked },
              { 56, 15, 0, 16, flat, unbanked },   { 58, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },  { 15,  29, 0, 24, flat, unbanked },  { 15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 15,  25, 0, 24, flat, unbanked },  { 15,  23, 0, 24, flat, unbanked },  { 15,  21, 0, 24, flat, unbanked },
             { 15,  20, 0, 24, flat, unbanked },  { 17,  20, 0, 16, flat, unbanked },  { 19,  20, 0, 16, flat, unbanked },  { 20,  20, 0, 16, flat, unbanked },
                 MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 20,  20, 0, 16, flat, unbanked },
                  MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { 20,  20, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),
             { 20,  20, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 19,  19, 0,  0, flat, unbanked },  { 17,  19, 0,  0, flat, unbanked },
             { 15,  19, 0, 24, flat, unbanked },  { 15,  19, 0, 24, flat, unbanked },  { 15,  17, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },
             { 15,  13, 0, 24, flat, unbanked },  { 15,  11, 0, 24, flat, unbanked },  { 15,   9, 5, 24, flat, unbanked },  { 15,   7, 5, 24, flat, unbanked },
             { 15,   5, 5, 24, flat, unbanked },  { 15,   3, 5, 24, flat, unbanked },  { 15,   1, 5, 24, flat, unbanked },  { 15,  -1, 5, 24, flat, unbanked },
             { 15,  -3, 5, 24, flat, unbanked },  { 15,  -5, 5, 24, flat, unbanked },  { 15,  -7, 5, 24, flat, unbanked },  { 15,  -9, 0, 24, flat, unbanked },
             { 15, -11, 0, 24, flat, unbanked },  { 15, -13, 0, 24, flat, unbanked },  { 15, -15, 0, 24, flat, unbanked },  { 15, -17, 0, 24, flat, unbanked },
             { 15, -19, 0, 24, flat, unbanked },  { 15, -21, 0, 24, flat, unbanked },  { 15, -23, 0, 24, flat, unbanked },  { 13, -23, 0,  0, flat, unbanked },
             { 11, -23, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             { 10, -23, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { 10, -23, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  { 10, -23, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 11, -23, 0, 16, flat, unbanked },
             { 13, -23, 0, 16, flat, unbanked },  { 15, -23, 0, 16, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),
             { 15, -23, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { 15, -23, 0, 24, flat, unbanked },
             { 15, -25, 0, 24, flat, unbanked },  { 15, -27, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_0, {
                          MINI_GOLF_STATE(Unk0),  {  31, 16, 0,  0, flat, unbanked },  {  29, 16, 0,  0, flat, unbanked },  {  27, 16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  27, 16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  27, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  27, 16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  27, 16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  27, 16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  25, 16, 0,  0, flat, unbanked },  {  23, 16, 0,  0, flat, unbanked },  {  21, 16, 0,  0, flat, unbanked },
             {  19, 16, 0,  0, flat, unbanked },  {  17, 16, 0,  0, flat, unbanked },  {  15, 16, 0,  0, flat, unbanked },  {  15, 18, 0,  8, flat, unbanked },
             {  15, 20, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             {  15, 20, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  {  15, 20, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  {  15, 20, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  15, 20, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  15, 20, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  15, 20, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  13, 20, 0,  0, flat, unbanked },  {  11, 20, 0,  0, flat, unbanked },  {   9, 20, 0,  0, flat, unbanked },
             {   7, 20, 0,  0, flat, unbanked },  {   5, 20, 0,  0, flat, unbanked },  {   3, 20, 0,  0, flat, unbanked },  {   1, 20, 0,  0, flat, unbanked },
             {  -1, 20, 0,  0, flat, unbanked },  {  -3, 20, 0,  0, flat, unbanked },  {  -3, 22, 0,  8, flat, unbanked },  {  -3, 24, 0,  8, flat, unbanked },
             {  -3, 26, 0,  8, flat, unbanked },  {  -3, 28, 0,  8, flat, unbanked },  {  -3, 30, 0,  8, flat, unbanked },  {  -3, 32, 0,  8, flat, unbanked },
             {  -3, 34, 0,  8, flat, unbanked },  {  -3, 36, 0,  8, flat, unbanked },  {  -3, 38, 0,  8, flat, unbanked },  {  -3, 40, 0,  8, flat, unbanked },
             {  -3, 42, 0,  8, flat, unbanked },  {  -5, 42, 0,  0, flat, unbanked },  {  -7, 42, 0,  0, flat, unbanked },  {  -9, 42, 0,  0, flat, unbanked },
             { -11, 42, 0,  0, flat, unbanked },  { -13, 42, 0,  0, flat, unbanked },  { -15, 42, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { -16, 41, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { -16, 41, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { -16, 41, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { -17, 43, 0,  8, flat, unbanked },  { -17, 45, 0,  8, flat, unbanked },  { -17, 47, 0,  8, flat, unbanked },
             { -17, 49, 0,  8, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),  { -17, 49, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { -17, 48, 0,  8, flat, unbanked },  { -17, 50, 0,  8, flat, unbanked },
             { -17, 52, 0,  8, flat, unbanked },  { -17, 54, 0,  8, flat, unbanked },  { -17, 56, 0,  8, flat, unbanked },  { -17, 58, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),  { -17, 60, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_1, {
                          MINI_GOLF_STATE(Unk0),   { 16,  0, 0,  8, flat, unbanked },   { 16,  2, 0,  8, flat, unbanked },   { 16,  4, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 16,  4, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   { 16,  4, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 16,  4, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   { 16,  6, 0,  8, flat, unbanked },   { 16,  8, 0,  8, flat, unbanked },   { 16, 10, 0,  8, flat, unbanked },
              { 16, 12, 0,  8, flat, unbanked },   { 16, 14, 0,  8, flat, unbanked },   { 16, 16, 0,  8, flat, unbanked },   { 18, 16, 0, 16, flat, unbanked },
              { 20, 16, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
              { 20, 16, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 20, 16, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),   { 20, 16, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 20, 16, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   { 20, 16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 20, 16, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   { 20, 18, 0,  8, flat, unbanked },   { 20, 20, 0,  8, flat, unbanked },   { 20, 22, 0,  8, flat, unbanked },
              { 20, 24, 0,  8, flat, unbanked },   { 20, 26, 0,  8, flat, unbanked },   { 20, 28, 0,  8, flat, unbanked },   { 20, 30, 0,  8, flat, unbanked },
              { 20, 32, 0,  8, flat, unbanked },   { 20, 34, 0,  8, flat, unbanked },   { 22, 34, 0, 16, flat, unbanked },   { 24, 34, 0, 16, flat, unbanked },
              { 26, 34, 0, 16, flat, unbanked },   { 28, 34, 0, 16, flat, unbanked },   { 30, 34, 0, 16, flat, unbanked },   { 32, 34, 0, 16, flat, unbanked },
              { 34, 34, 0, 16, flat, unbanked },   { 36, 34, 0, 16, flat, unbanked },   { 38, 34, 0, 16, flat, unbanked },   { 40, 34, 0, 16, flat, unbanked },
              { 42, 34, 0, 16, flat, unbanked },   { 42, 36, 0,  8, flat, unbanked },   { 42, 38, 0,  8, flat, unbanked },   { 42, 40, 0,  8, flat, unbanked },
              { 42, 42, 0,  8, flat, unbanked },   { 42, 44, 0,  8, flat, unbanked },   { 42, 46, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 41, 47, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   { 41, 47, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 41, 47, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   { 43, 48, 0, 16, flat, unbanked },   { 45, 48, 0, 16, flat, unbanked },   { 47, 48, 0, 16, flat, unbanked },
              { 49, 48, 0, 16, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),   { 49, 48, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),   { 48, 48, 0, 16, flat, unbanked },   { 50, 48, 0, 16, flat, unbanked },
              { 52, 48, 0, 16, flat, unbanked },   { 54, 48, 0, 16, flat, unbanked },   { 56, 48, 0, 16, flat, unbanked },   { 58, 48, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),   { 60, 48, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_2, {
                          MINI_GOLF_STATE(Unk0),  {  0,  15, 0, 16, flat, unbanked },  {  2,  15, 0, 16, flat, unbanked },  {  4,  15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  4,  15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  4,  15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  4,  15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  4,  15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  4,  15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  6,  15, 0, 16, flat, unbanked },  {  8,  15, 0, 16, flat, unbanked },  { 10,  15, 0, 16, flat, unbanked },
             { 12,  15, 0, 16, flat, unbanked },  { 14,  15, 0, 16, flat, unbanked },  { 16,  15, 0, 16, flat, unbanked },  { 16,  13, 0, 24, flat, unbanked },
             { 16,  11, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             { 16,  11, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { 16,  11, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  { 16,  11, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 16,  11, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 16,  11, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 16,  11, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 18,  11, 0, 16, flat, unbanked },  { 20,  11, 0, 16, flat, unbanked },  { 22,  11, 0, 16, flat, unbanked },
             { 24,  11, 0, 16, flat, unbanked },  { 26,  11, 0, 16, flat, unbanked },  { 28,  11, 0, 16, flat, unbanked },  { 30,  11, 0, 16, flat, unbanked },
             { 32,  11, 0, 16, flat, unbanked },  { 34,  11, 0, 16, flat, unbanked },  { 34,   9, 0, 24, flat, unbanked },  { 34,   7, 0, 24, flat, unbanked },
             { 34,   5, 0, 24, flat, unbanked },  { 34,   3, 0, 24, flat, unbanked },  { 34,   1, 0, 24, flat, unbanked },  { 34,  -1, 0, 24, flat, unbanked },
             { 34,  -3, 0, 24, flat, unbanked },  { 34,  -5, 0, 24, flat, unbanked },  { 34,  -7, 0, 24, flat, unbanked },  { 34,  -9, 0, 24, flat, unbanked },
             { 34, -11, 0, 24, flat, unbanked },  { 36, -11, 0, 16, flat, unbanked },  { 38, -11, 0, 16, flat, unbanked },  { 40, -11, 0, 16, flat, unbanked },
             { 42, -11, 0, 16, flat, unbanked },  { 44, -11, 0, 16, flat, unbanked },  { 46, -11, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 47, -10, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 47, -10, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 47, -10, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 48, -12, 0, 24, flat, unbanked },  { 48, -14, 0, 24, flat, unbanked },  { 48, -16, 0, 24, flat, unbanked },
             { 48, -18, 0, 24, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),  { 48, -18, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { 48, -17, 0, 24, flat, unbanked },  { 48, -19, 0, 24, flat, unbanked },
             { 48, -21, 0, 24, flat, unbanked },  { 48, -23, 0, 24, flat, unbanked },  { 48, -25, 0, 24, flat, unbanked },  { 48, -27, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),  { 48, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_3, {
                          MINI_GOLF_STATE(Unk0), {  15,  31, 0, 24, flat, unbanked }, {  15,  29, 0, 24, flat, unbanked }, {  15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2), {  15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5), {  15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3), {  15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5), {  15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk), {  15,  25, 0, 24, flat, unbanked }, {  15,  23, 0, 24, flat, unbanked }, {  15,  21, 0, 24, flat, unbanked },
            {  15,  19, 0, 24, flat, unbanked }, {  15,  17, 0, 24, flat, unbanked }, {  15,  15, 0, 24, flat, unbanked }, {  13,  15, 0,  0, flat, unbanked },
            {  11,  15, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
            {  11,  15, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5), {  11,  15, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2), {  11,  15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3), {  11,  15, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5), {  11,  15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  11,  15, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk), {  11,  13, 0, 24, flat, unbanked }, {  11,  11, 0, 24, flat, unbanked }, {  11,   9, 0, 24, flat, unbanked },
            {  11,   7, 0, 24, flat, unbanked }, {  11,   5, 0, 24, flat, unbanked }, {  11,   3, 0, 24, flat, unbanked }, {  11,   1, 0, 24, flat, unbanked },
            {  11,  -1, 0, 24, flat, unbanked }, {  11,  -3, 0, 24, flat, unbanked }, {   9,  -3, 0,  0, flat, unbanked }, {   7,  -3, 0,  0, flat, unbanked },
            {   5,  -3, 0,  0, flat, unbanked }, {   3,  -3, 0,  0, flat, unbanked }, {   1,  -3, 0,  0, flat, unbanked }, {  -1,  -3, 0,  0, flat, unbanked },
            {  -3,  -3, 0,  0, flat, unbanked }, {  -5,  -3, 0,  0, flat, unbanked }, {  -7,  -3, 0,  0, flat, unbanked }, {  -9,  -3, 0,  0, flat, unbanked },
            { -11,  -3, 0,  0, flat, unbanked }, { -11,  -5, 0, 24, flat, unbanked }, { -11,  -7, 0, 24, flat, unbanked }, { -11,  -9, 0, 24, flat, unbanked },
            { -11, -11, 0, 24, flat, unbanked }, { -11, -13, 0, 24, flat, unbanked }, { -11, -15, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3), { -10, -16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5), { -10, -16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2), { -10, -16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk), { -12, -17, 0,  0, flat, unbanked }, { -14, -17, 0,  0, flat, unbanked }, { -16, -17, 0,  0, flat, unbanked },
            { -18, -17, 0,  0, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5), { -18, -17, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6), { -17, -17, 0,  0, flat, unbanked }, { -19, -17, 0,  0, flat, unbanked },
            { -21, -17, 0,  0, flat, unbanked }, { -23, -17, 0,  0, flat, unbanked }, { -25, -17, 0,  0, flat, unbanked }, { -27, -17, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk3), { -29, -17, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_0, {
                          MINI_GOLF_STATE(Unk0), {  31,  16, 0,  0, flat, unbanked }, {  29,  16, 0,  0, flat, unbanked }, {  27,  16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2), {  27,  16, 0,  0, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5), {  27,  16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3), {  27,  16, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5), {  27,  16, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  27,  16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk), {  25,  16, 0,  0, flat, unbanked }, {  23,  16, 0,  0, flat, unbanked }, {  21,  16, 0,  0, flat, unbanked },
            {  19,  16, 0,  0, flat, unbanked }, {  17,  16, 0,  0, flat, unbanked }, {  15,  16, 0,  0, flat, unbanked }, {  15,  14, 0, 24, flat, unbanked },
            {  15,  12, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
            {  15,  12, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5), {  15,  12, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2), {  15,  12, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3), {  15,  12, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5), {  15,  12, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  15,  12, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk), {  13,  12, 0,  0, flat, unbanked }, {  11,  12, 0,  0, flat, unbanked }, {   9,  12, 0,  0, flat, unbanked },
            {   7,  12, 0,  0, flat, unbanked }, {   5,  12, 0,  0, flat, unbanked }, {   3,  12, 0,  0, flat, unbanked }, {   1,  12, 0,  0, flat, unbanked },
            {  -1,  12, 0,  0, flat, unbanked }, {  -3,  12, 0,  0, flat, unbanked }, {  -3,  10, 0, 24, flat, unbanked }, {  -3,   8, 0, 24, flat, unbanked },
            {  -3,   6, 0, 24, flat, unbanked }, {  -3,   4, 0, 24, flat, unbanked }, {  -3,   2, 0, 24, flat, unbanked }, {  -3,   0, 0, 24, flat, unbanked },
            {  -3,  -2, 0, 24, flat, unbanked }, {  -3,  -4, 0, 24, flat, unbanked }, {  -3,  -6, 0, 24, flat, unbanked }, {  -3,  -8, 0, 24, flat, unbanked },
            {  -3, -10, 0, 24, flat, unbanked }, {  -5, -10, 0,  0, flat, unbanked }, {  -7, -10, 0,  0, flat, unbanked }, {  -9, -10, 0,  0, flat, unbanked },
            { -11, -10, 0,  0, flat, unbanked }, { -13, -10, 0,  0, flat, unbanked }, { -15, -10, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3), { -16,  -9, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5), { -16,  -9, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2), { -16,  -9, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk), { -17, -11, 0, 24, flat, unbanked }, { -17, -13, 0, 24, flat, unbanked }, { -17, -15, 0, 24, flat, unbanked },
            { -17, -17, 0, 24, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5), { -17, -17, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6), { -17, -16, 0, 24, flat, unbanked }, { -17, -18, 0, 24, flat, unbanked },
            { -17, -20, 0, 24, flat, unbanked }, { -17, -22, 0, 24, flat, unbanked }, { -17, -24, 0, 24, flat, unbanked }, { -17, -26, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk3), { -17, -28, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_1, {
                          MINI_GOLF_STATE(Unk0),  {  16,  0, 0,  8, flat, unbanked },  {  16,  2, 0,  8, flat, unbanked },  {  16,  4, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  {  16,  4, 0,  8, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  {  16,  4, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  16,  4, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  16,  4, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  16,  4, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  16,  6, 0,  8, flat, unbanked },  {  16,  8, 0,  8, flat, unbanked },  {  16, 10, 0,  8, flat, unbanked },
             {  16, 12, 0,  8, flat, unbanked },  {  16, 14, 0,  8, flat, unbanked },  {  16, 16, 0,  8, flat, unbanked },  {  14, 16, 0,  0, flat, unbanked },
             {  12, 16, 0,  0, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             {  12, 16, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  {  12, 16, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  {  12, 16, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  12, 16, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  12, 16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  12, 16, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  {  12, 18, 0,  8, flat, unbanked },  {  12, 20, 0,  8, flat, unbanked },  {  12, 22, 0,  8, flat, unbanked },
             {  12, 24, 0,  8, flat, unbanked },  {  12, 26, 0,  8, flat, unbanked },  {  12, 28, 0,  8, flat, unbanked },  {  12, 30, 0,  8, flat, unbanked },
             {  12, 32, 0,  8, flat, unbanked },  {  12, 34, 0,  8, flat, unbanked },  {  10, 34, 0,  0, flat, unbanked },  {   8, 34, 0,  0, flat, unbanked },
             {   6, 34, 0,  0, flat, unbanked },  {   4, 34, 0,  0, flat, unbanked },  {   2, 34, 0,  0, flat, unbanked },  {   0, 34, 0,  0, flat, unbanked },
             {  -2, 34, 0,  0, flat, unbanked },  {  -4, 34, 0,  0, flat, unbanked },  {  -6, 34, 0,  0, flat, unbanked },  {  -8, 34, 0,  0, flat, unbanked },
             { -10, 34, 0,  0, flat, unbanked },  { -10, 36, 0,  8, flat, unbanked },  { -10, 38, 0,  8, flat, unbanked },  { -10, 40, 0,  8, flat, unbanked },
             { -10, 42, 0,  8, flat, unbanked },  { -10, 44, 0,  8, flat, unbanked },  { -10, 46, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  {  -9, 47, 0, 24, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  {  -9, 47, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  -9, 47, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { -11, 48, 0,  0, flat, unbanked },  { -13, 48, 0,  0, flat, unbanked },  { -15, 48, 0,  0, flat, unbanked },
             { -17, 48, 0,  0, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),  { -17, 48, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { -16, 48, 0,  0, flat, unbanked },  { -18, 48, 0,  0, flat, unbanked },
             { -20, 48, 0,  0, flat, unbanked },  { -22, 48, 0,  0, flat, unbanked },  { -24, 48, 0,  0, flat, unbanked },  { -26, 48, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),  { -28, 48, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },   {  2, 15, 0, 16, flat, unbanked },   {  4, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),   {  4, 15, 0, 16, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),   {  4, 15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   {  4, 15, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   {  4, 15, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  4, 15, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {  6, 15, 0, 16, flat, unbanked },   {  8, 15, 0, 16, flat, unbanked },   { 10, 15, 0, 16, flat, unbanked },
              { 12, 15, 0, 16, flat, unbanked },   { 14, 15, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },   { 16, 17, 0,  8, flat, unbanked },
              { 16, 19, 0,  8, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
              { 16, 19, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),   { 16, 19, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),   { 16, 19, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 16, 19, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   { 16, 19, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 16, 19, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   { 18, 19, 0, 16, flat, unbanked },   { 20, 19, 0, 16, flat, unbanked },   { 22, 19, 0, 16, flat, unbanked },
              { 24, 19, 0, 16, flat, unbanked },   { 26, 19, 0, 16, flat, unbanked },   { 28, 19, 0, 16, flat, unbanked },   { 30, 19, 0, 16, flat, unbanked },
              { 32, 19, 0, 16, flat, unbanked },   { 34, 19, 0, 16, flat, unbanked },   { 34, 21, 0,  8, flat, unbanked },   { 34, 23, 0,  8, flat, unbanked },
              { 34, 25, 0,  8, flat, unbanked },   { 34, 27, 0,  8, flat, unbanked },   { 34, 29, 0,  8, flat, unbanked },   { 34, 31, 0,  8, flat, unbanked },
              { 34, 33, 0,  8, flat, unbanked },   { 34, 35, 0,  8, flat, unbanked },   { 34, 37, 0,  8, flat, unbanked },   { 34, 39, 0,  8, flat, unbanked },
              { 34, 41, 0,  8, flat, unbanked },   { 36, 41, 0, 16, flat, unbanked },   { 38, 41, 0, 16, flat, unbanked },   { 40, 41, 0, 16, flat, unbanked },
              { 42, 41, 0, 16, flat, unbanked },   { 44, 41, 0, 16, flat, unbanked },   { 46, 41, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),   { 47, 40, 0,  0, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),   { 47, 40, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 47, 40, 0,  0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   { 48, 42, 0,  8, flat, unbanked },   { 48, 44, 0,  8, flat, unbanked },   { 48, 46, 0,  8, flat, unbanked },
              { 48, 48, 0,  8, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),   { 48, 48, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),   { 48, 47, 0,  8, flat, unbanked },   { 48, 49, 0,  8, flat, unbanked },
              { 48, 51, 0,  8, flat, unbanked },   { 48, 53, 0,  8, flat, unbanked },   { 48, 55, 0,  8, flat, unbanked },   { 48, 57, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),   { 48, 59, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },  { 15,  29, 0, 24, flat, unbanked },  { 15,  27, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 24, flat, unbanked },
               MINI_GOLF_ANIMATION(PlaceBallUp),               MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 15,  27, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 15,  27, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  27, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 15,  25, 0, 24, flat, unbanked },  { 15,  23, 0, 24, flat, unbanked },  { 15,  21, 0, 24, flat, unbanked },
             { 15,  19, 0, 24, flat, unbanked },  { 15,  17, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },  { 17,  15, 0, 16, flat, unbanked },
             { 19,  15, 0, 16, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),               MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),
             { 19,  15, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),               MINI_GOLF_STATE(Unk5),  { 19,  15, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),  { 19,  15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 19,  15, 0, 16, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 19,  15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 19,  15, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 19,  13, 0, 24, flat, unbanked },  { 19,  11, 0, 24, flat, unbanked },  { 19,   9, 0, 24, flat, unbanked },
             { 19,   7, 0, 24, flat, unbanked },  { 19,   5, 0, 24, flat, unbanked },  { 19,   3, 0, 24, flat, unbanked },  { 19,   1, 0, 24, flat, unbanked },
             { 19,  -1, 0, 24, flat, unbanked },  { 19,  -3, 0, 24, flat, unbanked },  { 21,  -3, 0, 16, flat, unbanked },  { 23,  -3, 0, 16, flat, unbanked },
             { 25,  -3, 0, 16, flat, unbanked },  { 27,  -3, 0, 16, flat, unbanked },  { 29,  -3, 0, 16, flat, unbanked },  { 31,  -3, 0, 16, flat, unbanked },
             { 33,  -3, 0, 16, flat, unbanked },  { 35,  -3, 0, 16, flat, unbanked },  { 37,  -3, 0, 16, flat, unbanked },  { 39,  -3, 0, 16, flat, unbanked },
             { 41,  -3, 0, 16, flat, unbanked },  { 41,  -5, 0, 24, flat, unbanked },  { 41,  -7, 0, 24, flat, unbanked },  { 41,  -9, 0, 24, flat, unbanked },
             { 41, -11, 0, 24, flat, unbanked },  { 41, -13, 0, 24, flat, unbanked },  { 41, -15, 0, 24, flat, unbanked },      MINI_GOLF_ANIMATION(SwingLeft),
                          MINI_GOLF_STATE(Unk5),               MINI_GOLF_STATE(Unk3),  { 40, -16, 0,  8, flat, unbanked },       MINI_GOLF_ANIMATION(PuttLeft),
                          MINI_GOLF_STATE(Unk5),  { 40, -16, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 40, -16, 0,  8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 42, -17, 0, 16, flat, unbanked },  { 44, -17, 0, 16, flat, unbanked },  { 46, -17, 0, 16, flat, unbanked },
             { 48, -17, 0, 16, flat, unbanked },     MINI_GOLF_ANIMATION(PickupBall),               MINI_GOLF_STATE(Unk5),  { 48, -17, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk6),  { 47, -17, 0, 16, flat, unbanked },  { 49, -17, 0, 16, flat, unbanked },
             { 51, -17, 0, 16, flat, unbanked },  { 53, -17, 0, 16, flat, unbanked },  { 55, -17, 0, 16, flat, unbanked },  { 57, -17, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk3),  { 59, -17, 0, 16, flat, unbanked },
        })

        static constexpr const VehicleInfoList * PlayerPathC[] = {
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3,
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // EndStation
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // BeginStation
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // MiddleStation
            &Up25_0,                  &Up25_1,                  &Up25_2,                  &Up25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60
            &FlatToUp25_0,            &FlatToUp25_1,            &FlatToUp25_2,            &FlatToUp25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp25
            &Up25ToFlat_0,            &Up25ToFlat_1,            &Up25ToFlat_2,            &Up25ToFlat_3,
            &Down25_0,                &Down25_1,                &Down25_2,                &Down25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60
            &FlatToDown25_0,          &FlatToDown25_1,          &FlatToDown25_2,          &FlatToDown25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown25
            &Down25ToFlat_0,          &Down25ToFlat_1,          &Down25ToFlat_2,          &Down25ToFlat_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BankedLeftQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BankedRightQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendLeft
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendRight
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftVerticalLoop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightVerticalLoop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankedQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankedQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesDown25
            &LeftQuarterTurn1Tile_0,  &LeftQuarterTurn1Tile_1,  &LeftQuarterTurn1Tile_2,  &LeftQuarterTurn1Tile_3,
            &RightQuarterTurn1Tile_0, &RightQuarterTurn1Tile_1, &RightQuarterTurn1Tile_2, &RightQuarterTurn1Tile_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftTwistDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightTwistDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftTwistUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightTwistUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // TowerBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // TowerSection
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToUp60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToFlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToDown60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToFlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendLeftCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendRightCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixUpSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixUpSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixDownSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixDownSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixUpLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixUpLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixDownLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixDownLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn1TileUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn1TileUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn1TileDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn1TileDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Brakes
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Booster
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Maze
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterBankedHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterBankedHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterBankedHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterBankedHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25LeftBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25RightBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Waterfall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Rapids
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // OnRidePhoto
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25LeftBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25RightBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Watersplash
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp60LongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToFlatLongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Whirlpool
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToFlatLongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown60LongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // CableLiftHill
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // ReverseFreefallSlope
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // ReverseFreefallVertical
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down90ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up90ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BrakeForDrop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthBankToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthBankToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthBankToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthBankToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60ToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60ToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LogFlumeReverser
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SpinningTunnel
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBarrelRollUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBarrelRollUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBarrelRollDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBarrelRollDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToLeftQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToRightQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesDown25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesDown25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // PoweredLift
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftLargeHalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightLargeHalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftLargeHalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightLargeHalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerTwistUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerTwistUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerTwistDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerTwistDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlyerHalfLoopUninvertedUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlyerHalfLoopInvertedDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HeartLineTransferUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HeartLineTransferDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHeartLineRoll
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHeartLineRoll
            &MinigolfHoleA_0,         &MinigolfHoleA_1,         &MinigolfHoleA_2,         &MinigolfHoleA_3,
            &MinigolfHoleB_0,         &MinigolfHoleB_1,         &MinigolfHoleB_2,         &MinigolfHoleB_3,
            &MinigolfHoleC_0,         &MinigolfHoleC_1,         &MinigolfHoleC_2,         &MinigolfHoleC_3,
            &MinigolfHoleD_0,         &MinigolfHoleD_1,         &MinigolfHoleD_2,         &MinigolfHoleD_3,
            &MinigolfHoleE_0,         &MinigolfHoleE_1,         &MinigolfHoleE_2,         &MinigolfHoleE_3,
        };
        // clang-format on
        static_assert(std::size(PlayerPathC) == kVehicleTrackSubpositionSizeMinigolf);
    } // namespace PlayerPathC

    namespace BallPathC
    {
        // clang-format off
        CREATE_VEHICLE_INFO(MinigolfHoleA_0, {
                          MINI_GOLF_STATE(Unk0),   {  31, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  26, 16, 0, 0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  26, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  25, 16, 0, 0, flat, unbanked },
              {  23, 15, 0, 0, flat, unbanked },   {  21, 15, 0, 0, flat, unbanked },   {  19, 14, 0, 0, flat, unbanked },   {  17, 13, 0, 0, flat, unbanked },
              {  15, 13, 0, 0, flat, unbanked },   {  14, 13, 0, 0, flat, unbanked },   {  13, 12, 0, 0, flat, unbanked },   {  12, 12, 0, 0, flat, unbanked },
              {  11, 12, 0, 0, flat, unbanked },   {  10, 11, 0, 0, flat, unbanked },   {   9, 11, 0, 0, flat, unbanked },   {   8, 11, 0, 0, flat, unbanked },
              {   7, 10, 0, 0, flat, unbanked },   {   6, 10, 0, 0, flat, unbanked },   {   5, 10, 0, 0, flat, unbanked },   {   4,  9, 0, 0, flat, unbanked },
              {   3,  9, 0, 0, flat, unbanked },   {   2,  9, 0, 0, flat, unbanked },   {   1,  8, 0, 0, flat, unbanked },   {   0,  8, 0, 0, flat, unbanked },
              {   0,  8, 0, 0, flat, unbanked },   {  -1,  9, 0, 0, flat, unbanked },   {  -2,  9, 0, 0, flat, unbanked },   {  -3,  9, 0, 0, flat, unbanked },
              {  -4, 10, 0, 0, flat, unbanked },   {  -5, 10, 0, 0, flat, unbanked },   {  -6, 10, 0, 0, flat, unbanked },   {  -7, 11, 0, 0, flat, unbanked },
              {  -8, 11, 0, 0, flat, unbanked },   {  -9, 11, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),
              { -10, 11, 0, 0, flat, unbanked },   { -11, 11, 0, 0, flat, unbanked },   { -13, 12, 0, 0, flat, unbanked },   { -14, 13, 0, 0, flat, unbanked },
              { -15, 13, 0, 0, flat, unbanked },   { -16, 14, 0, 0, flat, unbanked },   { -17, 14, 0, 0, flat, unbanked },   { -18, 15, 0, 0, flat, unbanked },
              { -19, 15, 0, 0, flat, unbanked },   { -20, 16, 0, 0, flat, unbanked },   { -21, 16, 0, 0, flat, unbanked },   { -22, 17, 0, 0, flat, unbanked },
              { -23, 17, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   { -24, 18, 0, 0, flat, unbanked },
              { -23, 18, 0, 0, flat, unbanked },   { -22, 18, 0, 0, flat, unbanked },   { -21, 18, 0, 0, flat, unbanked },   { -20, 18, 0, 0, flat, unbanked },
              { -19, 18, 0, 0, flat, unbanked },   { -18, 18, 0, 0, flat, unbanked },   { -17, 18, 0, 0, flat, unbanked },   { -16, 18, 0, 0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { -29, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),   { -29, 16, 0, 0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_1, {
                          MINI_GOLF_STATE(Unk0),    { 16,  0, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16,  5, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),    { 16,  5, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),    { 16,  6, 0, 8, flat, unbanked },
               { 15,  8, 0, 8, flat, unbanked },    { 15, 10, 0, 8, flat, unbanked },    { 14, 12, 0, 8, flat, unbanked },    { 13, 14, 0, 8, flat, unbanked },
               { 13, 16, 0, 8, flat, unbanked },    { 13, 17, 0, 8, flat, unbanked },    { 12, 18, 0, 8, flat, unbanked },    { 12, 19, 0, 8, flat, unbanked },
               { 12, 20, 0, 8, flat, unbanked },    { 11, 21, 0, 8, flat, unbanked },    { 11, 22, 0, 8, flat, unbanked },    { 11, 23, 0, 8, flat, unbanked },
               { 10, 24, 0, 8, flat, unbanked },    { 10, 25, 0, 8, flat, unbanked },    { 10, 26, 0, 8, flat, unbanked },    {  9, 27, 0, 8, flat, unbanked },
               {  9, 28, 0, 8, flat, unbanked },    {  9, 29, 0, 8, flat, unbanked },    {  8, 30, 0, 8, flat, unbanked },    {  8, 31, 0, 8, flat, unbanked },
               {  8, 31, 0, 8, flat, unbanked },    {  9, 32, 0, 8, flat, unbanked },    {  9, 33, 0, 8, flat, unbanked },    {  9, 34, 0, 8, flat, unbanked },
               { 10, 35, 0, 8, flat, unbanked },    { 10, 36, 0, 8, flat, unbanked },    { 10, 37, 0, 8, flat, unbanked },    { 11, 38, 0, 8, flat, unbanked },
               { 11, 39, 0, 8, flat, unbanked },    { 11, 40, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),
               { 11, 41, 0, 8, flat, unbanked },    { 11, 42, 0, 8, flat, unbanked },    { 12, 44, 0, 8, flat, unbanked },    { 13, 45, 0, 8, flat, unbanked },
               { 13, 46, 0, 8, flat, unbanked },    { 14, 47, 0, 8, flat, unbanked },    { 14, 48, 0, 8, flat, unbanked },    { 15, 49, 0, 8, flat, unbanked },
               { 15, 50, 0, 8, flat, unbanked },    { 16, 51, 0, 8, flat, unbanked },    { 16, 52, 0, 8, flat, unbanked },    { 17, 53, 0, 8, flat, unbanked },
               { 17, 54, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),    { 18, 55, 0, 8, flat, unbanked },
               { 18, 54, 0, 8, flat, unbanked },    { 18, 53, 0, 8, flat, unbanked },    { 18, 52, 0, 8, flat, unbanked },    { 18, 51, 0, 8, flat, unbanked },
               { 18, 50, 0, 8, flat, unbanked },    { 18, 49, 0, 8, flat, unbanked },    { 18, 48, 0, 8, flat, unbanked },    { 18, 47, 0, 8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),    { 16, 60, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),    { 16, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  5, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  5, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  6, 15, 0, 16, flat, unbanked },
              {  8, 16, 0, 16, flat, unbanked },   { 10, 16, 0, 16, flat, unbanked },   { 12, 17, 0, 16, flat, unbanked },   { 14, 18, 0, 16, flat, unbanked },
              { 16, 18, 0, 16, flat, unbanked },   { 17, 18, 0, 16, flat, unbanked },   { 18, 19, 0, 16, flat, unbanked },   { 19, 19, 0, 16, flat, unbanked },
              { 20, 19, 0, 16, flat, unbanked },   { 21, 20, 0, 16, flat, unbanked },   { 22, 20, 0, 16, flat, unbanked },   { 23, 20, 0, 16, flat, unbanked },
              { 24, 21, 0, 16, flat, unbanked },   { 25, 21, 0, 16, flat, unbanked },   { 26, 21, 0, 16, flat, unbanked },   { 27, 22, 0, 16, flat, unbanked },
              { 28, 22, 0, 16, flat, unbanked },   { 29, 22, 0, 16, flat, unbanked },   { 30, 23, 0, 16, flat, unbanked },   { 31, 23, 0, 16, flat, unbanked },
              { 31, 23, 0, 16, flat, unbanked },   { 32, 22, 0, 16, flat, unbanked },   { 33, 22, 0, 16, flat, unbanked },   { 34, 22, 0, 16, flat, unbanked },
              { 35, 21, 0, 16, flat, unbanked },   { 36, 21, 0, 16, flat, unbanked },   { 37, 21, 0, 16, flat, unbanked },   { 38, 20, 0, 16, flat, unbanked },
              { 39, 20, 0, 16, flat, unbanked },   { 40, 20, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),
              { 41, 20, 0, 16, flat, unbanked },   { 42, 20, 0, 16, flat, unbanked },   { 44, 19, 0, 16, flat, unbanked },   { 45, 18, 0, 16, flat, unbanked },
              { 46, 18, 0, 16, flat, unbanked },   { 47, 17, 0, 16, flat, unbanked },   { 48, 17, 0, 16, flat, unbanked },   { 49, 16, 0, 16, flat, unbanked },
              { 50, 16, 0, 16, flat, unbanked },   { 51, 15, 0, 16, flat, unbanked },   { 52, 15, 0, 16, flat, unbanked },   { 53, 14, 0, 16, flat, unbanked },
              { 54, 14, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   { 55, 13, 0, 16, flat, unbanked },
              { 54, 13, 0, 16, flat, unbanked },   { 53, 13, 0, 16, flat, unbanked },   { 52, 13, 0, 16, flat, unbanked },   { 51, 13, 0, 16, flat, unbanked },
              { 50, 13, 0, 16, flat, unbanked },   { 49, 13, 0, 16, flat, unbanked },   { 48, 13, 0, 16, flat, unbanked },   { 47, 13, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),   { 60, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),   { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleA_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15,  25, 0, 24, flat, unbanked },
             { 16,  23, 0, 24, flat, unbanked },  { 16,  21, 0, 24, flat, unbanked },  { 17,  19, 0, 24, flat, unbanked },  { 18,  17, 0, 24, flat, unbanked },
             { 18,  15, 0, 24, flat, unbanked },  { 18,  14, 0, 24, flat, unbanked },  { 19,  13, 0, 24, flat, unbanked },  { 19,  12, 0, 24, flat, unbanked },
             { 19,  11, 0, 24, flat, unbanked },  { 20,  10, 0, 24, flat, unbanked },  { 20,   9, 0, 24, flat, unbanked },  { 20,   8, 0, 24, flat, unbanked },
             { 21,   7, 0, 24, flat, unbanked },  { 21,   6, 0, 24, flat, unbanked },  { 21,   5, 0, 24, flat, unbanked },  { 22,   4, 0, 24, flat, unbanked },
             { 22,   3, 0, 24, flat, unbanked },  { 22,   2, 0, 24, flat, unbanked },  { 23,   1, 0, 24, flat, unbanked },  { 23,   0, 0, 24, flat, unbanked },
             { 23,   0, 0, 24, flat, unbanked },  { 22,  -1, 0, 24, flat, unbanked },  { 22,  -2, 0, 24, flat, unbanked },  { 22,  -3, 0, 24, flat, unbanked },
             { 21,  -4, 0, 24, flat, unbanked },  { 21,  -5, 0, 24, flat, unbanked },  { 21,  -6, 0, 24, flat, unbanked },  { 20,  -7, 0, 24, flat, unbanked },
             { 20,  -8, 0, 24, flat, unbanked },  { 20,  -9, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),
             { 20, -10, 0, 24, flat, unbanked },  { 20, -11, 0, 24, flat, unbanked },  { 19, -13, 0, 24, flat, unbanked },  { 18, -14, 0, 24, flat, unbanked },
             { 18, -15, 0, 24, flat, unbanked },  { 17, -16, 0, 24, flat, unbanked },  { 17, -17, 0, 24, flat, unbanked },  { 16, -18, 0, 24, flat, unbanked },
             { 16, -19, 0, 24, flat, unbanked },  { 15, -20, 0, 24, flat, unbanked },  { 15, -21, 0, 24, flat, unbanked },  { 14, -22, 0, 24, flat, unbanked },
             { 14, -23, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),  { 13, -24, 0, 24, flat, unbanked },
             { 13, -23, 0, 24, flat, unbanked },  { 13, -22, 0, 24, flat, unbanked },  { 13, -21, 0, 24, flat, unbanked },  { 13, -20, 0, 24, flat, unbanked },
             { 13, -19, 0, 24, flat, unbanked },  { 13, -18, 0, 24, flat, unbanked },  { 13, -17, 0, 24, flat, unbanked },  { 13, -16, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),  { 15, -29, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk6),
                          MINI_GOLF_STATE(Unk3),  { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_0, {
                          MINI_GOLF_STATE(Unk0),   {  31, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  26, 16, 0, 0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  26, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  25, 16, 0, 0, flat, unbanked },
              {  22, 16, 0, 0, flat, unbanked },   {  19, 15, 0, 0, flat, unbanked },   {  16, 15, 0, 0, flat, unbanked },   {  13, 14, 0, 0, flat, unbanked },
              {  10, 14, 0, 0, flat, unbanked },   {   7, 13, 0, 0, flat, unbanked },   {   4, 13, 0, 0, flat, unbanked },   {   1, 12, 0, 0, flat, unbanked },
              {  -2, 12, 0, 0, flat, unbanked },   {  -2, 15, 0, 0, flat, unbanked },   {  -2, 18, 0, 0, flat, unbanked },   {  -2, 21, 0, 0, flat, unbanked },
              {  -2, 24, 0, 0, flat, unbanked },   {  -2, 26, 0, 0, flat, unbanked },   {  -4, 26, 0, 0, flat, unbanked },   {  -6, 26, 0, 0, flat, unbanked },
              {  -8, 26, 0, 0, flat, unbanked },   { -10, 26, 0, 0, flat, unbanked },   { -11, 25, 0, 0, flat, unbanked },   { -11, 23, 0, 0, flat, unbanked },
              { -11, 21, 0, 0, flat, unbanked },   { -11, 19, 0, 0, flat, unbanked },   { -10, 17, 0, 0, flat, unbanked },   { -10, 15, 0, 0, flat, unbanked },
              { -10, 13, 0, 0, flat, unbanked },   { -12, 13, 0, 0, flat, unbanked },   { -14, 13, 0, 0, flat, unbanked },   { -16, 13, 0, 0, flat, unbanked },
              { -18, 13, 0, 0, flat, unbanked },   { -20, 14, 0, 0, flat, unbanked },   { -22, 14, 0, 0, flat, unbanked },   { -23, 14, 0, 0, flat, unbanked },
              { -24, 14, 0, 0, flat, unbanked },   { -25, 14, 0, 0, flat, unbanked },   { -26, 14, 0, 0, flat, unbanked },   { -27, 14, 0, 0, flat, unbanked },
              { -28, 14, 0, 0, flat, unbanked },   { -29, 14, 0, 0, flat, unbanked },   { -30, 14, 0, 0, flat, unbanked },   { -31, 14, 0, 0, flat, unbanked },
              { -30, 14, 0, 0, flat, unbanked },   { -29, 14, 0, 0, flat, unbanked },   { -28, 14, 0, 0, flat, unbanked },   { -27, 13, 0, 0, flat, unbanked },
              { -26, 13, 0, 0, flat, unbanked },   { -25, 13, 0, 0, flat, unbanked },   { -24, 13, 0, 0, flat, unbanked },   { -23, 13, 0, 0, flat, unbanked },
              { -22, 12, 0, 0, flat, unbanked },   { -21, 12, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),
              { -20, 12, 0, 0, flat, unbanked },   { -21, 12, 0, 0, flat, unbanked },   { -22, 13, 0, 0, flat, unbanked },   { -23, 14, 0, 0, flat, unbanked },
              { -24, 14, 0, 0, flat, unbanked },   { -25, 15, 0, 0, flat, unbanked },   { -26, 16, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk2),   { -29, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
              { -29, 16, 0, 0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_1, {
                          MINI_GOLF_STATE(Unk0),    { 16,  0, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16,  5, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),    { 16,  5, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),    { 16,  6, 0, 8, flat, unbanked },
               { 16,  9, 0, 8, flat, unbanked },    { 15, 12, 0, 8, flat, unbanked },    { 15, 15, 0, 8, flat, unbanked },    { 14, 18, 0, 8, flat, unbanked },
               { 14, 21, 0, 8, flat, unbanked },    { 13, 24, 0, 8, flat, unbanked },    { 13, 27, 0, 8, flat, unbanked },    { 12, 30, 0, 8, flat, unbanked },
               { 12, 33, 0, 8, flat, unbanked },    { 15, 33, 0, 8, flat, unbanked },    { 18, 33, 0, 8, flat, unbanked },    { 21, 33, 0, 8, flat, unbanked },
               { 24, 33, 0, 8, flat, unbanked },    { 26, 33, 0, 8, flat, unbanked },    { 26, 35, 0, 8, flat, unbanked },    { 26, 37, 0, 8, flat, unbanked },
               { 26, 39, 0, 8, flat, unbanked },    { 26, 41, 0, 8, flat, unbanked },    { 25, 42, 0, 8, flat, unbanked },    { 23, 42, 0, 8, flat, unbanked },
               { 21, 42, 0, 8, flat, unbanked },    { 19, 42, 0, 8, flat, unbanked },    { 17, 41, 0, 8, flat, unbanked },    { 15, 41, 0, 8, flat, unbanked },
               { 13, 41, 0, 8, flat, unbanked },    { 13, 43, 0, 8, flat, unbanked },    { 13, 45, 0, 8, flat, unbanked },    { 13, 47, 0, 8, flat, unbanked },
               { 13, 49, 0, 8, flat, unbanked },    { 14, 51, 0, 8, flat, unbanked },    { 14, 53, 0, 8, flat, unbanked },    { 14, 54, 0, 8, flat, unbanked },
               { 14, 55, 0, 8, flat, unbanked },    { 14, 56, 0, 8, flat, unbanked },    { 14, 57, 0, 8, flat, unbanked },    { 14, 58, 0, 8, flat, unbanked },
               { 14, 59, 0, 8, flat, unbanked },    { 14, 60, 0, 8, flat, unbanked },    { 14, 61, 0, 8, flat, unbanked },    { 14, 62, 0, 8, flat, unbanked },
               { 14, 61, 0, 8, flat, unbanked },    { 14, 60, 0, 8, flat, unbanked },    { 14, 59, 0, 8, flat, unbanked },    { 13, 58, 0, 8, flat, unbanked },
               { 13, 57, 0, 8, flat, unbanked },    { 13, 56, 0, 8, flat, unbanked },    { 13, 55, 0, 8, flat, unbanked },    { 13, 54, 0, 8, flat, unbanked },
               { 12, 53, 0, 8, flat, unbanked },    { 12, 52, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),
               { 12, 51, 0, 8, flat, unbanked },    { 12, 52, 0, 8, flat, unbanked },    { 13, 53, 0, 8, flat, unbanked },    { 14, 54, 0, 8, flat, unbanked },
               { 14, 55, 0, 8, flat, unbanked },    { 15, 56, 0, 8, flat, unbanked },    { 16, 57, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk2),    { 16, 60, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
               { 16, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  5, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  5, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  6, 15, 0, 16, flat, unbanked },
              {  9, 15, 0, 16, flat, unbanked },   { 12, 16, 0, 16, flat, unbanked },   { 15, 16, 0, 16, flat, unbanked },   { 18, 17, 0, 16, flat, unbanked },
              { 21, 17, 0, 16, flat, unbanked },   { 24, 18, 0, 16, flat, unbanked },   { 27, 18, 0, 16, flat, unbanked },   { 30, 19, 0, 16, flat, unbanked },
              { 33, 19, 0, 16, flat, unbanked },   { 33, 16, 0, 16, flat, unbanked },   { 33, 13, 0, 16, flat, unbanked },   { 33, 10, 0, 16, flat, unbanked },
              { 33,  7, 0, 16, flat, unbanked },   { 33,  5, 0, 16, flat, unbanked },   { 35,  5, 0, 16, flat, unbanked },   { 37,  5, 0, 16, flat, unbanked },
              { 39,  5, 0, 16, flat, unbanked },   { 41,  5, 0, 16, flat, unbanked },   { 42,  6, 0, 16, flat, unbanked },   { 42,  8, 0, 16, flat, unbanked },
              { 42, 10, 0, 16, flat, unbanked },   { 42, 12, 0, 16, flat, unbanked },   { 41, 14, 0, 16, flat, unbanked },   { 41, 16, 0, 16, flat, unbanked },
              { 41, 18, 0, 16, flat, unbanked },   { 43, 18, 0, 16, flat, unbanked },   { 45, 18, 0, 16, flat, unbanked },   { 47, 18, 0, 16, flat, unbanked },
              { 49, 18, 0, 16, flat, unbanked },   { 51, 17, 0, 16, flat, unbanked },   { 53, 17, 0, 16, flat, unbanked },   { 54, 17, 0, 16, flat, unbanked },
              { 55, 17, 0, 16, flat, unbanked },   { 56, 17, 0, 16, flat, unbanked },   { 57, 17, 0, 16, flat, unbanked },   { 58, 17, 0, 16, flat, unbanked },
              { 59, 17, 0, 16, flat, unbanked },   { 60, 17, 0, 16, flat, unbanked },   { 61, 17, 0, 16, flat, unbanked },   { 62, 17, 0, 16, flat, unbanked },
              { 61, 17, 0, 16, flat, unbanked },   { 60, 17, 0, 16, flat, unbanked },   { 59, 17, 0, 16, flat, unbanked },   { 58, 18, 0, 16, flat, unbanked },
              { 57, 18, 0, 16, flat, unbanked },   { 56, 18, 0, 16, flat, unbanked },   { 55, 18, 0, 16, flat, unbanked },   { 54, 18, 0, 16, flat, unbanked },
              { 53, 19, 0, 16, flat, unbanked },   { 52, 19, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),
              { 51, 19, 0, 16, flat, unbanked },   { 52, 19, 0, 16, flat, unbanked },   { 53, 18, 0, 16, flat, unbanked },   { 54, 17, 0, 16, flat, unbanked },
              { 55, 17, 0, 16, flat, unbanked },   { 56, 16, 0, 16, flat, unbanked },   { 57, 15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk2),   { 60, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
              { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleB_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15,  25, 0, 24, flat, unbanked },
             { 15,  22, 0, 24, flat, unbanked },  { 16,  19, 0, 24, flat, unbanked },  { 16,  16, 0, 24, flat, unbanked },  { 17,  13, 0, 24, flat, unbanked },
             { 17,  10, 0, 24, flat, unbanked },  { 18,   7, 0, 24, flat, unbanked },  { 18,   4, 0, 24, flat, unbanked },  { 19,   1, 0, 24, flat, unbanked },
             { 19,  -2, 0, 24, flat, unbanked },  { 16,  -2, 0, 24, flat, unbanked },  { 13,  -2, 0, 24, flat, unbanked },  { 10,  -2, 0, 24, flat, unbanked },
             {  7,  -2, 0, 24, flat, unbanked },  {  5,  -2, 0, 24, flat, unbanked },  {  5,  -4, 0, 24, flat, unbanked },  {  5,  -6, 0, 24, flat, unbanked },
             {  5,  -8, 0, 24, flat, unbanked },  {  5, -10, 0, 24, flat, unbanked },  {  6, -11, 0, 24, flat, unbanked },  {  8, -11, 0, 24, flat, unbanked },
             { 10, -11, 0, 24, flat, unbanked },  { 12, -11, 0, 24, flat, unbanked },  { 14, -10, 0, 24, flat, unbanked },  { 16, -10, 0, 24, flat, unbanked },
             { 18, -10, 0, 24, flat, unbanked },  { 18, -12, 0, 24, flat, unbanked },  { 18, -14, 0, 24, flat, unbanked },  { 18, -16, 0, 24, flat, unbanked },
             { 18, -18, 0, 24, flat, unbanked },  { 17, -20, 0, 24, flat, unbanked },  { 17, -22, 0, 24, flat, unbanked },  { 17, -23, 0, 24, flat, unbanked },
             { 17, -24, 0, 24, flat, unbanked },  { 17, -25, 0, 24, flat, unbanked },  { 17, -26, 0, 24, flat, unbanked },  { 17, -27, 0, 24, flat, unbanked },
             { 17, -28, 0, 24, flat, unbanked },  { 17, -29, 0, 24, flat, unbanked },  { 17, -30, 0, 24, flat, unbanked },  { 17, -31, 0, 24, flat, unbanked },
             { 17, -30, 0, 24, flat, unbanked },  { 17, -29, 0, 24, flat, unbanked },  { 17, -28, 0, 24, flat, unbanked },  { 18, -27, 0, 24, flat, unbanked },
             { 18, -26, 0, 24, flat, unbanked },  { 18, -25, 0, 24, flat, unbanked },  { 18, -24, 0, 24, flat, unbanked },  { 18, -23, 0, 24, flat, unbanked },
             { 19, -22, 0, 24, flat, unbanked },  { 19, -21, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),
             { 19, -20, 0, 24, flat, unbanked },  { 19, -21, 0, 24, flat, unbanked },  { 18, -22, 0, 24, flat, unbanked },  { 17, -23, 0, 24, flat, unbanked },
             { 17, -24, 0, 24, flat, unbanked },  { 16, -25, 0, 24, flat, unbanked },  { 15, -26, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk2),  { 15, -29, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
             { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_0, {
                          MINI_GOLF_STATE(Unk0),   {  31, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  26, 16, 0, 0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  26, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  24, 16, 0, 0, flat, unbanked },
              {  21, 15, 0, 0, flat, unbanked },   {  18, 14, 0, 0, flat, unbanked },   {  16, 14, 0, 0, flat, unbanked },   {  14, 14, 0, 0, flat, unbanked },
              {  12, 13, 0, 0, flat, unbanked },   {  10, 13, 0, 0, flat, unbanked },   {   9, 13, 0, 0, flat, unbanked },   {  11, 12, 0, 0, flat, unbanked },
              {  12, 12, 0, 0, flat, unbanked },   {  13, 12, 0, 0, flat, unbanked },   {  14, 12, 0, 0, flat, unbanked },   {  15, 12, 0, 0, flat, unbanked },
              {  16, 12, 0, 0, flat, unbanked },   {  17, 11, 0, 0, flat, unbanked },   {  18, 11, 0, 0, flat, unbanked },   {  19, 11, 0, 0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   {  20, 11, 0, 0, flat, unbanked },   {  18, 11, 0, 0, flat, unbanked },
              {  16, 11, 0, 0, flat, unbanked },   {  14, 11, 0, 0, flat, unbanked },   {  12, 11, 0, 0, flat, unbanked },   {  10, 11, 0, 0, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   {   7, 12, 0, 0, flat, unbanked },   {   5, 13, 0, 0, flat, unbanked },   {   3, 14, 0, 0, flat, unbanked },
              {   1, 15, 0, 0, flat, unbanked },   {  -1, 16, 0, 0, flat, unbanked },   {  -3, 17, 0, 0, flat, unbanked },   {  -5, 18, 0, 0, flat, unbanked },
              {  -7, 20, 0, 0, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   {  -9, 20, 0, 0, flat, unbanked },   { -11, 20, 0, 0, flat, unbanked },
              { -13, 20, 0, 0, flat, unbanked },   { -15, 20, 0, 0, flat, unbanked },   { -17, 20, 0, 0, flat, unbanked },   { -19, 20, 0, 0, flat, unbanked },
              { -21, 20, 0, 0, flat, unbanked },   { -22, 20, 0, 0, flat, unbanked },   { -23, 20, 0, 0, flat, unbanked },   { -24, 20, 0, 0, flat, unbanked },
              { -25, 20, 0, 0, flat, unbanked },   { -26, 20, 0, 0, flat, unbanked },   { -27, 20, 0, 0, flat, unbanked },   { -28, 20, 0, 0, flat, unbanked },
              { -29, 20, 0, 0, flat, unbanked },   { -30, 20, 0, 0, flat, unbanked },   { -29, 20, 0, 0, flat, unbanked },   { -28, 20, 0, 0, flat, unbanked },
              { -27, 20, 0, 0, flat, unbanked },   { -26, 20, 0, 0, flat, unbanked },   { -25, 20, 0, 0, flat, unbanked },   { -24, 20, 0, 0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   { -23, 20, 0, 0, flat, unbanked },   { -23, 19, 0, 0, flat, unbanked },
              { -23, 18, 0, 0, flat, unbanked },   { -23, 17, 0, 0, flat, unbanked },   { -23, 16, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk2),   { -29, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
              { -29, 16, 0, 0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_1, {
                          MINI_GOLF_STATE(Unk0),    { 16,  0, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),    { 16,  5, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),    { 16,  5, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),    { 16,  7, 0, 8, flat, unbanked },
               { 15, 10, 0, 8, flat, unbanked },    { 14, 13, 0, 8, flat, unbanked },    { 14, 15, 0, 8, flat, unbanked },    { 14, 17, 0, 8, flat, unbanked },
               { 13, 19, 0, 8, flat, unbanked },    { 13, 21, 0, 8, flat, unbanked },    { 13, 22, 0, 8, flat, unbanked },    { 12, 20, 0, 8, flat, unbanked },
               { 12, 19, 0, 8, flat, unbanked },    { 12, 18, 0, 8, flat, unbanked },    { 12, 17, 0, 8, flat, unbanked },    { 12, 16, 0, 8, flat, unbanked },
               { 12, 15, 0, 8, flat, unbanked },    { 11, 14, 0, 8, flat, unbanked },    { 11, 13, 0, 8, flat, unbanked },    { 11, 12, 0, 8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),    { 11, 11, 0, 8, flat, unbanked },    { 11, 13, 0, 8, flat, unbanked },
               { 11, 15, 0, 8, flat, unbanked },    { 11, 17, 0, 8, flat, unbanked },    { 11, 19, 0, 8, flat, unbanked },    { 11, 21, 0, 8, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),    { 12, 24, 0, 8, flat, unbanked },    { 13, 26, 0, 8, flat, unbanked },    { 14, 28, 0, 8, flat, unbanked },
               { 15, 30, 0, 8, flat, unbanked },    { 16, 32, 0, 8, flat, unbanked },    { 17, 34, 0, 8, flat, unbanked },    { 18, 36, 0, 8, flat, unbanked },
               { 20, 38, 0, 8, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),    { 20, 40, 0, 8, flat, unbanked },    { 20, 42, 0, 8, flat, unbanked },
               { 20, 44, 0, 8, flat, unbanked },    { 20, 46, 0, 8, flat, unbanked },    { 20, 48, 0, 8, flat, unbanked },    { 20, 50, 0, 8, flat, unbanked },
               { 20, 52, 0, 8, flat, unbanked },    { 20, 53, 0, 8, flat, unbanked },    { 20, 54, 0, 8, flat, unbanked },    { 20, 55, 0, 8, flat, unbanked },
               { 20, 56, 0, 8, flat, unbanked },    { 20, 57, 0, 8, flat, unbanked },    { 20, 58, 0, 8, flat, unbanked },    { 20, 59, 0, 8, flat, unbanked },
               { 20, 60, 0, 8, flat, unbanked },    { 20, 61, 0, 8, flat, unbanked },    { 20, 60, 0, 8, flat, unbanked },    { 20, 59, 0, 8, flat, unbanked },
               { 20, 58, 0, 8, flat, unbanked },    { 20, 57, 0, 8, flat, unbanked },    { 20, 56, 0, 8, flat, unbanked },    { 20, 55, 0, 8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),    { 20, 54, 0, 8, flat, unbanked },    { 19, 54, 0, 8, flat, unbanked },
               { 18, 54, 0, 8, flat, unbanked },    { 17, 54, 0, 8, flat, unbanked },    { 16, 54, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk2),    { 16, 60, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
               { 16, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  5, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  5, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  7, 15, 0, 16, flat, unbanked },
              { 10, 16, 0, 16, flat, unbanked },   { 13, 17, 0, 16, flat, unbanked },   { 15, 17, 0, 16, flat, unbanked },   { 17, 17, 0, 16, flat, unbanked },
              { 19, 18, 0, 16, flat, unbanked },   { 21, 18, 0, 16, flat, unbanked },   { 22, 18, 0, 16, flat, unbanked },   { 20, 19, 0, 16, flat, unbanked },
              { 19, 19, 0, 16, flat, unbanked },   { 18, 19, 0, 16, flat, unbanked },   { 17, 19, 0, 16, flat, unbanked },   { 16, 19, 0, 16, flat, unbanked },
              { 15, 19, 0, 16, flat, unbanked },   { 14, 20, 0, 16, flat, unbanked },   { 13, 20, 0, 16, flat, unbanked },   { 12, 20, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   { 11, 20, 0, 16, flat, unbanked },   { 13, 20, 0, 16, flat, unbanked },
              { 15, 20, 0, 16, flat, unbanked },   { 17, 20, 0, 16, flat, unbanked },   { 19, 20, 0, 16, flat, unbanked },   { 21, 20, 0, 16, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),   { 24, 19, 0, 16, flat, unbanked },   { 26, 18, 0, 16, flat, unbanked },   { 28, 17, 0, 16, flat, unbanked },
              { 30, 16, 0, 16, flat, unbanked },   { 32, 15, 0, 16, flat, unbanked },   { 34, 14, 0, 16, flat, unbanked },   { 36, 13, 0, 16, flat, unbanked },
              { 38, 11, 0, 16, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { 40, 11, 0, 16, flat, unbanked },   { 42, 11, 0, 16, flat, unbanked },
              { 44, 11, 0, 16, flat, unbanked },   { 46, 11, 0, 16, flat, unbanked },   { 48, 11, 0, 16, flat, unbanked },   { 50, 11, 0, 16, flat, unbanked },
              { 52, 11, 0, 16, flat, unbanked },   { 53, 11, 0, 16, flat, unbanked },   { 54, 11, 0, 16, flat, unbanked },   { 55, 11, 0, 16, flat, unbanked },
              { 56, 11, 0, 16, flat, unbanked },   { 57, 11, 0, 16, flat, unbanked },   { 58, 11, 0, 16, flat, unbanked },   { 59, 11, 0, 16, flat, unbanked },
              { 60, 11, 0, 16, flat, unbanked },   { 61, 11, 0, 16, flat, unbanked },   { 60, 11, 0, 16, flat, unbanked },   { 59, 11, 0, 16, flat, unbanked },
              { 58, 11, 0, 16, flat, unbanked },   { 57, 11, 0, 16, flat, unbanked },   { 56, 11, 0, 16, flat, unbanked },   { 55, 11, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   { 54, 11, 0, 16, flat, unbanked },   { 54, 12, 0, 16, flat, unbanked },
              { 54, 13, 0, 16, flat, unbanked },   { 54, 14, 0, 16, flat, unbanked },   { 54, 15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk2),   { 60, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
              { 60, 15, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleC_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15,  24, 0, 24, flat, unbanked },
             { 16,  21, 0, 24, flat, unbanked },  { 17,  18, 0, 24, flat, unbanked },  { 17,  16, 0, 24, flat, unbanked },  { 17,  14, 0, 24, flat, unbanked },
             { 18,  12, 0, 24, flat, unbanked },  { 18,  10, 0, 24, flat, unbanked },  { 18,   9, 0, 24, flat, unbanked },  { 19,  11, 0, 24, flat, unbanked },
             { 19,  12, 0, 24, flat, unbanked },  { 19,  13, 0, 24, flat, unbanked },  { 19,  14, 0, 24, flat, unbanked },  { 19,  15, 0, 24, flat, unbanked },
             { 19,  16, 0, 24, flat, unbanked },  { 20,  17, 0, 24, flat, unbanked },  { 20,  18, 0, 24, flat, unbanked },  { 20,  19, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),  { 20,  20, 0, 24, flat, unbanked },  { 20,  18, 0, 24, flat, unbanked },
             { 20,  16, 0, 24, flat, unbanked },  { 20,  14, 0, 24, flat, unbanked },  { 20,  12, 0, 24, flat, unbanked },  { 20,  10, 0, 24, flat, unbanked },
                      MINI_GOLF_ANIMATION(Walk),  { 19,   7, 0, 24, flat, unbanked },  { 18,   5, 0, 24, flat, unbanked },  { 17,   3, 0, 24, flat, unbanked },
             { 16,   1, 0, 24, flat, unbanked },  { 15,  -1, 0, 24, flat, unbanked },  { 14,  -3, 0, 24, flat, unbanked },  { 13,  -5, 0, 24, flat, unbanked },
             { 11,  -7, 0, 24, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),  { 11,  -9, 0, 24, flat, unbanked },  { 11, -11, 0, 24, flat, unbanked },
             { 11, -13, 0, 24, flat, unbanked },  { 11, -15, 0, 24, flat, unbanked },  { 11, -17, 0, 24, flat, unbanked },  { 11, -19, 0, 24, flat, unbanked },
             { 11, -21, 0, 24, flat, unbanked },  { 11, -22, 0, 24, flat, unbanked },  { 11, -23, 0, 24, flat, unbanked },  { 11, -24, 0, 24, flat, unbanked },
             { 11, -25, 0, 24, flat, unbanked },  { 11, -26, 0, 24, flat, unbanked },  { 11, -27, 0, 24, flat, unbanked },  { 11, -28, 0, 24, flat, unbanked },
             { 11, -29, 0, 24, flat, unbanked },  { 11, -30, 0, 24, flat, unbanked },  { 11, -29, 0, 24, flat, unbanked },  { 11, -28, 0, 24, flat, unbanked },
             { 11, -27, 0, 24, flat, unbanked },  { 11, -26, 0, 24, flat, unbanked },  { 11, -25, 0, 24, flat, unbanked },  { 11, -24, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),  { 11, -23, 0, 24, flat, unbanked },  { 12, -23, 0, 24, flat, unbanked },
             { 13, -23, 0, 24, flat, unbanked },  { 14, -23, 0, 24, flat, unbanked },  { 15, -23, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
                          MINI_GOLF_STATE(Unk2),  { 15, -29, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),
             { 15, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_0, {
                          MINI_GOLF_STATE(Unk0),   {  31, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  26, 16, 0, 0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  26, 16, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  24, 16, 0, 0, flat, unbanked },
              {  21, 17, 0, 0, flat, unbanked },   {  19, 17, 0, 0, flat, unbanked },   {  17, 17, 0, 0, flat, unbanked },   {  15, 16, 0, 0, flat, unbanked },
              {  13, 18, 0, 0, flat, unbanked },   {  11, 18, 0, 0, flat, unbanked },   {   9, 18, 0, 0, flat, unbanked },   {   7, 18, 0, 0, flat, unbanked },
              {   5, 18, 0, 0, flat, unbanked },   {   4, 18, 0, 0, flat, unbanked },   {   3, 19, 0, 0, flat, unbanked },   {   2, 19, 0, 0, flat, unbanked },
              {   1, 19, 0, 0, flat, unbanked },   {   0, 19, 0, 0, flat, unbanked },   {  -1, 19, 0, 0, flat, unbanked },   {  -2, 19, 0, 0, flat, unbanked },
              {  -1, 19, 0, 0, flat, unbanked },   {   0, 19, 0, 0, flat, unbanked },   {   1, 19, 0, 0, flat, unbanked },   {   2, 19, 0, 0, flat, unbanked },
              {   3, 19, 0, 0, flat, unbanked },   {   4, 19, 0, 0, flat, unbanked },   {   5, 19, 0, 0, flat, unbanked },   {   6, 20, 0, 0, flat, unbanked },
              {   7, 20, 0, 0, flat, unbanked },   {   8, 20, 0, 0, flat, unbanked },   {   9, 20, 0, 0, flat, unbanked },   {  10, 20, 0, 0, flat, unbanked },
              {  11, 20, 0, 0, flat, unbanked },   {  12, 20, 0, 0, flat, unbanked },   {  13, 20, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   {  14, 20, 0, 0, flat, unbanked },   {  12, 20, 0, 0, flat, unbanked },   {   9, 20, 0, 0, flat, unbanked },
              {   6, 20, 0, 0, flat, unbanked },   {   3, 20, 0, 0, flat, unbanked },   {   0, 20, 0, 0, flat, unbanked },   {  -3, 20, 0, 0, flat, unbanked },
              {  -6, 20, 0, 0, flat, unbanked },   {  -9, 20, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { -12, 20, 0, 0, flat, unbanked },
              { -15, 20, 0, 0, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { -18, 16, 0, 0, flat, unbanked },   { -21, 16, 0, 0, flat, unbanked },
              { -24, 16, 0, 0, flat, unbanked },   { -25, 15, 0, 0, flat, unbanked },   { -26, 13, 0, 0, flat, unbanked },   { -27, 11, 0, 0, flat, unbanked },
              { -28,  9, 0, 0, flat, unbanked },   { -26,  7, 0, 0, flat, unbanked },   { -25,  6, 0, 0, flat, unbanked },   { -24,  5, 1, 0, flat, unbanked },
              { -23,  4, 1, 0, flat, unbanked },   { -22,  3, 1, 0, flat, unbanked },   { -21,  2, 1, 0, flat, unbanked },   { -20,  2, 1, 0, flat, unbanked },
              { -19,  2, 1, 0, flat, unbanked },   { -18,  2, 1, 0, flat, unbanked },   { -17,  2, 1, 0, flat, unbanked },   { -17,  2, 1, 0, flat, unbanked },
              { -17,  2, 1, 0, flat, unbanked },   { -18,  2, 1, 0, flat, unbanked },   { -19,  2, 1, 0, flat, unbanked },   { -20,  2, 1, 0, flat, unbanked },
              { -21,  2, 1, 0, flat, unbanked },   { -22,  3, 1, 0, flat, unbanked },   { -23,  4, 1, 0, flat, unbanked },   { -24,  5, 1, 0, flat, unbanked },
              { -25,  6, 0, 0, flat, unbanked },   { -26,  7, 0, 0, flat, unbanked },   { -28,  9, 0, 0, flat, unbanked },   { -27, 11, 0, 0, flat, unbanked },
              { -26, 13, 0, 0, flat, unbanked },   { -25, 15, 0, 0, flat, unbanked },   { -24, 16, 0, 0, flat, unbanked },   { -21, 16, 0, 0, flat, unbanked },
              { -18, 16, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { -15, 20, 0, 0, flat, unbanked },   { -12, 20, 0, 0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  -9, 20, 0, 0, flat, unbanked },   {  -8, 20, 0, 0, flat, unbanked },   {  -7, 20, 0, 0, flat, unbanked },
              {  -6, 20, 0, 0, flat, unbanked },   {  -5, 20, 0, 0, flat, unbanked },   {  -4, 20, 0, 0, flat, unbanked },   {  -3, 20, 0, 0, flat, unbanked },
              {  -2, 20, 0, 0, flat, unbanked },   {  -1, 20, 0, 0, flat, unbanked },   {   0, 20, 0, 0, flat, unbanked },   {   1, 20, 0, 0, flat, unbanked },
              {   2, 20, 0, 0, flat, unbanked },   {   3, 20, 0, 0, flat, unbanked },   {   4, 20, 0, 0, flat, unbanked },   {   5, 20, 0, 0, flat, unbanked },
              {   6, 20, 0, 0, flat, unbanked },   {   7, 20, 0, 0, flat, unbanked },   {   8, 20, 0, 0, flat, unbanked },   {   9, 20, 0, 0, flat, unbanked },
              {  10, 20, 0, 0, flat, unbanked },   {  11, 20, 0, 0, flat, unbanked },   {  12, 20, 0, 0, flat, unbanked },   {  13, 20, 0, 0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   {  14, 20, 0, 0, flat, unbanked },   {  12, 20, 0, 0, flat, unbanked },
              {   9, 20, 0, 0, flat, unbanked },   {   6, 20, 0, 0, flat, unbanked },   {   3, 20, 0, 0, flat, unbanked },   {   0, 20, 0, 0, flat, unbanked },
              {  -3, 20, 0, 0, flat, unbanked },   {  -6, 20, 0, 0, flat, unbanked },   {  -9, 20, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { -12, 20, 0, 0, flat, unbanked },   { -15, 20, 0, 0, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { -18, 16, 0, 0, flat, unbanked },
              { -21, 16, 0, 0, flat, unbanked },   { -24, 16, 0, 0, flat, unbanked },   { -25, 15, 0, 0, flat, unbanked },   { -26, 13, 0, 0, flat, unbanked },
              { -27, 11, 0, 0, flat, unbanked },   { -28,  9, 0, 0, flat, unbanked },   { -26,  7, 0, 0, flat, unbanked },   { -24,  5, 1, 0, flat, unbanked },
              { -22,  3, 1, 0, flat, unbanked },   { -20,  2, 1, 0, flat, unbanked },   { -18,  2, 1, 0, flat, unbanked },   { -16,  3, 2, 0, flat, unbanked },
              { -14,  3, 2, 0, flat, unbanked },   { -14,  5, 2, 0, flat, unbanked },   { -13,  7, 2, 0, flat, unbanked },   { -12,  9, 3, 0, flat, unbanked },
              { -12, 11, 3, 0, flat, unbanked },   { -11, 13, 3, 0, flat, unbanked },   { -12, 15, 3, 0, flat, unbanked },   { -12, 17, 3, 0, flat, unbanked },
              { -12, 19, 3, 0, flat, unbanked },   { -13, 21, 3, 0, flat, unbanked },   { -13, 23, 3, 0, flat, unbanked },   { -13, 25, 3, 0, flat, unbanked },
              { -14, 27, 3, 0, flat, unbanked },   { -14, 29, 3, 0, flat, unbanked },   { -14, 30, 3, 0, flat, unbanked },   { -14, 31, 3, 0, flat, unbanked },
              { -15, 32, 3, 0, flat, unbanked },   { -15, 33, 3, 0, flat, unbanked },   { -15, 34, 3, 0, flat, unbanked },   { -15, 35, 3, 0, flat, unbanked },
              { -15, 36, 3, 0, flat, unbanked },   { -15, 38, 0, 0, flat, unbanked },   { -16, 40, 0, 0, flat, unbanked },   { -16, 41, 0, 0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   { -16, 42, 0, 0, flat, unbanked },   { -16, 43, 0, 0, flat, unbanked },
              { -16, 44, 0, 0, flat, unbanked },   { -16, 45, 0, 0, flat, unbanked },   { -17, 46, 0, 0, flat, unbanked },   { -17, 47, 0, 0, flat, unbanked },
              { -17, 48, 0, 0, flat, unbanked },   { -17, 49, 0, 0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
              { -17, 60, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),   { -17, 60, 0, 8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_1, {
                          MINI_GOLF_STATE(Unk0),   { 16,  0, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   { 16,  5, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   { 16,  5, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk3),   { 16,  7, 0,  8, flat, unbanked },
              { 17, 10, 0,  8, flat, unbanked },   { 17, 12, 0,  8, flat, unbanked },   { 17, 14, 0,  8, flat, unbanked },   { 16, 16, 0,  8, flat, unbanked },
              { 18, 18, 0,  8, flat, unbanked },   { 18, 20, 0,  8, flat, unbanked },   { 18, 22, 0,  8, flat, unbanked },   { 18, 24, 0,  8, flat, unbanked },
              { 18, 26, 0,  8, flat, unbanked },   { 18, 27, 0,  8, flat, unbanked },   { 19, 28, 0,  8, flat, unbanked },   { 19, 29, 0,  8, flat, unbanked },
              { 19, 30, 0,  8, flat, unbanked },   { 19, 31, 0,  8, flat, unbanked },   { 19, 32, 0,  8, flat, unbanked },   { 19, 33, 0,  8, flat, unbanked },
              { 19, 32, 0,  8, flat, unbanked },   { 19, 31, 0,  8, flat, unbanked },   { 19, 30, 0,  8, flat, unbanked },   { 19, 29, 0,  8, flat, unbanked },
              { 19, 28, 0,  8, flat, unbanked },   { 19, 27, 0,  8, flat, unbanked },   { 19, 26, 0,  8, flat, unbanked },   { 20, 25, 0,  8, flat, unbanked },
              { 20, 24, 0,  8, flat, unbanked },   { 20, 23, 0,  8, flat, unbanked },   { 20, 22, 0,  8, flat, unbanked },   { 20, 21, 0,  8, flat, unbanked },
              { 20, 20, 0,  8, flat, unbanked },   { 20, 19, 0,  8, flat, unbanked },   { 20, 18, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   { 20, 17, 0,  8, flat, unbanked },   { 20, 19, 0,  8, flat, unbanked },   { 20, 22, 0,  8, flat, unbanked },
              { 20, 25, 0,  8, flat, unbanked },   { 20, 28, 0,  8, flat, unbanked },   { 20, 31, 0,  8, flat, unbanked },   { 20, 34, 0,  8, flat, unbanked },
              { 20, 37, 0,  8, flat, unbanked },   { 20, 40, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 20, 43, 0,  8, flat, unbanked },
              { 20, 46, 0,  8, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { 16, 49, 0,  8, flat, unbanked },   { 16, 52, 0,  8, flat, unbanked },
              { 16, 55, 0,  8, flat, unbanked },   { 15, 56, 0,  8, flat, unbanked },   { 13, 57, 0,  8, flat, unbanked },   { 11, 58, 0,  8, flat, unbanked },
              {  9, 59, 0,  8, flat, unbanked },   {  7, 57, 0,  8, flat, unbanked },   {  6, 56, 0,  8, flat, unbanked },   {  5, 55, 1,  8, flat, unbanked },
              {  4, 54, 1,  8, flat, unbanked },   {  3, 53, 1,  8, flat, unbanked },   {  2, 52, 1,  8, flat, unbanked },   {  2, 51, 1,  8, flat, unbanked },
              {  2, 50, 1,  8, flat, unbanked },   {  2, 49, 1,  8, flat, unbanked },   {  2, 48, 1,  8, flat, unbanked },   {  2, 48, 1,  8, flat, unbanked },
              {  2, 48, 1,  8, flat, unbanked },   {  2, 49, 1,  8, flat, unbanked },   {  2, 50, 1,  8, flat, unbanked },   {  2, 51, 1,  8, flat, unbanked },
              {  2, 52, 1,  8, flat, unbanked },   {  3, 53, 1,  8, flat, unbanked },   {  4, 54, 1,  8, flat, unbanked },   {  5, 55, 1,  8, flat, unbanked },
              {  6, 56, 0,  8, flat, unbanked },   {  7, 57, 0,  8, flat, unbanked },   {  9, 59, 0,  8, flat, unbanked },   { 11, 58, 0,  8, flat, unbanked },
              { 13, 57, 0,  8, flat, unbanked },   { 15, 56, 0,  8, flat, unbanked },   { 16, 55, 0,  8, flat, unbanked },   { 16, 52, 0,  8, flat, unbanked },
              { 16, 49, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 20, 46, 0,  8, flat, unbanked },   { 20, 43, 0,  8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   { 20, 40, 0,  8, flat, unbanked },   { 20, 39, 0,  8, flat, unbanked },   { 20, 38, 0,  8, flat, unbanked },
              { 20, 37, 0,  8, flat, unbanked },   { 20, 36, 0,  8, flat, unbanked },   { 20, 35, 0,  8, flat, unbanked },   { 20, 34, 0,  8, flat, unbanked },
              { 20, 33, 0,  8, flat, unbanked },   { 20, 32, 0,  8, flat, unbanked },   { 20, 31, 0,  8, flat, unbanked },   { 20, 30, 0,  8, flat, unbanked },
              { 20, 29, 0,  8, flat, unbanked },   { 20, 28, 0,  8, flat, unbanked },   { 20, 27, 0,  8, flat, unbanked },   { 20, 26, 0,  8, flat, unbanked },
              { 20, 25, 0,  8, flat, unbanked },   { 20, 24, 0,  8, flat, unbanked },   { 20, 23, 0,  8, flat, unbanked },   { 20, 22, 0,  8, flat, unbanked },
              { 20, 21, 0,  8, flat, unbanked },   { 20, 20, 0,  8, flat, unbanked },   { 20, 19, 0,  8, flat, unbanked },   { 20, 18, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   { 20, 17, 0,  8, flat, unbanked },   { 20, 19, 0,  8, flat, unbanked },
              { 20, 22, 0,  8, flat, unbanked },   { 20, 25, 0,  8, flat, unbanked },   { 20, 28, 0,  8, flat, unbanked },   { 20, 31, 0,  8, flat, unbanked },
              { 20, 34, 0,  8, flat, unbanked },   { 20, 37, 0,  8, flat, unbanked },   { 20, 40, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { 20, 43, 0,  8, flat, unbanked },   { 20, 46, 0,  8, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { 16, 49, 0,  8, flat, unbanked },
              { 16, 52, 0,  8, flat, unbanked },   { 16, 55, 0,  8, flat, unbanked },   { 15, 56, 0,  8, flat, unbanked },   { 13, 57, 0,  8, flat, unbanked },
              { 11, 58, 0,  8, flat, unbanked },   {  9, 59, 0,  8, flat, unbanked },   {  7, 57, 0,  8, flat, unbanked },   {  5, 55, 1,  8, flat, unbanked },
              {  3, 53, 1,  8, flat, unbanked },   {  2, 51, 1,  8, flat, unbanked },   {  2, 49, 1,  8, flat, unbanked },   {  3, 47, 2,  8, flat, unbanked },
              {  3, 45, 2,  8, flat, unbanked },   {  5, 45, 2,  8, flat, unbanked },   {  7, 44, 2,  8, flat, unbanked },   {  9, 43, 3,  8, flat, unbanked },
              { 11, 43, 3,  8, flat, unbanked },   { 13, 42, 3,  8, flat, unbanked },   { 15, 43, 3,  8, flat, unbanked },   { 17, 43, 3,  8, flat, unbanked },
              { 19, 43, 3,  8, flat, unbanked },   { 21, 44, 3,  8, flat, unbanked },   { 23, 44, 3,  8, flat, unbanked },   { 25, 44, 3,  8, flat, unbanked },
              { 27, 45, 3,  8, flat, unbanked },   { 29, 45, 3,  8, flat, unbanked },   { 30, 45, 3,  8, flat, unbanked },   { 31, 45, 3,  8, flat, unbanked },
              { 32, 46, 3,  8, flat, unbanked },   { 33, 46, 3,  8, flat, unbanked },   { 34, 46, 3,  8, flat, unbanked },   { 35, 46, 3,  8, flat, unbanked },
              { 36, 46, 3,  8, flat, unbanked },   { 38, 46, 0,  8, flat, unbanked },   { 40, 47, 0,  8, flat, unbanked },   { 41, 47, 0,  8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   { 42, 47, 0,  8, flat, unbanked },   { 43, 47, 0,  8, flat, unbanked },
              { 44, 47, 0,  8, flat, unbanked },   { 45, 47, 0,  8, flat, unbanked },   { 46, 48, 0,  8, flat, unbanked },   { 47, 48, 0,  8, flat, unbanked },
              { 48, 48, 0,  8, flat, unbanked },   { 49, 48, 0,  8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
              { 60, 48, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),   { 60, 48, 0, 16, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_2, {
                          MINI_GOLF_STATE(Unk0),  {  0,  15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),  {  5,  15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  {  5,  15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),  {  7,  15, 0, 16, flat, unbanked },
             { 10,  14, 0, 16, flat, unbanked },  { 12,  14, 0, 16, flat, unbanked },  { 14,  14, 0, 16, flat, unbanked },  { 16,  15, 0, 16, flat, unbanked },
             { 18,  13, 0, 16, flat, unbanked },  { 20,  13, 0, 16, flat, unbanked },  { 22,  13, 0, 16, flat, unbanked },  { 24,  13, 0, 16, flat, unbanked },
             { 26,  13, 0, 16, flat, unbanked },  { 27,  13, 0, 16, flat, unbanked },  { 28,  12, 0, 16, flat, unbanked },  { 29,  12, 0, 16, flat, unbanked },
             { 30,  12, 0, 16, flat, unbanked },  { 31,  12, 0, 16, flat, unbanked },  { 32,  12, 0, 16, flat, unbanked },  { 33,  12, 0, 16, flat, unbanked },
             { 32,  12, 0, 16, flat, unbanked },  { 31,  12, 0, 16, flat, unbanked },  { 30,  12, 0, 16, flat, unbanked },  { 29,  12, 0, 16, flat, unbanked },
             { 28,  12, 0, 16, flat, unbanked },  { 27,  12, 0, 16, flat, unbanked },  { 26,  12, 0, 16, flat, unbanked },  { 25,  11, 0, 16, flat, unbanked },
             { 24,  11, 0, 16, flat, unbanked },  { 23,  11, 0, 16, flat, unbanked },  { 22,  11, 0, 16, flat, unbanked },  { 21,  11, 0, 16, flat, unbanked },
             { 20,  11, 0, 16, flat, unbanked },  { 19,  11, 0, 16, flat, unbanked },  { 18,  11, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),  { 17,  11, 0, 16, flat, unbanked },  { 19,  11, 0, 16, flat, unbanked },  { 22,  11, 0, 16, flat, unbanked },
             { 25,  11, 0, 16, flat, unbanked },  { 28,  11, 0, 16, flat, unbanked },  { 31,  11, 0, 16, flat, unbanked },  { 34,  11, 0, 16, flat, unbanked },
             { 37,  11, 0, 16, flat, unbanked },  { 40,  11, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 43,  11, 0, 16, flat, unbanked },
             { 46,  11, 0, 16, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),  { 49,  15, 0, 16, flat, unbanked },  { 52,  15, 0, 16, flat, unbanked },
             { 55,  15, 0, 16, flat, unbanked },  { 56,  16, 0, 16, flat, unbanked },  { 57,  18, 0, 16, flat, unbanked },  { 58,  20, 0, 16, flat, unbanked },
             { 59,  22, 0, 16, flat, unbanked },  { 57,  24, 0, 16, flat, unbanked },  { 56,  25, 0, 16, flat, unbanked },  { 55,  26, 1, 16, flat, unbanked },
             { 54,  27, 1, 16, flat, unbanked },  { 53,  28, 1, 16, flat, unbanked },  { 52,  29, 1, 16, flat, unbanked },  { 51,  29, 1, 16, flat, unbanked },
             { 50,  29, 1, 16, flat, unbanked },  { 49,  29, 1, 16, flat, unbanked },  { 48,  29, 1, 16, flat, unbanked },  { 48,  29, 1, 16, flat, unbanked },
             { 48,  29, 1, 16, flat, unbanked },  { 49,  29, 1, 16, flat, unbanked },  { 50,  29, 1, 16, flat, unbanked },  { 51,  29, 1, 16, flat, unbanked },
             { 52,  29, 1, 16, flat, unbanked },  { 53,  28, 1, 16, flat, unbanked },  { 54,  27, 1, 16, flat, unbanked },  { 55,  26, 1, 16, flat, unbanked },
             { 56,  25, 0, 16, flat, unbanked },  { 57,  24, 0, 16, flat, unbanked },  { 59,  22, 0, 16, flat, unbanked },  { 58,  20, 0, 16, flat, unbanked },
             { 57,  18, 0, 16, flat, unbanked },  { 56,  16, 0, 16, flat, unbanked },  { 55,  15, 0, 16, flat, unbanked },  { 52,  15, 0, 16, flat, unbanked },
             { 49,  15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 46,  11, 0, 16, flat, unbanked },  { 43,  11, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 40,  11, 0, 16, flat, unbanked },  { 39,  11, 0, 16, flat, unbanked },  { 38,  11, 0, 16, flat, unbanked },
             { 37,  11, 0, 16, flat, unbanked },  { 36,  11, 0, 16, flat, unbanked },  { 35,  11, 0, 16, flat, unbanked },  { 34,  11, 0, 16, flat, unbanked },
             { 33,  11, 0, 16, flat, unbanked },  { 32,  11, 0, 16, flat, unbanked },  { 31,  11, 0, 16, flat, unbanked },  { 30,  11, 0, 16, flat, unbanked },
             { 29,  11, 0, 16, flat, unbanked },  { 28,  11, 0, 16, flat, unbanked },  { 27,  11, 0, 16, flat, unbanked },  { 26,  11, 0, 16, flat, unbanked },
             { 25,  11, 0, 16, flat, unbanked },  { 24,  11, 0, 16, flat, unbanked },  { 23,  11, 0, 16, flat, unbanked },  { 22,  11, 0, 16, flat, unbanked },
             { 21,  11, 0, 16, flat, unbanked },  { 20,  11, 0, 16, flat, unbanked },  { 19,  11, 0, 16, flat, unbanked },  { 18,  11, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),  { 17,  11, 0, 16, flat, unbanked },  { 19,  11, 0, 16, flat, unbanked },
             { 22,  11, 0, 16, flat, unbanked },  { 25,  11, 0, 16, flat, unbanked },  { 28,  11, 0, 16, flat, unbanked },  { 31,  11, 0, 16, flat, unbanked },
             { 34,  11, 0, 16, flat, unbanked },  { 37,  11, 0, 16, flat, unbanked },  { 40,  11, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             { 43,  11, 0, 16, flat, unbanked },  { 46,  11, 0, 16, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),  { 49,  15, 0, 16, flat, unbanked },
             { 52,  15, 0, 16, flat, unbanked },  { 55,  15, 0, 16, flat, unbanked },  { 56,  16, 0, 16, flat, unbanked },  { 57,  18, 0, 16, flat, unbanked },
             { 58,  20, 0, 16, flat, unbanked },  { 59,  22, 0, 16, flat, unbanked },  { 57,  24, 0, 16, flat, unbanked },  { 55,  26, 1, 16, flat, unbanked },
             { 53,  28, 1, 16, flat, unbanked },  { 51,  29, 1, 16, flat, unbanked },  { 49,  29, 1, 16, flat, unbanked },  { 47,  28, 2, 16, flat, unbanked },
             { 45,  28, 2, 16, flat, unbanked },  { 45,  26, 2, 16, flat, unbanked },  { 44,  24, 2, 16, flat, unbanked },  { 43,  22, 3, 16, flat, unbanked },
             { 43,  20, 3, 16, flat, unbanked },  { 42,  18, 3, 16, flat, unbanked },  { 43,  16, 3, 16, flat, unbanked },  { 43,  14, 3, 16, flat, unbanked },
             { 43,  12, 3, 16, flat, unbanked },  { 44,  10, 3, 16, flat, unbanked },  { 44,   8, 3, 16, flat, unbanked },  { 44,   6, 3, 16, flat, unbanked },
             { 45,   4, 3, 16, flat, unbanked },  { 45,   2, 3, 16, flat, unbanked },  { 45,   1, 3, 16, flat, unbanked },  { 45,   0, 3, 16, flat, unbanked },
             { 46,  -1, 3, 16, flat, unbanked },  { 46,  -2, 3, 16, flat, unbanked },  { 46,  -3, 3, 16, flat, unbanked },  { 46,  -4, 3, 16, flat, unbanked },
             { 46,  -5, 3, 16, flat, unbanked },  { 46,  -7, 0, 16, flat, unbanked },  { 47,  -9, 0, 16, flat, unbanked },  { 47, -10, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),  { 47, -11, 0, 16, flat, unbanked },  { 47, -12, 0, 16, flat, unbanked },
             { 47, -13, 0, 16, flat, unbanked },  { 47, -14, 0, 16, flat, unbanked },  { 48, -15, 0, 16, flat, unbanked },  { 48, -16, 0, 16, flat, unbanked },
             { 48, -17, 0, 16, flat, unbanked },  { 48, -18, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
             { 48, -29, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),  { 48, -29, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleD_3, {
                          MINI_GOLF_STATE(Unk0), {  15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown), {  15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3), {  15,  24, 0, 24, flat, unbanked },
            {  14,  21, 0, 24, flat, unbanked }, {  14,  19, 0, 24, flat, unbanked }, {  14,  17, 0, 24, flat, unbanked }, {  15,  15, 0, 24, flat, unbanked },
            {  13,  13, 0, 24, flat, unbanked }, {  13,  11, 0, 24, flat, unbanked }, {  13,   9, 0, 24, flat, unbanked }, {  13,   7, 0, 24, flat, unbanked },
            {  13,   5, 0, 24, flat, unbanked }, {  13,   4, 0, 24, flat, unbanked }, {  12,   3, 0, 24, flat, unbanked }, {  12,   2, 0, 24, flat, unbanked },
            {  12,   1, 0, 24, flat, unbanked }, {  12,   0, 0, 24, flat, unbanked }, {  12,  -1, 0, 24, flat, unbanked }, {  12,  -2, 0, 24, flat, unbanked },
            {  12,  -1, 0, 24, flat, unbanked }, {  12,   0, 0, 24, flat, unbanked }, {  12,   1, 0, 24, flat, unbanked }, {  12,   2, 0, 24, flat, unbanked },
            {  12,   3, 0, 24, flat, unbanked }, {  12,   4, 0, 24, flat, unbanked }, {  12,   5, 0, 24, flat, unbanked }, {  11,   6, 0, 24, flat, unbanked },
            {  11,   7, 0, 24, flat, unbanked }, {  11,   8, 0, 24, flat, unbanked }, {  11,   9, 0, 24, flat, unbanked }, {  11,  10, 0, 24, flat, unbanked },
            {  11,  11, 0, 24, flat, unbanked }, {  11,  12, 0, 24, flat, unbanked }, {  11,  13, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3), {  11,  14, 0, 24, flat, unbanked }, {  11,  12, 0, 24, flat, unbanked }, {  11,   9, 0, 24, flat, unbanked },
            {  11,   6, 0, 24, flat, unbanked }, {  11,   3, 0, 24, flat, unbanked }, {  11,   0, 0, 24, flat, unbanked }, {  11,  -3, 0, 24, flat, unbanked },
            {  11,  -6, 0, 24, flat, unbanked }, {  11,  -9, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk), {  11, -12, 0, 24, flat, unbanked },
            {  11, -15, 0, 24, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown), {  15, -18, 0, 24, flat, unbanked }, {  15, -21, 0, 24, flat, unbanked },
            {  15, -24, 0, 24, flat, unbanked }, {  16, -25, 0, 24, flat, unbanked }, {  18, -26, 0, 24, flat, unbanked }, {  20, -27, 0, 24, flat, unbanked },
            {  22, -28, 0, 24, flat, unbanked }, {  24, -26, 0, 24, flat, unbanked }, {  25, -25, 0, 24, flat, unbanked }, {  26, -24, 1, 24, flat, unbanked },
            {  27, -23, 1, 24, flat, unbanked }, {  28, -22, 1, 24, flat, unbanked }, {  29, -21, 1, 24, flat, unbanked }, {  29, -20, 1, 24, flat, unbanked },
            {  29, -19, 1, 24, flat, unbanked }, {  29, -18, 1, 24, flat, unbanked }, {  29, -17, 1, 24, flat, unbanked }, {  29, -17, 1, 24, flat, unbanked },
            {  29, -17, 1, 24, flat, unbanked }, {  29, -18, 1, 24, flat, unbanked }, {  29, -19, 1, 24, flat, unbanked }, {  29, -20, 1, 24, flat, unbanked },
            {  29, -21, 1, 24, flat, unbanked }, {  28, -22, 1, 24, flat, unbanked }, {  27, -23, 1, 24, flat, unbanked }, {  26, -24, 1, 24, flat, unbanked },
            {  25, -25, 0, 24, flat, unbanked }, {  24, -26, 0, 24, flat, unbanked }, {  22, -28, 0, 24, flat, unbanked }, {  20, -27, 0, 24, flat, unbanked },
            {  18, -26, 0, 24, flat, unbanked }, {  16, -25, 0, 24, flat, unbanked }, {  15, -24, 0, 24, flat, unbanked }, {  15, -21, 0, 24, flat, unbanked },
            {  15, -18, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk), {  11, -15, 0, 24, flat, unbanked }, {  11, -12, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown), {  11,  -9, 0, 24, flat, unbanked }, {  11,  -8, 0, 24, flat, unbanked }, {  11,  -7, 0, 24, flat, unbanked },
            {  11,  -6, 0, 24, flat, unbanked }, {  11,  -5, 0, 24, flat, unbanked }, {  11,  -4, 0, 24, flat, unbanked }, {  11,  -3, 0, 24, flat, unbanked },
            {  11,  -2, 0, 24, flat, unbanked }, {  11,  -1, 0, 24, flat, unbanked }, {  11,   0, 0, 24, flat, unbanked }, {  11,   1, 0, 24, flat, unbanked },
            {  11,   2, 0, 24, flat, unbanked }, {  11,   3, 0, 24, flat, unbanked }, {  11,   4, 0, 24, flat, unbanked }, {  11,   5, 0, 24, flat, unbanked },
            {  11,   6, 0, 24, flat, unbanked }, {  11,   7, 0, 24, flat, unbanked }, {  11,   8, 0, 24, flat, unbanked }, {  11,   9, 0, 24, flat, unbanked },
            {  11,  10, 0, 24, flat, unbanked }, {  11,  11, 0, 24, flat, unbanked }, {  11,  12, 0, 24, flat, unbanked }, {  11,  13, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3), {  11,  14, 0, 24, flat, unbanked }, {  11,  12, 0, 24, flat, unbanked },
            {  11,   9, 0, 24, flat, unbanked }, {  11,   6, 0, 24, flat, unbanked }, {  11,   3, 0, 24, flat, unbanked }, {  11,   0, 0, 24, flat, unbanked },
            {  11,  -3, 0, 24, flat, unbanked }, {  11,  -6, 0, 24, flat, unbanked }, {  11,  -9, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
            {  11, -12, 0, 24, flat, unbanked }, {  11, -15, 0, 24, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown), {  15, -18, 0, 24, flat, unbanked },
            {  15, -21, 0, 24, flat, unbanked }, {  15, -24, 0, 24, flat, unbanked }, {  16, -25, 0, 24, flat, unbanked }, {  18, -26, 0, 24, flat, unbanked },
            {  20, -27, 0, 24, flat, unbanked }, {  22, -28, 0, 24, flat, unbanked }, {  24, -26, 0, 24, flat, unbanked }, {  26, -24, 1, 24, flat, unbanked },
            {  28, -22, 1, 24, flat, unbanked }, {  29, -20, 1, 24, flat, unbanked }, {  29, -18, 1, 24, flat, unbanked }, {  28, -16, 2, 24, flat, unbanked },
            {  28, -14, 2, 24, flat, unbanked }, {  26, -14, 2, 24, flat, unbanked }, {  24, -13, 2, 24, flat, unbanked }, {  22, -12, 3, 24, flat, unbanked },
            {  20, -12, 3, 24, flat, unbanked }, {  18, -11, 3, 24, flat, unbanked }, {  16, -12, 3, 24, flat, unbanked }, {  14, -12, 3, 24, flat, unbanked },
            {  12, -12, 3, 24, flat, unbanked }, {  10, -13, 3, 24, flat, unbanked }, {   8, -13, 3, 24, flat, unbanked }, {   6, -13, 3, 24, flat, unbanked },
            {   4, -14, 3, 24, flat, unbanked }, {   2, -14, 3, 24, flat, unbanked }, {   1, -14, 3, 24, flat, unbanked }, {   0, -14, 3, 24, flat, unbanked },
            {  -1, -15, 3, 24, flat, unbanked }, {  -2, -15, 3, 24, flat, unbanked }, {  -3, -15, 3, 24, flat, unbanked }, {  -4, -15, 3, 24, flat, unbanked },
            {  -5, -15, 3, 24, flat, unbanked }, {  -7, -15, 0, 24, flat, unbanked }, {  -9, -16, 0, 24, flat, unbanked }, { -10, -16, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3), { -11, -16, 0, 24, flat, unbanked }, { -12, -16, 0, 24, flat, unbanked },
            { -13, -16, 0, 24, flat, unbanked }, { -14, -16, 0, 24, flat, unbanked }, { -15, -17, 0, 24, flat, unbanked }, { -16, -17, 0, 24, flat, unbanked },
            { -17, -17, 0, 24, flat, unbanked }, { -18, -17, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
            { -29, -17, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3), { -29, -17, 0,  0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_0, {
                          MINI_GOLF_STATE(Unk0), {  31,  16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2), {  26,  16, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown), {  26,  16, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk3), {  24,  16, 0,  0, flat, unbanked },
            {  21,  15, 0,  0, flat, unbanked }, {  19,  15, 0,  0, flat, unbanked }, {  17,  15, 0,  0, flat, unbanked }, {  15,  16, 0,  0, flat, unbanked },
            {  13,  14, 0,  0, flat, unbanked }, {  11,  14, 0,  0, flat, unbanked }, {   9,  14, 0,  0, flat, unbanked }, {   7,  14, 0,  0, flat, unbanked },
            {   5,  14, 0,  0, flat, unbanked }, {   4,  14, 0,  0, flat, unbanked }, {   3,  13, 0,  0, flat, unbanked }, {   2,  13, 0,  0, flat, unbanked },
            {   1,  13, 0,  0, flat, unbanked }, {   0,  13, 0,  0, flat, unbanked }, {  -1,  13, 0,  0, flat, unbanked }, {  -2,  13, 0,  0, flat, unbanked },
            {  -1,  13, 0,  0, flat, unbanked }, {   0,  13, 0,  0, flat, unbanked }, {   1,  13, 0,  0, flat, unbanked }, {   2,  13, 0,  0, flat, unbanked },
            {   3,  13, 0,  0, flat, unbanked }, {   4,  13, 0,  0, flat, unbanked }, {   5,  13, 0,  0, flat, unbanked }, {   6,  12, 0,  0, flat, unbanked },
            {   7,  12, 0,  0, flat, unbanked }, {   8,  12, 0,  0, flat, unbanked }, {   9,  12, 0,  0, flat, unbanked }, {  10,  12, 0,  0, flat, unbanked },
            {  11,  12, 0,  0, flat, unbanked }, {  12,  12, 0,  0, flat, unbanked }, {  13,  12, 0,  0, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3), {  14,  12, 0,  0, flat, unbanked }, {  12,  12, 0,  0, flat, unbanked }, {   9,  12, 0,  0, flat, unbanked },
            {   6,  12, 0,  0, flat, unbanked }, {   3,  12, 0,  0, flat, unbanked }, {   0,  12, 0,  0, flat, unbanked }, {  -3,  12, 0,  0, flat, unbanked },
            {  -6,  12, 0,  0, flat, unbanked }, {  -9,  12, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk), { -12,  12, 0,  0, flat, unbanked },
            { -15,  12, 0,  0, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown), { -18,  16, 0,  0, flat, unbanked }, { -21,  16, 0,  0, flat, unbanked },
            { -24,  16, 0,  0, flat, unbanked }, { -25,  17, 0,  0, flat, unbanked }, { -26,  19, 0,  0, flat, unbanked }, { -27,  21, 0,  0, flat, unbanked },
            { -28,  23, 0,  0, flat, unbanked }, { -26,  25, 0,  0, flat, unbanked }, { -25,  26, 0,  0, flat, unbanked }, { -24,  27, 1,  0, flat, unbanked },
            { -23,  28, 1,  0, flat, unbanked }, { -22,  29, 1,  0, flat, unbanked }, { -21,  30, 1,  0, flat, unbanked }, { -20,  30, 1,  0, flat, unbanked },
            { -19,  30, 1,  0, flat, unbanked }, { -18,  30, 1,  0, flat, unbanked }, { -17,  30, 1,  0, flat, unbanked }, { -17,  30, 1,  0, flat, unbanked },
            { -17,  30, 1,  0, flat, unbanked }, { -18,  30, 1,  0, flat, unbanked }, { -19,  30, 1,  0, flat, unbanked }, { -20,  30, 1,  0, flat, unbanked },
            { -21,  30, 1,  0, flat, unbanked }, { -22,  29, 1,  0, flat, unbanked }, { -23,  28, 1,  0, flat, unbanked }, { -24,  27, 1,  0, flat, unbanked },
            { -25,  26, 0,  0, flat, unbanked }, { -26,  25, 0,  0, flat, unbanked }, { -28,  23, 0,  0, flat, unbanked }, { -27,  21, 0,  0, flat, unbanked },
            { -26,  19, 0,  0, flat, unbanked }, { -25,  17, 0,  0, flat, unbanked }, { -24,  16, 0,  0, flat, unbanked }, { -21,  16, 0,  0, flat, unbanked },
            { -18,  16, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk), { -15,  12, 0,  0, flat, unbanked }, { -12,  12, 0,  0, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown), {  -9,  12, 0,  0, flat, unbanked }, {  -8,  12, 0,  0, flat, unbanked }, {  -7,  12, 0,  0, flat, unbanked },
            {  -6,  12, 0,  0, flat, unbanked }, {  -5,  12, 0,  0, flat, unbanked }, {  -4,  12, 0,  0, flat, unbanked }, {  -3,  12, 0,  0, flat, unbanked },
            {  -2,  12, 0,  0, flat, unbanked }, {  -1,  12, 0,  0, flat, unbanked }, {   0,  12, 0,  0, flat, unbanked }, {   1,  12, 0,  0, flat, unbanked },
            {   2,  12, 0,  0, flat, unbanked }, {   3,  12, 0,  0, flat, unbanked }, {   4,  12, 0,  0, flat, unbanked }, {   5,  12, 0,  0, flat, unbanked },
            {   6,  12, 0,  0, flat, unbanked }, {   7,  12, 0,  0, flat, unbanked }, {   8,  12, 0,  0, flat, unbanked }, {   9,  12, 0,  0, flat, unbanked },
            {  10,  12, 0,  0, flat, unbanked }, {  11,  12, 0,  0, flat, unbanked }, {  12,  12, 0,  0, flat, unbanked }, {  13,  12, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3), {  14,  12, 0,  0, flat, unbanked }, {  12,  12, 0,  0, flat, unbanked },
            {   9,  12, 0,  0, flat, unbanked }, {   6,  12, 0,  0, flat, unbanked }, {   3,  12, 0,  0, flat, unbanked }, {   0,  12, 0,  0, flat, unbanked },
            {  -3,  12, 0,  0, flat, unbanked }, {  -6,  12, 0,  0, flat, unbanked }, {  -9,  12, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
            { -12,  12, 0,  0, flat, unbanked }, { -15,  12, 0,  0, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown), { -18,  16, 0,  0, flat, unbanked },
            { -21,  16, 0,  0, flat, unbanked }, { -24,  16, 0,  0, flat, unbanked }, { -25,  17, 0,  0, flat, unbanked }, { -26,  19, 0,  0, flat, unbanked },
            { -27,  21, 0,  0, flat, unbanked }, { -28,  23, 0,  0, flat, unbanked }, { -26,  25, 0,  0, flat, unbanked }, { -24,  27, 1,  0, flat, unbanked },
            { -22,  29, 1,  0, flat, unbanked }, { -20,  30, 1,  0, flat, unbanked }, { -18,  30, 1,  0, flat, unbanked }, { -16,  30, 2,  0, flat, unbanked },
            { -14,  29, 2,  0, flat, unbanked }, { -14,  27, 2,  0, flat, unbanked }, { -13,  25, 2,  0, flat, unbanked }, { -12,  23, 3,  0, flat, unbanked },
            { -12,  21, 3,  0, flat, unbanked }, { -11,  19, 3,  0, flat, unbanked }, { -12,  17, 3,  0, flat, unbanked }, { -12,  15, 3,  0, flat, unbanked },
            { -12,  13, 3,  0, flat, unbanked }, { -13,  11, 3,  0, flat, unbanked }, { -13,   9, 3,  0, flat, unbanked }, { -13,   7, 3,  0, flat, unbanked },
            { -14,   5, 3,  0, flat, unbanked }, { -14,   3, 3,  0, flat, unbanked }, { -14,   2, 3,  0, flat, unbanked }, { -14,   1, 3,  0, flat, unbanked },
            { -15,   0, 3,  0, flat, unbanked }, { -15,  -1, 3,  0, flat, unbanked }, { -15,  -2, 3,  0, flat, unbanked }, { -15,  -3, 3,  0, flat, unbanked },
            { -15,  -4, 3,  0, flat, unbanked }, { -15,  -6, 0,  0, flat, unbanked }, { -16,  -8, 0,  0, flat, unbanked }, { -16,  -9, 0,  0, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3), { -16, -10, 0,  0, flat, unbanked }, { -16, -11, 0,  0, flat, unbanked },
            { -16, -12, 0,  0, flat, unbanked }, { -16, -13, 0,  0, flat, unbanked }, { -17, -14, 0,  0, flat, unbanked }, { -17, -15, 0,  0, flat, unbanked },
            { -17, -16, 0,  0, flat, unbanked }, { -17, -17, 0,  0, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
            { -17, -28, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3), { -17, -28, 0, 24, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_1, {
                          MINI_GOLF_STATE(Unk0),   {  16,  0, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  16,  5, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  16,  5, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  16,  7, 0, 8, flat, unbanked },
              {  15, 10, 0, 8, flat, unbanked },   {  15, 12, 0, 8, flat, unbanked },   {  15, 14, 0, 8, flat, unbanked },   {  16, 16, 0, 8, flat, unbanked },
              {  14, 18, 0, 8, flat, unbanked },   {  14, 20, 0, 8, flat, unbanked },   {  14, 22, 0, 8, flat, unbanked },   {  14, 24, 0, 8, flat, unbanked },
              {  14, 26, 0, 8, flat, unbanked },   {  14, 27, 0, 8, flat, unbanked },   {  13, 28, 0, 8, flat, unbanked },   {  13, 29, 0, 8, flat, unbanked },
              {  13, 30, 0, 8, flat, unbanked },   {  13, 31, 0, 8, flat, unbanked },   {  13, 32, 0, 8, flat, unbanked },   {  13, 33, 0, 8, flat, unbanked },
              {  13, 32, 0, 8, flat, unbanked },   {  13, 31, 0, 8, flat, unbanked },   {  13, 30, 0, 8, flat, unbanked },   {  13, 29, 0, 8, flat, unbanked },
              {  13, 28, 0, 8, flat, unbanked },   {  13, 27, 0, 8, flat, unbanked },   {  13, 26, 0, 8, flat, unbanked },   {  12, 25, 0, 8, flat, unbanked },
              {  12, 24, 0, 8, flat, unbanked },   {  12, 23, 0, 8, flat, unbanked },   {  12, 22, 0, 8, flat, unbanked },   {  12, 21, 0, 8, flat, unbanked },
              {  12, 20, 0, 8, flat, unbanked },   {  12, 19, 0, 8, flat, unbanked },   {  12, 18, 0, 8, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   {  12, 17, 0, 8, flat, unbanked },   {  12, 19, 0, 8, flat, unbanked },   {  12, 22, 0, 8, flat, unbanked },
              {  12, 25, 0, 8, flat, unbanked },   {  12, 28, 0, 8, flat, unbanked },   {  12, 31, 0, 8, flat, unbanked },   {  12, 34, 0, 8, flat, unbanked },
              {  12, 37, 0, 8, flat, unbanked },   {  12, 40, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   {  12, 43, 0, 8, flat, unbanked },
              {  12, 46, 0, 8, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   {  16, 49, 0, 8, flat, unbanked },   {  16, 52, 0, 8, flat, unbanked },
              {  16, 55, 0, 8, flat, unbanked },   {  17, 56, 0, 8, flat, unbanked },   {  19, 57, 0, 8, flat, unbanked },   {  21, 58, 0, 8, flat, unbanked },
              {  23, 59, 0, 8, flat, unbanked },   {  25, 57, 0, 8, flat, unbanked },   {  26, 56, 0, 8, flat, unbanked },   {  27, 55, 1, 8, flat, unbanked },
              {  28, 54, 1, 8, flat, unbanked },   {  29, 53, 1, 8, flat, unbanked },   {  30, 52, 1, 8, flat, unbanked },   {  30, 51, 1, 8, flat, unbanked },
              {  30, 50, 1, 8, flat, unbanked },   {  30, 49, 1, 8, flat, unbanked },   {  30, 48, 1, 8, flat, unbanked },   {  30, 48, 1, 8, flat, unbanked },
              {  30, 48, 1, 8, flat, unbanked },   {  30, 49, 1, 8, flat, unbanked },   {  30, 50, 1, 8, flat, unbanked },   {  30, 51, 1, 8, flat, unbanked },
              {  30, 52, 1, 8, flat, unbanked },   {  29, 53, 1, 8, flat, unbanked },   {  28, 54, 1, 8, flat, unbanked },   {  27, 55, 1, 8, flat, unbanked },
              {  26, 56, 0, 8, flat, unbanked },   {  25, 57, 0, 8, flat, unbanked },   {  23, 59, 0, 8, flat, unbanked },   {  21, 58, 0, 8, flat, unbanked },
              {  19, 57, 0, 8, flat, unbanked },   {  17, 56, 0, 8, flat, unbanked },   {  16, 55, 0, 8, flat, unbanked },   {  16, 52, 0, 8, flat, unbanked },
              {  16, 49, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   {  12, 46, 0, 8, flat, unbanked },   {  12, 43, 0, 8, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  12, 40, 0, 8, flat, unbanked },   {  12, 39, 0, 8, flat, unbanked },   {  12, 38, 0, 8, flat, unbanked },
              {  12, 37, 0, 8, flat, unbanked },   {  12, 36, 0, 8, flat, unbanked },   {  12, 35, 0, 8, flat, unbanked },   {  12, 34, 0, 8, flat, unbanked },
              {  12, 33, 0, 8, flat, unbanked },   {  12, 32, 0, 8, flat, unbanked },   {  12, 31, 0, 8, flat, unbanked },   {  12, 30, 0, 8, flat, unbanked },
              {  12, 29, 0, 8, flat, unbanked },   {  12, 28, 0, 8, flat, unbanked },   {  12, 27, 0, 8, flat, unbanked },   {  12, 26, 0, 8, flat, unbanked },
              {  12, 25, 0, 8, flat, unbanked },   {  12, 24, 0, 8, flat, unbanked },   {  12, 23, 0, 8, flat, unbanked },   {  12, 22, 0, 8, flat, unbanked },
              {  12, 21, 0, 8, flat, unbanked },   {  12, 20, 0, 8, flat, unbanked },   {  12, 19, 0, 8, flat, unbanked },   {  12, 18, 0, 8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   {  12, 17, 0, 8, flat, unbanked },   {  12, 19, 0, 8, flat, unbanked },
              {  12, 22, 0, 8, flat, unbanked },   {  12, 25, 0, 8, flat, unbanked },   {  12, 28, 0, 8, flat, unbanked },   {  12, 31, 0, 8, flat, unbanked },
              {  12, 34, 0, 8, flat, unbanked },   {  12, 37, 0, 8, flat, unbanked },   {  12, 40, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              {  12, 43, 0, 8, flat, unbanked },   {  12, 46, 0, 8, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   {  16, 49, 0, 8, flat, unbanked },
              {  16, 52, 0, 8, flat, unbanked },   {  16, 55, 0, 8, flat, unbanked },   {  17, 56, 0, 8, flat, unbanked },   {  19, 57, 0, 8, flat, unbanked },
              {  21, 58, 0, 8, flat, unbanked },   {  23, 59, 0, 8, flat, unbanked },   {  25, 57, 0, 8, flat, unbanked },   {  27, 55, 1, 8, flat, unbanked },
              {  29, 53, 1, 8, flat, unbanked },   {  30, 51, 1, 8, flat, unbanked },   {  30, 49, 1, 8, flat, unbanked },   {  30, 47, 2, 8, flat, unbanked },
              {  29, 45, 2, 8, flat, unbanked },   {  27, 45, 2, 8, flat, unbanked },   {  25, 44, 2, 8, flat, unbanked },   {  23, 43, 3, 8, flat, unbanked },
              {  21, 43, 3, 8, flat, unbanked },   {  19, 42, 3, 8, flat, unbanked },   {  17, 43, 3, 8, flat, unbanked },   {  15, 43, 3, 8, flat, unbanked },
              {  13, 43, 3, 8, flat, unbanked },   {  11, 44, 3, 8, flat, unbanked },   {   9, 44, 3, 8, flat, unbanked },   {   7, 44, 3, 8, flat, unbanked },
              {   5, 45, 3, 8, flat, unbanked },   {   3, 45, 3, 8, flat, unbanked },   {   2, 45, 3, 8, flat, unbanked },   {   1, 45, 3, 8, flat, unbanked },
              {   0, 46, 3, 8, flat, unbanked },   {  -1, 46, 3, 8, flat, unbanked },   {  -2, 46, 3, 8, flat, unbanked },   {  -3, 46, 3, 8, flat, unbanked },
              {  -4, 46, 3, 8, flat, unbanked },   {  -6, 46, 0, 8, flat, unbanked },   {  -8, 47, 0, 8, flat, unbanked },   {  -9, 47, 0, 8, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   { -10, 47, 0, 8, flat, unbanked },   { -11, 47, 0, 8, flat, unbanked },
              { -12, 47, 0, 8, flat, unbanked },   { -13, 47, 0, 8, flat, unbanked },   { -14, 48, 0, 8, flat, unbanked },   { -15, 48, 0, 8, flat, unbanked },
              { -16, 48, 0, 8, flat, unbanked },   { -17, 48, 0, 8, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
              { -28, 48, 0, 0, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),   { -28, 48, 0, 0, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_2, {
                          MINI_GOLF_STATE(Unk0),   {  0, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),   {  5, 15, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   {  5, 15, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk3),   {  7, 15, 0, 16, flat, unbanked },
              { 10, 16, 0, 16, flat, unbanked },   { 12, 16, 0, 16, flat, unbanked },   { 14, 16, 0, 16, flat, unbanked },   { 16, 15, 0, 16, flat, unbanked },
              { 18, 17, 0, 16, flat, unbanked },   { 20, 17, 0, 16, flat, unbanked },   { 22, 17, 0, 16, flat, unbanked },   { 24, 17, 0, 16, flat, unbanked },
              { 26, 17, 0, 16, flat, unbanked },   { 27, 17, 0, 16, flat, unbanked },   { 28, 18, 0, 16, flat, unbanked },   { 29, 18, 0, 16, flat, unbanked },
              { 30, 18, 0, 16, flat, unbanked },   { 31, 18, 0, 16, flat, unbanked },   { 32, 18, 0, 16, flat, unbanked },   { 33, 18, 0, 16, flat, unbanked },
              { 32, 18, 0, 16, flat, unbanked },   { 31, 18, 0, 16, flat, unbanked },   { 30, 18, 0, 16, flat, unbanked },   { 29, 18, 0, 16, flat, unbanked },
              { 28, 18, 0, 16, flat, unbanked },   { 27, 18, 0, 16, flat, unbanked },   { 26, 18, 0, 16, flat, unbanked },   { 25, 19, 0, 16, flat, unbanked },
              { 24, 19, 0, 16, flat, unbanked },   { 23, 19, 0, 16, flat, unbanked },   { 22, 19, 0, 16, flat, unbanked },   { 21, 19, 0, 16, flat, unbanked },
              { 20, 19, 0, 16, flat, unbanked },   { 19, 19, 0, 16, flat, unbanked },   { 18, 19, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),   { 17, 19, 0, 16, flat, unbanked },   { 19, 19, 0, 16, flat, unbanked },   { 22, 19, 0, 16, flat, unbanked },
              { 25, 19, 0, 16, flat, unbanked },   { 28, 19, 0, 16, flat, unbanked },   { 31, 19, 0, 16, flat, unbanked },   { 34, 19, 0, 16, flat, unbanked },
              { 37, 19, 0, 16, flat, unbanked },   { 40, 19, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 43, 19, 0, 16, flat, unbanked },
              { 46, 19, 0, 16, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { 49, 15, 0, 16, flat, unbanked },   { 52, 15, 0, 16, flat, unbanked },
              { 55, 15, 0, 16, flat, unbanked },   { 56, 14, 0, 16, flat, unbanked },   { 57, 12, 0, 16, flat, unbanked },   { 58, 10, 0, 16, flat, unbanked },
              { 59,  8, 0, 16, flat, unbanked },   { 57,  6, 0, 16, flat, unbanked },   { 56,  5, 0, 16, flat, unbanked },   { 55,  4, 1, 16, flat, unbanked },
              { 54,  3, 1, 16, flat, unbanked },   { 53,  2, 1, 16, flat, unbanked },   { 52,  1, 1, 16, flat, unbanked },   { 51,  1, 1, 16, flat, unbanked },
              { 50,  1, 1, 16, flat, unbanked },   { 49,  1, 1, 16, flat, unbanked },   { 48,  1, 1, 16, flat, unbanked },   { 48,  1, 1, 16, flat, unbanked },
              { 48,  1, 1, 16, flat, unbanked },   { 49,  1, 1, 16, flat, unbanked },   { 50,  1, 1, 16, flat, unbanked },   { 51,  1, 1, 16, flat, unbanked },
              { 52,  1, 1, 16, flat, unbanked },   { 53,  2, 1, 16, flat, unbanked },   { 54,  3, 1, 16, flat, unbanked },   { 55,  4, 1, 16, flat, unbanked },
              { 56,  5, 0, 16, flat, unbanked },   { 57,  6, 0, 16, flat, unbanked },   { 59,  8, 0, 16, flat, unbanked },   { 58, 10, 0, 16, flat, unbanked },
              { 57, 12, 0, 16, flat, unbanked },   { 56, 14, 0, 16, flat, unbanked },   { 55, 15, 0, 16, flat, unbanked },   { 52, 15, 0, 16, flat, unbanked },
              { 49, 15, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),   { 46, 19, 0, 16, flat, unbanked },   { 43, 19, 0, 16, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),   { 40, 19, 0, 16, flat, unbanked },   { 39, 19, 0, 16, flat, unbanked },   { 38, 19, 0, 16, flat, unbanked },
              { 37, 19, 0, 16, flat, unbanked },   { 36, 19, 0, 16, flat, unbanked },   { 35, 19, 0, 16, flat, unbanked },   { 34, 19, 0, 16, flat, unbanked },
              { 33, 19, 0, 16, flat, unbanked },   { 32, 19, 0, 16, flat, unbanked },   { 31, 19, 0, 16, flat, unbanked },   { 30, 19, 0, 16, flat, unbanked },
              { 29, 19, 0, 16, flat, unbanked },   { 28, 19, 0, 16, flat, unbanked },   { 27, 19, 0, 16, flat, unbanked },   { 26, 19, 0, 16, flat, unbanked },
              { 25, 19, 0, 16, flat, unbanked },   { 24, 19, 0, 16, flat, unbanked },   { 23, 19, 0, 16, flat, unbanked },   { 22, 19, 0, 16, flat, unbanked },
              { 21, 19, 0, 16, flat, unbanked },   { 20, 19, 0, 16, flat, unbanked },   { 19, 19, 0, 16, flat, unbanked },   { 18, 19, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   { 17, 19, 0, 16, flat, unbanked },   { 19, 19, 0, 16, flat, unbanked },
              { 22, 19, 0, 16, flat, unbanked },   { 25, 19, 0, 16, flat, unbanked },   { 28, 19, 0, 16, flat, unbanked },   { 31, 19, 0, 16, flat, unbanked },
              { 34, 19, 0, 16, flat, unbanked },   { 37, 19, 0, 16, flat, unbanked },   { 40, 19, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
              { 43, 19, 0, 16, flat, unbanked },   { 46, 19, 0, 16, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),   { 49, 15, 0, 16, flat, unbanked },
              { 52, 15, 0, 16, flat, unbanked },   { 55, 15, 0, 16, flat, unbanked },   { 56, 14, 0, 16, flat, unbanked },   { 57, 12, 0, 16, flat, unbanked },
              { 58, 10, 0, 16, flat, unbanked },   { 59,  8, 0, 16, flat, unbanked },   { 57,  6, 0, 16, flat, unbanked },   { 55,  4, 1, 16, flat, unbanked },
              { 53,  2, 1, 16, flat, unbanked },   { 51,  1, 1, 16, flat, unbanked },   { 49,  1, 1, 16, flat, unbanked },   { 47,  1, 2, 16, flat, unbanked },
              { 45,  2, 2, 16, flat, unbanked },   { 45,  4, 2, 16, flat, unbanked },   { 44,  6, 2, 16, flat, unbanked },   { 43,  8, 3, 16, flat, unbanked },
              { 43, 10, 3, 16, flat, unbanked },   { 42, 12, 3, 16, flat, unbanked },   { 43, 14, 3, 16, flat, unbanked },   { 43, 16, 3, 16, flat, unbanked },
              { 43, 18, 3, 16, flat, unbanked },   { 44, 20, 3, 16, flat, unbanked },   { 44, 22, 3, 16, flat, unbanked },   { 44, 24, 3, 16, flat, unbanked },
              { 45, 26, 3, 16, flat, unbanked },   { 45, 28, 3, 16, flat, unbanked },   { 45, 29, 3, 16, flat, unbanked },   { 45, 30, 3, 16, flat, unbanked },
              { 46, 31, 3, 16, flat, unbanked },   { 46, 32, 3, 16, flat, unbanked },   { 46, 33, 3, 16, flat, unbanked },   { 46, 34, 3, 16, flat, unbanked },
              { 46, 35, 3, 16, flat, unbanked },   { 46, 37, 0, 16, flat, unbanked },   { 47, 39, 0, 16, flat, unbanked },   { 47, 40, 0, 16, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),   { 47, 41, 0, 16, flat, unbanked },   { 47, 42, 0, 16, flat, unbanked },
              { 47, 43, 0, 16, flat, unbanked },   { 47, 44, 0, 16, flat, unbanked },   { 48, 45, 0, 16, flat, unbanked },   { 48, 46, 0, 16, flat, unbanked },
              { 48, 47, 0, 16, flat, unbanked },   { 48, 48, 0, 16, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
              { 48, 59, 0,  8, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),   { 48, 59, 0,  8, flat, unbanked },
        })

        CREATE_VEHICLE_INFO(MinigolfHoleE_3, {
                          MINI_GOLF_STATE(Unk0),  { 15,  31, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),  { 15,  26, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 15,  26, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk3),  { 15,  24, 0, 24, flat, unbanked },
             { 16,  21, 0, 24, flat, unbanked },  { 16,  19, 0, 24, flat, unbanked },  { 16,  17, 0, 24, flat, unbanked },  { 15,  15, 0, 24, flat, unbanked },
             { 17,  13, 0, 24, flat, unbanked },  { 17,  11, 0, 24, flat, unbanked },  { 17,   9, 0, 24, flat, unbanked },  { 17,   7, 0, 24, flat, unbanked },
             { 17,   5, 0, 24, flat, unbanked },  { 17,   4, 0, 24, flat, unbanked },  { 18,   3, 0, 24, flat, unbanked },  { 18,   2, 0, 24, flat, unbanked },
             { 18,   1, 0, 24, flat, unbanked },  { 18,   0, 0, 24, flat, unbanked },  { 18,  -1, 0, 24, flat, unbanked },  { 18,  -2, 0, 24, flat, unbanked },
             { 18,  -1, 0, 24, flat, unbanked },  { 18,   0, 0, 24, flat, unbanked },  { 18,   1, 0, 24, flat, unbanked },  { 18,   2, 0, 24, flat, unbanked },
             { 18,   3, 0, 24, flat, unbanked },  { 18,   4, 0, 24, flat, unbanked },  { 18,   5, 0, 24, flat, unbanked },  { 19,   6, 0, 24, flat, unbanked },
             { 19,   7, 0, 24, flat, unbanked },  { 19,   8, 0, 24, flat, unbanked },  { 19,   9, 0, 24, flat, unbanked },  { 19,  10, 0, 24, flat, unbanked },
             { 19,  11, 0, 24, flat, unbanked },  { 19,  12, 0, 24, flat, unbanked },  { 19,  13, 0, 24, flat, unbanked },               MINI_GOLF_STATE(Unk2),
                          MINI_GOLF_STATE(Unk3),  { 19,  14, 0, 24, flat, unbanked },  { 19,  12, 0, 24, flat, unbanked },  { 19,   9, 0, 24, flat, unbanked },
             { 19,   6, 0, 24, flat, unbanked },  { 19,   3, 0, 24, flat, unbanked },  { 19,   0, 0, 24, flat, unbanked },  { 19,  -3, 0, 24, flat, unbanked },
             { 19,  -6, 0, 24, flat, unbanked },  { 19,  -9, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 19, -12, 0, 24, flat, unbanked },
             { 19, -15, 0, 24, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),  { 15, -18, 0, 24, flat, unbanked },  { 15, -21, 0, 24, flat, unbanked },
             { 15, -24, 0, 24, flat, unbanked },  { 14, -25, 0, 24, flat, unbanked },  { 12, -26, 0, 24, flat, unbanked },  { 10, -27, 0, 24, flat, unbanked },
             {  8, -28, 0, 24, flat, unbanked },  {  6, -26, 0, 24, flat, unbanked },  {  5, -25, 0, 24, flat, unbanked },  {  4, -24, 1, 24, flat, unbanked },
             {  3, -23, 1, 24, flat, unbanked },  {  2, -22, 1, 24, flat, unbanked },  {  1, -21, 1, 24, flat, unbanked },  {  1, -20, 1, 24, flat, unbanked },
             {  1, -19, 1, 24, flat, unbanked },  {  1, -18, 1, 24, flat, unbanked },  {  1, -17, 1, 24, flat, unbanked },  {  1, -17, 1, 24, flat, unbanked },
             {  1, -17, 1, 24, flat, unbanked },  {  1, -18, 1, 24, flat, unbanked },  {  1, -19, 1, 24, flat, unbanked },  {  1, -20, 1, 24, flat, unbanked },
             {  1, -21, 1, 24, flat, unbanked },  {  2, -22, 1, 24, flat, unbanked },  {  3, -23, 1, 24, flat, unbanked },  {  4, -24, 1, 24, flat, unbanked },
             {  5, -25, 0, 24, flat, unbanked },  {  6, -26, 0, 24, flat, unbanked },  {  8, -28, 0, 24, flat, unbanked },  { 10, -27, 0, 24, flat, unbanked },
             { 12, -26, 0, 24, flat, unbanked },  { 14, -25, 0, 24, flat, unbanked },  { 15, -24, 0, 24, flat, unbanked },  { 15, -21, 0, 24, flat, unbanked },
             { 15, -18, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),  { 19, -15, 0, 24, flat, unbanked },  { 19, -12, 0, 24, flat, unbanked },
             MINI_GOLF_ANIMATION(PlaceBallDown),  { 19,  -9, 0, 24, flat, unbanked },  { 19,  -8, 0, 24, flat, unbanked },  { 19,  -7, 0, 24, flat, unbanked },
             { 19,  -6, 0, 24, flat, unbanked },  { 19,  -5, 0, 24, flat, unbanked },  { 19,  -4, 0, 24, flat, unbanked },  { 19,  -3, 0, 24, flat, unbanked },
             { 19,  -2, 0, 24, flat, unbanked },  { 19,  -1, 0, 24, flat, unbanked },  { 19,   0, 0, 24, flat, unbanked },  { 19,   1, 0, 24, flat, unbanked },
             { 19,   2, 0, 24, flat, unbanked },  { 19,   3, 0, 24, flat, unbanked },  { 19,   4, 0, 24, flat, unbanked },  { 19,   5, 0, 24, flat, unbanked },
             { 19,   6, 0, 24, flat, unbanked },  { 19,   7, 0, 24, flat, unbanked },  { 19,   8, 0, 24, flat, unbanked },  { 19,   9, 0, 24, flat, unbanked },
             { 19,  10, 0, 24, flat, unbanked },  { 19,  11, 0, 24, flat, unbanked },  { 19,  12, 0, 24, flat, unbanked },  { 19,  13, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),  { 19,  14, 0, 24, flat, unbanked },  { 19,  12, 0, 24, flat, unbanked },
             { 19,   9, 0, 24, flat, unbanked },  { 19,   6, 0, 24, flat, unbanked },  { 19,   3, 0, 24, flat, unbanked },  { 19,   0, 0, 24, flat, unbanked },
             { 19,  -3, 0, 24, flat, unbanked },  { 19,  -6, 0, 24, flat, unbanked },  { 19,  -9, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),
             { 19, -12, 0, 24, flat, unbanked },  { 19, -15, 0, 24, flat, unbanked },  MINI_GOLF_ANIMATION(PlaceBallDown),  { 15, -18, 0, 24, flat, unbanked },
             { 15, -21, 0, 24, flat, unbanked },  { 15, -24, 0, 24, flat, unbanked },  { 14, -25, 0, 24, flat, unbanked },  { 12, -26, 0, 24, flat, unbanked },
             { 10, -27, 0, 24, flat, unbanked },  {  8, -28, 0, 24, flat, unbanked },  {  6, -26, 0, 24, flat, unbanked },  {  4, -24, 1, 24, flat, unbanked },
             {  2, -22, 1, 24, flat, unbanked },  {  1, -20, 1, 24, flat, unbanked },  {  1, -18, 1, 24, flat, unbanked },  {  1, -16, 2, 24, flat, unbanked },
             {  2, -14, 2, 24, flat, unbanked },  {  4, -14, 2, 24, flat, unbanked },  {  6, -13, 2, 24, flat, unbanked },  {  8, -12, 3, 24, flat, unbanked },
             { 10, -12, 3, 24, flat, unbanked },  { 12, -11, 3, 24, flat, unbanked },  { 14, -12, 3, 24, flat, unbanked },  { 16, -12, 3, 24, flat, unbanked },
             { 18, -12, 3, 24, flat, unbanked },  { 20, -13, 3, 24, flat, unbanked },  { 22, -13, 3, 24, flat, unbanked },  { 24, -13, 3, 24, flat, unbanked },
             { 26, -14, 3, 24, flat, unbanked },  { 28, -14, 3, 24, flat, unbanked },  { 29, -14, 3, 24, flat, unbanked },  { 30, -14, 3, 24, flat, unbanked },
             { 31, -15, 3, 24, flat, unbanked },  { 32, -15, 3, 24, flat, unbanked },  { 33, -15, 3, 24, flat, unbanked },  { 34, -15, 3, 24, flat, unbanked },
             { 35, -15, 3, 24, flat, unbanked },  { 37, -15, 0, 24, flat, unbanked },  { 39, -16, 0, 24, flat, unbanked },  { 40, -16, 0, 24, flat, unbanked },
                          MINI_GOLF_STATE(Unk2),               MINI_GOLF_STATE(Unk3),  { 41, -16, 0, 24, flat, unbanked },  { 42, -16, 0, 24, flat, unbanked },
             { 43, -16, 0, 24, flat, unbanked },  { 44, -16, 0, 24, flat, unbanked },  { 45, -17, 0, 24, flat, unbanked },  { 46, -17, 0, 24, flat, unbanked },
             { 47, -17, 0, 24, flat, unbanked },  { 48, -17, 0, 24, flat, unbanked },           MINI_GOLF_ANIMATION(Walk),               MINI_GOLF_STATE(Unk2),
             { 59, -17, 0, 16, flat, unbanked },               MINI_GOLF_STATE(Unk6),               MINI_GOLF_STATE(Unk3),  { 59, -17, 0, 16, flat, unbanked },
        })

        static constexpr const VehicleInfoList * BallPathC[] = {
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3,
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // EndStation
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // BeginStation
            &Flat_0,                  &Flat_1,                  &Flat_2,                  &Flat_3, // MiddleStation
            &Up25_0,                  &Up25_1,                  &Up25_2,                  &Up25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60
            &FlatToUp25_0,            &FlatToUp25_1,            &FlatToUp25_2,            &FlatToUp25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp25
            &Up25ToFlat_0,            &Up25ToFlat_1,            &Up25ToFlat_2,            &Up25ToFlat_3,
            &Down25_0,                &Down25_1,                &Down25_2,                &Down25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60
            &FlatToDown25_0,          &FlatToDown25_1,          &FlatToDown25_2,          &FlatToDown25_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown25
            &Down25ToFlat_0,          &Down25ToFlat_1,          &Down25ToFlat_2,          &Down25ToFlat_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BankedLeftQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BankedRightQuarterTurn5Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendLeft
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendRight
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftVerticalLoop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightVerticalLoop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankedQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankedQuarterTurn3Tiles
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesDown25
            &LeftQuarterTurn1Tile_0,  &LeftQuarterTurn1Tile_1,  &LeftQuarterTurn1Tile_2,  &LeftQuarterTurn1Tile_3,
            &RightQuarterTurn1Tile_0, &RightQuarterTurn1Tile_1, &RightQuarterTurn1Tile_2, &RightQuarterTurn1Tile_3,
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftTwistDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightTwistDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftTwistUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightTwistUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // TowerBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // TowerSection
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToUp60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25ToFlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToDown60Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown25Covered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25ToFlatCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn5TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn5TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendLeftCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SBendRightCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesCovered
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixUpSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixUpSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixDownSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixDownSmall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixUpLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixUpLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHalfBankedHelixDownLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHalfBankedHelixDownLarge
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn1TileUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn1TileUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn1TileDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn1TileDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Brakes
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Booster
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Maze
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterBankedHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterBankedHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterBankedHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterBankedHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterHelixLargeUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterHelixLargeDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25LeftBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up25RightBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Waterfall
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Rapids
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // OnRidePhoto
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25LeftBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down25RightBanked
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Watersplash
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToUp60LongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToFlatLongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Whirlpool
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToFlatLongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlatToDown60LongBase
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // CableLiftHill
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // ReverseFreefallSlope
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // ReverseFreefallVertical
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up60ToUp90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down90ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Up90ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // Down60ToDown90
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // BrakeForDrop
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthBankToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthBankToDiag
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftEighthBankToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightEighthBankToOrthogonal
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60ToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60ToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToUp60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToDown60
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown60ToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagFlatToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToFlat
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagUp25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBankToDown25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagDown25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // DiagRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LogFlumeReverser
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // SpinningTunnel
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBarrelRollUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBarrelRollUpToDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBarrelRollDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBarrelRollDownToUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftBankToLeftQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightBankToRightQuarterTurn3TilesUp25
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftQuarterTurn3TilesDown25ToLeftBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightQuarterTurn3TilesDown25ToRightBank
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // PoweredLift
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftLargeHalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightLargeHalfLoopUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftLargeHalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightLargeHalfLoopDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerTwistUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerTwistUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerTwistDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerTwistDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlyerHalfLoopUninvertedUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // FlyerHalfLoopInvertedDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerCorkscrewUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftFlyerCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightFlyerCorkscrewDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HeartLineTransferUp
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // HeartLineTransferDown
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // LeftHeartLineRoll
            &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, &TrackVehicleInfo_000000, // RightHeartLineRoll
            &MinigolfHoleA_0,         &MinigolfHoleA_1,         &MinigolfHoleA_2,         &MinigolfHoleA_3,
            &MinigolfHoleB_0,         &MinigolfHoleB_1,         &MinigolfHoleB_2,         &MinigolfHoleB_3,
            &MinigolfHoleC_0,         &MinigolfHoleC_1,         &MinigolfHoleC_2,         &MinigolfHoleC_3,
            &MinigolfHoleD_0,         &MinigolfHoleD_1,         &MinigolfHoleD_2,         &MinigolfHoleD_3,
            &MinigolfHoleE_0,         &MinigolfHoleE_1,         &MinigolfHoleE_2,         &MinigolfHoleE_3,
        };
        // clang-format on
        static_assert(std::size(BallPathC) == kVehicleTrackSubpositionSizeMinigolf);
    } // namespace BallPathC

} // namespace OpenRCT2::Subposition::Minigolf
