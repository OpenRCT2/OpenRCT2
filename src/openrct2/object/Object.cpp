#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <algorithm>
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../localisation/Language.h"
#include "../localisation/StringIds.h"
#include "Object.h"
#include "ObjectLimits.h"

Object::Object(const rct_object_entry &entry)
{
    _objectEntry = entry;

    char name[DAT_NAME_LENGTH + 1] = { 0 };
    std::copy_n(entry.name, DAT_NAME_LENGTH, name);
    _identifier = String::Duplicate(name);

    if (IsRCT1Object())
    {
        SetSourceGame(OBJECT_SOURCE_RCT1);
    }
    else if (IsAAObject())
    {
        SetSourceGame(OBJECT_SOURCE_ADDED_ATTRACTIONS);
    }
    else if (IsLLObject())
    {
        SetSourceGame(OBJECT_SOURCE_LOOPY_LANDSCAPES);
    }
    else if (IsOpenRCT2OfficialObject())
    {
        SetSourceGame(OBJECT_SOURCE_OPENRCT2_OFFICIAL);
    }
}

Object::~Object()
{
    Memory::Free(_identifier);
}

std::string Object::GetOverrideString(uint8 index) const
{
    const char * identifier = GetIdentifier();
    rct_string_id stringId = language_get_object_override_string_id(identifier, index);

    const utf8 * result = nullptr;
    if (stringId != STR_NONE)
    {
        result = language_get_string(stringId);
    }
    return String::ToStd(result);
}

std::string Object::GetString(uint8 index) const
{
    auto sz = GetOverrideString(index);
    if (sz.empty())
    {
        sz = GetStringTable().GetString(index);
    }
    return sz;
}

rct_object_entry Object::GetScgWallsHeader()
{
    return Object::CreateHeader("SCGWALLS", 207140231, 3518650219);
}

rct_object_entry Object::GetScgPathXHeader()
{
    return Object::CreateHeader("SCGPATHX", 207140231, 890227440);
}

rct_object_entry Object::CreateHeader(const char name[DAT_NAME_LENGTH + 1], uint32 flags, uint32 checksum)
{
    rct_object_entry header = { 0 };
    header.flags = flags;
    std::copy_n(name, DAT_NAME_LENGTH, header.name);
    header.checksum = checksum;
    return header;
}

void Object::SetSourceGame(const uint8 sourceGame)
{
    // FIXME: Temporary disabled because it breaks exporting to vanilla.
    /*_objectEntry.flags &= 0x0F;
    _objectEntry.flags |= (sourceGame << 4);*/
}

