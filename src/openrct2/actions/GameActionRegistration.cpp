#include "GameAction.h"
#include "PlaceParkEntranceAction.hpp"
#include "SetParkEntranceFeeAction.hpp"
#include "RideCreateAction.hpp"
#include "RideSetStatus.hpp"

namespace GameActions {

void Register()
{
    Register<SetParkEntranceFeeAction>();
    Register<PlaceParkEntranceAction>();
    Register<RideCreateAction>();
    Register<RideSetStatusAction>();
}

}