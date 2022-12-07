#pragma once

#include "../drawing/Image.h"

struct PaintSession;
struct CoordsXYZ;
struct BoundBoxXYZ;

class PaintHandler
{
public:
    PaintHandler() = default;
    virtual ~PaintHandler(){};

    virtual void OnPaintAddImageAsParent(
        PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox){};
    virtual void OnPaintAddImageAsOrphan(
        PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox){};
    virtual void OnPaintAddImageAsChild(
        PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox){};
    virtual void OnPaintAddImageAsChildRotated(
        PaintSession& session, const uint8_t direction, const ImageId image_id, const CoordsXYZ& offset,
        const CoordsXYZ& boundBoxSize, const CoordsXYZ& boundBoxOffset){};
    virtual void OnPaintAddImageAsParentRotated(
        PaintSession& session, const uint8_t direction, const ImageId image_id, const CoordsXYZ& offset,
        const CoordsXYZ& boundBoxSize){};
    virtual void OnPaintAddImageAsParentRotated(
        PaintSession& session, const uint8_t direction, const ImageId imageId, const CoordsXYZ& offset,
        const CoordsXYZ& boundBoxSize, const CoordsXYZ& boundBoxOffset){};
};
