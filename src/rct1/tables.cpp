#include <Windows.h>

#include "../common.h"
#include "../core/List.hpp"
#include "../ride/ride.h"
#include "import.h"

namespace RCT1
{
    const char * DefaultParkEntranceObject = "PKENT1  ";

    namespace Tables
    {
        uint8 GetRideType(uint8 rideType)
        {
            static uint8 map[] =
            {
                RIDE_TYPE_WOODEN_ROLLER_COASTER,
                RIDE_TYPE_STAND_UP_ROLLER_COASTER,
                RIDE_TYPE_SUSPENDED_SWINGING_COASTER,
                RIDE_TYPE_INVERTED_ROLLER_COASTER,
                RIDE_TYPE_JUNIOR_ROLLER_COASTER,
                RIDE_TYPE_MINIATURE_RAILWAY,
                RIDE_TYPE_MONORAIL,
                RIDE_TYPE_MINI_SUSPENDED_COASTER,
                RIDE_TYPE_BOAT_RIDE,
                RIDE_TYPE_WOODEN_WILD_MOUSE,
                RIDE_TYPE_STEEPLECHASE,
                RIDE_TYPE_CAR_RIDE,
                RIDE_TYPE_LAUNCHED_FREEFALL,
                RIDE_TYPE_BOBSLEIGH_COASTER,
                RIDE_TYPE_OBSERVATION_TOWER,
                RIDE_TYPE_LOOPING_ROLLER_COASTER,
                RIDE_TYPE_DINGHY_SLIDE,
                RIDE_TYPE_MINE_TRAIN_COASTER,
                RIDE_TYPE_CHAIRLIFT,
                RIDE_TYPE_CORKSCREW_ROLLER_COASTER,
                RIDE_TYPE_MAZE,
                RIDE_TYPE_SPIRAL_SLIDE,
                RIDE_TYPE_GO_KARTS,
                RIDE_TYPE_LOG_FLUME,
                RIDE_TYPE_RIVER_RAPIDS,
                RIDE_TYPE_DODGEMS,
                RIDE_TYPE_PIRATE_SHIP,
                RIDE_TYPE_SWINGING_INVERTER_SHIP,
                RIDE_TYPE_FOOD_STALL,
                RIDE_TYPE_FOOD_STALL,
                RIDE_TYPE_DRINK_STALL,
                RIDE_TYPE_FOOD_STALL,
                RIDE_TYPE_SHOP,
                RIDE_TYPE_MERRY_GO_ROUND,
                RIDE_TYPE_SHOP,
                RIDE_TYPE_INFORMATION_KIOSK,
                RIDE_TYPE_TOILETS,
                RIDE_TYPE_FERRIS_WHEEL,
                RIDE_TYPE_MOTION_SIMULATOR,
                RIDE_TYPE_3D_CINEMA,
                RIDE_TYPE_TOP_SPIN,
                RIDE_TYPE_SPACE_RINGS,
                RIDE_TYPE_REVERSE_FREEFALL_COASTER,
                RIDE_TYPE_SHOP,
                RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER,
                RIDE_TYPE_FOOD_STALL,
                RIDE_TYPE_TWIST,
                RIDE_TYPE_HAUNTED_HOUSE,
                RIDE_TYPE_FOOD_STALL,
                RIDE_TYPE_CIRCUS_SHOW,
                RIDE_TYPE_GHOST_TRAIN,
                RIDE_TYPE_TWISTER_ROLLER_COASTER,
                RIDE_TYPE_WOODEN_ROLLER_COASTER,
                RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER,
                RIDE_TYPE_WILD_MOUSE,
                RIDE_TYPE_FOOD_STALL,
                RIDE_TYPE_FOOD_STALL,
                RIDE_TYPE_SHOP,
                RIDE_TYPE_FOOD_STALL,
                RIDE_TYPE_VIRGINIA_REEL,
                RIDE_TYPE_SPLASH_BOATS,
                RIDE_TYPE_MINI_HELICOPTERS,
                RIDE_TYPE_LAY_DOWN_ROLLER_COASTER,
                RIDE_TYPE_SUSPENDED_MONORAIL,
                RIDE_TYPE_NULL,
                RIDE_TYPE_REVERSER_ROLLER_COASTER,
                RIDE_TYPE_HEARTLINE_TWISTER_COASTER,
                RIDE_TYPE_MINI_GOLF,
                RIDE_TYPE_NULL,
                RIDE_TYPE_ROTO_DROP,
                RIDE_TYPE_FLYING_SAUCERS,
                RIDE_TYPE_CROOKED_HOUSE,
                RIDE_TYPE_MONORAIL_CYCLES,
                RIDE_TYPE_COMPACT_INVERTED_COASTER,
                RIDE_TYPE_WATER_COASTER,
                RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER,
                RIDE_TYPE_INVERTED_HAIRPIN_COASTER,
                RIDE_TYPE_BOAT_RIDE,
                RIDE_TYPE_SHOP,
                RIDE_TYPE_RIVER_RAFTS,
                RIDE_TYPE_FOOD_STALL,
                RIDE_TYPE_ENTERPRISE,
                RIDE_TYPE_DRINK_STALL,
                RIDE_TYPE_FOOD_STALL,
                RIDE_TYPE_DRINK_STALL
            };
            return map[rideType];
        }

