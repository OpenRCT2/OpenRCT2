#ifndef _CLIMATE_H_
#define _CLIMATE_H_

#include "../common.h"

enum {
	CLIMATE_COOL_AND_WET,
	CLIMATE_WARM,
	CLIMATE_HOT_AND_DRY,
	CLIMATE_COLD
};

enum{
	WEATHER_SUNNY,
	WEATHER_PARTIALLY_CLOUDY,
	WEATHER_CLOUDY,
	WEATHER_RAIN,
	WEATHER_HEAVY_RAIN,
	WEATHER_THUNDER
};

typedef struct {
	sint8 temp_delta;
	sint8 effect_level;
	sint8 gloom_level;
	sint8 rain_level;
	uint32 sprite_id;
} rct_weather;

#define gClimate						RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE, uint8)
#define gClimateCurrentWeather			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER, uint8)
#define gClimateCurrentTemperature		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TEMPERATURE, sint8)
#define gClimateCurrentWeatherEffect	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_EFFECT, uint8)
#define gClimateCurrentWeatherGloom		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_WEATHER_GLOOM, uint8)
#define gClimateCurrentRainLevel		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RAIN_LEVEL, uint8)
#define gClimateNextWeather				RCT2_GLOBAL(RCT2_ADDRESS_NEXT_WEATHER, uint8)
#define gClimateNextTemperature			RCT2_GLOBAL(RCT2_ADDRESS_NEXT_TEMPERATURE, sint8)
#define gClimateNextWeatherEffect		RCT2_GLOBAL(RCT2_ADDRESS_NEXT_WEATHER_EFFECT, uint8)
#define gClimateNextWeatherGloom		RCT2_GLOBAL(RCT2_ADDRESS_NEXT_WEATHER_GLOOM, uint8)
#define gClimateNextRainLevel			RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RAIN_LEVEL, uint8)
#define gClimateUpdateTimer				RCT2_GLOBAL(RCT2_ADDRESS_CLIMATE_UPDATE_TIMER, uint16)

extern const rct_weather climate_weather_data[6];

extern uint16 gClimateLightningFlash;

int climate_celsius_to_fahrenheit(int celsius);
void climate_reset(int climate);
void climate_update();
void climate_update_sound();
void climate_force_weather(uint8 weather);

#endif
