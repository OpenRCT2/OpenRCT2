/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Entity.h"

#include "../core/DataSerialiser.h"
#include "../peep/Peep.h"
#include "../ride/Vehicle.h"
#include "Balloon.h"
#include "Duck.h"
#include "Fountain.h"
#include "MoneyEffect.h"
#include "Particle.h"

static void EntityBaseSerialise(EntityBase& base, DataSerialiser& stream)
{
    stream << base.Type;
    stream << base.sprite_index;
    stream << base.x;
    stream << base.y;
    stream << base.z;
    stream << base.sprite_direction;
}

void Litter::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << SubType;
    stream << creationTick;
}

void Balloon::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
    stream << popped;
    stream << time_to_move;
    stream << colour;
}

void Duck::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
    stream << target_x;
    stream << target_y;
    stream << state;
}

void MoneyEffect::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
    stream << MoveDelay;
    stream << NumMovements;
    stream << Vertical;
    stream << Value;
    stream << OffsetX;
    stream << Wiggle;
}

void VehicleCrashParticle::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
    stream << time_to_live;
    stream << colour;
    stream << crashed_sprite_base;
    stream << velocity_x;
    stream << velocity_y;
    stream << velocity_z;
    stream << acceleration_x;
    stream << acceleration_y;
    stream << acceleration_z;
}

void ExplosionFlare::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
}

void ExplosionCloud::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
}

void CrashSplashParticle::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
}

void SteamParticle::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
    stream << time_to_move;
}

void JumpingFountain::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << frame;
    stream << FountainType;
    stream << NumTicksAlive;
    stream << FountainFlags;
    stream << TargetX;
    stream << TargetY;
    stream << Iteration;
}

static void PeepBaseSerialise(Peep& base, DataSerialiser& stream)
{
    EntityBaseSerialise(base, stream);
    if (stream.IsLoading())
    {
        base.Name = nullptr;
    }
    stream << base.NextLoc;
    stream << base.NextFlags;
    stream << base.State;
    stream << base.SubState;
    stream << base.SpriteType;
    stream << base.TshirtColour;
    stream << base.TrousersColour;
    stream << base.DestinationX;
    stream << base.DestinationY;
    stream << base.DestinationTolerance;
    stream << base.Var37;
    stream << base.Energy;
    stream << base.EnergyTarget;
    stream << base.Mass;
    // stream << base.WindowInvalidateFlags;
    stream << base.CurrentRide;
    stream << base.CurrentRideStation;
    stream << base.CurrentTrain;
    stream << base.CurrentCar;
    stream << base.CurrentSeat;
    stream << base.SpecialSprite;
    stream << base.ActionSpriteType;
    stream << base.NextActionSpriteType;
    stream << base.ActionSpriteImageOffset;
    stream << base.Action;
    stream << base.ActionFrame;
    stream << base.StepProgress;
    stream << base.PeepDirection;
    stream << base.InteractionRideIndex;
    stream << base.Id;
    stream << base.PathCheckOptimisation;
    stream << base.PathfindGoal;
    stream << base.PathfindHistory;
    stream << base.WalkingFrameNum;
    stream << base.PeepFlags;
}

void Guest::Serialise(DataSerialiser& stream)
{
    PeepBaseSerialise(*this, stream);
    stream << GuestNumRides;
    stream << GuestNextInQueue;
    stream << ParkEntryTime;
    stream << GuestHeadingToRideId;
    stream << GuestIsLostCountdown;
    stream << GuestTimeOnRide;
    stream << PaidToEnter;
    stream << PaidOnRides;
    stream << PaidOnFood;
    stream << PaidOnDrink;
    stream << PaidOnSouvenirs;
    stream << OutsideOfPark;
    stream << Happiness;
    stream << HappinessTarget;
    stream << Nausea;
    stream << NauseaTarget;
    stream << Hunger;
    stream << Thirst;
    stream << Toilet;
    stream << TimeToConsume;
    stream << Intensity;
    stream << NauseaTolerance;
    stream << TimeInQueue;
    stream << CashInPocket;
    stream << CashSpent;
    stream << Photo1RideRef;
    stream << Photo2RideRef;
    stream << Photo3RideRef;
    stream << Photo4RideRef;
    stream << RejoinQueueTimeout;
    stream << PreviousRide;
    stream << PreviousRideTimeOut;
    stream << Thoughts;
    stream << LitterCount;
    stream << DisgustingCount;
    stream << AmountOfFood;
    stream << AmountOfDrinks;
    stream << AmountOfSouvenirs;
    stream << VandalismSeen;
    stream << VoucherType;
    stream << VoucherRideId;
    stream << SurroundingsThoughtTimeout;
    stream << Angriness;
    stream << TimeLost;
    stream << DaysInQueue;
    stream << BalloonColour;
    stream << UmbrellaColour;
    stream << HatColour;
    stream << FavouriteRide;
    stream << FavouriteRideRating;
    stream << ItemFlags;
}

void Staff::Serialise(DataSerialiser& stream)
{
    PeepBaseSerialise(*this, stream);
    stream << AssignedStaffType;
    stream << MechanicTimeSinceCall;
    stream << HireDate;
    stream << StaffId;
    stream << StaffOrders;
    stream << StaffMowingTimeout;
    stream << StaffLawnsMown;
    stream << StaffGardensWatered;
    stream << StaffLitterSwept;
    stream << StaffBinsEmptied;
}

void Vehicle::Serialise(DataSerialiser& stream)
{
    EntityBaseSerialise(*this, stream);
    stream << SubType;
    stream << Pitch;
    stream << bank_rotation;
    stream << remaining_distance;
    stream << velocity;
    stream << acceleration;
    stream << ride;
    stream << vehicle_type;
    stream << colours;
    stream << track_progress;
    stream << TrackTypeAndDirection;
    stream << TrackLocation;
    stream << next_vehicle_on_train;
    stream << prev_vehicle_on_ride;
    stream << next_vehicle_on_ride;
    stream << var_44;
    stream << mass;
    stream << update_flags;
    stream << SwingSprite;
    stream << current_station;
    stream << SwingPosition;
    stream << SwingSpeed;
    stream << status;
    stream << sub_state;
    stream << peep;
    stream << peep_tshirt_colours;
    stream << num_seats;
    stream << num_peeps;
    stream << next_free_seat;
    stream << restraints_position;
    stream << spin_speed;
    stream << sound2_flags;
    stream << spin_sprite;
    stream << sound1_id;
    stream << sound1_volume;
    stream << sound2_id;
    stream << sound2_volume;
    stream << sound_vector_factor;
    stream << var_C0;
    stream << speed;
    stream << powered_acceleration;
    stream << dodgems_collision_direction;
    stream << animation_frame;
    stream << animationState;
    stream << scream_sound_id;
    stream << TrackSubposition;
    stream << var_CE;
    stream << var_CF;
    stream << lost_time_out;
    stream << vertical_drop_countdown;
    stream << var_D3;
    stream << mini_golf_current_animation;
    stream << mini_golf_flags;
    stream << ride_subtype;
    stream << colours_extended;
    stream << seat_rotation;
    stream << target_seat_rotation;
    stream << BoatLocation;
    stream << IsCrashedVehicle;
}