        const char * GetRideTypeObject(uint8 rideType)
        {
            static const char * map[] =
            {
                "PTCT1   ",
                "TOGST   ",
                "ARRSW1  ",
                "NEMT    ",
                "ZLDB    ",
                "NRL     ",
                "MONO2   ",
                "BATFL   ",
                "RBOAT   ",
                "WMOUSE  ",
                "STEEP1  ",
                "SPCAR   ",
                "SSC1    ",
                "BOB1    ",
                "OBS1    ",
                "SCHT1   ",
                "DING1   ",
                "AMT1    ",
                "CLIFT1  ",
                "ARRT1   ",
                "HMAZE   ",
                "HSKELT  ",
                "KART1   ",
                "LFB1    ",
                "RAPBOAT ",
                "DODG1   ",
                "SWSH1   ",
                "SWSH2   ",
                "ICECR1  ",
                "CHPSH   ",
                "DRNKS   ",
                "CNDYF   ",
                "BURGB   ",
                "MGR1    ",
                "BALLN   ",
                "INFOK   ",
                "TLT1    ",
                "FWH1    ",
                "SIMPOD  ",
                "C3D     ",
                "TOPSP1  ",
                "SRINGS  ",
                "REVF1   ",
                "SOUVS   ",
                "BMVD    ",
                "PIZZS   ",
                "TWIST1  ",
                "HHBUILD ",
                "POPCS   ",
                "CIRCUS1 ",
                "GTC     ",
                "BMSD    ",
                "MFT     ",
                "SFRIC1  ",
                "SMC1    ",
                "HOTDS   ",
                "SQDST   ",
                "HATST   ",
                "TOFFS   ",
                "VREEL   ",
                "SPBOAT  ",
                "MONBK   ",
                "BMAIR   ",
                "SMONO   ",
                "        ",
                "REVCAR  ",
                "UTCAR   ",
                "GOLF1   ",
                "        ",
                "GDROP1  ",
                "FSAUC   ",
                "CHBUILD ",
                "HELICAR ",
                "SLCT    ",
                "CSTBOAT ",
                "THCAR   ",
                "IVMC1   ",
                "JSKI    ",
                "TSHRT   ",
                "RFTBOAT ",
                "DOUGH   ",
                "ENTERP  ",
                "COFFS   ",
                "CHCKS   ",
                "LEMST   ",
            };
            return map[rideType];
        }

