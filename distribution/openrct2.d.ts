/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

// OpenRCT2 Scripting API definition file

// To enable IntelliSense for your scripts in Visual Studio or Visual Studio Code,
// add the following line to the top of your script and change the path appropriately.
//
//   /// <reference path="/path/to/openrct2.d.ts" />
//

export type PluginType = "local" | "remote";

declare global {
    /**
     * Global context for accessing all other APIs.
     */
    /** APIs for cheats. */
    var cheats: Cheats;
    /** APIs for interacting with the stdout console. */
    var console: Console;
    /** Core APIs for plugins. */
    var context: Context;
    /** APIs for getting or setting the in-game date. */
    var date: GameDate;
    /** APIs for manipulating the map. */
    var map: GameMap;
    /** APIs for managing the server or interacting with the server or clients. */
    var network: Network;
    /** APIs for the park and management of it. */
    var park: Park;
    /**
     * APIs for controlling the user interface.
     * These will only be available to servers and clients that are not running headless mode.
     * Plugin writers should check if ui is available using `typeof ui !== 'undefined'`.
    */
    var ui: Ui;

    /** Selectable in-game colors. */
    enum Colour {
        Black,
        Grey,
        White,
        DarkPurple,
        LightPurple,
        BrightPurple,
        DarkBlue,
        LightBlue,
        IcyBlue,
        Teal,
        Aquamarine,
        SaturatedGreen,
        DarkGreen,
        MossGreen,
        BrightGreen,
        OliveGreen,
        DarkOliveGreen,
        BrightYellow,
        Yellow,
        DarkYellow,
        LightOrange,
        DarkOrange,
        LightBrown,
        SaturatedBrown,
        DarkBrown,
        SalmonPink,
        BordeauxRed,
        SaturatedRed,
        BrightRed,
        DarkPink,
        BrightPink,
        LightPink,
        ColourCount
    }

    /** Flags that determine how a ride departs the station. */
    enum DepartFlags {
        WaitForLoadMask = 7,
        WaitForLoad = 1 << 3,
        LeaveWhenAnotherArrives = 1 << 4,
        SynchroniseWithAdjacentStations = 1 << 5,
        WaitForMinimumLength = 1 << 6,
        WaitForMaximumLength = 1 << 7
    }

    /** Edge styles applied to land. */
    enum EdgeStyle {
        Rock,
        WoodRed,
        BoodBlack,
        Ice,
        Brick,
        IronRCT1,
        GreyRCT1,
        YellowRCT1,
        RedRCT1,
        PurpleRCT1,
        GreenRCT1,
        StoneBrownRCT1,
        StoneGreyRCT1,
        SkyscraperARCT1,
        SkyscraperBRCT1,
        EdgeStyleCount
    }

    /** Costumes available for Entertainers. */
    enum EntertainerCostume {
        Panda,
        Tiger,
        Elephant,
        Roman,
        Gorilla,
        Snowman,
        Knight,
        Astronaut,
        Bandit,
        Sheriff,
        Pirate,
        CostumeCount
    }

    /** Flags indicating the connection points for paths. */
    enum FootpathConnectionFlags {
        South = (1 << 0),
        Northeast = (1 << 1),
        West = (1 << 2),
        Southeast = (1 << 3),
        North = (1 << 4),
        Southwest = (1 << 5),
        East = (1 << 6),
        Northwest = (1 << 7)
    }

    /** Inspection interval. */
    enum InspectionInterval {
        Every10Minutes,
        Every20Minutes,
        Every30Minutes,
        Every45Minutes,
        EveryHour,
        Every2Hours,
        Never
    }

    /** All available music types. */
    enum Music {
        DodgemsBeat,
        FairgroundOrgan,
        RomanFanfare,
        Oriental,
        Martian,
        JungleDrums,
        Egyptian,
        Toyland,
        CircusShow,
        Space,
        Horror,
        Techno,
        Gentle,
        Summer,
        Water,
        WildWest,
        Jurassic,
        Rock,
        Ragtime,
        Fantasy,
        RockStyle2,
        Ice,
        Snow,
        CustomMusic1,
        CustomMusic2,
        Medieval,
        Urban,
        Organ,
        Mechanical,
        Modern,
        Pirates,
        RockStyle3,
        CandyStyle,
        MusicCount
    }

    /** Flags indicating ownership. */
    enum OwnershipFlags {
        Unowned = 0,
        ConstructionRightsOwned = (1 << 4),
        Owned = (1 << 5),
        ConstructionRightsAvailable = (1 << 6),
        OwnershipAvailable = (1 << 7)
    }

    /** Flags indicating various ride states and capabilities. */
    enum RideLifecycleFlags {
        OnTrack = 1 << 0,
        Tested = 1 << 1,
        TestInProgress = 1 << 2,
        NoRawStats = 1 << 3,
        PassStationNoStopping = 1 << 4,
        OnRidePhoto = 1 << 5,
        BreakdownPending = 1 << 6,
        BrokenDown = 1 << 7,
        DueInspection = 1 << 8,
        QueueFull = 1 << 9,
        Crashed = 1 << 10,
        HasStalledVehicle = 1 << 11,
        EverBeenOpened = 1 << 12,
        Music = 1 << 13,
        Indestructible = 1 << 14,
        IndestructibleTrack = 1 << 15,
        CableLiftHillComponentUsed = 1 << 16,
        CableLift = 1 << 17,
        /** Used for the Award for Best Custom-designed Rides. */
        NotCustomDesigned = 1 << 18,
        SixFlagsDeprecated = 1 << 19
    }

    /** Ride operating modes. */
    enum RideMode {
        Normal,
        ContinuousCircuit,
        ReverseInclineLaunchedShuttle,
        PoweredLaunchPassthrough, // RCT2 style, pass through station
        Shuttle,
        BoatHire,
        UpwardLaunch,
        RotatingLift,
        StationToStation,
        SingleRidePerAdmission,
        UnlimitedRidesPerAdmission = 10,
        Maze,
        Race,
        Dodgems,
        Swing,
        ShopStall,
        Rotation,
        ForwardRotation,
        BackwardRotation,
        FilmAvengingAviators,
        FilMouseTails = 20,
        SpaceRings,
        Beginners,
        LIMPoweredLaunch,
        FilmThrillRiders,
        FilmStormChasers,
        FilmSpaceRaiders,
        Intense,
        Berserk,
        HauntedHouse,
        CircusShow = 30,
        DownwardLaunch,
        CrookedHouse,
        FreefallDrop,
        ContinuousCircuitBlockSectioned,
        /** RCT1 style, don't pass through station. */
        PoweredLaunch,
        PoweredLaunchBlockSectioned,
        RideModeCount,
        Null = 255,
    }

    /** Shop items. */
    enum ShopItem {
        Balloon,
        Toy,
        Map,
        Photo,
        Umbrella,
        Drink,
        Burger,
        Chips,
        IceCream,
        CandyFloss,
        EmptyCan,
        Rubbish,
        EmptyBurgerBox,
        Pizza,
        Voucher,
        Popcorn,
        HotDog,
        Tentacle,
        Hat,
        ToffeeApple,
        TShirt,
        Doughnut,
        Coffee,
        EmptyCup,
        Chicken,
        Lemonade,
        EmptyBox,
        EmptyBottle = 27,
        Admission = 31,
        Photo2 = 32,
        Photo3,
        Photo4,
        Pretzel,
        Chocolate,
        IcedTea,
        FunnelCake,
        Sunglasses,
        BeefNoodles,
        FriedRiceNoodles,
        WontonSoup,
        MeatballSoup,
        FruitJuice,
        SoybeanMilk,
        Sujeonggwa,
        SubSandwich,
        Cookie,
        BowlRed,
        DrinkCarton,
        EmptyJuiceCup,
        RoastSausage,
        EmptyBowlBlue,
        ShopItemCount = 56,
        None = 255
    }

    /** Flags indicating slope. */
    enum SlopeFlags {
        Flat = 0x00,
        /** Not truly a slope, the game will reset this to flat and raise the surface height. (TODO is this correct?) */
        AllCornersUp = 0xFF,
    
        NorthCornerUp = (1 << 0),
        EastCornerUp = (1 << 1),
        SouthCornerUp = (1 << 2),
        WestCornerUp = (1 << 3),
        /** Combine with single corners to make the whole tile slope in that direction. TODO would this work with both up & down variations?*/
        SlopeDoubleHeight = (1 << 4),
    
        WestCornerDown = AllCornersUp & ~WestCornerUp,
        SouthCornerDown = AllCornersUp & ~SouthCornerUp,
        EastCornerDown = AllCornersUp & ~EastCornerUp,
        NorthCornerDown = AllCornersUp & ~NorthCornerUp,
    
        NortheastSideUp = NorthCornerUp | EastCornerUp,
        SoutheastSideUp = EastCornerUp | SouthCornerUp,
        NorthwestSideUp = NorthCornerUp | WestCornerUp,
        SouthwestSideUp = SouthCornerUp | WestCornerUp,
    
        WestEastValley = EastCornerUp | WestCornerUp,
        NorthSouthValley = NorthCornerUp | SouthCornerUp
    }

    /** Flags that apply to small scenery objects. */
    enum SmallSceneryFlags {
        FullTile = (1 << 0),
        VOffsetCentre = (1 << 1),
        RequireFlatSurface = (1 << 2),
        /** If set, the object is manually rotatable. If unset, rotation is automatic. */
        Rotatable = (1 << 3),
        Animated = (1 << 4),
        CanWither = (1 << 5),
        CanBeWatered = (1 << 6),
        AnimatedFG = (1 << 7),
        Diagonal = (1 << 8),
        HasGlass = (1 << 9),
        HasPrimaryColor = (1 << 10),
        FountainSpray1 = (1 << 11),
        FountainSpray4 = (1 << 12),
        IsClock = (1 << 13),
        SwampGoo = (1 << 14),
        HasFrameOffsets = (1 << 15),
        Flag17 = (1 << 16),
        /** Means scenery item can be placed in the air and over water. */
        Stackable = (1 << 17),
        NoWalls = (1 << 18),
        HasSecondaryColor = (1 << 19),
        NoSupports = (1 << 20),
        VisibleWhenZoomed = (1 << 21),
        Cog = (1 << 22),
        /** Means supports can be built on this object. Used for base blocks. */
        BuildDirectlyOnTop = (1 << 23),
        HalfSpace = (1 << 24),
        ThreeQuarters = (1 << 25),
        /** Used for scenery items which are support structures. */
        PaintSupports = (1 << 26),
        Flag27 = (1 << 27),
        IsTree = (1 << 28), // Added by OpenRCT2
    }

    /** Flags indicating the available orders for staff. Numeric values may mean different things for different staff types. */
    enum StaffOrderFlags {
        Sweeping = (1 << 0),
        WaterFlowers = (1 << 1),
        EmptyBins = (1 << 2),
        Mowing = (1 << 3),
        InspectRides = (1 << 0),
        FixRides = (1 << 1)
    }

    /** Surface types applied to land. */
    enum SurfaceStyle {
        Grass,
        Sand,
        Dirt,
        Rock,
        Martian,
        Checkerboard,
        GrassClumps,
        Ice,
        GridRed,
        GridYellow,
        GridBlue,
        GridGreen,
        SandDark,
        SandLight,
        SurfaceStyleCount //TODO should this include the technical types?
    }

    /** A list of window classifications, such as finances, ride list, etc. */
    type WindowClassification = 
        "main_window" | "top_toolbar" | "bottom_toolbar" | "tooltip" | "dropdown" | "about" | "publisher_credits" |
        "music_credits" | "error" | "ride" | "ride_construction" | "save_prompt" | "ride_list" | "construct_ride" |
        "demolish_ride_prompt" | "scenery" | "options" | "footpath" | "land" | "water" | "peep" | "guest_list" |
        "staff_list" | "fire_prompt" | "park_information" | "finances" | "title_menu" | "title_exit" |
        "recent_news" | "scenario_select" | "track_design_list" | "track_design_place" | "new_campaign" |
        "keyboard_shortcut_list" | "change_keyboard_shortcut" | "map" | "title_logo" | "banner" | "map_tooltip" |
        "editor_object_selection" | "editor_invention_list" | "editor_invention_list_drag" |
        "editor_scenario_options" | "editor_objective_options" | "manage_track_design" | "track_delete_prompt" |
        "install_track" | "clear_scenery" | "scenery_scatter" | "notification_options" | "cheats" | "research" |
        "viewport" | "text_input" | "map_gen" | "load_save" | "load_save_overwrite_prompt" | "title_options" |
        "land_rights" | "themes" | "tile_inspector" | "changelog" | "title_editor" | "title_command_editor" |
        "multiplayer" | "player" | "network_status" | "server_list" | "server_start" | "custom_currency_config" |
        "debug_paint" | "view_clipping" | "object_load_error" | "network" | "staff" | "editor_track_bottom_toolbar" |
        "editor_scenario_bottom_toolbar" | "chat" | "console" | "custom_window" | "null";

    /**
     * Registers the plugin. This should only be called once.
     * 
     * @param metadata Information about the plugin and the entry point.
     */
    function registerPlugin(metadata: PluginMetadata): void;

    /**
     * Represents a JavaScript object that can or should be disposed when no longer needed.
     */
    interface IDisposable {
        /** Dispose the object. */
        dispose(): void;
    }

    /**
     * A coordinate within the game's client screen in pixels.
     */
    interface ScreenCoordsXY {
        /** The horizontal coorindate in pixels. */
        x: number;
        /** The vertical coordinate in pixels. */
        y: number;
    }

    /**
     * A 2D coordinate within the game map.
     */
    interface CoordsXY {
        /** The x location on the map. Each in-game tile is a size of 32x32 units. */
        x: number;
        /** The y location on the map. Each in-game tile is a size of 32x32 units. */
        y: number;
    }

    /**
     * A 3D coordinate within the game map.
     */
    interface CoordsXYZ extends CoordsXY {
        /** The z location on the map. One land increment is 16 z units. A full-height wall is 32 z units. */
        z: number;
    }

    /**
     * A 3D coordinate within the game map which includes direction.
     */
    interface CoordsXYZD extends CoordsXYZ {
        /** A number between 0 and 3 indicating direction. */
        direction: number;
    }

    /**
     * A rectangular area specified using two coordinates.
     */
    interface MapRange {
        /** The top left map coordinate defining the range. */
        leftTop: CoordsXY;
        /** The bottom right map coordinate defining the range. */
        rightBottom: CoordsXY;
    }

    /**
     * Information about the plugin such as type, name, author, version, and entry point.
     */
    interface PluginMetadata {
        /** The plugin name. */
        name: string;
        /** The plugin version. Consider using semantic versioning (semver.org). */
        version: string;
        /** The plugin author(s). */
        authors: string | string[];
        /** The plugin type. */
        type: PluginType;
        /** The minimum API version required by the plugin. */
        minApiVersion?: number;
        /** The entry point for the plugin. */
        main: () => void;
    }

    /**
     * Console APIs. Currently they only interact with stdout.
     */
    interface Console {
        /**
         * Clear the console.
         */
        clear(): void;

        /**
         * Log a message in the console. TODO can this execute commands too?
         * 
         * @param message A message or content to print to the console.
         * @param optionalParams An array of parameters to pass to the console. TODO?
         */
        log(message?: any, ...optionalParams: any[]): void;

        /**
         * Execute a command using the legacy console REPL. This should not be used
         * by plugins, and exists only for servers to continue using old commands until
         * all functionality can be accomplished with this scripting API.
         *
         * @deprecated
         * @param command The command and arguments to execute.
         */
        executeLegacy(command: string): void;
    }

    /**
     * Core APIs for storage and subscriptions.
     */
    interface Context {
        /**
         * The user's current configuration.
         */
        configuration: Configuration;

        /**
         * Shared generic storage for all plugins. Data is persistant across instances
         * of OpenRCT2 and is stored externally as a single JSON file in the OpenRCT2
         * user directory. Internally it is a JavaScript object. Objects and arrays
         * are only copied by reference. The external file is only written when using
         * the `set` method, do not rely on the file being saved by modifying your own
         * objects. Functions and other internal structures will not be persisted.
         */
        sharedStorage: Configuration;

        /**
         * Render the current state of the map and save to disc.
         * Useful for server administration and timelapse creation.
         * 
         * @param options Options that control the capture and output file.
         */
        captureImage(options: CaptureOptions): void;

        /**
         * Get the loaded object at the given index.
         * 
         * @param type The object type.
         * @param index The index. TODO?
         * 
         * @return A loaded game object. TODO what if no object at index?
         */
        getObject(type: ObjectType, index: number): Object;

        /**
         * Get the loaded ride object at the given index.
         * 
         * @param type The "ride" object type.
         * @param index The index. TODO?
         * 
         * @return The loaded ride object. TODO what if no object at index?
         */
        getObject(type: "ride", index: number): RideObject;

        /**
         * Get the loaded small scenery object at the given index.
         * 
         * @param type The "small_scenery" object type.
         * @param index The index. TODO?
         * 
         * @return The loaded small scenery object. TODO what if no object at index?
         */
        getObject(type: "small_scenery", index: number): SmallSceneryObject;

        /**
         * Get all loaded objects of the given type.
         * 
         * @param type The object type.
         * 
         * @return An array of loaded objects.
         */
        getAllObjects(type: ObjectType): Object[];

        /**
         * Get all loaded objects of the "ride" object type.
         * 
         * @param type The "ride" object type.
         * 
         * @return An array of ride objects.
         */
        getAllObjects(type: "ride"): RideObject[];

        /**
         * Get a random integer within the specified range using the game's pseudo-
         * random number generator. This is part of the game state and shared across
         * all clients, you therefore must be in a context that can mutate the game
         * state. Use this to generate random numbers instead of Math.Random during
         * game logic routines such as hooks and game actions.
         * 
         * @param min The minimum value inclusive.
         * @param max The maximum value exclusive.
         * 
         * @return A random number in the given range.
         */
        getRandom(min: number, max: number): number;

        /**
         * Register a new game action that allows clients to interact with the game.
         * 
         * @param action The unique name of the action.
         * @param query Logic for validating and returning a price for an action.
         * @param execute Logic for validating and executing the action.
         * @throws An error if the action has already been registered by this or another plugin.
         */
        registerAction(
            action: string,
            query: (args: object) => GameActionResult,
            execute: (args: object) => GameActionResult): void;

        /**
         * Query the result of running a game action. This allows you to check the outcome and validity of
         * an action without actually executing it.
         * 
         * @param action The name of the action.
         * @param args The action parameters.
         * @param callback The function to be called with the result of the action.
         */
        queryAction(action: string, args: object, callback: (result: GameActionResult) => void): void;

        /**
         * Executes a game action. In a network game, this will send a request to the server and wait
         * for the server to reply.
         * 
         * @param action The name of the action.
         * @param args The action parameters.
         * @param callback The function to be called with the result of the action.
         */
        executeAction(action: string, args: object, callback: (result: GameActionResult) => void): void;

        /**
         * Subscribe to the given hook.
         * 
         * @param hook The hook type.
         * @param callback The function to execute when the hook is invoked.
         */
        subscribe(hook: HookType, callback: Function): IDisposable;

        /**
         * Subscribe to the query game action hook.
         * 
         * @param hook The "action.query" hook type.
         * @param callback The function to execute with the result of the game action.
         * 
         * @return A disposable object. Call `dispose()` on it to unsubscribe from the hook.
         */
        subscribe(hook: "action.query", callback: (e: GameActionEventArgs) => void): IDisposable;

        /**
         * Subscribe to the execute game action hook.
         * 
         * @param hook The "action.execute" hook type.
         * @param callback The function to execute with the result of the game action.
         * 
         * @return A disposable object. Call `dispose()` on it to unsubscribe from the hook.
         */
        subscribe(hook: "action.execute", callback: (e: GameActionEventArgs) => void): IDisposable;

        /**
         * Subscribe to the game tick hook.
         * 
         * @param hook The "interval.tick" hook type.
         * @param callback The function to execute when the hook is invoked.
         * 
         * @return A disposable object. Call `dispose()` on it to unsubscribe from the hook.
         */
        subscribe(hook: "interval.tick", callback: () => void): IDisposable;

        /**
         * Subscribe to the in-game day hook.
         * 
         * @param hook The "interval.day" hook type.
         * @param callback The function to execute when the hook is invoked.
         * 
         * @return A disposable object. Call `dispose()` on it to unsubscribe from the hook.
         */
        subscribe(hook: "interval.day", callback: () => void): IDisposable;

        /**
         * Subscribe to the network chat hook.
         * 
         * @param hook The "network.chat" hook type.
         * @param callback The function to execute with the result of the network chat event.
         * 
         * @return A disposable object. Call `dispose()` on it to unsubscribe from the hook.
         */
        subscribe(hook: "network.chat", callback: (e: NetworkChatEventArgs) => void): IDisposable;

        /**
         * Subscribe to the network authentication hook.
         * 
         * @param hook The "network.authenticate" hook type.
         * @param callback The function to execute with the result of the network authentication event.
         * 
         * @return A disposable object. Call `dispose()` on it to unsubscribe from the hook.
         */
        subscribe(hook: "network.authenticate", callback: (e: NetworkAuthenticateEventArgs) => void): IDisposable;

        /**
         * Subscribe to the join network event hook.
         * 
         * @param hook The "network.join" hook type.
         * @param callback The function to execute with the result of the join network event.
         * 
         * @return A disposable object. Call `dispose()` on it to unsubscribe from the hook.
         */
        subscribe(hook: "network.join", callback: (e: NetworkEventArgs) => void): IDisposable;

        /**
         * Subscribe to the leave network hook.
         * 
         * @param hook The "network.leave" hook type.
         * @param callback The function to execute with the result of the network event.
         * 
         * @return A disposable object. Call `dispose()` on it to unsubscribe from the hook.
         */
        subscribe(hook: "network.leave", callback: (e: NetworkEventArgs) => void): IDisposable;

        /**
         * Subscribe to the calulate ride ratings hook.
         * 
         * @param hook The "ride.ratings.calculate" hook type.
         * @param callback The function to execute with the result of the ride ratings calculation.
         * 
         * @return A disposable object. Call `dispose()` on it to unsubscribe from the hook.
         */
        subscribe(hook: "ride.ratings.calculate", callback: (e: RideRatingsCalculateArgs) => void): IDisposable;

		/**
		 * Subscribe to the action location hook.
		 * 
         * @param hook The "action.location" hook type.
         * @param callback The function to execute with the result of the action.
         * 
         * @return A disposable object. Call `dispose()` on it to unsubscribe from the hook.
		 */
        subscribe(hook: "action.location", callback: (e: ActionLocationArgs) => void): IDisposable;
    }

    /**
     * The game configuration.
     */
    interface Configuration {

        /**
         * Get all configuration values in the given namespace.
         * 
         * @param namespace The namespace the retrieve.
         * 
         * @return An object which maps the namespace to any object. TODO can this be worded better? Is it correct?
         */
        getAll(namespace: string): { [name: string]: any };

        /**
         * Get the value of the given key in the configuration.
         * 
         * @param key The key of the value to retrieve.
         * 
         * @return The configuration value if it exists, or undefined otherwise.
         */
        get<T>(key: string): T | undefined;

        /**
         * Get the value of the given key in the configuration.
         * 
         * @param key The key of the value to retrieve.
         * @param defaultValue The value to return if the key does not exist.
         * 
         * @return The configuration value if it exists, or defaultValue otherwise.
         */
        get<T>(key: string, defaultValue: T): T;

        /**
         * Set the value of the given key in the configuration. If the key does not exist in the
         * configuration, add the key and set its value.
         * 
         * @param key The key whose value will be set.
         * @param value The value of the key.
         */
        set<T>(key: string, value: T): void;

        /**
         * Determine if the configuration contains the given key.
         * 
         * @param key The key to check.
         * 
         * @return True if the key exists in the configuration, false otherwise.
         */
        has(key: string): boolean;
    }

    /**
     * Options for screenshot capture.
     */
    interface CaptureOptions {

        /**
         * A relative filename from the screenshot directory to save the capture as.
         * By default, the filename will be automatically generated using the system date and time.
         */
        filename?: string;

        /**
         * Width of the capture in pixels.
         * Do not set if you would like a giant screenshot.
         */
        width?: number;

        /**
         * Height of the capture in pixels.
         * Do not set if you would like a giant screenshot.
         */
        height?: number;

        /**
         * Map position to centre the view on in map units.
         * Do not set if you would like a giant screenshot.
         */
        position?: CoordsXY;

        /**
         * The zoom level, 0 is 1:1, 1 is 2:1, 2 is 4:1 etc.
         */
        zoom: number;

        /**
         * Rotation of the camera from 0 to 3.
         */
        rotation: number;
    }

    /** Game object types. */
    type ObjectType =
        "ride" |
        "small_scenery" |
        "large_scenery" |
        "wall" |
        "banner" |
        "footpath" |
        "footpath_addition" |
        "scenery_group" |
        "park_entrance" |
        "water" |
        "terrain_surface" |
        "terrain_edge" |
        "station" |
        "music";

    /** Hook types. */
    type HookType =
        "interval.tick" | "interval.day" |
        "network.chat" | "network.action" | "network.join" | "network.leave" |
        "ride.ratings.calculate" | "action.location";

    /** Types associated with spending money. */
    type ExpenditureType =
        "ride_construction" |
        "ride_runningcosts" |
        "land_purchase" |
        "landscaping" |
        "park_entrance_tickets" |
        "park_ride_tickets" |
        "shop_sales" |
        "shop_stock" |
        "food_drink_sales" |
        "food_drink_stock" |
        "wages" |
        "marketing" |
        "research" |
        "interest";

    /** 
     * Arugments for the GameActionEvent hook callback.
     */
    interface GameActionEventArgs {
        /** The number of the player who executed the action. TODO always 0 in single player? */
        readonly player: number;
        /** The event type. */
        readonly type: string;
        /** TODO True if the event only occurs in the client, false if needs to be synchronized to the network? */
        readonly isClientOnly: boolean;
        /** TODO */
        readonly args: object;
        /** The result of the action. */
        result: GameActionResult;
    }

    /** 
     * Representts the result of the game action.
     */
    interface GameActionResult {
        /** The error category. TODO enum */
        error?: number;
        /** The error title. Generally this is the same for all actions of the same type, e.g. "Land cannot be modified..." */
        errorTitle?: string;
        /** The error message. Usually this is the "Why", e.g. "Land not owned by park!" */
        errorMessage?: string;
        /** The position of the action, if any. If the action covers a range of tiles, the position is in the center. */
        position?: CoordsXYZ;
        /** The cost of the game action, if any. */
        cost?: number;
        /** The type of the cost of the game action for financial purposes, if any. */
        expenditureType?: ExpenditureType;
    }

    /** 
     * Represents the result of the ride creation game action.
     */
    interface RideCreateGameActionResult extends GameActionResult {
        /** The ride number. TODO reference? */
        readonly ride: number;
    }

    /** 
     * Arguments for the NetworkEvent hook callback.
     */
    interface NetworkEventArgs {
        /** The number of the player who executed the action. TODO always 0 in single player? */
        readonly player: number;
    }

    /** 
     * Arugments for the NetworkChatEvent hook callback. 
     */
    interface NetworkChatEventArgs extends NetworkEventArgs {
        /** The message text. */
        message: string;
    }

    /** 
     * Arugments for the NetworkAuthenticateEvent hook callback.
     */
    interface NetworkAuthenticateEventArgs {
        /** The name of the player authenticating. */
        readonly name: number;
        /** The IP address of the player authenticating. */
        readonly ipAddress: string;
        /** The public key hash of the player authenticating. */
        readonly publicKeyHash: string;
        /** True if the event should cancel the authentication, false otherwise. */
        cancel: boolean;
    }

    /** 
     * Arugments for the RideRatingCalculate hook callback.
     */
    interface RideRatingsCalculateArgs {
        /** The ride id. */
        readonly rideId: number;
        /** The ride excitement. */
        excitement: number;
        /** The ride intensity. */
        intensity: number;
        /** The ride nausea. */
        nausea: number;
    }

    /** 
     * Arugments for the ActionLocation hook callback.
     */
    interface ActionLocationArgs {
        readonly x: number;
        readonly y: number;
        readonly player: number;
        readonly type: number;
        readonly isClientOnly: boolean;
        result: boolean;
    }

    /**
     * APIs for the in-game date.
     */
    interface GameDate {
        /**
         * The total number of ticks that have elapsed since the beginning of the game / scenario. This
         * should never reset. TODO: so this isn't actually scenario, right? Only game?
         */
        readonly ticksElapsed: number;

        /**
         * The total number of months that have elapsed plus 8. This means the value will be 8 on March 1, Year 1,
         * and March 1, Year 2 will be 16, and so on. TODO: I corrected this, need to verify.
         * This represents the current date and may be reset by cheats or scripts.
         */
        monthsElapsed: number;

        /**
         * The total number of years that have elapsed. There are 8 months in each in-game year, so this
         * always equates to (monthsElapsed / 8).
         * Since the value is based on monthsElapsed, it may be reset by cheats or scripts.
         */
        readonly yearsElapsed: number;

        /**
         * How far through the month we are between 0 and 65535. This is incremented by 4 each tick, so
         * every month takes ~6.8 minutes to complete making a year take just under an hour.
         */
        monthProgress: number;

        /** The day of the month from 1 to 31. */
        readonly day: number;
        /** The current month of the year from 0 to 7, where 0 is March and 7 is October. */
        readonly month: number;
        /** The current year starting from 1. */
        readonly year: number;
    }

    /**
     * APIs for the map.
     */
    interface GameMap {
        /** The total size of the map. Each tile is 32 units, so this will be a multiple of 32. */
        readonly size: CoordsXY;
        /** The number of rides on the game map. */
        readonly numRides: number;
        /** The number of entities (guests, vehicles, ducks, etc) on the game map. */
        readonly numEntities: number;
        /** An array of all the rides on the map. */
        readonly rides: Ride[];

        /** 
         * Get the ride with the given ID.
         * 
         * @param id The ride ID.
         * 
         * @return The ride, or null if no ride exists with the given ID.
         */
        getRide(id: number): Ride;

        /**
         * Get the tile at the given position. TODO what if OOB? Why isn't the argument a CoordsXY?
         * 
         * @param x The x-coordinate of the tile.
         * @param y The y-coordinate of the tile.
         * 
         * @return The game tile. TODO what if OOB?
         */
        getTile(x: number, y: number): Tile;

        /**
         * Get the entity with the given ID.
         * 
         * @param id The entity ID.
         * 
         * @return The entity, or null if no entity exists with the given ID.
         */
        getEntity(id: number): Entity;

        /**
         * Get all map entities of the given type.
         * 
         * @param type The entity type (e.g. guest, vehicle, etc).
         * 
         * @return An array of entities.
         */
        getAllEntities(type: EntityType): Entity[];

        /**
         * Get all peep entities.
         * 
         * @param type The "peep" entity type.
         * 
         * @return An array of Peeps.
         */
        getAllEntities(type: "peep"): Peep[];
    }

    /** Type of tile elements (e.g. surface, footpath, track, etc). */
    type TileElementType =
        "surface" | "footpath" | "track" | "small_scenery" | "wall" | "entrance" | "large_scenery" | "banner"
        /** This only exists to retrieve the types for existing corrupt elements. For hiding elements, use the isHidden field instead. */
        | "openrct2_corrupt_deprecated";

    /**
     * A tile element.
     */
    interface BaseTileElement {
        /** The tile type. */
        type: TileElementType;
        /** The base height of the tile for collision. */
        baseHeight: number;
        /** The top height of the tile for collision. */
        clearanceHeight: number;

        /** 
         * Indicates if the tile is hidden. Take caution when changing this field, it may invalidate 
         * TileElements you have stored in your plugin.
         */
        isHidden: boolean;
    }

    /**
     * A surface element.
     */
    interface SurfaceElement extends BaseTileElement {
        /** A set of flags indicating slope. */
        slope: SlopeFlags;
        /** A number indicating the surface style. */
        surfaceStyle: SurfaceStyle;
        /** A number indicating the edge style. */
        edgeStyle: EdgeStyle;
        /** The surface height of the water. */
        waterHeight: number;
        /** The length of the grass from 0 to 6. Mowed = 0, Clear = 1-3, Clumps = 4-6. */
        grassLength: number;
        /** Flags indicating the ownership status (owned, unowned, etc). */
        ownership: OwnershipFlags;
        /** A number indicating the location of park fences. */
        parkFences: number;

        /** True if the surface is owned by the park, false otherwise. */
        readonly hasOwnership: boolean;
        /** True if the surface has construction rights owned by the park, false otherwise. */
        readonly hasConstructionRights: boolean;
    }

    /**
     * A footpath element.
     */
    interface FootpathElement extends BaseTileElement {
        /** A number from 0 to 15 indicating the footpath type. */
        footpathType: number;
        /** A set of flag indicating the path connection points.*/
        edgesAndCorners: FootpathConnectionFlags;
        /** A number from 0 to 3 indicating the slope direction, or null for a flat path. */
        slopeDirection: number | null;
        /** True if the path has a crossing which is occupied (e.g. by a train), false otherwise. */
        isBlockedByVehicle: boolean;
        /** Used for pathfinding. This value should not be adjusted. */
        isWide: boolean;
        /** Indicates if the path is a queue. */
        isQueue: boolean;
        /** A number from 0 to 3 indicating the queue banner direction, or null if no banner is present. */
        queueBannerDirection: number | null;
        /** If the path is a queue, this is the ride ID of the ride the queue is for. */
        ride: number;
        /** If the path is a queue, this is the station index of the station the queue leads to. */
        station: number;
        /** The index of the type of the path addition (e.g. bench, lamp, bin). */
        addition: number | null;
        /** Indicates if the path addition has been vandalized. */
        isAdditionBroken: boolean;
    }

    /**
     * A track element.
     */
    interface TrackElement extends BaseTileElement {
        /** A number indicating the track type. */
        trackType: number;
        /** Used for multi-tile track elements. The number increases from 0 to n, where n is the number of tiles occupied. */
        sequence: number;
        /** The id of the ride the track belongs to. */
        ride: number;
        /** A number from indexed from 0 indicating the station the track departs from. */
        station: number;
        /** True if the track has a chain lift, false otherwise. */
        hasChainLift: boolean;
    }

    /**
     * A scenery element a single tile or smaller.
     */
    interface SmallSceneryElement extends BaseTileElement {
        /** The object index within the tile. */
        object: number;
        /** The primary colour. */
        primaryColour: Colour;
        /** The secondary colour. */
        secondaryColour: Colour;
    }

    /** 
     * An entrance, exit, or park entrance. 
     */
    interface EntranceElement extends BaseTileElement {
        /** The object index within the tile. */
        object: number;
        /** Used for multi-tile entrance elements. Only applies to the park entrance and is numbered 0 to 2. */
        sequence: number;
        /** The ID of the ride the entrance/exit belongs to. TODO what if park entrance? */
        ride: number;
        /** The station number the entrance/exit belongs to, indexed from 0. */
        station: number;
    }

    /** 
     * A wall element that exists on a tile edge.
     */
    interface WallElement extends BaseTileElement {
        /** The object index within the tile. */
        object: number;
    }

    /**
     * A scenery element larger than one tile.
     */
    interface LargeSceneryElement extends BaseTileElement {
        /** The object index within the tile. */
        object: number;
        /** The primary colour. */
        primaryColour: Colour;
        /** The secondary colour. */
        secondaryColour: Colour;
    }

    /**
     * A banner element.
     */
    interface BannerElement extends BaseTileElement {
    }

    /**
     * A corrupt element. Often used to make the element above it invisible.
     */
    interface CorruptElement extends BaseTileElement {
    }

    /** A tile element, which can be a surface, footpath, or track element. */
    type TileElement = SurfaceElement | FootpathElement | TrackElement;

    /**
     * Represents a tile containing tile elements on the map. This is a fixed handle
     * for a given tile position. It can be re-used safely between game ticks.
     */
    interface Tile {
        /** The x position in tiles. */
        readonly x: number;
        /** The y position in tiles. */
        readonly y: number;
        /** An array of all the tile elements on this tile. */
        readonly elements: TileElement[];
        /** The number of tile elements on this tile. */
        readonly numElements: number;

        /**
         * Get or set the raw data for this tile.
         * This can provide more control and efficiency for tile manipulation but requires
         * knowledge of tile element structures and may change between versions of OpenRCT2.
         */
        data: Uint8Array;

        /** 
         * Gets the tile element at the given index on this tile. TODO what if OOB?
         * 
         * @param index The tile element index.
         * 
         * @return The tile element. TODO what if OOB?
         */
        getElement(index: number): TileElement;

        /** 
         * Gets the base tile element at the given index on this tile. TODO what if OOB?
         * 
         * @param index The base tile element index.
         * 
         * @return The base tile element. TODO what if OOB?
         */
        getElement<T extends BaseTileElement>(index: number): T;

        /** 
         * Inserts a new tile element at the given index on this tile. TODO what if OOB?
         * 
         * @param index The index at which to insert the element. TODO what if something's already at the index?
         * 
         * @return The updated tile element. TODO what if OOB?
         */
        insertElement(index: number): TileElement;

        /** 
         * Removes the tile element at the given index from this tile. TODO what if OOB?
         * 
         * @param index The tile element index.
         */
        removeElement(index: number): void;
    }

    /**
     * Represents the definition of a loaded object (.DAT or .json) such a ride type or scenery item.
     */
    interface Object {
        /** The object type. */
        readonly type: ObjectType;
        /** The index of the loaded object for the object type. */
        readonly index: number;
        /** The unique identifier of the object, e.g. "rct2.burgb". Only JSON objects will have an identifier. */
        readonly identifier: string;
        /** The object name in the user's current language. */
        readonly name: string;

        /**
         * The original unique identifier of the object, e.g. "BURGB   ".
         * This may have trailing spaces if the name is shorter than 8 characters.
         * Only .DAT objects or JSON objects based on .DAT objects will have legacy identifiers.
         */
        readonly legacyIdentifier: string;
    }

    /**
     * Represents the object definition of a ride or stall.
     */
    interface RideObject extends Object {
        /** The description of the ride / stall in the player's current language. */
        readonly description: string;
        /** A text description describing the capacity of the ride in the player's current language. */
        readonly capacity: string;
        /** The ride object flags. TODO not sure if these are the RIDE_ENTRY_FLAGs in Ride.h? */
        readonly flags: number;
        /** The ride types. Each ride can have up to three types (e.g. Gentle, Transport, etc). */
        readonly rideType: number[];
        /** The minimum number of cars per train. TODO w/ or w/o cheats? */
        readonly minCarsInTrain: number;
        /** The maximum number of cars per train. TODO w/ or w/o cheats? */
        readonly maxCarsInTrain: number;
        /** The maximum (?) number of cars per flat ride. TODO */
        readonly carsPerFlatRide: number;
        /** TODO? */
        readonly zeroCars: number;
        /** The vehicle shown in the train tab preview. */
        readonly tabVehicle: number;
        /** The default vehicle for the ride type. */
        readonly defaultVehicle: number;
        /** The vehicle number for the front car in a train. */
        readonly frontVehicle: number;
        /** The vehicle number for the second car in a train. */
        readonly secondVehicle: number;
        /** The vehicle number for the last car in a train. */
        readonly rearVehicle: number;
        /** The vehicle number for the third car in a train. */
        readonly thirdVehicle: number;
        /** An array of all the vehicles in the train. */
        readonly vehicles: RideObjectVehicle[];
        /** The exitement multiplier. */
        readonly excitementMultiplier: number;
        /** The intensity multiplier. */
        readonly intensityMultiplier: number;
        /** The nausea multiplier. */
        readonly nauseaMultiplier: number;
        /** The max support height for the ride in in-game units (e.g. 1 unit = 5 ft = 1.5m). */
        readonly maxHeight: number;
        /** A number indicating the first item sold (ride ticket, food, on-ride photo, etc). */
        readonly shopItem: ShopItem;
        /** A number indicating the second item sold (ride ticket, food, on-ride photo, etc). */
        readonly shopItemSecondary: ShopItem;
    }

    /**
     * Represents a defined vehicle within a Ride object definition.
     */
    interface RideObjectVehicle {
        readonly rotationFrameMask: number;
        readonly numVerticalFrames: number;
        readonly numHorizontalFrames: number;
        readonly spacing: number;
        readonly carMass: number;
        readonly tabHeight: number;
        /** The number of seats per car. */
        readonly numSeats: number;
        readonly spriteFlags: number;
        readonly spriteWidth: number;
        readonly spriteHeightNegative: number;
        readonly spriteHeightPositive: number;
        readonly animation: number;
        readonly flags: number;
        readonly baseNumFrames: number;
        readonly baseImageId: number;
        readonly restraintImageId: number;
        readonly gentleSlopeImageId: number;
        readonly steepSlopeImageId: number;
        readonly verticalSlopeImageId: number;
        readonly diagonalSlopeImageId: number;
        readonly bankedImageId: number;
        readonly inlineTwistImageId: number;
        readonly flatToGentleBankImageId: number;
        readonly diagonalToGentleSlopeBankImageId: number;
        readonly gentleSlopeToBankImageId: number;
        readonly gentleSlopeBankTurnImageId: number;
        readonly flatBankToGentleSlopeImageId: number;
        readonly curvedLiftHillImageId: number;
        readonly corkscrewImageId: number;
        readonly noVehicleImages: number;
        readonly noSeatingRows: number;
        /** A number indicating the inertia (tendency to keep current spin rate) of a spinning vehicle. */
        readonly spinningInertia: number;
        /** A number indicating the friction (force which slows spinning) of a spinning vehicle. */
        readonly spinningFriction: number;
        readonly frictionSoundId: number;
        readonly logFlumeReverserVehicleType: number;
        readonly soundRange: number;
        readonly doubleSoundFrequency: number;
        readonly poweredAcceleration: number;
        readonly poweredMaxSpeed: number;
        readonly carVisual: number;
        readonly effectVisual: number;
        readonly drawOrder: number;
        readonly numVerticalFramesOverride: number;
    }

    /**
     * Represents the object definition of a small scenery item such a tree.
     */
    interface SmallSceneryObject extends Object {
        /** Raw bit flags that describe characteristics of the scenery item. */
        readonly flags: SmallSceneryFlags;
        /** The default clearance height of the scenery item. 1 land height unit is 16 z-units, this is typically a multiple of 8. */
        readonly height: number;
        /** How much the scenery item costs to build. */
        readonly price: number;
        /** How much the scenery item costs to remove. */
        readonly removalPrice: number;
    }

    /**
     * Represents a ride or stall within the park.
     */
    interface Ride {
        /** The object metadata for this ride. */
        readonly object: RideObject;
        /** The unique ID / index of the ride. */
        readonly id: number;
        /** The type of the ride represented as the internal built-in ride type ID. */
        type: number;
        /** Whether the ride is a ride, shop or facility. */
        readonly classification: RideClassification;
        /** The generated or custom name of the ride. */
        name: string;
        /** Whether the ride is open, closed, testing, or simulating. */
        readonly status: RideStatus;
        /** Various flags related to the operation of the ride. */
        lifecycleFlags: RideLifecycleFlags;
        /** The operation mode. */
        mode: RideMode;
        /** Flags related to how trains depart. */
        departFlags: DepartFlags;
        /** The minimum time a train will wait at the station before departing. TODO units */
        minimumWaitingTime: number;
        /** The maximum time a train will wait at the station before departing. TODO units */
        maximumWaitingTime: number;
        /** The head vehicle IDs associated with the ride, one for each train. */
        readonly vehicles: number[];
        /** The track colour schemes for the ride. */
        colourSchemes: TrackColour[];
        /** The style used for the station, entrance, and exit building. */
        stationStyle: number;
        /** The music track to play at each station. */
        music: Music;
        /** Information about each station. */
        readonly stations: RideStation[];
        /** The admission price for the ride and the price of the on-ride photo, or the cost of each item of the stall. */
        price: number[];
        /** The total number of customers the ride has served since it was built. */
        totalCustomers: number;
        /** The date in months when the ride was built. Subtract this from `date.monthsElapsed` to get the age. */
        buildDate: number;
        /** How old the ride is in months. */
        readonly age: number;
        /** The running cost of the ride billed every fortnight. Multiply this by 16 to get the cost per hour (~1 in-game year). */
        runningCost: number;
        /** How often the ride should be inspected by a mechanic. */
        inspectionInterval: InspectionInterval;
        /** The value of the ride. Contributes to park value. */
        value: number;

        /**
         * The colour for each vehicle when the ride opens. Modifying this directly will not
         * change the colour of any currently running trains nor will it reflect them if they
         * have been modified.
         */
        vehicleColours: VehicleColour[];

        /**
         * The excitement metric of the ride represented as an integer. The value displayed in-game uses the last 2 digits
         * as the decimal portion.
         * For example, `652` equates to `6.52`.
         */
        excitement: number;

        /**
         * The intensity metric of the ride represented as an integer. The value displayed in-game uses the last 2 digits
         * as the decimal portion.
         * For example, `652` equates to `6.52`.
         */
        intensity: number;

        /**
         * The nausea metric of the ride represented as an integer. The value displayed in-game uses the last 2 digits
         * as the decimal portion.
         * For example, `652` equates to `6.52`.
         */
        nausea: number;
    }

    /** Indicates ride, stall, or facility. */
    type RideClassification = "ride" | "stall" | "facility";

    /** Indicates operating status (open, closed, testing, or simulating). */
    type RideStatus = "closed" | "open" | "testing" | "simulating";

    /** 
     * A track colour scheme.
     */
    interface TrackColour {
        /** The main colour of the scheme. */
        main: Colour;
        /** The secondary colour of the scheme. */
        additional: Colour;
        /** The support colour of the scheme. */
        supports: Colour;
    }

    /** 
     * A colour scheme used for a single vehicle.
     */
    interface VehicleColour {
        /** The body colour of the vehicle. */
        body: Colour;
        /** The trim colour of the vehicle. */
        trim: Colour;
        /** The third colour of the vehicle. TODO this is intended to be tertiary */
        ternary: Colour;
    }

    /**
     * Information associated with a ride station.
     */
    interface RideStation {
        /** The coordinate of the first tile of the station (with the control lights), in tiles. */
        start: CoordsXYZ;
        /** The length of the station in tiles. */
        length: number;
        /** The coordinate of the station entrance, in tiles. */
        entrance: CoordsXYZD;
        /** The coorindate of the station exit, in tiles. */
        exit: CoordsXYZD;
    }

    /** The type of a entity that may move (car, duck, peep). TODO missing litter, balloon */
    type EntityType =
        "car" | "duck" | "peep";

    /**
     * Represents an object "entity" on the map that can typically move and has a sub-tile coordinate.
     */
    interface Entity {
        /** The entity index within the entity list. */
        readonly id: number;
        /** The type of entity (e.g. car, peep, litter, etc). */
        readonly type: EntityType;
        /** The x-coordinate of the entity in game units. A tile is 32x32 units. */
        x: number;
        /** The y-coordinate of the entity in game units. A tile is 32x32 units. */
        y: number;
        /** The z-coordinate of the entity in game units. One land height increment is 16 z units. */
        z: number;

        /**
         * Removes the entity from the map.
         * Warning: Removing vehicles and peeps that are on rides is currently unsupported.
         */
        remove(): void;
    }

    /**
     * Represents a single car on a ride. A train is made up of multiple cars, but
     * something like boat hire will be one car per boat.
     */
    interface Car extends Entity {
        /**
         * The ride this car belongs to.
         */
        ride: number;

        /**
         * The ride object for this car, e.g. the ladybird trains object.
         */
        rideObject: number;

        /**
         * The vehicle type for the ride object used. This is a local index
         * into the ride object list of vehicle types.
         */
        vehicleObject: number;

        spriteType: number;

        /**
         * How many seats the car has, i.e. the capacity.
         */
        numSeats: number;

        /**
         * The next car on the same train. If this is the last or only car on the train,
         * this will return null.
         */
        nextCarOnTrain: number | null;

        /**
         * The previous car on the ride. This may be the on the same train or the previous
         * train. This will point to the last car if this is the first car on the ride.
         */
        previousCarOnRide: number;

        /**
         * The next car on the ride. This may be the on the same train or the next
         * train. This will point to the first car if this is the last car on the ride.
         */
        nextCarOnRide: number;

        /**
         * The current station the train is in or departing.
         */
        currentStation: number;

        /**
         * How heavy the car is. This is the sum of the mass of the empty car and the
         * mass of each guest that is riding it.
         */
        mass: number;

        /**
         * How much the car's velocity changes per tick.
         */
        acceleration: number;

        /**
         * How fast the car is moving.
         */
        velocity: number;

        /**
         * The current tilt of the car in the X/Y axis.
         */
        bankRotation: number;

        /**
         * The colour of the car.
         */
        colours: VehicleColour;

        /**
         * The acceleration for vehicles with constant power, e.g.
         * transport rides and boats.
         */
        poweredAcceleration: number;

        /**
         * The maximum speed for vehicles with constant power, e.g.
         * transport rides and boats.
         */
        poweredMaxSpeed: number;

        /**
         * Current status of the car or train.
         */
        status: VehicleStatus;

        /**
         * List of peep IDs ordered by seat.
         */
        peeps: (number | null)[];
    }

    type VehicleStatus =
        "arriving" |
        "crashed" |
        "crashing" |
        "crooked_house_operating" |
        "departing" |
        "doing_circus_show" |
        "ferris_wheel_rotating" |
        "haunted_house_operating" |
        "moving_to_end_of_station" |
        "operating_1a" |
        "rotating" |
        "showing_film" |
        "simulator_operating" |
        "space_rings_operating" |
        "starting" |
        "stopped_by_block_brake" |
        "stopping_1b" |
        "stopping" |
        "swinging" |
        "top_spin_operating" |
        "travelling_boat" |
        "travelling_cable_lift" |
        "travelling_dodgems" |
        "travelling" |
        "unloading_passengers_1c" |
        "unloading_passengers" |
        "waiting_for_cable_lift" |
        "waiting_for_passengers_17" |
        "waiting_for_passengers" |
        "waiting_to_depart" |
        "waiting_to_start";

    /**
     * Represents a guest or staff member.
     */
    interface Peep extends Entity {
        /** Whether the peep is a guest or staff member. */
        peepType: PeepType;
        /** Custom or generated name of the peep. */
        name: string;
        /** The peep's direct destination. */
        destination: CoordsXY;
        /** How tired the guest is between 32 and 128 where lower is more tired. */
        energy: number;
        /** The target energy value. Energy will increase / decrease slowly towards this value. */
        energyTarget: number;

        /**
         * Gets whether a given flag is set or not.
         * 
         * @param key The flag to test.
         * 
         * @return True if the flag is set, false otherwise.
         */
        getFlag(key: PeepFlags): boolean;

        /**
         * Sets the given flag to the given value.
         * 
         * @param key The flag to set.
         * @param value True to set the flag, false to clear it.
         */
        setFlag(key: PeepFlags, value: boolean): void;
    }

    /** Flags indicating various guest behaviors or actions. */
    type PeepFlags =
        "leavingPark" |
        "slowWalk" |
        "tracking" |
        "waving" |
        "hasPaidForParkEntry" |
        "photo" |
        "painting" |
        "wow" |
        "litter" |
        "lost" |
        "hunger" |
        "toilet" |
        "crowded" |
        "happiness" |
        "nausea" |
        "purple" |
        "pizza" |
        "explode" |
        "rideShouldBeMarkedAsFavourite" |
        "parkEntranceChosen" |
        "contagious" |
        "joy" |
        "angry" |
        "iceCream" |
        "hereWeAre";

    type PeepType = "guest" | "staff";

    /**
     * Represents a guest.
     */
    interface Guest extends Peep {
        /** Colour of the guest's t-shirt. */
        tshirtColour: Colour;
        /** Colour of the guest's trousers. */
        trousersColour: Colour; 
        /** Colour of the guest's balloon, if they have one. */
        balloonColour: Colour;
        /** Colour of the guest's hat, if they have one. */
        hatColour: Colour;
        /** Colour of the guest's umbrella, if they have one. */
        umbrellaColour: Colour;
        /** How happy the guest is between 0 and 255. Higher numbers indicate more happiness. */
        happiness: number;
        /** The target happiness value. Happiness will increase / decrease slowly towards this value. */
        happinessTarget: number;
        /** How nauseated the guest is between 0 and 255. Higher numbers indicate more nausea. */
        nausea: number;
        /** The target nausea value. Nausea will increase / decrease slowly towards this value. */
        nauseaTarget: number;
        /** How hungry the guest is between 0 and 255. Lower is more hungry. */
        hunger: number;
        /** How thirsty the guest is between 0 and 255. Lower is more thirsty. */
        thirst: number;
        /** How much the guest sneed to go to the toilet between 0 and 255. Higher numbers indicate greater need. */
        toilet: number;
        /** The mass of the guest, a random number between 45 and 76 inclusive. Affects vehicle mass. */
        mass: number;
        /** The guest's minimum preferred intensity between 0 and 15. */
        minIntensity: number;
        /** The guest's maximum preferred intensity between 0 and 15. */
        maxIntensity: number;
        /** The guest's tolerance to nauseating rides between 0 and 3. Lower numbers indicate lower tolerance. */
        nauseaTolerance: number;
        /** Amount of cash in the guest's pocket.  Represented as an integer (e.g. 150 = $1.50 = 150¥). */
        cash: number;
    }

    /**
     * Represents a staff member.
     */
    interface Staff extends Peep {
        /** The type of staff member (e.g. handyman, mechanic). */
        staffType: StaffType;
        /** Colour of the staff member. Not applicable for entertainers. */
        colour: Colour;
        /** The entertainer's costume, only applicable for entertainers. */
        costume: EntertainerCostume;

        /** 
         * A set of flags representing the enabled jobs the staff can do (e.g. sweep litter, water plants, 
         * inspect rides etc). Only applicable for Handymen and Mechanics. 
         */
        orders: StaffOrderFlags;
    }

    /** The available staff types (e.g. handyman, mechanic, etc). */
    type StaffType = "handyman" | "mechanic" | "security" | "entertainer";

    /**
     * Network APIs.
     * Use `network.status` to determine whether the current game is a client, server or in single player mode. TODO network.mode you mean?
     */
    interface Network {
        /** The network mode (none, server, client). */
        readonly mode: NetworkMode;
        /** The number of the groups on the server. TODO what if offline */
        readonly numGroups: number;
        /** The number of players on the server. TODO what if offline, can it be 0? */
        readonly numPlayers: number;
        /** An array with the list of the player groups. */
        readonly groups: PlayerGroup[];
        /** An array with the list of players. */
        readonly players: Player[];
        /** The current player object. */
        readonly currentPlayer: Player;
        /** The number of the group new players are assigned. */
        defaultGroup: number;

        /**
         * Get the server info (name, description, etc).
         */
        getServerInfo(): ServerInfo;

        /**
         * Create a new group. TODO add? Create? name?
         */
        addGroup(): void;

        /**
         * Get the group with the given index.
         * 
         * @param index The index of the group to retrieve. TODO what if OOB?
         * 
         * @return The player group.
         */
        getGroup(index: number): PlayerGroup;

        /**
         * Delete the group with the given index. TODO what happens to the players in that group?
         * 
         * @param index The index of the group to delete. TODO what if OOB?
         */
        removeGroup(index: number): void;

        /**
         * Get the player object for the given index.
         * 
         * @param index The index of the player to retrieve. TODO what if OOB?
         * 
         * @return The player object.
         */
        getPlayer(index: number): Player;

        /**
         * Kick the player with the given index from the server.
         * 
         * @param index The index of the player to kick. TODO what if OOB?
         */
        kickPlayer(index: number): void;

        /**
         * Send a message to all players.
         * 
         * @param message The message to send.
         */
        sendMessage(message: string): void;

        /**
         * Send a message to players with the given IDs.
         * 
         * @param message The message to send.
         * @param players An array of player IDs.
         */
        sendMessage(message: string, players: number[]): void;
    }

    /** The network mode. Select none for single player. */
    type NetworkMode = "none" | "server" | "client";

    /**
     * Represents a player within a network game.
     */
    interface Player {
        /** The unique player ID. */
        readonly id: number;
        /** The player name. */
        readonly name: string;
        /** The group number the player belongs to. */
        group: number;
        /** The ping of the player in ms. */
        readonly ping: number;
        /** The number of game actions executed by the player. */
        readonly commandsRan: number;
        /** The amount of money spent by the player. Represented as an integer (e.g. 150 = $1.50 = 150¥). */
        readonly moneySpent: number;
        /** The player's IP address. */
        readonly ipAddress: string;
        /** The player's public key hash. */
        readonly publicKeyHash: string;
    }

    /**
     * A group of players. Typically used to restrict permissions.
     */
    interface PlayerGroup {
        /** The unique group ID. */
        readonly id: number;
        /** The group name. */
        name: string;
        /** An array of permissions granted to members of the group. */
        permissions: PermissionType[];
    }

    /**
     * Server info. Commonly surfaced in the server browser.
     */
    interface ServerInfo {
        /** The name of the server. */
        readonly name: string;
        /** A description of the server. */
        readonly description: string;
        /** A message to be sent to the player upon log in. */
        readonly greeting: string;
        /** The name of the server provider. */
        readonly providerName: string;
        /** The server provider's email. */
        readonly providerEmail: string;
        /** The server provider's website. */
        readonly providerWebsite: string;
    }

    /** Multiplayer permission types. */
    type PermissionType =
        "chat" |
        "terraform" |
        "set_water_level" |
        "toggle_pause" |
        "create_ride" |
        "remove_ride" |
        "build_ride" |
        "ride_properties" |
        "scenery" |
        "path" |
        "clear_landscape" |
        "guest" |
        "staff" |
        "park_properties" |
        "park_funding" |
        "kick_player" |
        "modify_groups" |
        "set_player_group" |
        "cheat" |
        "toggle_scenery_cluster" |
        "passwordless_login" |
        "modify_tile" |
        "edit_scenario_options";

    /**
     * Park APIs
     */

    /** The type of park message, which determines icon and behaviour. */
    type ParkMessageType =
        "attraction" | "peep_on_attraction" | "peep" | "money" | "blank" | "research" | "guests" | "award" | "chart";

    /**
     * Represents an in-game message.
     */
    interface ParkMessage {
        /** Whether the message has been shown and archived. */
        readonly isArchived: boolean;
        /** The number of total elapsed months since the beginning of the game when the message was sent. */
        month: number;
        /** The day of the month this message was posted (1-31). */
        day: number;
        /** How old the message is in ticks. */
        tickCount: number;
        /** The format of the message such as the icon and whether location is enabled. */
        type: ParkMessageType;
        /** The message content. */
        text: string;

        /** 
         * The id of the mesage subject, if any. Use Ride ID for attraction, Entity ID for peep_on_attraction 
         * or peep, and researched item id for research.
         */
        subject?: number;

        /**
         * Removes the message.
         */
        remove(): void;
    }

    /**
     * The park message description. TODO what's the difference between this and ParkMessage?
     */
    interface ParkMessageDesc {
        /** The format of the message such as the icon and whether location is enabled. */
        type: ParkMessageType;
        /** The message content. */
        text: string;

        /** 
         * The id of the mesage subject, if any. Use Ride ID for attraction, Entity ID for peep_on_attraction 
         * or peep, and researched item id for research.
         */
        subject?: number;
    }

    /**
     * Represents the park.
     */
    interface Park {
        /** The amount of cash available to spend.  Represented as an integer (e.g. 150 = $1.50 = 150¥). */
        cash: number;
        /** The park rating (0-999). */
        rating: number;
        /** The current amount of the bank loan. Represented as an integer (e.g. $10,000 = 1000000). TODO since this can only be integers does it use the *100 multiplier for cash values? */
        bankLoan: number;
        /** The maximum possible bank loan. Represented as an integer (e.g. $10,000 = 1000000). TODO since this can only be integers does it use the *100 multiplier for cash values? */
        maxBankLoan: number;
		/** The park name. */
        name: string;
        /** An array of the most recent 61 park messages. */
        messages: ParkMessage[];

        /**
         * Post a massge to the in-game tracker.
         * 
         * @param message The message content.
         */
        postMessage(message: string): void;

        /**
         * Post a message to the in-game tracker.
         * 
         * @param message The message content. TODO what's the difference between this and the other method?
         */
        postMessage(message: ParkMessageDesc): void;
    }

    /**
     * Flags tracking enabled cheats.
     */
    interface Cheats {
        /** Allows changing ride types after constructing rides. */
        allowArbitraryRideTypeChanges: boolean;
        /** Allows track to be placed at increments of 8 z-units, which is not normally accessible by path. */
        allowTrackPlaceInvalidHeights: boolean;
        /** Allows building while the game is paused. */
        buildInPauseMode: boolean;
        /** Prevents all rides from breaking down. */
        disableAllBreakdowns: boolean;
        /** Prevents the brakes failure breakdown type. */
        disableBrakesFailure: boolean;
        /** Disables collision checking for all objects. */
        disableClearanceChecks: boolean;
        /** Prevents litter from being placed. */
        disableLittering: boolean;
        /** Prevents flowers and other plants from needing water. */
        disablePlantAging: boolean;
        /** Prevents ride value decrease as they age. */
        disableRideValueAging: boolean;
        /** Causes the game to ignore support limits. */
        disableSupportLimits: boolean;
        /** Ignores the normal limit for train length. The upper limit is still 255. */
        disableTrainLengthLimit: boolean;
        /** Prevents angry guests from causing vandalism. */
        disableVandalism: boolean;
        /** Enables all track pieces for which track and vehicle sprites exist for the given ride. TODO is this right? */
        enableAllDrawableTrackPieces: boolean;
        /** Allow placing a chain lift on any track piece. */
        enableChainLiftOnAllTrack: boolean;
        /** Unlocks limits on lift hill chain speed. */
        fastLiftHill: boolean;
        /** Freezes the weather to its current state. */
        freezeWeather: boolean;
        /** Will make all rides available to build regardless of if they have been researched or not. */
        ignoreResearchStatus: boolean;
        /** Guests will ignore ride intensity. */
        ignoreRideIntensity: boolean;
        /** Allow marketing campaigns to continue indefinitely. */
        neverendingMarketing: boolean;
        /** Allows changing certain settings normally only available in the scenario editor. */
        sandboxMode: boolean;
        /** Makes all operating modes available for every ride type. */
        showAllOperatingModes: boolean;
        /** Allows selecting vehicles from other track types. */
        showVehiclesFromOtherTrackTypes: boolean;
    }

    /**
     * User Interface APIs
     * These will only be available to servers and clients that are not running headless mode.
     * Plugin writers should check if ui is available using `typeof ui !== 'undefined'`.
     */
    interface Ui {
        /** The width of the client window in pixels. */
        readonly width: number;
        /** The height of the cleint window in pixels. */
        readonly height: number;
        /** The total number of in-game windows. TODO? */
        readonly windows: number;
        /** The window currently in focus. */
        readonly mainViewport: Viewport;
        /** The currently highlighted tiles. */
        readonly tileSelection: TileSelection;
        /** The current mouse tool. */
        readonly tool: Tool;

        /**
         * Get the window with the given ID.
         * 
         * @param id The window ID.
         * 
         * @return The window object, or null if none exists with the given ID.
         */
        getWindow(id: number): Window;

        /**
         * Get a window with the given classification. TODO not sure though because I would assume that it would be an array
         * 
         * @param classification TODO?
         * 
         * @return The window object.
         */
        getWindow(classification: string): Window;

        /**
         * Open a window with the given parameters.
         * 
         * @param desc An object containing information about the size and appaearance of the window.
         * 
         * @return An object representing the opened window
         */
        openWindow(desc: WindowDesc): Window;

        /**
         * Close all windows with the given classification. Alternatively, if an ID is given, close the window of the given classification and ID. TODO?
         * 
         * @param classification The window classification.
         * @param id (Optional) The id of the window to close.
         */
        closeWindows(classification: string, id?: number): void;

        /**
         * Close all windows. Performs the same operation as the shift+backspace shortcut.
         */
        closeAllWindows(): void;

        /**
         * Show a red error box.
         * 
         * @param title The title / first line of the box.
         * @param message The message / second line of the box.
         */
        showError(title: string, message: string): void;

        /**
         * Shows a text input prompt and calls the given callback when entered.
         * 
         * @param desc The parameters for the text input window.
         */
        showTextInput(desc: TextInputDesc): void;

        /**
         * Begins a new tool session. The cursor will change to the style specified by the
         * given tool descriptor and cursor events will be provided.
         * 
         * @param tool The properties and event handlers for the tool.
         */
        activateTool(tool: ToolDesc): void;

        /**
         * Add a menu item, i.e. to a dropdown. TODO?
         * 
         * @param text The menu item text.
         * @param callback A function to call upon selecting the menu item.
         */
        registerMenuItem(text: string, callback: () => void): void;
    }

    /**
     * Parameters for the text input window.
     */
    interface TextInputDesc {
        /** The title of the text input window. */
        title: string;
        /** The description to show above the text box. */
        description: string;
        /** The current value of the text box, if any. */
        initialValue?: string;
        /** (Optional) The maximum length the input can be. */
        maxLength?: number;

        /**
         * The function to call when the user has entered a new value and pressed OK. The function takes a
         * string and has no return value.
         */
        callback: (value: string) => void;
    }

    /**
     * Represents a range of selected tiles. TODO do you need both? Can the array be different than the range & vice versa?
     */
    interface TileSelection {
        /** A rectangular range of tiles. */
        range: MapRange;
        /** An array of tile coordinates. */
        tiles: CoordsXY[];
    }

    /**
     * A cursor tool.
     */
    interface Tool {
        /** The unique ID of the tool. TODO is there a set of values for this? */
        id: string;
        /** The cursor type (e.g. arrow, bench, bin, etc). */
        cursor: CursorType;

        /**
         * The function to call when the tool is canceled. Often will reset to the default cursor. The
         * function takes no arguments and returns no value.
         */
        cancel: () => void;
    }

    /**
     * Arguments associated with tool events.
     */
    interface ToolEventArgs {
        /** True if the tool is activated. Usually corresponds to the mouse button being down. */
        readonly isDown: boolean;
        /** The game client coordinates of the tool. */
        readonly screenCoords: ScreenCoordsXY;
        /** The map coordinates of the tool, if applicable. */
        readonly mapCoords?: CoordsXYZ;
        /** The map tile index, if applicable. */
        readonly tileElementIndex?: number;
        /** The ID of the entity under the tool. */
        readonly entityId?: number;
    }

    /**
     * Describes the properties and event handlers for a custom tool.
     */
    interface ToolDesc {
        /** The unique ID of the tool. */
        id: string;
        /** The cursor type for the custom tool (e.g. arrow, bench, bin, etc). */
        cursor?: CursorType;

        /**
         * A function called when the tool is started. The function takes no arguments and returns no value.
         */
        onStart: () => void;

        /** 
         * A function called when the tool is activated. Usually corresponds to the mouse button being pressed.
         * The function takes a ToolEventArgs instance and returns no value.
         */
        onDown: (e: ToolEventArgs) => void;

        /**
         * A function called when the tool is moved. Usually corresponds to the mouse being moved. The function
         * takes a ToolEventArgs instance and returns no value.
         */
        onMove: (e: ToolEventArgs) => void;

        /** 
         * A function called when the tool is deactivated. Usually corresponds to the mouse button being released.
         * The function takes a ToolEventArgs instance and returns no value.
         */
        onUp: (e: ToolEventArgs) => void;

        /**
         * A function called when the tool is finished. The function takes no arguments and returns no value.
         */
        onFinish: () => void;
    }

    /**
     * A string representing the cursor type.
     */
    type CursorType =
        "arrow" |
        "bench_down" |
        "bin_down" |
        "blank" |
        "cross_hair" |
        "diagonal_arrows" |
        "dig_down" |
        "entrance_down" |
        "fence_down" |
        "flower_down" |
        "fountain_down" |
        "hand_closed" |
        "hand_open" |
        "hand_point" |
        "house_down" |
        "lamppost_down" |
        "paint_down" |
        "path_down" |
        "picker" |
        "statue_down" |
        "tree_down" |
        "up_arrow" |
        "up_down_arrow" |
        "volcano_down" |
        "walk_down" |
        "water_down" |
        "zzz";

    /**
     * Represents the type of a widget, e.g. button or label.
     */
    type WidgetType =
        "button" | "checkbox" | "dropdown" | "groupbox" | "label" | "listview" | "spinner" | "viewport";

    /**
     * An interface that represents some UI element.
     */
    interface Widget {
        /** The type of widget (e.g. checkbox, dropdown, label, etc). */
        type: WidgetType;
        /** The horizotal position of the widget relative to the window, in pixels. */
        x: number;
        /** The vertical position of the widget relative to the window, in pixels. */
        y: number;
        /** The width of the widget, in pixels. */
        width: number;
        /** The height of the widget, in pixels. */
        height: number;
        /** (Optional) The name of the widget. */
        name?: string;
        /** (Optional) The widget tooltip. */
        tooltip?: string;
        /** Indicates if the widget can be interacted with. */
        isDisabled?: boolean;
    }

    /**
     * Represents a button widget, including callbacks.
     */
    interface ButtonWidget extends Widget {
        /** The button image. Commonly used for tabs. TODO what number for no image? */
        image: number;
        /** The button appearance, pressed or not pressed. */
        isPressed: boolean;
        /** The button text. */
        text: string;

        /**
         * Whether the button has a 3D border. By default, text buttons have borders and image 
         * buttons do not but it can be overridden using this property.
         */
        border?: boolean;

        /**
         * A function to be executed when the button is clicked. The function takes no argument and
         * returns no value.
         */
        onClick: () => void;
    }

    /**
     * Represents a checkbox widget, including callbacks.
     */
    interface CheckboxWidget extends Widget {
        /** The checkbox text. */
        text: string;
        /** The checkbox state. */
        isChecked: boolean;
        
        /**
         * A function to be executed when the state of the checkbox changes. The function takes a boolean
         * that represents the state of the checkbox and returns no value.
         */
        onChange: (isChecked: boolean) => void;
    }

    /**
     * Represents a dropdown widget.
     */
    interface DropdownWidget extends Widget {
        /** An array of strings available as dropdown options. */
        items: string[];
        /** The index of the currently selected item. */
        selectedIndex: number;

        /**
         * A function to be executed when the selected item changes. The function takes the index of the item
         * and returns no value.
         */
        onChange: (index: number) => void;
    }

    /** 
     * Represents a text label.
     */
    interface LabelWidget extends Widget {
        /** The label text. */
        text: string;

        /** 
         * A function to be called when the label changes. It is uncommon for this to do anything for labels.
         * The function takes an index (TODO - what does it mean?) and returns no value.
         */
        onChange: (index: number) => void;
    }

    /**
     * The sorting order to be used for list columns.
     */
    type SortOrder = "none" | "ascending" | "descending";

    /**
     * The scroll bar type(s) to be used.
     */
    type ScrollbarType = "none" | "horizontal" | "vertical" | "both";

    /**
     * Represents a column in a list.
     */
    interface ListViewColumn {
        /** Indicates the ability to sort by this column. TODO default? */
        canSort?: boolean;
        /** The current sort order for the column. */
        sortOrder?: SortOrder;
        /** (Optional) The column header. */
        header?: string;
        /** (Optional) The tooltop for the column header. */
        headerTooltip?: string;
        /** (Optional) The column width. TODO what if not defined? */
        width?: number;
        /** (Optional) A number used to size columns in a list by ratio. Consider using this for lists with no horizontal scroll. */
        ratioWidth?: number;
        /** (Optional) The minimum width of a column, in pixels. */
        minWidth?: number;
        /** (Optional) The maximum width of a column, in pixels. */
        maxWidth?: number;
    }

    /**
     * An array of strings in a list view.
     */
    type ListViewItem = string[];

    /**
     * Represents a selected cell.
     */
    interface RowColumn {
        /** The row number of the selected cell indexed from 0. */
        row: number;
        /** The column number of the selected cell indexed from 0. */
        column: number;
    }

    /** 
     * Represents a list view widget and callbacks for interacting with the list.
     */
    interface ListView extends Widget {
        /** (Optional) The scollbar type to use for the list. Vertical by default. */
        scrollbars?: ScrollbarType;
        /** (Optional) Indicates if alternating rows use different colouring to improve readability. */
        isStriped?: boolean;
        /** (Optional) Indicates if column headings should be shown. */
        showColumnHeaders?: boolean;
        /** (Optional) An array of columns in the list. */
        columns?: ListViewColumn[];
        /** (Optional) An array or strings or ListViewItem with the list items. */
        items?: string[] | ListViewItem[];
        /** (Optional) Indicates the row and column of a selected cell, if any. */
        selectedCell?: RowColumn;
        /** Indicates the highlighted cell, if any. */
        readonly highlightedCell?: RowColumn;
        /** (Optional) Indicates if cells can be selected in the list. TODO default? */
        canSelect?: boolean;

        /**
         * A function executed when a cell is highlighted. The function takes an item index and a column index
         * and returns no value. Note that the item index is not a row index because the list may be sorted.
         */
        onHighlight: (item: number, column: number) => void;

        /**
         * A function executed when a cell is clicked. The function takes an item index and a colimn index and
         * returns no value. Note that the item index is not a row index because the list may be sorted.
         */
        onClick: (item: number, column: number) => void;
    }

    /**
     * Represents a spinner widget which can be incremented or decremented, such as for price.
     */
    interface SpinnerWidget extends Widget {
        /** The spinner text. */
        text: string;

        /**
         * A function executed when the spinner is decremented. The function takes no arguments and returns no value.
         */
        onDecrement: () => void;

        /** 
         * A function executed when the spinner is incremented. The function takes no arguments and returns no value.
         */
        onIncrement: () => void;
    }

    /**
     * Represents a viewport widget.
     */
    interface ViewportWidget extends Widget {
        /** A viewport to a point on the map. */
        viewport: Viewport
    }

    /**
     * Represents a game window.
     */
    interface Window {
        /** TODO needs to be a number? */
        classification: WindowClassification;
        /** An identifier unique among open windows. */
        number: number;
        /** The horizontal position of the window in the game client, in pixels. */
        x: number;
        /** The vertical position of the window in the game client, in pixels. */
        y: number;
        /** The width of the window, in pixels. */
        width: number;
        /** The height of the window, in pixels. */
        height: number;
        /** The minimum width of the window, in pixels. */
        minWidth: number;
        /** The maximum width of the window, in pixels. */
        maxWidth: number;
        /** The minimum height of the window, in pixels. */
        minHeight: number;
        /** The maximum height of the window, in pixels. */
        maxHeight: number;
        /** TODO? */
        isSticky: boolean;
        /** An array of numbers representing the window colours. TODO does this use the selectable game colors? */
        colours: number[];
        /** The window title. */
        title: string;
        /** An array of widgets that make up the window. */
        widgets: Widget[];
        /** The index of the currently active tab. */
        tabIndex: number;

        /**
         * Close the window.
         */
        close(): void;

        /**
         * Bring the window to the front and make it the focused window.
         */
        bringToFront(): void;

        /**
         * Find the widget of the given type with the given name.
         * 
         * @param name The name of the widget. TODO what if there is more than one?
         * 
         * @return The widget, or TODO if it doesn't exist.
         */
        findWidget<T extends Widget>(name: string): T;
    }

    /** 
     * Represents a window description. Window descriptions are used to create windows with `ui.createWindow`.
     */
    interface WindowDesc {
        /** TODO? */
        classification: string;
        /** (Optional) The horizontal position of the window, in pixels. */
        x?: number;
        /** (Optional) The vertical position of the window, in pixels. */
        y?: number;
        /** The width of the window, in pixels. */
        width: number;
        /** The height of the window, in pixels. */
        height: number;
        /** The title of the window. */
        title: string;
        /** (Optional) The window ID. */
        id?: number;
        /** (Optional) The minimum width of the window, in pixels. */
        minWidth?: number;
        /** (Optional) The maximum width of the window, in pixels. */
        minHeight?: number;
        /** (Optional) The minimum height of the window, in pixels. */
        maxWidth?: number;
        /** (Optional) The maximum height of the window, in pixels. */
        maxHeight?: number;
        /** (Optional) An array of widgets that make up the window. */
        widgets?: Widget[];
        /** (Optional) The window colours. TODO does this use the selectable game colors? */
        colours?: number[];
        /** (Optional) An array of tabs in the winow. */
        tabs?: WindowTabDesc[];

        /**
         * A function executed when the window is closed.
         */
        onClose?: () => void;

        /**
         * A function executed when the window is updated. TODO when does this happen exactly?
         */
        onUpdate?: () => void;

        /**
         * A function executed when a new tab is selected.
         */
        tabChange?: () => void;
    }

    /**
     * Represents the animation state of an image in a tab.
     */
    interface ImageAnimation {
        /** The animation frame used when the tab is not selected. */
        frameBase: number;
        /** (Optional) The number of frames to use in the animation. */
        frameCount?: number;
        /** (Optional) How long to display each frame, in x. TODO units? Ticks? default if not provided? */
        frameDuration?: number;
        /** TODO offset from where? window? tab? */
        offset?: ScreenCoordsXY;
    }

    /**
     * Represents a window tab and its widgets.
     */
    interface WindowTabDesc {
        /** The image number or image animation to be used for the tab. */
        image: number | ImageAnimation;
        /** (Optional) A list of widgets in the tab. */
        widgets?: Widget[];
    }

    /**
     * Represents a viewport.
     */
    interface Viewport {
        /** The left edge of the viewport. TODO relative to where? */
        left: number;
        /** The top edge of the viewport. TODO relative to where? */
        top: number;
        /** The right edge of the viewport. TODO relative to where? */
        right: number;
        /** The bottom edge of the viewport. TODO relative to where? */
        bottom: number;
        /** The rotational position of the viewport (0-3). */
        rotation: number;
        /** The zoom level of the viewport (0-x). TODO */
        zoom: number;
        /** TODO? */
        visibilityFlags: number;

        /**
         * Get the center coorindates of the viewport.
         */
        getCentrePosition(): CoordsXY;

        /**
         * Change the viewport's position. TODO center position?  Upper left?
         * 
         * @param position The 2- or 3-dimensional coordinates to move to.
         */
        moveTo(position: CoordsXY | CoordsXYZ): void;

        /**
         * Scroll the viewport to the given position. TODO how is this different from move? scroll vs instant i guess?
         * 
         * @param position The 2- or 3-dimensional coordinatrs to scroll to.
         */
        scrollTo(position: CoordsXY | CoordsXYZ): void;
    }
}
