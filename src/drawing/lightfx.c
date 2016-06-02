#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#include "lightfx.h"
#include "drawing.h"
#include "../rct2.h"
#include "../interface/window.h"
#include "../interface/viewport.h"

static uint8 _bakedLightTexture_lantern_0[32*32];
static uint8 _bakedLightTexture_lantern_1[64*64];
static uint8 _bakedLightTexture_lantern_2[128*128];
static uint8 _bakedLightTexture_lantern_3[256*256];
static uint8 _bakedLightTexture_spot_0[32 * 32];
static uint8 _bakedLightTexture_spot_1[64 * 64];
static uint8 _bakedLightTexture_spot_2[128 * 128];
static uint8 _bakedLightTexture_spot_3[256 * 256];
static rct_drawpixelinfo	_pixelInfo;

static void* _light_rendered_buffer_back = 0;
static void* _light_rendered_buffer_front = 0;

static uint32	_lightPolution_back = 0;
static uint32	_lightPolution_front = 0;

typedef struct lightlist_entry {
	sint16	x, y, z;
	uint8	lightType;
	uint8	pad[3];
} lightlist_entry;

static lightlist_entry	_LightListA[16000];
static lightlist_entry	_LightListB[16000];

static lightlist_entry	*_LightListBack;
static lightlist_entry	*_LightListFront;

static uint32			LightListCurrentCountBack;
static uint32			LightListCurrentCountFront;

static sint16			_current_view_x_front			= 0;
static sint16			_current_view_y_front			= 0;
static uint8			_current_view_rotation_front	= 0;
static uint8			_current_view_zoom_front		= 0;
static sint16			_current_view_x_back			= 0;
static sint16			_current_view_y_back			= 0;
static uint8			_current_view_rotation_back		= 0;
static uint8			_current_view_zoom_back			= 0;
static uint8			_current_view_zoom_back_delay	= 0;

uint8 calc_light_intensity_lantern(sint32 x, sint32 y) {
	double distance = (double)(x * x + y * y);

	double light = 0.03 + pow(10.0 / (1.0 + distance / 100.0), 0.55);
	light *= min(1.0, max(0.0f, 2.0 - sqrt(distance) / 64));
	light *= 0.1f;

	return (uint8)(min(255.0, light * 255.0));
}

uint8 calc_light_intensity_spot(sint32 x, sint32 y) {
	double distance = (double)(x * x + y * y);

	double light = 0.3 + pow(10.0 / (1.0 + distance / 100.0), 0.75);
	light *= min(1.0, max(0.0f, 2.0 - sqrt(distance) / 64));
	light *= 0.5f;

	return (uint8)(min(255.0, light * 255.0)) >> 3;
}

void calc_rescale_light_half( uint8 *target, uint8 *source,uint32 targetWidth, uint32 targetHeight) {
	uint8 *parcerRead = source;
	uint8 *parcerWrite = target;

	for (uint32 y = 0; y < targetHeight; y++) {
		for (uint32 x = 0; x < targetWidth; x++) {
			*parcerWrite = (*parcerRead);
			parcerWrite++;
			parcerRead += 2;
		}
		parcerRead += targetWidth * 2;
	}
}

void lightfx_init()
{
	_LightListBack		= _LightListA;
	_LightListFront		= _LightListB;

	memset(_bakedLightTexture_lantern_0, 0xFF, 32 * 32);
	memset(_bakedLightTexture_lantern_1, 0xFF, 64 * 64);
	memset(_bakedLightTexture_lantern_2, 0xFF, 128 * 128);
	memset(_bakedLightTexture_lantern_3, 0xFF, 256 * 256);

	uint8 *parcer = _bakedLightTexture_lantern_3;

	for (int y = 0; y < 256; y++) {
		for (int x = 0; x < 256; x++) {
			*parcer = calc_light_intensity_lantern(x - 128, y - 128);
			parcer++;
		}
	}

	parcer = _bakedLightTexture_spot_3;

	for (int y = 0; y < 256; y++) {
		for (int x = 0; x < 256; x++) {
			*parcer = calc_light_intensity_spot(x - 128, y - 128);
			parcer++;
		}
	}

	calc_rescale_light_half(_bakedLightTexture_lantern_2, _bakedLightTexture_lantern_3, 128, 128);
	calc_rescale_light_half(_bakedLightTexture_lantern_1, _bakedLightTexture_lantern_2, 64, 64);
	calc_rescale_light_half(_bakedLightTexture_lantern_0, _bakedLightTexture_lantern_1, 32, 32);

	calc_rescale_light_half(_bakedLightTexture_spot_2, _bakedLightTexture_spot_3, 128, 128);
	calc_rescale_light_half(_bakedLightTexture_spot_1, _bakedLightTexture_spot_2, 64, 64);
	calc_rescale_light_half(_bakedLightTexture_spot_0, _bakedLightTexture_spot_1, 32, 32);
}	

