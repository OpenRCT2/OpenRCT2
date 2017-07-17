#include "GameAction.h"
#include "PlaceParkEntranceAction.hpp"
#include "SetParkEntranceFeeAction.hpp"

namespace GameActions {

void Register()
{
    Register<SetParkEntranceFeeAction>();
    Register<PlaceParkEntranceAction>();
}

}