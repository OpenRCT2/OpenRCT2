#include "PaintSessionTest.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <openrct2/OpenRCT2.h>
#include <openrct2/paint/Paint.SessionFlags.h>
#include <openrct2/platform/Platform.h>
#include <sstream>

using namespace PaintSessionTestData;
void PaintSessionTest::OnPaintAddImageAsParent(
    PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    auto data = Data{ DataType::AddImageAsParent, image_id.ToUInt32(), offset, boundBox };
    _data.push_back(data);
}

void PaintSessionTest::OnPaintAddImageAsOrphan(
    PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    auto data = Data{ DataType::AddImageAsOrphan, image_id.ToUInt32(), offset, boundBox };
    _data.push_back(data);
}

void PaintSessionTest::OnPaintAddImageAsChild(
    PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox)
{
    auto data = Data{ DataType::AddImageAsChild, image_id.ToUInt32(), offset, boundBox };
    _data.push_back(data);
}

void PaintSessionTest::OnPaintAddImageAsChildRotated(
    PaintSession& session, const uint8_t direction, const ImageId image_id, const CoordsXYZ& offset,
    const CoordsXYZ& boundBoxSize, const CoordsXYZ& boundBoxOffset)
{
    auto boundBox = BoundBoxXYZ{ boundBoxOffset, boundBoxSize };
    auto data = Data{ DataType::AddImageAsChildRotated, image_id.ToUInt32(), offset, boundBox };
    _data.push_back(data);
}

void PaintSessionTest::OnPaintAddImageAsParentRotated(
    PaintSession& session, const uint8_t direction, const ImageId image_id, const CoordsXYZ& offset,
    const CoordsXYZ& boundBoxSize)
{
    auto boundBox = BoundBoxXYZ{ offset, boundBoxSize };
    auto data = Data{ DataType::AddImageAsParentRotated, image_id.ToUInt32(), offset, boundBox, direction };
    _data.push_back(data);
}

void PaintSessionTest::OnPaintAddImageAsParentRotated(
    PaintSession& session, const uint8_t direction, const ImageId imageId, const CoordsXYZ& offset,
    const CoordsXYZ& boundBoxSize, const CoordsXYZ& boundBoxOffset)
{
    auto boundBox = BoundBoxXYZ{ offset, boundBoxSize };
    auto data = Data{ DataType::AddImageAsParentRotated, imageId.ToUInt32(), boundBoxOffset, boundBox, direction };
    _data.push_back(data);
}

void PaintSessionTest::DumpJson(const std::string& filepath) const
{
    json_t root;
    for (auto& elem : _data)
    {
        json_t json_elem;
        json_elem["type"] = elem.Type;
        json_elem["id"] = elem.Id;

        json_t json_offset;
        json_offset["x"] = elem.Offset.x;
        json_offset["y"] = elem.Offset.y;
        json_offset["z"] = elem.Offset.z;

        json_elem["offset"] = json_offset;

        json_t json_boundBox;
        json_boundBox["offset_x"] = elem.BoundBox.offset.x;
        json_boundBox["offset_y"] = elem.BoundBox.offset.y;
        json_boundBox["offset_z"] = elem.BoundBox.offset.z;
        json_boundBox["length_x"] = elem.BoundBox.length.x;
        json_boundBox["length_y"] = elem.BoundBox.length.y;
        json_boundBox["length_z"] = elem.BoundBox.length.z;

        json_elem["bound_box"] = json_boundBox;
        json_elem["direction"] = elem.Direction;
        root.push_back(json_elem);
    }
    auto stringContent = root.dump(4, ' ', true, nlohmann::detail::error_handler_t::strict);

    // write to the file
    auto path = Path::Combine(PaintSessionTest::PaintSessionTestDataPath(), filepath);
    std::ofstream file(path, std::ofstream::out);

    file << stringContent;
    file.close();
}

std::vector<PaintSessionTestData::Data> PaintSessionTest::LoadFromJson(const std::string& filename) const
{
    std::vector<PaintSessionTestData::Data> result;

    // load the file
    auto path = Path::Combine(PaintSessionTest::PaintSessionTestDataPath(), filename);
    std::ifstream file(path);

    // read all the lines and dump it into a string
    std::string jsonString;
    std::stringstream stringData;
    stringData << file.rdbuf();

    // parse the json
    auto jsonData = nlohmann::json::parse(stringData.str());

    // we expect an array of data
    for (auto& jsonElement : jsonData)
    {
        PaintSessionTestData::Data element;
        element.Type = static_cast<DataType>(jsonElement["type"]);
        element.Id = static_cast<uint32_t>(jsonElement["id"]);
        element.Direction = static_cast<uint8_t>(jsonElement["direction"]);

        auto offsetJson = jsonElement["offset"];
        element.Offset = CoordsXYZ{ static_cast<int32_t>(offsetJson["x"]), static_cast<int32_t>(offsetJson["y"]),
                                    static_cast<int32_t>(offsetJson["z"]) };

        auto boundBoxJson = jsonElement["bound_box"];
        element.BoundBox = BoundBoxXYZ{
            { static_cast<int32_t>(boundBoxJson["offset_x"]), static_cast<int32_t>(boundBoxJson["offset_y"]),
              static_cast<int32_t>(boundBoxJson["offset_z"]) },
            { static_cast<int32_t>(boundBoxJson["length_x"]), static_cast<int32_t>(boundBoxJson["length_y"]),
              static_cast<int32_t>(boundBoxJson["length_z"]) }
        };
        result.push_back(element);
    }
    return result;
}

void PaintSessionTest::CompareWith(const std::string& filename) const
{
    // assert that the length of the internal array is the same as the one we compare to
    auto dataToCompare = LoadFromJson(filename);
    ASSERT_EQ(_data.size(), dataToCompare.size());

    for (size_t i = 0; i < _data.size(); i++)
    {
        const auto& elem0 = _data[i];
        const auto& elem1 = dataToCompare[i];

        EXPECT_EQ(elem0.Id, elem1.Id);
        EXPECT_EQ(elem0.Direction, elem1.Direction);
        EXPECT_EQ(elem0.Type, elem1.Type);

        EXPECT_EQ(elem0.BoundBox.length.x, elem1.BoundBox.length.x);
        EXPECT_EQ(elem0.BoundBox.length.y, elem1.BoundBox.length.y);
        EXPECT_EQ(elem0.BoundBox.length.z, elem1.BoundBox.length.z);

        EXPECT_EQ(elem0.BoundBox.offset.x, elem1.BoundBox.offset.x);
        EXPECT_EQ(elem0.BoundBox.offset.y, elem1.BoundBox.offset.y);
        EXPECT_EQ(elem0.BoundBox.offset.z, elem1.BoundBox.offset.z);

        EXPECT_EQ(elem0.Offset.x, elem1.Offset.x);
        EXPECT_EQ(elem0.Offset.y, elem1.Offset.y);
        EXPECT_EQ(elem0.Offset.z, elem1.Offset.z);
    }
}

void PaintSessionTest::SetUp()
{
    // override the global paint handler pointer so that the paint functions call our functions
    gPaintHandler.reset(this);
    gOpenRCT2Headless = true;
    Platform::CoreInit();

    _context = OpenRCT2::CreateContext();
    _context->Initialise();
    _paintSession.Flags |= PaintSessionFlags::PassedSurface;
}
