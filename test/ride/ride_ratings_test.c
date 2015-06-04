#include "../../src/ride/ride_ratings.h"
#include "../../src/localisation/localisation.h"

#include "ride_ratings_test.h"

void test_ride_ratings_setup(CuTest* tc) {
	test_load_scenario(tc, "Six Flags Magic Mountain.SC6");
}

void test_ride_ratings(CuTest* tc) {
	int i;
	rct_ride *ride;
	rating_tuple *rideRatings = malloc(256 * sizeof(rating_tuple));
	char rideName[256];
	char buffer[256];

	// Store original ride ratings
	FOR_ALL_RIDES(i, ride)
		rideRatings[i] = ride->ratings;

	// Update all the ride ratings
	// TODO use a better function that can just be called once for each ride
	for (int i = 0; i < MAX_RIDES * 10; i++)
		ride_ratings_update_all();

	FOR_ALL_RIDES(i, ride) {
		format_string(rideName, ride->name, &ride->name_arguments);

		sprintf(buffer, "[%s.excitement]", rideName);
		CuAssertIntEquals_Msg(tc, buffer, rideRatings[i].excitement, ride->excitement);
		sprintf(buffer, "[%s.intensity]", rideName);
		CuAssertIntEquals_Msg(tc, buffer, rideRatings[i].intensity, ride->intensity);
		sprintf(buffer, "[%s.nausea]", rideName);
		CuAssertIntEquals_Msg(tc, buffer, rideRatings[i].nausea, ride->nausea);
	}

	free(rideRatings);
}