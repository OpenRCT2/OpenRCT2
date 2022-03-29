/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../Editor.h"
#include "../Game.h"
#include "../OpenRCT2.h"
#include "../actions/RideSetStatusAction.h"
#include "../audio/AudioMixer.h"
#include "../audio/audio.h"
#include "../config/Config.h"
#include "../core/Memory.hpp"
#include "../entity/EntityRegistry.h"
#include "../entity/Particle.h"
#include "../interface/Viewport.h"
#include "../localisation/Formatter.h"
#include "../localisation/Localisation.h"
#include "../management/NewsItem.h"
#include "../platform/Platform.h"
#include "../profiling/Profiling.h"
#include "../rct12/RCT12.h"
#include "../scenario/Scenario.h"
#include "../scripting/HookEngine.h"
#include "../scripting/ScriptEngine.h"
#include "../util/Util.h"
#include "../windows/Intent.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Surface.h"
#include "../world/Wall.h"
#include "CableLift.h"
#include "Ride.h"
#include "RideData.h"
#include "Station.h"
#include "Track.h"
#include "TrackData.h"
#include "TrainManager.h"
#include "Vehicle.h"
#include "VehicleData.h"
#include "VehicleSubpositionData.h"

#include <algorithm>
#include <iterator>

// These parameters determine when and how the rider will apply the brakes
struct RiderControlSettings
{
    int32_t maxSpeed;       // Preferred speed on straight track
    int32_t minSpeed;       // Preferred speed in the tightest turn
    int32_t brakeThreshold; // Tolerance for exceeding their preferred speed
    int32_t followDistance; // Preferred spacing between this rider and the vehicle in front of them
};

