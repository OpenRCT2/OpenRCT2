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

namespace OpenRCT2::Subposition::GoKartsMovingToLeftLane
{
    // clang-format off
    CREATE_VEHICLE_INFO(Flat_0, {
        { 31, 23, 0,  0, flat, unbanked }, { 30, 23, 0,  0, flat, unbanked }, { 29, 23, 0,  0, flat, unbanked }, { 28, 23, 0,  0, flat, unbanked }, { 27, 22, 0, 30, flat, unbanked },
        { 26, 22, 0, 30, flat, unbanked }, { 25, 22, 0, 30, flat, unbanked }, { 24, 21, 0, 30, flat, unbanked }, { 23, 21, 0, 30, flat, unbanked }, { 22, 20, 0, 30, flat, unbanked },
        { 21, 20, 0, 30, flat, unbanked }, { 20, 19, 0, 30, flat, unbanked }, { 19, 18, 0, 30, flat, unbanked }, { 18, 18, 0, 30, flat, unbanked }, { 17, 17, 0, 30, flat, unbanked },
        { 16, 16, 0, 30, flat, unbanked }, { 15, 16, 0, 30, flat, unbanked }, { 14, 15, 0, 30, flat, unbanked }, { 13, 14, 0, 30, flat, unbanked }, { 12, 14, 0, 30, flat, unbanked },
        { 11, 13, 0, 30, flat, unbanked }, { 10, 12, 0, 30, flat, unbanked }, {  9, 12, 0, 30, flat, unbanked }, {  8, 11, 0, 30, flat, unbanked }, {  7, 11, 0, 30, flat, unbanked },
        {  6, 10, 0, 30, flat, unbanked }, {  5, 10, 0, 30, flat, unbanked }, {  4, 10, 0, 30, flat, unbanked }, {  3,  9, 0,  0, flat, unbanked }, {  2,  9, 0,  0, flat, unbanked },
        {  1,  9, 0,  0, flat, unbanked }, {  0,  9, 0,  0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Flat_1, {
        { 23,  0, 0, 8, flat, unbanked }, { 23,  1, 0, 8, flat, unbanked }, { 23,  2, 0, 8, flat, unbanked }, { 23,  3, 0, 8, flat, unbanked }, { 22,  4, 0, 6, flat, unbanked }, { 22,  5, 0, 6, flat, unbanked },
        { 22,  6, 0, 6, flat, unbanked }, { 21,  7, 0, 6, flat, unbanked }, { 21,  8, 0, 6, flat, unbanked }, { 20,  9, 0, 6, flat, unbanked }, { 20, 10, 0, 6, flat, unbanked }, { 19, 11, 0, 6, flat, unbanked },
        { 18, 12, 0, 6, flat, unbanked }, { 18, 13, 0, 6, flat, unbanked }, { 17, 14, 0, 6, flat, unbanked }, { 16, 15, 0, 6, flat, unbanked }, { 16, 16, 0, 6, flat, unbanked }, { 15, 17, 0, 6, flat, unbanked },
        { 14, 18, 0, 6, flat, unbanked }, { 14, 19, 0, 6, flat, unbanked }, { 13, 20, 0, 6, flat, unbanked }, { 12, 21, 0, 6, flat, unbanked }, { 12, 22, 0, 6, flat, unbanked }, { 11, 23, 0, 6, flat, unbanked },
        { 11, 24, 0, 6, flat, unbanked }, { 10, 25, 0, 6, flat, unbanked }, { 10, 26, 0, 6, flat, unbanked }, { 10, 27, 0, 6, flat, unbanked }, {  9, 28, 0, 8, flat, unbanked }, {  9, 29, 0, 8, flat, unbanked },
        {  9, 30, 0, 8, flat, unbanked }, {  9, 31, 0, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Flat_2, {
        {  0,  9, 0, 16, flat, unbanked }, {  1,  9, 0, 16, flat, unbanked }, {  2,  9, 0, 16, flat, unbanked }, {  3,  9, 0, 16, flat, unbanked }, {  4, 10, 0, 14, flat, unbanked },
        {  5, 10, 0, 14, flat, unbanked }, {  6, 10, 0, 14, flat, unbanked }, {  7, 11, 0, 14, flat, unbanked }, {  8, 11, 0, 14, flat, unbanked }, {  9, 12, 0, 14, flat, unbanked },
        { 10, 12, 0, 14, flat, unbanked }, { 11, 13, 0, 14, flat, unbanked }, { 12, 14, 0, 14, flat, unbanked }, { 13, 14, 0, 14, flat, unbanked }, { 14, 15, 0, 14, flat, unbanked },
        { 15, 16, 0, 14, flat, unbanked }, { 16, 16, 0, 14, flat, unbanked }, { 17, 17, 0, 14, flat, unbanked }, { 18, 18, 0, 14, flat, unbanked }, { 19, 18, 0, 14, flat, unbanked },
        { 20, 19, 0, 14, flat, unbanked }, { 21, 20, 0, 14, flat, unbanked }, { 22, 20, 0, 14, flat, unbanked }, { 23, 21, 0, 14, flat, unbanked }, { 24, 21, 0, 14, flat, unbanked },
        { 25, 22, 0, 14, flat, unbanked }, { 26, 22, 0, 14, flat, unbanked }, { 27, 22, 0, 14, flat, unbanked }, { 28, 23, 0, 16, flat, unbanked }, { 29, 23, 0, 16, flat, unbanked },
        { 30, 23, 0, 16, flat, unbanked }, { 31, 23, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Flat_3, {
        {  9, 31, 0, 24, flat, unbanked }, {  9, 30, 0, 24, flat, unbanked }, {  9, 29, 0, 24, flat, unbanked }, {  9, 28, 0, 24, flat, unbanked }, { 10, 27, 0, 22, flat, unbanked },
        { 10, 26, 0, 22, flat, unbanked }, { 10, 25, 0, 22, flat, unbanked }, { 11, 24, 0, 22, flat, unbanked }, { 11, 23, 0, 22, flat, unbanked }, { 12, 22, 0, 22, flat, unbanked },
        { 12, 21, 0, 22, flat, unbanked }, { 13, 20, 0, 22, flat, unbanked }, { 14, 19, 0, 22, flat, unbanked }, { 14, 18, 0, 22, flat, unbanked }, { 15, 17, 0, 22, flat, unbanked },
        { 16, 16, 0, 22, flat, unbanked }, { 16, 15, 0, 22, flat, unbanked }, { 17, 14, 0, 22, flat, unbanked }, { 18, 13, 0, 22, flat, unbanked }, { 18, 12, 0, 22, flat, unbanked },
        { 19, 11, 0, 22, flat, unbanked }, { 20, 10, 0, 22, flat, unbanked }, { 20,  9, 0, 22, flat, unbanked }, { 21,  8, 0, 22, flat, unbanked }, { 21,  7, 0, 22, flat, unbanked },
        { 22,  6, 0, 22, flat, unbanked }, { 22,  5, 0, 22, flat, unbanked }, { 22,  4, 0, 22, flat, unbanked }, { 23,  3, 0, 24, flat, unbanked }, { 23,  2, 0, 24, flat, unbanked },
        { 23,  1, 0, 24, flat, unbanked }, { 23,  0, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25_0, {
        { 31, 23,  0, 0, up25, unbanked }, { 30, 23,  0, 0, up25, unbanked }, { 29, 23,  1, 0, up25, unbanked }, { 28, 23,  1, 0, up25, unbanked }, { 27, 23,  2, 0, up25, unbanked },
        { 26, 23,  2, 0, up25, unbanked }, { 25, 23,  3, 0, up25, unbanked }, { 24, 23,  3, 0, up25, unbanked }, { 23, 23,  4, 0, up25, unbanked }, { 22, 23,  4, 0, up25, unbanked },
        { 21, 23,  5, 0, up25, unbanked }, { 20, 23,  5, 0, up25, unbanked }, { 19, 23,  6, 0, up25, unbanked }, { 18, 23,  6, 0, up25, unbanked }, { 17, 23,  7, 0, up25, unbanked },
        { 16, 23,  7, 0, up25, unbanked }, { 15, 23,  8, 0, up25, unbanked }, { 14, 23,  8, 0, up25, unbanked }, { 13, 23,  9, 0, up25, unbanked }, { 12, 23,  9, 0, up25, unbanked },
        { 11, 23, 10, 0, up25, unbanked }, { 10, 23, 10, 0, up25, unbanked }, {  9, 23, 11, 0, up25, unbanked }, {  8, 23, 11, 0, up25, unbanked }, {  7, 23, 12, 0, up25, unbanked },
        {  6, 23, 12, 0, up25, unbanked }, {  5, 23, 13, 0, up25, unbanked }, {  4, 23, 13, 0, up25, unbanked }, {  3, 23, 14, 0, up25, unbanked }, {  2, 23, 14, 0, up25, unbanked },
        {  1, 23, 15, 0, up25, unbanked }, {  0, 23, 15, 0, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25_1, {
        { 23,  0,  0, 8, up25, unbanked }, { 23,  1,  1, 8, up25, unbanked }, { 23,  2,  1, 8, up25, unbanked }, { 23,  3,  2, 8, up25, unbanked }, { 23,  4,  2, 8, up25, unbanked },
        { 23,  5,  3, 8, up25, unbanked }, { 23,  6,  3, 8, up25, unbanked }, { 23,  7,  4, 8, up25, unbanked }, { 23,  8,  4, 8, up25, unbanked }, { 23,  9,  5, 8, up25, unbanked },
        { 23, 10,  5, 8, up25, unbanked }, { 23, 11,  6, 8, up25, unbanked }, { 23, 12,  6, 8, up25, unbanked }, { 23, 13,  7, 8, up25, unbanked }, { 23, 14,  7, 8, up25, unbanked },
        { 23, 15,  8, 8, up25, unbanked }, { 23, 16,  8, 8, up25, unbanked }, { 23, 17,  9, 8, up25, unbanked }, { 23, 18,  9, 8, up25, unbanked }, { 23, 19, 10, 8, up25, unbanked },
        { 23, 20, 10, 8, up25, unbanked }, { 23, 21, 11, 8, up25, unbanked }, { 23, 22, 11, 8, up25, unbanked }, { 23, 23, 12, 8, up25, unbanked }, { 23, 24, 12, 8, up25, unbanked },
        { 23, 25, 13, 8, up25, unbanked }, { 23, 26, 13, 8, up25, unbanked }, { 23, 27, 14, 8, up25, unbanked }, { 23, 28, 14, 8, up25, unbanked }, { 23, 29, 15, 8, up25, unbanked },
        { 23, 30, 15, 8, up25, unbanked }, { 23, 31, 16, 8, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25_2, {
        {  0, 9,  0, 16, up25, unbanked }, {  1, 9,  1, 16, up25, unbanked }, {  2, 9,  1, 16, up25, unbanked }, {  3, 9,  2, 16, up25, unbanked }, {  4, 9,  2, 16, up25, unbanked },
        {  5, 9,  3, 16, up25, unbanked }, {  6, 9,  3, 16, up25, unbanked }, {  7, 9,  4, 16, up25, unbanked }, {  8, 9,  4, 16, up25, unbanked }, {  9, 9,  5, 16, up25, unbanked },
        { 10, 9,  5, 16, up25, unbanked }, { 11, 9,  6, 16, up25, unbanked }, { 12, 9,  6, 16, up25, unbanked }, { 13, 9,  7, 16, up25, unbanked }, { 14, 9,  7, 16, up25, unbanked },
        { 15, 9,  8, 16, up25, unbanked }, { 16, 9,  8, 16, up25, unbanked }, { 17, 9,  9, 16, up25, unbanked }, { 18, 9,  9, 16, up25, unbanked }, { 19, 9, 10, 16, up25, unbanked },
        { 20, 9, 10, 16, up25, unbanked }, { 21, 9, 11, 16, up25, unbanked }, { 22, 9, 11, 16, up25, unbanked }, { 23, 9, 12, 16, up25, unbanked }, { 24, 9, 12, 16, up25, unbanked },
        { 25, 9, 13, 16, up25, unbanked }, { 26, 9, 13, 16, up25, unbanked }, { 27, 9, 14, 16, up25, unbanked }, { 28, 9, 14, 16, up25, unbanked }, { 29, 9, 15, 16, up25, unbanked },
        { 30, 9, 15, 16, up25, unbanked }, { 31, 9, 16, 16, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25_3, {
        { 9, 31,  0, 24, up25, unbanked }, { 9, 30,  0, 24, up25, unbanked }, { 9, 29,  1, 24, up25, unbanked }, { 9, 28,  1, 24, up25, unbanked }, { 9, 27,  2, 24, up25, unbanked },
        { 9, 26,  2, 24, up25, unbanked }, { 9, 25,  3, 24, up25, unbanked }, { 9, 24,  3, 24, up25, unbanked }, { 9, 23,  4, 24, up25, unbanked }, { 9, 22,  4, 24, up25, unbanked },
        { 9, 21,  5, 24, up25, unbanked }, { 9, 20,  5, 24, up25, unbanked }, { 9, 19,  6, 24, up25, unbanked }, { 9, 18,  6, 24, up25, unbanked }, { 9, 17,  7, 24, up25, unbanked },
        { 9, 16,  7, 24, up25, unbanked }, { 9, 15,  8, 24, up25, unbanked }, { 9, 14,  8, 24, up25, unbanked }, { 9, 13,  9, 24, up25, unbanked }, { 9, 12,  9, 24, up25, unbanked },
        { 9, 11, 10, 24, up25, unbanked }, { 9, 10, 10, 24, up25, unbanked }, { 9,  9, 11, 24, up25, unbanked }, { 9,  8, 11, 24, up25, unbanked }, { 9,  7, 12, 24, up25, unbanked },
        { 9,  6, 12, 24, up25, unbanked }, { 9,  5, 13, 24, up25, unbanked }, { 9,  4, 13, 24, up25, unbanked }, { 9,  3, 14, 24, up25, unbanked }, { 9,  2, 14, 24, up25, unbanked },
        { 9,  1, 15, 24, up25, unbanked }, { 9,  0, 15, 24, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToUp25_0, {
        { 31, 23, 0, 0, flat, unbanked }, { 30, 23, 0, 0, flat, unbanked }, { 29, 23, 0, 0, flat, unbanked }, { 28, 23, 0, 0, flat, unbanked }, { 27, 23, 0, 0, flat, unbanked }, { 26, 23, 0, 0, flat, unbanked },
        { 25, 23, 0, 0, flat, unbanked }, { 24, 23, 0, 0, flat, unbanked }, { 23, 23, 0, 0, up12, unbanked }, { 22, 23, 0, 0, up12, unbanked }, { 21, 23, 1, 0, up12, unbanked }, { 20, 23, 1, 0, up12, unbanked },
        { 19, 23, 1, 0, up12, unbanked }, { 18, 23, 1, 0, up12, unbanked }, { 17, 23, 1, 0, up12, unbanked }, { 16, 23, 1, 0, up12, unbanked }, { 15, 23, 2, 0, up12, unbanked }, { 14, 23, 2, 0, up12, unbanked },
        { 13, 23, 2, 0, up12, unbanked }, { 12, 23, 2, 0, up12, unbanked }, { 11, 23, 3, 0, up12, unbanked }, { 10, 23, 3, 0, up12, unbanked }, {  9, 23, 3, 0, up12, unbanked }, {  8, 23, 4, 0, up12, unbanked },
        {  7, 23, 4, 0, up25, unbanked }, {  6, 23, 5, 0, up25, unbanked }, {  5, 23, 5, 0, up25, unbanked }, {  4, 23, 6, 0, up25, unbanked }, {  3, 23, 6, 0, up25, unbanked }, {  2, 23, 7, 0, up25, unbanked },
        {  1, 23, 7, 0, up25, unbanked }, {  0, 23, 8, 0, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToUp25_1, {
        { 23,  0, 0, 8, flat, unbanked }, { 23,  1, 0, 8, flat, unbanked }, { 23,  2, 0, 8, flat, unbanked }, { 23,  3, 0, 8, flat, unbanked }, { 23,  4, 0, 8, flat, unbanked }, { 23,  5, 0, 8, flat, unbanked },
        { 23,  6, 0, 8, flat, unbanked }, { 23,  7, 0, 8, flat, unbanked }, { 23,  8, 0, 8, up12, unbanked }, { 23,  9, 0, 8, up12, unbanked }, { 23, 10, 1, 8, up12, unbanked }, { 23, 11, 1, 8, up12, unbanked },
        { 23, 12, 1, 8, up12, unbanked }, { 23, 13, 1, 8, up12, unbanked }, { 23, 14, 1, 8, up12, unbanked }, { 23, 15, 1, 8, up12, unbanked }, { 23, 16, 2, 8, up12, unbanked }, { 23, 17, 2, 8, up12, unbanked },
        { 23, 18, 2, 8, up12, unbanked }, { 23, 19, 2, 8, up12, unbanked }, { 23, 20, 3, 8, up12, unbanked }, { 23, 21, 3, 8, up12, unbanked }, { 23, 22, 3, 8, up12, unbanked }, { 23, 23, 4, 8, up12, unbanked },
        { 23, 24, 4, 8, up25, unbanked }, { 23, 25, 4, 8, up25, unbanked }, { 23, 26, 5, 8, up25, unbanked }, { 23, 27, 5, 8, up25, unbanked }, { 23, 28, 6, 8, up25, unbanked }, { 23, 29, 6, 8, up25, unbanked },
        { 23, 30, 7, 8, up25, unbanked }, { 23, 31, 7, 8, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToUp25_2, {
        {  0, 9, 0, 16, flat, unbanked }, {  1, 9, 0, 16, flat, unbanked }, {  2, 9, 0, 16, flat, unbanked }, {  3, 9, 0, 16, flat, unbanked }, {  4, 9, 0, 16, flat, unbanked }, {  5, 9, 0, 16, flat, unbanked },
        {  6, 9, 0, 16, flat, unbanked }, {  7, 9, 0, 16, flat, unbanked }, {  8, 9, 0, 16, up12, unbanked }, {  9, 9, 0, 16, up12, unbanked }, { 10, 9, 1, 16, up12, unbanked }, { 11, 9, 1, 16, up12, unbanked },
        { 12, 9, 1, 16, up12, unbanked }, { 13, 9, 1, 16, up12, unbanked }, { 14, 9, 1, 16, up12, unbanked }, { 15, 9, 1, 16, up12, unbanked }, { 16, 9, 2, 16, up12, unbanked }, { 17, 9, 2, 16, up12, unbanked },
        { 18, 9, 2, 16, up12, unbanked }, { 19, 9, 2, 16, up12, unbanked }, { 20, 9, 3, 16, up12, unbanked }, { 21, 9, 3, 16, up12, unbanked }, { 22, 9, 3, 16, up12, unbanked }, { 23, 9, 4, 16, up12, unbanked },
        { 24, 9, 4, 16, up25, unbanked }, { 25, 9, 4, 16, up25, unbanked }, { 26, 9, 5, 16, up25, unbanked }, { 27, 9, 5, 16, up25, unbanked }, { 28, 9, 6, 16, up25, unbanked }, { 29, 9, 6, 16, up25, unbanked },
        { 30, 9, 7, 16, up25, unbanked }, { 31, 9, 7, 16, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToUp25_3, {
        { 9, 31, 0, 24, flat, unbanked }, { 9, 30, 0, 24, flat, unbanked }, { 9, 29, 0, 24, flat, unbanked }, { 9, 28, 0, 24, flat, unbanked }, { 9, 27, 0, 24, flat, unbanked }, { 9, 26, 0, 24, flat, unbanked },
        { 9, 25, 0, 24, flat, unbanked }, { 9, 24, 0, 24, flat, unbanked }, { 9, 23, 0, 24, up12, unbanked }, { 9, 22, 0, 24, up12, unbanked }, { 9, 21, 1, 24, up12, unbanked }, { 9, 20, 1, 24, up12, unbanked },
        { 9, 19, 1, 24, up12, unbanked }, { 9, 18, 1, 24, up12, unbanked }, { 9, 17, 1, 24, up12, unbanked }, { 9, 16, 1, 24, up12, unbanked }, { 9, 15, 2, 24, up12, unbanked }, { 9, 14, 2, 24, up12, unbanked },
        { 9, 13, 2, 24, up12, unbanked }, { 9, 12, 2, 24, up12, unbanked }, { 9, 11, 3, 24, up12, unbanked }, { 9, 10, 3, 24, up12, unbanked }, { 9,  9, 3, 24, up12, unbanked }, { 9,  8, 4, 24, up12, unbanked },
        { 9,  7, 4, 24, up25, unbanked }, { 9,  6, 5, 24, up25, unbanked }, { 9,  5, 5, 24, up25, unbanked }, { 9,  4, 6, 24, up25, unbanked }, { 9,  3, 6, 24, up25, unbanked }, { 9,  2, 7, 24, up25, unbanked },
        { 9,  1, 7, 24, up25, unbanked }, { 9,  0, 8, 24, up25, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25ToFlat_0, {
        { 31, 23, 0, 0, up25, unbanked }, { 30, 23, 1, 0, up25, unbanked }, { 29, 23, 1, 0, up25, unbanked }, { 28, 23, 2, 0, up25, unbanked }, { 27, 23, 2, 0, up25, unbanked }, { 26, 23, 3, 0, up25, unbanked },
        { 25, 23, 3, 0, up25, unbanked }, { 24, 23, 3, 0, up25, unbanked }, { 23, 23, 4, 0, up12, unbanked }, { 22, 23, 4, 0, up12, unbanked }, { 21, 23, 4, 0, up12, unbanked }, { 20, 23, 5, 0, up12, unbanked },
        { 19, 23, 5, 0, up12, unbanked }, { 18, 23, 5, 0, up12, unbanked }, { 17, 23, 6, 0, up12, unbanked }, { 16, 23, 6, 0, up12, unbanked }, { 15, 23, 6, 0, up12, unbanked }, { 14, 23, 6, 0, up12, unbanked },
        { 13, 23, 7, 0, up12, unbanked }, { 12, 23, 7, 0, up12, unbanked }, { 11, 23, 7, 0, up12, unbanked }, { 10, 23, 7, 0, up12, unbanked }, {  9, 23, 7, 0, flat, unbanked }, {  8, 23, 7, 0, flat, unbanked },
        {  7, 23, 8, 0, flat, unbanked }, {  6, 23, 8, 0, flat, unbanked }, {  5, 23, 8, 0, flat, unbanked }, {  4, 23, 8, 0, flat, unbanked }, {  3, 23, 8, 0, flat, unbanked }, {  2, 23, 8, 0, flat, unbanked },
        {  1, 23, 8, 0, flat, unbanked }, {  0, 23, 8, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25ToFlat_1, {
        { 23,  0, 0, 8, up25, unbanked }, { 23,  1, 0, 8, up25, unbanked }, { 23,  2, 1, 8, up25, unbanked }, { 23,  3, 1, 8, up25, unbanked }, { 23,  4, 2, 8, up25, unbanked }, { 23,  5, 2, 8, up25, unbanked },
        { 23,  6, 3, 8, up25, unbanked }, { 23,  7, 3, 8, up25, unbanked }, { 23,  8, 4, 8, up12, unbanked }, { 23,  9, 4, 8, up12, unbanked }, { 23, 10, 4, 8, up12, unbanked }, { 23, 11, 5, 8, up12, unbanked },
        { 23, 12, 5, 8, up12, unbanked }, { 23, 13, 5, 8, up12, unbanked }, { 23, 14, 6, 8, up12, unbanked }, { 23, 15, 6, 8, up12, unbanked }, { 23, 16, 6, 8, up12, unbanked }, { 23, 17, 6, 8, up12, unbanked },
        { 23, 18, 7, 8, up12, unbanked }, { 23, 19, 7, 8, up12, unbanked }, { 23, 20, 7, 8, up12, unbanked }, { 23, 21, 7, 8, up12, unbanked }, { 23, 22, 7, 8, flat, unbanked }, { 23, 23, 7, 8, flat, unbanked },
        { 23, 24, 8, 8, flat, unbanked }, { 23, 25, 8, 8, flat, unbanked }, { 23, 26, 8, 8, flat, unbanked }, { 23, 27, 8, 8, flat, unbanked }, { 23, 28, 8, 8, flat, unbanked }, { 23, 29, 8, 8, flat, unbanked },
        { 23, 30, 8, 8, flat, unbanked }, { 23, 31, 8, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25ToFlat_2, {
        {  0, 9, 0, 16, up25, unbanked }, {  1, 9, 0, 16, up25, unbanked }, {  2, 9, 1, 16, up25, unbanked }, {  3, 9, 1, 16, up25, unbanked }, {  4, 9, 2, 16, up25, unbanked }, {  5, 9, 2, 16, up25, unbanked },
        {  6, 9, 3, 16, up25, unbanked }, {  7, 9, 3, 16, up25, unbanked }, {  8, 9, 4, 16, up12, unbanked }, {  9, 9, 4, 16, up12, unbanked }, { 10, 9, 4, 16, up12, unbanked }, { 11, 9, 5, 16, up12, unbanked },
        { 12, 9, 5, 16, up12, unbanked }, { 13, 9, 5, 16, up12, unbanked }, { 14, 9, 6, 16, up12, unbanked }, { 15, 9, 6, 16, up12, unbanked }, { 16, 9, 6, 16, up12, unbanked }, { 17, 9, 6, 16, up12, unbanked },
        { 18, 9, 7, 16, up12, unbanked }, { 19, 9, 7, 16, up12, unbanked }, { 20, 9, 7, 16, up12, unbanked }, { 21, 9, 7, 16, up12, unbanked }, { 22, 9, 7, 16, flat, unbanked }, { 23, 9, 7, 16, flat, unbanked },
        { 24, 9, 8, 16, flat, unbanked }, { 25, 9, 8, 16, flat, unbanked }, { 26, 9, 8, 16, flat, unbanked }, { 27, 9, 8, 16, flat, unbanked }, { 28, 9, 8, 16, flat, unbanked }, { 29, 9, 8, 16, flat, unbanked },
        { 30, 9, 8, 16, flat, unbanked }, { 31, 9, 8, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Up25ToFlat_3, {
        { 9, 31, 0, 24, up25, unbanked }, { 9, 30, 1, 24, up25, unbanked }, { 9, 29, 1, 24, up25, unbanked }, { 9, 28, 2, 24, up25, unbanked }, { 9, 27, 2, 24, up25, unbanked }, { 9, 26, 3, 24, up25, unbanked },
        { 9, 25, 3, 24, up25, unbanked }, { 9, 24, 3, 24, up25, unbanked }, { 9, 23, 4, 24, up12, unbanked }, { 9, 22, 4, 24, up12, unbanked }, { 9, 21, 4, 24, up12, unbanked }, { 9, 20, 5, 24, up12, unbanked },
        { 9, 19, 5, 24, up12, unbanked }, { 9, 18, 5, 24, up12, unbanked }, { 9, 17, 6, 24, up12, unbanked }, { 9, 16, 6, 24, up12, unbanked }, { 9, 15, 6, 24, up12, unbanked }, { 9, 14, 6, 24, up12, unbanked },
        { 9, 13, 7, 24, up12, unbanked }, { 9, 12, 7, 24, up12, unbanked }, { 9, 11, 7, 24, up12, unbanked }, { 9, 10, 7, 24, up12, unbanked }, { 9,  9, 7, 24, flat, unbanked }, { 9,  8, 7, 24, flat, unbanked },
        { 9,  7, 8, 24, flat, unbanked }, { 9,  6, 8, 24, flat, unbanked }, { 9,  5, 8, 24, flat, unbanked }, { 9,  4, 8, 24, flat, unbanked }, { 9,  3, 8, 24, flat, unbanked }, { 9,  2, 8, 24, flat, unbanked },
        { 9,  1, 8, 24, flat, unbanked }, { 9,  0, 8, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25_0, {
        { 31, 23, 15, 0, down25, unbanked }, { 30, 23, 14, 0, down25, unbanked }, { 29, 23, 14, 0, down25, unbanked }, { 28, 23, 13, 0, down25, unbanked }, { 27, 23, 13, 0, down25, unbanked },
        { 26, 23, 12, 0, down25, unbanked }, { 25, 23, 12, 0, down25, unbanked }, { 24, 23, 11, 0, down25, unbanked }, { 23, 23, 11, 0, down25, unbanked }, { 22, 23, 10, 0, down25, unbanked },
        { 21, 23, 10, 0, down25, unbanked }, { 20, 23,  9, 0, down25, unbanked }, { 19, 23,  9, 0, down25, unbanked }, { 18, 23,  8, 0, down25, unbanked }, { 17, 23,  8, 0, down25, unbanked },
        { 16, 23,  7, 0, down25, unbanked }, { 15, 23,  7, 0, down25, unbanked }, { 14, 23,  6, 0, down25, unbanked }, { 13, 23,  6, 0, down25, unbanked }, { 12, 23,  5, 0, down25, unbanked },
        { 11, 23,  5, 0, down25, unbanked }, { 10, 23,  4, 0, down25, unbanked }, {  9, 23,  4, 0, down25, unbanked }, {  8, 23,  3, 0, down25, unbanked }, {  7, 23,  3, 0, down25, unbanked },
        {  6, 23,  2, 0, down25, unbanked }, {  5, 23,  2, 0, down25, unbanked }, {  4, 23,  1, 0, down25, unbanked }, {  3, 23,  1, 0, down25, unbanked }, {  2, 23,  0, 0, down25, unbanked },
        {  1, 23,  0, 0, down25, unbanked }, {  0, 23, -1, 0, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25_1, {
        { 23,  0, 15, 8, down25, unbanked }, { 23,  1, 15, 8, down25, unbanked }, { 23,  2, 14, 8, down25, unbanked }, { 23,  3, 14, 8, down25, unbanked }, { 23,  4, 13, 8, down25, unbanked },
        { 23,  5, 13, 8, down25, unbanked }, { 23,  6, 12, 8, down25, unbanked }, { 23,  7, 12, 8, down25, unbanked }, { 23,  8, 11, 8, down25, unbanked }, { 23,  9, 11, 8, down25, unbanked },
        { 23, 10, 10, 8, down25, unbanked }, { 23, 11, 10, 8, down25, unbanked }, { 23, 12,  9, 8, down25, unbanked }, { 23, 13,  9, 8, down25, unbanked }, { 23, 14,  8, 8, down25, unbanked },
        { 23, 15,  8, 8, down25, unbanked }, { 23, 16,  7, 8, down25, unbanked }, { 23, 17,  7, 8, down25, unbanked }, { 23, 18,  6, 8, down25, unbanked }, { 23, 19,  6, 8, down25, unbanked },
        { 23, 20,  5, 8, down25, unbanked }, { 23, 21,  5, 8, down25, unbanked }, { 23, 22,  4, 8, down25, unbanked }, { 23, 23,  4, 8, down25, unbanked }, { 23, 24,  3, 8, down25, unbanked },
        { 23, 25,  3, 8, down25, unbanked }, { 23, 26,  2, 8, down25, unbanked }, { 23, 27,  2, 8, down25, unbanked }, { 23, 28,  1, 8, down25, unbanked }, { 23, 29,  1, 8, down25, unbanked },
        { 23, 30,  0, 8, down25, unbanked }, { 23, 31,  0, 8, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25_2, {
        {  0, 9, 15, 16, down25, unbanked }, {  1, 9, 15, 16, down25, unbanked }, {  2, 9, 14, 16, down25, unbanked }, {  3, 9, 14, 16, down25, unbanked }, {  4, 9, 13, 16, down25, unbanked },
        {  5, 9, 13, 16, down25, unbanked }, {  6, 9, 12, 16, down25, unbanked }, {  7, 9, 12, 16, down25, unbanked }, {  8, 9, 11, 16, down25, unbanked }, {  9, 9, 11, 16, down25, unbanked },
        { 10, 9, 10, 16, down25, unbanked }, { 11, 9, 10, 16, down25, unbanked }, { 12, 9,  9, 16, down25, unbanked }, { 13, 9,  9, 16, down25, unbanked }, { 14, 9,  8, 16, down25, unbanked },
        { 15, 9,  8, 16, down25, unbanked }, { 16, 9,  7, 16, down25, unbanked }, { 17, 9,  7, 16, down25, unbanked }, { 18, 9,  6, 16, down25, unbanked }, { 19, 9,  6, 16, down25, unbanked },
        { 20, 9,  5, 16, down25, unbanked }, { 21, 9,  5, 16, down25, unbanked }, { 22, 9,  4, 16, down25, unbanked }, { 23, 9,  4, 16, down25, unbanked }, { 24, 9,  3, 16, down25, unbanked },
        { 25, 9,  3, 16, down25, unbanked }, { 26, 9,  2, 16, down25, unbanked }, { 27, 9,  2, 16, down25, unbanked }, { 28, 9,  1, 16, down25, unbanked }, { 29, 9,  1, 16, down25, unbanked },
        { 30, 9,  0, 16, down25, unbanked }, { 31, 9,  0, 16, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25_3, {
        { 9, 31, 15, 24, down25, unbanked }, { 9, 30, 14, 24, down25, unbanked }, { 9, 29, 14, 24, down25, unbanked }, { 9, 28, 13, 24, down25, unbanked }, { 9, 27, 13, 24, down25, unbanked },
        { 9, 26, 12, 24, down25, unbanked }, { 9, 25, 12, 24, down25, unbanked }, { 9, 24, 11, 24, down25, unbanked }, { 9, 23, 11, 24, down25, unbanked }, { 9, 22, 10, 24, down25, unbanked },
        { 9, 21, 10, 24, down25, unbanked }, { 9, 20,  9, 24, down25, unbanked }, { 9, 19,  9, 24, down25, unbanked }, { 9, 18,  8, 24, down25, unbanked }, { 9, 17,  8, 24, down25, unbanked },
        { 9, 16,  7, 24, down25, unbanked }, { 9, 15,  7, 24, down25, unbanked }, { 9, 14,  6, 24, down25, unbanked }, { 9, 13,  6, 24, down25, unbanked }, { 9, 12,  5, 24, down25, unbanked },
        { 9, 11,  5, 24, down25, unbanked }, { 9, 10,  4, 24, down25, unbanked }, { 9,  9,  4, 24, down25, unbanked }, { 9,  8,  3, 24, down25, unbanked }, { 9,  7,  3, 24, down25, unbanked },
        { 9,  6,  2, 24, down25, unbanked }, { 9,  5,  2, 24, down25, unbanked }, { 9,  4,  1, 24, down25, unbanked }, { 9,  3,  1, 24, down25, unbanked }, { 9,  2,  0, 24, down25, unbanked },
        { 9,  1,  0, 24, down25, unbanked }, { 9,  0, -1, 24, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToDown25_0, {
        { 31, 23, 8, 0,   flat, unbanked }, { 30, 23, 8, 0,   flat, unbanked }, { 29, 23, 8, 0,   flat, unbanked }, { 28, 23, 8, 0,   flat, unbanked }, { 27, 23, 8, 0,   flat, unbanked },
        { 26, 23, 8, 0,   flat, unbanked }, { 25, 23, 8, 0,   flat, unbanked }, { 24, 23, 8, 0,   flat, unbanked }, { 23, 23, 7, 0, down12, unbanked }, { 22, 23, 7, 0, down12, unbanked },
        { 21, 23, 7, 0, down12, unbanked }, { 20, 23, 7, 0, down12, unbanked }, { 19, 23, 7, 0, down12, unbanked }, { 18, 23, 7, 0, down12, unbanked }, { 17, 23, 6, 0, down12, unbanked },
        { 16, 23, 6, 0, down12, unbanked }, { 15, 23, 6, 0, down12, unbanked }, { 14, 23, 6, 0, down12, unbanked }, { 13, 23, 5, 0, down12, unbanked }, { 12, 23, 5, 0, down12, unbanked },
        { 11, 23, 5, 0, down12, unbanked }, { 10, 23, 4, 0, down12, unbanked }, {  9, 23, 4, 0, down12, unbanked }, {  8, 23, 4, 0, down12, unbanked }, {  7, 23, 3, 0, down25, unbanked },
        {  6, 23, 3, 0, down25, unbanked }, {  5, 23, 2, 0, down25, unbanked }, {  4, 23, 2, 0, down25, unbanked }, {  3, 23, 1, 0, down25, unbanked }, {  2, 23, 1, 0, down25, unbanked },
        {  1, 23, 0, 0, down25, unbanked }, {  0, 23, 0, 0, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToDown25_1, {
        { 23,  0, 8, 8,   flat, unbanked }, { 23,  1, 8, 8,   flat, unbanked }, { 23,  2, 8, 8,   flat, unbanked }, { 23,  3, 8, 8,   flat, unbanked }, { 23,  4, 8, 8,   flat, unbanked },
        { 23,  5, 8, 8,   flat, unbanked }, { 23,  6, 8, 8,   flat, unbanked }, { 23,  7, 8, 8,   flat, unbanked }, { 23,  8, 7, 8, down12, unbanked }, { 23,  9, 7, 8, down12, unbanked },
        { 23, 10, 7, 8, down12, unbanked }, { 23, 11, 7, 8, down12, unbanked }, { 23, 12, 7, 8, down12, unbanked }, { 23, 13, 7, 8, down12, unbanked }, { 23, 14, 6, 8, down12, unbanked },
        { 23, 15, 6, 8, down12, unbanked }, { 23, 16, 6, 8, down12, unbanked }, { 23, 17, 6, 8, down12, unbanked }, { 23, 18, 5, 8, down12, unbanked }, { 23, 19, 5, 8, down12, unbanked },
        { 23, 20, 5, 8, down12, unbanked }, { 23, 21, 4, 8, down12, unbanked }, { 23, 22, 4, 8, down12, unbanked }, { 23, 23, 4, 8, down12, unbanked }, { 23, 24, 3, 8, down25, unbanked },
        { 23, 25, 3, 8, down25, unbanked }, { 23, 26, 3, 8, down25, unbanked }, { 23, 27, 2, 8, down25, unbanked }, { 23, 28, 2, 8, down25, unbanked }, { 23, 29, 1, 8, down25, unbanked },
        { 23, 30, 1, 8, down25, unbanked }, { 23, 31, 0, 8, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToDown25_2, {
        {  0, 9, 8, 16,   flat, unbanked }, {  1, 9, 8, 16,   flat, unbanked }, {  2, 9, 8, 16,   flat, unbanked }, {  3, 9, 8, 16,   flat, unbanked }, {  4, 9, 8, 16,   flat, unbanked },
        {  5, 9, 8, 16,   flat, unbanked }, {  6, 9, 8, 16,   flat, unbanked }, {  7, 9, 8, 16,   flat, unbanked }, {  8, 9, 7, 16, down12, unbanked }, {  9, 9, 7, 16, down12, unbanked },
        { 10, 9, 7, 16, down12, unbanked }, { 11, 9, 7, 16, down12, unbanked }, { 12, 9, 7, 16, down12, unbanked }, { 13, 9, 7, 16, down12, unbanked }, { 14, 9, 6, 16, down12, unbanked },
        { 15, 9, 6, 16, down12, unbanked }, { 16, 9, 6, 16, down12, unbanked }, { 17, 9, 6, 16, down12, unbanked }, { 18, 9, 5, 16, down12, unbanked }, { 19, 9, 5, 16, down12, unbanked },
        { 20, 9, 5, 16, down12, unbanked }, { 21, 9, 4, 16, down12, unbanked }, { 22, 9, 4, 16, down12, unbanked }, { 23, 9, 4, 16, down12, unbanked }, { 24, 9, 3, 16, down25, unbanked },
        { 25, 9, 3, 16, down25, unbanked }, { 26, 9, 3, 16, down25, unbanked }, { 27, 9, 2, 16, down25, unbanked }, { 28, 9, 2, 16, down25, unbanked }, { 29, 9, 1, 16, down25, unbanked },
        { 30, 9, 1, 16, down25, unbanked }, { 31, 9, 0, 16, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(FlatToDown25_3, {
        { 9, 31, 8, 24,   flat, unbanked }, { 9, 30, 8, 24,   flat, unbanked }, { 9, 29, 8, 24,   flat, unbanked }, { 9, 28, 8, 24,   flat, unbanked }, { 9, 27, 8, 24,   flat, unbanked },
        { 9, 26, 8, 24,   flat, unbanked }, { 9, 25, 8, 24,   flat, unbanked }, { 9, 24, 8, 24,   flat, unbanked }, { 9, 23, 7, 24, down12, unbanked }, { 9, 22, 7, 24, down12, unbanked },
        { 9, 21, 7, 24, down12, unbanked }, { 9, 20, 7, 24, down12, unbanked }, { 9, 19, 7, 24, down12, unbanked }, { 9, 18, 7, 24, down12, unbanked }, { 9, 17, 6, 24, down12, unbanked },
        { 9, 16, 6, 24, down12, unbanked }, { 9, 15, 6, 24, down12, unbanked }, { 9, 14, 6, 24, down12, unbanked }, { 9, 13, 5, 24, down12, unbanked }, { 9, 12, 5, 24, down12, unbanked },
        { 9, 11, 5, 24, down12, unbanked }, { 9, 10, 4, 24, down12, unbanked }, { 9,  9, 4, 24, down12, unbanked }, { 9,  8, 4, 24, down12, unbanked }, { 9,  7, 3, 24, down25, unbanked },
        { 9,  6, 3, 24, down25, unbanked }, { 9,  5, 2, 24, down25, unbanked }, { 9,  4, 2, 24, down25, unbanked }, { 9,  3, 1, 24, down25, unbanked }, { 9,  2, 1, 24, down25, unbanked },
        { 9,  1, 0, 24, down25, unbanked }, { 9,  0, 0, 24, down25, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25ToFlat_0, {
        { 31, 23, 7, 0, down25, unbanked }, { 30, 23, 7, 0, down25, unbanked }, { 29, 23, 6, 0, down25, unbanked }, { 28, 23, 6, 0, down25, unbanked }, { 27, 23, 5, 0, down25, unbanked },
        { 26, 23, 5, 0, down25, unbanked }, { 25, 23, 4, 0, down25, unbanked }, { 24, 23, 4, 0, down25, unbanked }, { 23, 23, 4, 0, down12, unbanked }, { 22, 23, 3, 0, down12, unbanked },
        { 21, 23, 3, 0, down12, unbanked }, { 20, 23, 3, 0, down12, unbanked }, { 19, 23, 2, 0, down12, unbanked }, { 18, 23, 2, 0, down12, unbanked }, { 17, 23, 2, 0, down12, unbanked },
        { 16, 23, 2, 0, down12, unbanked }, { 15, 23, 1, 0, down12, unbanked }, { 14, 23, 1, 0, down12, unbanked }, { 13, 23, 1, 0, down12, unbanked }, { 12, 23, 1, 0, down12, unbanked },
        { 11, 23, 1, 0, down12, unbanked }, { 10, 23, 1, 0, down12, unbanked }, {  9, 23, 0, 0,   flat, unbanked }, {  8, 23, 0, 0,   flat, unbanked }, {  7, 23, 0, 0,   flat, unbanked },
        {  6, 23, 0, 0,   flat, unbanked }, {  5, 23, 0, 0,   flat, unbanked }, {  4, 23, 0, 0,   flat, unbanked }, {  3, 23, 0, 0,   flat, unbanked }, {  2, 23, 0, 0,   flat, unbanked },
        {  1, 23, 0, 0,   flat, unbanked }, {  0, 23, 0, 0,   flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25ToFlat_1, {
        { 23,  0, 8, 8, down25, unbanked }, { 23,  1, 7, 8, down25, unbanked }, { 23,  2, 7, 8, down25, unbanked }, { 23,  3, 6, 8, down25, unbanked }, { 23,  4, 6, 8, down25, unbanked },
        { 23,  5, 5, 8, down25, unbanked }, { 23,  6, 5, 8, down25, unbanked }, { 23,  7, 4, 8, down25, unbanked }, { 23,  8, 4, 8, down12, unbanked }, { 23,  9, 3, 8, down12, unbanked },
        { 23, 10, 3, 8, down12, unbanked }, { 23, 11, 3, 8, down12, unbanked }, { 23, 12, 2, 8, down12, unbanked }, { 23, 13, 2, 8, down12, unbanked }, { 23, 14, 2, 8, down12, unbanked },
        { 23, 15, 2, 8, down12, unbanked }, { 23, 16, 1, 8, down12, unbanked }, { 23, 17, 1, 8, down12, unbanked }, { 23, 18, 1, 8, down12, unbanked }, { 23, 19, 1, 8, down12, unbanked },
        { 23, 20, 1, 8, down12, unbanked }, { 23, 21, 1, 8, down12, unbanked }, { 23, 22, 0, 8,   flat, unbanked }, { 23, 23, 0, 8,   flat, unbanked }, { 23, 24, 0, 8,   flat, unbanked },
        { 23, 25, 0, 8,   flat, unbanked }, { 23, 26, 0, 8,   flat, unbanked }, { 23, 27, 0, 8,   flat, unbanked }, { 23, 28, 0, 8,   flat, unbanked }, { 23, 29, 0, 8,   flat, unbanked },
        { 23, 30, 0, 8,   flat, unbanked }, { 23, 31, 0, 8,   flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25ToFlat_2, {
        {  0, 9, 8, 16, down25, unbanked }, {  1, 9, 7, 16, down25, unbanked }, {  2, 9, 7, 16, down25, unbanked }, {  3, 9, 6, 16, down25, unbanked }, {  4, 9, 6, 16, down25, unbanked },
        {  5, 9, 5, 16, down25, unbanked }, {  6, 9, 5, 16, down25, unbanked }, {  7, 9, 4, 16, down25, unbanked }, {  8, 9, 4, 16, down12, unbanked }, {  9, 9, 3, 16, down12, unbanked },
        { 10, 9, 3, 16, down12, unbanked }, { 11, 9, 3, 16, down12, unbanked }, { 12, 9, 2, 16, down12, unbanked }, { 13, 9, 2, 16, down12, unbanked }, { 14, 9, 2, 16, down12, unbanked },
        { 15, 9, 2, 16, down12, unbanked }, { 16, 9, 1, 16, down12, unbanked }, { 17, 9, 1, 16, down12, unbanked }, { 18, 9, 1, 16, down12, unbanked }, { 19, 9, 1, 16, down12, unbanked },
        { 20, 9, 1, 16, down12, unbanked }, { 21, 9, 1, 16, down12, unbanked }, { 22, 9, 0, 16,   flat, unbanked }, { 23, 9, 0, 16,   flat, unbanked }, { 24, 9, 0, 16,   flat, unbanked },
        { 25, 9, 0, 16,   flat, unbanked }, { 26, 9, 0, 16,   flat, unbanked }, { 27, 9, 0, 16,   flat, unbanked }, { 28, 9, 0, 16,   flat, unbanked }, { 29, 9, 0, 16,   flat, unbanked },
        { 30, 9, 0, 16,   flat, unbanked }, { 31, 9, 0, 16,   flat, unbanked },
    })

    CREATE_VEHICLE_INFO(Down25ToFlat_3, {
        { 9, 31, 7, 24, down25, unbanked }, { 9, 30, 7, 24, down25, unbanked }, { 9, 29, 6, 24, down25, unbanked }, { 9, 28, 6, 24, down25, unbanked }, { 9, 27, 5, 24, down25, unbanked },
        { 9, 26, 5, 24, down25, unbanked }, { 9, 25, 4, 24, down25, unbanked }, { 9, 24, 4, 24, down25, unbanked }, { 9, 23, 4, 24, down12, unbanked }, { 9, 22, 3, 24, down12, unbanked },
        { 9, 21, 3, 24, down12, unbanked }, { 9, 20, 3, 24, down12, unbanked }, { 9, 19, 2, 24, down12, unbanked }, { 9, 18, 2, 24, down12, unbanked }, { 9, 17, 2, 24, down12, unbanked },
        { 9, 16, 2, 24, down12, unbanked }, { 9, 15, 1, 24, down12, unbanked }, { 9, 14, 1, 24, down12, unbanked }, { 9, 13, 1, 24, down12, unbanked }, { 9, 12, 1, 24, down12, unbanked },
        { 9, 11, 1, 24, down12, unbanked }, { 9, 10, 1, 24, down12, unbanked }, { 9,  9, 0, 24,   flat, unbanked }, { 9,  8, 0, 24,   flat, unbanked }, { 9,  7, 0, 24,   flat, unbanked },
        { 9,  6, 0, 24,   flat, unbanked }, { 9,  5, 0, 24,   flat, unbanked }, { 9,  4, 0, 24,   flat, unbanked }, { 9,  3, 0, 24,   flat, unbanked }, { 9,  2, 0, 24,   flat, unbanked },
        { 9,  1, 0, 24,   flat, unbanked }, { 9,  0, 0, 24,   flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn5Tiles_0, {
        {  31,  23, 0,  0, flat, unbanked }, {  30,  23, 0,  0, flat, unbanked }, {  29,  23, 0,  0, flat, unbanked }, {  28,  23, 0,  0, flat, unbanked }, {  27,  23, 0,  0, flat, unbanked },
        {  26,  23, 0, 31, flat, unbanked }, {  25,  23, 0, 31, flat, unbanked }, {  24,  23, 0, 31, flat, unbanked }, {  23,  23, 0, 31, flat, unbanked }, {  22,  22, 0, 31, flat, unbanked },
        {  21,  22, 0, 31, flat, unbanked }, {  20,  22, 0, 31, flat, unbanked }, {  19,  22, 0, 31, flat, unbanked }, {  18,  22, 0, 31, flat, unbanked }, {  17,  21, 0, 31, flat, unbanked },
        {  16,  21, 0, 31, flat, unbanked }, {  15,  21, 0, 31, flat, unbanked }, {  14,  21, 0, 31, flat, unbanked }, {  13,  20, 0, 30, flat, unbanked }, {  12,  20, 0, 30, flat, unbanked },
        {  11,  20, 0, 30, flat, unbanked }, {  10,  19, 0, 30, flat, unbanked }, {   9,  19, 0, 30, flat, unbanked }, {   8,  19, 0, 30, flat, unbanked }, {   7,  18, 0, 30, flat, unbanked },
        {   6,  18, 0, 30, flat, unbanked }, {   5,  17, 0, 30, flat, unbanked }, {   4,  17, 0, 30, flat, unbanked }, {   3,  16, 0, 30, flat, unbanked }, {   2,  16, 0, 30, flat, unbanked },
        {   1,  15, 0, 29, flat, unbanked }, {   0,  15, 0, 29, flat, unbanked }, {  -1,  14, 0, 29, flat, unbanked }, {  -2,  14, 0, 29, flat, unbanked }, {  -3,  13, 0, 29, flat, unbanked },
        {  -4,  12, 0, 29, flat, unbanked }, {  -5,  12, 0, 29, flat, unbanked }, {  -6,  11, 0, 29, flat, unbanked }, {  -7,  10, 0, 29, flat, unbanked }, {  -8,  10, 0, 29, flat, unbanked },
        {  -9,   9, 0, 29, flat, unbanked }, { -10,   8, 0, 29, flat, unbanked }, { -11,   7, 0, 28, flat, unbanked }, { -12,   6, 0, 28, flat, unbanked }, { -13,   5, 0, 28, flat, unbanked },
        { -14,   4, 0, 28, flat, unbanked }, { -15,   3, 0, 28, flat, unbanked }, { -16,   2, 0, 28, flat, unbanked }, { -17,   1, 0, 28, flat, unbanked }, { -18,   0, 0, 28, flat, unbanked },
        { -19,  -1, 0, 28, flat, unbanked }, { -20,  -2, 0, 28, flat, unbanked }, { -21,  -3, 0, 27, flat, unbanked }, { -22,  -4, 0, 27, flat, unbanked }, { -23,  -5, 0, 27, flat, unbanked },
        { -24,  -6, 0, 27, flat, unbanked }, { -25,  -7, 0, 27, flat, unbanked }, { -25,  -8, 0, 27, flat, unbanked }, { -26,  -9, 0, 27, flat, unbanked }, { -27, -10, 0, 27, flat, unbanked },
        { -27, -11, 0, 27, flat, unbanked }, { -28, -12, 0, 27, flat, unbanked }, { -28, -13, 0, 27, flat, unbanked }, { -29, -14, 0, 27, flat, unbanked }, { -29, -15, 0, 26, flat, unbanked },
        { -30, -16, 0, 26, flat, unbanked }, { -30, -17, 0, 26, flat, unbanked }, { -31, -18, 0, 26, flat, unbanked }, { -31, -19, 0, 26, flat, unbanked }, { -32, -20, 0, 26, flat, unbanked },
        { -32, -21, 0, 26, flat, unbanked }, { -33, -22, 0, 26, flat, unbanked }, { -33, -23, 0, 26, flat, unbanked }, { -34, -24, 0, 26, flat, unbanked }, { -34, -25, 0, 26, flat, unbanked },
        { -34, -26, 0, 26, flat, unbanked }, { -35, -27, 0, 26, flat, unbanked }, { -35, -28, 0, 26, flat, unbanked }, { -35, -29, 0, 26, flat, unbanked }, { -36, -30, 0, 26, flat, unbanked },
        { -36, -31, 0, 26, flat, unbanked }, { -36, -32, 0, 25, flat, unbanked }, { -37, -33, 0, 25, flat, unbanked }, { -37, -34, 0, 25, flat, unbanked }, { -37, -35, 0, 25, flat, unbanked },
        { -37, -36, 0, 25, flat, unbanked }, { -38, -37, 0, 25, flat, unbanked }, { -38, -38, 0, 25, flat, unbanked }, { -38, -39, 0, 25, flat, unbanked }, { -38, -40, 0, 25, flat, unbanked },
        { -39, -41, 0, 25, flat, unbanked }, { -39, -42, 0, 25, flat, unbanked }, { -39, -43, 0, 25, flat, unbanked }, { -39, -44, 0, 25, flat, unbanked }, { -39, -45, 0, 25, flat, unbanked },
        { -39, -46, 0, 25, flat, unbanked }, { -40, -47, 0, 25, flat, unbanked }, { -40, -48, 0, 25, flat, unbanked }, { -40, -49, 0, 25, flat, unbanked }, { -40, -50, 0, 25, flat, unbanked },
        { -40, -51, 0, 25, flat, unbanked }, { -40, -52, 0, 24, flat, unbanked }, { -40, -53, 0, 24, flat, unbanked }, { -40, -54, 0, 24, flat, unbanked }, { -40, -55, 0, 24, flat, unbanked },
        { -40, -56, 0, 24, flat, unbanked }, { -41, -57, 0, 24, flat, unbanked }, { -41, -58, 0, 24, flat, unbanked }, { -41, -59, 0, 24, flat, unbanked }, { -41, -60, 0, 24, flat, unbanked },
        { -41, -61, 0, 24, flat, unbanked }, { -41, -62, 0, 24, flat, unbanked }, { -41, -63, 0, 24, flat, unbanked }, { -41, -64, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn5Tiles_1, {
        {  23,  0, 0, 8, flat, unbanked }, {  23,  1, 0, 8, flat, unbanked }, {  23,  2, 0, 8, flat, unbanked }, {  23,  3, 0, 8, flat, unbanked }, {  23,  4, 0, 8, flat, unbanked },
        {  23,  5, 0, 8, flat, unbanked }, {  23,  6, 0, 7, flat, unbanked }, {  23,  7, 0, 7, flat, unbanked }, {  23,  8, 0, 7, flat, unbanked }, {  23,  9, 0, 7, flat, unbanked },
        {  22, 10, 0, 7, flat, unbanked }, {  22, 11, 0, 7, flat, unbanked }, {  22, 12, 0, 7, flat, unbanked }, {  22, 13, 0, 7, flat, unbanked }, {  22, 14, 0, 7, flat, unbanked },
        {  21, 15, 0, 7, flat, unbanked }, {  21, 16, 0, 7, flat, unbanked }, {  21, 17, 0, 7, flat, unbanked }, {  21, 18, 0, 7, flat, unbanked }, {  20, 19, 0, 6, flat, unbanked },
        {  20, 20, 0, 6, flat, unbanked }, {  20, 21, 0, 6, flat, unbanked }, {  19, 22, 0, 6, flat, unbanked }, {  19, 23, 0, 6, flat, unbanked }, {  19, 24, 0, 6, flat, unbanked },
        {  18, 25, 0, 6, flat, unbanked }, {  18, 26, 0, 6, flat, unbanked }, {  17, 27, 0, 6, flat, unbanked }, {  17, 28, 0, 6, flat, unbanked }, {  16, 29, 0, 6, flat, unbanked },
        {  16, 30, 0, 6, flat, unbanked }, {  15, 31, 0, 5, flat, unbanked }, {  15, 32, 0, 5, flat, unbanked }, {  14, 33, 0, 5, flat, unbanked }, {  14, 34, 0, 5, flat, unbanked },
        {  13, 35, 0, 5, flat, unbanked }, {  12, 36, 0, 5, flat, unbanked }, {  12, 37, 0, 5, flat, unbanked }, {  11, 38, 0, 5, flat, unbanked }, {  10, 39, 0, 5, flat, unbanked },
        {  10, 40, 0, 5, flat, unbanked }, {   9, 41, 0, 5, flat, unbanked }, {   8, 42, 0, 5, flat, unbanked }, {   7, 43, 0, 4, flat, unbanked }, {   6, 44, 0, 4, flat, unbanked },
        {   5, 45, 0, 4, flat, unbanked }, {   4, 46, 0, 4, flat, unbanked }, {   3, 47, 0, 4, flat, unbanked }, {   2, 48, 0, 4, flat, unbanked }, {   1, 49, 0, 4, flat, unbanked },
        {   0, 50, 0, 4, flat, unbanked }, {  -1, 51, 0, 4, flat, unbanked }, {  -2, 52, 0, 4, flat, unbanked }, {  -3, 53, 0, 3, flat, unbanked }, {  -4, 54, 0, 3, flat, unbanked },
        {  -5, 55, 0, 3, flat, unbanked }, {  -6, 56, 0, 3, flat, unbanked }, {  -7, 57, 0, 3, flat, unbanked }, {  -8, 57, 0, 3, flat, unbanked }, {  -9, 58, 0, 3, flat, unbanked },
        { -10, 59, 0, 3, flat, unbanked }, { -11, 59, 0, 3, flat, unbanked }, { -12, 60, 0, 3, flat, unbanked }, { -13, 60, 0, 3, flat, unbanked }, { -14, 61, 0, 3, flat, unbanked },
        { -15, 61, 0, 2, flat, unbanked }, { -16, 62, 0, 2, flat, unbanked }, { -17, 62, 0, 2, flat, unbanked }, { -18, 63, 0, 2, flat, unbanked }, { -19, 63, 0, 2, flat, unbanked },
        { -20, 64, 0, 2, flat, unbanked }, { -21, 64, 0, 2, flat, unbanked }, { -22, 65, 0, 2, flat, unbanked }, { -23, 65, 0, 2, flat, unbanked }, { -24, 66, 0, 2, flat, unbanked },
        { -25, 66, 0, 2, flat, unbanked }, { -26, 66, 0, 2, flat, unbanked }, { -27, 67, 0, 2, flat, unbanked }, { -28, 67, 0, 2, flat, unbanked }, { -29, 67, 0, 2, flat, unbanked },
        { -30, 68, 0, 2, flat, unbanked }, { -31, 68, 0, 2, flat, unbanked }, { -32, 68, 0, 1, flat, unbanked }, { -33, 69, 0, 1, flat, unbanked }, { -34, 69, 0, 1, flat, unbanked },
        { -35, 69, 0, 1, flat, unbanked }, { -36, 69, 0, 1, flat, unbanked }, { -37, 70, 0, 1, flat, unbanked }, { -38, 70, 0, 1, flat, unbanked }, { -39, 70, 0, 1, flat, unbanked },
        { -40, 70, 0, 1, flat, unbanked }, { -41, 71, 0, 1, flat, unbanked }, { -42, 71, 0, 1, flat, unbanked }, { -43, 71, 0, 1, flat, unbanked }, { -44, 71, 0, 1, flat, unbanked },
        { -45, 71, 0, 1, flat, unbanked }, { -46, 71, 0, 1, flat, unbanked }, { -47, 72, 0, 1, flat, unbanked }, { -48, 72, 0, 1, flat, unbanked }, { -49, 72, 0, 1, flat, unbanked },
        { -50, 72, 0, 1, flat, unbanked }, { -51, 72, 0, 1, flat, unbanked }, { -52, 72, 0, 0, flat, unbanked }, { -53, 72, 0, 0, flat, unbanked }, { -54, 72, 0, 0, flat, unbanked },
        { -55, 72, 0, 0, flat, unbanked }, { -56, 72, 0, 0, flat, unbanked }, { -57, 73, 0, 0, flat, unbanked }, { -58, 73, 0, 0, flat, unbanked }, { -59, 73, 0, 0, flat, unbanked },
        { -60, 73, 0, 0, flat, unbanked }, { -61, 73, 0, 0, flat, unbanked }, { -62, 73, 0, 0, flat, unbanked }, { -63, 73, 0, 0, flat, unbanked }, { -64, 73, 0, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn5Tiles_2, {
        {  0,  9, 0, 16, flat, unbanked }, {  1,  9, 0, 16, flat, unbanked }, {  2,  9, 0, 16, flat, unbanked }, {  3,  9, 0, 16, flat, unbanked }, {  4,  9, 0, 16, flat, unbanked },
        {  5,  9, 0, 16, flat, unbanked }, {  6,  9, 0, 15, flat, unbanked }, {  7,  9, 0, 15, flat, unbanked }, {  8,  9, 0, 15, flat, unbanked }, {  9,  9, 0, 15, flat, unbanked },
        { 10, 10, 0, 15, flat, unbanked }, { 11, 10, 0, 15, flat, unbanked }, { 12, 10, 0, 15, flat, unbanked }, { 13, 10, 0, 15, flat, unbanked }, { 14, 10, 0, 15, flat, unbanked },
        { 15, 11, 0, 15, flat, unbanked }, { 16, 11, 0, 15, flat, unbanked }, { 17, 11, 0, 15, flat, unbanked }, { 18, 11, 0, 15, flat, unbanked }, { 19, 12, 0, 14, flat, unbanked },
        { 20, 12, 0, 14, flat, unbanked }, { 21, 12, 0, 14, flat, unbanked }, { 22, 13, 0, 14, flat, unbanked }, { 23, 13, 0, 14, flat, unbanked }, { 24, 13, 0, 14, flat, unbanked },
        { 25, 14, 0, 14, flat, unbanked }, { 26, 14, 0, 14, flat, unbanked }, { 27, 15, 0, 14, flat, unbanked }, { 28, 15, 0, 14, flat, unbanked }, { 29, 16, 0, 14, flat, unbanked },
        { 30, 16, 0, 14, flat, unbanked }, { 31, 17, 0, 13, flat, unbanked }, { 32, 17, 0, 13, flat, unbanked }, { 33, 18, 0, 13, flat, unbanked }, { 34, 18, 0, 13, flat, unbanked },
        { 35, 19, 0, 13, flat, unbanked }, { 36, 20, 0, 13, flat, unbanked }, { 37, 20, 0, 13, flat, unbanked }, { 38, 21, 0, 13, flat, unbanked }, { 39, 22, 0, 13, flat, unbanked },
        { 40, 22, 0, 13, flat, unbanked }, { 41, 23, 0, 13, flat, unbanked }, { 42, 24, 0, 13, flat, unbanked }, { 43, 25, 0, 12, flat, unbanked }, { 44, 26, 0, 12, flat, unbanked },
        { 45, 27, 0, 12, flat, unbanked }, { 46, 28, 0, 12, flat, unbanked }, { 47, 29, 0, 12, flat, unbanked }, { 48, 30, 0, 12, flat, unbanked }, { 49, 31, 0, 12, flat, unbanked },
        { 50, 32, 0, 12, flat, unbanked }, { 51, 33, 0, 12, flat, unbanked }, { 52, 34, 0, 12, flat, unbanked }, { 53, 35, 0, 11, flat, unbanked }, { 54, 36, 0, 11, flat, unbanked },
        { 55, 37, 0, 11, flat, unbanked }, { 56, 38, 0, 11, flat, unbanked }, { 57, 39, 0, 11, flat, unbanked }, { 57, 40, 0, 11, flat, unbanked }, { 58, 41, 0, 11, flat, unbanked },
        { 59, 42, 0, 11, flat, unbanked }, { 59, 43, 0, 11, flat, unbanked }, { 60, 44, 0, 11, flat, unbanked }, { 60, 45, 0, 11, flat, unbanked }, { 61, 46, 0, 11, flat, unbanked },
        { 61, 47, 0, 10, flat, unbanked }, { 62, 48, 0, 10, flat, unbanked }, { 62, 49, 0, 10, flat, unbanked }, { 63, 50, 0, 10, flat, unbanked }, { 63, 51, 0, 10, flat, unbanked },
        { 64, 52, 0, 10, flat, unbanked }, { 64, 53, 0, 10, flat, unbanked }, { 65, 54, 0, 10, flat, unbanked }, { 65, 55, 0, 10, flat, unbanked }, { 66, 56, 0, 10, flat, unbanked },
        { 66, 57, 0, 10, flat, unbanked }, { 66, 58, 0, 10, flat, unbanked }, { 67, 59, 0, 10, flat, unbanked }, { 67, 60, 0, 10, flat, unbanked }, { 67, 61, 0, 10, flat, unbanked },
        { 68, 62, 0, 10, flat, unbanked }, { 68, 63, 0, 10, flat, unbanked }, { 68, 64, 0,  9, flat, unbanked }, { 69, 65, 0,  9, flat, unbanked }, { 69, 66, 0,  9, flat, unbanked },
        { 69, 67, 0,  9, flat, unbanked }, { 69, 68, 0,  9, flat, unbanked }, { 70, 69, 0,  9, flat, unbanked }, { 70, 70, 0,  9, flat, unbanked }, { 70, 71, 0,  9, flat, unbanked },
        { 70, 72, 0,  9, flat, unbanked }, { 71, 73, 0,  9, flat, unbanked }, { 71, 74, 0,  9, flat, unbanked }, { 71, 75, 0,  9, flat, unbanked }, { 71, 76, 0,  9, flat, unbanked },
        { 71, 77, 0,  9, flat, unbanked }, { 71, 78, 0,  9, flat, unbanked }, { 72, 79, 0,  9, flat, unbanked }, { 72, 80, 0,  9, flat, unbanked }, { 72, 81, 0,  9, flat, unbanked },
        { 72, 82, 0,  9, flat, unbanked }, { 72, 83, 0,  9, flat, unbanked }, { 72, 84, 0,  8, flat, unbanked }, { 72, 85, 0,  8, flat, unbanked }, { 72, 86, 0,  8, flat, unbanked },
        { 72, 87, 0,  8, flat, unbanked }, { 72, 88, 0,  8, flat, unbanked }, { 73, 89, 0,  8, flat, unbanked }, { 73, 90, 0,  8, flat, unbanked }, { 73, 91, 0,  8, flat, unbanked },
        { 73, 92, 0,  8, flat, unbanked }, { 73, 93, 0,  8, flat, unbanked }, { 73, 94, 0,  8, flat, unbanked }, { 73, 95, 0,  8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn5Tiles_3, {
        {  9,  31, 0, 24, flat, unbanked }, {  9,  30, 0, 24, flat, unbanked }, {  9,  29, 0, 24, flat, unbanked }, {  9,  28, 0, 24, flat, unbanked }, {  9,  27, 0, 24, flat, unbanked },
        {  9,  26, 0, 23, flat, unbanked }, {  9,  25, 0, 23, flat, unbanked }, {  9,  24, 0, 23, flat, unbanked }, {  9,  23, 0, 23, flat, unbanked }, { 10,  22, 0, 23, flat, unbanked },
        { 10,  21, 0, 23, flat, unbanked }, { 10,  20, 0, 23, flat, unbanked }, { 10,  19, 0, 23, flat, unbanked }, { 10,  18, 0, 23, flat, unbanked }, { 11,  17, 0, 23, flat, unbanked },
        { 11,  16, 0, 23, flat, unbanked }, { 11,  15, 0, 23, flat, unbanked }, { 11,  14, 0, 23, flat, unbanked }, { 12,  13, 0, 22, flat, unbanked }, { 12,  12, 0, 22, flat, unbanked },
        { 12,  11, 0, 22, flat, unbanked }, { 13,  10, 0, 22, flat, unbanked }, { 13,   9, 0, 22, flat, unbanked }, { 13,   8, 0, 22, flat, unbanked }, { 14,   7, 0, 22, flat, unbanked },
        { 14,   6, 0, 22, flat, unbanked }, { 15,   5, 0, 22, flat, unbanked }, { 15,   4, 0, 22, flat, unbanked }, { 16,   3, 0, 22, flat, unbanked }, { 16,   2, 0, 22, flat, unbanked },
        { 17,   1, 0, 21, flat, unbanked }, { 17,   0, 0, 21, flat, unbanked }, { 18,  -1, 0, 21, flat, unbanked }, { 18,  -2, 0, 21, flat, unbanked }, { 19,  -3, 0, 21, flat, unbanked },
        { 20,  -4, 0, 21, flat, unbanked }, { 20,  -5, 0, 21, flat, unbanked }, { 21,  -6, 0, 21, flat, unbanked }, { 22,  -7, 0, 21, flat, unbanked }, { 22,  -8, 0, 21, flat, unbanked },
        { 23,  -9, 0, 21, flat, unbanked }, { 24, -10, 0, 21, flat, unbanked }, { 25, -11, 0, 20, flat, unbanked }, { 26, -12, 0, 20, flat, unbanked }, { 27, -13, 0, 20, flat, unbanked },
        { 28, -14, 0, 20, flat, unbanked }, { 29, -15, 0, 20, flat, unbanked }, { 30, -16, 0, 20, flat, unbanked }, { 31, -17, 0, 20, flat, unbanked }, { 32, -18, 0, 20, flat, unbanked },
        { 33, -19, 0, 20, flat, unbanked }, { 34, -20, 0, 20, flat, unbanked }, { 35, -21, 0, 19, flat, unbanked }, { 36, -22, 0, 19, flat, unbanked }, { 37, -23, 0, 19, flat, unbanked },
        { 38, -24, 0, 19, flat, unbanked }, { 39, -25, 0, 19, flat, unbanked }, { 40, -25, 0, 19, flat, unbanked }, { 41, -26, 0, 19, flat, unbanked }, { 42, -27, 0, 19, flat, unbanked },
        { 43, -27, 0, 19, flat, unbanked }, { 44, -28, 0, 19, flat, unbanked }, { 45, -28, 0, 19, flat, unbanked }, { 46, -29, 0, 19, flat, unbanked }, { 47, -29, 0, 18, flat, unbanked },
        { 48, -30, 0, 18, flat, unbanked }, { 49, -30, 0, 18, flat, unbanked }, { 50, -31, 0, 18, flat, unbanked }, { 51, -31, 0, 18, flat, unbanked }, { 52, -32, 0, 18, flat, unbanked },
        { 53, -32, 0, 18, flat, unbanked }, { 54, -33, 0, 18, flat, unbanked }, { 55, -33, 0, 18, flat, unbanked }, { 56, -34, 0, 18, flat, unbanked }, { 57, -34, 0, 18, flat, unbanked },
        { 58, -34, 0, 18, flat, unbanked }, { 59, -35, 0, 18, flat, unbanked }, { 60, -35, 0, 18, flat, unbanked }, { 61, -35, 0, 18, flat, unbanked }, { 62, -36, 0, 18, flat, unbanked },
        { 63, -36, 0, 18, flat, unbanked }, { 64, -36, 0, 17, flat, unbanked }, { 65, -37, 0, 17, flat, unbanked }, { 66, -37, 0, 17, flat, unbanked }, { 67, -37, 0, 17, flat, unbanked },
        { 68, -37, 0, 17, flat, unbanked }, { 69, -38, 0, 17, flat, unbanked }, { 70, -38, 0, 17, flat, unbanked }, { 71, -38, 0, 17, flat, unbanked }, { 72, -38, 0, 17, flat, unbanked },
        { 73, -39, 0, 17, flat, unbanked }, { 74, -39, 0, 17, flat, unbanked }, { 75, -39, 0, 17, flat, unbanked }, { 76, -39, 0, 17, flat, unbanked }, { 77, -39, 0, 17, flat, unbanked },
        { 78, -39, 0, 17, flat, unbanked }, { 79, -40, 0, 17, flat, unbanked }, { 80, -40, 0, 17, flat, unbanked }, { 81, -40, 0, 17, flat, unbanked }, { 82, -40, 0, 17, flat, unbanked },
        { 83, -40, 0, 17, flat, unbanked }, { 84, -40, 0, 16, flat, unbanked }, { 85, -40, 0, 16, flat, unbanked }, { 86, -40, 0, 16, flat, unbanked }, { 87, -40, 0, 16, flat, unbanked },
        { 88, -40, 0, 16, flat, unbanked }, { 89, -41, 0, 16, flat, unbanked }, { 90, -41, 0, 16, flat, unbanked }, { 91, -41, 0, 16, flat, unbanked }, { 92, -41, 0, 16, flat, unbanked },
        { 93, -41, 0, 16, flat, unbanked }, { 94, -41, 0, 16, flat, unbanked }, { 95, -41, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn5Tiles_0, {
        {  31, 23, 0, 0, flat, unbanked }, {  30, 23, 0, 0, flat, unbanked }, {  29, 23, 0, 0, flat, unbanked }, {  28, 23, 0, 0, flat, unbanked }, {  27, 23, 0, 0, flat, unbanked },
        {  26, 23, 0, 0, flat, unbanked }, {  25, 24, 0, 0, flat, unbanked }, {  24, 24, 0, 0, flat, unbanked }, {  23, 24, 0, 0, flat, unbanked }, {  22, 24, 0, 0, flat, unbanked },
        {  21, 24, 0, 0, flat, unbanked }, {  20, 24, 0, 1, flat, unbanked }, {  19, 24, 0, 1, flat, unbanked }, {  18, 24, 0, 1, flat, unbanked }, {  17, 24, 0, 1, flat, unbanked },
        {  16, 24, 0, 1, flat, unbanked }, {  15, 25, 0, 1, flat, unbanked }, {  14, 25, 0, 1, flat, unbanked }, {  13, 25, 0, 1, flat, unbanked }, {  12, 25, 0, 1, flat, unbanked },
        {  11, 25, 0, 1, flat, unbanked }, {  10, 25, 0, 1, flat, unbanked }, {   9, 26, 0, 1, flat, unbanked }, {   8, 26, 0, 1, flat, unbanked }, {   7, 26, 0, 1, flat, unbanked },
        {   6, 26, 0, 1, flat, unbanked }, {   5, 27, 0, 1, flat, unbanked }, {   4, 27, 0, 1, flat, unbanked }, {   3, 27, 0, 1, flat, unbanked }, {   2, 27, 0, 1, flat, unbanked },
        {   1, 28, 0, 1, flat, unbanked }, {   0, 28, 0, 2, flat, unbanked }, {  -1, 28, 0, 2, flat, unbanked }, {  -2, 29, 0, 2, flat, unbanked }, {  -3, 29, 0, 2, flat, unbanked },
        {  -4, 29, 0, 2, flat, unbanked }, {  -5, 30, 0, 2, flat, unbanked }, {  -6, 30, 0, 2, flat, unbanked }, {  -7, 30, 0, 2, flat, unbanked }, {  -8, 31, 0, 2, flat, unbanked },
        {  -9, 31, 0, 2, flat, unbanked }, { -10, 32, 0, 2, flat, unbanked }, { -11, 32, 0, 2, flat, unbanked }, { -12, 33, 0, 2, flat, unbanked }, { -13, 33, 0, 2, flat, unbanked },
        { -14, 33, 0, 2, flat, unbanked }, { -15, 34, 0, 2, flat, unbanked }, { -16, 35, 0, 2, flat, unbanked }, { -17, 35, 0, 3, flat, unbanked }, { -18, 36, 0, 3, flat, unbanked },
        { -19, 36, 0, 3, flat, unbanked }, { -20, 37, 0, 3, flat, unbanked }, { -21, 37, 0, 3, flat, unbanked }, { -22, 38, 0, 3, flat, unbanked }, { -23, 39, 0, 3, flat, unbanked },
        { -24, 39, 0, 3, flat, unbanked }, { -25, 40, 0, 3, flat, unbanked }, { -26, 41, 0, 3, flat, unbanked }, { -27, 42, 0, 3, flat, unbanked }, { -28, 42, 0, 3, flat, unbanked },
        { -29, 43, 0, 3, flat, unbanked }, { -30, 44, 0, 4, flat, unbanked }, { -31, 45, 0, 4, flat, unbanked }, { -32, 46, 0, 4, flat, unbanked }, { -33, 47, 0, 4, flat, unbanked },
        { -34, 48, 0, 4, flat, unbanked }, { -35, 49, 0, 4, flat, unbanked }, { -36, 50, 0, 4, flat, unbanked }, { -37, 51, 0, 4, flat, unbanked }, { -38, 52, 0, 4, flat, unbanked },
        { -39, 53, 0, 4, flat, unbanked }, { -40, 54, 0, 5, flat, unbanked }, { -41, 55, 0, 5, flat, unbanked }, { -42, 56, 0, 5, flat, unbanked }, { -43, 57, 0, 5, flat, unbanked },
        { -44, 58, 0, 5, flat, unbanked }, { -44, 59, 0, 5, flat, unbanked }, { -45, 60, 0, 5, flat, unbanked }, { -46, 61, 0, 5, flat, unbanked }, { -46, 62, 0, 5, flat, unbanked },
        { -47, 63, 0, 5, flat, unbanked }, { -47, 64, 0, 5, flat, unbanked }, { -48, 65, 0, 6, flat, unbanked }, { -48, 66, 0, 6, flat, unbanked }, { -49, 67, 0, 6, flat, unbanked },
        { -49, 68, 0, 6, flat, unbanked }, { -50, 69, 0, 6, flat, unbanked }, { -50, 70, 0, 6, flat, unbanked }, { -51, 71, 0, 6, flat, unbanked }, { -51, 72, 0, 6, flat, unbanked },
        { -51, 73, 0, 6, flat, unbanked }, { -52, 74, 0, 6, flat, unbanked }, { -52, 75, 0, 6, flat, unbanked }, { -52, 76, 0, 6, flat, unbanked }, { -53, 77, 0, 7, flat, unbanked },
        { -53, 78, 0, 7, flat, unbanked }, { -53, 79, 0, 7, flat, unbanked }, { -53, 80, 0, 7, flat, unbanked }, { -54, 81, 0, 7, flat, unbanked }, { -54, 82, 0, 7, flat, unbanked },
        { -54, 83, 0, 7, flat, unbanked }, { -54, 84, 0, 7, flat, unbanked }, { -54, 85, 0, 7, flat, unbanked }, { -55, 86, 0, 7, flat, unbanked }, { -55, 87, 0, 7, flat, unbanked },
        { -55, 88, 0, 7, flat, unbanked }, { -55, 89, 0, 7, flat, unbanked }, { -55, 90, 0, 8, flat, unbanked }, { -55, 91, 0, 8, flat, unbanked }, { -55, 92, 0, 8, flat, unbanked },
        { -55, 93, 0, 8, flat, unbanked }, { -55, 94, 0, 8, flat, unbanked }, { -55, 95, 0, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn5Tiles_1, {
        { 23,  0, 0,  8, flat, unbanked }, { 23,  1, 0,  8, flat, unbanked }, { 23,  2, 0,  8, flat, unbanked }, { 23,  3, 0,  8, flat, unbanked }, { 23,  4, 0,  8, flat, unbanked },
        { 23,  5, 0,  8, flat, unbanked }, { 23,  6, 0,  8, flat, unbanked }, { 24,  7, 0,  8, flat, unbanked }, { 24,  8, 0,  8, flat, unbanked }, { 24,  9, 0,  8, flat, unbanked },
        { 24, 10, 0,  8, flat, unbanked }, { 24, 11, 0,  8, flat, unbanked }, { 24, 12, 0,  9, flat, unbanked }, { 24, 13, 0,  9, flat, unbanked }, { 24, 14, 0,  9, flat, unbanked },
        { 24, 15, 0,  9, flat, unbanked }, { 24, 16, 0,  9, flat, unbanked }, { 25, 17, 0,  9, flat, unbanked }, { 25, 18, 0,  9, flat, unbanked }, { 25, 19, 0,  9, flat, unbanked },
        { 25, 20, 0,  9, flat, unbanked }, { 25, 21, 0,  9, flat, unbanked }, { 25, 22, 0,  9, flat, unbanked }, { 26, 23, 0,  9, flat, unbanked }, { 26, 24, 0,  9, flat, unbanked },
        { 26, 25, 0,  9, flat, unbanked }, { 26, 26, 0,  9, flat, unbanked }, { 27, 27, 0,  9, flat, unbanked }, { 27, 28, 0,  9, flat, unbanked }, { 27, 29, 0,  9, flat, unbanked },
        { 27, 30, 0,  9, flat, unbanked }, { 28, 31, 0,  9, flat, unbanked }, { 28, 32, 0, 10, flat, unbanked }, { 28, 33, 0, 10, flat, unbanked }, { 29, 34, 0, 10, flat, unbanked },
        { 29, 35, 0, 10, flat, unbanked }, { 29, 36, 0, 10, flat, unbanked }, { 30, 37, 0, 10, flat, unbanked }, { 30, 38, 0, 10, flat, unbanked }, { 30, 39, 0, 10, flat, unbanked },
        { 31, 40, 0, 10, flat, unbanked }, { 31, 41, 0, 10, flat, unbanked }, { 32, 42, 0, 10, flat, unbanked }, { 32, 43, 0, 10, flat, unbanked }, { 33, 44, 0, 10, flat, unbanked },
        { 33, 45, 0, 10, flat, unbanked }, { 33, 46, 0, 10, flat, unbanked }, { 34, 47, 0, 10, flat, unbanked }, { 35, 48, 0, 10, flat, unbanked }, { 35, 49, 0, 11, flat, unbanked },
        { 36, 50, 0, 11, flat, unbanked }, { 36, 51, 0, 11, flat, unbanked }, { 37, 52, 0, 11, flat, unbanked }, { 37, 53, 0, 11, flat, unbanked }, { 38, 54, 0, 11, flat, unbanked },
        { 39, 55, 0, 11, flat, unbanked }, { 39, 56, 0, 11, flat, unbanked }, { 40, 57, 0, 11, flat, unbanked }, { 41, 58, 0, 11, flat, unbanked }, { 42, 59, 0, 11, flat, unbanked },
        { 42, 60, 0, 11, flat, unbanked }, { 43, 61, 0, 11, flat, unbanked }, { 44, 62, 0, 12, flat, unbanked }, { 45, 63, 0, 12, flat, unbanked }, { 46, 64, 0, 12, flat, unbanked },
        { 47, 65, 0, 12, flat, unbanked }, { 48, 66, 0, 12, flat, unbanked }, { 49, 67, 0, 12, flat, unbanked }, { 50, 68, 0, 12, flat, unbanked }, { 51, 69, 0, 12, flat, unbanked },
        { 52, 70, 0, 12, flat, unbanked }, { 53, 71, 0, 12, flat, unbanked }, { 54, 72, 0, 13, flat, unbanked }, { 55, 73, 0, 13, flat, unbanked }, { 56, 74, 0, 13, flat, unbanked },
        { 57, 75, 0, 13, flat, unbanked }, { 58, 76, 0, 13, flat, unbanked }, { 59, 76, 0, 13, flat, unbanked }, { 60, 77, 0, 13, flat, unbanked }, { 61, 78, 0, 13, flat, unbanked },
        { 62, 78, 0, 13, flat, unbanked }, { 63, 79, 0, 13, flat, unbanked }, { 64, 79, 0, 13, flat, unbanked }, { 65, 80, 0, 14, flat, unbanked }, { 66, 80, 0, 14, flat, unbanked },
        { 67, 81, 0, 14, flat, unbanked }, { 68, 81, 0, 14, flat, unbanked }, { 69, 82, 0, 14, flat, unbanked }, { 70, 82, 0, 14, flat, unbanked }, { 71, 83, 0, 14, flat, unbanked },
        { 72, 83, 0, 14, flat, unbanked }, { 73, 83, 0, 14, flat, unbanked }, { 74, 84, 0, 14, flat, unbanked }, { 75, 84, 0, 14, flat, unbanked }, { 76, 84, 0, 14, flat, unbanked },
        { 77, 85, 0, 15, flat, unbanked }, { 78, 85, 0, 15, flat, unbanked }, { 79, 85, 0, 15, flat, unbanked }, { 80, 85, 0, 15, flat, unbanked }, { 81, 86, 0, 15, flat, unbanked },
        { 82, 86, 0, 15, flat, unbanked }, { 83, 86, 0, 15, flat, unbanked }, { 84, 86, 0, 15, flat, unbanked }, { 85, 86, 0, 15, flat, unbanked }, { 86, 87, 0, 15, flat, unbanked },
        { 87, 87, 0, 15, flat, unbanked }, { 88, 87, 0, 15, flat, unbanked }, { 89, 87, 0, 15, flat, unbanked }, { 90, 87, 0, 16, flat, unbanked }, { 91, 87, 0, 16, flat, unbanked },
        { 92, 87, 0, 16, flat, unbanked }, { 93, 87, 0, 16, flat, unbanked }, { 94, 87, 0, 16, flat, unbanked }, { 95, 87, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn5Tiles_2, {
        {  0,   9, 0, 16, flat, unbanked }, {  1,   9, 0, 16, flat, unbanked }, {  2,   9, 0, 16, flat, unbanked }, {  3,   9, 0, 16, flat, unbanked }, {  4,   9, 0, 16, flat, unbanked },
        {  5,   9, 0, 16, flat, unbanked }, {  6,   9, 0, 16, flat, unbanked }, {  7,   8, 0, 16, flat, unbanked }, {  8,   8, 0, 16, flat, unbanked }, {  9,   8, 0, 16, flat, unbanked },
        { 10,   8, 0, 16, flat, unbanked }, { 11,   8, 0, 16, flat, unbanked }, { 12,   8, 0, 17, flat, unbanked }, { 13,   8, 0, 17, flat, unbanked }, { 14,   8, 0, 17, flat, unbanked },
        { 15,   8, 0, 17, flat, unbanked }, { 16,   8, 0, 17, flat, unbanked }, { 17,   7, 0, 17, flat, unbanked }, { 18,   7, 0, 17, flat, unbanked }, { 19,   7, 0, 17, flat, unbanked },
        { 20,   7, 0, 17, flat, unbanked }, { 21,   7, 0, 17, flat, unbanked }, { 22,   7, 0, 17, flat, unbanked }, { 23,   6, 0, 17, flat, unbanked }, { 24,   6, 0, 17, flat, unbanked },
        { 25,   6, 0, 17, flat, unbanked }, { 26,   6, 0, 17, flat, unbanked }, { 27,   5, 0, 17, flat, unbanked }, { 28,   5, 0, 17, flat, unbanked }, { 29,   5, 0, 17, flat, unbanked },
        { 30,   5, 0, 17, flat, unbanked }, { 31,   4, 0, 17, flat, unbanked }, { 32,   4, 0, 18, flat, unbanked }, { 33,   4, 0, 18, flat, unbanked }, { 34,   3, 0, 18, flat, unbanked },
        { 35,   3, 0, 18, flat, unbanked }, { 36,   3, 0, 18, flat, unbanked }, { 37,   2, 0, 18, flat, unbanked }, { 38,   2, 0, 18, flat, unbanked }, { 39,   2, 0, 18, flat, unbanked },
        { 40,   1, 0, 18, flat, unbanked }, { 41,   1, 0, 18, flat, unbanked }, { 42,   0, 0, 18, flat, unbanked }, { 43,   0, 0, 18, flat, unbanked }, { 44,  -1, 0, 18, flat, unbanked },
        { 45,  -1, 0, 18, flat, unbanked }, { 46,  -1, 0, 18, flat, unbanked }, { 47,  -2, 0, 18, flat, unbanked }, { 48,  -3, 0, 18, flat, unbanked }, { 49,  -3, 0, 19, flat, unbanked },
        { 50,  -4, 0, 19, flat, unbanked }, { 51,  -4, 0, 19, flat, unbanked }, { 52,  -5, 0, 19, flat, unbanked }, { 53,  -5, 0, 19, flat, unbanked }, { 54,  -6, 0, 19, flat, unbanked },
        { 55,  -7, 0, 19, flat, unbanked }, { 56,  -7, 0, 19, flat, unbanked }, { 57,  -8, 0, 19, flat, unbanked }, { 58,  -9, 0, 19, flat, unbanked }, { 59, -10, 0, 19, flat, unbanked },
        { 60, -10, 0, 19, flat, unbanked }, { 61, -11, 0, 19, flat, unbanked }, { 62, -12, 0, 20, flat, unbanked }, { 63, -13, 0, 20, flat, unbanked }, { 64, -14, 0, 20, flat, unbanked },
        { 65, -15, 0, 20, flat, unbanked }, { 66, -16, 0, 20, flat, unbanked }, { 67, -17, 0, 20, flat, unbanked }, { 68, -18, 0, 20, flat, unbanked }, { 69, -19, 0, 20, flat, unbanked },
        { 70, -20, 0, 20, flat, unbanked }, { 71, -21, 0, 20, flat, unbanked }, { 72, -22, 0, 21, flat, unbanked }, { 73, -23, 0, 21, flat, unbanked }, { 74, -24, 0, 21, flat, unbanked },
        { 75, -25, 0, 21, flat, unbanked }, { 76, -26, 0, 21, flat, unbanked }, { 76, -27, 0, 21, flat, unbanked }, { 77, -28, 0, 21, flat, unbanked }, { 78, -29, 0, 21, flat, unbanked },
        { 78, -30, 0, 21, flat, unbanked }, { 79, -31, 0, 21, flat, unbanked }, { 79, -32, 0, 21, flat, unbanked }, { 80, -33, 0, 22, flat, unbanked }, { 80, -34, 0, 22, flat, unbanked },
        { 81, -35, 0, 22, flat, unbanked }, { 81, -36, 0, 22, flat, unbanked }, { 82, -37, 0, 22, flat, unbanked }, { 82, -38, 0, 22, flat, unbanked }, { 83, -39, 0, 22, flat, unbanked },
        { 83, -40, 0, 22, flat, unbanked }, { 83, -41, 0, 22, flat, unbanked }, { 84, -42, 0, 22, flat, unbanked }, { 84, -43, 0, 22, flat, unbanked }, { 84, -44, 0, 22, flat, unbanked },
        { 85, -45, 0, 23, flat, unbanked }, { 85, -46, 0, 23, flat, unbanked }, { 85, -47, 0, 23, flat, unbanked }, { 85, -48, 0, 23, flat, unbanked }, { 86, -49, 0, 23, flat, unbanked },
        { 86, -50, 0, 23, flat, unbanked }, { 86, -51, 0, 23, flat, unbanked }, { 86, -52, 0, 23, flat, unbanked }, { 86, -53, 0, 23, flat, unbanked }, { 87, -54, 0, 23, flat, unbanked },
        { 87, -55, 0, 23, flat, unbanked }, { 87, -56, 0, 23, flat, unbanked }, { 87, -57, 0, 23, flat, unbanked }, { 87, -58, 0, 24, flat, unbanked }, { 87, -59, 0, 24, flat, unbanked },
        { 87, -60, 0, 24, flat, unbanked }, { 87, -61, 0, 24, flat, unbanked }, { 87, -62, 0, 24, flat, unbanked }, { 87, -63, 0, 24, flat, unbanked }, { 87, -64, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn5Tiles_3, {
        {   9,  31, 0, 24, flat, unbanked }, {   9,  30, 0, 24, flat, unbanked }, {   9,  29, 0, 24, flat, unbanked }, {   9,  28, 0, 24, flat, unbanked }, {   9,  27, 0, 24, flat, unbanked },
        {   9,  26, 0, 24, flat, unbanked }, {   8,  25, 0, 24, flat, unbanked }, {   8,  24, 0, 24, flat, unbanked }, {   8,  23, 0, 24, flat, unbanked }, {   8,  22, 0, 24, flat, unbanked },
        {   8,  21, 0, 24, flat, unbanked }, {   8,  20, 0, 25, flat, unbanked }, {   8,  19, 0, 25, flat, unbanked }, {   8,  18, 0, 25, flat, unbanked }, {   8,  17, 0, 25, flat, unbanked },
        {   8,  16, 0, 25, flat, unbanked }, {   7,  15, 0, 25, flat, unbanked }, {   7,  14, 0, 25, flat, unbanked }, {   7,  13, 0, 25, flat, unbanked }, {   7,  12, 0, 25, flat, unbanked },
        {   7,  11, 0, 25, flat, unbanked }, {   7,  10, 0, 25, flat, unbanked }, {   6,   9, 0, 25, flat, unbanked }, {   6,   8, 0, 25, flat, unbanked }, {   6,   7, 0, 25, flat, unbanked },
        {   6,   6, 0, 25, flat, unbanked }, {   5,   5, 0, 25, flat, unbanked }, {   5,   4, 0, 25, flat, unbanked }, {   5,   3, 0, 25, flat, unbanked }, {   5,   2, 0, 25, flat, unbanked },
        {   4,   1, 0, 25, flat, unbanked }, {   4,   0, 0, 26, flat, unbanked }, {   4,  -1, 0, 26, flat, unbanked }, {   3,  -2, 0, 26, flat, unbanked }, {   3,  -3, 0, 26, flat, unbanked },
        {   3,  -4, 0, 26, flat, unbanked }, {   2,  -5, 0, 26, flat, unbanked }, {   2,  -6, 0, 26, flat, unbanked }, {   2,  -7, 0, 26, flat, unbanked }, {   1,  -8, 0, 26, flat, unbanked },
        {   1,  -9, 0, 26, flat, unbanked }, {   0, -10, 0, 26, flat, unbanked }, {   0, -11, 0, 26, flat, unbanked }, {  -1, -12, 0, 26, flat, unbanked }, {  -1, -13, 0, 26, flat, unbanked },
        {  -1, -14, 0, 26, flat, unbanked }, {  -2, -15, 0, 26, flat, unbanked }, {  -3, -16, 0, 26, flat, unbanked }, {  -3, -17, 0, 27, flat, unbanked }, {  -4, -18, 0, 27, flat, unbanked },
        {  -4, -19, 0, 27, flat, unbanked }, {  -5, -20, 0, 27, flat, unbanked }, {  -5, -21, 0, 27, flat, unbanked }, {  -6, -22, 0, 27, flat, unbanked }, {  -7, -23, 0, 27, flat, unbanked },
        {  -7, -24, 0, 27, flat, unbanked }, {  -8, -25, 0, 27, flat, unbanked }, {  -9, -26, 0, 27, flat, unbanked }, { -10, -27, 0, 27, flat, unbanked }, { -10, -28, 0, 27, flat, unbanked },
        { -11, -29, 0, 27, flat, unbanked }, { -12, -30, 0, 28, flat, unbanked }, { -13, -31, 0, 28, flat, unbanked }, { -14, -32, 0, 28, flat, unbanked }, { -15, -33, 0, 28, flat, unbanked },
        { -16, -34, 0, 28, flat, unbanked }, { -17, -35, 0, 28, flat, unbanked }, { -18, -36, 0, 28, flat, unbanked }, { -19, -37, 0, 28, flat, unbanked }, { -20, -38, 0, 28, flat, unbanked },
        { -21, -39, 0, 28, flat, unbanked }, { -22, -40, 0, 29, flat, unbanked }, { -23, -41, 0, 29, flat, unbanked }, { -24, -42, 0, 29, flat, unbanked }, { -25, -43, 0, 29, flat, unbanked },
        { -26, -44, 0, 29, flat, unbanked }, { -27, -44, 0, 29, flat, unbanked }, { -28, -45, 0, 29, flat, unbanked }, { -29, -46, 0, 29, flat, unbanked }, { -30, -46, 0, 29, flat, unbanked },
        { -31, -47, 0, 29, flat, unbanked }, { -32, -47, 0, 29, flat, unbanked }, { -33, -48, 0, 30, flat, unbanked }, { -34, -48, 0, 30, flat, unbanked }, { -35, -49, 0, 30, flat, unbanked },
        { -36, -49, 0, 30, flat, unbanked }, { -37, -50, 0, 30, flat, unbanked }, { -38, -50, 0, 30, flat, unbanked }, { -39, -51, 0, 30, flat, unbanked }, { -40, -51, 0, 30, flat, unbanked },
        { -41, -51, 0, 30, flat, unbanked }, { -42, -52, 0, 30, flat, unbanked }, { -43, -52, 0, 30, flat, unbanked }, { -44, -52, 0, 30, flat, unbanked }, { -45, -53, 0, 31, flat, unbanked },
        { -46, -53, 0, 31, flat, unbanked }, { -47, -53, 0, 31, flat, unbanked }, { -48, -53, 0, 31, flat, unbanked }, { -49, -54, 0, 31, flat, unbanked }, { -50, -54, 0, 31, flat, unbanked },
        { -51, -54, 0, 31, flat, unbanked }, { -52, -54, 0, 31, flat, unbanked }, { -53, -54, 0, 31, flat, unbanked }, { -54, -55, 0, 31, flat, unbanked }, { -55, -55, 0, 31, flat, unbanked },
        { -56, -55, 0, 31, flat, unbanked }, { -57, -55, 0, 31, flat, unbanked }, { -58, -55, 0,  0, flat, unbanked }, { -59, -55, 0,  0, flat, unbanked }, { -60, -55, 0,  0, flat, unbanked },
        { -61, -55, 0,  0, flat, unbanked }, { -62, -55, 0,  0, flat, unbanked }, { -63, -55, 0,  0, flat, unbanked }, { -64, -55, 0,  0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(SBendLeft_0, {
        {  31,  23, 0,  0, flat, unbanked }, {  30,  23, 0,  0, flat, unbanked }, {  29,  23, 0,  0, flat, unbanked }, {  28,  23, 0,  0, flat, unbanked }, {  27,  23, 0,  0, flat, unbanked },
        {  26,  23, 0,  0, flat, unbanked }, {  25,  23, 0, 31, flat, unbanked }, {  24,  23, 0, 31, flat, unbanked }, {  23,  23, 0, 31, flat, unbanked }, {  22,  23, 0, 31, flat, unbanked },
        {  21,  22, 0, 31, flat, unbanked }, {  20,  22, 0, 31, flat, unbanked }, {  19,  22, 0, 31, flat, unbanked }, {  18,  22, 0, 31, flat, unbanked }, {  17,  22, 0, 31, flat, unbanked },
        {  16,  21, 0, 31, flat, unbanked }, {  15,  21, 0, 31, flat, unbanked }, {  14,  21, 0, 31, flat, unbanked }, {  13,  21, 0, 31, flat, unbanked }, {  12,  20, 0, 30, flat, unbanked },
        {  11,  20, 0, 30, flat, unbanked }, {  10,  20, 0, 30, flat, unbanked }, {   9,  19, 0, 30, flat, unbanked }, {   8,  19, 0, 30, flat, unbanked }, {   7,  18, 0, 30, flat, unbanked },
        {   6,  18, 0, 30, flat, unbanked }, {   5,  17, 0, 30, flat, unbanked }, {   4,  17, 0, 30, flat, unbanked }, {   3,  16, 0, 29, flat, unbanked }, {   2,  16, 0, 29, flat, unbanked },
        {   1,  15, 0, 29, flat, unbanked }, {   0,  15, 0, 29, flat, unbanked }, {  -1,  14, 0, 29, flat, unbanked }, {  -2,  13, 0, 29, flat, unbanked }, {  -3,  13, 0, 29, flat, unbanked },
        {  -4,  12, 0, 29, flat, unbanked }, {  -5,  11, 0, 28, flat, unbanked }, {  -6,  10, 0, 28, flat, unbanked }, {  -7,   9, 0, 28, flat, unbanked }, {  -8,   8, 0, 28, flat, unbanked },
        {  -9,   7, 0, 28, flat, unbanked }, { -10,   6, 0, 28, flat, unbanked }, { -11,   5, 0, 28, flat, unbanked }, { -12,   4, 0, 28, flat, unbanked }, { -13,   3, 0, 28, flat, unbanked },
        { -14,   2, 0, 28, flat, unbanked }, { -15,   1, 0, 28, flat, unbanked }, { -16,   0, 0, 28, flat, unbanked }, { -17,  -1, 0, 28, flat, unbanked }, { -18,  -2, 0, 28, flat, unbanked },
        { -19,  -3, 0, 28, flat, unbanked }, { -20,  -4, 0, 28, flat, unbanked }, { -21,  -5, 0, 28, flat, unbanked }, { -22,  -6, 0, 28, flat, unbanked }, { -23,  -7, 0, 28, flat, unbanked },
        { -24,  -8, 0, 28, flat, unbanked }, { -25,  -9, 0, 28, flat, unbanked }, { -26, -10, 0, 28, flat, unbanked }, { -27, -11, 0, 29, flat, unbanked }, { -28, -12, 0, 29, flat, unbanked },
        { -29, -13, 0, 29, flat, unbanked }, { -30, -14, 0, 29, flat, unbanked }, { -31, -15, 0, 29, flat, unbanked }, { -32, -15, 0, 29, flat, unbanked }, { -33, -16, 0, 29, flat, unbanked },
        { -34, -16, 0, 29, flat, unbanked }, { -35, -17, 0, 30, flat, unbanked }, { -36, -17, 0, 30, flat, unbanked }, { -37, -18, 0, 30, flat, unbanked }, { -38, -18, 0, 30, flat, unbanked },
        { -39, -19, 0, 30, flat, unbanked }, { -40, -19, 0, 30, flat, unbanked }, { -41, -20, 0, 30, flat, unbanked }, { -42, -20, 0, 30, flat, unbanked }, { -43, -20, 0, 30, flat, unbanked },
        { -44, -21, 0, 31, flat, unbanked }, { -45, -21, 0, 31, flat, unbanked }, { -46, -21, 0, 31, flat, unbanked }, { -47, -21, 0, 31, flat, unbanked }, { -48, -22, 0, 31, flat, unbanked },
        { -49, -22, 0, 31, flat, unbanked }, { -50, -22, 0, 31, flat, unbanked }, { -51, -22, 0, 31, flat, unbanked }, { -52, -22, 0, 31, flat, unbanked }, { -53, -23, 0, 31, flat, unbanked },
        { -54, -23, 0, 31, flat, unbanked }, { -55, -23, 0, 31, flat, unbanked }, { -56, -23, 0, 31, flat, unbanked }, { -57, -23, 0,  0, flat, unbanked }, { -58, -23, 0,  0, flat, unbanked },
        { -59, -23, 0,  0, flat, unbanked }, { -60, -23, 0,  0, flat, unbanked }, { -61, -23, 0,  0, flat, unbanked }, { -62, -23, 0,  0, flat, unbanked }, { -63, -23, 0,  0, flat, unbanked },
        { -64, -23, 0,  0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(SBendLeft_1, {
        {  23,  0, 0, 8, flat, unbanked }, {  23,  1, 0, 8, flat, unbanked }, {  23,  2, 0, 8, flat, unbanked }, {  23,  3, 0, 8, flat, unbanked }, {  23,  4, 0, 8, flat, unbanked },
        {  23,  5, 0, 8, flat, unbanked }, {  23,  6, 0, 8, flat, unbanked }, {  23,  7, 0, 7, flat, unbanked }, {  23,  8, 0, 7, flat, unbanked }, {  23,  9, 0, 7, flat, unbanked },
        {  23, 10, 0, 7, flat, unbanked }, {  22, 11, 0, 7, flat, unbanked }, {  22, 12, 0, 7, flat, unbanked }, {  22, 13, 0, 7, flat, unbanked }, {  22, 14, 0, 7, flat, unbanked },
        {  22, 15, 0, 7, flat, unbanked }, {  21, 16, 0, 7, flat, unbanked }, {  21, 17, 0, 7, flat, unbanked }, {  21, 18, 0, 7, flat, unbanked }, {  21, 19, 0, 7, flat, unbanked },
        {  20, 20, 0, 6, flat, unbanked }, {  20, 21, 0, 6, flat, unbanked }, {  20, 22, 0, 6, flat, unbanked }, {  19, 23, 0, 6, flat, unbanked }, {  19, 24, 0, 6, flat, unbanked },
        {  18, 25, 0, 6, flat, unbanked }, {  18, 26, 0, 6, flat, unbanked }, {  17, 27, 0, 6, flat, unbanked }, {  17, 28, 0, 6, flat, unbanked }, {  16, 29, 0, 5, flat, unbanked },
        {  16, 30, 0, 5, flat, unbanked }, {  15, 31, 0, 5, flat, unbanked }, {  15, 32, 0, 5, flat, unbanked }, {  14, 33, 0, 5, flat, unbanked }, {  13, 34, 0, 5, flat, unbanked },
        {  13, 35, 0, 5, flat, unbanked }, {  12, 36, 0, 5, flat, unbanked }, {  11, 37, 0, 4, flat, unbanked }, {  10, 38, 0, 4, flat, unbanked }, {   9, 39, 0, 4, flat, unbanked },
        {   8, 40, 0, 4, flat, unbanked }, {   7, 41, 0, 4, flat, unbanked }, {   6, 42, 0, 4, flat, unbanked }, {   5, 43, 0, 4, flat, unbanked }, {   4, 44, 0, 4, flat, unbanked },
        {   3, 45, 0, 4, flat, unbanked }, {   2, 46, 0, 4, flat, unbanked }, {   1, 47, 0, 4, flat, unbanked }, {   0, 48, 0, 4, flat, unbanked }, {  -1, 49, 0, 4, flat, unbanked },
        {  -2, 50, 0, 4, flat, unbanked }, {  -3, 51, 0, 4, flat, unbanked }, {  -4, 52, 0, 4, flat, unbanked }, {  -5, 53, 0, 4, flat, unbanked }, {  -6, 54, 0, 4, flat, unbanked },
        {  -7, 55, 0, 4, flat, unbanked }, {  -8, 56, 0, 4, flat, unbanked }, {  -9, 57, 0, 4, flat, unbanked }, { -10, 58, 0, 4, flat, unbanked }, { -11, 59, 0, 5, flat, unbanked },
        { -12, 60, 0, 5, flat, unbanked }, { -13, 61, 0, 5, flat, unbanked }, { -14, 62, 0, 5, flat, unbanked }, { -15, 63, 0, 5, flat, unbanked }, { -15, 64, 0, 5, flat, unbanked },
        { -16, 65, 0, 5, flat, unbanked }, { -16, 66, 0, 5, flat, unbanked }, { -17, 67, 0, 6, flat, unbanked }, { -17, 68, 0, 6, flat, unbanked }, { -18, 69, 0, 6, flat, unbanked },
        { -18, 70, 0, 6, flat, unbanked }, { -19, 71, 0, 6, flat, unbanked }, { -19, 72, 0, 6, flat, unbanked }, { -20, 73, 0, 6, flat, unbanked }, { -20, 74, 0, 6, flat, unbanked },
        { -20, 75, 0, 6, flat, unbanked }, { -21, 76, 0, 7, flat, unbanked }, { -21, 77, 0, 7, flat, unbanked }, { -21, 78, 0, 7, flat, unbanked }, { -21, 79, 0, 7, flat, unbanked },
        { -22, 80, 0, 7, flat, unbanked }, { -22, 81, 0, 7, flat, unbanked }, { -22, 82, 0, 7, flat, unbanked }, { -22, 83, 0, 7, flat, unbanked }, { -22, 84, 0, 7, flat, unbanked },
        { -23, 85, 0, 7, flat, unbanked }, { -23, 86, 0, 7, flat, unbanked }, { -23, 87, 0, 7, flat, unbanked }, { -23, 88, 0, 7, flat, unbanked }, { -23, 89, 0, 8, flat, unbanked },
        { -23, 90, 0, 8, flat, unbanked }, { -23, 91, 0, 8, flat, unbanked }, { -23, 92, 0, 8, flat, unbanked }, { -23, 93, 0, 8, flat, unbanked }, { -23, 94, 0, 8, flat, unbanked },
        { -23, 95, 0, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(SBendLeft_2, {
        {  0,  9, 0, 16, flat, unbanked }, {  1,  9, 0, 16, flat, unbanked }, {  2,  9, 0, 16, flat, unbanked }, {  3,  9, 0, 16, flat, unbanked }, {  4,  9, 0, 16, flat, unbanked },
        {  5,  9, 0, 16, flat, unbanked }, {  6,  9, 0, 16, flat, unbanked }, {  7,  9, 0, 15, flat, unbanked }, {  8,  9, 0, 15, flat, unbanked }, {  9,  9, 0, 15, flat, unbanked },
        { 10,  9, 0, 15, flat, unbanked }, { 11, 10, 0, 15, flat, unbanked }, { 12, 10, 0, 15, flat, unbanked }, { 13, 10, 0, 15, flat, unbanked }, { 14, 10, 0, 15, flat, unbanked },
        { 15, 10, 0, 15, flat, unbanked }, { 16, 11, 0, 15, flat, unbanked }, { 17, 11, 0, 15, flat, unbanked }, { 18, 11, 0, 15, flat, unbanked }, { 19, 11, 0, 15, flat, unbanked },
        { 20, 12, 0, 14, flat, unbanked }, { 21, 12, 0, 14, flat, unbanked }, { 22, 12, 0, 14, flat, unbanked }, { 23, 13, 0, 14, flat, unbanked }, { 24, 13, 0, 14, flat, unbanked },
        { 25, 14, 0, 14, flat, unbanked }, { 26, 14, 0, 14, flat, unbanked }, { 27, 15, 0, 14, flat, unbanked }, { 28, 15, 0, 14, flat, unbanked }, { 29, 16, 0, 13, flat, unbanked },
        { 30, 16, 0, 13, flat, unbanked }, { 31, 17, 0, 13, flat, unbanked }, { 32, 17, 0, 13, flat, unbanked }, { 33, 18, 0, 13, flat, unbanked }, { 34, 19, 0, 13, flat, unbanked },
        { 35, 19, 0, 13, flat, unbanked }, { 36, 20, 0, 13, flat, unbanked }, { 37, 21, 0, 12, flat, unbanked }, { 38, 22, 0, 12, flat, unbanked }, { 39, 23, 0, 12, flat, unbanked },
        { 40, 24, 0, 12, flat, unbanked }, { 41, 25, 0, 12, flat, unbanked }, { 42, 26, 0, 12, flat, unbanked }, { 43, 27, 0, 12, flat, unbanked }, { 44, 28, 0, 12, flat, unbanked },
        { 45, 29, 0, 12, flat, unbanked }, { 46, 30, 0, 12, flat, unbanked }, { 47, 31, 0, 12, flat, unbanked }, { 48, 32, 0, 12, flat, unbanked }, { 49, 33, 0, 12, flat, unbanked },
        { 50, 34, 0, 12, flat, unbanked }, { 51, 35, 0, 12, flat, unbanked }, { 52, 36, 0, 12, flat, unbanked }, { 53, 37, 0, 12, flat, unbanked }, { 54, 38, 0, 12, flat, unbanked },
        { 55, 39, 0, 12, flat, unbanked }, { 56, 40, 0, 12, flat, unbanked }, { 57, 41, 0, 12, flat, unbanked }, { 58, 42, 0, 12, flat, unbanked }, { 59, 43, 0, 13, flat, unbanked },
        { 60, 44, 0, 13, flat, unbanked }, { 61, 45, 0, 13, flat, unbanked }, { 62, 46, 0, 13, flat, unbanked }, { 63, 47, 0, 13, flat, unbanked }, { 64, 47, 0, 13, flat, unbanked },
        { 65, 48, 0, 13, flat, unbanked }, { 66, 48, 0, 13, flat, unbanked }, { 67, 49, 0, 14, flat, unbanked }, { 68, 49, 0, 14, flat, unbanked }, { 69, 50, 0, 14, flat, unbanked },
        { 70, 50, 0, 14, flat, unbanked }, { 71, 51, 0, 14, flat, unbanked }, { 72, 51, 0, 14, flat, unbanked }, { 73, 52, 0, 14, flat, unbanked }, { 74, 52, 0, 14, flat, unbanked },
        { 75, 52, 0, 14, flat, unbanked }, { 76, 53, 0, 15, flat, unbanked }, { 77, 53, 0, 15, flat, unbanked }, { 78, 53, 0, 15, flat, unbanked }, { 79, 53, 0, 15, flat, unbanked },
        { 80, 54, 0, 15, flat, unbanked }, { 81, 54, 0, 15, flat, unbanked }, { 82, 54, 0, 15, flat, unbanked }, { 83, 54, 0, 15, flat, unbanked }, { 84, 54, 0, 15, flat, unbanked },
        { 85, 55, 0, 15, flat, unbanked }, { 86, 55, 0, 15, flat, unbanked }, { 87, 55, 0, 15, flat, unbanked }, { 88, 55, 0, 15, flat, unbanked }, { 89, 55, 0, 16, flat, unbanked },
        { 90, 55, 0, 16, flat, unbanked }, { 91, 55, 0, 16, flat, unbanked }, { 92, 55, 0, 16, flat, unbanked }, { 93, 55, 0, 16, flat, unbanked }, { 94, 55, 0, 16, flat, unbanked },
        { 95, 55, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(SBendLeft_3, {
        {  9,  31, 0, 24, flat, unbanked }, {  9,  30, 0, 24, flat, unbanked }, {  9,  29, 0, 24, flat, unbanked }, {  9,  28, 0, 24, flat, unbanked }, {  9,  27, 0, 24, flat, unbanked },
        {  9,  26, 0, 24, flat, unbanked }, {  9,  25, 0, 23, flat, unbanked }, {  9,  24, 0, 23, flat, unbanked }, {  9,  23, 0, 23, flat, unbanked }, {  9,  22, 0, 23, flat, unbanked },
        { 10,  21, 0, 23, flat, unbanked }, { 10,  20, 0, 23, flat, unbanked }, { 10,  19, 0, 23, flat, unbanked }, { 10,  18, 0, 23, flat, unbanked }, { 10,  17, 0, 23, flat, unbanked },
        { 11,  16, 0, 23, flat, unbanked }, { 11,  15, 0, 23, flat, unbanked }, { 11,  14, 0, 23, flat, unbanked }, { 11,  13, 0, 23, flat, unbanked }, { 12,  12, 0, 22, flat, unbanked },
        { 12,  11, 0, 22, flat, unbanked }, { 12,  10, 0, 22, flat, unbanked }, { 13,   9, 0, 22, flat, unbanked }, { 13,   8, 0, 22, flat, unbanked }, { 14,   7, 0, 22, flat, unbanked },
        { 14,   6, 0, 22, flat, unbanked }, { 15,   5, 0, 22, flat, unbanked }, { 15,   4, 0, 22, flat, unbanked }, { 16,   3, 0, 21, flat, unbanked }, { 16,   2, 0, 21, flat, unbanked },
        { 17,   1, 0, 21, flat, unbanked }, { 17,   0, 0, 21, flat, unbanked }, { 18,  -1, 0, 21, flat, unbanked }, { 19,  -2, 0, 21, flat, unbanked }, { 19,  -3, 0, 21, flat, unbanked },
        { 20,  -4, 0, 21, flat, unbanked }, { 21,  -5, 0, 20, flat, unbanked }, { 22,  -6, 0, 20, flat, unbanked }, { 23,  -7, 0, 20, flat, unbanked }, { 24,  -8, 0, 20, flat, unbanked },
        { 25,  -9, 0, 20, flat, unbanked }, { 26, -10, 0, 20, flat, unbanked }, { 27, -11, 0, 20, flat, unbanked }, { 28, -12, 0, 20, flat, unbanked }, { 29, -13, 0, 20, flat, unbanked },
        { 30, -14, 0, 20, flat, unbanked }, { 31, -15, 0, 20, flat, unbanked }, { 32, -16, 0, 20, flat, unbanked }, { 33, -17, 0, 20, flat, unbanked }, { 34, -18, 0, 20, flat, unbanked },
        { 35, -19, 0, 20, flat, unbanked }, { 36, -20, 0, 20, flat, unbanked }, { 37, -21, 0, 20, flat, unbanked }, { 38, -22, 0, 20, flat, unbanked }, { 39, -23, 0, 20, flat, unbanked },
        { 40, -24, 0, 20, flat, unbanked }, { 41, -25, 0, 20, flat, unbanked }, { 42, -26, 0, 20, flat, unbanked }, { 43, -27, 0, 21, flat, unbanked }, { 44, -28, 0, 21, flat, unbanked },
        { 45, -29, 0, 21, flat, unbanked }, { 46, -30, 0, 21, flat, unbanked }, { 47, -31, 0, 21, flat, unbanked }, { 47, -32, 0, 21, flat, unbanked }, { 48, -33, 0, 21, flat, unbanked },
        { 48, -34, 0, 21, flat, unbanked }, { 49, -35, 0, 22, flat, unbanked }, { 49, -36, 0, 22, flat, unbanked }, { 50, -37, 0, 22, flat, unbanked }, { 50, -38, 0, 22, flat, unbanked },
        { 51, -39, 0, 22, flat, unbanked }, { 51, -40, 0, 22, flat, unbanked }, { 52, -41, 0, 22, flat, unbanked }, { 52, -42, 0, 22, flat, unbanked }, { 52, -43, 0, 22, flat, unbanked },
        { 53, -44, 0, 23, flat, unbanked }, { 53, -45, 0, 23, flat, unbanked }, { 53, -46, 0, 23, flat, unbanked }, { 53, -47, 0, 23, flat, unbanked }, { 54, -48, 0, 23, flat, unbanked },
        { 54, -49, 0, 23, flat, unbanked }, { 54, -50, 0, 23, flat, unbanked }, { 54, -51, 0, 23, flat, unbanked }, { 54, -52, 0, 23, flat, unbanked }, { 55, -53, 0, 23, flat, unbanked },
        { 55, -54, 0, 23, flat, unbanked }, { 55, -55, 0, 23, flat, unbanked }, { 55, -56, 0, 23, flat, unbanked }, { 55, -57, 0, 24, flat, unbanked }, { 55, -58, 0, 24, flat, unbanked },
        { 55, -59, 0, 24, flat, unbanked }, { 55, -60, 0, 24, flat, unbanked }, { 55, -61, 0, 24, flat, unbanked }, { 55, -62, 0, 24, flat, unbanked }, { 55, -63, 0, 24, flat, unbanked },
        { 55, -64, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(SBendRight_0, {
        {  31, 23, 0, 0, flat, unbanked }, {  30, 23, 0, 0, flat, unbanked }, {  29, 23, 0, 0, flat, unbanked }, {  28, 23, 0, 0, flat, unbanked }, {  27, 23, 0, 0, flat, unbanked },
        {  26, 23, 0, 0, flat, unbanked }, {  25, 23, 0, 0, flat, unbanked }, {  24, 23, 0, 0, flat, unbanked }, {  23, 23, 0, 0, flat, unbanked }, {  22, 23, 0, 0, flat, unbanked },
        {  21, 23, 0, 0, flat, unbanked }, {  20, 23, 0, 0, flat, unbanked }, {  19, 23, 0, 0, flat, unbanked }, {  18, 23, 0, 0, flat, unbanked }, {  17, 23, 0, 0, flat, unbanked },
        {  16, 24, 0, 0, flat, unbanked }, {  15, 24, 0, 0, flat, unbanked }, {  14, 24, 0, 0, flat, unbanked }, {  13, 24, 0, 0, flat, unbanked }, {  12, 24, 0, 0, flat, unbanked },
        {  11, 24, 0, 0, flat, unbanked }, {  10, 24, 0, 1, flat, unbanked }, {   9, 24, 0, 1, flat, unbanked }, {   8, 24, 0, 1, flat, unbanked }, {   7, 24, 0, 1, flat, unbanked },
        {   6, 25, 0, 1, flat, unbanked }, {   5, 25, 0, 1, flat, unbanked }, {   4, 25, 0, 1, flat, unbanked }, {   3, 25, 0, 1, flat, unbanked }, {   2, 25, 0, 1, flat, unbanked },
        {   1, 26, 0, 1, flat, unbanked }, {   0, 26, 0, 1, flat, unbanked }, {  -1, 26, 0, 1, flat, unbanked }, {  -2, 26, 0, 2, flat, unbanked }, {  -3, 27, 0, 2, flat, unbanked },
        {  -4, 27, 0, 2, flat, unbanked }, {  -5, 27, 0, 2, flat, unbanked }, {  -6, 28, 0, 2, flat, unbanked }, {  -7, 28, 0, 2, flat, unbanked }, {  -8, 29, 0, 2, flat, unbanked },
        {  -9, 29, 0, 2, flat, unbanked }, { -10, 29, 0, 2, flat, unbanked }, { -11, 30, 0, 2, flat, unbanked }, { -12, 30, 0, 2, flat, unbanked }, { -13, 31, 0, 2, flat, unbanked },
        { -14, 31, 0, 2, flat, unbanked }, { -15, 32, 0, 2, flat, unbanked }, { -16, 32, 0, 2, flat, unbanked }, { -17, 33, 0, 2, flat, unbanked }, { -18, 33, 0, 2, flat, unbanked },
        { -19, 34, 0, 2, flat, unbanked }, { -20, 34, 0, 2, flat, unbanked }, { -21, 35, 0, 2, flat, unbanked }, { -22, 35, 0, 2, flat, unbanked }, { -23, 35, 0, 2, flat, unbanked },
        { -24, 36, 0, 2, flat, unbanked }, { -25, 36, 0, 2, flat, unbanked }, { -26, 37, 0, 2, flat, unbanked }, { -27, 37, 0, 2, flat, unbanked }, { -28, 37, 0, 2, flat, unbanked },
        { -29, 38, 0, 2, flat, unbanked }, { -30, 38, 0, 1, flat, unbanked }, { -31, 38, 0, 1, flat, unbanked }, { -32, 38, 0, 1, flat, unbanked }, { -33, 39, 0, 1, flat, unbanked },
        { -34, 39, 0, 1, flat, unbanked }, { -35, 39, 0, 1, flat, unbanked }, { -36, 39, 0, 1, flat, unbanked }, { -37, 39, 0, 1, flat, unbanked }, { -38, 40, 0, 1, flat, unbanked },
        { -39, 40, 0, 1, flat, unbanked }, { -40, 40, 0, 1, flat, unbanked }, { -41, 40, 0, 1, flat, unbanked }, { -42, 40, 0, 0, flat, unbanked }, { -43, 40, 0, 0, flat, unbanked },
        { -44, 40, 0, 0, flat, unbanked }, { -45, 40, 0, 0, flat, unbanked }, { -46, 40, 0, 0, flat, unbanked }, { -47, 40, 0, 0, flat, unbanked }, { -48, 41, 0, 0, flat, unbanked },
        { -49, 41, 0, 0, flat, unbanked }, { -50, 41, 0, 0, flat, unbanked }, { -51, 41, 0, 0, flat, unbanked }, { -52, 41, 0, 0, flat, unbanked }, { -53, 41, 0, 0, flat, unbanked },
        { -54, 41, 0, 0, flat, unbanked }, { -55, 41, 0, 0, flat, unbanked }, { -56, 41, 0, 0, flat, unbanked }, { -57, 41, 0, 0, flat, unbanked }, { -58, 41, 0, 0, flat, unbanked },
        { -59, 41, 0, 0, flat, unbanked }, { -60, 41, 0, 0, flat, unbanked }, { -61, 41, 0, 0, flat, unbanked }, { -62, 41, 0, 0, flat, unbanked }, { -63, 41, 0, 0, flat, unbanked },
        { -64, 41, 0, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(SBendRight_1, {
        { 23,  0, 0,  8, flat, unbanked }, { 23,  1, 0,  8, flat, unbanked }, { 23,  2, 0,  8, flat, unbanked }, { 23,  3, 0,  8, flat, unbanked }, { 23,  4, 0,  8, flat, unbanked },
        { 23,  5, 0,  8, flat, unbanked }, { 23,  6, 0,  8, flat, unbanked }, { 23,  7, 0,  8, flat, unbanked }, { 23,  8, 0,  8, flat, unbanked }, { 23,  9, 0,  8, flat, unbanked },
        { 23, 10, 0,  8, flat, unbanked }, { 23, 11, 0,  8, flat, unbanked }, { 23, 12, 0,  8, flat, unbanked }, { 23, 13, 0,  8, flat, unbanked }, { 23, 14, 0,  8, flat, unbanked },
        { 23, 15, 0,  8, flat, unbanked }, { 24, 16, 0,  8, flat, unbanked }, { 24, 17, 0,  8, flat, unbanked }, { 24, 18, 0,  8, flat, unbanked }, { 24, 19, 0,  8, flat, unbanked },
        { 24, 20, 0,  8, flat, unbanked }, { 24, 21, 0,  8, flat, unbanked }, { 24, 22, 0,  9, flat, unbanked }, { 24, 23, 0,  9, flat, unbanked }, { 24, 24, 0,  9, flat, unbanked },
        { 24, 25, 0,  9, flat, unbanked }, { 25, 26, 0,  9, flat, unbanked }, { 25, 27, 0,  9, flat, unbanked }, { 25, 28, 0,  9, flat, unbanked }, { 25, 29, 0,  9, flat, unbanked },
        { 25, 30, 0,  9, flat, unbanked }, { 26, 31, 0,  9, flat, unbanked }, { 26, 32, 0,  9, flat, unbanked }, { 26, 33, 0,  9, flat, unbanked }, { 26, 34, 0, 10, flat, unbanked },
        { 27, 35, 0, 10, flat, unbanked }, { 27, 36, 0, 10, flat, unbanked }, { 27, 37, 0, 10, flat, unbanked }, { 28, 38, 0, 10, flat, unbanked }, { 28, 39, 0, 10, flat, unbanked },
        { 29, 40, 0, 10, flat, unbanked }, { 29, 41, 0, 10, flat, unbanked }, { 29, 42, 0, 10, flat, unbanked }, { 30, 43, 0, 10, flat, unbanked }, { 30, 44, 0, 10, flat, unbanked },
        { 31, 45, 0, 10, flat, unbanked }, { 31, 46, 0, 10, flat, unbanked }, { 32, 47, 0, 10, flat, unbanked }, { 32, 48, 0, 10, flat, unbanked }, { 33, 49, 0, 10, flat, unbanked },
        { 33, 50, 0, 10, flat, unbanked }, { 34, 51, 0, 10, flat, unbanked }, { 34, 52, 0, 10, flat, unbanked }, { 35, 53, 0, 10, flat, unbanked }, { 35, 54, 0, 10, flat, unbanked },
        { 35, 55, 0, 10, flat, unbanked }, { 36, 56, 0, 10, flat, unbanked }, { 36, 57, 0, 10, flat, unbanked }, { 37, 58, 0, 10, flat, unbanked }, { 37, 59, 0, 10, flat, unbanked },
        { 37, 60, 0, 10, flat, unbanked }, { 38, 61, 0, 10, flat, unbanked }, { 38, 62, 0,  9, flat, unbanked }, { 38, 63, 0,  9, flat, unbanked }, { 38, 64, 0,  9, flat, unbanked },
        { 39, 65, 0,  9, flat, unbanked }, { 39, 66, 0,  9, flat, unbanked }, { 39, 67, 0,  9, flat, unbanked }, { 39, 68, 0,  9, flat, unbanked }, { 39, 69, 0,  9, flat, unbanked },
        { 40, 70, 0,  9, flat, unbanked }, { 40, 71, 0,  9, flat, unbanked }, { 40, 72, 0,  9, flat, unbanked }, { 40, 73, 0,  9, flat, unbanked }, { 40, 74, 0,  8, flat, unbanked },
        { 40, 75, 0,  8, flat, unbanked }, { 40, 76, 0,  8, flat, unbanked }, { 40, 77, 0,  8, flat, unbanked }, { 40, 78, 0,  8, flat, unbanked }, { 40, 79, 0,  8, flat, unbanked },
        { 41, 80, 0,  8, flat, unbanked }, { 41, 81, 0,  8, flat, unbanked }, { 41, 82, 0,  8, flat, unbanked }, { 41, 83, 0,  8, flat, unbanked }, { 41, 84, 0,  8, flat, unbanked },
        { 41, 85, 0,  8, flat, unbanked }, { 41, 86, 0,  8, flat, unbanked }, { 41, 87, 0,  8, flat, unbanked }, { 41, 88, 0,  8, flat, unbanked }, { 41, 89, 0,  8, flat, unbanked },
        { 41, 90, 0,  8, flat, unbanked }, { 41, 91, 0,  8, flat, unbanked }, { 41, 92, 0,  8, flat, unbanked }, { 41, 93, 0,  8, flat, unbanked }, { 41, 94, 0,  8, flat, unbanked },
        { 41, 95, 0,  8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(SBendRight_2, {
        {  0,  9, 0, 16, flat, unbanked }, {  1,  9, 0, 16, flat, unbanked }, {  2,  9, 0, 16, flat, unbanked }, {  3,  9, 0, 16, flat, unbanked }, {  4,  9, 0, 16, flat, unbanked },
        {  5,  9, 0, 16, flat, unbanked }, {  6,  9, 0, 16, flat, unbanked }, {  7,  9, 0, 16, flat, unbanked }, {  8,  9, 0, 16, flat, unbanked }, {  9,  9, 0, 16, flat, unbanked },
        { 10,  9, 0, 16, flat, unbanked }, { 11,  9, 0, 16, flat, unbanked }, { 12,  9, 0, 16, flat, unbanked }, { 13,  9, 0, 16, flat, unbanked }, { 14,  9, 0, 16, flat, unbanked },
        { 15,  9, 0, 16, flat, unbanked }, { 16,  8, 0, 16, flat, unbanked }, { 17,  8, 0, 16, flat, unbanked }, { 18,  8, 0, 16, flat, unbanked }, { 19,  8, 0, 16, flat, unbanked },
        { 20,  8, 0, 16, flat, unbanked }, { 21,  8, 0, 16, flat, unbanked }, { 22,  8, 0, 17, flat, unbanked }, { 23,  8, 0, 17, flat, unbanked }, { 24,  8, 0, 17, flat, unbanked },
        { 25,  8, 0, 17, flat, unbanked }, { 26,  7, 0, 17, flat, unbanked }, { 27,  7, 0, 17, flat, unbanked }, { 28,  7, 0, 17, flat, unbanked }, { 29,  7, 0, 17, flat, unbanked },
        { 30,  7, 0, 17, flat, unbanked }, { 31,  6, 0, 17, flat, unbanked }, { 32,  6, 0, 17, flat, unbanked }, { 33,  6, 0, 17, flat, unbanked }, { 34,  6, 0, 18, flat, unbanked },
        { 35,  5, 0, 18, flat, unbanked }, { 36,  5, 0, 18, flat, unbanked }, { 37,  5, 0, 18, flat, unbanked }, { 38,  4, 0, 18, flat, unbanked }, { 39,  4, 0, 18, flat, unbanked },
        { 40,  3, 0, 18, flat, unbanked }, { 41,  3, 0, 18, flat, unbanked }, { 42,  3, 0, 18, flat, unbanked }, { 43,  2, 0, 18, flat, unbanked }, { 44,  2, 0, 18, flat, unbanked },
        { 45,  1, 0, 18, flat, unbanked }, { 46,  1, 0, 18, flat, unbanked }, { 47,  0, 0, 18, flat, unbanked }, { 48,  0, 0, 18, flat, unbanked }, { 49, -1, 0, 18, flat, unbanked },
        { 50, -1, 0, 18, flat, unbanked }, { 51, -2, 0, 18, flat, unbanked }, { 52, -2, 0, 18, flat, unbanked }, { 53, -3, 0, 18, flat, unbanked }, { 54, -3, 0, 18, flat, unbanked },
        { 55, -3, 0, 18, flat, unbanked }, { 56, -4, 0, 18, flat, unbanked }, { 57, -4, 0, 18, flat, unbanked }, { 58, -5, 0, 18, flat, unbanked }, { 59, -5, 0, 18, flat, unbanked },
        { 60, -5, 0, 18, flat, unbanked }, { 61, -6, 0, 18, flat, unbanked }, { 62, -6, 0, 17, flat, unbanked }, { 63, -6, 0, 17, flat, unbanked }, { 64, -6, 0, 17, flat, unbanked },
        { 65, -7, 0, 17, flat, unbanked }, { 66, -7, 0, 17, flat, unbanked }, { 67, -7, 0, 17, flat, unbanked }, { 68, -7, 0, 17, flat, unbanked }, { 69, -7, 0, 17, flat, unbanked },
        { 70, -8, 0, 17, flat, unbanked }, { 71, -8, 0, 17, flat, unbanked }, { 72, -8, 0, 17, flat, unbanked }, { 73, -8, 0, 17, flat, unbanked }, { 74, -8, 0, 16, flat, unbanked },
        { 75, -8, 0, 16, flat, unbanked }, { 76, -8, 0, 16, flat, unbanked }, { 77, -8, 0, 16, flat, unbanked }, { 78, -8, 0, 16, flat, unbanked }, { 79, -8, 0, 16, flat, unbanked },
        { 80, -9, 0, 16, flat, unbanked }, { 81, -9, 0, 16, flat, unbanked }, { 82, -9, 0, 16, flat, unbanked }, { 83, -9, 0, 16, flat, unbanked }, { 84, -9, 0, 16, flat, unbanked },
        { 85, -9, 0, 16, flat, unbanked }, { 86, -9, 0, 16, flat, unbanked }, { 87, -9, 0, 16, flat, unbanked }, { 88, -9, 0, 16, flat, unbanked }, { 89, -9, 0, 16, flat, unbanked },
        { 90, -9, 0, 16, flat, unbanked }, { 91, -9, 0, 16, flat, unbanked }, { 92, -9, 0, 16, flat, unbanked }, { 93, -9, 0, 16, flat, unbanked }, { 94, -9, 0, 16, flat, unbanked },
        { 95, -9, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(SBendRight_3, {
        {  9,  31, 0, 24, flat, unbanked }, {  9,  30, 0, 24, flat, unbanked }, {  9,  29, 0, 24, flat, unbanked }, {  9,  28, 0, 24, flat, unbanked }, {  9,  27, 0, 24, flat, unbanked },
        {  9,  26, 0, 24, flat, unbanked }, {  9,  25, 0, 24, flat, unbanked }, {  9,  24, 0, 24, flat, unbanked }, {  9,  23, 0, 24, flat, unbanked }, {  9,  22, 0, 24, flat, unbanked },
        {  9,  21, 0, 24, flat, unbanked }, {  9,  20, 0, 24, flat, unbanked }, {  9,  19, 0, 24, flat, unbanked }, {  9,  18, 0, 24, flat, unbanked }, {  9,  17, 0, 24, flat, unbanked },
        {  8,  16, 0, 24, flat, unbanked }, {  8,  15, 0, 24, flat, unbanked }, {  8,  14, 0, 24, flat, unbanked }, {  8,  13, 0, 24, flat, unbanked }, {  8,  12, 0, 24, flat, unbanked },
        {  8,  11, 0, 24, flat, unbanked }, {  8,  10, 0, 25, flat, unbanked }, {  8,   9, 0, 25, flat, unbanked }, {  8,   8, 0, 25, flat, unbanked }, {  8,   7, 0, 25, flat, unbanked },
        {  7,   6, 0, 25, flat, unbanked }, {  7,   5, 0, 25, flat, unbanked }, {  7,   4, 0, 25, flat, unbanked }, {  7,   3, 0, 25, flat, unbanked }, {  7,   2, 0, 25, flat, unbanked },
        {  6,   1, 0, 25, flat, unbanked }, {  6,   0, 0, 25, flat, unbanked }, {  6,  -1, 0, 25, flat, unbanked }, {  6,  -2, 0, 26, flat, unbanked }, {  5,  -3, 0, 26, flat, unbanked },
        {  5,  -4, 0, 26, flat, unbanked }, {  5,  -5, 0, 26, flat, unbanked }, {  4,  -6, 0, 26, flat, unbanked }, {  4,  -7, 0, 26, flat, unbanked }, {  3,  -8, 0, 26, flat, unbanked },
        {  3,  -9, 0, 26, flat, unbanked }, {  3, -10, 0, 26, flat, unbanked }, {  2, -11, 0, 26, flat, unbanked }, {  2, -12, 0, 26, flat, unbanked }, {  1, -13, 0, 26, flat, unbanked },
        {  1, -14, 0, 26, flat, unbanked }, {  0, -15, 0, 26, flat, unbanked }, {  0, -16, 0, 26, flat, unbanked }, { -1, -17, 0, 26, flat, unbanked }, { -1, -18, 0, 26, flat, unbanked },
        { -2, -19, 0, 26, flat, unbanked }, { -2, -20, 0, 26, flat, unbanked }, { -3, -21, 0, 26, flat, unbanked }, { -3, -22, 0, 26, flat, unbanked }, { -3, -23, 0, 26, flat, unbanked },
        { -4, -24, 0, 26, flat, unbanked }, { -4, -25, 0, 26, flat, unbanked }, { -5, -26, 0, 26, flat, unbanked }, { -5, -27, 0, 26, flat, unbanked }, { -5, -28, 0, 26, flat, unbanked },
        { -6, -29, 0, 26, flat, unbanked }, { -6, -30, 0, 25, flat, unbanked }, { -6, -31, 0, 25, flat, unbanked }, { -6, -32, 0, 25, flat, unbanked }, { -7, -33, 0, 25, flat, unbanked },
        { -7, -34, 0, 25, flat, unbanked }, { -7, -35, 0, 25, flat, unbanked }, { -7, -36, 0, 25, flat, unbanked }, { -7, -37, 0, 25, flat, unbanked }, { -8, -38, 0, 25, flat, unbanked },
        { -8, -39, 0, 25, flat, unbanked }, { -8, -40, 0, 25, flat, unbanked }, { -8, -41, 0, 25, flat, unbanked }, { -8, -42, 0, 24, flat, unbanked }, { -8, -43, 0, 24, flat, unbanked },
        { -8, -44, 0, 24, flat, unbanked }, { -8, -45, 0, 24, flat, unbanked }, { -8, -46, 0, 24, flat, unbanked }, { -8, -47, 0, 24, flat, unbanked }, { -9, -48, 0, 24, flat, unbanked },
        { -9, -49, 0, 24, flat, unbanked }, { -9, -50, 0, 24, flat, unbanked }, { -9, -51, 0, 24, flat, unbanked }, { -9, -52, 0, 24, flat, unbanked }, { -9, -53, 0, 24, flat, unbanked },
        { -9, -54, 0, 24, flat, unbanked }, { -9, -55, 0, 24, flat, unbanked }, { -9, -56, 0, 24, flat, unbanked }, { -9, -57, 0, 24, flat, unbanked }, { -9, -58, 0, 24, flat, unbanked },
        { -9, -59, 0, 24, flat, unbanked }, { -9, -60, 0, 24, flat, unbanked }, { -9, -61, 0, 24, flat, unbanked }, { -9, -62, 0, 24, flat, unbanked }, { -9, -63, 0, 24, flat, unbanked },
        { -9, -64, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn3Tiles_0, {
        { 31,  23, 0,  0, flat, unbanked }, { 30,  23, 0,  0, flat, unbanked }, { 29,  23, 0,  0, flat, unbanked }, { 28,  23, 0, 31, flat, unbanked }, { 27,  23, 0, 31, flat, unbanked },
        { 26,  23, 0, 31, flat, unbanked }, { 25,  23, 0, 31, flat, unbanked }, { 24,  22, 0, 31, flat, unbanked }, { 23,  22, 0, 31, flat, unbanked }, { 22,  22, 0, 31, flat, unbanked },
        { 21,  21, 0, 30, flat, unbanked }, { 20,  21, 0, 30, flat, unbanked }, { 19,  21, 0, 30, flat, unbanked }, { 18,  20, 0, 30, flat, unbanked }, { 17,  20, 0, 30, flat, unbanked },
        { 16,  19, 0, 30, flat, unbanked }, { 15,  19, 0, 29, flat, unbanked }, { 14,  18, 0, 29, flat, unbanked }, { 13,  18, 0, 29, flat, unbanked }, { 12,  17, 0, 29, flat, unbanked },
        { 11,  16, 0, 29, flat, unbanked }, { 10,  16, 0, 29, flat, unbanked }, {  9,  15, 0, 29, flat, unbanked }, {  8,  14, 0, 28, flat, unbanked }, {  7,  13, 0, 28, flat, unbanked },
        {  6,  12, 0, 28, flat, unbanked }, {  5,  11, 0, 28, flat, unbanked }, {  4,  10, 0, 28, flat, unbanked }, {  3,   9, 0, 27, flat, unbanked }, {  2,   8, 0, 27, flat, unbanked },
        {  1,   7, 0, 27, flat, unbanked }, {  0,   6, 0, 27, flat, unbanked }, {  0,   5, 0, 27, flat, unbanked }, { -1,   4, 0, 27, flat, unbanked }, { -2,   3, 0, 27, flat, unbanked },
        { -2,   2, 0, 26, flat, unbanked }, { -3,   1, 0, 26, flat, unbanked }, { -3,   0, 0, 26, flat, unbanked }, { -4,  -1, 0, 26, flat, unbanked }, { -4,  -2, 0, 26, flat, unbanked },
        { -4,  -3, 0, 26, flat, unbanked }, { -5,  -4, 0, 26, flat, unbanked }, { -5,  -5, 0, 26, flat, unbanked }, { -5,  -6, 0, 26, flat, unbanked }, { -6,  -7, 0, 25, flat, unbanked },
        { -6,  -8, 0, 25, flat, unbanked }, { -6,  -9, 0, 25, flat, unbanked }, { -7, -10, 0, 25, flat, unbanked }, { -7, -11, 0, 25, flat, unbanked }, { -7, -12, 0, 25, flat, unbanked },
        { -7, -13, 0, 25, flat, unbanked }, { -7, -14, 0, 25, flat, unbanked }, { -8, -15, 0, 25, flat, unbanked }, { -8, -16, 0, 25, flat, unbanked }, { -8, -17, 0, 25, flat, unbanked },
        { -8, -18, 0, 25, flat, unbanked }, { -8, -19, 0, 25, flat, unbanked }, { -8, -20, 0, 24, flat, unbanked }, { -8, -21, 0, 24, flat, unbanked }, { -8, -22, 0, 24, flat, unbanked },
        { -8, -23, 0, 24, flat, unbanked }, { -9, -24, 0, 24, flat, unbanked }, { -9, -25, 0, 24, flat, unbanked }, { -9, -26, 0, 24, flat, unbanked }, { -9, -27, 0, 24, flat, unbanked },
        { -9, -28, 0, 24, flat, unbanked }, { -9, -29, 0, 24, flat, unbanked }, { -9, -30, 0, 24, flat, unbanked }, { -9, -31, 0, 24, flat, unbanked }, { -9, -32, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn3Tiles_1, {
        {  23,  0, 0, 8, flat, unbanked }, {  23,  1, 0, 8, flat, unbanked }, {  23,  2, 0, 8, flat, unbanked }, {  23,  3, 0, 8, flat, unbanked }, {  23,  4, 0, 7, flat, unbanked },
        {  23,  5, 0, 7, flat, unbanked }, {  23,  6, 0, 7, flat, unbanked }, {  23,  7, 0, 7, flat, unbanked }, {  22,  8, 0, 7, flat, unbanked }, {  22,  9, 0, 7, flat, unbanked },
        {  22, 10, 0, 7, flat, unbanked }, {  21, 11, 0, 6, flat, unbanked }, {  21, 12, 0, 6, flat, unbanked }, {  21, 13, 0, 6, flat, unbanked }, {  20, 14, 0, 6, flat, unbanked },
        {  20, 15, 0, 6, flat, unbanked }, {  19, 16, 0, 6, flat, unbanked }, {  19, 17, 0, 5, flat, unbanked }, {  18, 18, 0, 5, flat, unbanked }, {  18, 19, 0, 5, flat, unbanked },
        {  17, 20, 0, 5, flat, unbanked }, {  16, 21, 0, 5, flat, unbanked }, {  16, 22, 0, 5, flat, unbanked }, {  15, 23, 0, 5, flat, unbanked }, {  14, 24, 0, 4, flat, unbanked },
        {  13, 25, 0, 4, flat, unbanked }, {  12, 26, 0, 4, flat, unbanked }, {  11, 27, 0, 4, flat, unbanked }, {  10, 28, 0, 4, flat, unbanked }, {   9, 29, 0, 3, flat, unbanked },
        {   8, 30, 0, 3, flat, unbanked }, {   7, 31, 0, 3, flat, unbanked }, {   6, 32, 0, 3, flat, unbanked }, {   5, 32, 0, 3, flat, unbanked }, {   4, 33, 0, 3, flat, unbanked },
        {   3, 34, 0, 3, flat, unbanked }, {   2, 34, 0, 2, flat, unbanked }, {   1, 35, 0, 2, flat, unbanked }, {   0, 35, 0, 2, flat, unbanked }, {  -1, 36, 0, 2, flat, unbanked },
        {  -2, 36, 0, 2, flat, unbanked }, {  -3, 36, 0, 2, flat, unbanked }, {  -4, 37, 0, 2, flat, unbanked }, {  -5, 37, 0, 2, flat, unbanked }, {  -6, 37, 0, 2, flat, unbanked },
        {  -7, 38, 0, 1, flat, unbanked }, {  -8, 38, 0, 1, flat, unbanked }, {  -9, 38, 0, 1, flat, unbanked }, { -10, 39, 0, 1, flat, unbanked }, { -11, 39, 0, 1, flat, unbanked },
        { -12, 39, 0, 1, flat, unbanked }, { -13, 39, 0, 1, flat, unbanked }, { -14, 39, 0, 1, flat, unbanked }, { -15, 40, 0, 1, flat, unbanked }, { -16, 40, 0, 1, flat, unbanked },
        { -17, 40, 0, 1, flat, unbanked }, { -18, 40, 0, 1, flat, unbanked }, { -19, 40, 0, 1, flat, unbanked }, { -20, 40, 0, 0, flat, unbanked }, { -21, 40, 0, 0, flat, unbanked },
        { -22, 40, 0, 0, flat, unbanked }, { -23, 40, 0, 0, flat, unbanked }, { -24, 41, 0, 0, flat, unbanked }, { -25, 41, 0, 0, flat, unbanked }, { -26, 41, 0, 0, flat, unbanked },
        { -27, 41, 0, 0, flat, unbanked }, { -28, 41, 0, 0, flat, unbanked }, { -29, 41, 0, 0, flat, unbanked }, { -30, 41, 0, 0, flat, unbanked }, { -31, 41, 0, 0, flat, unbanked },
        { -32, 41, 0, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn3Tiles_2, {
        {  0,  9, 0, 16, flat, unbanked }, {  1,  9, 0, 16, flat, unbanked }, {  2,  9, 0, 16, flat, unbanked }, {  3,  9, 0, 16, flat, unbanked }, {  4,  9, 0, 15, flat, unbanked },
        {  5,  9, 0, 15, flat, unbanked }, {  6,  9, 0, 15, flat, unbanked }, {  7,  9, 0, 15, flat, unbanked }, {  8, 10, 0, 15, flat, unbanked }, {  9, 10, 0, 15, flat, unbanked },
        { 10, 10, 0, 15, flat, unbanked }, { 11, 11, 0, 14, flat, unbanked }, { 12, 11, 0, 14, flat, unbanked }, { 13, 11, 0, 14, flat, unbanked }, { 14, 12, 0, 14, flat, unbanked },
        { 15, 12, 0, 14, flat, unbanked }, { 16, 13, 0, 14, flat, unbanked }, { 17, 13, 0, 13, flat, unbanked }, { 18, 14, 0, 13, flat, unbanked }, { 19, 14, 0, 13, flat, unbanked },
        { 20, 15, 0, 13, flat, unbanked }, { 21, 16, 0, 13, flat, unbanked }, { 22, 16, 0, 13, flat, unbanked }, { 23, 17, 0, 13, flat, unbanked }, { 24, 18, 0, 12, flat, unbanked },
        { 25, 19, 0, 12, flat, unbanked }, { 26, 20, 0, 12, flat, unbanked }, { 27, 21, 0, 12, flat, unbanked }, { 28, 22, 0, 12, flat, unbanked }, { 29, 23, 0, 11, flat, unbanked },
        { 30, 24, 0, 11, flat, unbanked }, { 31, 25, 0, 11, flat, unbanked }, { 32, 26, 0, 11, flat, unbanked }, { 32, 27, 0, 11, flat, unbanked }, { 33, 28, 0, 11, flat, unbanked },
        { 34, 29, 0, 11, flat, unbanked }, { 34, 30, 0, 10, flat, unbanked }, { 35, 31, 0, 10, flat, unbanked }, { 35, 32, 0, 10, flat, unbanked }, { 36, 33, 0, 10, flat, unbanked },
        { 36, 34, 0, 10, flat, unbanked }, { 36, 35, 0, 10, flat, unbanked }, { 37, 36, 0, 10, flat, unbanked }, { 37, 37, 0, 10, flat, unbanked }, { 37, 38, 0, 10, flat, unbanked },
        { 38, 39, 0,  9, flat, unbanked }, { 38, 40, 0,  9, flat, unbanked }, { 38, 41, 0,  9, flat, unbanked }, { 39, 42, 0,  9, flat, unbanked }, { 39, 43, 0,  9, flat, unbanked },
        { 39, 44, 0,  9, flat, unbanked }, { 39, 45, 0,  9, flat, unbanked }, { 39, 46, 0,  9, flat, unbanked }, { 40, 47, 0,  9, flat, unbanked }, { 40, 48, 0,  9, flat, unbanked },
        { 40, 49, 0,  9, flat, unbanked }, { 40, 50, 0,  9, flat, unbanked }, { 40, 51, 0,  9, flat, unbanked }, { 40, 52, 0,  8, flat, unbanked }, { 40, 53, 0,  8, flat, unbanked },
        { 40, 54, 0,  8, flat, unbanked }, { 40, 55, 0,  8, flat, unbanked }, { 41, 56, 0,  8, flat, unbanked }, { 41, 57, 0,  8, flat, unbanked }, { 41, 58, 0,  8, flat, unbanked },
        { 41, 59, 0,  8, flat, unbanked }, { 41, 60, 0,  8, flat, unbanked }, { 41, 61, 0,  8, flat, unbanked }, { 41, 62, 0,  8, flat, unbanked }, { 41, 63, 0,  8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn3Tiles_3, {
        {  9, 31, 0, 24, flat, unbanked }, {  9, 30, 0, 24, flat, unbanked }, {  9, 29, 0, 24, flat, unbanked }, {  9, 28, 0, 23, flat, unbanked }, {  9, 27, 0, 23, flat, unbanked },
        {  9, 26, 0, 23, flat, unbanked }, {  9, 25, 0, 23, flat, unbanked }, { 10, 24, 0, 23, flat, unbanked }, { 10, 23, 0, 23, flat, unbanked }, { 10, 22, 0, 23, flat, unbanked },
        { 11, 21, 0, 22, flat, unbanked }, { 11, 20, 0, 22, flat, unbanked }, { 11, 19, 0, 22, flat, unbanked }, { 12, 18, 0, 22, flat, unbanked }, { 12, 17, 0, 22, flat, unbanked },
        { 13, 16, 0, 22, flat, unbanked }, { 13, 15, 0, 21, flat, unbanked }, { 14, 14, 0, 21, flat, unbanked }, { 14, 13, 0, 21, flat, unbanked }, { 15, 12, 0, 21, flat, unbanked },
        { 16, 11, 0, 21, flat, unbanked }, { 16, 10, 0, 21, flat, unbanked }, { 17,  9, 0, 21, flat, unbanked }, { 18,  8, 0, 20, flat, unbanked }, { 19,  7, 0, 20, flat, unbanked },
        { 20,  6, 0, 20, flat, unbanked }, { 21,  5, 0, 20, flat, unbanked }, { 22,  4, 0, 20, flat, unbanked }, { 23,  3, 0, 19, flat, unbanked }, { 24,  2, 0, 19, flat, unbanked },
        { 25,  1, 0, 19, flat, unbanked }, { 26,  0, 0, 19, flat, unbanked }, { 27,  0, 0, 19, flat, unbanked }, { 28, -1, 0, 19, flat, unbanked }, { 29, -2, 0, 19, flat, unbanked },
        { 30, -2, 0, 18, flat, unbanked }, { 31, -3, 0, 18, flat, unbanked }, { 32, -3, 0, 18, flat, unbanked }, { 33, -4, 0, 18, flat, unbanked }, { 34, -4, 0, 18, flat, unbanked },
        { 35, -4, 0, 18, flat, unbanked }, { 36, -5, 0, 18, flat, unbanked }, { 37, -5, 0, 18, flat, unbanked }, { 38, -5, 0, 18, flat, unbanked }, { 39, -6, 0, 17, flat, unbanked },
        { 40, -6, 0, 17, flat, unbanked }, { 41, -6, 0, 17, flat, unbanked }, { 42, -7, 0, 17, flat, unbanked }, { 43, -7, 0, 17, flat, unbanked }, { 44, -7, 0, 17, flat, unbanked },
        { 45, -7, 0, 17, flat, unbanked }, { 46, -7, 0, 17, flat, unbanked }, { 47, -8, 0, 17, flat, unbanked }, { 48, -8, 0, 17, flat, unbanked }, { 49, -8, 0, 17, flat, unbanked },
        { 50, -8, 0, 17, flat, unbanked }, { 51, -8, 0, 17, flat, unbanked }, { 52, -8, 0, 16, flat, unbanked }, { 53, -8, 0, 16, flat, unbanked }, { 54, -8, 0, 16, flat, unbanked },
        { 55, -8, 0, 16, flat, unbanked }, { 56, -9, 0, 16, flat, unbanked }, { 57, -9, 0, 16, flat, unbanked }, { 58, -9, 0, 16, flat, unbanked }, { 59, -9, 0, 16, flat, unbanked },
        { 60, -9, 0, 16, flat, unbanked }, { 61, -9, 0, 16, flat, unbanked }, { 62, -9, 0, 16, flat, unbanked }, { 63, -9, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn3Tiles_0, {
        {  31, 23, 0, 0, flat, unbanked }, {  30, 23, 0, 0, flat, unbanked }, {  29, 23, 0, 0, flat, unbanked }, {  28, 23, 0, 0, flat, unbanked }, {  27, 23, 0, 0, flat, unbanked },
        {  26, 23, 0, 0, flat, unbanked }, {  25, 23, 0, 0, flat, unbanked }, {  24, 24, 0, 0, flat, unbanked }, {  23, 24, 0, 0, flat, unbanked }, {  22, 24, 0, 0, flat, unbanked },
        {  21, 24, 0, 0, flat, unbanked }, {  20, 24, 0, 1, flat, unbanked }, {  19, 24, 0, 1, flat, unbanked }, {  18, 24, 0, 1, flat, unbanked }, {  17, 24, 0, 1, flat, unbanked },
        {  16, 24, 0, 1, flat, unbanked }, {  15, 25, 0, 1, flat, unbanked }, {  14, 25, 0, 1, flat, unbanked }, {  13, 25, 0, 1, flat, unbanked }, {  12, 25, 0, 1, flat, unbanked },
        {  11, 25, 0, 1, flat, unbanked }, {  10, 26, 0, 1, flat, unbanked }, {   9, 26, 0, 1, flat, unbanked }, {   8, 26, 0, 1, flat, unbanked }, {   7, 27, 0, 2, flat, unbanked },
        {   6, 27, 0, 2, flat, unbanked }, {   5, 27, 0, 2, flat, unbanked }, {   4, 28, 0, 2, flat, unbanked }, {   3, 28, 0, 2, flat, unbanked }, {   2, 28, 0, 2, flat, unbanked },
        {   1, 29, 0, 2, flat, unbanked }, {   0, 29, 0, 2, flat, unbanked }, {  -1, 30, 0, 2, flat, unbanked }, {  -2, 30, 0, 3, flat, unbanked }, {  -3, 31, 0, 3, flat, unbanked },
        {  -4, 31, 0, 3, flat, unbanked }, {  -5, 32, 0, 3, flat, unbanked }, {  -6, 33, 0, 3, flat, unbanked }, {  -7, 34, 0, 3, flat, unbanked }, {  -8, 34, 0, 3, flat, unbanked },
        {  -9, 35, 0, 4, flat, unbanked }, { -10, 36, 0, 4, flat, unbanked }, { -11, 37, 0, 4, flat, unbanked }, { -12, 38, 0, 4, flat, unbanked }, { -13, 39, 0, 4, flat, unbanked },
        { -14, 40, 0, 4, flat, unbanked }, { -15, 41, 0, 5, flat, unbanked }, { -16, 42, 0, 5, flat, unbanked }, { -17, 43, 0, 5, flat, unbanked }, { -18, 44, 0, 5, flat, unbanked },
        { -18, 45, 0, 5, flat, unbanked }, { -19, 46, 0, 5, flat, unbanked }, { -19, 47, 0, 6, flat, unbanked }, { -20, 48, 0, 6, flat, unbanked }, { -20, 49, 0, 6, flat, unbanked },
        { -21, 50, 0, 6, flat, unbanked }, { -21, 51, 0, 6, flat, unbanked }, { -21, 52, 0, 6, flat, unbanked }, { -22, 53, 0, 7, flat, unbanked }, { -22, 54, 0, 7, flat, unbanked },
        { -22, 55, 0, 7, flat, unbanked }, { -22, 56, 0, 7, flat, unbanked }, { -23, 57, 0, 7, flat, unbanked }, { -23, 58, 0, 7, flat, unbanked }, { -23, 59, 0, 7, flat, unbanked },
        { -23, 60, 0, 8, flat, unbanked }, { -23, 61, 0, 8, flat, unbanked }, { -23, 62, 0, 8, flat, unbanked }, { -23, 63, 0, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn3Tiles_1, {
        { 23,  0, 0,  8, flat, unbanked }, { 23,  1, 0,  8, flat, unbanked }, { 23,  2, 0,  8, flat, unbanked }, { 23,  3, 0,  8, flat, unbanked }, { 23,  4, 0,  8, flat, unbanked },
        { 23,  5, 0,  8, flat, unbanked }, { 23,  6, 0,  8, flat, unbanked }, { 23,  7, 0,  8, flat, unbanked }, { 24,  8, 0,  8, flat, unbanked }, { 24,  9, 0,  8, flat, unbanked },
        { 24, 10, 0,  8, flat, unbanked }, { 24, 11, 0,  8, flat, unbanked }, { 24, 12, 0,  9, flat, unbanked }, { 24, 13, 0,  9, flat, unbanked }, { 24, 14, 0,  9, flat, unbanked },
        { 24, 15, 0,  9, flat, unbanked }, { 24, 16, 0,  9, flat, unbanked }, { 25, 17, 0,  9, flat, unbanked }, { 25, 18, 0,  9, flat, unbanked }, { 25, 19, 0,  9, flat, unbanked },
        { 25, 20, 0,  9, flat, unbanked }, { 25, 21, 0,  9, flat, unbanked }, { 26, 22, 0,  9, flat, unbanked }, { 26, 23, 0,  9, flat, unbanked }, { 26, 24, 0,  9, flat, unbanked },
        { 27, 25, 0, 10, flat, unbanked }, { 27, 26, 0, 10, flat, unbanked }, { 27, 27, 0, 10, flat, unbanked }, { 28, 28, 0, 10, flat, unbanked }, { 28, 29, 0, 10, flat, unbanked },
        { 28, 30, 0, 10, flat, unbanked }, { 29, 31, 0, 10, flat, unbanked }, { 29, 32, 0, 10, flat, unbanked }, { 30, 33, 0, 10, flat, unbanked }, { 30, 34, 0, 11, flat, unbanked },
        { 31, 35, 0, 11, flat, unbanked }, { 31, 36, 0, 11, flat, unbanked }, { 32, 37, 0, 11, flat, unbanked }, { 33, 38, 0, 11, flat, unbanked }, { 34, 39, 0, 11, flat, unbanked },
        { 34, 40, 0, 11, flat, unbanked }, { 35, 41, 0, 12, flat, unbanked }, { 36, 42, 0, 12, flat, unbanked }, { 37, 43, 0, 12, flat, unbanked }, { 38, 44, 0, 12, flat, unbanked },
        { 39, 45, 0, 12, flat, unbanked }, { 40, 46, 0, 12, flat, unbanked }, { 41, 47, 0, 13, flat, unbanked }, { 42, 48, 0, 13, flat, unbanked }, { 43, 49, 0, 13, flat, unbanked },
        { 44, 50, 0, 13, flat, unbanked }, { 45, 50, 0, 13, flat, unbanked }, { 46, 51, 0, 13, flat, unbanked }, { 47, 51, 0, 14, flat, unbanked }, { 48, 52, 0, 14, flat, unbanked },
        { 49, 52, 0, 14, flat, unbanked }, { 50, 53, 0, 14, flat, unbanked }, { 51, 53, 0, 14, flat, unbanked }, { 52, 53, 0, 14, flat, unbanked }, { 53, 54, 0, 15, flat, unbanked },
        { 54, 54, 0, 15, flat, unbanked }, { 55, 54, 0, 15, flat, unbanked }, { 56, 54, 0, 15, flat, unbanked }, { 57, 55, 0, 15, flat, unbanked }, { 58, 55, 0, 15, flat, unbanked },
        { 59, 55, 0, 15, flat, unbanked }, { 60, 55, 0, 16, flat, unbanked }, { 61, 55, 0, 16, flat, unbanked }, { 62, 55, 0, 16, flat, unbanked }, { 63, 55, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn3Tiles_2, {
        {  0,   9, 0, 16, flat, unbanked }, {  1,   9, 0, 16, flat, unbanked }, {  2,   9, 0, 16, flat, unbanked }, {  3,   9, 0, 16, flat, unbanked }, {  4,   9, 0, 16, flat, unbanked },
        {  5,   9, 0, 16, flat, unbanked }, {  6,   9, 0, 16, flat, unbanked }, {  7,   9, 0, 16, flat, unbanked }, {  8,   8, 0, 16, flat, unbanked }, {  9,   8, 0, 16, flat, unbanked },
        { 10,   8, 0, 16, flat, unbanked }, { 11,   8, 0, 16, flat, unbanked }, { 12,   8, 0, 17, flat, unbanked }, { 13,   8, 0, 17, flat, unbanked }, { 14,   8, 0, 17, flat, unbanked },
        { 15,   8, 0, 17, flat, unbanked }, { 16,   8, 0, 17, flat, unbanked }, { 17,   7, 0, 17, flat, unbanked }, { 18,   7, 0, 17, flat, unbanked }, { 19,   7, 0, 17, flat, unbanked },
        { 20,   7, 0, 17, flat, unbanked }, { 21,   7, 0, 17, flat, unbanked }, { 22,   6, 0, 17, flat, unbanked }, { 23,   6, 0, 17, flat, unbanked }, { 24,   6, 0, 17, flat, unbanked },
        { 25,   5, 0, 18, flat, unbanked }, { 26,   5, 0, 18, flat, unbanked }, { 27,   5, 0, 18, flat, unbanked }, { 28,   4, 0, 18, flat, unbanked }, { 29,   4, 0, 18, flat, unbanked },
        { 30,   4, 0, 18, flat, unbanked }, { 31,   3, 0, 18, flat, unbanked }, { 32,   3, 0, 18, flat, unbanked }, { 33,   2, 0, 18, flat, unbanked }, { 34,   2, 0, 19, flat, unbanked },
        { 35,   1, 0, 19, flat, unbanked }, { 36,   1, 0, 19, flat, unbanked }, { 37,   0, 0, 19, flat, unbanked }, { 38,  -1, 0, 19, flat, unbanked }, { 39,  -2, 0, 19, flat, unbanked },
        { 40,  -2, 0, 19, flat, unbanked }, { 41,  -3, 0, 20, flat, unbanked }, { 42,  -4, 0, 20, flat, unbanked }, { 43,  -5, 0, 20, flat, unbanked }, { 44,  -6, 0, 20, flat, unbanked },
        { 45,  -7, 0, 20, flat, unbanked }, { 46,  -8, 0, 20, flat, unbanked }, { 47,  -9, 0, 21, flat, unbanked }, { 48, -10, 0, 21, flat, unbanked }, { 49, -11, 0, 21, flat, unbanked },
        { 50, -12, 0, 21, flat, unbanked }, { 50, -13, 0, 21, flat, unbanked }, { 51, -14, 0, 21, flat, unbanked }, { 51, -15, 0, 22, flat, unbanked }, { 52, -16, 0, 22, flat, unbanked },
        { 52, -17, 0, 22, flat, unbanked }, { 53, -18, 0, 22, flat, unbanked }, { 53, -19, 0, 22, flat, unbanked }, { 53, -20, 0, 22, flat, unbanked }, { 54, -21, 0, 23, flat, unbanked },
        { 54, -22, 0, 23, flat, unbanked }, { 54, -23, 0, 23, flat, unbanked }, { 54, -24, 0, 23, flat, unbanked }, { 55, -25, 0, 23, flat, unbanked }, { 55, -26, 0, 23, flat, unbanked },
        { 55, -27, 0, 23, flat, unbanked }, { 55, -28, 0, 24, flat, unbanked }, { 55, -29, 0, 24, flat, unbanked }, { 55, -30, 0, 24, flat, unbanked }, { 55, -31, 0, 24, flat, unbanked },
        { 55, -32, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn3Tiles_3, {
        {   9,  31, 0, 24, flat, unbanked }, {   9,  30, 0, 24, flat, unbanked }, {   9,  29, 0, 24, flat, unbanked }, {   9,  28, 0, 24, flat, unbanked }, {   9,  27, 0, 24, flat, unbanked },
        {   9,  26, 0, 24, flat, unbanked }, {   9,  25, 0, 24, flat, unbanked }, {   8,  24, 0, 24, flat, unbanked }, {   8,  23, 0, 24, flat, unbanked }, {   8,  22, 0, 24, flat, unbanked },
        {   8,  21, 0, 24, flat, unbanked }, {   8,  20, 0, 25, flat, unbanked }, {   8,  19, 0, 25, flat, unbanked }, {   8,  18, 0, 25, flat, unbanked }, {   8,  17, 0, 25, flat, unbanked },
        {   8,  16, 0, 25, flat, unbanked }, {   7,  15, 0, 25, flat, unbanked }, {   7,  14, 0, 25, flat, unbanked }, {   7,  13, 0, 25, flat, unbanked }, {   7,  12, 0, 25, flat, unbanked },
        {   7,  11, 0, 25, flat, unbanked }, {   6,  10, 0, 25, flat, unbanked }, {   6,   9, 0, 25, flat, unbanked }, {   6,   8, 0, 25, flat, unbanked }, {   5,   7, 0, 26, flat, unbanked },
        {   5,   6, 0, 26, flat, unbanked }, {   5,   5, 0, 26, flat, unbanked }, {   4,   4, 0, 26, flat, unbanked }, {   4,   3, 0, 26, flat, unbanked }, {   4,   2, 0, 26, flat, unbanked },
        {   3,   1, 0, 26, flat, unbanked }, {   3,   0, 0, 26, flat, unbanked }, {   2,  -1, 0, 26, flat, unbanked }, {   2,  -2, 0, 27, flat, unbanked }, {   1,  -3, 0, 27, flat, unbanked },
        {   1,  -4, 0, 27, flat, unbanked }, {   0,  -5, 0, 27, flat, unbanked }, {  -1,  -6, 0, 27, flat, unbanked }, {  -2,  -7, 0, 27, flat, unbanked }, {  -2,  -8, 0, 27, flat, unbanked },
        {  -3,  -9, 0, 28, flat, unbanked }, {  -4, -10, 0, 28, flat, unbanked }, {  -5, -11, 0, 28, flat, unbanked }, {  -6, -12, 0, 28, flat, unbanked }, {  -7, -13, 0, 28, flat, unbanked },
        {  -8, -14, 0, 28, flat, unbanked }, {  -9, -15, 0, 29, flat, unbanked }, { -10, -16, 0, 29, flat, unbanked }, { -11, -17, 0, 29, flat, unbanked }, { -12, -18, 0, 29, flat, unbanked },
        { -13, -18, 0, 29, flat, unbanked }, { -14, -19, 0, 29, flat, unbanked }, { -15, -19, 0, 30, flat, unbanked }, { -16, -20, 0, 30, flat, unbanked }, { -17, -20, 0, 30, flat, unbanked },
        { -18, -21, 0, 30, flat, unbanked }, { -19, -21, 0, 30, flat, unbanked }, { -20, -21, 0, 30, flat, unbanked }, { -21, -22, 0, 31, flat, unbanked }, { -22, -22, 0, 31, flat, unbanked },
        { -23, -22, 0, 31, flat, unbanked }, { -24, -22, 0, 31, flat, unbanked }, { -25, -23, 0, 31, flat, unbanked }, { -26, -23, 0, 31, flat, unbanked }, { -27, -23, 0, 31, flat, unbanked },
        { -28, -23, 0,  0, flat, unbanked }, { -29, -23, 0,  0, flat, unbanked }, { -30, -23, 0,  0, flat, unbanked }, { -31, -23, 0,  0, flat, unbanked }, { -32, -23, 0,  0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn1Tile_0, {
        { 31, 23, 0,  0, flat, unbanked }, { 30, 23, 0,  0, flat, unbanked }, { 29, 23, 0,  0, flat, unbanked }, { 28, 23, 0, 31, flat, unbanked }, { 27, 23, 0, 31, flat, unbanked },
        { 26, 22, 0, 31, flat, unbanked }, { 25, 22, 0, 31, flat, unbanked }, { 24, 22, 0, 30, flat, unbanked }, { 23, 21, 0, 30, flat, unbanked }, { 22, 21, 0, 30, flat, unbanked },
        { 21, 21, 0, 30, flat, unbanked }, { 20, 20, 0, 29, flat, unbanked }, { 19, 20, 0, 29, flat, unbanked }, { 18, 19, 0, 29, flat, unbanked }, { 17, 18, 0, 29, flat, unbanked },
        { 16, 17, 0, 28, flat, unbanked }, { 15, 16, 0, 28, flat, unbanked }, { 14, 15, 0, 27, flat, unbanked }, { 13, 14, 0, 27, flat, unbanked }, { 12, 13, 0, 27, flat, unbanked },
        { 12, 12, 0, 27, flat, unbanked }, { 11, 11, 0, 26, flat, unbanked }, { 11, 10, 0, 26, flat, unbanked }, { 11,  9, 0, 26, flat, unbanked }, { 10,  8, 0, 26, flat, unbanked },
        { 10,  7, 0, 25, flat, unbanked }, { 10,  6, 0, 25, flat, unbanked }, {  9,  5, 0, 25, flat, unbanked }, {  9,  4, 0, 25, flat, unbanked }, {  9,  3, 0, 24, flat, unbanked },
        {  9,  2, 0, 24, flat, unbanked }, {  9,  1, 0, 24, flat, unbanked }, {  9,  0, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn1Tile_1, {
        { 23,  0, 0, 8, flat, unbanked }, { 23,  1, 0, 8, flat, unbanked }, { 23,  2, 0, 8, flat, unbanked }, { 23,  3, 0, 8, flat, unbanked }, { 23,  4, 0, 7, flat, unbanked }, { 23,  5, 0, 7, flat, unbanked },
        { 22,  6, 0, 7, flat, unbanked }, { 22,  7, 0, 7, flat, unbanked }, { 22,  8, 0, 6, flat, unbanked }, { 21,  9, 0, 6, flat, unbanked }, { 21, 10, 0, 6, flat, unbanked }, { 21, 11, 0, 6, flat, unbanked },
        { 20, 12, 0, 5, flat, unbanked }, { 20, 13, 0, 5, flat, unbanked }, { 19, 14, 0, 5, flat, unbanked }, { 18, 15, 0, 5, flat, unbanked }, { 17, 16, 0, 4, flat, unbanked }, { 16, 17, 0, 4, flat, unbanked },
        { 15, 18, 0, 3, flat, unbanked }, { 14, 19, 0, 3, flat, unbanked }, { 13, 20, 0, 3, flat, unbanked }, { 12, 20, 0, 3, flat, unbanked }, { 11, 21, 0, 2, flat, unbanked }, { 10, 21, 0, 2, flat, unbanked },
        {  9, 21, 0, 2, flat, unbanked }, {  8, 22, 0, 2, flat, unbanked }, {  7, 22, 0, 1, flat, unbanked }, {  6, 22, 0, 1, flat, unbanked }, {  5, 23, 0, 1, flat, unbanked }, {  4, 23, 0, 1, flat, unbanked },
        {  3, 23, 0, 0, flat, unbanked }, {  2, 23, 0, 0, flat, unbanked }, {  1, 23, 0, 0, flat, unbanked }, {  0, 23, 0, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn1Tile_2, {
        {  0,  9, 0, 16, flat, unbanked }, {  1,  9, 0, 16, flat, unbanked }, {  2,  9, 0, 16, flat, unbanked }, {  3,  9, 0, 16, flat, unbanked }, {  4,  9, 0, 15, flat, unbanked },
        {  5, 10, 0, 15, flat, unbanked }, {  6, 10, 0, 15, flat, unbanked }, {  7, 10, 0, 15, flat, unbanked }, {  8, 11, 0, 14, flat, unbanked }, {  9, 11, 0, 14, flat, unbanked },
        { 10, 11, 0, 14, flat, unbanked }, { 11, 12, 0, 14, flat, unbanked }, { 12, 12, 0, 13, flat, unbanked }, { 13, 13, 0, 13, flat, unbanked }, { 14, 14, 0, 13, flat, unbanked },
        { 15, 15, 0, 13, flat, unbanked }, { 16, 16, 0, 12, flat, unbanked }, { 17, 17, 0, 12, flat, unbanked }, { 18, 18, 0, 11, flat, unbanked }, { 19, 19, 0, 11, flat, unbanked },
        { 20, 20, 0, 11, flat, unbanked }, { 20, 21, 0, 11, flat, unbanked }, { 21, 22, 0, 10, flat, unbanked }, { 21, 23, 0, 10, flat, unbanked }, { 21, 24, 0, 10, flat, unbanked },
        { 22, 25, 0, 10, flat, unbanked }, { 22, 26, 0,  9, flat, unbanked }, { 22, 27, 0,  9, flat, unbanked }, { 23, 28, 0,  9, flat, unbanked }, { 23, 29, 0,  9, flat, unbanked },
        { 23, 30, 0,  8, flat, unbanked }, { 23, 31, 0,  8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftQuarterTurn1Tile_3, {
        {  9, 31, 0, 24, flat, unbanked }, {  9, 30, 0, 24, flat, unbanked }, {  9, 29, 0, 24, flat, unbanked }, {  9, 28, 0, 23, flat, unbanked }, {  9, 27, 0, 23, flat, unbanked },
        { 10, 26, 0, 23, flat, unbanked }, { 10, 25, 0, 23, flat, unbanked }, { 10, 24, 0, 22, flat, unbanked }, { 11, 23, 0, 22, flat, unbanked }, { 11, 22, 0, 22, flat, unbanked },
        { 11, 21, 0, 22, flat, unbanked }, { 12, 20, 0, 21, flat, unbanked }, { 12, 19, 0, 21, flat, unbanked }, { 13, 18, 0, 21, flat, unbanked }, { 14, 17, 0, 21, flat, unbanked },
        { 15, 16, 0, 20, flat, unbanked }, { 16, 15, 0, 20, flat, unbanked }, { 17, 14, 0, 19, flat, unbanked }, { 18, 13, 0, 19, flat, unbanked }, { 19, 12, 0, 19, flat, unbanked },
        { 20, 12, 0, 19, flat, unbanked }, { 21, 11, 0, 18, flat, unbanked }, { 22, 11, 0, 18, flat, unbanked }, { 23, 11, 0, 18, flat, unbanked }, { 24, 10, 0, 18, flat, unbanked },
        { 25, 10, 0, 17, flat, unbanked }, { 26, 10, 0, 17, flat, unbanked }, { 27,  9, 0, 17, flat, unbanked }, { 28,  9, 0, 17, flat, unbanked }, { 29,  9, 0, 16, flat, unbanked },
        { 30,  9, 0, 16, flat, unbanked }, { 31,  9, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn1Tile_0, {
        { 31, 23, 0, 0, flat, unbanked }, { 30, 23, 0, 1, flat, unbanked }, { 29, 23, 0, 1, flat, unbanked }, { 28, 24, 0, 2, flat, unbanked }, { 27, 24, 0, 2, flat, unbanked }, { 26, 25, 0, 4, flat, unbanked },
        { 25, 26, 0, 4, flat, unbanked }, { 24, 27, 0, 6, flat, unbanked }, { 24, 28, 0, 6, flat, unbanked }, { 23, 29, 0, 7, flat, unbanked }, { 23, 30, 0, 7, flat, unbanked }, { 23, 31, 0, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn1Tile_1, {
        { 23, 0, 0,  8, flat, unbanked }, { 23, 1, 0,  8, flat, unbanked }, { 23, 2, 0,  9, flat, unbanked }, { 23, 3, 0,  9, flat, unbanked }, { 24, 4, 0, 10, flat, unbanked }, { 24, 5, 0, 10, flat, unbanked },
        { 25, 6, 0, 12, flat, unbanked }, { 26, 7, 0, 12, flat, unbanked }, { 27, 8, 0, 14, flat, unbanked }, { 28, 8, 0, 14, flat, unbanked }, { 29, 9, 0, 15, flat, unbanked }, { 30, 9, 0, 15, flat, unbanked },
        { 31, 9, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn1Tile_2, {
        { 0, 9, 0, 16, flat, unbanked }, { 1, 9, 0, 16, flat, unbanked }, { 2, 9, 0, 17, flat, unbanked }, { 3, 9, 0, 17, flat, unbanked }, { 4, 8, 0, 18, flat, unbanked }, { 5, 8, 0, 18, flat, unbanked },
        { 6, 7, 0, 20, flat, unbanked }, { 7, 6, 0, 20, flat, unbanked }, { 8, 5, 0, 22, flat, unbanked }, { 8, 4, 0, 22, flat, unbanked }, { 9, 3, 0, 23, flat, unbanked }, { 9, 2, 0, 23, flat, unbanked },
        { 9, 1, 0, 24, flat, unbanked }, { 9, 0, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightQuarterTurn1Tile_3, {
        { 9, 31, 0, 24, flat, unbanked }, { 9, 30, 0, 25, flat, unbanked }, { 9, 29, 0, 25, flat, unbanked }, { 8, 28, 0, 26, flat, unbanked }, { 8, 27, 0, 26, flat, unbanked }, { 7, 26, 0, 28, flat, unbanked },
        { 6, 25, 0, 28, flat, unbanked }, { 5, 24, 0, 30, flat, unbanked }, { 4, 24, 0, 30, flat, unbanked }, { 3, 23, 0, 31, flat, unbanked }, { 2, 23, 0, 31, flat, unbanked }, { 1, 23, 0,  0, flat, unbanked },
        { 0, 23, 0,  0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftEighthToDiag_0, {
        {  31,  23, 0,  0, flat, unbanked }, {  30,  23, 0,  0, flat, unbanked }, {  29,  23, 0,  0, flat, unbanked }, {  28,  23, 0,  0, flat, unbanked }, {  27,  23, 0,  0, flat, unbanked },
        {  26,  23, 0, 31, flat, unbanked }, {  25,  23, 0, 31, flat, unbanked }, {  24,  23, 0, 31, flat, unbanked }, {  23,  23, 0, 31, flat, unbanked }, {  22,  22, 0, 31, flat, unbanked },
        {  21,  22, 0, 31, flat, unbanked }, {  20,  22, 0, 31, flat, unbanked }, {  19,  22, 0, 31, flat, unbanked }, {  18,  22, 0, 31, flat, unbanked }, {  17,  21, 0, 31, flat, unbanked },
        {  16,  21, 0, 31, flat, unbanked }, {  15,  21, 0, 31, flat, unbanked }, {  14,  21, 0, 31, flat, unbanked }, {  13,  20, 0, 30, flat, unbanked }, {  12,  20, 0, 30, flat, unbanked },
        {  11,  20, 0, 30, flat, unbanked }, {  10,  19, 0, 30, flat, unbanked }, {   9,  19, 0, 30, flat, unbanked }, {   8,  19, 0, 30, flat, unbanked }, {   7,  18, 0, 30, flat, unbanked },
        {   6,  18, 0, 30, flat, unbanked }, {   5,  17, 0, 30, flat, unbanked }, {   4,  17, 0, 30, flat, unbanked }, {   3,  16, 0, 30, flat, unbanked }, {   2,  16, 0, 30, flat, unbanked },
        {   1,  15, 0, 30, flat, unbanked }, {   0,  15, 0, 30, flat, unbanked }, {  -1,  14, 0, 30, flat, unbanked }, {  -2,  14, 0, 29, flat, unbanked }, {  -3,  13, 0, 29, flat, unbanked },
        {  -4,  13, 0, 29, flat, unbanked }, {  -5,  12, 0, 29, flat, unbanked }, {  -6,  12, 0, 29, flat, unbanked }, {  -7,  11, 0, 29, flat, unbanked }, {  -8,  10, 0, 29, flat, unbanked },
        {  -9,  10, 0, 29, flat, unbanked }, { -10,   9, 0, 29, flat, unbanked }, { -11,   8, 0, 29, flat, unbanked }, { -12,   8, 0, 29, flat, unbanked }, { -13,   7, 0, 29, flat, unbanked },
        { -14,   6, 0, 29, flat, unbanked }, { -15,   6, 0, 29, flat, unbanked }, { -16,   5, 0, 29, flat, unbanked }, { -17,   4, 0, 29, flat, unbanked }, { -18,   3, 0, 29, flat, unbanked },
        { -19,   2, 0, 29, flat, unbanked }, { -20,   2, 0, 29, flat, unbanked }, { -21,   1, 0, 29, flat, unbanked }, { -22,   0, 0, 29, flat, unbanked }, { -23,  -1, 0, 28, flat, unbanked },
        { -24,  -2, 0, 28, flat, unbanked }, { -25,  -2, 0, 28, flat, unbanked }, { -26,  -3, 0, 28, flat, unbanked }, { -27,  -4, 0, 28, flat, unbanked }, { -28,  -5, 0, 28, flat, unbanked },
        { -29,  -6, 0, 28, flat, unbanked }, { -30,  -7, 0, 28, flat, unbanked }, { -31,  -8, 0, 28, flat, unbanked }, { -32,  -9, 0, 28, flat, unbanked }, { -33,  -9, 0, 28, flat, unbanked },
        { -34, -10, 0, 28, flat, unbanked }, { -35, -11, 0, 28, flat, unbanked }, { -36, -12, 0, 28, flat, unbanked }, { -37, -13, 0, 28, flat, unbanked }, { -38, -14, 0, 28, flat, unbanked },
        { -39, -15, 0, 28, flat, unbanked }, { -40, -16, 0, 28, flat, unbanked }, { -41, -17, 0, 28, flat, unbanked }, { -42, -18, 0, 28, flat, unbanked }, { -43, -19, 0, 28, flat, unbanked },
        { -44, -20, 0, 28, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftEighthToDiag_1, {
        {  23,  0, 0, 8, flat, unbanked }, {  23,  1, 0, 8, flat, unbanked }, {  23,  2, 0, 8, flat, unbanked }, {  23,  3, 0, 8, flat, unbanked }, {  23,  4, 0, 8, flat, unbanked },
        {  23,  5, 0, 8, flat, unbanked }, {  23,  6, 0, 7, flat, unbanked }, {  23,  7, 0, 7, flat, unbanked }, {  23,  8, 0, 7, flat, unbanked }, {  23,  9, 0, 7, flat, unbanked },
        {  22, 10, 0, 7, flat, unbanked }, {  22, 11, 0, 7, flat, unbanked }, {  22, 12, 0, 7, flat, unbanked }, {  22, 13, 0, 7, flat, unbanked }, {  22, 14, 0, 7, flat, unbanked },
        {  21, 15, 0, 7, flat, unbanked }, {  21, 16, 0, 7, flat, unbanked }, {  21, 17, 0, 7, flat, unbanked }, {  21, 18, 0, 7, flat, unbanked }, {  20, 19, 0, 6, flat, unbanked },
        {  20, 20, 0, 6, flat, unbanked }, {  20, 21, 0, 6, flat, unbanked }, {  19, 22, 0, 6, flat, unbanked }, {  19, 23, 0, 6, flat, unbanked }, {  19, 24, 0, 6, flat, unbanked },
        {  18, 25, 0, 6, flat, unbanked }, {  18, 26, 0, 6, flat, unbanked }, {  17, 27, 0, 6, flat, unbanked }, {  17, 28, 0, 6, flat, unbanked }, {  16, 29, 0, 6, flat, unbanked },
        {  16, 30, 0, 6, flat, unbanked }, {  15, 31, 0, 6, flat, unbanked }, {  15, 32, 0, 6, flat, unbanked }, {  14, 33, 0, 6, flat, unbanked }, {  14, 34, 0, 5, flat, unbanked },
        {  13, 35, 0, 5, flat, unbanked }, {  13, 36, 0, 5, flat, unbanked }, {  12, 37, 0, 5, flat, unbanked }, {  12, 38, 0, 5, flat, unbanked }, {  11, 39, 0, 5, flat, unbanked },
        {  10, 40, 0, 5, flat, unbanked }, {  10, 41, 0, 5, flat, unbanked }, {   9, 42, 0, 5, flat, unbanked }, {   8, 43, 0, 5, flat, unbanked }, {   8, 44, 0, 5, flat, unbanked },
        {   7, 45, 0, 5, flat, unbanked }, {   6, 46, 0, 5, flat, unbanked }, {   6, 47, 0, 5, flat, unbanked }, {   5, 48, 0, 5, flat, unbanked }, {   4, 49, 0, 5, flat, unbanked },
        {   3, 50, 0, 5, flat, unbanked }, {   2, 51, 0, 5, flat, unbanked }, {   2, 52, 0, 5, flat, unbanked }, {   1, 53, 0, 5, flat, unbanked }, {   0, 54, 0, 5, flat, unbanked },
        {  -1, 55, 0, 4, flat, unbanked }, {  -2, 56, 0, 4, flat, unbanked }, {  -2, 57, 0, 4, flat, unbanked }, {  -3, 58, 0, 4, flat, unbanked }, {  -4, 59, 0, 4, flat, unbanked },
        {  -5, 60, 0, 4, flat, unbanked }, {  -6, 61, 0, 4, flat, unbanked }, {  -7, 62, 0, 4, flat, unbanked }, {  -8, 63, 0, 4, flat, unbanked }, {  -9, 64, 0, 4, flat, unbanked },
        {  -9, 65, 0, 4, flat, unbanked }, { -10, 66, 0, 4, flat, unbanked }, { -11, 67, 0, 4, flat, unbanked }, { -12, 68, 0, 4, flat, unbanked }, { -13, 69, 0, 4, flat, unbanked },
        { -14, 70, 0, 4, flat, unbanked }, { -15, 71, 0, 4, flat, unbanked }, { -16, 72, 0, 4, flat, unbanked }, { -17, 73, 0, 4, flat, unbanked }, { -18, 74, 0, 4, flat, unbanked },
        { -19, 75, 0, 4, flat, unbanked }, { -20, 76, 0, 4, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftEighthToDiag_2, {
        {  0,  9, 0, 16, flat, unbanked }, {  1,  9, 0, 16, flat, unbanked }, {  2,  9, 0, 16, flat, unbanked }, {  3,  9, 0, 16, flat, unbanked }, {  4,  9, 0, 16, flat, unbanked },
        {  5,  9, 0, 16, flat, unbanked }, {  6,  9, 0, 15, flat, unbanked }, {  7,  9, 0, 15, flat, unbanked }, {  8,  9, 0, 15, flat, unbanked }, {  9,  9, 0, 15, flat, unbanked },
        { 10, 10, 0, 15, flat, unbanked }, { 11, 10, 0, 15, flat, unbanked }, { 12, 10, 0, 15, flat, unbanked }, { 13, 10, 0, 15, flat, unbanked }, { 14, 10, 0, 15, flat, unbanked },
        { 15, 11, 0, 15, flat, unbanked }, { 16, 11, 0, 15, flat, unbanked }, { 17, 11, 0, 15, flat, unbanked }, { 18, 11, 0, 15, flat, unbanked }, { 19, 12, 0, 14, flat, unbanked },
        { 20, 12, 0, 14, flat, unbanked }, { 21, 12, 0, 14, flat, unbanked }, { 22, 13, 0, 14, flat, unbanked }, { 23, 13, 0, 14, flat, unbanked }, { 24, 13, 0, 14, flat, unbanked },
        { 25, 14, 0, 14, flat, unbanked }, { 26, 14, 0, 14, flat, unbanked }, { 27, 15, 0, 14, flat, unbanked }, { 28, 15, 0, 14, flat, unbanked }, { 29, 16, 0, 14, flat, unbanked },
        { 30, 16, 0, 14, flat, unbanked }, { 31, 17, 0, 14, flat, unbanked }, { 32, 17, 0, 14, flat, unbanked }, { 33, 18, 0, 14, flat, unbanked }, { 34, 18, 0, 13, flat, unbanked },
        { 35, 19, 0, 13, flat, unbanked }, { 36, 19, 0, 13, flat, unbanked }, { 37, 20, 0, 13, flat, unbanked }, { 38, 20, 0, 13, flat, unbanked }, { 39, 21, 0, 13, flat, unbanked },
        { 40, 22, 0, 13, flat, unbanked }, { 41, 22, 0, 13, flat, unbanked }, { 42, 23, 0, 13, flat, unbanked }, { 43, 24, 0, 13, flat, unbanked }, { 44, 24, 0, 13, flat, unbanked },
        { 45, 25, 0, 13, flat, unbanked }, { 46, 26, 0, 13, flat, unbanked }, { 47, 26, 0, 13, flat, unbanked }, { 48, 27, 0, 13, flat, unbanked }, { 49, 28, 0, 13, flat, unbanked },
        { 50, 29, 0, 13, flat, unbanked }, { 51, 30, 0, 13, flat, unbanked }, { 52, 30, 0, 13, flat, unbanked }, { 53, 31, 0, 13, flat, unbanked }, { 54, 32, 0, 13, flat, unbanked },
        { 55, 33, 0, 12, flat, unbanked }, { 56, 34, 0, 12, flat, unbanked }, { 57, 34, 0, 12, flat, unbanked }, { 58, 35, 0, 12, flat, unbanked }, { 59, 36, 0, 12, flat, unbanked },
        { 60, 37, 0, 12, flat, unbanked }, { 61, 38, 0, 12, flat, unbanked }, { 62, 39, 0, 12, flat, unbanked }, { 63, 40, 0, 12, flat, unbanked }, { 64, 41, 0, 12, flat, unbanked },
        { 65, 41, 0, 12, flat, unbanked }, { 66, 42, 0, 12, flat, unbanked }, { 67, 43, 0, 12, flat, unbanked }, { 68, 44, 0, 12, flat, unbanked }, { 69, 45, 0, 12, flat, unbanked },
        { 70, 46, 0, 12, flat, unbanked }, { 71, 47, 0, 12, flat, unbanked }, { 72, 48, 0, 12, flat, unbanked }, { 73, 49, 0, 12, flat, unbanked }, { 74, 50, 0, 12, flat, unbanked },
        { 75, 51, 0, 12, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftEighthToDiag_3, {
        {  9,  31, 0, 24, flat, unbanked }, {  9,  30, 0, 24, flat, unbanked }, {  9,  29, 0, 24, flat, unbanked }, {  9,  28, 0, 24, flat, unbanked }, {  9,  27, 0, 24, flat, unbanked },
        {  9,  26, 0, 23, flat, unbanked }, {  9,  25, 0, 23, flat, unbanked }, {  9,  24, 0, 23, flat, unbanked }, {  9,  23, 0, 23, flat, unbanked }, { 10,  22, 0, 23, flat, unbanked },
        { 10,  21, 0, 23, flat, unbanked }, { 10,  20, 0, 23, flat, unbanked }, { 10,  19, 0, 23, flat, unbanked }, { 10,  18, 0, 23, flat, unbanked }, { 11,  17, 0, 23, flat, unbanked },
        { 11,  16, 0, 23, flat, unbanked }, { 11,  15, 0, 23, flat, unbanked }, { 11,  14, 0, 23, flat, unbanked }, { 12,  13, 0, 22, flat, unbanked }, { 12,  12, 0, 22, flat, unbanked },
        { 12,  11, 0, 22, flat, unbanked }, { 13,  10, 0, 22, flat, unbanked }, { 13,   9, 0, 22, flat, unbanked }, { 13,   8, 0, 22, flat, unbanked }, { 14,   7, 0, 22, flat, unbanked },
        { 14,   6, 0, 22, flat, unbanked }, { 15,   5, 0, 22, flat, unbanked }, { 15,   4, 0, 22, flat, unbanked }, { 16,   3, 0, 22, flat, unbanked }, { 16,   2, 0, 22, flat, unbanked },
        { 17,   1, 0, 22, flat, unbanked }, { 17,   0, 0, 22, flat, unbanked }, { 18,  -1, 0, 22, flat, unbanked }, { 18,  -2, 0, 21, flat, unbanked }, { 19,  -3, 0, 21, flat, unbanked },
        { 19,  -4, 0, 21, flat, unbanked }, { 20,  -5, 0, 21, flat, unbanked }, { 20,  -6, 0, 21, flat, unbanked }, { 21,  -7, 0, 21, flat, unbanked }, { 22,  -8, 0, 21, flat, unbanked },
        { 22,  -9, 0, 21, flat, unbanked }, { 23, -10, 0, 21, flat, unbanked }, { 24, -11, 0, 21, flat, unbanked }, { 24, -12, 0, 21, flat, unbanked }, { 25, -13, 0, 21, flat, unbanked },
        { 26, -14, 0, 21, flat, unbanked }, { 26, -15, 0, 21, flat, unbanked }, { 27, -16, 0, 21, flat, unbanked }, { 28, -17, 0, 21, flat, unbanked }, { 29, -18, 0, 21, flat, unbanked },
        { 30, -19, 0, 21, flat, unbanked }, { 30, -20, 0, 21, flat, unbanked }, { 31, -21, 0, 21, flat, unbanked }, { 32, -22, 0, 21, flat, unbanked }, { 33, -23, 0, 20, flat, unbanked },
        { 34, -24, 0, 20, flat, unbanked }, { 34, -25, 0, 20, flat, unbanked }, { 35, -26, 0, 20, flat, unbanked }, { 36, -27, 0, 20, flat, unbanked }, { 37, -28, 0, 20, flat, unbanked },
        { 38, -29, 0, 20, flat, unbanked }, { 39, -30, 0, 20, flat, unbanked }, { 40, -31, 0, 20, flat, unbanked }, { 41, -32, 0, 20, flat, unbanked }, { 41, -33, 0, 20, flat, unbanked },
        { 42, -34, 0, 20, flat, unbanked }, { 43, -35, 0, 20, flat, unbanked }, { 44, -36, 0, 20, flat, unbanked }, { 45, -37, 0, 20, flat, unbanked }, { 46, -38, 0, 20, flat, unbanked },
        { 47, -39, 0, 20, flat, unbanked }, { 48, -40, 0, 20, flat, unbanked }, { 49, -41, 0, 20, flat, unbanked }, { 50, -42, 0, 20, flat, unbanked }, { 51, -43, 0, 20, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightEighthToDiag_0, {
        {  31, 23, 0, 0, flat, unbanked }, {  30, 23, 0, 0, flat, unbanked }, {  29, 23, 0, 0, flat, unbanked }, {  28, 23, 0, 0, flat, unbanked }, {  27, 23, 0, 0, flat, unbanked },
        {  26, 23, 0, 0, flat, unbanked }, {  25, 23, 0, 0, flat, unbanked }, {  24, 23, 0, 0, flat, unbanked }, {  23, 23, 0, 0, flat, unbanked }, {  22, 23, 0, 0, flat, unbanked },
        {  21, 23, 0, 0, flat, unbanked }, {  20, 23, 0, 0, flat, unbanked }, {  19, 23, 0, 0, flat, unbanked }, {  18, 23, 0, 0, flat, unbanked }, {  17, 23, 0, 0, flat, unbanked },
        {  16, 23, 0, 0, flat, unbanked }, {  15, 23, 0, 0, flat, unbanked }, {  14, 23, 0, 0, flat, unbanked }, {  13, 23, 0, 0, flat, unbanked }, {  12, 24, 0, 0, flat, unbanked },
        {  11, 24, 0, 0, flat, unbanked }, {  10, 24, 0, 0, flat, unbanked }, {   9, 24, 0, 0, flat, unbanked }, {   8, 24, 0, 0, flat, unbanked }, {   7, 24, 0, 0, flat, unbanked },
        {   6, 24, 0, 0, flat, unbanked }, {   5, 24, 0, 0, flat, unbanked }, {   4, 24, 0, 0, flat, unbanked }, {   3, 24, 0, 0, flat, unbanked }, {   2, 24, 0, 0, flat, unbanked },
        {   1, 24, 0, 0, flat, unbanked }, {   0, 24, 0, 0, flat, unbanked }, {  -1, 24, 0, 0, flat, unbanked }, {  -2, 24, 0, 0, flat, unbanked }, {  -3, 24, 0, 1, flat, unbanked },
        {  -4, 25, 0, 1, flat, unbanked }, {  -5, 25, 0, 1, flat, unbanked }, {  -6, 25, 0, 1, flat, unbanked }, {  -7, 25, 0, 1, flat, unbanked }, {  -8, 25, 0, 1, flat, unbanked },
        {  -9, 25, 0, 1, flat, unbanked }, { -10, 25, 0, 1, flat, unbanked }, { -11, 26, 0, 1, flat, unbanked }, { -12, 26, 0, 1, flat, unbanked }, { -13, 26, 0, 1, flat, unbanked },
        { -14, 26, 0, 1, flat, unbanked }, { -15, 26, 0, 1, flat, unbanked }, { -16, 26, 0, 1, flat, unbanked }, { -17, 27, 0, 1, flat, unbanked }, { -18, 27, 0, 1, flat, unbanked },
        { -19, 27, 0, 1, flat, unbanked }, { -20, 27, 0, 1, flat, unbanked }, { -21, 28, 0, 1, flat, unbanked }, { -22, 28, 0, 1, flat, unbanked }, { -23, 28, 0, 1, flat, unbanked },
        { -24, 28, 0, 2, flat, unbanked }, { -25, 29, 0, 2, flat, unbanked }, { -26, 29, 0, 2, flat, unbanked }, { -27, 29, 0, 2, flat, unbanked }, { -28, 30, 0, 2, flat, unbanked },
        { -29, 30, 0, 2, flat, unbanked }, { -30, 31, 0, 2, flat, unbanked }, { -31, 31, 0, 2, flat, unbanked }, { -32, 31, 0, 2, flat, unbanked }, { -33, 32, 0, 2, flat, unbanked },
        { -34, 32, 0, 2, flat, unbanked }, { -35, 33, 0, 2, flat, unbanked }, { -36, 33, 0, 2, flat, unbanked }, { -37, 34, 0, 2, flat, unbanked }, { -38, 34, 0, 3, flat, unbanked },
        { -39, 35, 0, 3, flat, unbanked }, { -40, 35, 0, 3, flat, unbanked }, { -41, 36, 0, 3, flat, unbanked }, { -42, 37, 0, 3, flat, unbanked }, { -43, 37, 0, 3, flat, unbanked },
        { -44, 38, 0, 3, flat, unbanked }, { -45, 39, 0, 3, flat, unbanked }, { -46, 40, 0, 3, flat, unbanked }, { -47, 40, 0, 3, flat, unbanked }, { -48, 41, 0, 4, flat, unbanked },
        { -49, 42, 0, 4, flat, unbanked }, { -50, 43, 0, 4, flat, unbanked }, { -51, 44, 0, 4, flat, unbanked }, { -52, 44, 0, 4, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightEighthToDiag_1, {
        { 23,  0, 0,  8, flat, unbanked }, { 23,  1, 0,  8, flat, unbanked }, { 23,  2, 0,  8, flat, unbanked }, { 23,  3, 0,  8, flat, unbanked }, { 23,  4, 0,  8, flat, unbanked },
        { 23,  5, 0,  8, flat, unbanked }, { 23,  6, 0,  8, flat, unbanked }, { 23,  7, 0,  8, flat, unbanked }, { 23,  8, 0,  8, flat, unbanked }, { 23,  9, 0,  8, flat, unbanked },
        { 23, 10, 0,  8, flat, unbanked }, { 23, 11, 0,  8, flat, unbanked }, { 23, 12, 0,  8, flat, unbanked }, { 23, 13, 0,  8, flat, unbanked }, { 23, 14, 0,  8, flat, unbanked },
        { 23, 15, 0,  8, flat, unbanked }, { 23, 16, 0,  8, flat, unbanked }, { 23, 17, 0,  8, flat, unbanked }, { 23, 18, 0,  8, flat, unbanked }, { 23, 19, 0,  8, flat, unbanked },
        { 24, 20, 0,  8, flat, unbanked }, { 24, 21, 0,  8, flat, unbanked }, { 24, 22, 0,  8, flat, unbanked }, { 24, 23, 0,  8, flat, unbanked }, { 24, 24, 0,  8, flat, unbanked },
        { 24, 25, 0,  8, flat, unbanked }, { 24, 26, 0,  8, flat, unbanked }, { 24, 27, 0,  8, flat, unbanked }, { 24, 28, 0,  8, flat, unbanked }, { 24, 29, 0,  8, flat, unbanked },
        { 24, 30, 0,  8, flat, unbanked }, { 24, 31, 0,  8, flat, unbanked }, { 24, 32, 0,  8, flat, unbanked }, { 24, 33, 0,  8, flat, unbanked }, { 24, 34, 0,  8, flat, unbanked },
        { 24, 35, 0,  9, flat, unbanked }, { 25, 36, 0,  9, flat, unbanked }, { 25, 37, 0,  9, flat, unbanked }, { 25, 38, 0,  9, flat, unbanked }, { 25, 39, 0,  9, flat, unbanked },
        { 25, 40, 0,  9, flat, unbanked }, { 25, 41, 0,  9, flat, unbanked }, { 25, 42, 0,  9, flat, unbanked }, { 26, 43, 0,  9, flat, unbanked }, { 26, 44, 0,  9, flat, unbanked },
        { 26, 45, 0,  9, flat, unbanked }, { 26, 46, 0,  9, flat, unbanked }, { 26, 47, 0,  9, flat, unbanked }, { 26, 48, 0,  9, flat, unbanked }, { 27, 49, 0,  9, flat, unbanked },
        { 27, 50, 0,  9, flat, unbanked }, { 27, 51, 0,  9, flat, unbanked }, { 27, 52, 0,  9, flat, unbanked }, { 28, 53, 0,  9, flat, unbanked }, { 28, 54, 0,  9, flat, unbanked },
        { 28, 55, 0,  9, flat, unbanked }, { 28, 56, 0, 10, flat, unbanked }, { 29, 57, 0, 10, flat, unbanked }, { 29, 58, 0, 10, flat, unbanked }, { 29, 59, 0, 10, flat, unbanked },
        { 30, 60, 0, 10, flat, unbanked }, { 30, 61, 0, 10, flat, unbanked }, { 31, 62, 0, 10, flat, unbanked }, { 31, 63, 0, 10, flat, unbanked }, { 31, 64, 0, 10, flat, unbanked },
        { 32, 65, 0, 10, flat, unbanked }, { 32, 66, 0, 10, flat, unbanked }, { 33, 67, 0, 10, flat, unbanked }, { 33, 68, 0, 10, flat, unbanked }, { 34, 69, 0, 10, flat, unbanked },
        { 34, 70, 0, 11, flat, unbanked }, { 35, 71, 0, 11, flat, unbanked }, { 35, 72, 0, 11, flat, unbanked }, { 36, 73, 0, 11, flat, unbanked }, { 37, 74, 0, 11, flat, unbanked },
        { 37, 75, 0, 11, flat, unbanked }, { 38, 76, 0, 11, flat, unbanked }, { 39, 77, 0, 11, flat, unbanked }, { 40, 78, 0, 11, flat, unbanked }, { 40, 79, 0, 11, flat, unbanked },
        { 41, 80, 0, 12, flat, unbanked }, { 42, 81, 0, 12, flat, unbanked }, { 43, 82, 0, 12, flat, unbanked }, { 44, 83, 0, 12, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightEighthToDiag_2, {
        {  0,   9, 0, 16, flat, unbanked }, {  1,   9, 0, 16, flat, unbanked }, {  2,   9, 0, 16, flat, unbanked }, {  3,   9, 0, 16, flat, unbanked }, {  4,   9, 0, 16, flat, unbanked },
        {  5,   9, 0, 16, flat, unbanked }, {  6,   9, 0, 16, flat, unbanked }, {  7,   9, 0, 16, flat, unbanked }, {  8,   9, 0, 16, flat, unbanked }, {  9,   9, 0, 16, flat, unbanked },
        { 10,   9, 0, 16, flat, unbanked }, { 11,   9, 0, 16, flat, unbanked }, { 12,   9, 0, 16, flat, unbanked }, { 13,   9, 0, 16, flat, unbanked }, { 14,   9, 0, 16, flat, unbanked },
        { 15,   9, 0, 16, flat, unbanked }, { 16,   9, 0, 16, flat, unbanked }, { 17,   9, 0, 16, flat, unbanked }, { 18,   9, 0, 16, flat, unbanked }, { 19,   9, 0, 16, flat, unbanked },
        { 20,   8, 0, 16, flat, unbanked }, { 21,   8, 0, 16, flat, unbanked }, { 22,   8, 0, 16, flat, unbanked }, { 23,   8, 0, 16, flat, unbanked }, { 24,   8, 0, 16, flat, unbanked },
        { 25,   8, 0, 16, flat, unbanked }, { 26,   8, 0, 16, flat, unbanked }, { 27,   8, 0, 16, flat, unbanked }, { 28,   8, 0, 16, flat, unbanked }, { 29,   8, 0, 16, flat, unbanked },
        { 30,   8, 0, 16, flat, unbanked }, { 31,   8, 0, 16, flat, unbanked }, { 32,   8, 0, 16, flat, unbanked }, { 33,   8, 0, 16, flat, unbanked }, { 34,   8, 0, 16, flat, unbanked },
        { 35,   8, 0, 17, flat, unbanked }, { 36,   7, 0, 17, flat, unbanked }, { 37,   7, 0, 17, flat, unbanked }, { 38,   7, 0, 17, flat, unbanked }, { 39,   7, 0, 17, flat, unbanked },
        { 40,   7, 0, 17, flat, unbanked }, { 41,   7, 0, 17, flat, unbanked }, { 42,   7, 0, 17, flat, unbanked }, { 43,   6, 0, 17, flat, unbanked }, { 44,   6, 0, 17, flat, unbanked },
        { 45,   6, 0, 17, flat, unbanked }, { 46,   6, 0, 17, flat, unbanked }, { 47,   6, 0, 17, flat, unbanked }, { 48,   6, 0, 17, flat, unbanked }, { 49,   5, 0, 17, flat, unbanked },
        { 50,   5, 0, 17, flat, unbanked }, { 51,   5, 0, 17, flat, unbanked }, { 52,   5, 0, 17, flat, unbanked }, { 53,   4, 0, 17, flat, unbanked }, { 54,   4, 0, 17, flat, unbanked },
        { 55,   4, 0, 17, flat, unbanked }, { 56,   4, 0, 18, flat, unbanked }, { 57,   3, 0, 18, flat, unbanked }, { 58,   3, 0, 18, flat, unbanked }, { 59,   3, 0, 18, flat, unbanked },
        { 60,   2, 0, 18, flat, unbanked }, { 61,   2, 0, 18, flat, unbanked }, { 62,   1, 0, 18, flat, unbanked }, { 63,   1, 0, 18, flat, unbanked }, { 64,   1, 0, 18, flat, unbanked },
        { 65,   0, 0, 18, flat, unbanked }, { 66,   0, 0, 18, flat, unbanked }, { 67,  -1, 0, 18, flat, unbanked }, { 68,  -1, 0, 18, flat, unbanked }, { 69,  -2, 0, 18, flat, unbanked },
        { 70,  -2, 0, 19, flat, unbanked }, { 71,  -3, 0, 19, flat, unbanked }, { 72,  -3, 0, 19, flat, unbanked }, { 73,  -4, 0, 19, flat, unbanked }, { 74,  -5, 0, 19, flat, unbanked },
        { 75,  -5, 0, 19, flat, unbanked }, { 76,  -6, 0, 19, flat, unbanked }, { 77,  -7, 0, 19, flat, unbanked }, { 78,  -8, 0, 19, flat, unbanked }, { 79,  -8, 0, 19, flat, unbanked },
        { 80,  -9, 0, 20, flat, unbanked }, { 81, -10, 0, 20, flat, unbanked }, { 82, -11, 0, 20, flat, unbanked }, { 83, -12, 0, 20, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightEighthToDiag_3, {
        {   9,  31, 0, 24, flat, unbanked }, {   9,  30, 0, 24, flat, unbanked }, {   9,  29, 0, 24, flat, unbanked }, {   9,  28, 0, 24, flat, unbanked }, {   9,  27, 0, 24, flat, unbanked },
        {   9,  26, 0, 24, flat, unbanked }, {   9,  25, 0, 24, flat, unbanked }, {   9,  24, 0, 24, flat, unbanked }, {   9,  23, 0, 24, flat, unbanked }, {   9,  22, 0, 24, flat, unbanked },
        {   9,  21, 0, 24, flat, unbanked }, {   9,  20, 0, 24, flat, unbanked }, {   9,  19, 0, 24, flat, unbanked }, {   9,  18, 0, 24, flat, unbanked }, {   9,  17, 0, 24, flat, unbanked },
        {   9,  16, 0, 24, flat, unbanked }, {   9,  15, 0, 24, flat, unbanked }, {   9,  14, 0, 24, flat, unbanked }, {   9,  13, 0, 24, flat, unbanked }, {   8,  12, 0, 24, flat, unbanked },
        {   8,  11, 0, 24, flat, unbanked }, {   8,  10, 0, 24, flat, unbanked }, {   8,   9, 0, 24, flat, unbanked }, {   8,   8, 0, 24, flat, unbanked }, {   8,   7, 0, 24, flat, unbanked },
        {   8,   6, 0, 24, flat, unbanked }, {   8,   5, 0, 24, flat, unbanked }, {   8,   4, 0, 24, flat, unbanked }, {   8,   3, 0, 24, flat, unbanked }, {   8,   2, 0, 24, flat, unbanked },
        {   8,   1, 0, 24, flat, unbanked }, {   8,   0, 0, 24, flat, unbanked }, {   8,  -1, 0, 24, flat, unbanked }, {   8,  -2, 0, 24, flat, unbanked }, {   8,  -3, 0, 25, flat, unbanked },
        {   7,  -4, 0, 25, flat, unbanked }, {   7,  -5, 0, 25, flat, unbanked }, {   7,  -6, 0, 25, flat, unbanked }, {   7,  -7, 0, 25, flat, unbanked }, {   7,  -8, 0, 25, flat, unbanked },
        {   7,  -9, 0, 25, flat, unbanked }, {   7, -10, 0, 25, flat, unbanked }, {   6, -11, 0, 25, flat, unbanked }, {   6, -12, 0, 25, flat, unbanked }, {   6, -13, 0, 25, flat, unbanked },
        {   6, -14, 0, 25, flat, unbanked }, {   6, -15, 0, 25, flat, unbanked }, {   6, -16, 0, 25, flat, unbanked }, {   5, -17, 0, 25, flat, unbanked }, {   5, -18, 0, 25, flat, unbanked },
        {   5, -19, 0, 25, flat, unbanked }, {   5, -20, 0, 25, flat, unbanked }, {   4, -21, 0, 25, flat, unbanked }, {   4, -22, 0, 25, flat, unbanked }, {   4, -23, 0, 25, flat, unbanked },
        {   4, -24, 0, 26, flat, unbanked }, {   3, -25, 0, 26, flat, unbanked }, {   3, -26, 0, 26, flat, unbanked }, {   3, -27, 0, 26, flat, unbanked }, {   2, -28, 0, 26, flat, unbanked },
        {   2, -29, 0, 26, flat, unbanked }, {   1, -30, 0, 26, flat, unbanked }, {   1, -31, 0, 26, flat, unbanked }, {   1, -32, 0, 26, flat, unbanked }, {   0, -33, 0, 26, flat, unbanked },
        {   0, -34, 0, 26, flat, unbanked }, {  -1, -35, 0, 26, flat, unbanked }, {  -1, -36, 0, 26, flat, unbanked }, {  -2, -37, 0, 26, flat, unbanked }, {  -2, -38, 0, 27, flat, unbanked },
        {  -3, -39, 0, 27, flat, unbanked }, {  -3, -40, 0, 27, flat, unbanked }, {  -4, -41, 0, 27, flat, unbanked }, {  -5, -42, 0, 27, flat, unbanked }, {  -5, -43, 0, 27, flat, unbanked },
        {  -6, -44, 0, 27, flat, unbanked }, {  -7, -45, 0, 27, flat, unbanked }, {  -8, -46, 0, 27, flat, unbanked }, {  -8, -47, 0, 27, flat, unbanked }, {  -9, -48, 0, 28, flat, unbanked },
        { -10, -49, 0, 28, flat, unbanked }, { -11, -50, 0, 28, flat, unbanked }, { -12, -51, 0, 28, flat, unbanked }, { -12, -52, 0, 28, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftEighthToOrthogonal_0, {
        {  20, 21, 0, 4, flat, unbanked }, {  19, 22, 0, 4, flat, unbanked }, {  18, 23, 0, 4, flat, unbanked }, {  17, 24, 0, 3, flat, unbanked }, {  16, 25, 0, 3, flat, unbanked },
        {  15, 26, 0, 3, flat, unbanked }, {  14, 27, 0, 3, flat, unbanked }, {  13, 27, 0, 3, flat, unbanked }, {  12, 28, 0, 3, flat, unbanked }, {  11, 28, 0, 3, flat, unbanked },
        {  10, 29, 0, 3, flat, unbanked }, {   9, 30, 0, 3, flat, unbanked }, {   8, 30, 0, 3, flat, unbanked }, {   7, 31, 0, 2, flat, unbanked }, {   6, 31, 0, 2, flat, unbanked },
        {   5, 32, 0, 2, flat, unbanked }, {   4, 32, 0, 2, flat, unbanked }, {   3, 33, 0, 2, flat, unbanked }, {   2, 33, 0, 2, flat, unbanked }, {   1, 34, 0, 2, flat, unbanked },
        {   0, 34, 0, 2, flat, unbanked }, {  -1, 34, 0, 2, flat, unbanked }, {  -2, 35, 0, 2, flat, unbanked }, {  -3, 35, 0, 2, flat, unbanked }, {  -4, 35, 0, 2, flat, unbanked },
        {  -5, 36, 0, 2, flat, unbanked }, {  -6, 36, 0, 2, flat, unbanked }, {  -7, 36, 0, 1, flat, unbanked }, {  -8, 37, 0, 1, flat, unbanked }, {  -9, 37, 0, 1, flat, unbanked },
        { -10, 37, 0, 1, flat, unbanked }, { -11, 38, 0, 1, flat, unbanked }, { -12, 38, 0, 1, flat, unbanked }, { -13, 38, 0, 1, flat, unbanked }, { -14, 38, 0, 1, flat, unbanked },
        { -15, 38, 0, 1, flat, unbanked }, { -16, 39, 0, 1, flat, unbanked }, { -17, 39, 0, 1, flat, unbanked }, { -18, 39, 0, 1, flat, unbanked }, { -19, 39, 0, 1, flat, unbanked },
        { -20, 39, 0, 1, flat, unbanked }, { -21, 40, 0, 1, flat, unbanked }, { -22, 40, 0, 1, flat, unbanked }, { -23, 40, 0, 1, flat, unbanked }, { -24, 40, 0, 1, flat, unbanked },
        { -25, 40, 0, 1, flat, unbanked }, { -26, 40, 0, 1, flat, unbanked }, { -27, 40, 0, 1, flat, unbanked }, { -28, 40, 0, 1, flat, unbanked }, { -29, 41, 0, 0, flat, unbanked },
        { -30, 41, 0, 0, flat, unbanked }, { -31, 41, 0, 0, flat, unbanked }, { -32, 41, 0, 0, flat, unbanked }, { -33, 41, 0, 0, flat, unbanked }, { -34, 41, 0, 0, flat, unbanked },
        { -35, 41, 0, 0, flat, unbanked }, { -36, 41, 0, 0, flat, unbanked }, { -37, 41, 0, 0, flat, unbanked }, { -38, 41, 0, 0, flat, unbanked }, { -39, 41, 0, 0, flat, unbanked },
        { -40, 41, 0, 0, flat, unbanked }, { -41, 41, 0, 0, flat, unbanked }, { -42, 41, 0, 0, flat, unbanked }, { -43, 41, 0, 0, flat, unbanked }, { -44, 41, 0, 0, flat, unbanked },
        { -45, 41, 0, 0, flat, unbanked }, { -46, 41, 0, 0, flat, unbanked }, { -47, 42, 0, 0, flat, unbanked }, { -48, 42, 0, 0, flat, unbanked }, { -49, 42, 0, 0, flat, unbanked },
        { -50, 42, 0, 0, flat, unbanked }, { -51, 42, 0, 0, flat, unbanked }, { -52, 42, 0, 0, flat, unbanked }, { -53, 42, 0, 0, flat, unbanked }, { -54, 42, 0, 0, flat, unbanked },
        { -55, 42, 0, 0, flat, unbanked }, { -56, 42, 0, 0, flat, unbanked }, { -57, 42, 0, 0, flat, unbanked }, { -58, 42, 0, 0, flat, unbanked }, { -59, 42, 0, 0, flat, unbanked },
        { -60, 42, 0, 0, flat, unbanked }, { -61, 42, 0, 0, flat, unbanked }, { -62, 42, 0, 0, flat, unbanked }, { -63, 42, 0, 0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftEighthToOrthogonal_1, {
        { 21, 11, 0, 12, flat, unbanked }, { 22, 12, 0, 12, flat, unbanked }, { 23, 13, 0, 12, flat, unbanked }, { 24, 14, 0, 12, flat, unbanked }, { 25, 15, 0, 11, flat, unbanked },
        { 26, 16, 0, 11, flat, unbanked }, { 27, 17, 0, 11, flat, unbanked }, { 28, 18, 0, 11, flat, unbanked }, { 28, 19, 0, 11, flat, unbanked }, { 29, 20, 0, 11, flat, unbanked },
        { 29, 21, 0, 11, flat, unbanked }, { 30, 22, 0, 11, flat, unbanked }, { 31, 23, 0, 11, flat, unbanked }, { 31, 24, 0, 11, flat, unbanked }, { 32, 25, 0, 10, flat, unbanked },
        { 32, 26, 0, 10, flat, unbanked }, { 33, 27, 0, 10, flat, unbanked }, { 33, 28, 0, 10, flat, unbanked }, { 34, 29, 0, 10, flat, unbanked }, { 34, 30, 0, 10, flat, unbanked },
        { 35, 31, 0, 10, flat, unbanked }, { 35, 32, 0, 10, flat, unbanked }, { 35, 33, 0, 10, flat, unbanked }, { 36, 34, 0, 10, flat, unbanked }, { 36, 35, 0, 10, flat, unbanked },
        { 36, 36, 0, 10, flat, unbanked }, { 37, 37, 0, 10, flat, unbanked }, { 37, 38, 0, 10, flat, unbanked }, { 37, 39, 0,  9, flat, unbanked }, { 38, 40, 0,  9, flat, unbanked },
        { 38, 41, 0,  9, flat, unbanked }, { 38, 42, 0,  9, flat, unbanked }, { 39, 43, 0,  9, flat, unbanked }, { 39, 44, 0,  9, flat, unbanked }, { 39, 45, 0,  9, flat, unbanked },
        { 39, 46, 0,  9, flat, unbanked }, { 39, 47, 0,  9, flat, unbanked }, { 40, 48, 0,  9, flat, unbanked }, { 40, 49, 0,  9, flat, unbanked }, { 40, 50, 0,  9, flat, unbanked },
        { 40, 51, 0,  9, flat, unbanked }, { 40, 52, 0,  9, flat, unbanked }, { 41, 53, 0,  9, flat, unbanked }, { 41, 54, 0,  9, flat, unbanked }, { 41, 55, 0,  9, flat, unbanked },
        { 41, 56, 0,  9, flat, unbanked }, { 41, 57, 0,  9, flat, unbanked }, { 41, 58, 0,  9, flat, unbanked }, { 41, 59, 0,  9, flat, unbanked }, { 41, 60, 0,  9, flat, unbanked },
        { 42, 61, 0,  8, flat, unbanked }, { 42, 62, 0,  8, flat, unbanked }, { 42, 63, 0,  8, flat, unbanked }, { 42, 64, 0,  8, flat, unbanked }, { 42, 65, 0,  8, flat, unbanked },
        { 42, 66, 0,  8, flat, unbanked }, { 42, 67, 0,  8, flat, unbanked }, { 42, 68, 0,  8, flat, unbanked }, { 42, 69, 0,  8, flat, unbanked }, { 42, 70, 0,  8, flat, unbanked },
        { 42, 71, 0,  8, flat, unbanked }, { 42, 72, 0,  8, flat, unbanked }, { 42, 73, 0,  8, flat, unbanked }, { 42, 74, 0,  8, flat, unbanked }, { 42, 75, 0,  8, flat, unbanked },
        { 42, 76, 0,  8, flat, unbanked }, { 42, 77, 0,  8, flat, unbanked }, { 42, 78, 0,  8, flat, unbanked }, { 43, 79, 0,  8, flat, unbanked }, { 43, 80, 0,  8, flat, unbanked },
        { 43, 81, 0,  8, flat, unbanked }, { 43, 82, 0,  8, flat, unbanked }, { 43, 83, 0,  8, flat, unbanked }, { 43, 84, 0,  8, flat, unbanked }, { 43, 85, 0,  8, flat, unbanked },
        { 43, 86, 0,  8, flat, unbanked }, { 43, 87, 0,  8, flat, unbanked }, { 43, 88, 0,  8, flat, unbanked }, { 43, 89, 0,  8, flat, unbanked }, { 43, 90, 0,  8, flat, unbanked },
        { 43, 91, 0,  8, flat, unbanked }, { 43, 92, 0,  8, flat, unbanked }, { 43, 93, 0,  8, flat, unbanked }, { 43, 94, 0,  8, flat, unbanked }, { 43, 95, 0,  8, flat, unbanked },
        { 43, 96, 0,  8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftEighthToOrthogonal_2, {
        { 11,  10, 0, 20, flat, unbanked }, { 12,   9, 0, 20, flat, unbanked }, { 13,   8, 0, 20, flat, unbanked }, { 14,   7, 0, 20, flat, unbanked }, { 15,   6, 0, 19, flat, unbanked },
        { 16,   5, 0, 19, flat, unbanked }, { 17,   4, 0, 19, flat, unbanked }, { 18,   3, 0, 19, flat, unbanked }, { 19,   3, 0, 19, flat, unbanked }, { 20,   2, 0, 19, flat, unbanked },
        { 21,   2, 0, 19, flat, unbanked }, { 22,   1, 0, 19, flat, unbanked }, { 23,   0, 0, 19, flat, unbanked }, { 24,   0, 0, 19, flat, unbanked }, { 25,  -1, 0, 18, flat, unbanked },
        { 26,  -1, 0, 18, flat, unbanked }, { 27,  -2, 0, 18, flat, unbanked }, { 28,  -2, 0, 18, flat, unbanked }, { 29,  -3, 0, 18, flat, unbanked }, { 30,  -3, 0, 18, flat, unbanked },
        { 31,  -4, 0, 18, flat, unbanked }, { 32,  -4, 0, 18, flat, unbanked }, { 33,  -4, 0, 18, flat, unbanked }, { 34,  -5, 0, 18, flat, unbanked }, { 35,  -5, 0, 18, flat, unbanked },
        { 36,  -5, 0, 18, flat, unbanked }, { 37,  -6, 0, 18, flat, unbanked }, { 38,  -6, 0, 18, flat, unbanked }, { 39,  -6, 0, 17, flat, unbanked }, { 40,  -7, 0, 17, flat, unbanked },
        { 41,  -7, 0, 17, flat, unbanked }, { 42,  -7, 0, 17, flat, unbanked }, { 43,  -8, 0, 17, flat, unbanked }, { 44,  -8, 0, 17, flat, unbanked }, { 45,  -8, 0, 17, flat, unbanked },
        { 46,  -8, 0, 17, flat, unbanked }, { 47,  -8, 0, 17, flat, unbanked }, { 48,  -9, 0, 17, flat, unbanked }, { 49,  -9, 0, 17, flat, unbanked }, { 50,  -9, 0, 17, flat, unbanked },
        { 51,  -9, 0, 17, flat, unbanked }, { 52,  -9, 0, 17, flat, unbanked }, { 53, -10, 0, 17, flat, unbanked }, { 54, -10, 0, 17, flat, unbanked }, { 55, -10, 0, 17, flat, unbanked },
        { 56, -10, 0, 17, flat, unbanked }, { 57, -10, 0, 17, flat, unbanked }, { 58, -10, 0, 17, flat, unbanked }, { 59, -10, 0, 17, flat, unbanked }, { 60, -10, 0, 17, flat, unbanked },
        { 61, -11, 0, 16, flat, unbanked }, { 62, -11, 0, 16, flat, unbanked }, { 63, -11, 0, 16, flat, unbanked }, { 64, -11, 0, 16, flat, unbanked }, { 65, -11, 0, 16, flat, unbanked },
        { 66, -11, 0, 16, flat, unbanked }, { 67, -11, 0, 16, flat, unbanked }, { 68, -11, 0, 16, flat, unbanked }, { 69, -11, 0, 16, flat, unbanked }, { 70, -11, 0, 16, flat, unbanked },
        { 71, -11, 0, 16, flat, unbanked }, { 72, -11, 0, 16, flat, unbanked }, { 73, -11, 0, 16, flat, unbanked }, { 74, -11, 0, 16, flat, unbanked }, { 75, -11, 0, 16, flat, unbanked },
        { 76, -11, 0, 16, flat, unbanked }, { 77, -11, 0, 16, flat, unbanked }, { 78, -11, 0, 16, flat, unbanked }, { 79, -12, 0, 16, flat, unbanked }, { 80, -12, 0, 16, flat, unbanked },
        { 81, -12, 0, 16, flat, unbanked }, { 82, -12, 0, 16, flat, unbanked }, { 83, -12, 0, 16, flat, unbanked }, { 84, -12, 0, 16, flat, unbanked }, { 85, -12, 0, 16, flat, unbanked },
        { 86, -12, 0, 16, flat, unbanked }, { 87, -12, 0, 16, flat, unbanked }, { 88, -12, 0, 16, flat, unbanked }, { 89, -12, 0, 16, flat, unbanked }, { 90, -12, 0, 16, flat, unbanked },
        { 91, -12, 0, 16, flat, unbanked }, { 92, -12, 0, 16, flat, unbanked }, { 93, -12, 0, 16, flat, unbanked }, { 94, -12, 0, 16, flat, unbanked }, { 95, -12, 0, 16, flat, unbanked },
        { 96, -12, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(LeftEighthToOrthogonal_3, {
        {  10,  20, 0, 28, flat, unbanked }, {   9,  19, 0, 28, flat, unbanked }, {   8,  18, 0, 28, flat, unbanked }, {   7,  17, 0, 27, flat, unbanked }, {   6,  16, 0, 27, flat, unbanked },
        {   5,  15, 0, 27, flat, unbanked }, {   4,  14, 0, 27, flat, unbanked }, {   4,  13, 0, 27, flat, unbanked }, {   3,  12, 0, 27, flat, unbanked }, {   3,  11, 0, 27, flat, unbanked },
        {   2,  10, 0, 27, flat, unbanked }, {   1,   9, 0, 27, flat, unbanked }, {   1,   8, 0, 27, flat, unbanked }, {   0,   7, 0, 26, flat, unbanked }, {   0,   6, 0, 26, flat, unbanked },
        {  -1,   5, 0, 26, flat, unbanked }, {  -1,   4, 0, 26, flat, unbanked }, {  -2,   3, 0, 26, flat, unbanked }, {  -2,   2, 0, 26, flat, unbanked }, {  -3,   1, 0, 26, flat, unbanked },
        {  -3,   0, 0, 26, flat, unbanked }, {  -3,  -1, 0, 26, flat, unbanked }, {  -4,  -2, 0, 26, flat, unbanked }, {  -4,  -3, 0, 26, flat, unbanked }, {  -4,  -4, 0, 26, flat, unbanked },
        {  -5,  -5, 0, 26, flat, unbanked }, {  -5,  -6, 0, 26, flat, unbanked }, {  -5,  -7, 0, 25, flat, unbanked }, {  -6,  -8, 0, 25, flat, unbanked }, {  -6,  -9, 0, 25, flat, unbanked },
        {  -6, -10, 0, 25, flat, unbanked }, {  -7, -11, 0, 25, flat, unbanked }, {  -7, -12, 0, 25, flat, unbanked }, {  -7, -13, 0, 25, flat, unbanked }, {  -7, -14, 0, 25, flat, unbanked },
        {  -7, -15, 0, 25, flat, unbanked }, {  -8, -16, 0, 25, flat, unbanked }, {  -8, -17, 0, 25, flat, unbanked }, {  -8, -18, 0, 25, flat, unbanked }, {  -8, -19, 0, 25, flat, unbanked },
        {  -8, -20, 0, 25, flat, unbanked }, {  -9, -21, 0, 25, flat, unbanked }, {  -9, -22, 0, 25, flat, unbanked }, {  -9, -23, 0, 25, flat, unbanked }, {  -9, -24, 0, 25, flat, unbanked },
        {  -9, -25, 0, 25, flat, unbanked }, {  -9, -26, 0, 25, flat, unbanked }, {  -9, -27, 0, 25, flat, unbanked }, {  -9, -28, 0, 25, flat, unbanked }, { -10, -29, 0, 24, flat, unbanked },
        { -10, -30, 0, 24, flat, unbanked }, { -10, -31, 0, 24, flat, unbanked }, { -10, -32, 0, 24, flat, unbanked }, { -10, -33, 0, 24, flat, unbanked }, { -10, -34, 0, 24, flat, unbanked },
        { -10, -35, 0, 24, flat, unbanked }, { -10, -36, 0, 24, flat, unbanked }, { -10, -37, 0, 24, flat, unbanked }, { -10, -38, 0, 24, flat, unbanked }, { -10, -39, 0, 24, flat, unbanked },
        { -10, -40, 0, 24, flat, unbanked }, { -10, -41, 0, 24, flat, unbanked }, { -10, -42, 0, 24, flat, unbanked }, { -10, -43, 0, 24, flat, unbanked }, { -10, -44, 0, 24, flat, unbanked },
        { -10, -45, 0, 24, flat, unbanked }, { -10, -46, 0, 24, flat, unbanked }, { -11, -47, 0, 24, flat, unbanked }, { -11, -48, 0, 24, flat, unbanked }, { -11, -49, 0, 24, flat, unbanked },
        { -11, -50, 0, 24, flat, unbanked }, { -11, -51, 0, 24, flat, unbanked }, { -11, -52, 0, 24, flat, unbanked }, { -11, -53, 0, 24, flat, unbanked }, { -11, -54, 0, 24, flat, unbanked },
        { -11, -55, 0, 24, flat, unbanked }, { -11, -56, 0, 24, flat, unbanked }, { -11, -57, 0, 24, flat, unbanked }, { -11, -58, 0, 24, flat, unbanked }, { -11, -59, 0, 24, flat, unbanked },
        { -11, -60, 0, 24, flat, unbanked }, { -11, -61, 0, 24, flat, unbanked }, { -11, -62, 0, 24, flat, unbanked }, { -11, -63, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightEighthToOrthogonal_0, {
        {  20, 21, 0, 4, flat, unbanked }, {  19, 22, 0, 4, flat, unbanked }, {  18, 23, 0, 4, flat, unbanked }, {  17, 24, 0, 4, flat, unbanked }, {  16, 25, 0, 4, flat, unbanked },
        {  15, 26, 0, 4, flat, unbanked }, {  14, 27, 0, 4, flat, unbanked }, {  13, 28, 0, 4, flat, unbanked }, {  12, 29, 0, 4, flat, unbanked }, {  11, 30, 0, 4, flat, unbanked },
        {  10, 31, 0, 4, flat, unbanked }, {   9, 32, 0, 4, flat, unbanked }, {   9, 33, 0, 4, flat, unbanked }, {   8, 34, 0, 4, flat, unbanked }, {   7, 35, 0, 4, flat, unbanked },
        {   6, 36, 0, 4, flat, unbanked }, {   5, 37, 0, 4, flat, unbanked }, {   4, 38, 0, 4, flat, unbanked }, {   3, 39, 0, 4, flat, unbanked }, {   2, 40, 0, 4, flat, unbanked },
        {   2, 41, 0, 4, flat, unbanked }, {   1, 42, 0, 5, flat, unbanked }, {   0, 43, 0, 5, flat, unbanked }, {  -1, 44, 0, 5, flat, unbanked }, {  -2, 45, 0, 5, flat, unbanked },
        {  -2, 46, 0, 5, flat, unbanked }, {  -3, 47, 0, 5, flat, unbanked }, {  -4, 48, 0, 5, flat, unbanked }, {  -5, 49, 0, 5, flat, unbanked }, {  -5, 50, 0, 5, flat, unbanked },
        {  -6, 51, 0, 5, flat, unbanked }, {  -7, 52, 0, 5, flat, unbanked }, {  -7, 53, 0, 5, flat, unbanked }, {  -8, 54, 0, 5, flat, unbanked }, {  -9, 55, 0, 5, flat, unbanked },
        {  -9, 56, 0, 5, flat, unbanked }, { -10, 57, 0, 5, flat, unbanked }, { -11, 58, 0, 5, flat, unbanked }, { -11, 59, 0, 5, flat, unbanked }, { -12, 60, 0, 5, flat, unbanked },
        { -12, 61, 0, 5, flat, unbanked }, { -13, 62, 0, 6, flat, unbanked }, { -13, 63, 0, 6, flat, unbanked }, { -14, 64, 0, 6, flat, unbanked }, { -14, 65, 0, 6, flat, unbanked },
        { -15, 66, 0, 6, flat, unbanked }, { -15, 67, 0, 6, flat, unbanked }, { -16, 68, 0, 6, flat, unbanked }, { -16, 69, 0, 6, flat, unbanked }, { -16, 70, 0, 6, flat, unbanked },
        { -17, 71, 0, 6, flat, unbanked }, { -17, 72, 0, 6, flat, unbanked }, { -18, 73, 0, 6, flat, unbanked }, { -18, 74, 0, 6, flat, unbanked }, { -18, 75, 0, 6, flat, unbanked },
        { -19, 76, 0, 7, flat, unbanked }, { -19, 77, 0, 7, flat, unbanked }, { -19, 78, 0, 7, flat, unbanked }, { -19, 79, 0, 7, flat, unbanked }, { -20, 80, 0, 7, flat, unbanked },
        { -20, 81, 0, 7, flat, unbanked }, { -20, 82, 0, 7, flat, unbanked }, { -20, 83, 0, 7, flat, unbanked }, { -20, 84, 0, 7, flat, unbanked }, { -21, 85, 0, 7, flat, unbanked },
        { -21, 86, 0, 7, flat, unbanked }, { -21, 87, 0, 7, flat, unbanked }, { -21, 88, 0, 7, flat, unbanked }, { -21, 89, 0, 8, flat, unbanked }, { -21, 90, 0, 8, flat, unbanked },
        { -21, 91, 0, 8, flat, unbanked }, { -21, 92, 0, 8, flat, unbanked }, { -21, 93, 0, 8, flat, unbanked }, { -21, 94, 0, 8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightEighthToOrthogonal_1, {
        { 21, 11, 0, 12, flat, unbanked }, { 22, 12, 0, 12, flat, unbanked }, { 23, 13, 0, 12, flat, unbanked }, { 24, 14, 0, 12, flat, unbanked }, { 25, 15, 0, 12, flat, unbanked },
        { 26, 16, 0, 12, flat, unbanked }, { 27, 17, 0, 12, flat, unbanked }, { 28, 18, 0, 12, flat, unbanked }, { 29, 19, 0, 12, flat, unbanked }, { 30, 20, 0, 12, flat, unbanked },
        { 31, 21, 0, 12, flat, unbanked }, { 32, 22, 0, 12, flat, unbanked }, { 33, 23, 0, 12, flat, unbanked }, { 34, 23, 0, 12, flat, unbanked }, { 35, 24, 0, 12, flat, unbanked },
        { 36, 25, 0, 12, flat, unbanked }, { 37, 26, 0, 12, flat, unbanked }, { 38, 27, 0, 12, flat, unbanked }, { 39, 28, 0, 12, flat, unbanked }, { 40, 29, 0, 12, flat, unbanked },
        { 41, 30, 0, 12, flat, unbanked }, { 42, 30, 0, 12, flat, unbanked }, { 43, 31, 0, 13, flat, unbanked }, { 44, 32, 0, 13, flat, unbanked }, { 45, 33, 0, 13, flat, unbanked },
        { 46, 34, 0, 13, flat, unbanked }, { 47, 34, 0, 13, flat, unbanked }, { 48, 35, 0, 13, flat, unbanked }, { 49, 36, 0, 13, flat, unbanked }, { 50, 37, 0, 13, flat, unbanked },
        { 51, 37, 0, 13, flat, unbanked }, { 52, 38, 0, 13, flat, unbanked }, { 53, 39, 0, 13, flat, unbanked }, { 54, 39, 0, 13, flat, unbanked }, { 55, 40, 0, 13, flat, unbanked },
        { 56, 41, 0, 13, flat, unbanked }, { 57, 41, 0, 13, flat, unbanked }, { 58, 42, 0, 13, flat, unbanked }, { 59, 43, 0, 13, flat, unbanked }, { 60, 43, 0, 13, flat, unbanked },
        { 61, 44, 0, 13, flat, unbanked }, { 62, 44, 0, 13, flat, unbanked }, { 63, 45, 0, 14, flat, unbanked }, { 64, 45, 0, 14, flat, unbanked }, { 65, 46, 0, 14, flat, unbanked },
        { 66, 46, 0, 14, flat, unbanked }, { 67, 47, 0, 14, flat, unbanked }, { 68, 47, 0, 14, flat, unbanked }, { 69, 48, 0, 14, flat, unbanked }, { 70, 48, 0, 14, flat, unbanked },
        { 71, 48, 0, 14, flat, unbanked }, { 72, 49, 0, 14, flat, unbanked }, { 73, 49, 0, 14, flat, unbanked }, { 74, 50, 0, 14, flat, unbanked }, { 75, 50, 0, 14, flat, unbanked },
        { 76, 50, 0, 14, flat, unbanked }, { 77, 51, 0, 15, flat, unbanked }, { 78, 51, 0, 15, flat, unbanked }, { 79, 51, 0, 15, flat, unbanked }, { 80, 51, 0, 15, flat, unbanked },
        { 81, 52, 0, 15, flat, unbanked }, { 82, 52, 0, 15, flat, unbanked }, { 83, 52, 0, 15, flat, unbanked }, { 84, 52, 0, 15, flat, unbanked }, { 85, 52, 0, 15, flat, unbanked },
        { 86, 53, 0, 15, flat, unbanked }, { 87, 53, 0, 15, flat, unbanked }, { 88, 53, 0, 15, flat, unbanked }, { 89, 53, 0, 15, flat, unbanked }, { 90, 53, 0, 16, flat, unbanked },
        { 91, 53, 0, 16, flat, unbanked }, { 92, 53, 0, 16, flat, unbanked }, { 93, 53, 0, 16, flat, unbanked }, { 94, 53, 0, 16, flat, unbanked }, { 95, 53, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightEighthToOrthogonal_2, {
        { 11,  10, 0, 20, flat, unbanked }, { 12,   9, 0, 20, flat, unbanked }, { 13,   8, 0, 20, flat, unbanked }, { 14,   7, 0, 20, flat, unbanked }, { 15,   6, 0, 20, flat, unbanked },
        { 16,   5, 0, 20, flat, unbanked }, { 17,   4, 0, 20, flat, unbanked }, { 18,   3, 0, 20, flat, unbanked }, { 19,   2, 0, 20, flat, unbanked }, { 20,   1, 0, 20, flat, unbanked },
        { 21,   0, 0, 20, flat, unbanked }, { 22,  -1, 0, 20, flat, unbanked }, { 23,  -2, 0, 20, flat, unbanked }, { 23,  -3, 0, 20, flat, unbanked }, { 24,  -4, 0, 20, flat, unbanked },
        { 25,  -5, 0, 20, flat, unbanked }, { 26,  -6, 0, 20, flat, unbanked }, { 27,  -7, 0, 20, flat, unbanked }, { 28,  -8, 0, 20, flat, unbanked }, { 29,  -9, 0, 20, flat, unbanked },
        { 30, -10, 0, 20, flat, unbanked }, { 30, -11, 0, 20, flat, unbanked }, { 31, -12, 0, 21, flat, unbanked }, { 32, -13, 0, 21, flat, unbanked }, { 33, -14, 0, 21, flat, unbanked },
        { 34, -15, 0, 21, flat, unbanked }, { 34, -16, 0, 21, flat, unbanked }, { 35, -17, 0, 21, flat, unbanked }, { 36, -18, 0, 21, flat, unbanked }, { 37, -19, 0, 21, flat, unbanked },
        { 37, -20, 0, 21, flat, unbanked }, { 38, -21, 0, 21, flat, unbanked }, { 39, -22, 0, 21, flat, unbanked }, { 39, -23, 0, 21, flat, unbanked }, { 40, -24, 0, 21, flat, unbanked },
        { 41, -25, 0, 21, flat, unbanked }, { 41, -26, 0, 21, flat, unbanked }, { 42, -27, 0, 21, flat, unbanked }, { 43, -28, 0, 21, flat, unbanked }, { 43, -29, 0, 21, flat, unbanked },
        { 44, -30, 0, 21, flat, unbanked }, { 44, -31, 0, 21, flat, unbanked }, { 45, -32, 0, 22, flat, unbanked }, { 45, -33, 0, 22, flat, unbanked }, { 46, -34, 0, 22, flat, unbanked },
        { 46, -35, 0, 22, flat, unbanked }, { 47, -36, 0, 22, flat, unbanked }, { 47, -37, 0, 22, flat, unbanked }, { 48, -38, 0, 22, flat, unbanked }, { 48, -39, 0, 22, flat, unbanked },
        { 48, -40, 0, 22, flat, unbanked }, { 49, -41, 0, 22, flat, unbanked }, { 49, -42, 0, 22, flat, unbanked }, { 50, -43, 0, 22, flat, unbanked }, { 50, -44, 0, 22, flat, unbanked },
        { 50, -45, 0, 22, flat, unbanked }, { 51, -46, 0, 23, flat, unbanked }, { 51, -47, 0, 23, flat, unbanked }, { 51, -48, 0, 23, flat, unbanked }, { 51, -49, 0, 23, flat, unbanked },
        { 52, -50, 0, 23, flat, unbanked }, { 52, -51, 0, 23, flat, unbanked }, { 52, -52, 0, 23, flat, unbanked }, { 52, -53, 0, 23, flat, unbanked }, { 52, -54, 0, 23, flat, unbanked },
        { 53, -55, 0, 23, flat, unbanked }, { 53, -56, 0, 23, flat, unbanked }, { 53, -57, 0, 23, flat, unbanked }, { 53, -58, 0, 23, flat, unbanked }, { 53, -59, 0, 24, flat, unbanked },
        { 53, -60, 0, 24, flat, unbanked }, { 53, -61, 0, 24, flat, unbanked }, { 53, -62, 0, 24, flat, unbanked }, { 53, -63, 0, 24, flat, unbanked }, { 53, -64, 0, 24, flat, unbanked },
        { 53, -65, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(RightEighthToOrthogonal_3, {
        {  10,  20, 0, 28, flat, unbanked }, {   9,  19, 0, 28, flat, unbanked }, {   8,  18, 0, 28, flat, unbanked }, {   7,  17, 0, 28, flat, unbanked }, {   6,  16, 0, 28, flat, unbanked },
        {   5,  15, 0, 28, flat, unbanked }, {   4,  14, 0, 28, flat, unbanked }, {   3,  13, 0, 28, flat, unbanked }, {   2,  12, 0, 28, flat, unbanked }, {   1,  11, 0, 28, flat, unbanked },
        {   0,  10, 0, 28, flat, unbanked }, {  -1,   9, 0, 28, flat, unbanked }, {  -2,   9, 0, 28, flat, unbanked }, {  -3,   8, 0, 28, flat, unbanked }, {  -4,   7, 0, 28, flat, unbanked },
        {  -5,   6, 0, 28, flat, unbanked }, {  -6,   5, 0, 28, flat, unbanked }, {  -7,   4, 0, 28, flat, unbanked }, {  -8,   3, 0, 28, flat, unbanked }, {  -9,   2, 0, 28, flat, unbanked },
        { -10,   2, 0, 28, flat, unbanked }, { -11,   1, 0, 29, flat, unbanked }, { -12,   0, 0, 29, flat, unbanked }, { -13,  -1, 0, 29, flat, unbanked }, { -14,  -2, 0, 29, flat, unbanked },
        { -15,  -2, 0, 29, flat, unbanked }, { -16,  -3, 0, 29, flat, unbanked }, { -17,  -4, 0, 29, flat, unbanked }, { -18,  -5, 0, 29, flat, unbanked }, { -19,  -5, 0, 29, flat, unbanked },
        { -20,  -6, 0, 29, flat, unbanked }, { -21,  -7, 0, 29, flat, unbanked }, { -22,  -7, 0, 29, flat, unbanked }, { -23,  -8, 0, 29, flat, unbanked }, { -24,  -9, 0, 29, flat, unbanked },
        { -25,  -9, 0, 29, flat, unbanked }, { -26, -10, 0, 29, flat, unbanked }, { -27, -11, 0, 29, flat, unbanked }, { -28, -11, 0, 29, flat, unbanked }, { -29, -12, 0, 29, flat, unbanked },
        { -30, -12, 0, 29, flat, unbanked }, { -31, -13, 0, 30, flat, unbanked }, { -32, -13, 0, 30, flat, unbanked }, { -33, -14, 0, 30, flat, unbanked }, { -34, -14, 0, 30, flat, unbanked },
        { -35, -15, 0, 30, flat, unbanked }, { -36, -15, 0, 30, flat, unbanked }, { -37, -16, 0, 30, flat, unbanked }, { -38, -16, 0, 30, flat, unbanked }, { -39, -16, 0, 30, flat, unbanked },
        { -40, -17, 0, 30, flat, unbanked }, { -41, -17, 0, 30, flat, unbanked }, { -42, -18, 0, 30, flat, unbanked }, { -43, -18, 0, 30, flat, unbanked }, { -44, -18, 0, 30, flat, unbanked },
        { -45, -19, 0, 31, flat, unbanked }, { -46, -19, 0, 31, flat, unbanked }, { -47, -19, 0, 31, flat, unbanked }, { -48, -19, 0, 31, flat, unbanked }, { -49, -20, 0, 31, flat, unbanked },
        { -50, -20, 0, 31, flat, unbanked }, { -51, -20, 0, 31, flat, unbanked }, { -52, -20, 0, 31, flat, unbanked }, { -53, -20, 0, 31, flat, unbanked }, { -54, -21, 0, 31, flat, unbanked },
        { -55, -21, 0, 31, flat, unbanked }, { -56, -21, 0, 31, flat, unbanked }, { -57, -21, 0, 31, flat, unbanked }, { -58, -21, 0,  0, flat, unbanked }, { -59, -21, 0,  0, flat, unbanked },
        { -60, -21, 0,  0, flat, unbanked }, { -61, -21, 0,  0, flat, unbanked }, { -62, -21, 0,  0, flat, unbanked }, { -63, -21, 0,  0, flat, unbanked }, { -64, -21, 0,  0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(DiagFlat_0, {
        {  19, 20, 0, 4, flat, unbanked }, {  18, 21, 0, 4, flat, unbanked }, {  17, 22, 0, 3, flat, unbanked }, {  16, 22, 0, 3, flat, unbanked }, {  15, 23, 0, 3, flat, unbanked },
        {  14, 24, 0, 3, flat, unbanked }, {  13, 25, 0, 3, flat, unbanked }, {  12, 25, 0, 3, flat, unbanked }, {  11, 26, 0, 3, flat, unbanked }, {  10, 26, 0, 3, flat, unbanked },
        {   9, 27, 0, 3, flat, unbanked }, {   8, 27, 0, 2, flat, unbanked }, {   7, 28, 0, 2, flat, unbanked }, {   6, 28, 0, 2, flat, unbanked }, {   5, 29, 0, 2, flat, unbanked },
        {   4, 29, 0, 2, flat, unbanked }, {   3, 30, 0, 2, flat, unbanked }, {   2, 30, 0, 2, flat, unbanked }, {   1, 31, 0, 2, flat, unbanked }, {   0, 31, 0, 2, flat, unbanked },
        {  -1, 32, 0, 2, flat, unbanked }, {  -2, 32, 0, 2, flat, unbanked }, {  -3, 33, 0, 2, flat, unbanked }, {  -4, 33, 0, 2, flat, unbanked }, {  -5, 34, 0, 2, flat, unbanked },
        {  -6, 34, 0, 2, flat, unbanked }, {  -7, 35, 0, 2, flat, unbanked }, {  -8, 35, 0, 3, flat, unbanked }, {  -9, 36, 0, 3, flat, unbanked }, { -10, 36, 0, 3, flat, unbanked },
        { -11, 37, 0, 3, flat, unbanked }, { -12, 37, 0, 3, flat, unbanked }, { -13, 38, 0, 3, flat, unbanked }, { -14, 39, 0, 3, flat, unbanked }, { -15, 39, 0, 3, flat, unbanked },
        { -16, 40, 0, 3, flat, unbanked }, { -17, 41, 0, 4, flat, unbanked }, { -18, 42, 0, 4, flat, unbanked }, { -19, 43, 0, 4, flat, unbanked }, { -20, 43, 0, 4, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(DiagFlat_1, {
        { 20, 12, 0, 12, flat, unbanked }, { 21, 13, 0, 12, flat, unbanked }, { 22, 14, 0, 12, flat, unbanked }, { 23, 15, 0, 11, flat, unbanked }, { 23, 16, 0, 11, flat, unbanked },
        { 24, 17, 0, 11, flat, unbanked }, { 25, 18, 0, 11, flat, unbanked }, { 26, 19, 0, 11, flat, unbanked }, { 26, 20, 0, 11, flat, unbanked }, { 27, 21, 0, 11, flat, unbanked },
        { 27, 22, 0, 11, flat, unbanked }, { 28, 23, 0, 11, flat, unbanked }, { 28, 24, 0, 10, flat, unbanked }, { 29, 25, 0, 10, flat, unbanked }, { 29, 26, 0, 10, flat, unbanked },
        { 30, 27, 0, 10, flat, unbanked }, { 30, 28, 0, 10, flat, unbanked }, { 31, 29, 0, 10, flat, unbanked }, { 31, 30, 0, 10, flat, unbanked }, { 32, 31, 0, 10, flat, unbanked },
        { 32, 32, 0, 10, flat, unbanked }, { 33, 33, 0, 10, flat, unbanked }, { 33, 34, 0, 10, flat, unbanked }, { 34, 35, 0, 10, flat, unbanked }, { 34, 36, 0, 10, flat, unbanked },
        { 35, 37, 0, 10, flat, unbanked }, { 35, 38, 0, 10, flat, unbanked }, { 36, 39, 0, 10, flat, unbanked }, { 36, 40, 0, 11, flat, unbanked }, { 37, 41, 0, 11, flat, unbanked },
        { 37, 42, 0, 11, flat, unbanked }, { 38, 43, 0, 11, flat, unbanked }, { 38, 44, 0, 11, flat, unbanked }, { 39, 45, 0, 11, flat, unbanked }, { 40, 46, 0, 11, flat, unbanked },
        { 40, 47, 0, 11, flat, unbanked }, { 41, 48, 0, 11, flat, unbanked }, { 42, 49, 0, 12, flat, unbanked }, { 43, 50, 0, 12, flat, unbanked }, { 44, 51, 0, 12, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(DiagFlat_2, {
        { 12,  11, 0, 20, flat, unbanked }, { 13,  10, 0, 20, flat, unbanked }, { 14,   9, 0, 20, flat, unbanked }, { 15,   8, 0, 19, flat, unbanked }, { 16,   8, 0, 19, flat, unbanked },
        { 17,   7, 0, 19, flat, unbanked }, { 18,   6, 0, 19, flat, unbanked }, { 19,   5, 0, 19, flat, unbanked }, { 20,   5, 0, 19, flat, unbanked }, { 21,   4, 0, 19, flat, unbanked },
        { 22,   4, 0, 19, flat, unbanked }, { 23,   3, 0, 19, flat, unbanked }, { 24,   3, 0, 18, flat, unbanked }, { 25,   2, 0, 18, flat, unbanked }, { 26,   2, 0, 18, flat, unbanked },
        { 27,   1, 0, 18, flat, unbanked }, { 28,   1, 0, 18, flat, unbanked }, { 29,   0, 0, 18, flat, unbanked }, { 30,   0, 0, 18, flat, unbanked }, { 31,  -1, 0, 18, flat, unbanked },
        { 32,  -1, 0, 18, flat, unbanked }, { 33,  -2, 0, 18, flat, unbanked }, { 34,  -2, 0, 18, flat, unbanked }, { 35,  -3, 0, 18, flat, unbanked }, { 36,  -3, 0, 18, flat, unbanked },
        { 37,  -4, 0, 18, flat, unbanked }, { 38,  -4, 0, 18, flat, unbanked }, { 39,  -5, 0, 18, flat, unbanked }, { 40,  -5, 0, 19, flat, unbanked }, { 41,  -6, 0, 19, flat, unbanked },
        { 42,  -6, 0, 19, flat, unbanked }, { 43,  -7, 0, 19, flat, unbanked }, { 44,  -7, 0, 19, flat, unbanked }, { 45,  -8, 0, 19, flat, unbanked }, { 46,  -9, 0, 19, flat, unbanked },
        { 47,  -9, 0, 19, flat, unbanked }, { 48, -10, 0, 19, flat, unbanked }, { 49, -11, 0, 20, flat, unbanked }, { 50, -12, 0, 20, flat, unbanked }, { 51, -13, 0, 20, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(DiagFlat_3, {
        {  11,  19, 0, 28, flat, unbanked }, {  10,  18, 0, 28, flat, unbanked }, {   9,  17, 0, 27, flat, unbanked }, {   9,  16, 0, 27, flat, unbanked }, {   8,  15, 0, 27, flat, unbanked },
        {   7,  14, 0, 27, flat, unbanked }, {   6,  13, 0, 27, flat, unbanked }, {   6,  12, 0, 27, flat, unbanked }, {   5,  11, 0, 27, flat, unbanked }, {   5,  10, 0, 27, flat, unbanked },
        {   4,   9, 0, 27, flat, unbanked }, {   4,   8, 0, 26, flat, unbanked }, {   3,   7, 0, 26, flat, unbanked }, {   3,   6, 0, 26, flat, unbanked }, {   2,   5, 0, 26, flat, unbanked },
        {   2,   4, 0, 26, flat, unbanked }, {   1,   3, 0, 26, flat, unbanked }, {   1,   2, 0, 26, flat, unbanked }, {   0,   1, 0, 26, flat, unbanked }, {   0,   0, 0, 26, flat, unbanked },
        {  -1,  -1, 0, 26, flat, unbanked }, {  -1,  -2, 0, 26, flat, unbanked }, {  -2,  -3, 0, 26, flat, unbanked }, {  -2,  -4, 0, 26, flat, unbanked }, {  -3,  -5, 0, 26, flat, unbanked },
        {  -3,  -6, 0, 26, flat, unbanked }, {  -4,  -7, 0, 26, flat, unbanked }, {  -4,  -8, 0, 27, flat, unbanked }, {  -5,  -9, 0, 27, flat, unbanked }, {  -5, -10, 0, 27, flat, unbanked },
        {  -6, -11, 0, 27, flat, unbanked }, {  -6, -12, 0, 27, flat, unbanked }, {  -7, -13, 0, 27, flat, unbanked }, {  -8, -14, 0, 27, flat, unbanked }, {  -8, -15, 0, 27, flat, unbanked },
        {  -9, -16, 0, 27, flat, unbanked }, { -10, -17, 0, 28, flat, unbanked }, { -11, -18, 0, 28, flat, unbanked }, { -12, -19, 0, 28, flat, unbanked }, { -12, -20, 0, 28, flat, unbanked },
    })

    static constexpr const VehicleInfoList * TrackVehicleInfoListGoKartsMovingToLeftLane[] = {
        &Flat_0,                                      &Flat_1,                                      &Flat_2,                                      &Flat_3,
        &Flat_0,                                      &Flat_1,                                      &Flat_2,                                      &Flat_3, // EndStation
        &Flat_0,                                      &Flat_1,                                      &Flat_2,                                      &Flat_3, // BeginStation
        &Flat_0,                                      &Flat_1,                                      &Flat_2,                                      &Flat_3, // MiddleStation
        &Up25_0,                                      &Up25_1,                                      &Up25_2,                                      &Up25_3,
        &Default::Up60_0,                             &Default::Up60_1,                             &Default::Up60_2,                             &Default::Up60_3,
        &FlatToUp25_0,                                &FlatToUp25_1,                                &FlatToUp25_2,                                &FlatToUp25_3,
        &Default::Up25ToUp60_0,                       &Default::Up25ToUp60_1,                       &Default::Up25ToUp60_2,                       &Default::Up25ToUp60_3,
        &Default::Up60ToUp25_0,                       &Default::Up60ToUp25_1,                       &Default::Up60ToUp25_2,                       &Default::Up60ToUp25_3,
        &Up25ToFlat_0,                                &Up25ToFlat_1,                                &Up25ToFlat_2,                                &Up25ToFlat_3,
        &Down25_0,                                    &Down25_1,                                    &Down25_2,                                    &Down25_3,
        &Default::Down60_0,                           &Default::Down60_1,                           &Default::Down60_2,                           &Default::Down60_3,
        &FlatToDown25_0,                              &FlatToDown25_1,                              &FlatToDown25_2,                              &FlatToDown25_3,
        &Default::Down25ToDown60_0,                   &Default::Down25ToDown60_1,                   &Default::Down25ToDown60_2,                   &Default::Down25ToDown60_3,
        &Default::Down60ToDown25_0,                   &Default::Down60ToDown25_1,                   &Default::Down60ToDown25_2,                   &Default::Down60ToDown25_3,
        &Down25ToFlat_0,                              &Down25ToFlat_1,                              &Down25ToFlat_2,                              &Down25ToFlat_3,
        &LeftQuarterTurn5Tiles_0,                     &LeftQuarterTurn5Tiles_1,                     &LeftQuarterTurn5Tiles_2,                     &LeftQuarterTurn5Tiles_3,
        &RightQuarterTurn5Tiles_0,                    &RightQuarterTurn5Tiles_1,                    &RightQuarterTurn5Tiles_2,                    &RightQuarterTurn5Tiles_3,
        &Default::FlatToLeftBank_0,                   &Default::FlatToLeftBank_1,                   &Default::FlatToLeftBank_2,                   &Default::FlatToLeftBank_3,
        &Default::FlatToRightBank_0,                  &Default::FlatToRightBank_1,                  &Default::FlatToRightBank_2,                  &Default::FlatToRightBank_3,
        &Default::LeftBankToFlat_0,                   &Default::LeftBankToFlat_1,                   &Default::LeftBankToFlat_2,                   &Default::LeftBankToFlat_3,
        &Default::RightBankToFlat_0,                  &Default::RightBankToFlat_1,                  &Default::RightBankToFlat_2,                  &Default::RightBankToFlat_3,
        &Default::BankedLeftQuarterTurn5Tiles_0,      &Default::BankedLeftQuarterTurn5Tiles_1,      &Default::BankedLeftQuarterTurn5Tiles_2,      &Default::BankedLeftQuarterTurn5Tiles_3,
        &Default::BankedRightQuarterTurn5Tiles_0,     &Default::BankedRightQuarterTurn5Tiles_1,     &Default::BankedRightQuarterTurn5Tiles_2,     &Default::BankedRightQuarterTurn5Tiles_3,
        &Default::LeftBankToUp25_0,                   &Default::LeftBankToUp25_1,                   &Default::LeftBankToUp25_2,                   &Default::LeftBankToUp25_3,
        &Default::RightBankToUp25_0,                  &Default::RightBankToUp25_1,                  &Default::RightBankToUp25_2,                  &Default::RightBankToUp25_3,
        &Default::Up25ToLeftBank_0,                   &Default::Up25ToLeftBank_1,                   &Default::Up25ToLeftBank_2,                   &Default::Up25ToLeftBank_3,
        &Default::Up25ToRightBank_0,                  &Default::Up25ToRightBank_1,                  &Default::Up25ToRightBank_2,                  &Default::Up25ToRightBank_3,
        &Default::LeftBankToDown25_0,                 &Default::LeftBankToDown25_1,                 &Default::LeftBankToDown25_2,                 &Default::LeftBankToDown25_3,
        &Default::RightBankToDown25_0,                &Default::RightBankToDown25_1,                &Default::RightBankToDown25_2,                &Default::RightBankToDown25_3,
        &Default::Down25ToLeftBank_0,                 &Default::Down25ToLeftBank_1,                 &Default::Down25ToLeftBank_2,                 &Default::Down25ToLeftBank_3,
        &Default::Down25ToRightBank_0,                &Default::Down25ToRightBank_1,                &Default::Down25ToRightBank_2,                &Default::Down25ToRightBank_3,
        &Default::LeftBank_0,                         &Default::LeftBank_1,                         &Default::LeftBank_2,                         &Default::LeftBank_3,
        &Default::RightBank_0,                        &Default::RightBank_1,                        &Default::RightBank_2,                        &Default::RightBank_3,
        &Default::LeftQuarterTurn5TilesUp25_0,        &Default::LeftQuarterTurn5TilesUp25_1,        &Default::LeftQuarterTurn5TilesUp25_2,        &Default::LeftQuarterTurn5TilesUp25_3,
        &Default::RightQuarterTurn5TilesUp25_0,       &Default::RightQuarterTurn5TilesUp25_1,       &Default::RightQuarterTurn5TilesUp25_2,       &Default::RightQuarterTurn5TilesUp25_3,
        &Default::LeftQuarterTurn5TilesDown25_0,      &Default::LeftQuarterTurn5TilesDown25_1,      &Default::LeftQuarterTurn5TilesDown25_2,      &Default::LeftQuarterTurn5TilesDown25_3,
        &Default::RightQuarterTurn5TilesDown25_0,     &Default::RightQuarterTurn5TilesDown25_1,     &Default::RightQuarterTurn5TilesDown25_2,     &Default::RightQuarterTurn5TilesDown25_3,
        &SBendLeft_0,                                 &SBendLeft_1,                                 &SBendLeft_2,                                 &SBendLeft_3,
        &SBendRight_0,                                &SBendRight_1,                                &SBendRight_2,                                &SBendRight_3,
        &Default::LeftVerticalLoop_0,                 &Default::LeftVerticalLoop_1,                 &Default::LeftVerticalLoop_2,                 &Default::LeftVerticalLoop_3,
        &Default::RightVerticalLoop_0,                &Default::RightVerticalLoop_1,                &Default::RightVerticalLoop_2,                &Default::RightVerticalLoop_3,
        &LeftQuarterTurn3Tiles_0,                     &LeftQuarterTurn3Tiles_1,                     &LeftQuarterTurn3Tiles_2,                     &LeftQuarterTurn3Tiles_3,
        &RightQuarterTurn3Tiles_0,                    &RightQuarterTurn3Tiles_1,                    &RightQuarterTurn3Tiles_2,                    &RightQuarterTurn3Tiles_3,
        &Default::LeftBankedQuarterTurn3Tiles_0,      &Default::LeftBankedQuarterTurn3Tiles_1,      &Default::LeftBankedQuarterTurn3Tiles_2,      &Default::LeftBankedQuarterTurn3Tiles_3,
        &Default::RightBankedQuarterTurn3Tiles_0,     &Default::RightBankedQuarterTurn3Tiles_1,     &Default::RightBankedQuarterTurn3Tiles_2,     &Default::RightBankedQuarterTurn3Tiles_3,
        &Default::LeftQuarterTurn3TilesUp25_0,        &Default::LeftQuarterTurn3TilesUp25_1,        &Default::LeftQuarterTurn3TilesUp25_2,        &Default::LeftQuarterTurn3TilesUp25_3,
        &Default::RightQuarterTurn3TilesUp25_0,       &Default::RightQuarterTurn3TilesUp25_1,       &Default::RightQuarterTurn3TilesUp25_2,       &Default::RightQuarterTurn3TilesUp25_3,
        &Default::LeftQuarterTurn3TilesDown25_0,      &Default::LeftQuarterTurn3TilesDown25_1,      &Default::LeftQuarterTurn3TilesDown25_2,      &Default::LeftQuarterTurn3TilesDown25_3,
        &Default::RightQuarterTurn3TilesDown25_0,     &Default::RightQuarterTurn3TilesDown25_1,     &Default::RightQuarterTurn3TilesDown25_2,     &Default::RightQuarterTurn3TilesDown25_3,
        &LeftQuarterTurn1Tile_0,                      &LeftQuarterTurn1Tile_1,                      &LeftQuarterTurn1Tile_2,                      &LeftQuarterTurn1Tile_3,
        &RightQuarterTurn1Tile_0,                     &RightQuarterTurn1Tile_1,                     &RightQuarterTurn1Tile_2,                     &RightQuarterTurn1Tile_3,
        &Default::LeftTwistDownToUp_0,                &Default::LeftTwistDownToUp_1,                &Default::LeftTwistDownToUp_2,                &Default::LeftTwistDownToUp_3,
        &Default::RightTwistDownToUp_0,               &Default::RightTwistDownToUp_1,               &Default::RightTwistDownToUp_2,               &Default::RightTwistDownToUp_3,
        &Default::LeftTwistUpToDown_0,                &Default::LeftTwistUpToDown_1,                &Default::LeftTwistUpToDown_2,                &Default::LeftTwistUpToDown_3,
        &Default::RightTwistUpToDown_0,               &Default::RightTwistUpToDown_1,               &Default::RightTwistUpToDown_2,               &Default::RightTwistUpToDown_3,
        &Default::HalfLoopUp_0,                       &Default::HalfLoopUp_1,                       &Default::HalfLoopUp_2,                       &Default::HalfLoopUp_3,
        &Default::HalfLoopDown_0,                     &Default::HalfLoopDown_1,                     &Default::HalfLoopDown_2,                     &Default::HalfLoopDown_3,
        &Default::LeftCorkscrewUp_0,                  &Default::LeftCorkscrewUp_1,                  &Default::LeftCorkscrewUp_2,                  &Default::LeftCorkscrewUp_3,
        &Default::RightCorkscrewUp_0,                 &Default::RightCorkscrewUp_1,                 &Default::RightCorkscrewUp_2,                 &Default::RightCorkscrewUp_3,
        &Default::LeftCorkscrewDown_0,                &Default::LeftCorkscrewDown_1,                &Default::LeftCorkscrewDown_2,                &Default::LeftCorkscrewDown_3,
        &Default::RightCorkscrewDown_0,               &Default::RightCorkscrewDown_1,               &Default::RightCorkscrewDown_2,               &Default::RightCorkscrewDown_3,
        &Default::FlatToUp60_0,                       &Default::FlatToUp60_1,                       &Default::FlatToUp60_2,                       &Default::FlatToUp60_3,
        &Default::Up60ToFlat_0,                       &Default::Up60ToFlat_1,                       &Default::Up60ToFlat_2,                       &Default::Up60ToFlat_3,
        &Default::FlatToDown60_0,                     &Default::FlatToDown60_1,                     &Default::FlatToDown60_2,                     &Default::FlatToDown60_3,
        &Default::Down60ToFlat_0,                     &Default::Down60ToFlat_1,                     &Default::Down60ToFlat_2,                     &Default::Down60ToFlat_3,
        &Default::TowerBase_0,                        &Default::TowerBase_1,                        &Default::TowerBase_2,                        &Default::TowerBase_3,
        &Default::TowerSection_0,                     &Default::TowerSection_1,                     &Default::TowerSection_2,                     &Default::TowerSection_3,
        &Default::Flat_0,                             &Default::Flat_1,                             &Default::Flat_2,                             &Default::Flat_3, // FlatCovered
        &Default::Up25_0,                             &Default::Up25_1,                             &Default::Up25_2,                             &Default::Up25_3, // Up25Covered
        &Default::Up60_0,                             &Default::Up60_1,                             &Default::Up60_2,                             &Default::Up60_3, // Up60Covered
        &Default::FlatToUp25_0,                       &Default::FlatToUp25_1,                       &Default::FlatToUp25_2,                       &Default::FlatToUp25_3, // FlatToUp25Covered
        &Default::Up25ToUp60_0,                       &Default::Up25ToUp60_1,                       &Default::Up25ToUp60_2,                       &Default::Up25ToUp60_3, // Up25ToUp60Covered
        &Default::Up60ToUp25_0,                       &Default::Up60ToUp25_1,                       &Default::Up60ToUp25_2,                       &Default::Up60ToUp25_3, // Up60ToUp25Covered
        &Default::Up25ToFlat_0,                       &Default::Up25ToFlat_1,                       &Default::Up25ToFlat_2,                       &Default::Up25ToFlat_3, // Up25ToFlatCovered
        &Default::Down25_0,                           &Default::Down25_1,                           &Default::Down25_2,                           &Default::Down25_3, // Down25Covered
        &Default::Down60_0,                           &Default::Down60_1,                           &Default::Down60_2,                           &Default::Down60_3, // Down60Covered
        &Default::FlatToDown25_0,                     &Default::FlatToDown25_1,                     &Default::FlatToDown25_2,                     &Default::FlatToDown25_3, // FlatToDown25Covered
        &Default::Down25ToDown60_0,                   &Default::Down25ToDown60_1,                   &Default::Down25ToDown60_2,                   &Default::Down25ToDown60_3, // Down25ToDown60Covered
        &Default::Down60ToDown25_0,                   &Default::Down60ToDown25_1,                   &Default::Down60ToDown25_2,                   &Default::Down60ToDown25_3, // Down60ToDown25Covered
        &Default::Down25ToFlat_0,                     &Default::Down25ToFlat_1,                     &Default::Down25ToFlat_2,                     &Default::Down25ToFlat_3, // Down25ToFlatCovered
        &Default::LeftQuarterTurn5Tiles_0,            &Default::LeftQuarterTurn5Tiles_1,            &Default::LeftQuarterTurn5Tiles_2,            &Default::LeftQuarterTurn5Tiles_3, // LeftQuarterTurn5TilesCovered
        &Default::RightQuarterTurn5Tiles_0,           &Default::RightQuarterTurn5Tiles_1,           &Default::RightQuarterTurn5Tiles_2,           &Default::RightQuarterTurn5Tiles_3, // RightQuarterTurn5TilesCovered
        &Default::SBendLeft_0,                        &Default::SBendLeft_1,                        &Default::SBendLeft_2,                        &Default::SBendLeft_3, // SBendLeftCovered
        &Default::SBendRight_0,                       &Default::SBendRight_1,                       &Default::SBendRight_2,                       &Default::SBendRight_3, // SBendRightCovered
        &Default::LeftQuarterTurn3Tiles_0,            &Default::LeftQuarterTurn3Tiles_1,            &Default::LeftQuarterTurn3Tiles_2,            &Default::LeftQuarterTurn3Tiles_3, // LeftQuarterTurn3TilesCovered
        &Default::RightQuarterTurn3Tiles_0,           &Default::RightQuarterTurn3Tiles_1,           &Default::RightQuarterTurn3Tiles_2,           &Default::RightQuarterTurn3Tiles_3, // RightQuarterTurn3TilesCovered
        &Default::LeftHalfBankedHelixUpSmall_0,       &Default::LeftHalfBankedHelixUpSmall_1,       &Default::LeftHalfBankedHelixUpSmall_2,       &Default::LeftHalfBankedHelixUpSmall_3,
        &Default::RightHalfBankedHelixUpSmall_0,      &Default::RightHalfBankedHelixUpSmall_1,      &Default::RightHalfBankedHelixUpSmall_2,      &Default::RightHalfBankedHelixUpSmall_3,
        &Default::LeftHalfBankedHelixDownSmall_0,     &Default::LeftHalfBankedHelixDownSmall_1,     &Default::LeftHalfBankedHelixDownSmall_2,     &Default::LeftHalfBankedHelixDownSmall_3,
        &Default::RightHalfBankedHelixDownSmall_0,    &Default::RightHalfBankedHelixDownSmall_1,    &Default::RightHalfBankedHelixDownSmall_2,    &Default::RightHalfBankedHelixDownSmall_3,
        &Default::LeftHalfBankedHelixUpLarge_0,       &Default::LeftHalfBankedHelixUpLarge_1,       &Default::LeftHalfBankedHelixUpLarge_2,       &Default::LeftHalfBankedHelixUpLarge_3,
        &Default::RightHalfBankedHelixUpLarge_0,      &Default::RightHalfBankedHelixUpLarge_1,      &Default::RightHalfBankedHelixUpLarge_2,      &Default::RightHalfBankedHelixUpLarge_3,
        &Default::LeftHalfBankedHelixDownLarge_0,     &Default::LeftHalfBankedHelixDownLarge_1,     &Default::LeftHalfBankedHelixDownLarge_2,     &Default::LeftHalfBankedHelixDownLarge_3,
        &Default::RightHalfBankedHelixDownLarge_0,    &Default::RightHalfBankedHelixDownLarge_1,    &Default::RightHalfBankedHelixDownLarge_2,    &Default::RightHalfBankedHelixDownLarge_3,
        &Default::LeftQuarterTurn1TileUp60_0,         &Default::LeftQuarterTurn1TileUp60_1,         &Default::LeftQuarterTurn1TileUp60_2,         &Default::LeftQuarterTurn1TileUp60_3,
        &Default::RightQuarterTurn1TileUp60_0,        &Default::RightQuarterTurn1TileUp60_1,        &Default::RightQuarterTurn1TileUp60_2,        &Default::RightQuarterTurn1TileUp60_3,
        &Default::LeftQuarterTurn1TileDown60_0,       &Default::LeftQuarterTurn1TileDown60_1,       &Default::LeftQuarterTurn1TileDown60_2,       &Default::LeftQuarterTurn1TileDown60_3,
        &Default::RightQuarterTurn1TileDown60_0,      &Default::RightQuarterTurn1TileDown60_1,      &Default::RightQuarterTurn1TileDown60_2,      &Default::RightQuarterTurn1TileDown60_3,
        &Default::Flat_0,                             &Default::Flat_1,                             &Default::Flat_2,                             &Default::Flat_3, // Brakes
        &Default::Flat_0,                             &Default::Flat_1,                             &Default::Flat_2,                             &Default::Flat_3, // Booster
        &TrackVehicleInfo_000000,                     &TrackVehicleInfo_000000,                     &TrackVehicleInfo_000000,                     &TrackVehicleInfo_000000, // Maze
        &Default::LeftQuarterBankedHelixLargeUp_0,    &Default::LeftQuarterBankedHelixLargeUp_1,    &Default::LeftQuarterBankedHelixLargeUp_2,    &Default::LeftQuarterBankedHelixLargeUp_3,
        &Default::RightQuarterBankedHelixLargeUp_0,   &Default::RightQuarterBankedHelixLargeUp_1,   &Default::RightQuarterBankedHelixLargeUp_2,   &Default::RightQuarterBankedHelixLargeUp_3,
        &Default::LeftQuarterBankedHelixLargeDown_0,  &Default::LeftQuarterBankedHelixLargeDown_1,  &Default::LeftQuarterBankedHelixLargeDown_2,  &Default::LeftQuarterBankedHelixLargeDown_3,
        &Default::RightQuarterBankedHelixLargeDown_0, &Default::RightQuarterBankedHelixLargeDown_1, &Default::RightQuarterBankedHelixLargeDown_2, &Default::RightQuarterBankedHelixLargeDown_3,
        &Default::LeftQuarterHelixLargeUp_0,          &Default::LeftQuarterHelixLargeUp_1,          &Default::LeftQuarterHelixLargeUp_2,          &Default::LeftQuarterHelixLargeUp_3,
        &Default::RightQuarterHelixLargeUp_0,         &Default::RightQuarterHelixLargeUp_1,         &Default::RightQuarterHelixLargeUp_2,         &Default::RightQuarterHelixLargeUp_3,
        &Default::LeftQuarterHelixLargeDown_0,        &Default::LeftQuarterHelixLargeDown_1,        &Default::LeftQuarterHelixLargeDown_2,        &Default::LeftQuarterHelixLargeDown_3,
        &Default::RightQuarterHelixLargeDown_0,       &Default::RightQuarterHelixLargeDown_1,       &Default::RightQuarterHelixLargeDown_2,       &Default::RightQuarterHelixLargeDown_3,
        &Default::Up25LeftBanked_0,                   &Default::Up25LeftBanked_1,                   &Default::Up25LeftBanked_2,                   &Default::Up25LeftBanked_3,
        &Default::Up25RightBanked_0,                  &Default::Up25RightBanked_1,                  &Default::Up25RightBanked_2,                  &Default::Up25RightBanked_3,
        &Default::Flat_0,                             &Default::Flat_1,                             &Default::Flat_2,                             &Default::Flat_3, // Waterfall
        &Default::Rapids_0,                           &Default::Rapids_1,                           &Default::Rapids_2,                           &Default::Rapids_3,
        &Default::Flat_0,                             &Default::Flat_1,                             &Default::Flat_2,                             &Default::Flat_3, // OnRidePhoto
        &Default::Down25LeftBanked_0,                 &Default::Down25LeftBanked_1,                 &Default::Down25LeftBanked_2,                 &Default::Down25LeftBanked_3,
        &Default::Down25RightBanked_0,                &Default::Down25RightBanked_1,                &Default::Down25RightBanked_2,                &Default::Down25RightBanked_3,
        &Default::Watersplash_0,                      &Default::Watersplash_1,                      &Default::Watersplash_2,                      &Default::Watersplash_3,
        &Default::FlatToUp60LongBase_0,               &Default::FlatToUp60LongBase_1,               &Default::FlatToUp60LongBase_2,               &Default::FlatToUp60LongBase_3,
        &Default::Up60ToFlatLongBase_0,               &Default::Up60ToFlatLongBase_1,               &Default::Up60ToFlatLongBase_2,               &Default::Up60ToFlatLongBase_3,
        &Default::Flat_0,                             &Default::Flat_1,                             &Default::Flat_2,                             &Default::Flat_3, // Whirlpool
        &Default::Down60ToFlatLongBase_0,             &Default::Down60ToFlatLongBase_1,             &Default::Down60ToFlatLongBase_2,             &Default::Down60ToFlatLongBase_3,
        &Default::FlatToDown60LongBase_0,             &Default::FlatToDown60LongBase_1,             &Default::FlatToDown60LongBase_2,             &Default::FlatToDown60LongBase_3,
        &Default::CableLiftHill_0,                    &Default::CableLiftHill_1,                    &Default::CableLiftHill_2,                    &Default::CableLiftHill_3,
        &Default::ReverseFreefallSlope_0,             &Default::ReverseFreefallSlope_1,             &Default::ReverseFreefallSlope_2,             &Default::ReverseFreefallSlope_3,
        &Default::ReverseFreefallVertical_0,          &Default::ReverseFreefallVertical_1,          &Default::ReverseFreefallVertical_2,          &Default::ReverseFreefallVertical_3,
        &Default::Up90_0,                             &Default::Up90_1,                             &Default::Up90_2,                             &Default::Up90_3,
        &Default::Down90_0,                           &Default::Down90_1,                           &Default::Down90_2,                           &Default::Down90_3,
        &Default::Up60ToUp90_0,                       &Default::Up60ToUp90_1,                       &Default::Up60ToUp90_2,                       &Default::Up60ToUp90_3,
        &Default::Down90ToDown60_0,                   &Default::Down90ToDown60_1,                   &Default::Down90ToDown60_2,                   &Default::Down90ToDown60_3,
        &Default::Up90ToUp60_0,                       &Default::Up90ToUp60_1,                       &Default::Up90ToUp60_2,                       &Default::Up90ToUp60_3,
        &Default::Down60ToDown90_0,                   &Default::Down60ToDown90_1,                   &Default::Down60ToDown90_2,                   &Default::Down60ToDown90_3,
        &Default::FlatToDown60_0,                     &Default::FlatToDown60_1,                     &Default::FlatToDown60_2,                     &Default::FlatToDown60_3, // BrakeForDrop
        &LeftEighthToDiag_0,                          &LeftEighthToDiag_1,                          &LeftEighthToDiag_2,                          &LeftEighthToDiag_3,
        &RightEighthToDiag_0,                         &RightEighthToDiag_1,                         &RightEighthToDiag_2,                         &RightEighthToDiag_3,
        &LeftEighthToOrthogonal_0,                    &LeftEighthToOrthogonal_1,                    &LeftEighthToOrthogonal_2,                    &LeftEighthToOrthogonal_3,
        &RightEighthToOrthogonal_0,                   &RightEighthToOrthogonal_1,                   &RightEighthToOrthogonal_2,                   &RightEighthToOrthogonal_3,
        &Default::LeftEighthBankToDiag_0,             &Default::LeftEighthBankToDiag_1,             &Default::LeftEighthBankToDiag_2,             &Default::LeftEighthBankToDiag_3,
        &Default::RightEighthBankToDiag_0,            &Default::RightEighthBankToDiag_1,            &Default::RightEighthBankToDiag_2,            &Default::RightEighthBankToDiag_3,
        &Default::LeftEighthBankToOrthogonal_0,       &Default::LeftEighthBankToOrthogonal_1,       &Default::LeftEighthBankToOrthogonal_2,       &Default::LeftEighthBankToOrthogonal_3,
        &Default::RightEighthBankToOrthogonal_0,      &Default::RightEighthBankToOrthogonal_1,      &Default::RightEighthBankToOrthogonal_2,      &Default::RightEighthBankToOrthogonal_3,
        &DiagFlat_0,                                  &DiagFlat_1,                                  &DiagFlat_2,                                  &DiagFlat_3,
    };
    // clang-format on
    static_assert(std::size(TrackVehicleInfoListGoKartsMovingToLeftLane) == kVehicleTrackSubpositionSizeGoKartsSwitchingLanes);
} // namespace OpenRCT2::Subposition::GoKartsMovingToLeftLane