        const char * GetVehicleObject(uint8 vehicleType)
        {
            static const char * map[] =
            {
                "PTCT1   ",  // RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN
                "TOGST   ",  // RCT1_VEHICLE_TYPE_STEEL_ROLLER_COASTER_TRAIN_BACKWARDS
                "ARRSW1  ",  // RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN
                "NEMT    ",  // RCT1_VEHICLE_TYPE_INVERTED_COASTER_TRAIN (Not in RCT2)
                "ZLDB    ",  // RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_CARS
                "NRL     ",  // RCT1_VEHICLE_TYPE_LADYBIRD_CARS
                "MONO2   ",  // RCT1_VEHICLE_TYPE_STANDUP_ROLLER_COASTER_CARS
                "BATFL   ",  // RCT1_VEHICLE_TYPE_SPINNING_CARS
                "RBOAT   ",  // RCT1_VEHICLE_TYPE_SINGLE_PERSON_SWINGING_CHAIRS
                "WMOUSE  ",  // RCT1_VEHICLE_TYPE_SWANS_PEDAL_BOATS
                "STEEP1  ",  // RCT1_VEHICLE_TYPE_LARGE_MONORAIL_TRAIN
                "SPCAR   ",  // RCT1_VEHICLE_TYPE_CANOES
                "SSC1    ",  // RCT1_VEHICLE_TYPE_ROWING_BOATS
                "BOB1    ",  // RCT1_VEHICLE_TYPE_STEAM_TRAIN
                "OBS1    ",  // RCT1_VEHICLE_TYPE_WOODEN_MOUSE_CARS
                "SCHT1   ",  // RCT1_VEHICLE_TYPE_BUMPER_BOATS
                "DING1   ",  // RCT1_VEHICLE_TYPE_WOODEN_ROLLER_COASTER_TRAIN_BACKWARDS
                "AMT1    ",  // RCT1_VEHICLE_TYPE_ROCKET_CARS
                "CLIFT1  ",  // RCT1_VEHICLE_TYPE_HORSES // Steeplechase
                "ARRT1   ",  // RCT1_VEHICLE_TYPE_SPORTSCARS
                "HMAZE   ",  // RCT1_VEHICLE_TYPE_LYING_DOWN_SWINGING_CARS (Inverted single-rail)
                "HSKELT  ",  // RCT1_VEHICLE_TYPE_WOODEN_MINE_CARS
                "KART1   ",  // RCT1_VEHICLE_TYPE_SUSPENDED_SWINGING_AIRPLANE_CARS
                "LFB1    ",  // RCT1_VEHICLE_TYPE_SMALL_MONORAIL_CARS
                "RAPBOAT ",  // RCT1_VEHICLE_TYPE_WATER_TRICYCLES
                "DODG1   ",  // RCT1_VEHICLE_TYPE_LAUNCHED_FREEFALL_CAR
                "SWSH1   ",  // RCT1_VEHICLE_TYPE_BOBSLEIGH_CARS
                "SWSH2   ",  // RCT1_VEHICLE_TYPE_DINGHIES
                "ICECR1  ",  // RCT1_VEHICLE_TYPE_ROTATING_CABIN
                "CHPSH   ",  // RCT1_VEHICLE_TYPE_MINE_TRAIN
                "DRNKS   ",  // RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS
                "CNDYF   ",  // RCT1_VEHICLE_TYPE_CORKSCREW_ROLLER_COASTER_TRAIN
                "BURGB   ",  // RCT1_VEHICLE_TYPE_MOTORBIKES
                "MGR1    ",  // RCT1_VEHICLE_TYPE_RACING_CARS
                "BALLN   ",  // RCT1_VEHICLE_TYPE_TRUCKS
                "INFOK   ",  // RCT1_VEHICLE_TYPE_GO_KARTS
                "TLT1    ",  // RCT1_VEHICLE_TYPE_RAPIDS_BOATS
                "FWH1    ",  // RCT1_VEHICLE_TYPE_LOG_FLUME_BOATS
                "SIMPOD  ",  // RCT1_VEHICLE_TYPE_DODGEMS
                "C3D     ",  // RCT1_VEHICLE_TYPE_SWINGING_SHIP
                "TOPSP1  ",  // RCT1_VEHICLE_TYPE_SWINGING_INVERTER_SHIP
                "SRINGS  ",  // RCT1_VEHICLE_TYPE_MERRY_GO_ROUND
                "REVF1   ",  // RCT1_VEHICLE_TYPE_FERRIS_WHEEL
                "SOUVS   ",  // RCT1_VEHICLE_TYPE_SIMULATOR_POD
                "BMVD    ",  // RCT1_VEHICLE_TYPE_CINEMA_BUILDING
                "PIZZS   ",  // RCT1_VEHICLE_TYPE_TOPSPIN_CAR
                "TWIST1  ",  // RCT1_VEHICLE_TYPE_SPACE_RINGS
                "HHBUILD ",  // RCT1_VEHICLE_TYPE_REVERSE_FREEFALL_ROLLER_COASTER_CAR
                "POPCS   ",  // RCT1_VEHICLE_TYPE_VERTICAL_ROLLER_COASTER_CARS
                "CIRCUS1 ",  // RCT1_VEHICLE_TYPE_CAT_CARS
                "GTC     ",  // RCT1_VEHICLE_TYPE_TWIST_ARMS_AND_CARS
                "BMSD    ",  // RCT1_VEHICLE_TYPE_HAUNTED_HOUSE_BUILDING
                "MFT     ",  // RCT1_VEHICLE_TYPE_LOG_CARS
                "SFRIC1  ",  // RCT1_VEHICLE_TYPE_CIRCUS_TENT
                "SMC1    ",  // RCT1_VEHICLE_TYPE_GHOST_TRAIN_CARS
                "HOTDS   ",  // RCT1_VEHICLE_TYPE_STEEL_TWISTER_ROLLER_COASTER_TRAIN
                "SQDST   ",  // RCT1_VEHICLE_TYPE_WOODEN_TWISTER_ROLLER_COASTER_TRAIN
                "HATST   ",  // RCT1_VEHICLE_TYPE_WOODEN_SIDE_FRICTION_CARS
                "TOFFS   ",  // RCT1_VEHICLE_TYPE_VINTAGE_CARS
                "VREEL   ",  // RCT1_VEHICLE_TYPE_STEAM_TRAIN_COVERED_CARS
                "SPBOAT  ",  // RCT1_VEHICLE_TYPE_STAND_UP_STEEL_TWISTER_ROLLER_COASTER_TRAIN
                "MONBK   ",  // RCT1_VEHICLE_TYPE_FLOORLESS_STEEL_TWISTER_ROLLER_COASTER_TRAIN
                "BMAIR   ",  // RCT1_VEHICLE_TYPE_STEEL_MOUSE_CARS
                "SMONO   ",  // RCT1_VEHICLE_TYPE_CHAIRLIFT_CARS_ALTERNATIVE
                "        ",  // RCT1_VEHICLE_TYPE_SUSPENDED_MONORAIL_TRAIN
                "REVCAR  ",  // RCT1_VEHICLE_TYPE_HELICOPTER_CARS
                "UTCAR   ",  // RCT1_VEHICLE_TYPE_VIRGINIA_REEL_TUBS
                "GOLF1   ",  // RCT1_VEHICLE_TYPE_REVERSER_CARS
                "        ",  // RCT1_VEHICLE_TYPE_GOLFERS
                "GDROP1  ",  // RCT1_VEHICLE_TYPE_RIVER_RIDE_BOATS
                "FSAUC   ",  // RCT1_VEHICLE_TYPE_FLYING_ROLLER_COASTER_TRAIN
                "CHBUILD ",  // RCT1_VEHICLE_TYPE_NON_LOOPING_STEEL_TWISTER_ROLLER_COASTER_TRAIN
                "HELICAR ",  // RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS
                "SLCT    ",  // RCT1_VEHICLE_TYPE_HEARTLINE_TWISTER_CARS_REVERSED
                "CSTBOAT ",  // RCT1_VEHICLE_TYPE_RESERVED
                "THCAR   ",  // RCT1_VEHICLE_TYPE_ROTODROP_CAR
                "IVMC1   ",  // RCT1_VEHICLE_TYPE_FLYING_SAUCERS
                "JSKI    ",  // RCT1_VEHICLE_TYPE_CROOKED_HOUSE_BUILDING
                "TSHRT   ",  // RCT1_VEHICLE_TYPE_BICYCLES
                "RFTBOAT ",  // RCT1_VEHICLE_TYPE_HYPERCOASTER_TRAIN
                "DOUGH   ",  // RCT1_VEHICLE_TYPE_4_ACROSS_INVERTED_COASTER_TRAIN
                "ENTERP  ",  // RCT1_VEHICLE_TYPE_WATER_COASTER_BOATS
                "COFFS   ",  // RCT1_VEHICLE_TYPE_FACEOFF_CARS
                "CHCKS   ",  // RCT1_VEHICLE_TYPE_JET_SKIS
                "LEMST   ",  // RCT1_VEHICLE_TYPE_RAFT_BOATS
                "WMSPIN  ",  // RCT1_VEHICLE_TYPE_AMERICAN_STYLE_STEAM_TRAIN
                "SWANS   ",  // RCT1_VEHICLE_TYPE_AIR_POWERED_COASTER_TRAIN
                "MONO1   ",  // RCT1_VEHICLE_TYPE_SUSPENDED_WILD_MOUSE_CARS (Inverted Hairpin in RCT2)
                "CBOAT   ",  // RCT1_VEHICLE_TYPE_ENTERPRISE_WHEEL
            };
            return map[vehicleType];
        }