static RiderControlSettings riderTable[256] = {
    { 597126, 353262, 393697, 6 },  { 743659, 335715, 247092, 9 },  { 471260, 286344, 387545, 9 },
    { 800946, 247296, 190684, 11 }, { 699534, 377098, 353287, 8 },  { 744840, 236432, 134903, 10 },
    { 701881, 407136, 274539, 6 },  { 833042, 407712, 126769, 9 },  { 668948, 197142, 111162, 12 },
    { 675312, 410578, 283984, 8 },  { 512795, 337863, 532370, 6 },  { 642924, 385510, 214744, 7 },
    { 374502, 237846, 400423, 8 },  { 770565, 392199, 359074, 8 },  { 641450, 247832, 154621, 9 },
    { 820283, 245796, 218160, 11 }, { 948149, 408433, 238264, 10 }, { 790721, 450758, 420886, 7 },
    { 783771, 328145, 216011, 9 },  { 606795, 360987, 267713, 6 },  { 856584, 672404, 313325, 2 },
    { 617554, 292947, 357428, 9 },  { 572832, 352608, 192844, 6 },  { 719275, 403972, 435946, 9 },
    { 721581, 314852, 208986, 8 },  { 825588, 508921, 318000, 5 },  { 797958, 507867, 461140, 5 },
    { 881340, 645810, 252674, 5 },  { 795025, 432291, 322508, 6 },  { 750790, 578912, 580999, 5 },
    { 938412, 609080, 397930, 7 },  { 730820, 541045, 581398, 4 },  { 787909, 322580, 214494, 10 },
    { 888003, 334092, 167933, 9 },  { 661080, 404215, 235686, 6 },  { 773143, 535874, 521405, 6 },
    { 601080, 330741, 310526, 7 },  { 658394, 360142, 438065, 7 },  { 907981, 290038, 169404, 9 },
    { 841013, 519199, 268211, 6 },  { 619269, 340547, 231494, 9 },  { 694809, 476921, 467906, 4 },
    { 734300, 545457, 580499, 4 },  { 930628, 518222, 381038, 6 },  { 861524, 636099, 528160, 3 },
    { 803993, 471695, 445629, 7 },  { 687626, 438512, 310196, 8 },  { 774443, 277267, 222672, 11 },
    { 785140, 267532, 138660, 10 }, { 811898, 247334, 101837, 10 }, { 548407, 309568, 227508, 8 },
    { 707711, 541851, 298159, 4 },  { 610475, 414771, 482907, 5 },  { 805540, 345640, 364440, 8 },
    { 477535, 299549, 164564, 8 },  { 755616, 482138, 533994, 7 },  { 838966, 476914, 265307, 7 },
    { 782412, 551604, 318156, 6 },  { 843099, 287783, 279865, 9 },  { 538914, 369026, 292329, 6 },
    { 807103, 407604, 432702, 8 },  { 765491, 332250, 133376, 9 },  { 636951, 466777, 491452, 5 },
    { 875193, 202011, 162637, 12 }, { 818563, 613405, 513792, 3 },  { 738963, 222171, 128932, 10 },
    { 542171, 197468, 160294, 11 }, { 726196, 480287, 286767, 4 },  { 655673, 392474, 326720, 5 },
    { 815189, 437618, 241230, 8 },  { 665048, 501612, 394447, 4 },  { 508462, 271081, 346987, 10 },
    { 644385, 336994, 152733, 9 },  { 612279, 391510, 469414, 5 },  { 567836, 291687, 349698, 9 },
    { 716968, 291316, 247989, 8 },  { 834982, 231868, 128111, 12 }, { 808683, 604715, 560643, 5 },
    { 884600, 395986, 231530, 8 },  { 349307, 217940, 168760, 10 }, { 768015, 447538, 154161, 6 },
    { 747210, 536320, 471791, 6 },  { 714659, 415963, 200747, 7 },  { 718122, 539936, 478635, 6 },
    { 712347, 545846, 505653, 3 },  { 632325, 197406, 107103, 13 }, { 822027, 200793, 191260, 11 },
    { 703051, 202321, 201001, 10 }, { 736628, 427753, 246411, 6 },  { 733139, 279446, 301253, 11 },
    { 878209, 698816, 575626, 4 },  { 903482, 544627, 331369, 5 },  { 627559, 385127, 332857, 7 },
    { 706549, 478487, 548834, 4 },  { 793578, 363674, 149061, 8 },  { 676566, 203338, 152631, 10 },
    { 654298, 311778, 126003, 8 },  { 749593, 463146, 179666, 8 },  { 440735, 197233, 136356, 11 },
    { 816865, 224025, 167271, 11 }, { 667655, 274493, 233726, 9 },  { 464533, 200895, 117184, 13 },
    { 695994, 214618, 198776, 11 }, { 520947, 355334, 238861, 4 },  { 457277, 210092, 195551, 12 },
    { 516952, 248957, 319745, 11 }, { 845227, 497802, 177438, 6 },  { 483421, 323920, 274495, 6 },
    { 769287, 508463, 402035, 4 },  { 792143, 456852, 144004, 7 },  { 662411, 490378, 238697, 3 },
    { 802462, 284054, 246080, 11 }, { 778389, 302111, 286748, 9 },  { 420286, 225210, 132409, 8 },
    { 557146, 238366, 95265, 12 },  { 731979, 279920, 222679, 11 }, { 683982, 207310, 111268, 12 },
    { 722734, 493050, 466635, 5 },  { 761748, 404999, 389876, 6 },  { 753195, 602448, 673292, 2 },
    { 392978, 257733, 192420, 9 },  { 608648, 210629, 173958, 10 }, { 629164, 370401, 453347, 6 },
    { 777066, 552738, 501266, 4 },  { 554308, 292484, 135227, 7 },  { 575253, 310272, 277346, 8 },
    { 762990, 528673, 361078, 6 },  { 829257, 605282, 331741, 3 },  { 872283, 689740, 239513, 3 },
    { 725041, 381087, 236822, 8 },  { 595104, 356278, 185933, 6 },  { 708872, 227196, 193627, 12 },
    { 650114, 430401, 448902, 7 },  { 741306, 241408, 130482, 12 }, { 697177, 229199, 117195, 10 },
    { 494213, 270366, 366390, 8 },  { 685201, 242900, 188830, 10 }, { 723888, 284606, 302735, 8 },
    { 796485, 469322, 414208, 7 },  { 751991, 594280, 211065, 5 },  { 305580, 217827, 411166, 7 },
    { 645835, 458046, 623644, 4 },  { 624299, 322514, 298155, 9 },  { 918126, 290759, 280991, 9 },
    { 559914, 331153, 157117, 7 },  { 635423, 314328, 373472, 8 },  { 715814, 370422, 289727, 7 },
    { 891567, 339163, 302924, 10 }, { 740133, 379768, 262121, 7 },  { 647272, 240484, 160570, 10 },
    { 704218, 278033, 270732, 11 }, { 586673, 269145, 142158, 9 },  { 614059, 257897, 211920, 11 },
    { 766750, 451919, 465631, 7 },  { 680297, 388863, 350611, 6 },  { 923989, 444227, 329656, 7 },
    { 847399, 524492, 336551, 6 },  { 720428, 429459, 298123, 8 },  { 674052, 361074, 299345, 9 },
    { 528505, 207461, 75056, 12 },  { 672785, 197923, 91253, 12 },  { 728506, 291985, 269529, 10 },
    { 532089, 336684, 161604, 5 },  { 759281, 213388, 107094, 10 }, { 622641, 198732, 177286, 10 },
    { 813533, 332364, 244695, 8 },  { 899276, 199463, 116952, 12 }, { 756833, 542445, 416007, 4 },
    { 760512, 348329, 360893, 7 },  { 431124, 289796, 292361, 6 },  { 648699, 315002, 118564, 10 },
    { 688832, 457232, 412054, 5 },  { 565256, 321229, 284692, 8 },  { 698357, 471963, 526610, 5 },
    { 682758, 222561, 190982, 12 }, { 584473, 303025, 385763, 8 },  { 535556, 248491, 212931, 8 },
    { 686415, 466330, 519414, 6 },  { 737794, 446296, 309648, 8 },  { 524794, 229841, 219583, 10 },
    { 823795, 445589, 217293, 8 },  { 779721, 312356, 172765, 10 }, { 854207, 610113, 617821, 3 },
    { 786520, 424892, 318968, 7 },  { 620965, 288099, 167874, 10 }, { 789309, 353223, 331260, 8 },
    { 713504, 559233, 246878, 4 },  { 810281, 473106, 298576, 8 },  { 864095, 418795, 224640, 8 },
    { 727351, 221675, 124139, 11 }, { 638464, 331503, 273620, 8 },  { 615817, 240638, 122094, 10 },
    { 851886, 547473, 410834, 4 },  { 671512, 220145, 122677, 9 },  { 593048, 320358, 379925, 8 },
    { 625938, 210751, 135585, 10 }, { 666355, 484353, 304272, 6 },  { 692429, 221114, 97549, 11 },
    { 705384, 449745, 352418, 4 },  { 651519, 445073, 282917, 6 },  { 630752, 454251, 307037, 4 },
    { 693620, 401170, 416823, 8 },  { 503935, 335379, 364381, 7 },  { 633881, 425151, 395046, 6 },
    { 710032, 256007, 175843, 12 }, { 746024, 516552, 484615, 6 },  { 449390, 264525, 126390, 8 },
    { 735463, 572600, 494315, 4 },  { 742481, 564482, 362756, 4 },  { 599118, 392901, 244893, 5 },
    { 670233, 215471, 82501, 12 },  { 570361, 325916, 345857, 8 },  { 407803, 247942, 247244, 9 },
    { 681530, 499728, 520309, 5 },  { 679059, 243507, 157668, 9 },  { 499193, 336747, 310000, 5 },
    { 866741, 672412, 254848, 3 },  { 754403, 381408, 249028, 7 },  { 729663, 426012, 216056, 6 },
    { 831134, 394964, 235630, 10 }, { 579951, 305436, 360528, 7 },  { 849618, 615033, 622304, 5 },
    { 700708, 311435, 163556, 9 },  { 748400, 557469, 357103, 5 },  { 657038, 390428, 143171, 7 },
    { 869468, 237583, 110289, 12 }, { 827409, 570724, 306546, 6 },  { 711190, 302815, 195445, 8 },
    { 758054, 331870, 272018, 9 },  { 659742, 217126, 126639, 12 }, { 771851, 514686, 254163, 5 },
    { 663733, 440108, 326197, 5 },  { 588834, 213692, 178557, 10 }, { 764238, 203619, 105288, 13 },
    { 690035, 202010, 125927, 11 }, { 677815, 452464, 417573, 6 },  { 691233, 366563, 210373, 9 },
    { 895316, 420962, 413227, 7 },  { 859022, 453666, 407748, 8 },  { 775750, 205333, 78547, 13 },
    { 962620, 670223, 599500, 5 },  { 799445, 212141, 107545, 12 }, { 488966, 220998, 245094, 10 },
    { 582233, 221677, 186846, 12 }, { 912833, 729283, 311606, 3 },  { 603012, 315332, 356485, 9 },
    { 590959, 263482, 286186, 8 },  { 781062, 582032, 349539, 5 },  { 562616, 214802, 219711, 10 },
    { 639964, 393297, 512468, 5 },  { 545333, 228934, 115244, 10 }, { 652913, 293544, 330861, 9 },
    { 836956, 571437, 357082, 5 },  { 551397, 282284, 225356, 10 }, { 577625, 284878, 250819, 10 },
    { 604917, 280526, 225782, 9 }
};

