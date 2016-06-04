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
	uint8	lightIntensity;
	uint32	lightID;
	uint16	lightIDqualifier;
	uint8	lightLinger;
	uint8	pad[1];
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

	return (uint8)(min(255.0, light * 255.0)) >> 4;
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

extern void sub_68862C();
extern void viewport_paint_setup();

void lightfx_prepare_light_list()
{
	for (uint32 light = 0; light < LightListCurrentCountFront; light++) {
		lightlist_entry *entry = &_LightListFront[light];

		if (entry->z == 0x7FFF) {
			entry->lightIntensity = 0xFF;
			continue;
		}

		rct_xyz16 coord_3d = {
			.x = entry->x,
			.y = entry->y,
			.z = entry->z
		};

		rct_xy16 coord_2d = coordinate_3d_to_2d(&coord_3d, _current_view_rotation_front);

		entry->x = coord_2d.x;// - (_current_view_x_front);
		entry->y = coord_2d.y;// - (_current_view_y_front);

		sint32 posOnScreenX = entry->x - _current_view_x_front;
		sint32 posOnScreenY = entry->y - _current_view_y_front;

		posOnScreenX >>= _current_view_zoom_front;
		posOnScreenY >>= _current_view_zoom_front;

		if ((posOnScreenX < -128) ||
			(posOnScreenY < -128) ||
			(posOnScreenX > _pixelInfo.width + 128) ||
			(posOnScreenY > _pixelInfo.height + 128)) {
			entry->lightType = LIGHTFX_LIGHT_TYPE_NONE;
			continue;
		}
			

	//	entry->x >>= _current_view_zoom_front;
	//	entry->y >>= _current_view_zoom_front;

		static sint16 offsetPattern[26]	= {		0, 0,		-4, 0,		0, -3,		4, 0,		0, 3,
															-2, -1,		-1, -1,		2, 1,		1, 1,
															-3, -2,		-3, 2,		3, -2,		3, 2	};

		int mapFrontDiv = 1 << _current_view_zoom_front;

		uint32 lightIntensityOccluded = 0x0;

		sint32 dirVecX = 707;
		sint32 dirVecY = 707;

		sint32 tileOffsetX = 0;
		sint32 tileOffsetY = 0;

		switch (_current_view_rotation_front) {
		case 0:
			dirVecX	= 707;
			dirVecY = 707;
			tileOffsetX = 0;
			tileOffsetY = 0;
			break;
		case 1:
			dirVecX = -707;
			dirVecY = 707;
			tileOffsetX = 16;
			tileOffsetY = 0;
			break;
		case 2:
			dirVecX = -707;
			dirVecY = -707;
			tileOffsetX = 32;
			tileOffsetY = 32;
			break;
		case 3:
			dirVecX = 707;
			dirVecY = -707;
			tileOffsetX = 0;
			tileOffsetY = 16;
			break;
		default:
			dirVecX = 0;
			dirVecY = 0;
			break;
		}

		if (true) {
			int totalSamplePoints = 5;
			int startSamplePoint = 1;
			int lastSampleCount = 0;

			if ((entry->lightIDqualifier & 0xF) == LIGHTFX_LIGHT_QUALIFIER_MAP) {
				startSamplePoint = 0;
				totalSamplePoints = 1;
			}

			for (int pat = startSamplePoint; pat < totalSamplePoints; pat++) {
				rct_xy16 mapCoord = { 0 };

				rct_map_element *mapElement = 0;

				int interactionType;

				rct_window *w = window_get_main();
				if (w != NULL) {
				//	get_map_coordinates_from_pos(entry->x + offsetPattern[pat*2] / mapFrontDiv, entry->y + offsetPattern[pat*2+1] / mapFrontDiv, VIEWPORT_INTERACTION_MASK_NONE, &mapCoord.x, &mapCoord.y, &interactionType, &mapElement, NULL);

					RCT2_GLOBAL(0x9AC154, uint16_t) = ~VIEWPORT_INTERACTION_MASK_SPRITE & 0xFFFF;
					RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_ZOOM, uint16_t) = _current_view_zoom_front;
					RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_X, int16_t) = entry->x + offsetPattern[0 + pat * 2] / mapFrontDiv;
					RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_Y, int16_t) = entry->y + offsetPattern[1 + pat * 2] / mapFrontDiv;
					rct_drawpixelinfo* dpi = RCT2_ADDRESS(RCT2_ADDRESS_VIEWPORT_DPI, rct_drawpixelinfo);
					dpi->x = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_X, int16_t);
					dpi->y = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_Y, int16_t);
					dpi->zoom_level = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_ZOOM, uint16_t);
					dpi->height = 1;
					dpi->width = 1;
					RCT2_GLOBAL(0xEE7880, uint32_t) = 0xF1A4CC;
					RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*) = dpi;
					painter_setup();
					viewport_paint_setup();
					sub_688217();
					sub_68862C();

				//	log_warning("[%i, %i]", dpi->x, dpi->y);

					mapCoord.x = RCT2_GLOBAL(0x9AC14C, int16_t) + tileOffsetX;
					mapCoord.y = RCT2_GLOBAL(0x9AC14E, int16_t) + tileOffsetY;
					interactionType = RCT2_GLOBAL(0x9AC148, uint8_t);
					mapElement = RCT2_GLOBAL(0x9AC150, rct_map_element*);

					//RCT2_GLOBAL(0x9AC154, uint16_t) = VIEWPORT_INTERACTION_MASK_NONE;
					//RCT2_GLOBAL(0x9AC148, uint8_t) = 0;
					//RCT2_GLOBAL(0x9AC138 + 4, int16_t) = screenX;
					//RCT2_GLOBAL(0x9AC138 + 6, int16_t) = screenY;
					//if (screenX >= 0 && screenX < (int)myviewport->width && screenY >= 0 && screenY < (int)myviewport->height)
					//{
					//	screenX <<= myviewport->zoom;
					//	screenY <<= myviewport->zoom;
					//	screenX += (int)myviewport->view_x;
					//	screenY += (int)myviewport->view_y;
					//	RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_ZOOM, uint16_t) = myviewport->zoom;
					//	screenX &= (0xFFFF << myviewport->zoom) & 0xFFFF;
					//	screenY &= (0xFFFF << myviewport->zoom) & 0xFFFF;
					//	RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_X, int16_t) = screenX;
					//	RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_Y, int16_t) = screenY;
					//	rct_drawpixelinfo* dpi = RCT2_ADDRESS(RCT2_ADDRESS_VIEWPORT_DPI, rct_drawpixelinfo);
					//	dpi->y = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_Y, int16_t);
					//	dpi->height = 1;
					//	dpi->zoom_level = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_ZOOM, uint16_t);
					//	dpi->x = RCT2_GLOBAL(RCT2_ADDRESS_VIEWPORT_PAINT_X, int16_t);
					//	dpi->width = 1;
					//	RCT2_GLOBAL(0xEE7880, uint32_t) = 0xF1A4CC;
					//	RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*) = dpi;
					//	painter_setup();
					//	viewport_paint_setup();
					//	sub_688217();
					//	sub_68862C();
					//}
				}	

				sint32 minDist = 0;
				sint32 baseHeight = -999;

				if (interactionType != VIEWPORT_INTERACTION_ITEM_SPRITE && mapElement) {
					baseHeight = mapElement->base_height;
				}

				minDist = ((baseHeight * 8) - coord_3d.z) / 2;

				sint32 deltaX = mapCoord.x - coord_3d.x;
				sint32 deltaY = mapCoord.y - coord_3d.y;

				sint32 projDot = (dirVecX * deltaX + dirVecY * deltaY) / 1000;

				projDot = max(minDist, projDot);

				if (projDot < 5) {
					lightIntensityOccluded	+= 100;
				}
				else {
					lightIntensityOccluded	+= max(0, 200 - (projDot * 20));
				}

			//	log_warning("light %i [%i, %i, %i], [%i, %i] minDist to %i: %i; projdot: %i", light, coord_3d.x, coord_3d.y, coord_3d.z, mapCoord.x, mapCoord.y, baseHeight, minDist, projDot);


				if (pat == 0) {
					if (lightIntensityOccluded == 100)
						break;
					if (_current_view_zoom_front > 2)
						break;
					totalSamplePoints += 4;
				}
				else if (pat == 4) {
					if (_current_view_zoom_front > 1)
						break;
					if (lightIntensityOccluded == 0 || lightIntensityOccluded == 500)
						break;
					lastSampleCount = lightIntensityOccluded / 500;
				//	break;
					totalSamplePoints += 4;
				}
				else if (pat == 8) {
					break;
					if (_current_view_zoom_front > 0)
						break;
					int newSampleCount = lightIntensityOccluded / 900;
					if (abs(newSampleCount - lastSampleCount) < 10)
						break;
					totalSamplePoints += 4;
				}
			}

			totalSamplePoints -= startSamplePoint;

		//	lightIntensityOccluded = totalSamplePoints * 100;
		
		//	log_warning("sample-count: %i, occlusion: %i", totalSamplePoints, lightIntensityOccluded);

			if (lightIntensityOccluded == 0) {
				entry->lightType = LIGHTFX_LIGHT_TYPE_NONE;
				continue;
			}
			
		//	log_warning("sample-count: %i, occlusion: %i", totalSamplePoints, lightIntensityOccluded / totalSamplePoints);

			entry->lightIntensity = min(0xFF, (entry->lightIntensity * lightIntensityOccluded) / (totalSamplePoints * 100));
			entry->lightIntensity = max(0x00, entry->lightIntensity - _current_view_zoom_front * 5);
		}

		if (_current_view_zoom_front > 0) {
			if ((entry->lightType & 0x3) < _current_view_zoom_front) {
				entry->lightType = LIGHTFX_LIGHT_TYPE_NONE;
				continue;
			}

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

		sint32		inRectCentreX	= entry->x;
		sint32		inRectCentreY	= entry->y;

		if (entry->z != 0x7FFF) {
			inRectCentreX -= _current_view_x_front;
			inRectCentreY -= _current_view_y_front;
			inRectCentreX >>= _current_view_zoom_front;
			inRectCentreY >>= _current_view_zoom_front;
		}

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

		bufWriteX	= inRectCentreX - bufReadWidth / 2;
		bufWriteY	= inRectCentreY - bufReadHeight / 2;

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

		if (entry->lightIntensity == 0xFF) {
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
		else {
			for (int y = 0; y < bufWriteHeight; y++) {
				for (int x = 0; x < bufWriteWidth; x++) {
					*bufWriteBase = min(0xFF, *bufWriteBase + (((*bufReadBase) * (1 + entry->lightIntensity)) >> 8));
					bufWriteBase++;
					bufReadBase++;
				}

				bufWriteBase += bufWriteSkip;
				bufReadBase += bufReadSkip;
			}
		}
	}
}