bool Object::IsRCT1Object()
{
    static const char _rct1Objects[][9] =
    {
        "CLIFT1  ",
        "MONO1   ",
        "MONO2   ",
        "NRL     ",
        "CTCAR   ",
        "DODG1   ",
        "FWH1    ",
        "HHBUILD ",
        "HMAZE   ",
        "HSKELT  ",
        "MGR1    ",
        "OBS1    ",
        "RCR     ",
        "SPCAR   ",
        "SRINGS  ",
        "TRUCK1  ",
        "AMT1    ",
        "ARRSW1  ",
        "ARRSW2  ",
        "BMVD    ",
        "BOB1    ",
        "PTCT1   ",
        "RCKC    ",
        "REVF1   ",
        "SKYTR   ",
        "STEEP1  ",
        "STEEP2  ",
        "TOGST   ",
        "WMMINE  ",
        "WMOUSE  ",
        "ZLDB    ",
        "ZLOG    ",
        "C3D     ",
        "KART1   ",
        "SIMPOD  ",
        "SSC1    ",
        "SWSH1   ",
        "SWSH2   ",
        "TOPSP1  ",
        "TWIST1  ",
        "BBOAT   ",
        "CBOAT   ",
        "DING1   ",
        "LFB1    ",
        "RAPBOAT ",
        "RBOAT   ",
        "SWANS   ",
        "TRIKE   ",
        "BALLN   ",
        "BURGB   ",
        "CHPSH   ",
        "CNDYF   ",
        "DRNKS   ",
        "ICECR1  ",
        "INFOK   ",
        "PIZZS   ",
        "POPCS   ",
        "SOUVS   ",
        "TLT1    ",

        // Small scenery
        "ALLSORT1",
        "ALLSORT2",
        "TAC     ",
        "TAL     ",
        "TAP     ",
        "TAS     ",
        "TAS1    ",
        "TAS2    ",
        "TAS3    ",
        "TAS4    ",
        "TB1     ",
        "TB2     ",
        "TBC     ",
        "TBP     ",
        "TBR     ",
        "TBR1    ",
        "TBR2    ",
        "TBR3    ",
        "TBR4    ",
        "TBW     ",
        "TCB     ",
        "TCC     ",
        "TCE     ",
        "TCF     ",
        "TCJ     ",
        "TCL     ",
        "TCO     ",
        "TCRP    ",
        "TCT     ",
        "TCT1    ",
        "TCT2    ",
        "TCY     ",
        "TDM     ",
        "TEL     ",
        "TEN     ",
        "TEP     ",
        "TERB    ",
        "TERS    ",
        "TES1    ",
        "TF1     ",
        "TF2     ",
        "TGHC    ",
        "TGHC2   ",
        "TGS     ",
        "TH1     ",
        "TH2     ",
        "THL     ",
        "THRS    ",
        "THT     ",
        "TIC     ",
        "TITC    ",
        "TK1     ",
        "TK2     ",
        "TK3     ",
        "TK4     ",
        "TL0     ",
        "TL1     ",
        "TL2     ",
        "TL3     ",
        "TLC     ",
        "TLP     ",
        "TLY     ",
        "TM0     ",
        "TM1     ",
        "TM2     ",
        "TM3     ",
        "TMBJ    ",
        "TMC     ",
        "TMG     ",
        "TMJ     ",
        "TML     ",
        "TMO1    ",
        "TMO2    ",
        "TMO3    ",
        "TMO4    ",
        "TMO5    ",
        "TMP     ",
        "TMS1    ",
        "TMW     ",
        "TMZP    ",
        "TNS     ",
        "TP1     ",
        "TP2     ",
        "TPM     ",
        "TQ1     ",
        "TQ2     ",
        "TR1     ",
        "TR2     ",
        "TRC     ",
        "TRF     ",
        "TRF2    ",
        "TRMS    ",
        "TRWS    ",
        "TS0     ",
        "TS1     ",
        "TS2     ",
        "TS3     ",
        "TS4     ",
        "TS5     ",
        "TS6     ",
        "TSB     ",
        "TSC     ",
        "TSD     ",
        "TSH     ",
        "TSH0    ",
        "TSH1    ",
        "TSH2    ",
        "TSH3    ",
        "TSH4    ",
        "TSH5    ",
        "TSP     ",
        "TSQ     ",
        "TST1    ",
        "TST2    ",
        "TST3    ",
        "TST4    ",
        "TST5    ",
        "TSTD    ",
        "TT1     ",
        "TUS     ",
        "TVL     ",
        "TWH1    ",
        "TWH2    ",
        "TWN     ",
        "TWP     ",
        "TWW     ",
        "TDF     ",
        "TEF     ",
        "TQF     ",
        "TTF     ",
        "TWF     ",
        "TCK     ",
        "TG1     ",
        "TG10    ",
        "TG11    ",
        "TG12    ",
        "TG13    ",
        "TG14    ",
        "TG15    ",
        "TG16    ",
        "TG17    ",
        "TG18    ",
        "TG19    ",
        "TG2     ",
        "TG20    ",
        "TG21    ",
        "TG3     ",
        "TG4     ",
        "TG5     ",
        "TG6     ",
        "TG7     ",
        "TG8     ",
        "TG9     ",

        // Large Scenery
        "SCLN    ",
        "SHS1    ",
        "SHS2    ",
        "SMH1    ",
        "SMH2    ",
        "SMN1    ",
        "SCOL    ",
        "SMB     ",
        "SPYR    ",
        "SSPX    ",

        // Walls
        "WC3     ",
        "WBR1    ",
        "WBR2    ",
        "WBR3    ",
        "WBRG    ",
        "WCH     ",
        "WCHG    ",
        "WCW1    ",
        "WCW2    ",
        "WEW     ",
        "WFW1    ",
        "WFWG    ",
        "WHG     ",
        "WHGG    ",
        "WMF     ",
        "WMFG    ",
        "WMW     ",
        "WMWW    ",
        "WPF     ",
        "WPFG    ",
        "WRW     ",
        "WSW     ",
        "WSW1    ",
        "WSW2    ",
        "WSWG    ",

        // Paths
        "PATHCRZY",
        "PATHDIRT",
        "ROAD    ",
        "TARMAC  ",

        // Path additions
        "LAMP1   ",
        "LAMP2   ",
        "LAMP3   ",
        "LAMP4   ",
        "LITTER1 ",
        "BENCH1  ",
        "BENCHSTN",
        "JUMPFNT1",

        // Scenery groups
        "SCGFENCE",
        "SCGGARDN",
        "SCGPATHX",
        "SCGSHRUB",
        "SCGTREES",

        "SCGCLASS",
        "SCGEGYPT",
        "SCGMART ",
        "SCGMINE ",
        "SCGWOND ",

        // Park entrance
        "PKENT1  ",

        // Water
        "WTRCYAN ",
    };

    for (const auto entry : _rct1Objects)
    {
        if (String::Equals(_identifier, entry))
            return true;
    }

    return false;
}

