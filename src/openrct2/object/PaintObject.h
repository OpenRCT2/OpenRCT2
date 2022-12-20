#pragma once

#include "Object.h"

#include <memory>
#include <vector>
#include <optional>

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

    const std::string GetScript() const
    {
        return _scriptContent;
    }

    //this is set when it encounters an error
    std::string Error;
private:
    std::string _scriptContent;
};
