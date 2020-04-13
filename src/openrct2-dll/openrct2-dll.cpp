#include "../openrct2/core/Path.hpp"
#include "../openrct2/core/String.hpp"

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

        // this one gives lots of "Object not found" messages.
        //context->LoadParkFromFile(std::string(filepath));

        load_from_sv6(filepath);
        game_load_init();

        Park& park = context->GetGameState()->GetPark();
        const char* name = park.Name.c_str();

        printf("(me) LoadPark() = %s\n", name);

    }


    EXPORT const char* GetParkName()
    {
        Park& park = context->GetGameState()->GetPark();
        const char* name = park.Name.c_str();

        printf("(me) GetParkName() = %s\n", name);
        return name;
    }


    EXPORT TileElement* GetMapElementAt(int x, int y)
    {
        TileElement* element = map_get_first_element_at({x, y});

        printf("(me) GetMapElementAt( %i, %i )\n", x, y);
        return element;
    }


    EXPORT int GetMapElementsAt(int x, int y, TileElement* elements, int arraySize)
    {
        int elementCount = 0;

        TileElement* element = map_get_first_element_at({ x, y });

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
