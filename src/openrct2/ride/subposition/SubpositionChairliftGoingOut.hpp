/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once
#include "SubpositionCommon.hpp"
#include "SubpositionDefault.hpp"

using enum VehiclePitch;
using enum VehicleRoll;

namespace OpenRCT2::Subposition::ChairliftGoingOut
{
    // clang-format off
    CREATE_VEHICLE_INFO(Flat_0, {
        { 31, 6, 0, 0, flat, unbanked }, { 30, 6, 0, 0, flat, unbanked }, { 29, 6, 0, 0, flat, unbanked }, { 28, 6, 0, 0, flat, unbanked }, { 27, 6, 0, 0, flat, unbanked }, { 26, 6, 0, 0, flat, unbanked },
        { 25, 6, 0, 0, flat, unbanked }, { 24, 6, 0, 0, flat, unbanked }, { 23, 6, 0, 0, flat, unbanked }, { 22, 6, 0, 0, flat, unbanked }, { 21, 6, 0, 0, flat, unbanked }, { 20, 6, 0, 0, flat, unbanked },
        { 19, 6, 0, 0, flat, unbanked }, { 18, 6, 0, 0, flat, unbanked }, { 17, 6, 0, 0, flat, unbanked }, { 16, 6, 0, 0, flat, unbanked }, { 15, 6, 0, 0, flat, unbanked }, { 14, 6, 0, 0, flat, unbanked },
        { 13, 6, 0, 0, flat, unbanked }, { 12, 6, 0, 0, flat, unbanked }, { 11, 6, 0, 0, flat, unbanked }, { 10, 6, 0, 0, flat, unbanked }, {  9, 6, 0, 0, flat, unbanked }, {  8, 6, 0, 0, flat, unbanked },
        {  7, 6, 0, 0, flat, unbanked }, {  6, 6, 0, 0, flat, unbanked }, {  5, 6, 0, 0, flat, unbanked }, {  4, 6, 0, 0, flat, unbanked }, {  3, 6, 0, 0, flat, unbanked }, {  2, 6, 0, 0, flat, unbanked },
        {  1, 6, 0, 0, flat, unbanked }, {  0, 6, 0, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Flat_1, {
        { 6,  0, 0, 8, flat, unbanked }, { 6,  1, 0, 8, flat, unbanked }, { 6,  2, 0, 8, flat, unbanked }, { 6,  3, 0, 8, flat, unbanked }, { 6,  4, 0, 8, flat, unbanked }, { 6,  5, 0, 8, flat, unbanked },
        { 6,  6, 0, 8, flat, unbanked }, { 6,  7, 0, 8, flat, unbanked }, { 6,  8, 0, 8, flat, unbanked }, { 6,  9, 0, 8, flat, unbanked }, { 6, 10, 0, 8, flat, unbanked }, { 6, 11, 0, 8, flat, unbanked },
        { 6, 12, 0, 8, flat, unbanked }, { 6, 13, 0, 8, flat, unbanked }, { 6, 14, 0, 8, flat, unbanked }, { 6, 15, 0, 8, flat, unbanked }, { 6, 16, 0, 8, flat, unbanked }, { 6, 17, 0, 8, flat, unbanked },
        { 6, 18, 0, 8, flat, unbanked }, { 6, 19, 0, 8, flat, unbanked }, { 6, 20, 0, 8, flat, unbanked }, { 6, 21, 0, 8, flat, unbanked }, { 6, 22, 0, 8, flat, unbanked }, { 6, 23, 0, 8, flat, unbanked },
        { 6, 24, 0, 8, flat, unbanked }, { 6, 25, 0, 8, flat, unbanked }, { 6, 26, 0, 8, flat, unbanked }, { 6, 27, 0, 8, flat, unbanked }, { 6, 28, 0, 8, flat, unbanked }, { 6, 29, 0, 8, flat, unbanked },
        { 6, 30, 0, 8, flat, unbanked }, { 6, 31, 0, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Flat_2, {
        {  0, 26, 0, 16, flat, unbanked }, {  1, 26, 0, 16, flat, unbanked }, {  2, 26, 0, 16, flat, unbanked }, {  3, 26, 0, 16, flat, unbanked }, {  4, 26, 0, 16, flat, unbanked },
        {  5, 26, 0, 16, flat, unbanked }, {  6, 26, 0, 16, flat, unbanked }, {  7, 26, 0, 16, flat, unbanked }, {  8, 26, 0, 16, flat, unbanked }, {  9, 26, 0, 16, flat, unbanked },
        { 10, 26, 0, 16, flat, unbanked }, { 11, 26, 0, 16, flat, unbanked }, { 12, 26, 0, 16, flat, unbanked }, { 13, 26, 0, 16, flat, unbanked }, { 14, 26, 0, 16, flat, unbanked },
        { 15, 26, 0, 16, flat, unbanked }, { 16, 26, 0, 16, flat, unbanked }, { 17, 26, 0, 16, flat, unbanked }, { 18, 26, 0, 16, flat, unbanked }, { 19, 26, 0, 16, flat, unbanked },
        { 20, 26, 0, 16, flat, unbanked }, { 21, 26, 0, 16, flat, unbanked }, { 22, 26, 0, 16, flat, unbanked }, { 23, 26, 0, 16, flat, unbanked }, { 24, 26, 0, 16, flat, unbanked },
        { 25, 26, 0, 16, flat, unbanked }, { 26, 26, 0, 16, flat, unbanked }, { 27, 26, 0, 16, flat, unbanked }, { 28, 26, 0, 16, flat, unbanked }, { 29, 26, 0, 16, flat, unbanked },
        { 30, 26, 0, 16, flat, unbanked }, { 31, 26, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Flat_3, {
        { 26, 31, 0, 24, flat, unbanked }, { 26, 30, 0, 24, flat, unbanked }, { 26, 29, 0, 24, flat, unbanked }, { 26, 28, 0, 24, flat, unbanked }, { 26, 27, 0, 24, flat, unbanked },
        { 26, 26, 0, 24, flat, unbanked }, { 26, 25, 0, 24, flat, unbanked }, { 26, 24, 0, 24, flat, unbanked }, { 26, 23, 0, 24, flat, unbanked }, { 26, 22, 0, 24, flat, unbanked },
        { 26, 21, 0, 24, flat, unbanked }, { 26, 20, 0, 24, flat, unbanked }, { 26, 19, 0, 24, flat, unbanked }, { 26, 18, 0, 24, flat, unbanked }, { 26, 17, 0, 24, flat, unbanked },
        { 26, 16, 0, 24, flat, unbanked }, { 26, 15, 0, 24, flat, unbanked }, { 26, 14, 0, 24, flat, unbanked }, { 26, 13, 0, 24, flat, unbanked }, { 26, 12, 0, 24, flat, unbanked },
        { 26, 11, 0, 24, flat, unbanked }, { 26, 10, 0, 24, flat, unbanked }, { 26,  9, 0, 24, flat, unbanked }, { 26,  8, 0, 24, flat, unbanked }, { 26,  7, 0, 24, flat, unbanked },
        { 26,  6, 0, 24, flat, unbanked }, { 26,  5, 0, 24, flat, unbanked }, { 26,  4, 0, 24, flat, unbanked }, { 26,  3, 0, 24, flat, unbanked }, { 26,  2, 0, 24, flat, unbanked },
        { 26,  1, 0, 24, flat, unbanked }, { 26,  0, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25_0, {
        { 31, 6,  0, 0, up25, unbanked }, { 30, 6,  1, 0, up25, unbanked }, { 29, 6,  1, 0, up25, unbanked }, { 28, 6,  2, 0, up25, unbanked }, { 27, 6,  2, 0, up25, unbanked }, { 26, 6,  3, 0, up25, unbanked },
        { 25, 6,  3, 0, up25, unbanked }, { 24, 6,  4, 0, up25, unbanked }, { 23, 6,  4, 0, up25, unbanked }, { 22, 6,  5, 0, up25, unbanked }, { 21, 6,  5, 0, up25, unbanked }, { 20, 6,  6, 0, up25, unbanked },
        { 19, 6,  6, 0, up25, unbanked }, { 18, 6,  7, 0, up25, unbanked }, { 17, 6,  7, 0, up25, unbanked }, { 16, 6,  8, 0, up25, unbanked }, { 15, 6,  8, 0, up25, unbanked }, { 14, 6,  9, 0, up25, unbanked },
        { 13, 6,  9, 0, up25, unbanked }, { 12, 6, 10, 0, up25, unbanked }, { 11, 6, 10, 0, up25, unbanked }, { 10, 6, 11, 0, up25, unbanked }, {  9, 6, 11, 0, up25, unbanked }, {  8, 6, 12, 0, up25, unbanked },
        {  7, 6, 12, 0, up25, unbanked }, {  6, 6, 13, 0, up25, unbanked }, {  5, 6, 13, 0, up25, unbanked }, {  4, 6, 14, 0, up25, unbanked }, {  3, 6, 14, 0, up25, unbanked }, {  2, 6, 15, 0, up25, unbanked },
        {  1, 6, 15, 0, up25, unbanked }, {  0, 6, 16, 0, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25_1, {
        { 6,  0,  0, 8, up25, unbanked }, { 6,  1,  0, 8, up25, unbanked }, { 6,  2,  1, 8, up25, unbanked }, { 6,  3,  1, 8, up25, unbanked }, { 6,  4,  2, 8, up25, unbanked }, { 6,  5,  2, 8, up25, unbanked },
        { 6,  6,  3, 8, up25, unbanked }, { 6,  7,  3, 8, up25, unbanked }, { 6,  8,  4, 8, up25, unbanked }, { 6,  9,  4, 8, up25, unbanked }, { 6, 10,  5, 8, up25, unbanked }, { 6, 11,  5, 8, up25, unbanked },
        { 6, 12,  6, 8, up25, unbanked }, { 6, 13,  6, 8, up25, unbanked }, { 6, 14,  7, 8, up25, unbanked }, { 6, 15,  7, 8, up25, unbanked }, { 6, 16,  8, 8, up25, unbanked }, { 6, 17,  8, 8, up25, unbanked },
        { 6, 18,  9, 8, up25, unbanked }, { 6, 19,  9, 8, up25, unbanked }, { 6, 20, 10, 8, up25, unbanked }, { 6, 21, 10, 8, up25, unbanked }, { 6, 22, 11, 8, up25, unbanked }, { 6, 23, 11, 8, up25, unbanked },
        { 6, 24, 12, 8, up25, unbanked }, { 6, 25, 12, 8, up25, unbanked }, { 6, 26, 13, 8, up25, unbanked }, { 6, 27, 13, 8, up25, unbanked }, { 6, 28, 14, 8, up25, unbanked }, { 6, 29, 14, 8, up25, unbanked },
        { 6, 30, 15, 8, up25, unbanked }, { 6, 31, 15, 8, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25_2, {
        {  0, 26,  0, 16, up25, unbanked }, {  1, 26,  0, 16, up25, unbanked }, {  2, 26,  1, 16, up25, unbanked }, {  3, 26,  1, 16, up25, unbanked }, {  4, 26,  2, 16, up25, unbanked },
        {  5, 26,  2, 16, up25, unbanked }, {  6, 26,  3, 16, up25, unbanked }, {  7, 26,  3, 16, up25, unbanked }, {  8, 26,  4, 16, up25, unbanked }, {  9, 26,  4, 16, up25, unbanked },
        { 10, 26,  5, 16, up25, unbanked }, { 11, 26,  5, 16, up25, unbanked }, { 12, 26,  6, 16, up25, unbanked }, { 13, 26,  6, 16, up25, unbanked }, { 14, 26,  7, 16, up25, unbanked },
        { 15, 26,  7, 16, up25, unbanked }, { 16, 26,  8, 16, up25, unbanked }, { 17, 26,  8, 16, up25, unbanked }, { 18, 26,  9, 16, up25, unbanked }, { 19, 26,  9, 16, up25, unbanked },
        { 20, 26, 10, 16, up25, unbanked }, { 21, 26, 10, 16, up25, unbanked }, { 22, 26, 11, 16, up25, unbanked }, { 23, 26, 11, 16, up25, unbanked }, { 24, 26, 12, 16, up25, unbanked },
        { 25, 26, 12, 16, up25, unbanked }, { 26, 26, 13, 16, up25, unbanked }, { 27, 26, 13, 16, up25, unbanked }, { 28, 26, 14, 16, up25, unbanked }, { 29, 26, 14, 16, up25, unbanked },
        { 30, 26, 15, 16, up25, unbanked }, { 31, 26, 15, 16, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25_3, {
        { 26, 31,  0, 24, up25, unbanked }, { 26, 30,  1, 24, up25, unbanked }, { 26, 29,  1, 24, up25, unbanked }, { 26, 28,  2, 24, up25, unbanked }, { 26, 27,  2, 24, up25, unbanked },
        { 26, 26,  3, 24, up25, unbanked }, { 26, 25,  3, 24, up25, unbanked }, { 26, 24,  4, 24, up25, unbanked }, { 26, 23,  4, 24, up25, unbanked }, { 26, 22,  5, 24, up25, unbanked },
        { 26, 21,  5, 24, up25, unbanked }, { 26, 20,  6, 24, up25, unbanked }, { 26, 19,  6, 24, up25, unbanked }, { 26, 18,  7, 24, up25, unbanked }, { 26, 17,  7, 24, up25, unbanked },
        { 26, 16,  8, 24, up25, unbanked }, { 26, 15,  8, 24, up25, unbanked }, { 26, 14,  9, 24, up25, unbanked }, { 26, 13,  9, 24, up25, unbanked }, { 26, 12, 10, 24, up25, unbanked },
        { 26, 11, 10, 24, up25, unbanked }, { 26, 10, 11, 24, up25, unbanked }, { 26,  9, 11, 24, up25, unbanked }, { 26,  8, 12, 24, up25, unbanked }, { 26,  7, 12, 24, up25, unbanked },
        { 26,  6, 13, 24, up25, unbanked }, { 26,  5, 13, 24, up25, unbanked }, { 26,  4, 14, 24, up25, unbanked }, { 26,  3, 14, 24, up25, unbanked }, { 26,  2, 15, 24, up25, unbanked },
        { 26,  1, 15, 24, up25, unbanked }, { 26,  0, 16, 24, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToUp25_0, {
        { 31, 6, 0, 0, flat, unbanked }, { 30, 6, 0, 0, flat, unbanked }, { 29, 6, 0, 0, flat, unbanked }, { 28, 6, 0, 0, flat, unbanked }, { 27, 6, 0, 0, flat, unbanked }, { 26, 6, 0, 0, flat, unbanked },
        { 25, 6, 0, 0, flat, unbanked }, { 24, 6, 0, 0, flat, unbanked }, { 23, 6, 0, 0, up12, unbanked }, { 22, 6, 0, 0, up12, unbanked }, { 21, 6, 1, 0, up12, unbanked }, { 20, 6, 1, 0, up12, unbanked },
        { 19, 6, 1, 0, up12, unbanked }, { 18, 6, 1, 0, up12, unbanked }, { 17, 6, 1, 0, up12, unbanked }, { 16, 6, 1, 0, up12, unbanked }, { 15, 6, 2, 0, up12, unbanked }, { 14, 6, 2, 0, up12, unbanked },
        { 13, 6, 2, 0, up12, unbanked }, { 12, 6, 2, 0, up12, unbanked }, { 11, 6, 3, 0, up12, unbanked }, { 10, 6, 3, 0, up12, unbanked }, {  9, 6, 3, 0, up12, unbanked }, {  8, 6, 4, 0, up12, unbanked },
        {  7, 6, 4, 0, up25, unbanked }, {  6, 6, 5, 0, up25, unbanked }, {  5, 6, 5, 0, up25, unbanked }, {  4, 6, 6, 0, up25, unbanked }, {  3, 6, 6, 0, up25, unbanked }, {  2, 6, 7, 0, up25, unbanked },
        {  1, 6, 7, 0, up25, unbanked }, {  0, 6, 8, 0, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToUp25_1, {
        { 6,  0, 0, 8, flat, unbanked }, { 6,  1, 0, 8, flat, unbanked }, { 6,  2, 0, 8, flat, unbanked }, { 6,  3, 0, 8, flat, unbanked }, { 6,  4, 0, 8, flat, unbanked }, { 6,  5, 0, 8, flat, unbanked },
        { 6,  6, 0, 8, flat, unbanked }, { 6,  7, 0, 8, flat, unbanked }, { 6,  8, 0, 8, up12, unbanked }, { 6,  9, 0, 8, up12, unbanked }, { 6, 10, 1, 8, up12, unbanked }, { 6, 11, 1, 8, up12, unbanked },
        { 6, 12, 1, 8, up12, unbanked }, { 6, 13, 1, 8, up12, unbanked }, { 6, 14, 1, 8, up12, unbanked }, { 6, 15, 1, 8, up12, unbanked }, { 6, 16, 2, 8, up12, unbanked }, { 6, 17, 2, 8, up12, unbanked },
        { 6, 18, 2, 8, up12, unbanked }, { 6, 19, 2, 8, up12, unbanked }, { 6, 20, 3, 8, up12, unbanked }, { 6, 21, 3, 8, up12, unbanked }, { 6, 22, 3, 8, up12, unbanked }, { 6, 23, 4, 8, up12, unbanked },
        { 6, 24, 4, 8, up25, unbanked }, { 6, 25, 4, 8, up25, unbanked }, { 6, 26, 5, 8, up25, unbanked }, { 6, 27, 5, 8, up25, unbanked }, { 6, 28, 6, 8, up25, unbanked }, { 6, 29, 6, 8, up25, unbanked },
        { 6, 30, 7, 8, up25, unbanked }, { 6, 31, 7, 8, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToUp25_2, {
        {  0, 26, 0, 16, flat, unbanked }, {  1, 26, 0, 16, flat, unbanked }, {  2, 26, 0, 16, flat, unbanked }, {  3, 26, 0, 16, flat, unbanked }, {  4, 26, 0, 16, flat, unbanked },
        {  5, 26, 0, 16, flat, unbanked }, {  6, 26, 0, 16, flat, unbanked }, {  7, 26, 0, 16, flat, unbanked }, {  8, 26, 0, 16, up12, unbanked }, {  9, 26, 0, 16, up12, unbanked },
        { 10, 26, 1, 16, up12, unbanked }, { 11, 26, 1, 16, up12, unbanked }, { 12, 26, 1, 16, up12, unbanked }, { 13, 26, 1, 16, up12, unbanked }, { 14, 26, 1, 16, up12, unbanked },
        { 15, 26, 1, 16, up12, unbanked }, { 16, 26, 2, 16, up12, unbanked }, { 17, 26, 2, 16, up12, unbanked }, { 18, 26, 2, 16, up12, unbanked }, { 19, 26, 2, 16, up12, unbanked },
        { 20, 26, 3, 16, up12, unbanked }, { 21, 26, 3, 16, up12, unbanked }, { 22, 26, 3, 16, up12, unbanked }, { 23, 26, 4, 16, up12, unbanked }, { 24, 26, 4, 16, up25, unbanked },
        { 25, 26, 4, 16, up25, unbanked }, { 26, 26, 5, 16, up25, unbanked }, { 27, 26, 5, 16, up25, unbanked }, { 28, 26, 6, 16, up25, unbanked }, { 29, 26, 6, 16, up25, unbanked },
        { 30, 26, 7, 16, up25, unbanked }, { 31, 26, 7, 16, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToUp25_3, {
        { 26, 31, 0, 24, flat, unbanked }, { 26, 30, 0, 24, flat, unbanked }, { 26, 29, 0, 24, flat, unbanked }, { 26, 28, 0, 24, flat, unbanked }, { 26, 27, 0, 24, flat, unbanked },
        { 26, 26, 0, 24, flat, unbanked }, { 26, 25, 0, 24, flat, unbanked }, { 26, 24, 0, 24, flat, unbanked }, { 26, 23, 0, 24, up12, unbanked }, { 26, 22, 0, 24, up12, unbanked },
        { 26, 21, 1, 24, up12, unbanked }, { 26, 20, 1, 24, up12, unbanked }, { 26, 19, 1, 24, up12, unbanked }, { 26, 18, 1, 24, up12, unbanked }, { 26, 17, 1, 24, up12, unbanked },
        { 26, 16, 1, 24, up12, unbanked }, { 26, 15, 2, 24, up12, unbanked }, { 26, 14, 2, 24, up12, unbanked }, { 26, 13, 2, 24, up12, unbanked }, { 26, 12, 2, 24, up12, unbanked },
        { 26, 11, 3, 24, up12, unbanked }, { 26, 10, 3, 24, up12, unbanked }, { 26,  9, 3, 24, up12, unbanked }, { 26,  8, 4, 24, up12, unbanked }, { 26,  7, 4, 24, up25, unbanked },
        { 26,  6, 5, 24, up25, unbanked }, { 26,  5, 5, 24, up25, unbanked }, { 26,  4, 6, 24, up25, unbanked }, { 26,  3, 6, 24, up25, unbanked }, { 26,  2, 7, 24, up25, unbanked },
        { 26,  1, 7, 24, up25, unbanked }, { 26,  0, 8, 24, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25ToFlat_0, {
        { 31, 6, 0, 0, up25, unbanked }, { 30, 6, 1, 0, up25, unbanked }, { 29, 6, 1, 0, up25, unbanked }, { 28, 6, 2, 0, up25, unbanked }, { 27, 6, 2, 0, up25, unbanked }, { 26, 6, 3, 0, up25, unbanked },
        { 25, 6, 3, 0, up25, unbanked }, { 24, 6, 3, 0, up25, unbanked }, { 23, 6, 4, 0, up12, unbanked }, { 22, 6, 4, 0, up12, unbanked }, { 21, 6, 4, 0, up12, unbanked }, { 20, 6, 5, 0, up12, unbanked },
        { 19, 6, 5, 0, up12, unbanked }, { 18, 6, 5, 0, up12, unbanked }, { 17, 6, 6, 0, up12, unbanked }, { 16, 6, 6, 0, up12, unbanked }, { 15, 6, 6, 0, up12, unbanked }, { 14, 6, 6, 0, up12, unbanked },
        { 13, 6, 7, 0, up12, unbanked }, { 12, 6, 7, 0, up12, unbanked }, { 11, 6, 7, 0, up12, unbanked }, { 10, 6, 7, 0, up12, unbanked }, {  9, 6, 7, 0, flat, unbanked }, {  8, 6, 7, 0, flat, unbanked },
        {  7, 6, 8, 0, flat, unbanked }, {  6, 6, 8, 0, flat, unbanked }, {  5, 6, 8, 0, flat, unbanked }, {  4, 6, 8, 0, flat, unbanked }, {  3, 6, 8, 0, flat, unbanked }, {  2, 6, 8, 0, flat, unbanked },
        {  1, 6, 8, 0, flat, unbanked }, {  0, 6, 8, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25ToFlat_1, {
        { 6,  0, 0, 8, up25, unbanked }, { 6,  1, 0, 8, up25, unbanked }, { 6,  2, 1, 8, up25, unbanked }, { 6,  3, 1, 8, up25, unbanked }, { 6,  4, 2, 8, up25, unbanked }, { 6,  5, 2, 8, up25, unbanked },
        { 6,  6, 3, 8, up25, unbanked }, { 6,  7, 3, 8, up25, unbanked }, { 6,  8, 4, 8, up12, unbanked }, { 6,  9, 4, 8, up12, unbanked }, { 6, 10, 4, 8, up12, unbanked }, { 6, 11, 5, 8, up12, unbanked },
        { 6, 12, 5, 8, up12, unbanked }, { 6, 13, 5, 8, up12, unbanked }, { 6, 14, 6, 8, up12, unbanked }, { 6, 15, 6, 8, up12, unbanked }, { 6, 16, 6, 8, up12, unbanked }, { 6, 17, 6, 8, up12, unbanked },
        { 6, 18, 7, 8, up12, unbanked }, { 6, 19, 7, 8, up12, unbanked }, { 6, 20, 7, 8, up12, unbanked }, { 6, 21, 7, 8, up12, unbanked }, { 6, 22, 7, 8, flat, unbanked }, { 6, 23, 7, 8, flat, unbanked },
        { 6, 24, 8, 8, flat, unbanked }, { 6, 25, 8, 8, flat, unbanked }, { 6, 26, 8, 8, flat, unbanked }, { 6, 27, 8, 8, flat, unbanked }, { 6, 28, 8, 8, flat, unbanked }, { 6, 29, 8, 8, flat, unbanked },
        { 6, 30, 8, 8, flat, unbanked }, { 6, 31, 8, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25ToFlat_2, {
        {  0, 26, 0, 16, up25, unbanked }, {  1, 26, 0, 16, up25, unbanked }, {  2, 26, 1, 16, up25, unbanked }, {  3, 26, 1, 16, up25, unbanked }, {  4, 26, 2, 16, up25, unbanked },
        {  5, 26, 2, 16, up25, unbanked }, {  6, 26, 3, 16, up25, unbanked }, {  7, 26, 3, 16, up25, unbanked }, {  8, 26, 4, 16, up12, unbanked }, {  9, 26, 4, 16, up12, unbanked },
        { 10, 26, 4, 16, up12, unbanked }, { 11, 26, 5, 16, up12, unbanked }, { 12, 26, 5, 16, up12, unbanked }, { 13, 26, 5, 16, up12, unbanked }, { 14, 26, 6, 16, up12, unbanked },
        { 15, 26, 6, 16, up12, unbanked }, { 16, 26, 6, 16, up12, unbanked }, { 17, 26, 6, 16, up12, unbanked }, { 18, 26, 7, 16, up12, unbanked }, { 19, 26, 7, 16, up12, unbanked },
        { 20, 26, 7, 16, up12, unbanked }, { 21, 26, 7, 16, up12, unbanked }, { 22, 26, 7, 16, flat, unbanked }, { 23, 26, 7, 16, flat, unbanked }, { 24, 26, 8, 16, flat, unbanked },
        { 25, 26, 8, 16, flat, unbanked }, { 26, 26, 8, 16, flat, unbanked }, { 27, 26, 8, 16, flat, unbanked }, { 28, 26, 8, 16, flat, unbanked }, { 29, 26, 8, 16, flat, unbanked },
        { 30, 26, 8, 16, flat, unbanked }, { 31, 26, 8, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25ToFlat_3, {
        { 26, 31, 0, 24, up25, unbanked }, { 26, 30, 1, 24, up25, unbanked }, { 26, 29, 1, 24, up25, unbanked }, { 26, 28, 2, 24, up25, unbanked }, { 26, 27, 2, 24, up25, unbanked },
        { 26, 26, 3, 24, up25, unbanked }, { 26, 25, 3, 24, up25, unbanked }, { 26, 24, 3, 24, up25, unbanked }, { 26, 23, 4, 24, up12, unbanked }, { 26, 22, 4, 24, up12, unbanked },
        { 26, 21, 4, 24, up12, unbanked }, { 26, 20, 5, 24, up12, unbanked }, { 26, 19, 5, 24, up12, unbanked }, { 26, 18, 5, 24, up12, unbanked }, { 26, 17, 6, 24, up12, unbanked },
        { 26, 16, 6, 24, up12, unbanked }, { 26, 15, 6, 24, up12, unbanked }, { 26, 14, 6, 24, up12, unbanked }, { 26, 13, 7, 24, up12, unbanked }, { 26, 12, 7, 24, up12, unbanked },
        { 26, 11, 7, 24, up12, unbanked }, { 26, 10, 7, 24, up12, unbanked }, { 26,  9, 7, 24, flat, unbanked }, { 26,  8, 7, 24, flat, unbanked }, { 26,  7, 8, 24, flat, unbanked },
        { 26,  6, 8, 24, flat, unbanked }, { 26,  5, 8, 24, flat, unbanked }, { 26,  4, 8, 24, flat, unbanked }, { 26,  3, 8, 24, flat, unbanked }, { 26,  2, 8, 24, flat, unbanked },
        { 26,  1, 8, 24, flat, unbanked }, { 26,  0, 8, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25_0, {
        { 31, 6, 15, 0, down25, unbanked }, { 30, 6, 15, 0, down25, unbanked }, { 29, 6, 14, 0, down25, unbanked }, { 28, 6, 14, 0, down25, unbanked }, { 27, 6, 13, 0, down25, unbanked },
        { 26, 6, 13, 0, down25, unbanked }, { 25, 6, 12, 0, down25, unbanked }, { 24, 6, 12, 0, down25, unbanked }, { 23, 6, 11, 0, down25, unbanked }, { 22, 6, 11, 0, down25, unbanked },
        { 21, 6, 10, 0, down25, unbanked }, { 20, 6, 10, 0, down25, unbanked }, { 19, 6,  9, 0, down25, unbanked }, { 18, 6,  9, 0, down25, unbanked }, { 17, 6,  8, 0, down25, unbanked },
        { 16, 6,  8, 0, down25, unbanked }, { 15, 6,  7, 0, down25, unbanked }, { 14, 6,  7, 0, down25, unbanked }, { 13, 6,  6, 0, down25, unbanked }, { 12, 6,  6, 0, down25, unbanked },
        { 11, 6,  5, 0, down25, unbanked }, { 10, 6,  5, 0, down25, unbanked }, {  9, 6,  4, 0, down25, unbanked }, {  8, 6,  4, 0, down25, unbanked }, {  7, 6,  3, 0, down25, unbanked },
        {  6, 6,  3, 0, down25, unbanked }, {  5, 6,  2, 0, down25, unbanked }, {  4, 6,  2, 0, down25, unbanked }, {  3, 6,  1, 0, down25, unbanked }, {  2, 6,  1, 0, down25, unbanked },
        {  1, 6,  0, 0, down25, unbanked }, {  0, 6,  0, 0, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25_1, {
        { 6,  0, 15, 8, down25, unbanked }, { 6,  1, 14, 8, down25, unbanked }, { 6,  2, 14, 8, down25, unbanked }, { 6,  3, 13, 8, down25, unbanked }, { 6,  4, 13, 8, down25, unbanked },
        { 6,  5, 12, 8, down25, unbanked }, { 6,  6, 12, 8, down25, unbanked }, { 6,  7, 11, 8, down25, unbanked }, { 6,  8, 11, 8, down25, unbanked }, { 6,  9, 10, 8, down25, unbanked },
        { 6, 10, 10, 8, down25, unbanked }, { 6, 11,  9, 8, down25, unbanked }, { 6, 12,  9, 8, down25, unbanked }, { 6, 13,  8, 8, down25, unbanked }, { 6, 14,  8, 8, down25, unbanked },
        { 6, 15,  7, 8, down25, unbanked }, { 6, 16,  7, 8, down25, unbanked }, { 6, 17,  6, 8, down25, unbanked }, { 6, 18,  6, 8, down25, unbanked }, { 6, 19,  5, 8, down25, unbanked },
        { 6, 20,  5, 8, down25, unbanked }, { 6, 21,  4, 8, down25, unbanked }, { 6, 22,  4, 8, down25, unbanked }, { 6, 23,  3, 8, down25, unbanked }, { 6, 24,  3, 8, down25, unbanked },
        { 6, 25,  2, 8, down25, unbanked }, { 6, 26,  2, 8, down25, unbanked }, { 6, 27,  1, 8, down25, unbanked }, { 6, 28,  1, 8, down25, unbanked }, { 6, 29,  0, 8, down25, unbanked },
        { 6, 30,  0, 8, down25, unbanked }, { 6, 31, -1, 8, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25_2, {
        {  0, 26, 15, 16, down25, unbanked }, {  1, 26, 14, 16, down25, unbanked }, {  2, 26, 14, 16, down25, unbanked }, {  3, 26, 13, 16, down25, unbanked }, {  4, 26, 13, 16, down25, unbanked },
        {  5, 26, 12, 16, down25, unbanked }, {  6, 26, 12, 16, down25, unbanked }, {  7, 26, 11, 16, down25, unbanked }, {  8, 26, 11, 16, down25, unbanked }, {  9, 26, 10, 16, down25, unbanked },
        { 10, 26, 10, 16, down25, unbanked }, { 11, 26,  9, 16, down25, unbanked }, { 12, 26,  9, 16, down25, unbanked }, { 13, 26,  8, 16, down25, unbanked }, { 14, 26,  8, 16, down25, unbanked },
        { 15, 26,  7, 16, down25, unbanked }, { 16, 26,  7, 16, down25, unbanked }, { 17, 26,  6, 16, down25, unbanked }, { 18, 26,  6, 16, down25, unbanked }, { 19, 26,  5, 16, down25, unbanked },
        { 20, 26,  5, 16, down25, unbanked }, { 21, 26,  4, 16, down25, unbanked }, { 22, 26,  4, 16, down25, unbanked }, { 23, 26,  3, 16, down25, unbanked }, { 24, 26,  3, 16, down25, unbanked },
        { 25, 26,  2, 16, down25, unbanked }, { 26, 26,  2, 16, down25, unbanked }, { 27, 26,  1, 16, down25, unbanked }, { 28, 26,  1, 16, down25, unbanked }, { 29, 26,  0, 16, down25, unbanked },
        { 30, 26,  0, 16, down25, unbanked }, { 31, 26, -1, 16, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25_3, {
        { 26, 31, 15, 24, down25, unbanked }, { 26, 30, 15, 24, down25, unbanked }, { 26, 29, 14, 24, down25, unbanked }, { 26, 28, 14, 24, down25, unbanked }, { 26, 27, 13, 24, down25, unbanked },
        { 26, 26, 13, 24, down25, unbanked }, { 26, 25, 12, 24, down25, unbanked }, { 26, 24, 12, 24, down25, unbanked }, { 26, 23, 11, 24, down25, unbanked }, { 26, 22, 11, 24, down25, unbanked },
        { 26, 21, 10, 24, down25, unbanked }, { 26, 20, 10, 24, down25, unbanked }, { 26, 19,  9, 24, down25, unbanked }, { 26, 18,  9, 24, down25, unbanked }, { 26, 17,  8, 24, down25, unbanked },
        { 26, 16,  8, 24, down25, unbanked }, { 26, 15,  7, 24, down25, unbanked }, { 26, 14,  7, 24, down25, unbanked }, { 26, 13,  6, 24, down25, unbanked }, { 26, 12,  6, 24, down25, unbanked },
        { 26, 11,  5, 24, down25, unbanked }, { 26, 10,  5, 24, down25, unbanked }, { 26,  9,  4, 24, down25, unbanked }, { 26,  8,  4, 24, down25, unbanked }, { 26,  7,  3, 24, down25, unbanked },
        { 26,  6,  3, 24, down25, unbanked }, { 26,  5,  2, 24, down25, unbanked }, { 26,  4,  2, 24, down25, unbanked }, { 26,  3,  1, 24, down25, unbanked }, { 26,  2,  1, 24, down25, unbanked },
        { 26,  1,  0, 24, down25, unbanked }, { 26,  0,  0, 24, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToDown25_0, {
        { 31, 6, 8, 0,   flat, unbanked }, { 30, 6, 8, 0,   flat, unbanked }, { 29, 6, 8, 0,   flat, unbanked }, { 28, 6, 8, 0,   flat, unbanked }, { 27, 6, 8, 0,   flat, unbanked },
        { 26, 6, 8, 0,   flat, unbanked }, { 25, 6, 8, 0,   flat, unbanked }, { 24, 6, 8, 0,   flat, unbanked }, { 23, 6, 7, 0, down12, unbanked }, { 22, 6, 7, 0, down12, unbanked },
        { 21, 6, 7, 0, down12, unbanked }, { 20, 6, 7, 0, down12, unbanked }, { 19, 6, 7, 0, down12, unbanked }, { 18, 6, 7, 0, down12, unbanked }, { 17, 6, 6, 0, down12, unbanked },
        { 16, 6, 6, 0, down12, unbanked }, { 15, 6, 6, 0, down12, unbanked }, { 14, 6, 6, 0, down12, unbanked }, { 13, 6, 5, 0, down12, unbanked }, { 12, 6, 5, 0, down12, unbanked },
        { 11, 6, 5, 0, down12, unbanked }, { 10, 6, 4, 0, down12, unbanked }, {  9, 6, 4, 0, down12, unbanked }, {  8, 6, 4, 0, down12, unbanked }, {  7, 6, 3, 0, down25, unbanked },
        {  6, 6, 3, 0, down25, unbanked }, {  5, 6, 2, 0, down25, unbanked }, {  4, 6, 2, 0, down25, unbanked }, {  3, 6, 1, 0, down25, unbanked }, {  2, 6, 1, 0, down25, unbanked },
        {  1, 6, 0, 0, down25, unbanked }, {  0, 6, 0, 0, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToDown25_1, {
        { 6,  0, 8, 8,   flat, unbanked }, { 6,  1, 8, 8,   flat, unbanked }, { 6,  2, 8, 8,   flat, unbanked }, { 6,  3, 8, 8,   flat, unbanked }, { 6,  4, 8, 8,   flat, unbanked },
        { 6,  5, 8, 8,   flat, unbanked }, { 6,  6, 8, 8,   flat, unbanked }, { 6,  7, 8, 8,   flat, unbanked }, { 6,  8, 7, 8, down12, unbanked }, { 6,  9, 7, 8, down12, unbanked },
        { 6, 10, 7, 8, down12, unbanked }, { 6, 11, 7, 8, down12, unbanked }, { 6, 12, 7, 8, down12, unbanked }, { 6, 13, 7, 8, down12, unbanked }, { 6, 14, 6, 8, down12, unbanked },
        { 6, 15, 6, 8, down12, unbanked }, { 6, 16, 6, 8, down12, unbanked }, { 6, 17, 6, 8, down12, unbanked }, { 6, 18, 5, 8, down12, unbanked }, { 6, 19, 5, 8, down12, unbanked },
        { 6, 20, 5, 8, down12, unbanked }, { 6, 21, 4, 8, down12, unbanked }, { 6, 22, 4, 8, down12, unbanked }, { 6, 23, 4, 8, down12, unbanked }, { 6, 24, 3, 8, down25, unbanked },
        { 6, 25, 3, 8, down25, unbanked }, { 6, 26, 3, 8, down25, unbanked }, { 6, 27, 2, 8, down25, unbanked }, { 6, 28, 2, 8, down25, unbanked }, { 6, 29, 1, 8, down25, unbanked },
        { 6, 30, 1, 8, down25, unbanked }, { 6, 31, 0, 8, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToDown25_2, {
        {  0, 26, 8, 16,   flat, unbanked }, {  1, 26, 8, 16,   flat, unbanked }, {  2, 26, 8, 16,   flat, unbanked }, {  3, 26, 8, 16,   flat, unbanked }, {  4, 26, 8, 16,   flat, unbanked },
        {  5, 26, 8, 16,   flat, unbanked }, {  6, 26, 8, 16,   flat, unbanked }, {  7, 26, 8, 16,   flat, unbanked }, {  8, 26, 7, 16, down12, unbanked }, {  9, 26, 7, 16, down12, unbanked },
        { 10, 26, 7, 16, down12, unbanked }, { 11, 26, 7, 16, down12, unbanked }, { 12, 26, 7, 16, down12, unbanked }, { 13, 26, 7, 16, down12, unbanked }, { 14, 26, 6, 16, down12, unbanked },
        { 15, 26, 6, 16, down12, unbanked }, { 16, 26, 6, 16, down12, unbanked }, { 17, 26, 6, 16, down12, unbanked }, { 18, 26, 5, 16, down12, unbanked }, { 19, 26, 5, 16, down12, unbanked },
        { 20, 26, 5, 16, down12, unbanked }, { 21, 26, 4, 16, down12, unbanked }, { 22, 26, 4, 16, down12, unbanked }, { 23, 26, 4, 16, down12, unbanked }, { 24, 26, 3, 16, down25, unbanked },
        { 25, 26, 3, 16, down25, unbanked }, { 26, 26, 3, 16, down25, unbanked }, { 27, 26, 2, 16, down25, unbanked }, { 28, 26, 2, 16, down25, unbanked }, { 29, 26, 1, 16, down25, unbanked },
        { 30, 26, 1, 16, down25, unbanked }, { 31, 26, 0, 16, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToDown25_3, {
        { 26, 31, 8, 24,   flat, unbanked }, { 26, 30, 8, 24,   flat, unbanked }, { 26, 29, 8, 24,   flat, unbanked }, { 26, 28, 8, 24,   flat, unbanked }, { 26, 27, 8, 24,   flat, unbanked },
        { 26, 26, 8, 24,   flat, unbanked }, { 26, 25, 8, 24,   flat, unbanked }, { 26, 24, 8, 24,   flat, unbanked }, { 26, 23, 7, 24, down12, unbanked }, { 26, 22, 7, 24, down12, unbanked },
        { 26, 21, 7, 24, down12, unbanked }, { 26, 20, 7, 24, down12, unbanked }, { 26, 19, 7, 24, down12, unbanked }, { 26, 18, 7, 24, down12, unbanked }, { 26, 17, 6, 24, down12, unbanked },
        { 26, 16, 6, 24, down12, unbanked }, { 26, 15, 6, 24, down12, unbanked }, { 26, 14, 6, 24, down12, unbanked }, { 26, 13, 5, 24, down12, unbanked }, { 26, 12, 5, 24, down12, unbanked },
        { 26, 11, 5, 24, down12, unbanked }, { 26, 10, 4, 24, down12, unbanked }, { 26,  9, 4, 24, down12, unbanked }, { 26,  8, 4, 24, down12, unbanked }, { 26,  7, 3, 24, down25, unbanked },
        { 26,  6, 3, 24, down25, unbanked }, { 26,  5, 2, 24, down25, unbanked }, { 26,  4, 2, 24, down25, unbanked }, { 26,  3, 1, 24, down25, unbanked }, { 26,  2, 1, 24, down25, unbanked },
        { 26,  1, 0, 24, down25, unbanked }, { 26,  0, 0, 24, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25ToFlat_0, {
        { 31, 6, 7, 0, down25, unbanked }, { 30, 6, 7, 0, down25, unbanked }, { 29, 6, 6, 0, down25, unbanked }, { 28, 6, 6, 0, down25, unbanked }, { 27, 6, 5, 0, down25, unbanked },
        { 26, 6, 5, 0, down25, unbanked }, { 25, 6, 4, 0, down25, unbanked }, { 24, 6, 4, 0, down25, unbanked }, { 23, 6, 4, 0, down12, unbanked }, { 22, 6, 3, 0, down12, unbanked },
        { 21, 6, 3, 0, down12, unbanked }, { 20, 6, 3, 0, down12, unbanked }, { 19, 6, 2, 0, down12, unbanked }, { 18, 6, 2, 0, down12, unbanked }, { 17, 6, 2, 0, down12, unbanked },
        { 16, 6, 2, 0, down12, unbanked }, { 15, 6, 1, 0, down12, unbanked }, { 14, 6, 1, 0, down12, unbanked }, { 13, 6, 1, 0, down12, unbanked }, { 12, 6, 1, 0, down12, unbanked },
        { 11, 6, 1, 0, down12, unbanked }, { 10, 6, 1, 0, down12, unbanked }, {  9, 6, 0, 0,   flat, unbanked }, {  8, 6, 0, 0,   flat, unbanked }, {  7, 6, 0, 0,   flat, unbanked },
        {  6, 6, 0, 0,   flat, unbanked }, {  5, 6, 0, 0,   flat, unbanked }, {  4, 6, 0, 0,   flat, unbanked }, {  3, 6, 0, 0,   flat, unbanked }, {  2, 6, 0, 0,   flat, unbanked },
        {  1, 6, 0, 0,   flat, unbanked }, {  0, 6, 0, 0,   flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25ToFlat_1, {
        { 6,  0, 8, 8, down25, unbanked }, { 6,  1, 7, 8, down25, unbanked }, { 6,  2, 7, 8, down25, unbanked }, { 6,  3, 6, 8, down25, unbanked }, { 6,  4, 6, 8, down25, unbanked },
        { 6,  5, 5, 8, down25, unbanked }, { 6,  6, 5, 8, down25, unbanked }, { 6,  7, 4, 8, down25, unbanked }, { 6,  8, 4, 8, down12, unbanked }, { 6,  9, 3, 8, down12, unbanked },
        { 6, 10, 3, 8, down12, unbanked }, { 6, 11, 3, 8, down12, unbanked }, { 6, 12, 2, 8, down12, unbanked }, { 6, 13, 2, 8, down12, unbanked }, { 6, 14, 2, 8, down12, unbanked },
        { 6, 15, 2, 8, down12, unbanked }, { 6, 16, 1, 8, down12, unbanked }, { 6, 17, 1, 8, down12, unbanked }, { 6, 18, 1, 8, down12, unbanked }, { 6, 19, 1, 8, down12, unbanked },
        { 6, 20, 1, 8, down12, unbanked }, { 6, 21, 1, 8, down12, unbanked }, { 6, 22, 0, 8,   flat, unbanked }, { 6, 23, 0, 8,   flat, unbanked }, { 6, 24, 0, 8,   flat, unbanked },
        { 6, 25, 0, 8,   flat, unbanked }, { 6, 26, 0, 8,   flat, unbanked }, { 6, 27, 0, 8,   flat, unbanked }, { 6, 28, 0, 8,   flat, unbanked }, { 6, 29, 0, 8,   flat, unbanked },
        { 6, 30, 0, 8,   flat, unbanked }, { 6, 31, 0, 8,   flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25ToFlat_2, {
        {  0, 26, 8, 16, down25, unbanked }, {  1, 26, 7, 16, down25, unbanked }, {  2, 26, 7, 16, down25, unbanked }, {  3, 26, 6, 16, down25, unbanked }, {  4, 26, 6, 16, down25, unbanked },
        {  5, 26, 5, 16, down25, unbanked }, {  6, 26, 5, 16, down25, unbanked }, {  7, 26, 4, 16, down25, unbanked }, {  8, 26, 4, 16, down12, unbanked }, {  9, 26, 3, 16, down12, unbanked },
        { 10, 26, 3, 16, down12, unbanked }, { 11, 26, 3, 16, down12, unbanked }, { 12, 26, 2, 16, down12, unbanked }, { 13, 26, 2, 16, down12, unbanked }, { 14, 26, 2, 16, down12, unbanked },
        { 15, 26, 2, 16, down12, unbanked }, { 16, 26, 1, 16, down12, unbanked }, { 17, 26, 1, 16, down12, unbanked }, { 18, 26, 1, 16, down12, unbanked }, { 19, 26, 1, 16, down12, unbanked },
        { 20, 26, 1, 16, down12, unbanked }, { 21, 26, 1, 16, down12, unbanked }, { 22, 26, 0, 16,   flat, unbanked }, { 23, 26, 0, 16,   flat, unbanked }, { 24, 26, 0, 16,   flat, unbanked },
        { 25, 26, 0, 16,   flat, unbanked }, { 26, 26, 0, 16,   flat, unbanked }, { 27, 26, 0, 16,   flat, unbanked }, { 28, 26, 0, 16,   flat, unbanked }, { 29, 26, 0, 16,   flat, unbanked },
        { 30, 26, 0, 16,   flat, unbanked }, { 31, 26, 0, 16,   flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25ToFlat_3, {
        { 26, 31, 7, 24, down25, unbanked }, { 26, 30, 7, 24, down25, unbanked }, { 26, 29, 6, 24, down25, unbanked }, { 26, 28, 6, 24, down25, unbanked }, { 26, 27, 5, 24, down25, unbanked },
        { 26, 26, 5, 24, down25, unbanked }, { 26, 25, 4, 24, down25, unbanked }, { 26, 24, 4, 24, down25, unbanked }, { 26, 23, 4, 24, down12, unbanked }, { 26, 22, 3, 24, down12, unbanked },
        { 26, 21, 3, 24, down12, unbanked }, { 26, 20, 3, 24, down12, unbanked }, { 26, 19, 2, 24, down12, unbanked }, { 26, 18, 2, 24, down12, unbanked }, { 26, 17, 2, 24, down12, unbanked },
        { 26, 16, 2, 24, down12, unbanked }, { 26, 15, 1, 24, down12, unbanked }, { 26, 14, 1, 24, down12, unbanked }, { 26, 13, 1, 24, down12, unbanked }, { 26, 12, 1, 24, down12, unbanked },
        { 26, 11, 1, 24, down12, unbanked }, { 26, 10, 1, 24, down12, unbanked }, { 26,  9, 0, 24,   flat, unbanked }, { 26,  8, 0, 24,   flat, unbanked }, { 26,  7, 0, 24,   flat, unbanked },
        { 26,  6, 0, 24,   flat, unbanked }, { 26,  5, 0, 24,   flat, unbanked }, { 26,  4, 0, 24,   flat, unbanked }, { 26,  3, 0, 24,   flat, unbanked }, { 26,  2, 0, 24,   flat, unbanked },
        { 26,  1, 0, 24,   flat, unbanked }, { 26,  0, 0, 24,   flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn1Tile_0, {
        { 31, 6, 0,  0, flat, unbanked }, { 30, 6, 0, 30, flat, unbanked }, { 29, 5, 0, 30, flat, unbanked }, { 28, 5, 0, 28, flat, unbanked }, { 27, 4, 0, 28, flat, unbanked }, { 27, 3, 0, 26, flat, unbanked },
        { 26, 2, 0, 26, flat, unbanked }, { 26, 1, 0, 24, flat, unbanked }, { 26, 0, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn1Tile_1, {
        { 6, 0, 0, 8, flat, unbanked }, { 6, 1, 0, 8, flat, unbanked }, { 6, 2, 0, 6, flat, unbanked }, { 5, 3, 0, 6, flat, unbanked }, { 5, 4, 0, 4, flat, unbanked }, { 4, 5, 0, 4, flat, unbanked },
        { 3, 5, 0, 2, flat, unbanked }, { 2, 6, 0, 2, flat, unbanked }, { 1, 6, 0, 0, flat, unbanked }, { 0, 6, 0, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn1Tile_2, {
        { 0, 26, 0, 16, flat, unbanked }, { 1, 26, 0, 16, flat, unbanked }, { 2, 26, 0, 14, flat, unbanked }, { 3, 27, 0, 14, flat, unbanked }, { 4, 27, 0, 12, flat, unbanked }, { 5, 28, 0, 12, flat, unbanked },
        { 5, 29, 0, 10, flat, unbanked }, { 6, 30, 0, 10, flat, unbanked }, { 6, 31, 0,  8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn1Tile_3, {
        { 26, 31, 0, 24, flat, unbanked }, { 26, 30, 0, 22, flat, unbanked }, { 27, 29, 0, 22, flat, unbanked }, { 27, 28, 0, 20, flat, unbanked }, { 28, 27, 0, 20, flat, unbanked },
        { 29, 27, 0, 18, flat, unbanked }, { 30, 26, 0, 18, flat, unbanked }, { 31, 26, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn1Tile_0, {
        { 31,  6, 0, 0, flat, unbanked }, { 30,  6, 0, 0, flat, unbanked }, { 29,  6, 0, 0, flat, unbanked }, { 28,  6, 0, 0, flat, unbanked }, { 27,  6, 0, 0, flat, unbanked }, { 26,  7, 0, 1, flat, unbanked },
        { 25,  7, 0, 1, flat, unbanked }, { 24,  7, 0, 1, flat, unbanked }, { 23,  7, 0, 1, flat, unbanked }, { 22,  8, 0, 2, flat, unbanked }, { 21,  8, 0, 2, flat, unbanked }, { 20,  8, 0, 2, flat, unbanked },
        { 19,  9, 0, 3, flat, unbanked }, { 18,  9, 0, 3, flat, unbanked }, { 17, 10, 0, 3, flat, unbanked }, { 16, 11, 0, 3, flat, unbanked }, { 15, 12, 0, 4, flat, unbanked }, { 14, 13, 0, 4, flat, unbanked },
        { 13, 14, 0, 4, flat, unbanked }, { 12, 15, 0, 4, flat, unbanked }, { 11, 16, 0, 5, flat, unbanked }, { 10, 17, 0, 5, flat, unbanked }, {  9, 18, 0, 5, flat, unbanked }, {  9, 19, 0, 5, flat, unbanked },
        {  8, 20, 0, 6, flat, unbanked }, {  8, 21, 0, 6, flat, unbanked }, {  8, 22, 0, 6, flat, unbanked }, {  7, 23, 0, 7, flat, unbanked }, {  7, 24, 0, 7, flat, unbanked }, {  7, 25, 0, 7, flat, unbanked },
        {  7, 26, 0, 7, flat, unbanked }, {  6, 27, 0, 8, flat, unbanked }, {  6, 28, 0, 8, flat, unbanked }, {  6, 29, 0, 8, flat, unbanked }, {  6, 30, 0, 8, flat, unbanked }, {  6, 31, 0, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn1Tile_1, {
        {  6,  0, 0,  8, flat, unbanked }, {  6,  1, 0,  8, flat, unbanked }, {  6,  2, 0,  8, flat, unbanked }, {  6,  3, 0,  8, flat, unbanked }, {  6,  4, 0,  8, flat, unbanked },
        {  6,  5, 0,  8, flat, unbanked }, {  7,  6, 0,  9, flat, unbanked }, {  7,  7, 0,  9, flat, unbanked }, {  7,  8, 0,  9, flat, unbanked }, {  7,  9, 0,  9, flat, unbanked },
        {  8, 10, 0, 10, flat, unbanked }, {  8, 11, 0, 10, flat, unbanked }, {  8, 12, 0, 10, flat, unbanked }, {  9, 13, 0, 11, flat, unbanked }, {  9, 14, 0, 11, flat, unbanked },
        { 10, 15, 0, 11, flat, unbanked }, { 11, 16, 0, 11, flat, unbanked }, { 12, 17, 0, 12, flat, unbanked }, { 13, 18, 0, 12, flat, unbanked }, { 14, 19, 0, 12, flat, unbanked },
        { 15, 20, 0, 12, flat, unbanked }, { 16, 21, 0, 13, flat, unbanked }, { 17, 22, 0, 13, flat, unbanked }, { 18, 23, 0, 13, flat, unbanked }, { 19, 23, 0, 13, flat, unbanked },
        { 20, 24, 0, 14, flat, unbanked }, { 21, 24, 0, 14, flat, unbanked }, { 22, 24, 0, 14, flat, unbanked }, { 23, 25, 0, 15, flat, unbanked }, { 24, 25, 0, 15, flat, unbanked },
        { 25, 25, 0, 15, flat, unbanked }, { 26, 25, 0, 15, flat, unbanked }, { 27, 26, 0, 16, flat, unbanked }, { 28, 26, 0, 16, flat, unbanked }, { 29, 26, 0, 16, flat, unbanked },
        { 30, 26, 0, 16, flat, unbanked }, { 31, 26, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn1Tile_2, {
        {  0, 26, 0, 16, flat, unbanked }, {  1, 26, 0, 16, flat, unbanked }, {  2, 26, 0, 16, flat, unbanked }, {  3, 26, 0, 16, flat, unbanked }, {  4, 26, 0, 16, flat, unbanked },
        {  5, 26, 0, 16, flat, unbanked }, {  6, 25, 0, 17, flat, unbanked }, {  7, 25, 0, 17, flat, unbanked }, {  8, 25, 0, 17, flat, unbanked }, {  9, 25, 0, 17, flat, unbanked },
        { 10, 24, 0, 18, flat, unbanked }, { 11, 24, 0, 18, flat, unbanked }, { 12, 24, 0, 18, flat, unbanked }, { 13, 23, 0, 19, flat, unbanked }, { 14, 23, 0, 19, flat, unbanked },
        { 15, 22, 0, 19, flat, unbanked }, { 16, 21, 0, 19, flat, unbanked }, { 17, 20, 0, 20, flat, unbanked }, { 18, 19, 0, 20, flat, unbanked }, { 19, 18, 0, 20, flat, unbanked },
        { 20, 17, 0, 20, flat, unbanked }, { 21, 16, 0, 21, flat, unbanked }, { 22, 15, 0, 21, flat, unbanked }, { 23, 14, 0, 21, flat, unbanked }, { 23, 13, 0, 21, flat, unbanked },
        { 24, 12, 0, 22, flat, unbanked }, { 24, 11, 0, 22, flat, unbanked }, { 24, 10, 0, 22, flat, unbanked }, { 25,  9, 0, 23, flat, unbanked }, { 25,  8, 0, 23, flat, unbanked },
        { 25,  7, 0, 23, flat, unbanked }, { 25,  6, 0, 23, flat, unbanked }, { 26,  5, 0, 24, flat, unbanked }, { 26,  4, 0, 24, flat, unbanked }, { 26,  3, 0, 24, flat, unbanked },
        { 26,  2, 0, 24, flat, unbanked }, { 26,  1, 0, 24, flat, unbanked }, { 26,  0, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn1Tile_3, {
        { 26, 31, 0, 24, flat, unbanked }, { 26, 30, 0, 24, flat, unbanked }, { 26, 29, 0, 24, flat, unbanked }, { 26, 28, 0, 24, flat, unbanked }, { 26, 27, 0, 24, flat, unbanked },
        { 25, 26, 0, 25, flat, unbanked }, { 25, 25, 0, 25, flat, unbanked }, { 25, 24, 0, 25, flat, unbanked }, { 25, 23, 0, 25, flat, unbanked }, { 24, 22, 0, 26, flat, unbanked },
        { 24, 21, 0, 26, flat, unbanked }, { 24, 20, 0, 26, flat, unbanked }, { 23, 19, 0, 27, flat, unbanked }, { 23, 18, 0, 27, flat, unbanked }, { 22, 17, 0, 27, flat, unbanked },
        { 21, 16, 0, 27, flat, unbanked }, { 20, 15, 0, 28, flat, unbanked }, { 19, 14, 0, 28, flat, unbanked }, { 18, 13, 0, 28, flat, unbanked }, { 17, 12, 0, 28, flat, unbanked },
        { 16, 11, 0, 29, flat, unbanked }, { 15, 10, 0, 29, flat, unbanked }, { 14,  9, 0, 29, flat, unbanked }, { 13,  9, 0, 29, flat, unbanked }, { 12,  8, 0, 30, flat, unbanked },
        { 11,  8, 0, 30, flat, unbanked }, { 10,  8, 0, 30, flat, unbanked }, {  9,  7, 0, 31, flat, unbanked }, {  8,  7, 0, 31, flat, unbanked }, {  7,  7, 0, 31, flat, unbanked },
        {  6,  7, 0, 31, flat, unbanked }, {  5,  6, 0,  0, flat, unbanked }, {  4,  6, 0,  0, flat, unbanked }, {  3,  6, 0,  0, flat, unbanked }, {  2,  6, 0,  0, flat, unbanked },
        {  1,  6, 0,  0, flat, unbanked }, {  0,  6, 0,  0, flat, unbanked },
    })

    static constexpr const VehicleInfoList * TrackVehicleInfoListChairliftGoingOut[] = {
        &Flat_0,                                      &Flat_1,                                      &Flat_2,                                      &Flat_3,                                      // Flat
        &Flat_0,                                      &Flat_1,                                      &Flat_2,                                      &Flat_3,                                      // EndStation
        &Flat_0,                                      &Flat_1,                                      &Flat_2,                                      &Flat_3,                                      // BeginStation
        &Flat_0,                                      &Flat_1,                                      &Flat_2,                                      &Flat_3,                                      // MiddleStation
        &Up25_0,                                      &Up25_1,                                      &Up25_2,                                      &Up25_3,                                      // Up25
        &Default::Up60_0,                             &Default::Up60_1,                             &Default::Up60_2,                             &Default::Up60_3,                             // Up60
        &FlatToUp25_0,                                &FlatToUp25_1,                                &FlatToUp25_2,                                &FlatToUp25_3,                                // FlatToUp25
        &Default::Up25ToUp60_0,                       &Default::Up25ToUp60_1,                       &Default::Up25ToUp60_2,                       &Default::Up25ToUp60_3,                       // Up25ToUp60
        &Default::Up60ToUp25_0,                       &Default::Up60ToUp25_1,                       &Default::Up60ToUp25_2,                       &Default::Up60ToUp25_3,                       // Up60ToUp25
        &Up25ToFlat_0,                                &Up25ToFlat_1,                                &Up25ToFlat_2,                                &Up25ToFlat_3,                                // Up25ToFlat
        &Down25_0,                                    &Down25_1,                                    &Down25_2,                                    &Down25_3,                                    // Down25
        &Default::Down60_0,                           &Default::Down60_1,                           &Default::Down60_2,                           &Default::Down60_3,                           // Down60
        &FlatToDown25_0,                              &FlatToDown25_1,                              &FlatToDown25_2,                              &FlatToDown25_3,                              // FlatToDown25
        &Default::Down25ToDown60_0,                   &Default::Down25ToDown60_1,                   &Default::Down25ToDown60_2,                   &Default::Down25ToDown60_3,                   // Down25ToDown60
        &Default::Down60ToDown25_0,                   &Default::Down60ToDown25_1,                   &Default::Down60ToDown25_2,                   &Default::Down60ToDown25_3,                   // Down60ToDown25
        &Down25ToFlat_0,                              &Down25ToFlat_1,                              &Down25ToFlat_2,                              &Down25ToFlat_3,                              // Down25ToFlat
        &Default::LeftQuarterTurn5Tiles_0,            &Default::LeftQuarterTurn5Tiles_1,            &Default::LeftQuarterTurn5Tiles_2,            &Default::LeftQuarterTurn5Tiles_3,            // LeftQuarterTurn5Tiles
        &Default::RightQuarterTurn5Tiles_0,           &Default::RightQuarterTurn5Tiles_1,           &Default::RightQuarterTurn5Tiles_2,           &Default::RightQuarterTurn5Tiles_3,           // RightQuarterTurn5Tiles
        &Default::FlatToLeftBank_0,                   &Default::FlatToLeftBank_1,                   &Default::FlatToLeftBank_2,                   &Default::FlatToLeftBank_3,                   // FlatToLeftBank
        &Default::FlatToRightBank_0,                  &Default::FlatToRightBank_1,                  &Default::FlatToRightBank_2,                  &Default::FlatToRightBank_3,                  // FlatToRightBank
        &Default::LeftBankToFlat_0,                   &Default::LeftBankToFlat_1,                   &Default::LeftBankToFlat_2,                   &Default::LeftBankToFlat_3,                   // LeftBankToFlat
        &Default::RightBankToFlat_0,                  &Default::RightBankToFlat_1,                  &Default::RightBankToFlat_2,                  &Default::RightBankToFlat_3,                  // RightBankToFlat
        &Default::BankedLeftQuarterTurn5Tiles_0,      &Default::BankedLeftQuarterTurn5Tiles_1,      &Default::BankedLeftQuarterTurn5Tiles_2,      &Default::BankedLeftQuarterTurn5Tiles_3,      // BankedLeftQuarterTurn5Tiles
        &Default::BankedRightQuarterTurn5Tiles_0,     &Default::BankedRightQuarterTurn5Tiles_1,     &Default::BankedRightQuarterTurn5Tiles_2,     &Default::BankedRightQuarterTurn5Tiles_3,     // BankedRightQuarterTurn5Tiles
        &Default::LeftBankToUp25_0,                   &Default::LeftBankToUp25_1,                   &Default::LeftBankToUp25_2,                   &Default::LeftBankToUp25_3,                   // LeftBankToUp25
        &Default::RightBankToUp25_0,                  &Default::RightBankToUp25_1,                  &Default::RightBankToUp25_2,                  &Default::RightBankToUp25_3,                  // RightBankToUp25
        &Default::Up25ToLeftBank_0,                   &Default::Up25ToLeftBank_1,                   &Default::Up25ToLeftBank_2,                   &Default::Up25ToLeftBank_3,                   // Up25ToLeftBank
        &Default::Up25ToRightBank_0,                  &Default::Up25ToRightBank_1,                  &Default::Up25ToRightBank_2,                  &Default::Up25ToRightBank_3,                  // Up25ToRightBank
        &Default::LeftBankToDown25_0,                 &Default::LeftBankToDown25_1,                 &Default::LeftBankToDown25_2,                 &Default::LeftBankToDown25_3,                 // LeftBankToDown25
        &Default::RightBankToDown25_0,                &Default::RightBankToDown25_1,                &Default::RightBankToDown25_2,                &Default::RightBankToDown25_3,                // RightBankToDown25
        &Default::Down25ToLeftBank_0,                 &Default::Down25ToLeftBank_1,                 &Default::Down25ToLeftBank_2,                 &Default::Down25ToLeftBank_3,                 // Down25ToLeftBank
        &Default::Down25ToRightBank_0,                &Default::Down25ToRightBank_1,                &Default::Down25ToRightBank_2,                &Default::Down25ToRightBank_3,                // Down25ToRightBank
        &Default::LeftBank_0,                         &Default::LeftBank_1,                         &Default::LeftBank_2,                         &Default::LeftBank_3,                         // LeftBank
        &Default::RightBank_0,                        &Default::RightBank_1,                        &Default::RightBank_2,                        &Default::RightBank_3,                        // RightBank
        &Default::LeftQuarterTurn5TilesUp25_0,        &Default::LeftQuarterTurn5TilesUp25_1,        &Default::LeftQuarterTurn5TilesUp25_2,        &Default::LeftQuarterTurn5TilesUp25_3,        // LeftQuarterTurn5TilesUp25
        &Default::RightQuarterTurn5TilesUp25_0,       &Default::RightQuarterTurn5TilesUp25_1,       &Default::RightQuarterTurn5TilesUp25_2,       &Default::RightQuarterTurn5TilesUp25_3,       // RightQuarterTurn5TilesUp25
        &Default::LeftQuarterTurn5TilesDown25_0,      &Default::LeftQuarterTurn5TilesDown25_1,      &Default::LeftQuarterTurn5TilesDown25_2,      &Default::LeftQuarterTurn5TilesDown25_3,      // LeftQuarterTurn5TilesDown25
        &Default::RightQuarterTurn5TilesDown25_0,     &Default::RightQuarterTurn5TilesDown25_1,     &Default::RightQuarterTurn5TilesDown25_2,     &Default::RightQuarterTurn5TilesDown25_3,     // RightQuarterTurn5TilesDown25
        &Default::SBendLeft_0,                        &Default::SBendLeft_1,                        &Default::SBendLeft_2,                        &Default::SBendLeft_3,                        // SBendLeft
        &Default::SBendRight_0,                       &Default::SBendRight_1,                       &Default::SBendRight_2,                       &Default::SBendRight_3,                       // SBendRight
        &Default::LeftVerticalLoop_0,                 &Default::LeftVerticalLoop_1,                 &Default::LeftVerticalLoop_2,                 &Default::LeftVerticalLoop_3,                 // LeftVerticalLoop
        &Default::RightVerticalLoop_0,                &Default::RightVerticalLoop_1,                &Default::RightVerticalLoop_2,                &Default::RightVerticalLoop_3,                // RightVerticalLoop
        &Default::LeftQuarterTurn3Tiles_0,            &Default::LeftQuarterTurn3Tiles_1,            &Default::LeftQuarterTurn3Tiles_2,            &Default::LeftQuarterTurn3Tiles_3,            // LeftQuarterTurn3Tiles
        &Default::RightQuarterTurn3Tiles_0,           &Default::RightQuarterTurn3Tiles_1,           &Default::RightQuarterTurn3Tiles_2,           &Default::RightQuarterTurn3Tiles_3,           // RightQuarterTurn3Tiles
        &Default::LeftBankedQuarterTurn3Tiles_0,      &Default::LeftBankedQuarterTurn3Tiles_1,      &Default::LeftBankedQuarterTurn3Tiles_2,      &Default::LeftBankedQuarterTurn3Tiles_3,      // LeftBankedQuarterTurn3Tiles
        &Default::RightBankedQuarterTurn3Tiles_0,     &Default::RightBankedQuarterTurn3Tiles_1,     &Default::RightBankedQuarterTurn3Tiles_2,     &Default::RightBankedQuarterTurn3Tiles_3,     // RightBankedQuarterTurn3Tiles
        &Default::LeftQuarterTurn3TilesUp25_0,        &Default::LeftQuarterTurn3TilesUp25_1,        &Default::LeftQuarterTurn3TilesUp25_2,        &Default::LeftQuarterTurn3TilesUp25_3,        // LeftQuarterTurn3TilesUp25
        &Default::RightQuarterTurn3TilesUp25_0,       &Default::RightQuarterTurn3TilesUp25_1,       &Default::RightQuarterTurn3TilesUp25_2,       &Default::RightQuarterTurn3TilesUp25_3,       // RightQuarterTurn3TilesUp25
        &Default::LeftQuarterTurn3TilesDown25_0,      &Default::LeftQuarterTurn3TilesDown25_1,      &Default::LeftQuarterTurn3TilesDown25_2,      &Default::LeftQuarterTurn3TilesDown25_3,      // LeftQuarterTurn3TilesDown25
        &Default::RightQuarterTurn3TilesDown25_0,     &Default::RightQuarterTurn3TilesDown25_1,     &Default::RightQuarterTurn3TilesDown25_2,     &Default::RightQuarterTurn3TilesDown25_3,     // RightQuarterTurn3TilesDown25
        &LeftQuarterTurn1Tile_0,                      &LeftQuarterTurn1Tile_1,                      &LeftQuarterTurn1Tile_2,                      &LeftQuarterTurn1Tile_3,                      // LeftQuarterTurn1Tile
        &RightQuarterTurn1Tile_0,                     &RightQuarterTurn1Tile_1,                     &RightQuarterTurn1Tile_2,                     &RightQuarterTurn1Tile_3,                     // RightQuarterTurn1Tile
        &Default::LeftTwistDownToUp_0,                &Default::LeftTwistDownToUp_1,                &Default::LeftTwistDownToUp_2,                &Default::LeftTwistDownToUp_3,                // LeftTwistDownToUp
        &Default::RightTwistDownToUp_0,               &Default::RightTwistDownToUp_1,               &Default::RightTwistDownToUp_2,               &Default::RightTwistDownToUp_3,               // RightTwistDownToUp
        &Default::LeftTwistUpToDown_0,                &Default::LeftTwistUpToDown_1,                &Default::LeftTwistUpToDown_2,                &Default::LeftTwistUpToDown_3,                // LeftTwistUpToDown
        &Default::RightTwistUpToDown_0,               &Default::RightTwistUpToDown_1,               &Default::RightTwistUpToDown_2,               &Default::RightTwistUpToDown_3,               // RightTwistUpToDown
        &Default::HalfLoopUp_0,                       &Default::HalfLoopUp_1,                       &Default::HalfLoopUp_2,                       &Default::HalfLoopUp_3,                       // HalfLoopUp
        &Default::HalfLoopDown_0,                     &Default::HalfLoopDown_1,                     &Default::HalfLoopDown_2,                     &Default::HalfLoopDown_3,                     // HalfLoopDown
        &Default::LeftCorkscrewUp_0,                  &Default::LeftCorkscrewUp_1,                  &Default::LeftCorkscrewUp_2,                  &Default::LeftCorkscrewUp_3,                  // LeftCorkscrewUp
        &Default::RightCorkscrewUp_0,                 &Default::RightCorkscrewUp_1,                 &Default::RightCorkscrewUp_2,                 &Default::RightCorkscrewUp_3,                 // RightCorkscrewUp
        &Default::LeftCorkscrewDown_0,                &Default::LeftCorkscrewDown_1,                &Default::LeftCorkscrewDown_2,                &Default::LeftCorkscrewDown_3,                // LeftCorkscrewDown
        &Default::RightCorkscrewDown_0,               &Default::RightCorkscrewDown_1,               &Default::RightCorkscrewDown_2,               &Default::RightCorkscrewDown_3,               // RightCorkscrewDown
        &Default::FlatToUp60_0,                       &Default::FlatToUp60_1,                       &Default::FlatToUp60_2,                       &Default::FlatToUp60_3,                       // FlatToUp60
        &Default::Up60ToFlat_0,                       &Default::Up60ToFlat_1,                       &Default::Up60ToFlat_2,                       &Default::Up60ToFlat_3,                       // Up60ToFlat
        &Default::FlatToDown60_0,                     &Default::FlatToDown60_1,                     &Default::FlatToDown60_2,                     &Default::FlatToDown60_3,                     // FlatToDown60
        &Default::Down60ToFlat_0,                     &Default::Down60ToFlat_1,                     &Default::Down60ToFlat_2,                     &Default::Down60ToFlat_3,                     // Down60ToFlat
        &Default::TowerBase_0,                        &Default::TowerBase_1,                        &Default::TowerBase_2,                        &Default::TowerBase_3,                        // TowerBase
        &Default::TowerSection_0,                     &Default::TowerSection_1,                     &Default::TowerSection_2,                     &Default::TowerSection_3,                     // TowerSection
        &Default::Flat_0,                             &Default::Flat_1,                             &Default::Flat_2,                             &Default::Flat_3,                             // FlatCovered
        &Default::Up25_0,                             &Default::Up25_1,                             &Default::Up25_2,                             &Default::Up25_3,                             // Up25Covered
        &Default::Up60_0,                             &Default::Up60_1,                             &Default::Up60_2,                             &Default::Up60_3,                             // Up60Covered
        &Default::FlatToUp25_0,                       &Default::FlatToUp25_1,                       &Default::FlatToUp25_2,                       &Default::FlatToUp25_3,                       // FlatToUp25Covered
        &Default::Up25ToUp60_0,                       &Default::Up25ToUp60_1,                       &Default::Up25ToUp60_2,                       &Default::Up25ToUp60_3,                       // Up25ToUp60Covered
        &Default::Up60ToUp25_0,                       &Default::Up60ToUp25_1,                       &Default::Up60ToUp25_2,                       &Default::Up60ToUp25_3,                       // Up60ToUp25Covered
        &Default::Up25ToFlat_0,                       &Default::Up25ToFlat_1,                       &Default::Up25ToFlat_2,                       &Default::Up25ToFlat_3,                       // Up25ToFlatCovered
        &Default::Down25_0,                           &Default::Down25_1,                           &Default::Down25_2,                           &Default::Down25_3,                           // Down25Covered
        &Default::Down60_0,                           &Default::Down60_1,                           &Default::Down60_2,                           &Default::Down60_3,                           // Down60Covered
        &Default::FlatToDown25_0,                     &Default::FlatToDown25_1,                     &Default::FlatToDown25_2,                     &Default::FlatToDown25_3,                     // FlatToDown25Covered
        &Default::Down25ToDown60_0,                   &Default::Down25ToDown60_1,                   &Default::Down25ToDown60_2,                   &Default::Down25ToDown60_3,                   // Down25ToDown60Covered
        &Default::Down60ToDown25_0,                   &Default::Down60ToDown25_1,                   &Default::Down60ToDown25_2,                   &Default::Down60ToDown25_3,                   // Down60ToDown25Covered
        &Default::Down25ToFlat_0,                     &Default::Down25ToFlat_1,                     &Default::Down25ToFlat_2,                     &Default::Down25ToFlat_3,                     // Down25ToFlatCovered
        &Default::LeftQuarterTurn5Tiles_0,            &Default::LeftQuarterTurn5Tiles_1,            &Default::LeftQuarterTurn5Tiles_2,            &Default::LeftQuarterTurn5Tiles_3,            // LeftQuarterTurn5TilesCovered
        &Default::RightQuarterTurn5Tiles_0,           &Default::RightQuarterTurn5Tiles_1,           &Default::RightQuarterTurn5Tiles_2,           &Default::RightQuarterTurn5Tiles_3,           // RightQuarterTurn5TilesCovered
        &Default::SBendLeft_0,                        &Default::SBendLeft_1,                        &Default::SBendLeft_2,                        &Default::SBendLeft_3,                        // SBendLeftCovered
        &Default::SBendRight_0,                       &Default::SBendRight_1,                       &Default::SBendRight_2,                       &Default::SBendRight_3,                       // SBendRightCovered
        &Default::LeftQuarterTurn3Tiles_0,            &Default::LeftQuarterTurn3Tiles_1,            &Default::LeftQuarterTurn3Tiles_2,            &Default::LeftQuarterTurn3Tiles_3,            // LeftQuarterTurn3TilesCovered
        &Default::RightQuarterTurn3Tiles_0,           &Default::RightQuarterTurn3Tiles_1,           &Default::RightQuarterTurn3Tiles_2,           &Default::RightQuarterTurn3Tiles_3,           // RightQuarterTurn3TilesCovered
        &Default::LeftHalfBankedHelixUpSmall_0,       &Default::LeftHalfBankedHelixUpSmall_1,       &Default::LeftHalfBankedHelixUpSmall_2,       &Default::LeftHalfBankedHelixUpSmall_3,       // LeftHalfBankedHelixUpSmall
        &Default::RightHalfBankedHelixUpSmall_0,      &Default::RightHalfBankedHelixUpSmall_1,      &Default::RightHalfBankedHelixUpSmall_2,      &Default::RightHalfBankedHelixUpSmall_3,      // RightHalfBankedHelixUpSmall
        &Default::LeftHalfBankedHelixDownSmall_0,     &Default::LeftHalfBankedHelixDownSmall_1,     &Default::LeftHalfBankedHelixDownSmall_2,     &Default::LeftHalfBankedHelixDownSmall_3,     // LeftHalfBankedHelixDownSmall
        &Default::RightHalfBankedHelixDownSmall_0,    &Default::RightHalfBankedHelixDownSmall_1,    &Default::RightHalfBankedHelixDownSmall_2,    &Default::RightHalfBankedHelixDownSmall_3,    // RightHalfBankedHelixDownSmall
        &Default::LeftHalfBankedHelixUpLarge_0,       &Default::LeftHalfBankedHelixUpLarge_1,       &Default::LeftHalfBankedHelixUpLarge_2,       &Default::LeftHalfBankedHelixUpLarge_3,       // LeftHalfBankedHelixUpLarge
        &Default::RightHalfBankedHelixUpLarge_0,      &Default::RightHalfBankedHelixUpLarge_1,      &Default::RightHalfBankedHelixUpLarge_2,      &Default::RightHalfBankedHelixUpLarge_3,      // RightHalfBankedHelixUpLarge
        &Default::LeftHalfBankedHelixDownLarge_0,     &Default::LeftHalfBankedHelixDownLarge_1,     &Default::LeftHalfBankedHelixDownLarge_2,     &Default::LeftHalfBankedHelixDownLarge_3,     // LeftHalfBankedHelixDownLarge
        &Default::RightHalfBankedHelixDownLarge_0,    &Default::RightHalfBankedHelixDownLarge_1,    &Default::RightHalfBankedHelixDownLarge_2,    &Default::RightHalfBankedHelixDownLarge_3,    // RightHalfBankedHelixDownLarge
        &Default::LeftQuarterTurn1TileUp60_0,         &Default::LeftQuarterTurn1TileUp60_1,         &Default::LeftQuarterTurn1TileUp60_2,         &Default::LeftQuarterTurn1TileUp60_3,         // LeftQuarterTurn1TileUp60
        &Default::RightQuarterTurn1TileUp60_0,        &Default::RightQuarterTurn1TileUp60_1,        &Default::RightQuarterTurn1TileUp60_2,        &Default::RightQuarterTurn1TileUp60_3,        // RightQuarterTurn1TileUp60
        &Default::LeftQuarterTurn1TileDown60_0,       &Default::LeftQuarterTurn1TileDown60_1,       &Default::LeftQuarterTurn1TileDown60_2,       &Default::LeftQuarterTurn1TileDown60_3,       // LeftQuarterTurn1TileDown60
        &Default::RightQuarterTurn1TileDown60_0,      &Default::RightQuarterTurn1TileDown60_1,      &Default::RightQuarterTurn1TileDown60_2,      &Default::RightQuarterTurn1TileDown60_3,      // RightQuarterTurn1TileDown60
        &Flat_0,                                      &Flat_1,                                      &Flat_2,                                      &Flat_3,                                      // Brakes
        &Flat_0,                                      &Flat_1,                                      &Flat_2,                                      &Flat_3,                                      // Booster
        &TrackVehicleInfo_000000,                     &TrackVehicleInfo_000000,                     &TrackVehicleInfo_000000,                     &TrackVehicleInfo_000000,                     // Maze
        &Default::LeftQuarterBankedHelixLargeUp_0,    &Default::LeftQuarterBankedHelixLargeUp_1,    &Default::LeftQuarterBankedHelixLargeUp_2,    &Default::LeftQuarterBankedHelixLargeUp_3,    // LeftQuarterBankedHelixLargeUp
        &Default::RightQuarterBankedHelixLargeUp_0,   &Default::RightQuarterBankedHelixLargeUp_1,   &Default::RightQuarterBankedHelixLargeUp_2,   &Default::RightQuarterBankedHelixLargeUp_3,   // RightQuarterBankedHelixLargeUp
        &Default::LeftQuarterBankedHelixLargeDown_0,  &Default::LeftQuarterBankedHelixLargeDown_1,  &Default::LeftQuarterBankedHelixLargeDown_2,  &Default::LeftQuarterBankedHelixLargeDown_3,  // LeftQuarterBankedHelixLargeDown
        &Default::RightQuarterBankedHelixLargeDown_0, &Default::RightQuarterBankedHelixLargeDown_1, &Default::RightQuarterBankedHelixLargeDown_2, &Default::RightQuarterBankedHelixLargeDown_3, // RightQuarterBankedHelixLargeDown
        &Default::LeftQuarterHelixLargeUp_0,          &Default::LeftQuarterHelixLargeUp_1,          &Default::LeftQuarterHelixLargeUp_2,          &Default::LeftQuarterHelixLargeUp_3,          // LeftQuarterHelixLargeUp
        &Default::RightQuarterHelixLargeUp_0,         &Default::RightQuarterHelixLargeUp_1,         &Default::RightQuarterHelixLargeUp_2,         &Default::RightQuarterHelixLargeUp_3,         // RightQuarterHelixLargeUp
        &Default::LeftQuarterHelixLargeDown_0,        &Default::LeftQuarterHelixLargeDown_1,        &Default::LeftQuarterHelixLargeDown_2,        &Default::LeftQuarterHelixLargeDown_3,        // LeftQuarterHelixLargeDown
        &Default::RightQuarterHelixLargeDown_0,       &Default::RightQuarterHelixLargeDown_1,       &Default::RightQuarterHelixLargeDown_2,       &Default::RightQuarterHelixLargeDown_3,       // RightQuarterHelixLargeDown
        &Default::Up25LeftBanked_0,                   &Default::Up25LeftBanked_1,                   &Default::Up25LeftBanked_2,                   &Default::Up25LeftBanked_3,                   // Up25LeftBanked
        &Default::Up25RightBanked_0,                  &Default::Up25RightBanked_1,                  &Default::Up25RightBanked_2,                  &Default::Up25RightBanked_3,                  // Up25RightBanked
        &Default::Flat_0,                             &Default::Flat_1,                             &Default::Flat_2,                             &Default::Flat_3,                             // Waterfall
        &Default::Rapids_0,                           &Default::Rapids_1,                           &Default::Rapids_2,                           &Default::Rapids_3,                           // Rapids
        &Default::Flat_0,                             &Default::Flat_1,                             &Default::Flat_2,                             &Default::Flat_3,                             // OnRidePhoto
        &Default::Down25LeftBanked_0,                 &Default::Down25LeftBanked_1,                 &Default::Down25LeftBanked_2,                 &Default::Down25LeftBanked_3,                 // Down25LeftBanked
        &Default::Down25RightBanked_0,                &Default::Down25RightBanked_1,                &Default::Down25RightBanked_2,                &Default::Down25RightBanked_3,                // Down25RightBanked
        &Default::Watersplash_0,                      &Default::Watersplash_1,                      &Default::Watersplash_2,                      &Default::Watersplash_3,                      // Watersplash
        &Default::FlatToUp60LongBase_0,               &Default::FlatToUp60LongBase_1,               &Default::FlatToUp60LongBase_2,               &Default::FlatToUp60LongBase_3,               // FlatToUp60LongBase
        &Default::Up60ToFlatLongBase_0,               &Default::Up60ToFlatLongBase_1,               &Default::Up60ToFlatLongBase_2,               &Default::Up60ToFlatLongBase_3,               // Up60ToFlatLongBase
        &Default::Flat_0,                             &Default::Flat_1,                             &Default::Flat_2,                             &Default::Flat_3,                             // Whirlpool
        &Default::Down60ToFlatLongBase_0,             &Default::Down60ToFlatLongBase_1,             &Default::Down60ToFlatLongBase_2,             &Default::Down60ToFlatLongBase_3,             // Down60ToFlatLongBase
        &Default::FlatToDown60LongBase_0,             &Default::FlatToDown60LongBase_1,             &Default::FlatToDown60LongBase_2,             &Default::FlatToDown60LongBase_3,             // FlatToDown60LongBase
        &Default::CableLiftHill_0,                    &Default::CableLiftHill_1,                    &Default::CableLiftHill_2,                    &Default::CableLiftHill_3,                    // CableLiftHill
        &Default::ReverseFreefallSlope_0,             &Default::ReverseFreefallSlope_1,             &Default::ReverseFreefallSlope_2,             &Default::ReverseFreefallSlope_3,             // ReverseFreefallSlope
        &Default::ReverseFreefallVertical_0,          &Default::ReverseFreefallVertical_1,          &Default::ReverseFreefallVertical_2,          &Default::ReverseFreefallVertical_3,          // ReverseFreefallVertical
        &Default::Up90_0,                             &Default::Up90_1,                             &Default::Up90_2,                             &Default::Up90_3,                             // Up90
        &Default::Down90_0,                           &Default::Down90_1,                           &Default::Down90_2,                           &Default::Down90_3,                           // Down90
        &Default::Up60ToUp90_0,                       &Default::Up60ToUp90_1,                       &Default::Up60ToUp90_2,                       &Default::Up60ToUp90_3,                       // Up60ToUp90
        &Default::Down90ToDown60_0,                   &Default::Down90ToDown60_1,                   &Default::Down90ToDown60_2,                   &Default::Down90ToDown60_3,                   // Down90ToDown60
        &Default::Up90ToUp60_0,                       &Default::Up90ToUp60_1,                       &Default::Up90ToUp60_2,                       &Default::Up90ToUp60_3,                       // Up90ToUp60
        &Default::Down60ToDown90_0,                   &Default::Down60ToDown90_1,                   &Default::Down60ToDown90_2,                   &Default::Down60ToDown90_3,                   // Down60ToDown90
        &Default::FlatToDown60_0,                     &Default::FlatToDown60_1,                     &Default::FlatToDown60_2,                     &Default::FlatToDown60_3,                     // BrakeForDrop
        &Default::LeftEighthToDiag_0,                 &Default::LeftEighthToDiag_1,                 &Default::LeftEighthToDiag_2,                 &Default::LeftEighthToDiag_3,                 // LeftEighthToDiag
        &Default::RightEighthToDiag_0,                &Default::RightEighthToDiag_1,                &Default::RightEighthToDiag_2,                &Default::RightEighthToDiag_3,                // RightEighthToDiag
        &Default::LeftEighthToOrthogonal_0,           &Default::LeftEighthToOrthogonal_1,           &Default::LeftEighthToOrthogonal_2,           &Default::LeftEighthToOrthogonal_3,           // LeftEighthToOrthogonal
        &Default::RightEighthToOrthogonal_0,          &Default::RightEighthToOrthogonal_1,          &Default::RightEighthToOrthogonal_2,          &Default::RightEighthToOrthogonal_3,          // RightEighthToOrthogonal
        &Default::LeftEighthBankToDiag_0,             &Default::LeftEighthBankToDiag_1,             &Default::LeftEighthBankToDiag_2,             &Default::LeftEighthBankToDiag_3,             // LeftEighthBankToDiag
        &Default::RightEighthBankToDiag_0,            &Default::RightEighthBankToDiag_1,            &Default::RightEighthBankToDiag_2,            &Default::RightEighthBankToDiag_3,            // RightEighthBankToDiag
        &Default::LeftEighthBankToOrthogonal_0,       &Default::LeftEighthBankToOrthogonal_1,       &Default::LeftEighthBankToOrthogonal_2,       &Default::LeftEighthBankToOrthogonal_3,       // LeftEighthBankToOrthogonal
        &Default::RightEighthBankToOrthogonal_0,      &Default::RightEighthBankToOrthogonal_1,      &Default::RightEighthBankToOrthogonal_2,      &Default::RightEighthBankToOrthogonal_3,      // RightEighthBankToOrthogonal
        &Default::DiagFlat_0,                         &Default::DiagFlat_1,                         &Default::DiagFlat_2,                         &Default::DiagFlat_3,                         // DiagFlat
        &Default::DiagUp25_0,                         &Default::DiagUp25_1,                         &Default::DiagUp25_2,                         &Default::DiagUp25_3,                         // DiagUp25
        &Default::DiagUp60_0,                         &Default::DiagUp60_1,                         &Default::DiagUp60_2,                         &Default::DiagUp60_3,                         // DiagUp60
        &Default::DiagFlatToUp25_0,                   &Default::DiagFlatToUp25_1,                   &Default::DiagFlatToUp25_2,                   &Default::DiagFlatToUp25_3,                   // DiagFlatToUp25
        &Default::DiagUp25ToUp60_0,                   &Default::DiagUp25ToUp60_1,                   &Default::DiagUp25ToUp60_2,                   &Default::DiagUp25ToUp60_3,                   // DiagUp25ToUp60
        &Default::DiagUp60ToUp25_0,                   &Default::DiagUp60ToUp25_1,                   &Default::DiagUp60ToUp25_2,                   &Default::DiagUp60ToUp25_3,                   // DiagUp60ToUp25
        &Default::DiagUp25ToFlat_0,                   &Default::DiagUp25ToFlat_1,                   &Default::DiagUp25ToFlat_2,                   &Default::DiagUp25ToFlat_3,                   // DiagUp25ToFlat
        &Default::DiagDown25_0,                       &Default::DiagDown25_1,                       &Default::DiagDown25_2,                       &Default::DiagDown25_3,                       // DiagDown25
        &Default::DiagDown60_0,                       &Default::DiagDown60_1,                       &Default::DiagDown60_2,                       &Default::DiagDown60_3,                       // DiagDown60
        &Default::DiagFlatToDown25_0,                 &Default::DiagFlatToDown25_1,                 &Default::DiagFlatToDown25_2,                 &Default::DiagFlatToDown25_3,                 // DiagFlatToDown25
        &Default::DiagDown25ToDown60_0,               &Default::DiagDown25ToDown60_1,               &Default::DiagDown25ToDown60_2,               &Default::DiagDown25ToDown60_3,               // DiagDown25ToDown60
        &Default::DiagDown60ToDown25_0,               &Default::DiagDown60ToDown25_1,               &Default::DiagDown60ToDown25_2,               &Default::DiagDown60ToDown25_3,               // DiagDown60ToDown25
        &Default::DiagDown25ToFlat_0,                 &Default::DiagDown25ToFlat_1,                 &Default::DiagDown25ToFlat_2,                 &Default::DiagDown25ToFlat_3,                 // DiagDown25ToFlat
        &Default::DiagFlatToUp60_0,                   &Default::DiagFlatToUp60_1,                   &Default::DiagFlatToUp60_2,                   &Default::DiagFlatToUp60_3,                   // DiagFlatToUp60
        &Default::DiagUp60ToFlat_0,                   &Default::DiagUp60ToFlat_1,                   &Default::DiagUp60ToFlat_2,                   &Default::DiagUp60ToFlat_3,                   // DiagUp60ToFlat
        &Default::DiagFlatToDown60_0,                 &Default::DiagFlatToDown60_1,                 &Default::DiagFlatToDown60_2,                 &Default::DiagFlatToDown60_3,                 // DiagFlatToDown60
        &Default::DiagDown60ToFlat_0,                 &Default::DiagDown60ToFlat_1,                 &Default::DiagDown60ToFlat_2,                 &Default::DiagDown60ToFlat_3,                 // DiagDown60ToFlat
        &Default::DiagFlatToLeftBank_0,               &Default::DiagFlatToLeftBank_1,               &Default::DiagFlatToLeftBank_2,               &Default::DiagFlatToLeftBank_3,               // DiagFlatToLeftBank
        &Default::DiagFlatToRightBank_0,              &Default::DiagFlatToRightBank_1,              &Default::DiagFlatToRightBank_2,              &Default::DiagFlatToRightBank_3,              // DiagFlatToRightBank
        &Default::DiagLeftBankToFlat_0,               &Default::DiagLeftBankToFlat_1,               &Default::DiagLeftBankToFlat_2,               &Default::DiagLeftBankToFlat_3,               // DiagLeftBankToFlat
        &Default::DiagRightBankToFlat_0,              &Default::DiagRightBankToFlat_1,              &Default::DiagRightBankToFlat_2,              &Default::DiagRightBankToFlat_3,              // DiagRightBankToFlat
        &Default::DiagLeftBankToUp25_0,               &Default::DiagLeftBankToUp25_1,               &Default::DiagLeftBankToUp25_2,               &Default::DiagLeftBankToUp25_3,               // DiagLeftBankToUp25
        &Default::DiagRightBankToUp25_0,              &Default::DiagRightBankToUp25_1,              &Default::DiagRightBankToUp25_2,              &Default::DiagRightBankToUp25_3,              // DiagRightBankToUp25
        &Default::DiagUp25ToLeftBank_0,               &Default::DiagUp25ToLeftBank_1,               &Default::DiagUp25ToLeftBank_2,               &Default::DiagUp25ToLeftBank_3,               // DiagUp25ToLeftBank
        &Default::DiagUp25ToRightBank_0,              &Default::DiagUp25ToRightBank_1,              &Default::DiagUp25ToRightBank_2,              &Default::DiagUp25ToRightBank_3,              // DiagUp25ToRightBank
        &Default::DiagLeftBankToDown25_0,             &Default::DiagLeftBankToDown25_1,             &Default::DiagLeftBankToDown25_2,             &Default::DiagLeftBankToDown25_3,             // DiagLeftBankToDown25
        &Default::DiagRightBankToDown25_0,            &Default::DiagRightBankToDown25_1,            &Default::DiagRightBankToDown25_2,            &Default::DiagRightBankToDown25_3,            // DiagRightBankToDown25
        &Default::DiagDown25ToLeftBank_0,             &Default::DiagDown25ToLeftBank_1,             &Default::DiagDown25ToLeftBank_2,             &Default::DiagDown25ToLeftBank_3,             // DiagDown25ToLeftBank
        &Default::DiagDown25ToRightBank_0,            &Default::DiagDown25ToRightBank_1,            &Default::DiagDown25ToRightBank_2,            &Default::DiagDown25ToRightBank_3,            // DiagDown25ToRightBank
        &Default::DiagLeftBank_0,                     &Default::DiagLeftBank_1,                     &Default::DiagLeftBank_2,                     &Default::DiagLeftBank_3,                     // DiagLeftBank
        &Default::DiagRightBank_0,                    &Default::DiagRightBank_1,                    &Default::DiagRightBank_2,                    &Default::DiagRightBank_3,                    // DiagRightBank
        &Default::LogFlumeReverser_0,                 &Default::LogFlumeReverser_1,                 &Default::LogFlumeReverser_2,                 &Default::LogFlumeReverser_3,                 // LogFlumeReverser
    };
    //clang-format on
    static_assert(std::size(TrackVehicleInfoListChairliftGoingOut) == kVehicleTrackSubpositionSizeChairliftGoingOut);
} // namespace OpenRCT2::Subposition::ChairliftGoingOut
