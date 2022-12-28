#include "PaintScriptEngine.h"

#include "../../Context.h"
#include "../../core/Console.hpp"
#include "../../object/ObjectList.h"
#include "../../object/ObjectManager.h"
#include "../../object/ObjectRepository.h"
#include "../../object/PaintObject.h"
#include "../../ride/Ride.h"
#include "../../ride/RideData.h"
#include "../Supports.h"
#include "bindings/PsPaintSession.h"
#include "bindings/PsImageId.h"
#include "bindings/PsPaint.h"
#include "bindings/PsGlobalFunctions.h"

using namespace OpenRCT2::Scripting;
namespace OpenRCT2::PaintScripting
{

    PaintScriptEngine::PaintScriptEngine()
    {
    }

    void PaintScriptEngine::Initialize()
    {
        // get all the paint object from the repository and load them
        // TO-DO : when ride type objects become a thing, load the paint objects at the same time as the ride type objects that
        // need them
        // auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
        auto& repoManager = OpenRCT2::GetContext()->GetObjectRepository();

        // we need to store a pointer reference of the paint object in the RTD
        // this is to minimize the search operations at runtime
        // since this is needed, the whole const constexpr for RideTypeDescriptors needs to be removed for now...
        for (ObjectEntryIndex i = 0; i < std::size(RideTypeDescriptors); i++)
        {
            auto& rtd = GetRideTypeDescriptor(i);

            if (rtd.PaintObjectId != nullptr)
            {
                auto repoItem = repoManager.FindObject(rtd.PaintObjectId);
                auto test = repoManager.LoadObject(repoItem);
                repoManager.RegisterLoadedObject(repoItem, std::move(test));

                rtd.PaintObjectPtr = static_cast<PaintObject*>(repoItem->LoadedObject.get());
                repoItem->LoadedObject->Load();
            }
        }

        _lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::bit32);
        PsPaintSession::Register(_lua);
        PsImageId::Register(_lua);
        PsPaint::Register(_lua);
        PsGlobalFunctions::Register(_lua);

        _lua["Paint"] = _paint;
    }

    void PaintScriptEngine::CallScript(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height,
        const TrackElement& trackElement, PaintObject* paintObject)
    {
        // for now, use a mutex since the PaintObject is static
        std::lock_guard lockGuard(_mutex);

        // if there was an error, don't run the script again
        if (!paintObject->Error.empty())
            return;

        _lua["Paint"] = PsPaint(session, trackSequence, direction, height);

        auto& scriptContent = paintObject->GetScript();
        auto result = _lua.safe_script(scriptContent, sol::script_pass_on_error);
        if (!result.valid())
        {
            sol::error err = result;
            Console::Error::WriteLine("Lua script crashes for paint object %s : %s", paintObject->GetName().c_str(), err.what());
            paintObject->Error = err.what();
        }
    }

} // namespace OpenRCT2::PaintScripting