int32_t Vehicle::CalculateRiderBraking() const
{
    if (num_peeps == 0)
        return 0;

    RiderControlSettings riderSettings = riderTable[peep[0].ToUnderlying() & 0xFF];

    // Acceleration to apply when rider is braking
    int32_t minBrake = (1 << 16);
    // Acceleration to apply when rider is braking hard (used when going much faster than the rider's preferred speed or very
    // close to the rider in front)
    int32_t maxBrake = (15 << 16);

    // Velocity above which riders will attempt to maintain separation from the vehicle in front
    int32_t minFollowVelocity = 1 << 16;

    // Brake if close to the vehicle in front
    Vehicle* prevVehicle = GetEntity<Vehicle>(prev_vehicle_on_ride);
    if (prevVehicle != nullptr && this != prevVehicle)
    {
        int32_t followDistance = std::max(
            0, (riderSettings.followDistance * (_vehicleVelocityF64E08 - minFollowVelocity)) >> 15);
        int32_t distance = std::max(abs(x - prevVehicle->x), abs(y - prevVehicle->y));
        int32_t relativeVelocity = velocity - prevVehicle->velocity;
        int32_t z_diff = abs(z - prevVehicle->z);

        if (distance < followDistance && z_diff < 16 && relativeVelocity > -(1 << 16))
        {
            if (distance < followDistance / 2 || relativeVelocity > riderSettings.brakeThreshold)
            {
                return -maxBrake;
            }
            else
            {
                return -minBrake;
            }
        }
    }

    // Brake more on turns
    int32_t targetSpeed = 0;
    auto trackType = GetTrackType();
    switch (trackType)
    {
        case TrackElemType::LeftQuarterTurn3Tiles:
        case TrackElemType::RightQuarterTurn3Tiles:
            targetSpeed = riderSettings.minSpeed;
            break;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
        case TrackElemType::RightQuarterTurn3TilesUp25:
        case TrackElemType::LeftQuarterTurn3TilesDown25:
        case TrackElemType::RightQuarterTurn3TilesDown25:
        case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
        case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
        case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
        case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
            targetSpeed = (riderSettings.maxSpeed + 3 * riderSettings.minSpeed) / 4;
            break;
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
        case TrackElemType::RightBankedQuarterTurn3Tiles:
        case TrackElemType::LeftQuarterTurn5Tiles:
        case TrackElemType::RightQuarterTurn5Tiles:
            targetSpeed = (riderSettings.maxSpeed + riderSettings.minSpeed) / 2;
            break;
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
        case TrackElemType::BankedRightQuarterTurn5Tiles:
        case TrackElemType::LeftQuarterTurn5TilesUp25:
        case TrackElemType::RightQuarterTurn5TilesUp25:
        case TrackElemType::LeftQuarterTurn5TilesDown25:
        case TrackElemType::RightQuarterTurn5TilesDown25:
        case TrackElemType::LeftEighthToDiag:
        case TrackElemType::RightEighthToDiag:
        case TrackElemType::LeftEighthToOrthogonal:
        case TrackElemType::RightEighthToOrthogonal:
            targetSpeed = (3 * riderSettings.maxSpeed + riderSettings.minSpeed) / 4;
            break;
        default:
            targetSpeed = riderSettings.maxSpeed;
            break;
    }

    // Brake if car exceeds rider's preferred max speed
    if (_vehicleVelocityF64E08 > targetSpeed + riderSettings.brakeThreshold)
    {
        return -maxBrake;
    }
    else if (_vehicleVelocityF64E08 > targetSpeed)
    {
        return -minBrake;
    }

    return 0;
}
