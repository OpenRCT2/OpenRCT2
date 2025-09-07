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

namespace OpenRCT2::Subposition::ChairliftBullwheels
{
    // clang-format off
    CREATE_VEHICLE_INFO(EndBullwheel_0, {
        { 31,  6, 0,  0, flat, unbanked }, { 30,  6, 0,  0, flat, unbanked }, { 29,  6, 0,  0, flat, unbanked }, { 28,  6, 0,  0, flat, unbanked }, { 27,  6, 0,  0, flat, unbanked },
        { 26,  6, 0,  0, flat, unbanked }, { 25,  6, 0,  0, flat, unbanked }, { 24,  6, 0,  0, flat, unbanked }, { 23,  6, 0,  0, flat, unbanked }, { 22,  6, 0,  0, flat, unbanked },
        { 21,  6, 0,  0, flat, unbanked }, { 20,  6, 0,  0, flat, unbanked }, { 19,  6, 0,  0, flat, unbanked }, { 18,  6, 0,  0, flat, unbanked }, { 17,  6, 0,  0, flat, unbanked },
        { 16,  6, 0,  0, flat, unbanked }, { 15,  6, 0,  0, flat, unbanked }, { 14,  6, 0,  1, flat, unbanked }, { 13,  6, 0,  1, flat, unbanked }, { 12,  7, 0,  2, flat, unbanked },
        { 11,  7, 0,  2, flat, unbanked }, { 10,  8, 0,  3, flat, unbanked }, {  9,  8, 0,  3, flat, unbanked }, {  8,  9, 0,  4, flat, unbanked }, {  7, 10, 0,  5, flat, unbanked },
        {  7, 11, 0,  5, flat, unbanked }, {  6, 12, 0,  6, flat, unbanked }, {  6, 13, 0,  6, flat, unbanked }, {  6, 14, 0,  7, flat, unbanked }, {  6, 15, 0,  7, flat, unbanked },
        {  6, 16, 0,  8, flat, unbanked }, {  6, 17, 0,  9, flat, unbanked }, {  6, 18, 0,  9, flat, unbanked }, {  6, 19, 0, 10, flat, unbanked }, {  6, 20, 0, 10, flat, unbanked },
        {  7, 21, 0, 11, flat, unbanked }, {  7, 22, 0, 11, flat, unbanked }, {  8, 23, 0, 12, flat, unbanked }, {  9, 24, 0, 13, flat, unbanked }, { 10, 24, 0, 13, flat, unbanked },
        { 11, 25, 0, 14, flat, unbanked }, { 12, 25, 0, 14, flat, unbanked }, { 13, 26, 0, 15, flat, unbanked }, { 14, 26, 0, 15, flat, unbanked }, { 15, 26, 0, 16, flat, unbanked },
        { 16, 26, 0, 16, flat, unbanked }, { 17, 26, 0, 16, flat, unbanked }, { 18, 26, 0, 16, flat, unbanked }, { 19, 26, 0, 16, flat, unbanked }, { 20, 26, 0, 16, flat, unbanked },
        { 21, 26, 0, 16, flat, unbanked }, { 22, 26, 0, 16, flat, unbanked }, { 23, 26, 0, 16, flat, unbanked }, { 24, 26, 0, 16, flat, unbanked }, { 25, 26, 0, 16, flat, unbanked },
        { 26, 26, 0, 16, flat, unbanked }, { 27, 26, 0, 16, flat, unbanked }, { 28, 26, 0, 16, flat, unbanked }, { 29, 26, 0, 16, flat, unbanked }, { 30, 26, 0, 16, flat, unbanked },
        { 31, 26, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(EndBullwheel_1, {
        {  6,  0, 0,  8, flat, unbanked }, {  6,  1, 0,  8, flat, unbanked }, {  6,  2, 0,  8, flat, unbanked }, {  6,  3, 0,  8, flat, unbanked }, {  6,  4, 0,  8, flat, unbanked },
        {  6,  5, 0,  8, flat, unbanked }, {  6,  6, 0,  8, flat, unbanked }, {  6,  7, 0,  8, flat, unbanked }, {  6,  8, 0,  8, flat, unbanked }, {  6,  9, 0,  8, flat, unbanked },
        {  6, 10, 0,  8, flat, unbanked }, {  6, 11, 0,  8, flat, unbanked }, {  6, 12, 0,  8, flat, unbanked }, {  6, 13, 0,  8, flat, unbanked }, {  6, 14, 0,  8, flat, unbanked },
        {  6, 15, 0,  8, flat, unbanked }, {  6, 16, 0,  8, flat, unbanked }, {  6, 17, 0,  9, flat, unbanked }, {  6, 18, 0,  9, flat, unbanked }, {  7, 19, 0, 10, flat, unbanked },
        {  7, 20, 0, 10, flat, unbanked }, {  8, 21, 0, 11, flat, unbanked }, {  8, 22, 0, 11, flat, unbanked }, {  9, 23, 0, 12, flat, unbanked }, { 10, 24, 0, 13, flat, unbanked },
        { 11, 24, 0, 13, flat, unbanked }, { 12, 25, 0, 14, flat, unbanked }, { 13, 25, 0, 14, flat, unbanked }, { 14, 25, 0, 15, flat, unbanked }, { 15, 25, 0, 15, flat, unbanked },
        { 16, 25, 0, 16, flat, unbanked }, { 17, 25, 0, 17, flat, unbanked }, { 18, 25, 0, 17, flat, unbanked }, { 19, 25, 0, 18, flat, unbanked }, { 20, 25, 0, 18, flat, unbanked },
        { 21, 24, 0, 19, flat, unbanked }, { 22, 24, 0, 19, flat, unbanked }, { 23, 23, 0, 20, flat, unbanked }, { 24, 22, 0, 21, flat, unbanked }, { 24, 21, 0, 21, flat, unbanked },
        { 25, 20, 0, 22, flat, unbanked }, { 25, 19, 0, 22, flat, unbanked }, { 26, 18, 0, 23, flat, unbanked }, { 26, 17, 0, 23, flat, unbanked }, { 26, 16, 0, 24, flat, unbanked },
        { 26, 15, 0, 24, flat, unbanked }, { 26, 14, 0, 24, flat, unbanked }, { 26, 13, 0, 24, flat, unbanked }, { 26, 12, 0, 24, flat, unbanked }, { 26, 11, 0, 24, flat, unbanked },
        { 26, 10, 0, 24, flat, unbanked }, { 26,  9, 0, 24, flat, unbanked }, { 26,  8, 0, 24, flat, unbanked }, { 26,  7, 0, 24, flat, unbanked }, { 26,  6, 0, 24, flat, unbanked },
        { 26,  5, 0, 24, flat, unbanked }, { 26,  4, 0, 24, flat, unbanked }, { 26,  3, 0, 24, flat, unbanked }, { 26,  2, 0, 24, flat, unbanked }, { 26,  1, 0, 24, flat, unbanked },
        { 26,  0, 0, 24, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(EndBullwheel_2, {
        {  0, 26, 0, 16, flat, unbanked }, {  1, 26, 0, 16, flat, unbanked }, {  2, 26, 0, 16, flat, unbanked }, {  3, 26, 0, 16, flat, unbanked }, {  4, 26, 0, 16, flat, unbanked },
        {  5, 26, 0, 16, flat, unbanked }, {  6, 26, 0, 16, flat, unbanked }, {  7, 26, 0, 16, flat, unbanked }, {  8, 26, 0, 16, flat, unbanked }, {  9, 26, 0, 16, flat, unbanked },
        { 10, 26, 0, 16, flat, unbanked }, { 11, 26, 0, 16, flat, unbanked }, { 12, 26, 0, 16, flat, unbanked }, { 13, 26, 0, 16, flat, unbanked }, { 14, 26, 0, 16, flat, unbanked },
        { 15, 26, 0, 16, flat, unbanked }, { 16, 26, 0, 16, flat, unbanked }, { 17, 26, 0, 17, flat, unbanked }, { 18, 26, 0, 17, flat, unbanked }, { 19, 25, 0, 18, flat, unbanked },
        { 20, 25, 0, 18, flat, unbanked }, { 21, 24, 0, 19, flat, unbanked }, { 22, 24, 0, 19, flat, unbanked }, { 23, 23, 0, 20, flat, unbanked }, { 24, 22, 0, 21, flat, unbanked },
        { 24, 21, 0, 21, flat, unbanked }, { 25, 20, 0, 22, flat, unbanked }, { 25, 19, 0, 22, flat, unbanked }, { 25, 18, 0, 23, flat, unbanked }, { 25, 17, 0, 23, flat, unbanked },
        { 25, 16, 0, 24, flat, unbanked }, { 25, 15, 0, 25, flat, unbanked }, { 25, 14, 0, 25, flat, unbanked }, { 25, 13, 0, 26, flat, unbanked }, { 25, 12, 0, 26, flat, unbanked },
        { 24, 11, 0, 27, flat, unbanked }, { 24, 10, 0, 27, flat, unbanked }, { 23,  9, 0, 28, flat, unbanked }, { 22,  8, 0, 29, flat, unbanked }, { 21,  8, 0, 29, flat, unbanked },
        { 20,  7, 0, 30, flat, unbanked }, { 19,  7, 0, 30, flat, unbanked }, { 18,  6, 0, 31, flat, unbanked }, { 17,  6, 0, 31, flat, unbanked }, { 16,  6, 0,  0, flat, unbanked },
        { 15,  6, 0,  0, flat, unbanked }, { 14,  6, 0,  0, flat, unbanked }, { 13,  6, 0,  0, flat, unbanked }, { 12,  6, 0,  0, flat, unbanked }, { 11,  6, 0,  0, flat, unbanked },
        { 10,  6, 0,  0, flat, unbanked }, {  9,  6, 0,  0, flat, unbanked }, {  8,  6, 0,  0, flat, unbanked }, {  7,  6, 0,  0, flat, unbanked }, {  6,  6, 0,  0, flat, unbanked },
        {  5,  6, 0,  0, flat, unbanked }, {  4,  6, 0,  0, flat, unbanked }, {  3,  6, 0,  0, flat, unbanked }, {  2,  6, 0,  0, flat, unbanked }, {  1,  6, 0,  0, flat, unbanked },
        {  0,  6, 0,  0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(EndBullwheel_3, {
        { 26, 31, 0, 24, flat, unbanked }, { 26, 30, 0, 24, flat, unbanked }, { 26, 29, 0, 24, flat, unbanked }, { 26, 28, 0, 24, flat, unbanked }, { 26, 27, 0, 24, flat, unbanked },
        { 26, 26, 0, 24, flat, unbanked }, { 26, 25, 0, 24, flat, unbanked }, { 26, 24, 0, 24, flat, unbanked }, { 26, 23, 0, 24, flat, unbanked }, { 26, 22, 0, 24, flat, unbanked },
        { 26, 21, 0, 24, flat, unbanked }, { 26, 20, 0, 24, flat, unbanked }, { 26, 19, 0, 24, flat, unbanked }, { 26, 18, 0, 24, flat, unbanked }, { 26, 17, 0, 24, flat, unbanked },
        { 26, 16, 0, 24, flat, unbanked }, { 26, 15, 0, 24, flat, unbanked }, { 26, 14, 0, 25, flat, unbanked }, { 26, 13, 0, 25, flat, unbanked }, { 25, 12, 0, 26, flat, unbanked },
        { 25, 11, 0, 26, flat, unbanked }, { 24, 10, 0, 27, flat, unbanked }, { 24,  9, 0, 27, flat, unbanked }, { 23,  8, 0, 28, flat, unbanked }, { 22,  7, 0, 29, flat, unbanked },
        { 21,  7, 0, 29, flat, unbanked }, { 20,  6, 0, 30, flat, unbanked }, { 19,  6, 0, 30, flat, unbanked }, { 18,  6, 0, 31, flat, unbanked }, { 17,  6, 0, 31, flat, unbanked },
        { 16,  6, 0,  0, flat, unbanked }, { 15,  6, 0,  1, flat, unbanked }, { 14,  6, 0,  1, flat, unbanked }, { 13,  6, 0,  2, flat, unbanked }, { 12,  6, 0,  2, flat, unbanked },
        { 11,  7, 0,  3, flat, unbanked }, { 10,  7, 0,  3, flat, unbanked }, {  9,  8, 0,  4, flat, unbanked }, {  8,  9, 0,  5, flat, unbanked }, {  8, 10, 0,  5, flat, unbanked },
        {  7, 11, 0,  6, flat, unbanked }, {  7, 12, 0,  6, flat, unbanked }, {  6, 13, 0,  7, flat, unbanked }, {  6, 14, 0,  7, flat, unbanked }, {  6, 15, 0,  8, flat, unbanked },
        {  6, 16, 0,  8, flat, unbanked }, {  6, 17, 0,  8, flat, unbanked }, {  6, 18, 0,  8, flat, unbanked }, {  6, 19, 0,  8, flat, unbanked }, {  6, 20, 0,  8, flat, unbanked },
        {  6, 21, 0,  8, flat, unbanked }, {  6, 22, 0,  8, flat, unbanked }, {  6, 23, 0,  8, flat, unbanked }, {  6, 24, 0,  8, flat, unbanked }, {  6, 25, 0,  8, flat, unbanked },
        {  6, 26, 0,  8, flat, unbanked }, {  6, 27, 0,  8, flat, unbanked }, {  6, 28, 0,  8, flat, unbanked }, {  6, 29, 0,  8, flat, unbanked }, {  6, 30, 0,  8, flat, unbanked },
        {  6, 31, 0,  8, flat, unbanked },
    })
        CREATE_VEHICLE_INFO(StartBullwheel_0, {
        {  0, 26, 0, 16, flat, unbanked }, {  1, 26, 0, 16, flat, unbanked }, {  2, 26, 0, 16, flat, unbanked }, {  3, 26, 0, 16, flat, unbanked }, {  4, 26, 0, 16, flat, unbanked },
        {  5, 26, 0, 16, flat, unbanked }, {  6, 26, 0, 16, flat, unbanked }, {  7, 26, 0, 16, flat, unbanked }, {  8, 26, 0, 16, flat, unbanked }, {  9, 26, 0, 16, flat, unbanked },
        { 10, 26, 0, 16, flat, unbanked }, { 11, 26, 0, 16, flat, unbanked }, { 12, 26, 0, 16, flat, unbanked }, { 13, 26, 0, 16, flat, unbanked }, { 14, 26, 0, 16, flat, unbanked },
        { 15, 26, 0, 16, flat, unbanked }, { 16, 26, 0, 16, flat, unbanked }, { 17, 26, 0, 17, flat, unbanked }, { 18, 26, 0, 17, flat, unbanked }, { 19, 25, 0, 18, flat, unbanked },
        { 20, 25, 0, 18, flat, unbanked }, { 21, 24, 0, 19, flat, unbanked }, { 22, 24, 0, 19, flat, unbanked }, { 23, 23, 0, 20, flat, unbanked }, { 24, 22, 0, 21, flat, unbanked },
        { 24, 21, 0, 21, flat, unbanked }, { 25, 20, 0, 22, flat, unbanked }, { 25, 19, 0, 22, flat, unbanked }, { 25, 18, 0, 23, flat, unbanked }, { 25, 17, 0, 23, flat, unbanked },
        { 25, 16, 0, 24, flat, unbanked }, { 25, 15, 0, 25, flat, unbanked }, { 25, 14, 0, 25, flat, unbanked }, { 25, 13, 0, 26, flat, unbanked }, { 25, 12, 0, 26, flat, unbanked },
        { 24, 11, 0, 27, flat, unbanked }, { 24, 10, 0, 27, flat, unbanked }, { 23,  9, 0, 28, flat, unbanked }, { 22,  8, 0, 29, flat, unbanked }, { 21,  8, 0, 29, flat, unbanked },
        { 20,  7, 0, 30, flat, unbanked }, { 19,  7, 0, 30, flat, unbanked }, { 18,  6, 0, 31, flat, unbanked }, { 17,  6, 0, 31, flat, unbanked }, { 16,  6, 0,  0, flat, unbanked },
        { 15,  6, 0,  0, flat, unbanked }, { 14,  6, 0,  0, flat, unbanked }, { 13,  6, 0,  0, flat, unbanked }, { 12,  6, 0,  0, flat, unbanked }, { 11,  6, 0,  0, flat, unbanked },
        { 10,  6, 0,  0, flat, unbanked }, {  9,  6, 0,  0, flat, unbanked }, {  8,  6, 0,  0, flat, unbanked }, {  7,  6, 0,  0, flat, unbanked }, {  6,  6, 0,  0, flat, unbanked },
        {  5,  6, 0,  0, flat, unbanked }, {  4,  6, 0,  0, flat, unbanked }, {  3,  6, 0,  0, flat, unbanked }, {  2,  6, 0,  0, flat, unbanked }, {  1,  6, 0,  0, flat, unbanked },
        {  0,  6, 0,  0, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(StartBullwheel_1, {
        { 26, 31, 0, 24, flat, unbanked }, { 26, 30, 0, 24, flat, unbanked }, { 26, 29, 0, 24, flat, unbanked }, { 26, 28, 0, 24, flat, unbanked }, { 26, 27, 0, 24, flat, unbanked },
        { 26, 26, 0, 24, flat, unbanked }, { 26, 25, 0, 24, flat, unbanked }, { 26, 24, 0, 24, flat, unbanked }, { 26, 23, 0, 24, flat, unbanked }, { 26, 22, 0, 24, flat, unbanked },
        { 26, 21, 0, 24, flat, unbanked }, { 26, 20, 0, 24, flat, unbanked }, { 26, 19, 0, 24, flat, unbanked }, { 26, 18, 0, 24, flat, unbanked }, { 26, 17, 0, 24, flat, unbanked },
        { 26, 16, 0, 24, flat, unbanked }, { 26, 15, 0, 24, flat, unbanked }, { 26, 14, 0, 25, flat, unbanked }, { 26, 13, 0, 25, flat, unbanked }, { 25, 12, 0, 26, flat, unbanked },
        { 25, 11, 0, 26, flat, unbanked }, { 24, 10, 0, 27, flat, unbanked }, { 24,  9, 0, 27, flat, unbanked }, { 23,  8, 0, 28, flat, unbanked }, { 22,  7, 0, 29, flat, unbanked },
        { 21,  7, 0, 29, flat, unbanked }, { 20,  6, 0, 30, flat, unbanked }, { 19,  6, 0, 30, flat, unbanked }, { 18,  6, 0, 31, flat, unbanked }, { 17,  6, 0, 31, flat, unbanked },
        { 16,  6, 0,  0, flat, unbanked }, { 15,  6, 0,  1, flat, unbanked }, { 14,  6, 0,  1, flat, unbanked }, { 13,  6, 0,  2, flat, unbanked }, { 12,  6, 0,  2, flat, unbanked },
        { 11,  7, 0,  3, flat, unbanked }, { 10,  7, 0,  3, flat, unbanked }, {  9,  8, 0,  4, flat, unbanked }, {  8,  9, 0,  5, flat, unbanked }, {  8, 10, 0,  5, flat, unbanked },
        {  7, 11, 0,  6, flat, unbanked }, {  7, 12, 0,  6, flat, unbanked }, {  6, 13, 0,  7, flat, unbanked }, {  6, 14, 0,  7, flat, unbanked }, {  6, 15, 0,  8, flat, unbanked },
        {  6, 16, 0,  8, flat, unbanked }, {  6, 17, 0,  8, flat, unbanked }, {  6, 18, 0,  8, flat, unbanked }, {  6, 19, 0,  8, flat, unbanked }, {  6, 20, 0,  8, flat, unbanked },
        {  6, 21, 0,  8, flat, unbanked }, {  6, 22, 0,  8, flat, unbanked }, {  6, 23, 0,  8, flat, unbanked }, {  6, 24, 0,  8, flat, unbanked }, {  6, 25, 0,  8, flat, unbanked },
        {  6, 26, 0,  8, flat, unbanked }, {  6, 27, 0,  8, flat, unbanked }, {  6, 28, 0,  8, flat, unbanked }, {  6, 29, 0,  8, flat, unbanked }, {  6, 30, 0,  8, flat, unbanked },
        {  6, 31, 0,  8, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(StartBullwheel_2, {
        { 31,  6, 0,  0, flat, unbanked }, { 30,  6, 0,  0, flat, unbanked }, { 29,  6, 0,  0, flat, unbanked }, { 28,  6, 0,  0, flat, unbanked }, { 27,  6, 0,  0, flat, unbanked },
        { 26,  6, 0,  0, flat, unbanked }, { 25,  6, 0,  0, flat, unbanked }, { 24,  6, 0,  0, flat, unbanked }, { 23,  6, 0,  0, flat, unbanked }, { 22,  6, 0,  0, flat, unbanked },
        { 21,  6, 0,  0, flat, unbanked }, { 20,  6, 0,  0, flat, unbanked }, { 19,  6, 0,  0, flat, unbanked }, { 18,  6, 0,  0, flat, unbanked }, { 17,  6, 0,  0, flat, unbanked },
        { 16,  6, 0,  0, flat, unbanked }, { 15,  6, 0,  0, flat, unbanked }, { 14,  6, 0,  1, flat, unbanked }, { 13,  6, 0,  1, flat, unbanked }, { 12,  7, 0,  2, flat, unbanked },
        { 11,  7, 0,  2, flat, unbanked }, { 10,  8, 0,  3, flat, unbanked }, {  9,  8, 0,  3, flat, unbanked }, {  8,  9, 0,  4, flat, unbanked }, {  7, 10, 0,  5, flat, unbanked },
        {  7, 11, 0,  5, flat, unbanked }, {  6, 12, 0,  6, flat, unbanked }, {  6, 13, 0,  6, flat, unbanked }, {  6, 14, 0,  7, flat, unbanked }, {  6, 15, 0,  7, flat, unbanked },
        {  6, 16, 0,  8, flat, unbanked }, {  6, 17, 0,  9, flat, unbanked }, {  6, 18, 0,  9, flat, unbanked }, {  6, 19, 0, 10, flat, unbanked }, {  6, 20, 0, 10, flat, unbanked },
        {  7, 21, 0, 11, flat, unbanked }, {  7, 22, 0, 11, flat, unbanked }, {  8, 23, 0, 12, flat, unbanked }, {  9, 24, 0, 13, flat, unbanked }, { 10, 24, 0, 13, flat, unbanked },
        { 11, 25, 0, 14, flat, unbanked }, { 12, 25, 0, 14, flat, unbanked }, { 13, 26, 0, 15, flat, unbanked }, { 14, 26, 0, 15, flat, unbanked }, { 15, 26, 0, 16, flat, unbanked },
        { 16, 26, 0, 16, flat, unbanked }, { 17, 26, 0, 16, flat, unbanked }, { 18, 26, 0, 16, flat, unbanked }, { 19, 26, 0, 16, flat, unbanked }, { 20, 26, 0, 16, flat, unbanked },
        { 21, 26, 0, 16, flat, unbanked }, { 22, 26, 0, 16, flat, unbanked }, { 23, 26, 0, 16, flat, unbanked }, { 24, 26, 0, 16, flat, unbanked }, { 25, 26, 0, 16, flat, unbanked },
        { 26, 26, 0, 16, flat, unbanked }, { 27, 26, 0, 16, flat, unbanked }, { 28, 26, 0, 16, flat, unbanked }, { 29, 26, 0, 16, flat, unbanked }, { 30, 26, 0, 16, flat, unbanked },
        { 31, 26, 0, 16, flat, unbanked },
    })

    CREATE_VEHICLE_INFO(StartBullwheel_3, {
        {  6,  0, 0,  8, flat, unbanked }, {  6,  1, 0,  8, flat, unbanked }, {  6,  2, 0,  8, flat, unbanked }, {  6,  3, 0,  8, flat, unbanked }, {  6,  4, 0,  8, flat, unbanked },
        {  6,  5, 0,  8, flat, unbanked }, {  6,  6, 0,  8, flat, unbanked }, {  6,  7, 0,  8, flat, unbanked }, {  6,  8, 0,  8, flat, unbanked }, {  6,  9, 0,  8, flat, unbanked },
        {  6, 10, 0,  8, flat, unbanked }, {  6, 11, 0,  8, flat, unbanked }, {  6, 12, 0,  8, flat, unbanked }, {  6, 13, 0,  8, flat, unbanked }, {  6, 14, 0,  8, flat, unbanked },
        {  6, 15, 0,  8, flat, unbanked }, {  6, 16, 0,  8, flat, unbanked }, {  6, 17, 0,  9, flat, unbanked }, {  6, 18, 0,  9, flat, unbanked }, {  7, 19, 0, 10, flat, unbanked },
        {  7, 20, 0, 10, flat, unbanked }, {  8, 21, 0, 11, flat, unbanked }, {  8, 22, 0, 11, flat, unbanked }, {  9, 23, 0, 12, flat, unbanked }, { 10, 24, 0, 13, flat, unbanked },
        { 11, 24, 0, 13, flat, unbanked }, { 12, 25, 0, 14, flat, unbanked }, { 13, 25, 0, 14, flat, unbanked }, { 14, 25, 0, 15, flat, unbanked }, { 15, 25, 0, 15, flat, unbanked },
        { 16, 25, 0, 16, flat, unbanked }, { 17, 25, 0, 17, flat, unbanked }, { 18, 25, 0, 17, flat, unbanked }, { 19, 25, 0, 18, flat, unbanked }, { 20, 25, 0, 18, flat, unbanked },
        { 21, 24, 0, 19, flat, unbanked }, { 22, 24, 0, 19, flat, unbanked }, { 23, 23, 0, 20, flat, unbanked }, { 24, 22, 0, 21, flat, unbanked }, { 24, 21, 0, 21, flat, unbanked },
        { 25, 20, 0, 22, flat, unbanked }, { 25, 19, 0, 22, flat, unbanked }, { 26, 18, 0, 23, flat, unbanked }, { 26, 17, 0, 23, flat, unbanked }, { 26, 16, 0, 24, flat, unbanked },
        { 26, 15, 0, 24, flat, unbanked }, { 26, 14, 0, 24, flat, unbanked }, { 26, 13, 0, 24, flat, unbanked }, { 26, 12, 0, 24, flat, unbanked }, { 26, 11, 0, 24, flat, unbanked },
        { 26, 10, 0, 24, flat, unbanked }, { 26,  9, 0, 24, flat, unbanked }, { 26,  8, 0, 24, flat, unbanked }, { 26,  7, 0, 24, flat, unbanked }, { 26,  6, 0, 24, flat, unbanked },
        { 26,  5, 0, 24, flat, unbanked }, { 26,  4, 0, 24, flat, unbanked }, { 26,  3, 0, 24, flat, unbanked }, { 26,  2, 0, 24, flat, unbanked }, { 26,  1, 0, 24, flat, unbanked },
        { 26,  0, 0, 24, flat, unbanked },
    })

    static constexpr const VehicleInfoList * TrackVehicleInfoListChairliftEndBullwheel[] = {
        &EndBullwheel_0,                           &EndBullwheel_1,                           &EndBullwheel_2,                           &EndBullwheel_3,                           // Flat
        &EndBullwheel_0,                           &EndBullwheel_1,                           &EndBullwheel_2,                           &EndBullwheel_3,                           // EndStation
        &EndBullwheel_0,                           &EndBullwheel_1,                           &EndBullwheel_2,                           &EndBullwheel_3,                           // BeginStation
        &EndBullwheel_0,                           &EndBullwheel_1,                           &EndBullwheel_2,                           &EndBullwheel_3,                           // MiddleStation
        &Default::Up25_0,                          &Default::Up25_1,                          &Default::Up25_2,                          &Default::Up25_3,                          // Up25
        &Default::Up60_0,                          &Default::Up60_1,                          &Default::Up60_2,                          &Default::Up60_3,                          // Up60
        &Default::FlatToUp25_0,                    &Default::FlatToUp25_1,                    &Default::FlatToUp25_2,                    &Default::FlatToUp25_3,                    // FlatToUp25
        &Default::Up25ToUp60_0,                    &Default::Up25ToUp60_1,                    &Default::Up25ToUp60_2,                    &Default::Up25ToUp60_3,                    // Up25ToUp60
        &Default::Up60ToUp25_0,                    &Default::Up60ToUp25_1,                    &Default::Up60ToUp25_2,                    &Default::Up60ToUp25_3,                    // Up60ToUp25
        &Default::Up25ToFlat_0,                    &Default::Up25ToFlat_1,                    &Default::Up25ToFlat_2,                    &Default::Up25ToFlat_3,                    // Up25ToFlat
        &Default::Down25_0,                        &Default::Down25_1,                        &Default::Down25_2,                        &Default::Down25_3,                        // Down25
        &Default::Down60_0,                        &Default::Down60_1,                        &Default::Down60_2,                        &Default::Down60_3,                        // Down60
        &Default::FlatToDown25_0,                  &Default::FlatToDown25_1,                  &Default::FlatToDown25_2,                  &Default::FlatToDown25_3,                  // FlatToDown25
        &Default::Down25ToDown60_0,                &Default::Down25ToDown60_1,                &Default::Down25ToDown60_2,                &Default::Down25ToDown60_3,                // Down25ToDown60
        &Default::Down60ToDown25_0,                &Default::Down60ToDown25_1,                &Default::Down60ToDown25_2,                &Default::Down60ToDown25_3,                // Down60ToDown25
        &Default::Down25ToFlat_0,                  &Default::Down25ToFlat_1,                  &Default::Down25ToFlat_2,                  &Default::Down25ToFlat_3,                  // Down25ToFlat
        &Default::LeftQuarterTurn5Tiles_0,         &Default::LeftQuarterTurn5Tiles_1,         &Default::LeftQuarterTurn5Tiles_2,         &Default::LeftQuarterTurn5Tiles_3,         // LeftQuarterTurn5Tiles
        &Default::RightQuarterTurn5Tiles_0,        &Default::RightQuarterTurn5Tiles_1,        &Default::RightQuarterTurn5Tiles_2,        &Default::RightQuarterTurn5Tiles_3,        // RightQuarterTurn5Tiles
        &Default::FlatToLeftBank_0,                &Default::FlatToLeftBank_1,                &Default::FlatToLeftBank_2,                &Default::FlatToLeftBank_3,                // FlatToLeftBank
        &Default::FlatToRightBank_0,               &Default::FlatToRightBank_1,               &Default::FlatToRightBank_2,               &Default::FlatToRightBank_3,               // FlatToRightBank
        &Default::LeftBankToFlat_0,                &Default::LeftBankToFlat_1,                &Default::LeftBankToFlat_2,                &Default::LeftBankToFlat_3,                // LeftBankToFlat
        &Default::RightBankToFlat_0,               &Default::RightBankToFlat_1,               &Default::RightBankToFlat_2,               &Default::RightBankToFlat_3,               // RightBankToFlat
        &Default::BankedLeftQuarterTurn5Tiles_0,   &Default::BankedLeftQuarterTurn5Tiles_1,   &Default::BankedLeftQuarterTurn5Tiles_2,   &Default::BankedLeftQuarterTurn5Tiles_3,   // BankedLeftQuarterTurn5Tiles
        &Default::BankedRightQuarterTurn5Tiles_0,  &Default::BankedRightQuarterTurn5Tiles_1,  &Default::BankedRightQuarterTurn5Tiles_2,  &Default::BankedRightQuarterTurn5Tiles_3,  // BankedRightQuarterTurn5Tiles
        &Default::LeftBankToUp25_0,                &Default::LeftBankToUp25_1,                &Default::LeftBankToUp25_2,                &Default::LeftBankToUp25_3,                // LeftBankToUp25
        &Default::RightBankToUp25_0,               &Default::RightBankToUp25_1,               &Default::RightBankToUp25_2,               &Default::RightBankToUp25_3,               // RightBankToUp25
        &Default::Up25ToLeftBank_0,                &Default::Up25ToLeftBank_1,                &Default::Up25ToLeftBank_2,                &Default::Up25ToLeftBank_3,                // Up25ToLeftBank
        &Default::Up25ToRightBank_0,               &Default::Up25ToRightBank_1,               &Default::Up25ToRightBank_2,               &Default::Up25ToRightBank_3,               // Up25ToRightBank
        &Default::LeftBankToDown25_0,              &Default::LeftBankToDown25_1,              &Default::LeftBankToDown25_2,              &Default::LeftBankToDown25_3,              // LeftBankToDown25
        &Default::RightBankToDown25_0,             &Default::RightBankToDown25_1,             &Default::RightBankToDown25_2,             &Default::RightBankToDown25_3,             // RightBankToDown25
        &Default::Down25ToLeftBank_0,              &Default::Down25ToLeftBank_1,              &Default::Down25ToLeftBank_2,              &Default::Down25ToLeftBank_3,              // Down25ToLeftBank
        &Default::Down25ToRightBank_0,             &Default::Down25ToRightBank_1,             &Default::Down25ToRightBank_2,             &Default::Down25ToRightBank_3,             // Down25ToRightBank
        &Default::LeftBank_0,                      &Default::LeftBank_1,                      &Default::LeftBank_2,                      &Default::LeftBank_3,                      // LeftBank
        &Default::RightBank_0,                     &Default::RightBank_1,                     &Default::RightBank_2,                     &Default::RightBank_3,                     // RightBank
        &Default::LeftQuarterTurn5TilesUp25_0,     &Default::LeftQuarterTurn5TilesUp25_1,     &Default::LeftQuarterTurn5TilesUp25_2,     &Default::LeftQuarterTurn5TilesUp25_3,     // LeftQuarterTurn5TilesUp25
        &Default::RightQuarterTurn5TilesUp25_0,    &Default::RightQuarterTurn5TilesUp25_1,    &Default::RightQuarterTurn5TilesUp25_2,    &Default::RightQuarterTurn5TilesUp25_3,    // RightQuarterTurn5TilesUp25
        &Default::LeftQuarterTurn5TilesDown25_0,   &Default::LeftQuarterTurn5TilesDown25_1,   &Default::LeftQuarterTurn5TilesDown25_2,   &Default::LeftQuarterTurn5TilesDown25_3,   // LeftQuarterTurn5TilesDown25
        &Default::RightQuarterTurn5TilesDown25_0,  &Default::RightQuarterTurn5TilesDown25_1,  &Default::RightQuarterTurn5TilesDown25_2,  &Default::RightQuarterTurn5TilesDown25_3,  // RightQuarterTurn5TilesDown25
        &Default::SBendLeft_0,                     &Default::SBendLeft_1,                     &Default::SBendLeft_2,                     &Default::SBendLeft_3,                     // SBendLeft
        &Default::SBendRight_0,                    &Default::SBendRight_1,                    &Default::SBendRight_2,                    &Default::SBendRight_3,                    // SBendRight
        &Default::LeftVerticalLoop_0,              &Default::LeftVerticalLoop_1,              &Default::LeftVerticalLoop_2,              &Default::LeftVerticalLoop_3,              // LeftVerticalLoop
        &Default::RightVerticalLoop_0,             &Default::RightVerticalLoop_1,             &Default::RightVerticalLoop_2,             &Default::RightVerticalLoop_3,             // RightVerticalLoop
        &Default::LeftQuarterTurn3Tiles_0,         &Default::LeftQuarterTurn3Tiles_1,         &Default::LeftQuarterTurn3Tiles_2,         &Default::LeftQuarterTurn3Tiles_3,         // LeftQuarterTurn3Tiles
        &Default::RightQuarterTurn3Tiles_0,        &Default::RightQuarterTurn3Tiles_1,        &Default::RightQuarterTurn3Tiles_2,        &Default::RightQuarterTurn3Tiles_3,        // RightQuarterTurn3Tiles
        &Default::LeftBankedQuarterTurn3Tiles_0,   &Default::LeftBankedQuarterTurn3Tiles_1,   &Default::LeftBankedQuarterTurn3Tiles_2,   &Default::LeftBankedQuarterTurn3Tiles_3,   // LeftBankedQuarterTurn3Tiles
        &Default::RightBankedQuarterTurn3Tiles_0,  &Default::RightBankedQuarterTurn3Tiles_1,  &Default::RightBankedQuarterTurn3Tiles_2,  &Default::RightBankedQuarterTurn3Tiles_3,  // RightBankedQuarterTurn3Tiles
        &Default::LeftQuarterTurn3TilesUp25_0,     &Default::LeftQuarterTurn3TilesUp25_1,     &Default::LeftQuarterTurn3TilesUp25_2,     &Default::LeftQuarterTurn3TilesUp25_3,     // LeftQuarterTurn3TilesUp25
        &Default::RightQuarterTurn3TilesUp25_0,    &Default::RightQuarterTurn3TilesUp25_1,    &Default::RightQuarterTurn3TilesUp25_2,    &Default::RightQuarterTurn3TilesUp25_3,    // RightQuarterTurn3TilesUp25
        &Default::LeftQuarterTurn3TilesDown25_0,   &Default::LeftQuarterTurn3TilesDown25_1,   &Default::LeftQuarterTurn3TilesDown25_2,   &Default::LeftQuarterTurn3TilesDown25_3,   // LeftQuarterTurn3TilesDown25
        &Default::RightQuarterTurn3TilesDown25_0,  &Default::RightQuarterTurn3TilesDown25_1,  &Default::RightQuarterTurn3TilesDown25_2,  &Default::RightQuarterTurn3TilesDown25_3,  // RightQuarterTurn3TilesDown25
        &Default::LeftQuarterTurn1Tile_0,          &Default::LeftQuarterTurn1Tile_1,          &Default::LeftQuarterTurn1Tile_2,          &Default::LeftQuarterTurn1Tile_3,          // LeftQuarterTurn1Tile
        &Default::RightQuarterTurn1Tile_0,         &Default::RightQuarterTurn1Tile_1,         &Default::RightQuarterTurn1Tile_2,         &Default::RightQuarterTurn1Tile_3,         // RightQuarterTurn1Tile
        &Default::LeftTwistDownToUp_0,             &Default::LeftTwistDownToUp_1,             &Default::LeftTwistDownToUp_2,             &Default::LeftTwistDownToUp_3,             // LeftTwistDownToUp
        &Default::RightTwistDownToUp_0,            &Default::RightTwistDownToUp_1,            &Default::RightTwistDownToUp_2,            &Default::RightTwistDownToUp_3,            // RightTwistDownToUp
        &Default::LeftTwistUpToDown_0,             &Default::LeftTwistUpToDown_1,             &Default::LeftTwistUpToDown_2,             &Default::LeftTwistUpToDown_3,             // LeftTwistUpToDown
        &Default::RightTwistUpToDown_0,            &Default::RightTwistUpToDown_1,            &Default::RightTwistUpToDown_2,            &Default::RightTwistUpToDown_3,            // RightTwistUpToDown
        &Default::HalfLoopUp_0,                    &Default::HalfLoopUp_1,                    &Default::HalfLoopUp_2,                    &Default::HalfLoopUp_3,                    // HalfLoopUp
        &Default::HalfLoopDown_0,                  &Default::HalfLoopDown_1,                  &Default::HalfLoopDown_2,                  &Default::HalfLoopDown_3,                  // HalfLoopDown
        &Default::LeftCorkscrewUp_0,               &Default::LeftCorkscrewUp_1,               &Default::LeftCorkscrewUp_2,               &Default::LeftCorkscrewUp_3,               // LeftCorkscrewUp
        &Default::RightCorkscrewUp_0,              &Default::RightCorkscrewUp_1,              &Default::RightCorkscrewUp_2,              &Default::RightCorkscrewUp_3,              // RightCorkscrewUp
        &Default::LeftCorkscrewDown_0,             &Default::LeftCorkscrewDown_1,             &Default::LeftCorkscrewDown_2,             &Default::LeftCorkscrewDown_3,             // LeftCorkscrewDown
        &Default::RightCorkscrewDown_0,            &Default::RightCorkscrewDown_1,            &Default::RightCorkscrewDown_2,            &Default::RightCorkscrewDown_3,            // RightCorkscrewDown
        &Default::FlatToUp60_0,                    &Default::FlatToUp60_1,                    &Default::FlatToUp60_2,                    &Default::FlatToUp60_3,                    // FlatToUp60
        &Default::Up60ToFlat_0,                    &Default::Up60ToFlat_1,                    &Default::Up60ToFlat_2,                    &Default::Up60ToFlat_3,                    // Up60ToFlat
        &Default::FlatToDown60_0,                  &Default::FlatToDown60_1,                  &Default::FlatToDown60_2,                  &Default::FlatToDown60_3,                  // FlatToDown60
        &Default::Down60ToFlat_0,                  &Default::Down60ToFlat_1,                  &Default::Down60ToFlat_2,                  &Default::Down60ToFlat_3,                  // Down60ToFlat
        &Default::TowerBase_0,                     &Default::TowerBase_1,                     &Default::TowerBase_2,                     &Default::TowerBase_3,                     // TowerBase
        &Default::TowerSection_0,                  &Default::TowerSection_1,                  &Default::TowerSection_2,                  &Default::TowerSection_3,                  // TowerSection
        &Default::Flat_0,                          &Default::Flat_1,                          &Default::Flat_2,                          &Default::Flat_3,                          // FlatCovered
        &Default::Up25_0,                          &Default::Up25_1,                          &Default::Up25_2,                          &Default::Up25_3,                          // Up25Covered
        &Default::Up60_0,                          &Default::Up60_1,                          &Default::Up60_2,                          &Default::Up60_3,                          // Up60Covered
        &Default::FlatToUp25_0,                    &Default::FlatToUp25_1,                    &Default::FlatToUp25_2,                    &Default::FlatToUp25_3,                    // FlatToUp25Covered
        &Default::Up25ToUp60_0,                    &Default::Up25ToUp60_1,                    &Default::Up25ToUp60_2,                    &Default::Up25ToUp60_3,                    // Up25ToUp60Covered
        &Default::Up60ToUp25_0,                    &Default::Up60ToUp25_1,                    &Default::Up60ToUp25_2,                    &Default::Up60ToUp25_3,                    // Up60ToUp25Covered
        &Default::Up25ToFlat_0,                    &Default::Up25ToFlat_1,                    &Default::Up25ToFlat_2,                    &Default::Up25ToFlat_3,                    // Up25ToFlatCovered
        &Default::Down25_0,                        &Default::Down25_1,                        &Default::Down25_2,                        &Default::Down25_3,                        // Down25Covered
        &Default::Down60_0,                        &Default::Down60_1,                        &Default::Down60_2,                        &Default::Down60_3,                        // Down60Covered
        &Default::FlatToDown25_0,                  &Default::FlatToDown25_1,                  &Default::FlatToDown25_2,                  &Default::FlatToDown25_3,                  // FlatToDown25Covered
        &Default::Down25ToDown60_0,                &Default::Down25ToDown60_1,                &Default::Down25ToDown60_2,                &Default::Down25ToDown60_3,                // Down25ToDown60Covered
        &Default::Down60ToDown25_0,                &Default::Down60ToDown25_1,                &Default::Down60ToDown25_2,                &Default::Down60ToDown25_3,                // Down60ToDown25Covered
        &Default::Down25ToFlat_0,                  &Default::Down25ToFlat_1,                  &Default::Down25ToFlat_2,                  &Default::Down25ToFlat_3,                  // Down25ToFlatCovered
        &Default::LeftQuarterTurn5Tiles_0,         &Default::LeftQuarterTurn5Tiles_1,         &Default::LeftQuarterTurn5Tiles_2,         &Default::LeftQuarterTurn5Tiles_3,         // LeftQuarterTurn5TilesCovered
        &Default::RightQuarterTurn5Tiles_0,        &Default::RightQuarterTurn5Tiles_1,        &Default::RightQuarterTurn5Tiles_2,        &Default::RightQuarterTurn5Tiles_3,        // RightQuarterTurn5TilesCovered
        &Default::SBendLeft_0,                     &Default::SBendLeft_1,                     &Default::SBendLeft_2,                     &Default::SBendLeft_3,                     // SBendLeftCovered
        &Default::SBendRight_0,                    &Default::SBendRight_1,                    &Default::SBendRight_2,                    &Default::SBendRight_3,                    // SBendRightCovered
        &Default::LeftQuarterTurn3Tiles_0,         &Default::LeftQuarterTurn3Tiles_1,         &Default::LeftQuarterTurn3Tiles_2,         &Default::LeftQuarterTurn3Tiles_3,         // LeftQuarterTurn3TilesCovered
        &Default::RightQuarterTurn3Tiles_0,        &Default::RightQuarterTurn3Tiles_1,        &Default::RightQuarterTurn3Tiles_2,        &Default::RightQuarterTurn3Tiles_3,        // RightQuarterTurn3TilesCovered
        &Default::LeftHalfBankedHelixUpSmall_0,    &Default::LeftHalfBankedHelixUpSmall_1,    &Default::LeftHalfBankedHelixUpSmall_2,    &Default::LeftHalfBankedHelixUpSmall_3,    // LeftHalfBankedHelixUpSmall
        &Default::RightHalfBankedHelixUpSmall_0,   &Default::RightHalfBankedHelixUpSmall_1,   &Default::RightHalfBankedHelixUpSmall_2,   &Default::RightHalfBankedHelixUpSmall_3,   // RightHalfBankedHelixUpSmall
        &Default::LeftHalfBankedHelixDownSmall_0,  &Default::LeftHalfBankedHelixDownSmall_1,  &Default::LeftHalfBankedHelixDownSmall_2,  &Default::LeftHalfBankedHelixDownSmall_3,  // LeftHalfBankedHelixDownSmall
        &Default::RightHalfBankedHelixDownSmall_0, &Default::RightHalfBankedHelixDownSmall_1, &Default::RightHalfBankedHelixDownSmall_2, &Default::RightHalfBankedHelixDownSmall_3, // RightHalfBankedHelixDownSmall
        &Default::LeftHalfBankedHelixUpLarge_0,    &Default::LeftHalfBankedHelixUpLarge_1,    &Default::LeftHalfBankedHelixUpLarge_2,    &Default::LeftHalfBankedHelixUpLarge_3,    // LeftHalfBankedHelixUpLarge
        &Default::RightHalfBankedHelixUpLarge_0,   &Default::RightHalfBankedHelixUpLarge_1,   &Default::RightHalfBankedHelixUpLarge_2,   &Default::RightHalfBankedHelixUpLarge_3,   // RightHalfBankedHelixUpLarge
        &Default::LeftHalfBankedHelixDownLarge_0,  &Default::LeftHalfBankedHelixDownLarge_1,  &Default::LeftHalfBankedHelixDownLarge_2,  &Default::LeftHalfBankedHelixDownLarge_3,  // LeftHalfBankedHelixDownLarge
        &Default::RightHalfBankedHelixDownLarge_0, &Default::RightHalfBankedHelixDownLarge_1, &Default::RightHalfBankedHelixDownLarge_2, &Default::RightHalfBankedHelixDownLarge_3, // RightHalfBankedHelixDownLarge
        &Default::LeftQuarterTurn1TileUp60_0,      &Default::LeftQuarterTurn1TileUp60_1,      &Default::LeftQuarterTurn1TileUp60_2,      &Default::LeftQuarterTurn1TileUp60_3,      // LeftQuarterTurn1TileUp60
        &Default::RightQuarterTurn1TileUp60_0,     &Default::RightQuarterTurn1TileUp60_1,     &Default::RightQuarterTurn1TileUp60_2,     &Default::RightQuarterTurn1TileUp60_3,     // RightQuarterTurn1TileUp60
        &Default::LeftQuarterTurn1TileDown60_0,    &Default::LeftQuarterTurn1TileDown60_1,    &Default::LeftQuarterTurn1TileDown60_2,    &Default::LeftQuarterTurn1TileDown60_3,    // LeftQuarterTurn1TileDown60
        &Default::RightQuarterTurn1TileDown60_0,   &Default::RightQuarterTurn1TileDown60_1,   &Default::RightQuarterTurn1TileDown60_2,   &Default::RightQuarterTurn1TileDown60_3,   // RightQuarterTurn1TileDown60
        &EndBullwheel_0,                           &EndBullwheel_1,                           &EndBullwheel_2,                           &EndBullwheel_3,                           // Brakes
        &EndBullwheel_0,                           &EndBullwheel_1,                           &EndBullwheel_2,                           &EndBullwheel_3,                           // Booster
    };
    static_assert(std::size(TrackVehicleInfoListChairliftEndBullwheel) == kVehicleTrackSubpositionSizeChairliftGoingBack);

    static constexpr const VehicleInfoList * TrackVehicleInfoListChairliftStartBullwheel[] = {
        &StartBullwheel_0,                         &StartBullwheel_1,                         &StartBullwheel_2,                         &StartBullwheel_3,                         // Flat
        &StartBullwheel_0,                         &StartBullwheel_1,                         &StartBullwheel_2,                         &StartBullwheel_3,                         // EndStation
        &StartBullwheel_0,                         &StartBullwheel_1,                         &StartBullwheel_2,                         &StartBullwheel_3,                         // BeginStation
        &StartBullwheel_0,                         &StartBullwheel_1,                         &StartBullwheel_2,                         &StartBullwheel_3,                         // MiddleStation
        &Default::Up25_0,                          &Default::Up25_1,                          &Default::Up25_2,                          &Default::Up25_3,                          // Up25
        &Default::Up60_0,                          &Default::Up60_1,                          &Default::Up60_2,                          &Default::Up60_3,                          // Up60
        &Default::FlatToUp25_0,                    &Default::FlatToUp25_1,                    &Default::FlatToUp25_2,                    &Default::FlatToUp25_3,                    // FlatToUp25
        &Default::Up25ToUp60_0,                    &Default::Up25ToUp60_1,                    &Default::Up25ToUp60_2,                    &Default::Up25ToUp60_3,                    // Up25ToUp60
        &Default::Up60ToUp25_0,                    &Default::Up60ToUp25_1,                    &Default::Up60ToUp25_2,                    &Default::Up60ToUp25_3,                    // Up60ToUp25
        &Default::Up25ToFlat_0,                    &Default::Up25ToFlat_1,                    &Default::Up25ToFlat_2,                    &Default::Up25ToFlat_3,                    // Up25ToFlat
        &Default::Down25_0,                        &Default::Down25_1,                        &Default::Down25_2,                        &Default::Down25_3,                        // Down25
        &Default::Down60_0,                        &Default::Down60_1,                        &Default::Down60_2,                        &Default::Down60_3,                        // Down60
        &Default::FlatToDown25_0,                  &Default::FlatToDown25_1,                  &Default::FlatToDown25_2,                  &Default::FlatToDown25_3,                  // FlatToDown25
        &Default::Down25ToDown60_0,                &Default::Down25ToDown60_1,                &Default::Down25ToDown60_2,                &Default::Down25ToDown60_3,                // Down25ToDown60
        &Default::Down60ToDown25_0,                &Default::Down60ToDown25_1,                &Default::Down60ToDown25_2,                &Default::Down60ToDown25_3,                // Down60ToDown25
        &Default::Down25ToFlat_0,                  &Default::Down25ToFlat_1,                  &Default::Down25ToFlat_2,                  &Default::Down25ToFlat_3,                  // Down25ToFlat
        &Default::LeftQuarterTurn5Tiles_0,         &Default::LeftQuarterTurn5Tiles_1,         &Default::LeftQuarterTurn5Tiles_2,         &Default::LeftQuarterTurn5Tiles_3,         // LeftQuarterTurn5Tiles
        &Default::RightQuarterTurn5Tiles_0,        &Default::RightQuarterTurn5Tiles_1,        &Default::RightQuarterTurn5Tiles_2,        &Default::RightQuarterTurn5Tiles_3,        // RightQuarterTurn5Tiles
        &Default::FlatToLeftBank_0,                &Default::FlatToLeftBank_1,                &Default::FlatToLeftBank_2,                &Default::FlatToLeftBank_3,                // FlatToLeftBank
        &Default::FlatToRightBank_0,               &Default::FlatToRightBank_1,               &Default::FlatToRightBank_2,               &Default::FlatToRightBank_3,               // FlatToRightBank
        &Default::LeftBankToFlat_0,                &Default::LeftBankToFlat_1,                &Default::LeftBankToFlat_2,                &Default::LeftBankToFlat_3,                // LeftBankToFlat
        &Default::RightBankToFlat_0,               &Default::RightBankToFlat_1,               &Default::RightBankToFlat_2,               &Default::RightBankToFlat_3,               // RightBankToFlat
        &Default::BankedLeftQuarterTurn5Tiles_0,   &Default::BankedLeftQuarterTurn5Tiles_1,   &Default::BankedLeftQuarterTurn5Tiles_2,   &Default::BankedLeftQuarterTurn5Tiles_3,   // BankedLeftQuarterTurn5Tiles
        &Default::BankedRightQuarterTurn5Tiles_0,  &Default::BankedRightQuarterTurn5Tiles_1,  &Default::BankedRightQuarterTurn5Tiles_2,  &Default::BankedRightQuarterTurn5Tiles_3,  // BankedRightQuarterTurn5Tiles
        &Default::LeftBankToUp25_0,                &Default::LeftBankToUp25_1,                &Default::LeftBankToUp25_2,                &Default::LeftBankToUp25_3,                // LeftBankToUp25
        &Default::RightBankToUp25_0,               &Default::RightBankToUp25_1,               &Default::RightBankToUp25_2,               &Default::RightBankToUp25_3,               // RightBankToUp25
        &Default::Up25ToLeftBank_0,                &Default::Up25ToLeftBank_1,                &Default::Up25ToLeftBank_2,                &Default::Up25ToLeftBank_3,                // Up25ToLeftBank
        &Default::Up25ToRightBank_0,               &Default::Up25ToRightBank_1,               &Default::Up25ToRightBank_2,               &Default::Up25ToRightBank_3,               // Up25ToRightBank
        &Default::LeftBankToDown25_0,              &Default::LeftBankToDown25_1,              &Default::LeftBankToDown25_2,              &Default::LeftBankToDown25_3,              // LeftBankToDown25
        &Default::RightBankToDown25_0,             &Default::RightBankToDown25_1,             &Default::RightBankToDown25_2,             &Default::RightBankToDown25_3,             // RightBankToDown25
        &Default::Down25ToLeftBank_0,              &Default::Down25ToLeftBank_1,              &Default::Down25ToLeftBank_2,              &Default::Down25ToLeftBank_3,              // Down25ToLeftBank
        &Default::Down25ToRightBank_0,             &Default::Down25ToRightBank_1,             &Default::Down25ToRightBank_2,             &Default::Down25ToRightBank_3,             // Down25ToRightBank
        &Default::LeftBank_0,                      &Default::LeftBank_1,                      &Default::LeftBank_2,                      &Default::LeftBank_3,                      // LeftBank
        &Default::RightBank_0,                     &Default::RightBank_1,                     &Default::RightBank_2,                     &Default::RightBank_3,                     // RightBank
        &Default::LeftQuarterTurn5TilesUp25_0,     &Default::LeftQuarterTurn5TilesUp25_1,     &Default::LeftQuarterTurn5TilesUp25_2,     &Default::LeftQuarterTurn5TilesUp25_3,     // LeftQuarterTurn5TilesUp25
        &Default::RightQuarterTurn5TilesUp25_0,    &Default::RightQuarterTurn5TilesUp25_1,    &Default::RightQuarterTurn5TilesUp25_2,    &Default::RightQuarterTurn5TilesUp25_3,    // RightQuarterTurn5TilesUp25
        &Default::LeftQuarterTurn5TilesDown25_0,   &Default::LeftQuarterTurn5TilesDown25_1,   &Default::LeftQuarterTurn5TilesDown25_2,   &Default::LeftQuarterTurn5TilesDown25_3,   // LeftQuarterTurn5TilesDown25
        &Default::RightQuarterTurn5TilesDown25_0,  &Default::RightQuarterTurn5TilesDown25_1,  &Default::RightQuarterTurn5TilesDown25_2,  &Default::RightQuarterTurn5TilesDown25_3,  // RightQuarterTurn5TilesDown25
        &Default::SBendLeft_0,                     &Default::SBendLeft_1,                     &Default::SBendLeft_2,                     &Default::SBendLeft_3,                     // SBendLeft
        &Default::SBendRight_0,                    &Default::SBendRight_1,                    &Default::SBendRight_2,                    &Default::SBendRight_3,                    // SBendRight
        &Default::LeftVerticalLoop_0,              &Default::LeftVerticalLoop_1,              &Default::LeftVerticalLoop_2,              &Default::LeftVerticalLoop_3,              // LeftVerticalLoop
        &Default::RightVerticalLoop_0,             &Default::RightVerticalLoop_1,             &Default::RightVerticalLoop_2,             &Default::RightVerticalLoop_3,             // RightVerticalLoop
        &Default::LeftQuarterTurn3Tiles_0,         &Default::LeftQuarterTurn3Tiles_1,         &Default::LeftQuarterTurn3Tiles_2,         &Default::LeftQuarterTurn3Tiles_3,         // LeftQuarterTurn3Tiles
        &Default::RightQuarterTurn3Tiles_0,        &Default::RightQuarterTurn3Tiles_1,        &Default::RightQuarterTurn3Tiles_2,        &Default::RightQuarterTurn3Tiles_3,        // RightQuarterTurn3Tiles
        &Default::LeftBankedQuarterTurn3Tiles_0,   &Default::LeftBankedQuarterTurn3Tiles_1,   &Default::LeftBankedQuarterTurn3Tiles_2,   &Default::LeftBankedQuarterTurn3Tiles_3,   // LeftBankedQuarterTurn3Tiles
        &Default::RightBankedQuarterTurn3Tiles_0,  &Default::RightBankedQuarterTurn3Tiles_1,  &Default::RightBankedQuarterTurn3Tiles_2,  &Default::RightBankedQuarterTurn3Tiles_3,  // RightBankedQuarterTurn3Tiles
        &Default::LeftQuarterTurn3TilesUp25_0,     &Default::LeftQuarterTurn3TilesUp25_1,     &Default::LeftQuarterTurn3TilesUp25_2,     &Default::LeftQuarterTurn3TilesUp25_3,     // LeftQuarterTurn3TilesUp25
        &Default::RightQuarterTurn3TilesUp25_0,    &Default::RightQuarterTurn3TilesUp25_1,    &Default::RightQuarterTurn3TilesUp25_2,    &Default::RightQuarterTurn3TilesUp25_3,    // RightQuarterTurn3TilesUp25
        &Default::LeftQuarterTurn3TilesDown25_0,   &Default::LeftQuarterTurn3TilesDown25_1,   &Default::LeftQuarterTurn3TilesDown25_2,   &Default::LeftQuarterTurn3TilesDown25_3,   // LeftQuarterTurn3TilesDown25
        &Default::RightQuarterTurn3TilesDown25_0,  &Default::RightQuarterTurn3TilesDown25_1,  &Default::RightQuarterTurn3TilesDown25_2,  &Default::RightQuarterTurn3TilesDown25_3,  // RightQuarterTurn3TilesDown25
        &Default::LeftQuarterTurn1Tile_0,          &Default::LeftQuarterTurn1Tile_1,          &Default::LeftQuarterTurn1Tile_2,          &Default::LeftQuarterTurn1Tile_3,          // LeftQuarterTurn1Tile
        &Default::RightQuarterTurn1Tile_0,         &Default::RightQuarterTurn1Tile_1,         &Default::RightQuarterTurn1Tile_2,         &Default::RightQuarterTurn1Tile_3,         // RightQuarterTurn1Tile
        &Default::LeftTwistDownToUp_0,             &Default::LeftTwistDownToUp_1,             &Default::LeftTwistDownToUp_2,             &Default::LeftTwistDownToUp_3,             // LeftTwistDownToUp
        &Default::RightTwistDownToUp_0,            &Default::RightTwistDownToUp_1,            &Default::RightTwistDownToUp_2,            &Default::RightTwistDownToUp_3,            // RightTwistDownToUp
        &Default::LeftTwistUpToDown_0,             &Default::LeftTwistUpToDown_1,             &Default::LeftTwistUpToDown_2,             &Default::LeftTwistUpToDown_3,             // LeftTwistUpToDown
        &Default::RightTwistUpToDown_0,            &Default::RightTwistUpToDown_1,            &Default::RightTwistUpToDown_2,            &Default::RightTwistUpToDown_3,            // RightTwistUpToDown
        &Default::HalfLoopUp_0,                    &Default::HalfLoopUp_1,                    &Default::HalfLoopUp_2,                    &Default::HalfLoopUp_3,                    // HalfLoopUp
        &Default::HalfLoopDown_0,                  &Default::HalfLoopDown_1,                  &Default::HalfLoopDown_2,                  &Default::HalfLoopDown_3,                  // HalfLoopDown
        &Default::LeftCorkscrewUp_0,               &Default::LeftCorkscrewUp_1,               &Default::LeftCorkscrewUp_2,               &Default::LeftCorkscrewUp_3,               // LeftCorkscrewUp
        &Default::RightCorkscrewUp_0,              &Default::RightCorkscrewUp_1,              &Default::RightCorkscrewUp_2,              &Default::RightCorkscrewUp_3,              // RightCorkscrewUp
        &Default::LeftCorkscrewDown_0,             &Default::LeftCorkscrewDown_1,             &Default::LeftCorkscrewDown_2,             &Default::LeftCorkscrewDown_3,             // LeftCorkscrewDown
        &Default::RightCorkscrewDown_0,            &Default::RightCorkscrewDown_1,            &Default::RightCorkscrewDown_2,            &Default::RightCorkscrewDown_3,            // RightCorkscrewDown
        &Default::FlatToUp60_0,                    &Default::FlatToUp60_1,                    &Default::FlatToUp60_2,                    &Default::FlatToUp60_3,                    // FlatToUp60
        &Default::Up60ToFlat_0,                    &Default::Up60ToFlat_1,                    &Default::Up60ToFlat_2,                    &Default::Up60ToFlat_3,                    // Up60ToFlat
        &Default::FlatToDown60_0,                  &Default::FlatToDown60_1,                  &Default::FlatToDown60_2,                  &Default::FlatToDown60_3,                  // FlatToDown60
        &Default::Down60ToFlat_0,                  &Default::Down60ToFlat_1,                  &Default::Down60ToFlat_2,                  &Default::Down60ToFlat_3,                  // Down60ToFlat
        &Default::TowerBase_0,                     &Default::TowerBase_1,                     &Default::TowerBase_2,                     &Default::TowerBase_3,                     // TowerBase
        &Default::TowerSection_0,                  &Default::TowerSection_1,                  &Default::TowerSection_2,                  &Default::TowerSection_3,                  // TowerSection
        &Default::Flat_0,                          &Default::Flat_1,                          &Default::Flat_2,                          &Default::Flat_3,                          // FlatCovered
        &Default::Up25_0,                          &Default::Up25_1,                          &Default::Up25_2,                          &Default::Up25_3,                          // Up25Covered
        &Default::Up60_0,                          &Default::Up60_1,                          &Default::Up60_2,                          &Default::Up60_3,                          // Up60Covered
        &Default::FlatToUp25_0,                    &Default::FlatToUp25_1,                    &Default::FlatToUp25_2,                    &Default::FlatToUp25_3,                    // FlatToUp25Covered
        &Default::Up25ToUp60_0,                    &Default::Up25ToUp60_1,                    &Default::Up25ToUp60_2,                    &Default::Up25ToUp60_3,                    // Up25ToUp60Covered
        &Default::Up60ToUp25_0,                    &Default::Up60ToUp25_1,                    &Default::Up60ToUp25_2,                    &Default::Up60ToUp25_3,                    // Up60ToUp25Covered
        &Default::Up25ToFlat_0,                    &Default::Up25ToFlat_1,                    &Default::Up25ToFlat_2,                    &Default::Up25ToFlat_3,                    // Up25ToFlatCovered
        &Default::Down25_0,                        &Default::Down25_1,                        &Default::Down25_2,                        &Default::Down25_3,                        // Down25Covered
        &Default::Down60_0,                        &Default::Down60_1,                        &Default::Down60_2,                        &Default::Down60_3,                        // Down60Covered
        &Default::FlatToDown25_0,                  &Default::FlatToDown25_1,                  &Default::FlatToDown25_2,                  &Default::FlatToDown25_3,                  // FlatToDown25Covered
        &Default::Down25ToDown60_0,                &Default::Down25ToDown60_1,                &Default::Down25ToDown60_2,                &Default::Down25ToDown60_3,                // Down25ToDown60Covered
        &Default::Down60ToDown25_0,                &Default::Down60ToDown25_1,                &Default::Down60ToDown25_2,                &Default::Down60ToDown25_3,                // Down60ToDown25Covered
        &Default::Down25ToFlat_0,                  &Default::Down25ToFlat_1,                  &Default::Down25ToFlat_2,                  &Default::Down25ToFlat_3,                  // Down25ToFlatCovered
        &Default::LeftQuarterTurn5Tiles_0,         &Default::LeftQuarterTurn5Tiles_1,         &Default::LeftQuarterTurn5Tiles_2,         &Default::LeftQuarterTurn5Tiles_3,         // LeftQuarterTurn5TilesCovered
        &Default::RightQuarterTurn5Tiles_0,        &Default::RightQuarterTurn5Tiles_1,        &Default::RightQuarterTurn5Tiles_2,        &Default::RightQuarterTurn5Tiles_3,        // RightQuarterTurn5TilesCovered
        &Default::SBendLeft_0,                     &Default::SBendLeft_1,                     &Default::SBendLeft_2,                     &Default::SBendLeft_3,                     // SBendLeftCovered
        &Default::SBendRight_0,                    &Default::SBendRight_1,                    &Default::SBendRight_2,                    &Default::SBendRight_3,                    // SBendRightCovered
        &Default::LeftQuarterTurn3Tiles_0,         &Default::LeftQuarterTurn3Tiles_1,         &Default::LeftQuarterTurn3Tiles_2,         &Default::LeftQuarterTurn3Tiles_3,         // LeftQuarterTurn3TilesCovered
        &Default::RightQuarterTurn3Tiles_0,        &Default::RightQuarterTurn3Tiles_1,        &Default::RightQuarterTurn3Tiles_2,        &Default::RightQuarterTurn3Tiles_3,        // RightQuarterTurn3TilesCovered
        &Default::LeftHalfBankedHelixUpSmall_0,    &Default::LeftHalfBankedHelixUpSmall_1,    &Default::LeftHalfBankedHelixUpSmall_2,    &Default::LeftHalfBankedHelixUpSmall_3,    // LeftHalfBankedHelixUpSmall
        &Default::RightHalfBankedHelixUpSmall_0,   &Default::RightHalfBankedHelixUpSmall_1,   &Default::RightHalfBankedHelixUpSmall_2,   &Default::RightHalfBankedHelixUpSmall_3,   // RightHalfBankedHelixUpSmall
        &Default::LeftHalfBankedHelixDownSmall_0,  &Default::LeftHalfBankedHelixDownSmall_1,  &Default::LeftHalfBankedHelixDownSmall_2,  &Default::LeftHalfBankedHelixDownSmall_3,  // LeftHalfBankedHelixDownSmall
        &Default::RightHalfBankedHelixDownSmall_0, &Default::RightHalfBankedHelixDownSmall_1, &Default::RightHalfBankedHelixDownSmall_2, &Default::RightHalfBankedHelixDownSmall_3, // RightHalfBankedHelixDownSmall
        &Default::LeftHalfBankedHelixUpLarge_0,    &Default::LeftHalfBankedHelixUpLarge_1,    &Default::LeftHalfBankedHelixUpLarge_2,    &Default::LeftHalfBankedHelixUpLarge_3,    // LeftHalfBankedHelixUpLarge
        &Default::RightHalfBankedHelixUpLarge_0,   &Default::RightHalfBankedHelixUpLarge_1,   &Default::RightHalfBankedHelixUpLarge_2,   &Default::RightHalfBankedHelixUpLarge_3,   // RightHalfBankedHelixUpLarge
        &Default::LeftHalfBankedHelixDownLarge_0,  &Default::LeftHalfBankedHelixDownLarge_1,  &Default::LeftHalfBankedHelixDownLarge_2,  &Default::LeftHalfBankedHelixDownLarge_3,  // LeftHalfBankedHelixDownLarge
        &Default::RightHalfBankedHelixDownLarge_0, &Default::RightHalfBankedHelixDownLarge_1, &Default::RightHalfBankedHelixDownLarge_2, &Default::RightHalfBankedHelixDownLarge_3, // RightHalfBankedHelixDownLarge
        &Default::LeftQuarterTurn1TileUp60_0,      &Default::LeftQuarterTurn1TileUp60_1,      &Default::LeftQuarterTurn1TileUp60_2,      &Default::LeftQuarterTurn1TileUp60_3,      // LeftQuarterTurn1TileUp60
        &Default::RightQuarterTurn1TileUp60_0,     &Default::RightQuarterTurn1TileUp60_1,     &Default::RightQuarterTurn1TileUp60_2,     &Default::RightQuarterTurn1TileUp60_3,     // RightQuarterTurn1TileUp60
        &Default::LeftQuarterTurn1TileDown60_0,    &Default::LeftQuarterTurn1TileDown60_1,    &Default::LeftQuarterTurn1TileDown60_2,    &Default::LeftQuarterTurn1TileDown60_3,    // LeftQuarterTurn1TileDown60
        &Default::RightQuarterTurn1TileDown60_0,   &Default::RightQuarterTurn1TileDown60_1,   &Default::RightQuarterTurn1TileDown60_2,   &Default::RightQuarterTurn1TileDown60_3,   // RightQuarterTurn1TileDown60
        &StartBullwheel_0,                         &StartBullwheel_1,                         &StartBullwheel_2,                         &StartBullwheel_3,                         // Brakes
        &StartBullwheel_0,                         &StartBullwheel_1,                         &StartBullwheel_2,                         &StartBullwheel_3,                         // Booster
    };
    static_assert(std::size(TrackVehicleInfoListChairliftStartBullwheel) == kVehicleTrackSubpositionSizeChairliftGoingBack);

    //clang-format on
}