bool Object::IsAAObject()
{
    static const char _aaObjects[][9] =
    {
        // Rides / vehicles / stalls
        "BMFL    ",
        "BMRB    ",
        "BMSD    ",
        "BMSU    ",
        "CHBUILD ",
        "CIRCUS1 ",
        "CLIFT2  ",
        "FSAUC   ",
        "GDROP1  ",
        "GOLF1   ",
        "GTC     ",
        "HATST   ",
        "HELICAR ",
        "HOTDS   ",
        "MFT     ",
        "MONBK   ",
        "NRL2    ",
        "REVCAR  ",
        "SFRIC1  ",
        "SMC1    ",
        "SMONO   ",
        "SPBOAT  ",
        "SQDST   ",
        "TOFFS   ",
        "UTCAR   ",
        "UTCARR  ",
        "VCR     ",
        "VEKST   ",
        "VREEL   ",

        // Small scenery
        "TBN     ",
        "TBN1    ",
        "TDN4    ",
        "TDN5    ",
        "TDT1    ",
        "TDT2    ",
        "TDT3    ",
        "TGC1    ",
        "TGC2    ",
        "TGE1    ",
        "TGE2    ",
        "TGE3    ",
        "TGE4    ",
        "TGE5    ",
        "TGS1    ",
        "TGS2    ",
        "TGS3    ",
        "TGS4    ",
        "TJB1    ",
        "TJB2    ",
        "TJB3    ",
        "TJB4    ",
        "TJF     ",
        "TJP1    ",
        "TJP2    ",
        "TJT1    ",
        "TJT2    ",
        "TJT3    ",
        "TJT4    ",
        "TJT5    ",
        "TJT6    ",
        "TMM1    ",
        "TMM2    ",
        "TMM3    ",

        // Large scenery
        "SDN1    ",
        "SDN2    ",
        "SDN3    ",
        "SSK1    ",

        // Walls
        "WALLGL16",
        "WBW     ",
        "WGW2    ",
        "WJF     ",
        "WPW1    ",
        "WPW2    ",
        "WWTW    ",

        // Banners
        "BN1     ",
        "BN2     ",
        "BN3     ",
        "BN4     ",
        "BN5     ",
        "BN6     ",

        // Paths
        "PATHASH ",
        "PATHSPCE",
        "TARMACB ",
        "TARMACG ",

        // Scenery groups
        "SCGABSTR",
        "SCGJUNGL",
        "SCGJURAS",
        "SCGSPOOK",
    };

    for (const auto entry : _aaObjects)
    {
        if (String::Equals(_identifier, entry))
            return true;
    }

    return false;
}