        const char * GetSmallSceneryObject(uint8 smallSceneryType)
        {
            static const char * map[] =
            {
                "TL0     ",
                "TL1     ",
                "TL2     ",
                "TL3     ",
                "TM0     ",
                "TM1     ",
                "TM2     ",
                "TM3     ",
                "TS0     ",
                "TS1     ",
                "TS2     ",
                "TS3     ",
                "TS4     ",
                "TS5     ",
                "TS6     ",
                "TIC     ",
                "TLC     ",
                "TMC     ",
                "TMP     ",
                "TITC    ",
                "TGHC    ",
                "TAC     ",
                "TGHC2   ",
                "TCJ     ",
                "TMBJ    ",
                "TCF     ",
                "TCL     ",
                "TRF     ",
                "TRF2    ",
                "TEL     ",
                "TAP     ",
                "TSP     ",
                "TMZP    ",
                "TCRP    ",
                "TBP     ",
                "TLP     ",
                "TWP     ",
                "TAS     ",
                "TMG     ",
                "TWW     ",
                "TSB     ",
                "TVL     ",
                "TCT     ",
                "TEF     ",
                "TAL     ",
                "TSQ     ",
                "THT     ",
                "TCB     ",
                "TDM     ",
                "TSD     ",
                "TGS     ",
                "TUS     ",
                "TH1     ",
                "TBC     ",
                "TH2     ",
                "TPM     ",
                "TSC     ",
                "TG1     ",
                "TWF     ",
                "TSH0    ",
                "TSH1    ",
                "TSH2    ",
                "TSH3    ",
                "TSH4    ",
                "TSH5    ",
                "TG2     ",
                "TG3     ",
                "TG4     ",
                "TG5     ",
                "TG6     ",
                "TG7     ",
                "TG8     ",
                "TG9     ",
                "TG10    ",
                "TG11    ",
                "TG12    ",
                "TG13    ",
                "TG14    ",
                "TT1     ",
                "TDF     ",
                "TSH     ",
                "THRS    ",
                "TSTD    ",
                "TRMS    ",
                "TRWS    ",
                "TRC     ",
                "TQF     ",
                "TES1    ",
                "TEN     ",
                "TERS    ",
                "TERB    ",
                "TEP     ",
                "TST1    ",
                "TST2    ",
                "TMS1    ",
                "TAS1    ",
                "TAS2    ",
                "TAS3    ",
                "TST3    ",
                "TST4    ",
                "TST5    ",
                "TAS4    ",
                "TCY     ",
                "TBW     ",
                "TBR1    ",
                "TBR2    ",
                "TML     ",
                "TMW     ",
                "TBR3    ",
                "TBR4    ",
                "TMJ     ",
                "TBR     ",
                "TMO1    ",
                "TMO2    ",
                "TMO3    ",
                "TMO4    ",
                "TMO5    ",
                "TWH1    ",
                "TWH2    ",
                "TNS     ",
                "TP1     ",
                "TP2     ",
                "TK1     ",
                "TK2     ",
                "TR1     ",
                "TR2     ",
                "TQ1     ",
                "TQ2     ",
                "TWN     ",
                "TCE     ",
                "TCO     ",
                "THL     ",
                "TCC     ",
                "TB1     ",
                "TB2     ",
                "TK3     ",
                "TK4     ",
                "TBN     ",
                "TBN1    ",
                "TDT1    ",
                "TDT2    ",
                "TDT3    ",
                "TMM1    ",
                "TMM2    ",
                "TMM3    ",
                "TGS1    ",
                "TGS2    ",
                "TGS3    ",
                "TGS4    ",
                "TDN4    ",
                "TDN5    ",
                "TJT1    ",
                "TJT2    ",
                "TJB1    ",
                "TTF     ",
                "TF1     ",
                "TF2     ",
                "TGE1    ",
                "TJT3    ",
                "TJT4    ",
                "TJP1    ",
                "TJB2    ",
                "TGE2    ",
                "TJT5    ",
                "TJB3    ",
                "TJB4    ",
                "TJT6    ",
                "TJP2    ",
                "TGE3    ",
                "TCK     ",
                "TGE4    ",
                "TGE5    ",
                "TG15    ",
                "TG16    ",
                "TG17    ",
                "TG18    ",
                "TG19    ",
                "TG20    ",
                "TG21    ",
                "TSM     ",
                "TIG     ",
                "TCFS    ",
                "TRFS    ",
                "TRF3    ",
                "TNSS    ",
                "TCT1    ",
                "TCT2    ",
                "TSF1    ",
                "TSF2    ",
                "TSF3    ",
                "TCN     ",
                "TTG     ",
                "TSNC    ",
                "TSNB    ",
                "TSCP    ",
                "TCD     ",
                "TSG     ",
                "TSK     ",
                "TGH1    ",
                "TGH2    ",
                "TSMP    ",
                "TJF     ",
                "TLY     ",
                "TGC1    ",
                "TGC2    ",
                "TGG     ",
                "TSPH    ",
                "TOH1    ",
                "TOH2    ",
                "TOT1    ",
                "TOT2    ",
                "TOS     ",
                "TOT3    ",
                "TOT4    ",
                "TSC2    ",
                "TSP1    ",
                "TOH3    ",
                "TSP2    ",
                "ROMROOF1",
                "GEOROOF1",
                "TNTROOF1",
                "JNGROOF1",
                "MINROOF1",
                "ROMROOF2",
                "GEOROOF2",
                "PAGROOF1",
                "SPCROOF1",
                "ROOF1   ",
                "ROOF2   ",
                "ROOF3   ",
                "ROOF4   ",
                "ROOF5   ",
                "ROOF6   ",
                "ROOF7   ",
                "ROOF8   ",
                "ROOF9   ",
                "ROOF10  ",
                "ROOF11  ",
                "ROOF12  ",
                "ROOF13  ",
                "ROOF14  ",
                "IGROOF  ",
                "CORROOF ",
                "CORROOF2",
            };
            return map[smallSceneryType];
        }