void lightfx_update_buffers(rct_drawpixelinfo *info)
{
	_light_rendered_buffer_front	= realloc(_light_rendered_buffer_front,	info->width * info->height);
	_light_rendered_buffer_back		= realloc(_light_rendered_buffer_back,	info->width * info->height);

	memcpy(&_pixelInfo, info, sizeof(rct_drawpixelinfo));
}

void lightfx_prepare_light_list()
{

	for (uint32 light = 0; light < LightListCurrentCountFront; light++) {
		lightlist_entry *entry = &_LightListFront[light];

		if (entry->z == 0x7FFF) {
			continue;
		}

		rct_xyz16 coord_3d = {
			.x = entry->x,
			.y = entry->y,
			.z = entry->z
		};

		rct_xy16 coord_2d = coordinate_3d_to_2d(&coord_3d, _current_view_rotation_front);

		entry->x = coord_2d.x - _current_view_x_front;
		entry->y = coord_2d.y - _current_view_y_front;

		if (_current_view_zoom_front > 0) {
			if ((entry->lightType & 0x3) < _current_view_zoom_front) {
				entry->lightType = LIGHTFX_LIGHT_TYPE_NONE;
				continue;
			}

			entry->x >>= _current_view_zoom_front;
			entry->y >>= _current_view_zoom_front;
			entry->lightType -= _current_view_zoom_front;
		}
	}
}

void lightfx_swap_buffers()
{
	void *tmp = _light_rendered_buffer_back;
	_light_rendered_buffer_back = _light_rendered_buffer_front;
	_light_rendered_buffer_front = tmp;

	tmp = _light_rendered_buffer_back;
	_light_rendered_buffer_back = _light_rendered_buffer_front;
	_light_rendered_buffer_front = tmp;

	tmp = _LightListBack;
	_LightListBack = _LightListFront;
	_LightListFront = tmp;

	LightListCurrentCountFront = LightListCurrentCountBack;
	LightListCurrentCountBack = 0x0;

	uint32 uTmp = _lightPolution_back;
	_lightPolution_back	= _lightPolution_front;
	_lightPolution_front = uTmp;

	_current_view_x_front = _current_view_x_back;
	_current_view_y_front = _current_view_y_back;
	_current_view_rotation_front = _current_view_rotation_back;
	_current_view_zoom_front = _current_view_zoom_back_delay;
	_current_view_zoom_back_delay = _current_view_zoom_back;
}

void lightfx_update_viewport_settings()
{
	rct_window *mainWindow = window_get_main();
	if (mainWindow) {
		_current_view_x_back = mainWindow->viewport->view_x;
		_current_view_y_back = mainWindow->viewport->view_y;
		_current_view_rotation_back = get_current_rotation();
		_current_view_zoom_back = mainWindow->viewport->zoom;
	}
}