bool Object::IsLLObject()
{
    static const char _llObjects[][9] =
    {
        // Rides / vehicles / stalls
        "AML1    ",
        "ARRT2   ",
        "CHCKS   ",
        "COFFS   ",
        "CSTBOAT ",
        "DOUGH   ",
        "ENTERP  ",
        "IVMC1   ",
        "JSKI    ",
        "LEMST   ",
        "NEMT    ",
        "RFTBOAT ",
        "SLCFO   ",
        "THCAR   ",
        "TSHRT   ",

        // Small scenery
        "TCD     ",
        "TCFS    ",
        "TCN     ",
        "TGG     ",
        "TGH1    ",
        "TGH2    ",
        "TIG     ",
        "TNSS    ",
        "TOH1    ",
        "TOH2    ",
        "TOH3    ",
        "TOS     ",
        "TOT1    ",
        "TOT2    ",
        "TOT3    ",
        "TOT4    ",
        "TRF3    ",
        "TRFS    ",
        "TSC2    ",
        "TSCP    ",
        "TSF1    ",
        "TSF2    ",
        "TSF3    ",
        "TSG     ",
        "TSK     ",
        "TSM     ",
        "TSMP    ",
        "TSNB    ",
        "TSNC    ",
        "TSP1    ",
        "TSP2    ",
        "TSPH    ",
        "TTG     ",

        // Large scenery
        "SAH     ",
        "SAH2    ",
        "SAH3    ",
        "SCT     ",
        "SGP     ",
        "SIP     ",
        "SOB     ",
        "SOH1    ",
        "SOH2    ",
        "SOH3    ",
        "SPG     ",
        "SPS     ",
        "SSH     ",
        "SSR     ",
        "SST     ",
        "STB1    ",
        "STB2    ",
        "STG1    ",
        "STG2    ",
        "STH     ",

        // Walls
        "WC1     ",
        "WC10    ",
        "WC11    ",
        "WC12    ",
        "WC13    ",
        "WC14    ",
        "WC15    ",
        "WC16    ",
        "WC17    ",
        "WC18    ",
        "WC2     ",
        "WC4     ",
        "WC5     ",
        "WC6     ",
        "WC7     ",
        "WC8     ",
        "WC9     ",
        "WPW3    ",

        // Banners
        "BN7     ",
        "BN8     ",
        "BN9     ",

        // Path additions
        "BENCHSPC",
        "JUMPSNW1",
        "LITTERSP",

        // Scenery groups
        "SCGHALLO",
        "SCGMEDIE",
        "SCGORIEN",
        "SCGSNOW ",
        "SCGSPACE",
        "SCGURBAN",

        // Water
        "WTRORNG ",
    };

    for (const auto entry : _llObjects)
    {
        if (String::Equals(_identifier, entry))
            return true;
    }

    return false;
}

bool Object::IsOpenRCT2OfficialObject()
{
    static const char _openRCT2OfficialObjects[][9] =
    {
        // Offical extended scenery set
        "XXBBBR01",
        "TTRFTL02",
        "TTRFTL03",
        "TTRFTL04",
        "TTRFTL07",
        "TTRFTL08",
        "TTPIRF02",
        "TTPIRF03",
        "TTPIRF04",
        "TTPIRF05",
        "TTPIRF07",
        "TTPIRF08",
        "MG-PRAR ",
        "TTRFWD01",
        "TTRFWD02",
        "TTRFWD03",
        "TTRFWD04",
        "TTRFWD05",
        "TTRFWD06",
        "TTRFWD07",
        "TTRFWD08",
        "TTRFGL01",
        "TTRFGL02",
        "TTRFGL03",
        "ACWW33  ",
        "ACWWF32 ",

        // Official DLC
        "BIGPANDA",
        "LITTERPA",
        "PANDAGR ",
        "SCGPANDA",
        "WTRPINK ",
        "ZPANDA  ",
    };

    for (const auto entry : _openRCT2OfficialObjects)
    {
        if (String::Equals(_identifier, entry))
            return true;
    }

    return false;
}


#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#endif

std::string Object::GetName() const
{
    return GetString(OBJ_STRING_ID_NAME);
}

#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic pop
#endif