void* lightfx_get_front_buffer()
{
	return _light_rendered_buffer_front;
}

void lightfx_add_3d_light(uint32 lightID, uint16 lightIDqualifier, sint16 x, sint16 y, uint16 z, uint8 lightType)
{
	if (LightListCurrentCountBack == 15999) {
		return;
	}

//	log_warning("%i lights in back", LightListCurrentCountBack);

	for (uint32 i = 0; i < LightListCurrentCountBack; i++) {
		lightlist_entry *entry = &_LightListBack[i];
		if (entry->lightID != lightID)
			continue;
		if (entry->lightIDqualifier != lightIDqualifier)
			continue;

		entry->x = x;
		entry->y = y;
		entry->z = z;
		entry->lightType = lightType;
		entry->lightIntensity = 0xFF;
		entry->lightID = lightID;
		entry->lightIDqualifier = lightIDqualifier;
		entry->lightLinger = 1;

		return;
	}

	lightlist_entry *entry = &_LightListBack[LightListCurrentCountBack++];

	entry->x				= x;
	entry->y				= y;
	entry->z				= z;
	entry->lightType		= lightType;
	entry->lightIntensity	= 0xFF;
	entry->lightID			= lightID;
	entry->lightIDqualifier = lightIDqualifier;
	entry->lightLinger		= 1;

//	log_warning("new 3d light");
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

	lightfx_add_3d_light((x << 16) | y, (offsetZ << 8) | LIGHTFX_LIGHT_QUALIFIER_MAP, x, y, offsetZ, lightType);
}

