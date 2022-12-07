#pragma once

#include "TestData.h"

#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/paint/Paint.h>
#include <openrct2/paint/PaintHandler.h>
#include <openrct2/ride/Ride.h>

namespace PaintSessionTestData
{
    enum class DataType
    {
        AddImageAsParent,
        AddImageAsOrphan,
        AddImageAsChild,
        AddImageAsChildRotated,
        AddImageAsParentRotated
    };

    struct Data
    {
        DataType Type;
        uint32_t Id;
        CoordsXYZ Offset;
        BoundBoxXYZ BoundBox;
        uint8_t Direction;

        Data() = default;
        Data(
            const DataType& type, const uint32_t id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox,
            uint8_t direction = 0)
            : Type(type)
            , Id(id)
            , Offset(offset)
            , BoundBox(boundBox)
            , Direction(direction)
        {
        }
    };
} // namespace PaintSessionTestData

class PaintSessionTest : public testing::Test, public PaintHandler
{
public:
    PaintSessionTest() = default;
    virtual ~PaintSessionTest(){};
    void OnPaintAddImageAsParent(
        PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox) override;
    void OnPaintAddImageAsOrphan(
        PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox) override;
    void OnPaintAddImageAsChild(
        PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox) override;
    void OnPaintAddImageAsChildRotated(
        PaintSession& session, const uint8_t direction, const ImageId image_id, const CoordsXYZ& offset,
        const CoordsXYZ& boundBoxSize, const CoordsXYZ& boundBoxOffset) override;
    void OnPaintAddImageAsParentRotated(
        PaintSession& session, const uint8_t direction, const ImageId image_id, const CoordsXYZ& offset,
        const CoordsXYZ& boundBoxSize) override;
    void OnPaintAddImageAsParentRotated(
        PaintSession& session, const uint8_t direction, const ImageId imageId, const CoordsXYZ& offset,
        const CoordsXYZ& boundBoxSize, const CoordsXYZ& boundBoxOffset) override;

    // json writing and reading functions
    void DumpJson(const std::string& filepath) const;
    std::vector<PaintSessionTestData::Data> LoadFromJson(const std::string& filename) const;

    // test data comparison function
    void CompareWith(const std::string& filename) const;

    static std::string PaintSessionTestDataPath()
    {
        return Path::Combine(TestData::GetBasePath(), u8"paint_session");
    }

protected:
    void SetUp() override;
    PaintSession _paintSession;
    Ride _ride;
    std::unique_ptr<OpenRCT2::IContext> _context;

private:
    std::vector<PaintSessionTestData::Data> _data;
};