void lightfx_render_lights_to_frontbuffer()
{
	memset(_light_rendered_buffer_front, 0, _pixelInfo.width * _pixelInfo.height);

	_lightPolution_back = 0;

//	log_warning("%i lights", LightListCurrentCountFront);

	for (uint32 light = 0; light < LightListCurrentCountFront; light++) {

		const uint8	*bufReadBase	= 0;
		uint8		*bufWriteBase	= _light_rendered_buffer_front;
		uint32		bufReadWidth, bufReadHeight;
		sint32		bufWriteX, bufWriteY;
		sint32		bufWriteWidth, bufWriteHeight;
		uint32		bufReadSkip, bufWriteSkip;

		lightlist_entry	* entry = &_LightListFront[light];

		switch (entry->lightType) {
			case LIGHTFX_LIGHT_TYPE_LANTERN_0:
				bufReadWidth	= 32;
				bufReadHeight	= 32;
				bufReadBase		= _bakedLightTexture_lantern_0;
				break;
			case LIGHTFX_LIGHT_TYPE_LANTERN_1:
				bufReadWidth	= 64;
				bufReadHeight	= 64;
				bufReadBase		= _bakedLightTexture_lantern_1;
				break;
			case LIGHTFX_LIGHT_TYPE_LANTERN_2:
				bufReadWidth	= 128;
				bufReadHeight	= 128;
				bufReadBase		= _bakedLightTexture_lantern_2;
				break;
			case LIGHTFX_LIGHT_TYPE_LANTERN_3:
				bufReadWidth	= 256;
				bufReadHeight	= 256;
				bufReadBase		= _bakedLightTexture_lantern_3;
				break;
			case LIGHTFX_LIGHT_TYPE_SPOT_0:
				bufReadWidth = 32;
				bufReadHeight = 32;
				bufReadBase = _bakedLightTexture_spot_0;
				break;
			case LIGHTFX_LIGHT_TYPE_SPOT_1:
				bufReadWidth = 64;
				bufReadHeight = 64;
				bufReadBase = _bakedLightTexture_spot_1;
				break;
			case LIGHTFX_LIGHT_TYPE_SPOT_2:
				bufReadWidth = 128;
				bufReadHeight = 128;
				bufReadBase = _bakedLightTexture_spot_2;
				break;
			case LIGHTFX_LIGHT_TYPE_SPOT_3:
				bufReadWidth = 256;
				bufReadHeight = 256;
				bufReadBase = _bakedLightTexture_spot_3;
				break;
			default:
				continue;
		}

		bufWriteX	= entry->x - bufReadWidth / 2;
		bufWriteY	= entry->y - bufReadHeight / 2;

		bufWriteWidth	= bufReadWidth;
		bufWriteHeight	= bufReadHeight;

		if (bufWriteX < 0) {
			bufReadBase		+= -bufWriteX;
			bufWriteWidth	+= bufWriteX;
		}
		else {
			bufWriteBase	+= bufWriteX;
		}

		if (bufWriteWidth <= 0)
			continue;

		if (bufWriteY < 0) {
			bufReadBase		+= -bufWriteY * bufReadWidth;
			bufWriteHeight	+= bufWriteY;
		}
		else {
			bufWriteBase	+= bufWriteY * _pixelInfo.width;
		}


		if (bufWriteHeight <= 0)
			continue;

		sint32	rightEdge = bufWriteX + bufWriteWidth;
		sint32	bottomEdge = bufWriteY + bufWriteHeight;

		if (rightEdge > _pixelInfo.width) {
			bufWriteWidth -= rightEdge - _pixelInfo.width;
		}
		if (bottomEdge > _pixelInfo.height) {
			bufWriteHeight -= bottomEdge - _pixelInfo.height;
		}

		if (bufWriteWidth <= 0)
			continue;
		if (bufWriteHeight <= 0)
			continue;

		_lightPolution_back += (bufWriteWidth * bufWriteHeight) / 256;

		bufReadSkip		= bufReadWidth - bufWriteWidth;
		bufWriteSkip	= _pixelInfo.width - bufWriteWidth;

		for (int y = 0; y < bufWriteHeight; y++) {
			for (int x = 0; x < bufWriteWidth; x++) {
				*bufWriteBase = min(0xFF, *bufWriteBase + *bufReadBase);
				bufWriteBase++;
				bufReadBase++;
			}

			bufWriteBase	+= bufWriteSkip;
			bufReadBase		+= bufReadSkip;
		}
	}
}

void* lightfx_get_front_buffer()
{
	return _light_rendered_buffer_front;
}

void lightfx_add_3d_light(sint16 x, sint16 y, uint16 z, uint8 lightType)
{
	if (LightListCurrentCountBack == 15999) {
		return;
	}

	for (uint32 i = 0; i < LightListCurrentCountBack; i++) {
		lightlist_entry *entry = &_LightListBack[i];
		if (entry->x != x)
			continue;
		if (entry->y != y)
			continue;
		if (entry->z != z)
			continue;
		if (entry->lightType != lightType)
			continue;

		return;
	}

	lightlist_entry *entry = &_LightListBack[LightListCurrentCountBack++];

	entry->x			= x;
	entry->y			= y;
	entry->z			= z;
	entry->lightType	= lightType;
}

void lightfx_add_3d_light_magic_from_drawing_tile(sint16 offsetX, sint16 offsetY, sint16 offsetZ, uint8 lightType)
{
	sint16	x = RCT2_GLOBAL(0x9DE56A, sint16) + offsetX;
	sint16	y = RCT2_GLOBAL(0x9DE56C, sint16) + offsetY;

	switch (get_current_rotation()) {
	case 0:
		x += 16;
		y += 16;
		break;
	case 1:
		x += 16;
		y += 16;
		break;
	case 2:
		x += 16;
		y -= 16;
		break;
	case 3:
		x += 16;
		y -= 16;
		break;
	default:
		return;
	}

	lightfx_add_3d_light(x, y, offsetZ, lightType);
}

uint32 lightfx_get_light_polution()
{
	return _lightPolution_front;
}