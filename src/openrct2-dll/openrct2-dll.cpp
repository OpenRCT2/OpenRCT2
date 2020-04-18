#include "../openrct2/core/Path.hpp"
#include "../openrct2/core/String.hpp"
#include "../openrct2/world/Location.hpp"

#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/platform/platform.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Park.h>


#define EXPORT __declspec(dllexport)

using namespace OpenRCT2;


std::unique_ptr<IContext> context;

extern "C"
{
    EXPORT void StartGame(const char* datapath)
    {
        printf("(me) StartGame( %s )\n", datapath);

        _log_levels[DIAGNOSTIC_LEVEL_VERBOSE] = true;

        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;

        Path::GetAbsolute(gCustomOpenrctDataPath, std::size(gCustomOpenrctDataPath), datapath);

        String::Set(gCustomRCT1DataPath, std::size(gCustomRCT1DataPath), "D:\\Games\\RollerCoaster Tycoon Deluxe\\data");
        String::Set(gCustomRCT2DataPath, std::size(gCustomRCT2DataPath), "D:\\Games\\RollerCoaster Tycoon 2 Triple Thrill Pack\\data");

        printf("(me) gCustomRCT1DataPath = %s\n(me) gCustomRCT2DataPath = %s\n", gCustomRCT1DataPath, gCustomRCT2DataPath);

        // Create a plain context
        core_init();
        context = CreateContext(); // OpenRCT2::CreateContext()
        bool result = context->Initialise();

        printf("(me) Initialise = %i\n", result);
    }


    EXPORT void StopGame()
    {
        printf("(me) StopGame()\n");

        context = nullptr;
    }


    EXPORT void LoadPark(const char* filepath)
    {
        printf("(me) LoadPark( %s )\n", filepath);

        // this one gives lots of "Object not found" messages 
        context->LoadParkFromFile(std::string(filepath));

        //load_from_sv6(filepath);
        //game_load_init();

        Park& park = context->GetGameState()->GetPark();
        const char* name = park.Name.c_str();

        printf("(me) LoadPark() = %s\n", name);

    }


    EXPORT const char* GetParkNamePtr()
    {
        Park& park = context->GetGameState()->GetPark();
        const char* name = park.Name.c_str();

        printf("(me) GetParkName() = %s\n", name);
        return name;
    }


    EXPORT int GetMapSize()
    {
        return gMapSize;
    }


    EXPORT void GetMapElementAt(int x, int y, TileElement* element)
    {
        CoordsXY coords = CoordsXY(x * COORDS_XY_STEP, y * COORDS_XY_STEP).ToTileStart();
        *element = *map_get_first_element_at(coords);

        printf("(me) GetMapElementAt( %i, %i ) = ( %i, %i )\n", x, y, coords.x, coords.y);
        printf("(me)   -> %i \t(type)\n", element->type);
        printf("(me)   -> %i \t(flags)\n", element->Flags);
        printf("(me)   -> %i \t(base_height)\n", element->base_height);
        printf("(me)   -> %i \t(clearance_height)\n", element->clearance_height);
        printf("(me)   -> %i \t(pad 0x1)\n", element->pad_04[0]);
        printf("(me)   -> %i \t(pad 0x2)\n", element->pad_04[1]);
        printf("(me)   -> %i \t(pad 0x3)\n", element->pad_04[2]);
        printf("(me)   -> %i \t(pad 0x4)\n", element->pad_04[3]);
        printf("(me)   -> %i \t(pad 0x5)\n", element->pad_08[0]);
        printf("(me)   -> %i \t(pad 0x6)\n", element->pad_08[1]);
        printf("(me)   -> %i \t(pad 0x7)\n", element->pad_08[2]);
        printf("(me)   -> %i \t(pad 0x8)\n", element->pad_08[3]);
        printf("(me)   -> %i \t(pad 0x9)\n", element->pad_08[4]);
        printf("(me)   -> %i \t(pad 0xA)\n", element->pad_08[5]);
        printf("(me)   -> %i \t(pad 0xB)\n", element->pad_08[6]);
        printf("(me)   -> %i \t(pad 0xC)\n", element->pad_08[7]);
    }


    EXPORT int GetMapElementsAt(int x, int y, TileElement* elements, int arraySize)
    {
        int elementCount = 0;

        CoordsXY coords = CoordsXY(x * COORDS_XY_STEP, y * COORDS_XY_STEP).ToTileStart();
        TileElement* element = map_get_first_element_at(coords);

        for (int i = 0; i < arraySize; i++)
        {
            elements[i] = *element;
            elementCount++;

            if (element->IsLastForTile())
                break;

            element++;
        }

        printf("(me) GetMapElementsAt( %i, %i ) = %i\n", x, y, elementCount);
        return elementCount;
    }
}
