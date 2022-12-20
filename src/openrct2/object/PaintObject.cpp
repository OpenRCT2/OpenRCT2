#include "PaintObject.h"

#include "../Context.h"
#include "../core/Json.hpp"
#include "../paint/scripting/PaintScriptEngine.h"
#include "../ride/Ride.h"
#include "../scripting/Duktape.hpp"
#include "../world/TileElement.h"
#include "../world/Location.hpp"
#include "../object/ObjectManager.h"

PaintObject::PaintObject()
{
}


void PaintObject::Paint(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
}

void PaintObject::Load()
{
}

void PaintObject::Unload()
{
}

void PaintObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    auto properties = root["properties"];
    if (properties.is_object())
    {
        // load the script file
        auto scriptFile = properties["scriptFile"];
        if (scriptFile.is_string())
        {
            auto scriptBytes = context->GetData(scriptFile);
            u8string result(scriptBytes.size(), 0);
            std::copy(scriptBytes.begin(), scriptBytes.end(), result.begin());

            _scriptContent = result;
        }
        //_scriptContent = scriptFile;
    }
    PopulateTablesFromJson(context, root);

    // always load the paint objects at the start of the game for now
    //auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
    //objManager.LoadObject(GetIdentifier());
}
