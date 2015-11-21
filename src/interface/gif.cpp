
extern "C" {

#include "../addresses.h"
#include "../common.h"
#include "../drawing/drawing.h"
#include "../platform/platform.h"
#include "screenshot.h"

#include "../gif.h"

bool gGifRecordingActive = false;

static GifWriter _gifWriter;
static int _gifWidth;
static int _gifHeight;
static int _gifDelay;
static uint8 *_rgbFrameData;

void screenshot_gif_begin()
{
	utf8 outPath[MAX_PATH];
	platform_get_user_directory(outPath, "screenshot");
	strcat(outPath, "test.gif");

	_gifWidth = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16);
	_gifHeight = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16);
	_gifDelay = 2;
	if (!GifBegin(&_gifWriter, outPath, _gifWidth, _gifHeight, _gifDelay)) {
		log_error("Unable to begin GIF recording.");
		return;
	}

	gGifRecordingActive = true;
	_rgbFrameData = (uint8*)malloc(_gifWidth * _gifHeight * 4);
}

void screenshot_gif_update()
{
	if (!gGifRecordingActive) {
		return;
	}

	uint8 *palette = RCT2_ADDRESS(RCT2_ADDRESS_PALETTE, uint8);
	rct_drawpixelinfo *dpi = RCT2_ADDRESS(RCT2_ADDRESS_SCREEN_DPI, rct_drawpixelinfo);

	int width = min(_gifWidth, dpi->width);
	int height = min(_gifHeight, dpi->height);
	uint8 *src = dpi->bits;
	uint8 *dst = _rgbFrameData;

	memset(dst, 0, _gifWidth * _gifHeight * 4);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			uint8 colour = *src++;
			uint8 *paletteEntry = &palette[colour * 4];
			*dst++ = paletteEntry[2];
			*dst++ = paletteEntry[1];
			*dst++ = paletteEntry[0];
			*dst++ = 255;
		}
		src += dpi->pitch;
	}

	if (!GifWriteFrame(&_gifWriter, _rgbFrameData, _gifWidth, _gifHeight, _gifDelay)) {
		log_error("Unable to write GIF frame.");
		return;
	}
}

void screenshot_gif_end()
{
	gGifRecordingActive = false;

	GifEnd(&_gifWriter);
	SafeFree(_rgbFrameData);
}

}