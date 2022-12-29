#pragma once

#include "Object.h"

#include <memory>
#include <vector>
#include <optional>
#include <sol/sol.hpp>

struct Ride;
struct TrackElement;
class DukValue;

class PaintObject : public Object
{
public:
    PaintObject();
    //PaintObject(const PaintObject& other);

    void Paint(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement);

    void Load() override;
    void Unload() override;
    void ReadJson(IReadObjectContext* context, json_t& root) override;

    int32_t GetLoadedScriptIndex() const
    {
        return _loadedScriptIndex;
    }

    //this is set when it encounters an error
    std::string Error;
private:
    int32_t _loadedScriptIndex;
};
