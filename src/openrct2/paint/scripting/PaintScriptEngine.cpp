#include "PaintScriptEngine.h"

#include "../../Context.h"
#include "../../object/ObjectList.h"
#include "../../object/ObjectManager.h"
#include "../../object/ObjectRepository.h"
#include "../../object/PaintObject.h"
#include "../../ride/Ride.h"
#include "../../ride/RideData.h"
#include "bindings/PsPaintObject.h"
#include "bindings/PsImageId.h"
#include "bindings/PsPaintSession.h"
#include "bindings/PsGlobalFunctions.h"

using namespace OpenRCT2::Scripting;
namespace OpenRCT2::PaintScripting
{
    PaintScriptEngine::PaintScriptEngine(): _scriptsCount(0)
    {
    }

    uint32_t PaintScriptEngine::AddScript(const std::string& scriptContent)
    {
        _scriptsCount++;
        duk_push_string(_context, scriptContent.c_str());
        auto index = duk_get_top_index(_context);
        return index;
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

        //load the paintObject functions and singleton
        PsPaintObject::Register(_context);

        //load the image id constructors
        PsImageId::Register(_context);

        // register the global functions
        PsGlobalFunctions::Register(_context);
    }

    void PaintScriptEngine::CallScript(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, Direction direction, int32_t height, const TrackElement& trackElement,
        PaintObject* paintObject)
    {
        // if there was an error, don't run the script again
        if (!paintObject->Error.empty())
            return;

        //for now, use a mutex since the PaintObject is static
        std::lock_guard lockGuard(_mutex);

        //update the static values of the Paint objects
        PsPaintObject::Update(session, ride, trackSequence, direction, height, trackElement);

        

        // run the script
#pragma warning(disable : 4189)
        auto scriptString = duk_get_string(_context, paintObject->GetScriptStackIndex());
        auto rc = duk_peval_string(_context, scriptString);
        if (rc == DUK_EXEC_ERROR)
        {
            if (duk_is_error(_context, -1))
            {
                duk_get_prop_string(_context, -1, "stack");
                paintObject->Error = duk_safe_to_string(_context, -1);
                Console::Error::WriteLine(
                    "Error with paint object %s : %s", paintObject->GetIdentifier().data(), paintObject->Error.c_str());
            }
        }

        // pop the error/result
        duk_pop(_context);
    }

} // namespace OpenRCT2::PaintScripting