        const char * GetLargeSceneryObject(uint8 largeSceneryType)
        {
            static const char * map[] =
            {
                "SCOL    ",
                "SHS1    ",
                "SSPX    ",
                "SHS2    ",
                "SCLN    ",
                "SMH1    ",
                "SMH2    ",
                "SVLC    ",
                "SPYR    ",
                "SMN1    ",
                "SMB     ",
                "SSK1    ",
                "SDN1    ",
                "SDN2    ",
                "SDN3    ",
                "SIP     ",
                "STB1    ",
                "STB2    ",
                "STG1    ",
                "STG2    ",
                "SCT     ",
                "SOH1    ",
                "SOH2    ",
                "SOH3    ",
                "SGP     ",
                "SSR     ",
                "STH     ",
                "SAH     ",
                "SPS     ",
                "SPG     ",
                "SOB     ",
                "SAH2    ",
                "SST     ",
                "SSH     ",
                "SAH3    ",
                "SSIG1   ",
                "SSIG2   ",
                "SSIG3   ",
                "SSIG4   ",
            };
            return map[largeSceneryType];
        }

        const char * GetWallObject(uint8 wallType)
        {
            static const char * map[] =
            {
                "WMF     ",
                "WMFG    ",
                "WRW     ",
                "WEW     ",
                "WHG     ",
                "WHGG    ",
                "WCW1    ",
                "WCW2    ",
                "WSW     ",
                "WSWG    ",
                "WMW     ",
                "WALLGL16",
                "WFW1    ",
                "WFWG    ",
                "WPW1    ",
                "WPW2    ",
                "WPF     ",
                "WPFG    ",
                "WWTW    ",
                "WMWW    ",
                "WSW1    ",
                "WSW2    ",
                "WGW2    ",
                "WBW     ",
                "WBR1    ",
                "WBRG    ",
                "WALLCFAR",
                "WALLPOST",
                "WBR2    ",
                "WBR3    ",
                "WPW3    ",
                "WJF     ",
                "WCH     ",
                "WCHG    ",
                "WC1     ",
                "WC2     ",
                "WC3     ",
                "WC4     ",
                "WC5     ",
                "WC6     ",
                "WC7     ",
                "WC8     ",
                "WC9     ",
                "WC10    ",
                "WC11    ",
                "WC12    ",
                "WC13    ",
                "WC14    ",
                "WC15    ",
                "WC16    ",
                "WC17    ",
                "WC18    ",
                "WALLBRDR",
                "WALLBR32",
                "WALLBR16",
                "WALLBR8 ",
                "WALLCF8 ",
                "WALLCF16",
                "WALLCF32",
                "WALLBB8 ",
                "WALLBB16",
                "WALLBB32",
                "WALLRS8 ",
                "WALLRS16",
                "WALLRS32",
                "WALLCB8 ",
                "WALLCB16",
                "WALLCB32",
                "WALLGL8 ",
                "WALLGL32",
                "WALLWD8 ",
                "WALLWD16",
                "WALLWD32",
                "WALLTN32",
                "WALLJN32",
                "WALLMN32",
                "WALLSP32",
                "WALLPG32",
                "WALLU132",
                "WALLU232",
                "WALLCZ32",
                "WALLCW32",
                "WALLCY32",
                "WALLCX32",
                "WBR1A   ",
                "WBR2A   ",
                "WRWA    ",
                "WWTWA   ",
                "WALLIG16",
                "WALLIG24",
                "WALLCO16",
                "WALLCFDR",
                "WALLCBDR",
                "WALLBRWN",
                "WALLCFWN",
                "WALLCBWN",
            };
            return map[wallType];
        }

