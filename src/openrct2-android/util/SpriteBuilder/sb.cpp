#include "openrct2/CmdlineSprite.h"
#include "openrct2/Context.h"
#include "openrct2/drawing/Drawing.h"
#include "openrct2/object/Object.h"

bool gOpenRCT2Headless = true;
bool gOpenRCT2NoGraphics = false;
float gDayNightCycle = 0;
uint16_t gClimateLightningFlash = 0;

void mask_avx2(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap)
{
    mask_scalar(width, height, maskSrc, colourSrc, dst, maskWrap, colourWrap, dstWrap);
}
void mask_sse4_1(
    int32_t width, int32_t height, const uint8_t* RESTRICT maskSrc, const uint8_t* RESTRICT colourSrc, uint8_t* RESTRICT dst,
    int32_t maskWrap, int32_t colourWrap, int32_t dstWrap)
{
    mask_scalar(width, height, maskSrc, colourSrc, dst, maskWrap, colourWrap, dstWrap);
}

void* object_entry_get_chunk(ObjectType objectType, ObjectEntryIndex index)
{
    throw std::runtime_error("invalid call");
    return nullptr;
}
int32_t context_get_width()
{
    throw std::runtime_error("invalid call");
    return 0;
}
int32_t context_get_height()
{
    throw std::runtime_error("invalid call");
    return 0;
}
void platform_get_openrct2_data_path(utf8* outPath, size_t outSize)
{
    throw std::runtime_error("invalid call");
}
void FASTCALL gfx_sprite_to_buffer(rct_drawpixelinfo& dpi, const DrawSpriteArgs& args)
{
    throw std::runtime_error("invalid call");
}
const rct_g1_element* gfx_get_g1_element(ImageId image_id)
{
    throw std::runtime_error("invalid call");
    return nullptr;
}
const rct_g1_element* gfx_get_g1_element(ImageIndex image_idx)
{
    throw std::runtime_error("invalid call");
    return nullptr;
}
void context_set_fullscreen_mode(int32_t mode)
{
    throw std::runtime_error("invalid_call");
}
void context_recreate_window()
{
    throw std::runtime_error("invalid_call");
}
namespace OpenRCT2
{
    [[nodiscard]] IContext* GetContext()
    {
        throw std::runtime_error("invalid_call");
        return nullptr;
    }
} // namespace OpenRCT2

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Need at least one argument\n");
    }
    auto result = cmdline_for_sprite(&argv[1], argc - 1);
    if (result != 1)
    {
        fprintf(stderr, "Command failed\n");
        return 1;
    }
    return 0;
}
