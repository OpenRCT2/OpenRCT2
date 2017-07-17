#include "PlaceParkEntranceAction.hpp"
#include "SetParkEntranceFeeAction.hpp"

extern "C"
{
#pragma region PlaceParkEntranceAction
    money32 place_park_entrance(sint16 x, sint16 y, sint16 z, uint8 direction)
    {
        auto gameAction = PlaceParkEntranceAction();
        gameAction.x = x;
        gameAction.y = y;
        gameAction.z = z;
        gameAction.direction = direction;
        auto result = GameActions::Execute(&gameAction);
        if (result.Error == GA_ERROR::OK)
        {
            return 0;
        }
        else
        {
            return MONEY32_UNDEFINED;
        }
    }

    /**
    *
    *  rct2: 0x006666E7
    */
    void game_command_place_park_entrance(sint32* eax,
                                          sint32* ebx,
                                          sint32* ecx,
                                          sint32* edx,
                                          sint32* esi,
                                          sint32* edi,
                                          sint32* ebp)
    {
        Guard::Assert(false, "GAME_COMMAND_PLACE_PARK_ENTRANCE DEPRECIATED");
    }

    /**
    *
    *  rct2: 0x00666F4E
    */
    money32 park_entrance_place_ghost(sint32 x, sint32 y, sint32 z, sint32 direction)
    {
        park_entrance_remove_ghost();

        auto gameAction = PlaceParkEntranceAction();
        gameAction.x = x;
        gameAction.y = y;
        gameAction.z = z;
        gameAction.direction = direction;
        gameAction.SetFlags(GAME_COMMAND_FLAG_GHOST);

        auto result = GameActions::Execute(&gameAction);
        if (result.Error == GA_ERROR::OK)
        {
            gParkEntranceGhostPosition.x = x;
            gParkEntranceGhostPosition.y = y;
            gParkEntranceGhostPosition.z = z;
            gParkEntranceGhostDirection = direction;
            gParkEntranceGhostExists = true;
        }
        return result.Cost;
    }
#pragma endregion

#pragma region SetParkEntranceFeeAction
    void park_set_entrance_fee(money32 value)
    {
        auto gameAction = SetParkEntranceFeeAction();
        gameAction.Fee = (money16)value;
        gameAction.SetCallback([]()
        {
            log_info("GameAction Callback executed");
        });
        GameActions::Execute(&gameAction);
    }

    void game_command_set_park_entrance_fee(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
    {
        auto gameAction = SetParkEntranceFeeAction();
        gameAction.Fee = (*edi & 0xFFFF);
        GameActions::Execute(&gameAction);
    }
#pragma endregion
}