        const char * GetSceneryGroupObject(uint8 sceneryGroupType)
        {
            static const char * map[] =
            {
                "        ", // RCT1_SCENERY_THEME_GENERAL
                "SCGMINE ", // RCT1_SCENERY_THEME_MINE
                "SCGCLASS", // RCT1_SCENERY_THEME_CLASSICAL_ROMAN
                "SCGEGYPT", // RCT1_SCENERY_THEME_EGYPTIAN
                "SCGMART ", // RCT1_SCENERY_THEME_MARTIAN
                "SCGWOND ", // RCT1_SCENERY_THEME_TOYLAND
                "SCGJURAS", // RCT1_SCENERY_THEME_JURASSIC
                "SCGSPOOK", // RCT1_SCENERY_THEME_GRAVEYARD
                "SCGJUNGL", // RCT1_SCENERY_THEME_JUNGLE
                "SCGABSTR", // RCT1_SCENERY_THEME_ABSTRACT
                "SCGSNOW ", // RCT1_SCENERY_THEME_SNOW_ICE
                "SCGMEDIE", // RCT1_SCENERY_THEME_MEDIEVAL
                "SCGSPACE", // RCT1_SCENERY_THEME_SPACE
                "SCGHALLO", // RCT1_SCENERY_THEME_CREEPY
                "SCGURBAN", // RCT1_SCENERY_THEME_URBAN
                "SCGORIEN", // RCT1_SCENERY_THEME_PAGODA
            };
            return map[sceneryGroupType];
        }