uint32 lightfx_get_light_polution()
{
	return _lightPolution_front;
}

void lightfx_add_lights_magic_vehicles()
{
	for (uint16 i = gSpriteListHead[SPRITE_LIST_VEHICLE]; i != SPRITE_INDEX_NULL; i = g_sprite_list[i].vehicle.next) {
		uint32	vehicleID = i;

		rct_vehicle *mother_vehicle = GET_VEHICLE(i);
		rct_vehicle *vehicle = mother_vehicle;

		rct_ride_entry *rideEntry = 0;
		const rct_ride_entry_vehicle *vehicleEntry;

		if (mother_vehicle->ride_subtype == 0xFF) {
			continue;
		}
		else {
			rideEntry = get_ride_entry(mother_vehicle->ride_subtype);
			vehicleEntry = &rideEntry->vehicles[mother_vehicle->vehicle_type];
		}

		for (uint16 q = i; q != SPRITE_INDEX_NULL; ) {
			rct_vehicle *vehicle = GET_VEHICLE(q);

			vehicleID = q;
			if (vehicle->next_vehicle_on_train == q)
				break;
			q = vehicle->next_vehicle_on_train;

			sint16 place_x, place_y, place_z;

			place_x = vehicle->x;
			place_y = vehicle->y;
			place_z = vehicle->z;

			static const sint16 offsetLookup[32] = { 10, 10, 9, 8, 7, 6, 4, 2, 0, -2, -4, -6, -7, -8, -9, -10, -10, -10, -9, -8, -7, -6, -4, -2, 0, 2, 4, 6, 7, 8, 9, 10 };

			rct_ride *ride = get_ride(vehicle->ride);
			switch (ride->type) {
			case RIDE_TYPE_OBSERVATION_TOWER:
				lightfx_add_3d_light(vehicleID, 0x0000 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, vehicle->x, vehicle->y + 16, vehicle->z, LIGHTFX_LIGHT_TYPE_SPOT_3);
				lightfx_add_3d_light(vehicleID, 0x0100 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, vehicle->x + 16, vehicle->y, vehicle->z, LIGHTFX_LIGHT_TYPE_SPOT_3);
				lightfx_add_3d_light(vehicleID, 0x0200 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, vehicle->x - 16, vehicle->y, vehicle->z, LIGHTFX_LIGHT_TYPE_SPOT_3);
				lightfx_add_3d_light(vehicleID, 0x0300 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, vehicle->x, vehicle->y - 16, vehicle->z, LIGHTFX_LIGHT_TYPE_SPOT_3);
				break;
			case RIDE_TYPE_MINE_TRAIN_COASTER:
			case RIDE_TYPE_GHOST_TRAIN:
				if (vehicle == vehicle_get_head(vehicle)) {
					place_x -= offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
					place_y -= offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
					lightfx_add_3d_light(vehicleID, 0x0000 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z, LIGHTFX_LIGHT_TYPE_SPOT_3);
				}
				break;
			case RIDE_TYPE_CHAIRLIFT:
				lightfx_add_3d_light(vehicleID, 0x0000 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z - 16, LIGHTFX_LIGHT_TYPE_LANTERN_2);
				break;
			case RIDE_TYPE_BOAT_RIDE:
			case RIDE_TYPE_CAR_RIDE:
			case RIDE_TYPE_GO_KARTS:
			case RIDE_TYPE_DODGEMS:
			case RIDE_TYPE_MINI_HELICOPTERS:
			case RIDE_TYPE_MONORAIL_CYCLES:
			case RIDE_TYPE_SUBMARINE_RIDE:
			case RIDE_TYPE_SPLASH_BOATS:
			case RIDE_TYPE_WATER_COASTER: {
				rct_vehicle *vehicle_draw = vehicle_get_head(vehicle);
				if (vehicle_draw->next_vehicle_on_train != 0xFFFF) {
					vehicle_draw = GET_VEHICLE(vehicle_draw->next_vehicle_on_train);
				}
				place_x = vehicle_draw->x;
				place_y = vehicle_draw->y;
				place_z = vehicle_draw->z;
				place_x -= offsetLookup[(vehicle_draw->sprite_direction + 0) % 32];
				place_y -= offsetLookup[(vehicle_draw->sprite_direction + 8) % 32];
				lightfx_add_3d_light(vehicleID, 0x0000 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z, LIGHTFX_LIGHT_TYPE_SPOT_2);
				place_x -= offsetLookup[(vehicle_draw->sprite_direction + 0) % 32];
				place_y -= offsetLookup[(vehicle_draw->sprite_direction + 8) % 32];
				lightfx_add_3d_light(vehicleID, 0x0100 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z, LIGHTFX_LIGHT_TYPE_SPOT_2);
				break;
			}
			case RIDE_TYPE_MONORAIL:
				lightfx_add_3d_light(vehicleID, 0x0000 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, vehicle->x, vehicle->y, vehicle->z + 12, LIGHTFX_LIGHT_TYPE_SPOT_2);
				if (vehicle == vehicle_get_head(vehicle)) {
					place_x -= offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
					place_y -= offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
					lightfx_add_3d_light(vehicleID, 0x0100 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 10, LIGHTFX_LIGHT_TYPE_LANTERN_3);
					place_x -= offsetLookup[(vehicle->sprite_direction + 0) % 32] * 3;
					place_y -= offsetLookup[(vehicle->sprite_direction + 8) % 32] * 3;
					lightfx_add_3d_light(vehicleID, 0x0200 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 2, LIGHTFX_LIGHT_TYPE_LANTERN_3);
				}
				if (vehicle == vehicle_get_tail(vehicle)) {
					place_x += offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
					place_y += offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
					lightfx_add_3d_light(vehicleID, 0x0300 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 10, LIGHTFX_LIGHT_TYPE_LANTERN_3);
					place_x += offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
					place_y += offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
					lightfx_add_3d_light(vehicleID, 0x0400 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 2, LIGHTFX_LIGHT_TYPE_LANTERN_3);
				}
				break;
			case RIDE_TYPE_MINIATURE_RAILWAY:
				if (vehicle == vehicle_get_head(vehicle)) {
					place_x -= offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
					place_y -= offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
					lightfx_add_3d_light(vehicleID, 0x0100 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 10, LIGHTFX_LIGHT_TYPE_LANTERN_3);
					place_x -= offsetLookup[(vehicle->sprite_direction + 0) % 32] * 2;
					place_y -= offsetLookup[(vehicle->sprite_direction + 8) % 32] * 2;
					lightfx_add_3d_light(vehicleID, 0x0200 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 2, LIGHTFX_LIGHT_TYPE_LANTERN_3);

				}
				else {
					lightfx_add_3d_light(vehicleID, 0x0000 | LIGHTFX_LIGHT_QUALIFIER_SPRITE, place_x, place_y, place_z + 10, LIGHTFX_LIGHT_TYPE_LANTERN_3);
				}
				break;
			default:
				break;
			};
		}
	}
}