        const char * GetWaterObject(uint8 waterType)
        {
            static const char * map[] =
            {
                "WTRCYAN ",
                "WTRORNG ",
            };
            return map[waterType];
        }

        const List<const char *> GetPreferedRideEntryOrder(uint8 rideType)
        {
            static const List<const char *> preferedRideEntryOrder[] =
            {
                { "SPDRCR  "},                                                              // RIDE_TYPE_SPIRAL_ROLLER_COASTER
                { "TOGST   "},                                                              // RIDE_TYPE_STAND_UP_ROLLER_COASTER
                { "ARRSW1  ", "VEKVAMP ", "ARRSW2 "},                                       // RIDE_TYPE_SUSPENDED_SWINGING_COASTER
                { "NEMT    "},                                                              // RIDE_TYPE_INVERTED_ROLLER_COASTER
                { "ZLDB    ", "ZLOG    "},                                                  // RIDE_TYPE_JUNIOR_ROLLER_COASTER
                { "NRL     ", "NRL2    ", "AML1    ", "TRAM1   "},                          // RIDE_TYPE_MINIATURE_RAILWAY
                { "MONO1   ", "MONO2   ", "MONO3   "},                                      // RIDE_TYPE_MONORAIL
                { "BATFL   ", "SKYTR   "},                                                  // RIDE_TYPE_MINI_SUSPENDED_COASTER
                { "RBOAT   ", "BBOAT   ", "CBOAT   ", "SWANS   ", "TRIKE   ", "JSKI    " }, // RIDE_TYPE_BOAT_RIDE
                { "WMOUSE  ", "WMMINE  "},                                                  // RIDE_TYPE_WOODEN_WILD_MOUSE
                { "STEEP1  ", "STEEP2  ", "SBOX    "},                                      // RIDE_TYPE_STEEPLECHASE
                { "SPCAR   ", "RCR     ", "TRUCK1  ", "VCR     ", "CTCAR   " },             // RIDE_TYPE_CAR_RIDE
                { "SSC1    " },                                                             // RIDE_TYPE_LAUNCHED_FREEFALL
                { "BOB1    ", "INTBOB  " },                                                 // RIDE_TYPE_BOBSLEIGH_COASTER
                { "OBS1    ", "OBS2    " },                                                 // RIDE_TYPE_OBSERVATION_TOWER
                { "SCHT1   " },                                                             // RIDE_TYPE_LOOPING_ROLLER_COASTER
                { "DING1   " },                                                             // RIDE_TYPE_DINGHY_SLIDE
                { "AMT1    " },                                                             // RIDE_TYPE_MINE_TRAIN_COASTER
                { "CLIFT1  ", "CLIFT2  " },                                                 // RIDE_TYPE_CHAIRLIFT
                { "ARRT1   ", "ARRT2   " },                                                 // RIDE_TYPE_CORKSCREW_ROLLER_COASTER
                { },                                                                        // RIDE_TYPE_MAZE
                { },                                                                        // RIDE_TYPE_SPIRAL_SLIDE
                { "KART1   " },                                                             // RIDE_TYPE_GO_KARTS
                { "LFB1    " },                                                             // RIDE_TYPE_LOG_FLUME
                { "RAPBOAT " },                                                             // RIDE_TYPE_RIVER_RAPIDS
                { },                                                                        // RIDE_TYPE_DODGEMS
                { },                                                                        // RIDE_TYPE_PIRATE_SHIP
                { },                                                                        // RIDE_TYPE_SWINGING_INVERTER_SHIP
                { },                                                                        // RIDE_TYPE_FOOD_STALL
                { },                                                                        // RIDE_TYPE_1D
                { },                                                                        // RIDE_TYPE_DRINK_STALL
                { },                                                                        // RIDE_TYPE_1F
                { },                                                                        // RIDE_TYPE_SHOP
                { },                                                                        // RIDE_TYPE_MERRY_GO_ROUND
                { },                                                                        // RIDE_TYPE_22
                { },                                                                        // RIDE_TYPE_INFORMATION_KIOSK
                { },                                                                        // RIDE_TYPE_TOILETS
                { },                                                                        // RIDE_TYPE_FERRIS_WHEEL
                { },                                                                        // RIDE_TYPE_MOTION_SIMULATOR
                { },                                                                        // RIDE_TYPE_3D_CINEMA
                { },                                                                        // RIDE_TYPE_TOP_SPIN
                { },                                                                        // RIDE_TYPE_SPACE_RINGS
                { "REVF1   " },                                                             // RIDE_TYPE_REVERSE_FREEFALL_COASTER
                { "LIFT1   " },                                                             // RIDE_TYPE_LIFT
                { "BMVD    " },                                                             // RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER
                { },                                                                        // RIDE_TYPE_CASH_MACHINE
                { },                                                                        // RIDE_TYPE_TWIST
                { },                                                                        // RIDE_TYPE_HAUNTED_HOUSE
                { },                                                                        // RIDE_TYPE_FIRST_AID
                { },                                                                        // RIDE_TYPE_CIRCUS_SHOW
                { "GTC     ", "HMCAR   " },                                                 // RIDE_TYPE_GHOST_TRAIN
                { "BMSD    ", "BMSU    ", "BMFL    ", "BMRB    ", "GOLTR   " },             // RIDE_TYPE_TWISTER_ROLLER_COASTER
                { "PTCT1   ", "MFT     ", "PTCT2   " },                                     // RIDE_TYPE_WOODEN_ROLLER_COASTER
                { "SFRIC1  " },                                                             // RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER
                { "SMC1    ", "SMC2    ", "WMSPIN  " },                                     // RIDE_TYPE_WILD_MOUSE
                { "ARRX    " },                                                             // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER
                { },                                                                        // RIDE_TYPE_38
                { "BMAIR   " },                                                             // RIDE_TYPE_FLYING_ROLLER_COASTER
                { },                                                                        // RIDE_TYPE_3A
                { "VREEL   " },                                                             // RIDE_TYPE_VIRGINIA_REEL
                { "SPBOAT  " },                                                             // RIDE_TYPE_SPLASH_BOATS
                { "HELICAR " },                                                             // RIDE_TYPE_MINI_HELICOPTERS
                { "VEKST   " },                                                             // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER
                { "SMONO   " },                                                             // RIDE_TYPE_SUSPENDED_MONORAIL
                { },                                                                        // RIDE_TYPE_40
                { "REVCAR  " },                                                             // RIDE_TYPE_REVERSER_ROLLER_COASTER
                { "UTCAR   ", "UTCARR  " },                                                 // RIDE_TYPE_HEARTLINE_TWISTER_COASTER
                { },                                                                        // RIDE_TYPE_MINI_GOLF
                { "INTST   " },                                                             // RIDE_TYPE_GIGA_COASTER
                { "GDROP1  " },                                                             // RIDE_TYPE_ROTO_DROP
                { },                                                                        // RIDE_TYPE_FLYING_SAUCERS
                { },                                                                        // RIDE_TYPE_CROOKED_HOUSE
                { "MONBK   " },                                                             // RIDE_TYPE_MONORAIL_CYCLES
                { "SLCT    ", "SLCFO    ", "VEKDV   " },                                    // RIDE_TYPE_COMPACT_INVERTED_COASTER
                { "CSTBOAT " },                                                             // RIDE_TYPE_WATER_COASTER
                { "THCAR   " },                                                             // RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
                { "IVMC1   " },                                                             // RIDE_TYPE_INVERTED_HAIRPIN_COASTER
                { },                                                                        // RIDE_TYPE_MAGIC_CARPET
                { "SUBMAR  " },                                                             // RIDE_TYPE_SUBMARINE_RIDE
                { "RFTBOAT " },                                                             // RIDE_TYPE_RIVER_RAFTS
                { },                                                                        // RIDE_TYPE_50
                { },                                                                        // RIDE_TYPE_ENTERPRISE
                { },                                                                        // RIDE_TYPE_52
                { },                                                                        // RIDE_TYPE_53
                { },                                                                        // RIDE_TYPE_54
                { },                                                                        // RIDE_TYPE_55
                { "INTINV  " },                                                             // RIDE_TYPE_INVERTED_IMPULSE_COASTER
                { "WCATC   ", "RCKC     ", "JSTAR1  " },                                    // RIDE_TYPE_MINI_ROLLER_COASTER
                { "PMT1    " },                                                             // RIDE_TYPE_MINE_RIDE
                { "PREMT1  " },                                                             // RIDE_TYPE_59
            };                                                                              // RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
            return preferedRideEntryOrder[rideType];
        }
    }
}
