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

    /**
     * Registers the plugin. This only only be called once.
     * @param metadata Information about the plugin and the entry point.
     */
    function registerPlugin(metadata: PluginMetadata): void;

    /**
     * Represents a JavaScript object that can or should be disposed when no longer needed.
     */
    interface IDisposable {
        dispose(): void;
    }

    /**
     * A coordinate within the game's client screen in pixels.
     */
    interface ScreenCoordsXY {
        x: number;
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
        leftTop: CoordsXY;
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
         * @param type The "small_cenery" object type.
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
         */
        subscribe(hook: "action.query", callback: (e: GameActionEventArgs) => void): IDisposable;

        /**
         * Subscribe to the execute game action hook.
         * 
         * @param hook The "action.execute" hook type.
         * @param callback The function to execute with the result of the game action.
         */
        subscribe(hook: "action.execute", callback: (e: GameActionEventArgs) => void): IDisposable;

        /**
         * Subscribe to the game tick hook.
         * 
         * @param hook The "interval.tick" hook type.
         * @param callback The function to execute when the hook is invoked.
         */
        subscribe(hook: "interval.tick", callback: () => void): IDisposable;

        /**
         * Subscribe to the in-game day hook.
         * 
         * @param hook The "interval.day" hook type.
         * @param callback The function to execute when the hook is invoked.
         */
        subscribe(hook: "interval.day", callback: () => void): IDisposable;

        /**
         * Subscribe to the network chat hook.
         * 
         * @param hook The "network.chat" hook type.
         * @param callback The function to execute with the result of the network chat event.
         */
        subscribe(hook: "network.chat", callback: (e: NetworkChatEventArgs) => void): IDisposable;

        /**
         * Subscribe to the network authentication hook.
         * 
         * @param hook The "network.authenticate" hook type.
         * @param callback The function to execute with the result of the network authentication event.
         */
        subscribe(hook: "network.authenticate", callback: (e: NetworkAuthenticateEventArgs) => void): IDisposable;

        /**
         * Subscribe to the join network event hook.
         * 
         * @param hook The "network.join" hook type.
         * @param callback The function to execute with the result of the join network event.
         */
        subscribe(hook: "network.join", callback: (e: NetworkEventArgs) => void): IDisposable;

        /**
         * Subscribe to the leave network hook.
         * 
         * @param hook The "network.leave" hook type.
         * @param callback The function to execute with the result of the network event.
         */
        subscribe(hook: "network.leave", callback: (e: NetworkEventArgs) => void): IDisposable;

        /**
         * Subscribe to the calulate ride ratings hook.
         * 
         * @param hook The "ride.ratings.calculate" hook type.
         * @param callback The function to execute with the result of the ride ratings calculation.
         */
        subscribe(hook: "ride.ratings.calculate", callback: (e: RideRatingsCalculateArgs) => void): IDisposable;
    }

    /**
     * The game configuration.
     */
    interface Configuration {

        /**
         * Get the configuration for the given namespace.
         * 
         * @param namespace TODO?
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
        "ride.ratings.calculate";

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

    /** Arugments for the GameActionEvent hook callback. */
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

    /** The result of the game action. */
    interface GameActionResult {
        /** TODO? */
        error?: number;
        /** TODO? */
        errorTitle?: string;
        /** The error message which is displayed in a pop-up, if specified. */
        errorMessage?: string;
        /** The position of the error message, if any. TODO position of the error or just the position of the action? */
        position?: CoordsXYZ;
        /** The cost of the game action, if any. */
        cost?: number;
        /** The type of the cost of the game action for financial purposes, if any. */
        expenditureType?: ExpenditureType;
    }

    /** The result of the ride creation game action. */
    interface RideCreateGameActionResult extends GameActionResult {
        /** The ride number. TODO reference? */
        readonly ride: number;
    }

    /** Arguments for the NetworkEvent hook callback. */
    interface NetworkEventArgs {
        /** The number of the player who executed the action. TODO always 0 in single player? */
        readonly player: number;
    }

    /** Arugments for the NetworkChatEvent hook callback. */
    interface NetworkChatEventArgs extends NetworkEventArgs {
        /** The message text. */
        message: string;
    }

    /** Arugments for the NetworkAuthenticateEvent hook callback. */
    interface NetworkAuthenticateEventArgs {
        readonly name: number;
        readonly ipAddress: string;
        readonly publicKeyHash: string;
        cancel: boolean;
    }

    /** Arugments for the RideRatingCalculate hook callback. */
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
         * How far through the month we are between 0 and 65536. This is incremented by 4 each tick, so
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
        /** The total size of the map. TODO in tiles? */
        readonly size: CoordsXY;
        /** The number of rides on the game map. */
        readonly numRides: number;
        /** The number of objects on the game map. */
        readonly numEntities: number;
        /** An array of all the rides on the map. */
        readonly rides: Ride[];

        /** Get the ride with the given ID. TODO what if doesn't exist?
         * 
         * @param id The ride id.
         * 
         * @return The ride. TODO what if doesn't exist?
         */
        getRide(id: number): Ride;

        /**
         * Get the tile at the given position. TODO what if OOB?
         * 
         * @param x The x-coordinate of the tile.
         * @param y The y-coordinate of the tile.
         * 
         * @return The game tile. TODO what if OOB?
         */
        getTile(x: number, y: number): Tile;

        /**
         * Get the object with the given id. TODO what if doesn't exist?
         * 
         * @param id The object id.
         * 
         * @return The object. TODO what if doesn't exist?
         */
        getEntity(id: number): Entity;

        /**
         * Get all map objects of the given type.
         * 
         * @param type The object type.
         * 
         * @return An array of objects.
         */
        getAllEntities(type: EntityType): Entity[];

        /**
         * Get all peep objects.
         * 
         * @param type The "peep" object type.
         * 
         * @return An array of Peeps.
         */
        getAllEntities(type: "peep"): Peep[];
    }

    /** Type of tile elements (e.g. surface, footpath, track). */
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
        /** Indicates if the tile is hidden. Take caution when changing this field, it may invalidate 
         * TileElements you have stored in your plugin. 
         */
        isHidden: boolean;
    }

    /**
     * A surface element.
     */
    interface SurfaceElement extends BaseTileElement {
        /** TODO what does the number mean? */
        slope: number;
        /** A number indicating the surface style. */
        surfaceStyle: number;
        /** A number indicating the edge style (blended or not?) TODO: verify */
        edgeStyle: number;
        /** The surface height of the water. */
        waterHeight: number;
        /** The length of the grass from X to Y. TODO */
        grassLength: number;
        /** A number from x to y indicating the ownership status. TODO */
        ownership: number;
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
        /** A number indicating the footpath type. TODO */
        footpathType: number;
        /** A number indicating the connection points on the path. TODO */
        edgesAndCorners: number;
        /** A number between x and y indicating the slope direction, or null for a flat path. TODO */
        slopeDirection: number | null;
        /** True if the path has a crossing which is occupied (e.g. by a train), false otherwise. */
        isBlockedByVehicle: boolean;
        /** TODO? Is this for better pathfinding? */
        isWide: boolean;

        /** Indicates if the path is a queue. */
        isQueue: boolean;
        /** A number between x and y indicating the queue banner direction, or null if no banner is present. TODO */
        queueBannerDirection: number | null;
        /** TODO? */
        ride: number;
        /** TODO? */
        station: number;

        /** TODO? */
        addition: number | null;
        /** TODO? */
        isAdditionBroken: boolean;
    }

    /**
     * A track element.
     */
    interface TrackElement extends BaseTileElement {
        /** A number indicating the track type. */
        trackType: number;
        /** TODO? */
        sequence: number;
        /** The id of the ride the track belongs to. */
        ride: number;
        /** TODO? */
        station: number;
        /** True if the track has a chain lift, false otherwise. */
        hasChainLift: boolean;
    }

    /**
     * A scenery element a single tile or smaller.
     */
    interface SmallSceneryElement extends BaseTileElement {
        /** TODO? */
        object: number;
        /** The number from x to y indicating the primary color. TODO */
        primaryColour: number;
        /** The number from x to y indicating the secondary color. TODO */
        secondaryColour: number;
    }

    /** An entrance (or exit?) TODO */
    interface EntranceElement extends BaseTileElement {
        /** TODO? */
        object: number;
        /** TODO? */
        sequence: number;
        /** The id of the ride the entrance/exit belongs to. */
        ride: number;
        /** The station number on the ride (x to y). TODO */
        station: number;
    }

    /** 
     * A wall element.
     */
    interface WallElement extends BaseTileElement {
        /** TODO? */
        object: number;
    }

    /**
     * A scenery element larger than one tile.
     */
    interface LargeSceneryElement extends BaseTileElement {
        /** TODO? */
        object: number;
        /** The number from x to y indicating the primary color. TODO */
        primaryColour: number;
        /** The number from x to y indicating the secondary color. TODO */
        secondaryColour: number;
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
        /**
         * The object type.
         */
        readonly type: ObjectType;

        /**
         * The index of the loaded object for the object type.
         */
        readonly index: number;

        /**
         * The unique identifier of the object, e.g. "rct2.burgb".
         * Only JSON objects will have an identifier.
         */
        readonly identifier: string;

        /**
         * The original unique identifier of the object, e.g. "BURGB   ".
         * This may have trailing spaces if the name is shorter than 8 characters.
         * Only .DAT objects or JSON objects based on .DAT objects will have legacy identifiers.
         */
        readonly legacyIdentifier: string;

        /**
         * The name in the user's current language.
         */
        readonly name: string;
    }

    /**
     * Represents the object definition of a ride or stall.
     */
    interface RideObject extends Object {
        /** The description of the ride / stall in the player's current language. */
        readonly description: string;
        /** A text description describing the capacity of the ride in the player's current language. */
        readonly capacity: string;
        /** The ride object flags. TODO */
        readonly flags: number;
        /** The ride type. TODO why is this an array */
        readonly rideType: number[];
        /** The minimum number of cars per train. TODO w/ or w/o cheats? */
        readonly minCarsInTrain: number;
        /** The maximum number of cars per train. TODO w/ or w/o cheats? */
        readonly maxCarsInTrain: number;
        /** The maximum (?) number of cars per flat ride. TODO */
        readonly carsPerFlatRide: number;
        /** TODO? */
        readonly zeroCars: number;
        /** The vehicle shown in the train tab preview? TODO */
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
        /** The max support height for the ride. TODO units */
        readonly maxHeight: number;
        /** If the ride is a shop, a number indicating the item sold. TODO on ride photos? */
        readonly shopItem: number;
        /** If the ride is a show, a number indicating the second item sold. TODO */
        readonly shopItemSecondary: number;
    }

    /**
     * Represents a defined vehicle within a Ride object definition.
     */
    interface RideObjectVehicle {
        readonly rotationFrameMask: number;
        readonly numVerticalFrames: number;
        readonly numHorizontalFrames: number;
        readonly spacing: number;
        /** The vehicle mass for a single car. TODO units */
        readonly carMass: number;
        readonly tabHeight: number;
        /** The number of seats per car. */
        readonly numSeats: number;
        readonly spriteFlags: number;
        readonly spriteWidth: number;
        readonly spriteHeightNegative: number;
        readonly spriteHeightPositive: number;
        /** The animation frame. TODO what if not animated */
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
        /** A number indicating the inertia (tendency to keep spinning) of a spinning vehicle. TODO */
        readonly spinningInertia: number;
        /** A number indicating the friction (tendency to stop spinning) of a spinning vehicle. TODO */
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
        /**
         * Raw bit flags that describe characteristics of the scenery item.
         */
        readonly flags: number;

        /**
         * The default clearance height of the scenery item.
         */
        readonly height: number;

        /**
         * How much the scenery item costs to build.
         */
        readonly price: number;

        /**
         * How much the scenery item costs to remove.
         */
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
        /** Whether the ride is open, closed or testing. */
        readonly status: RideStatus;
        /** Various flags related to the operation of the ride. */
        lifecycleFlags: number;
        /** The operation mode. TODO */
        mode: number;
        /** Flags related to how trains depart. */
        departFlags: number;
        /** The minimum time a train will wait at the station before departing. TODO units */
        minimumWaitingTime: number;
        /** The maximum time a train will wait at the station before departing. TODO units */
        maximumWaitingTime: number;
        /** The head vehicle IDs associated with the ride, one for each train. */
        readonly vehicles: number[];
        /** The track colour schemes for the ride. */
        colourSchemes: TrackColour[];
        /** The style used for the station, entrance, and exit building. TODO range */
        stationStyle: number;
        /** The music track to play at each station. TODO range */
        music: number;
        /** Information about each station. */
        readonly stations: RideStation[];
        /** The admission price for the ride and the price of the on-ride photo, or the cost of each item of the stall. TODO which one is [0] and [1] */
        price: number[];
        /** The total number of customers the ride has served since it was built. */
        totalCustomers: number;
        /** The date in months when the ride was built. Subtract this from `date.monthsElapsed` to get the age. */
        buildDate: number;
        /** How old the ride is in months. */
        readonly age: number;
        /** The running cost of the ride billed every fortnight. Multiply this by 16 to get the cost per hour (~1 in-game year). */
        runningCost: number;
        /** How often the ride should be inspected by a mechanic. TODO units */
        inspectionInterval: number;
        /** The value of the ride. TODO units? */
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

    /** A track color scheme. */
    interface TrackColour {
        /** The main color of the scheme. TODO value */
        main: number;
        /** The secondary color of the scheme. TODO value */
        additional: number;
        /** The support color of the scheme. TODO value */
        supports: number;
    }

    /** A color scheme used for a single vehicle. */
    interface VehicleColour {
        /** The body color of the vehicle. TODO value */
        body: number;
        /** The trim color of the vehicle. TODO value */
        trim: number;
        /** The third color of the vehicle. TODO value, also I think this is intended to be tertiary */
        ternary: number;
    }

    /**
     * Information associated with a ride station.
     */
    interface RideStation {
        /** The coordinate of the first tile of the station, in tiles. TODO start = has lights? */
        start: CoordsXYZ;
        /** The length of the station in tiles. */
        length: number;
        /** The coordinate of the station entrance, in tiles. */
        entrance: CoordsXYZD;
        /** The coorindate of the station exit, in tiles. */
        exit: CoordsXYZD;
    }

    /** The type of a entity that may move (car, duck, peep). */
    type EntityType =
        "car" | "duck" | "peep";

    /**
     * Represents an object "entity" on the map that can may move and has a sub-tile coordinate.
     */
    interface Entity {
        /** The entity index within the entity list. */
        readonly id: number;
        /** The type of entity, e.g. car, duck, litter, or peep. TODO litter actually isn't an option? */
        readonly type: EntityType;
        /** The x-coordinate of the entity in game units. */
        x: number;
        /** The y-coordinate of the entity in game units. */
        y: number;
        /** The z-coordinate of the entity in game units. */
        z: number;

        /**
         * Removes the entity from the map.
         * Warning: Removing vehicles and peeps that are on rides is currently unsupported.
         */
        remove(): void;
    }

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
        /** Colour of the guest's t-shirt. TODO range */
        tshirtColour: number;
        /** Colour of the guest's trousers. TODO range*/
        trousersColour: number; 
        /** Colour of the guest's balloon. TODO range, what if no balloon?*/
        balloonColour: number;
        /**Colour of the guest's hat. TODO range, what if no hat?*/
        hatColour: number;
        /**Colour of the guest's umbrella. TODO range, what if no umbrella?*/
        umbrellaColour: number;
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
        /** The mass of the guest. Affects vehicle mass. TODO units */
        mass: number;
        /** The guest's minimum preferred intensity between 0 and 15. */
        minIntensity: number;
        /** The guest's maximum preferred intensity between 0 and 15. */
        maxIntensity: number;
        /** The guest's tolerance to nauseating rides between 0 and 3. Lower numbers indicate lower tolerance. */
        nauseaTolerance: number;
        /** Amount of cash in the guest's pocket. TODO units */
        cash: number;
    }

    /**
     * Represents a staff member.
     */
    interface Staff extends Peep {
        /** The type of staff member (e.g. handyman, mechanic). */
        staffType: StaffType;
        /** Colour of the staff member. Not applicable for entertainers. TODO values*/
        colour: number;
        /** The entertainer's costume, only applicable for entertainers. TODO values*/
        costume: number;
        /** The enabled jobs the staff can do, e.g. sweep litter, water plants, inspect rides etc. TODO values*/
        orders: number;
    }

    /** The available staff types (e.g. handyman, mechanic). */
    type StaffType = "handyman" | "mechanic" | "security" | "entertainer";

    /**
     * Network APIs.
     * Use `network.status` to determine whether the current game is a client, server or in single player mode. TODO network.mode you mean?
     */
    interface Network {
        readonly mode: NetworkMode;
        readonly numGroups: number;
        readonly numPlayers: number;
        readonly groups: PlayerGroup[];
        readonly players: Player[];
        readonly currentPlayer: Player;
        defaultGroup: number;

        getServerInfo(): ServerInfo;
        addGroup(): void;
        getGroup(index: number): PlayerGroup;
        removeGroup(index: number): void;
        getPlayer(index: number): Player;
        kickPlayer(index: number): void;
        sendMessage(message: string): void;
        sendMessage(message: string, players: number[]): void;
    }

    type NetworkMode = "none" | "server" | "client";

    /**
     * Represents a player within a network game.
     */
    interface Player {
        readonly id: number;
        readonly name: string;
        group: number;
        readonly ping: number;
        readonly commandsRan: number;
        readonly moneySpent: number;
        readonly ipAddress: string;
        readonly publicKeyHash: string;
    }

    interface PlayerGroup {
        readonly id: number;
        name: string;
        permissions: PermissionType[];
    }

    interface ServerInfo {
        readonly name: string;
        readonly description: string;
        readonly greeting: string;
        readonly providerName: string;
        readonly providerEmail: string;
        readonly providerWebsite: string;
    }

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

    /**
     * The type of park message, including icon and behaviour.
     */
    type ParkMessageType =
        "attraction" | "peep_on_attraction" | "peep" | "money" | "blank" | "research" | "guests" | "award" | "chart";

    interface ParkMessage {
        /**
         * Whether the message has been shown and archived.
         */
        readonly isArchived: boolean;

        /**
         * The date this message was posted in total elapsed months.
         */
        month: number;

        /**
         * The day of the month this message was posted.
         */
        day: number;

        /**
         * How old the message is in number of ticks.
         */
        tickCount: number;

        /**
         * The format of the message such as the icon and whether location is enabled.
         */
        type: ParkMessageType;

        /**
         * The actual message content.
         */
        text: string;

        /**
         * Ride ID for attraction.
         * Entity ID for peep_on_attraction or peep.
         * Researched item for research.
         */
        subject?: number;

        /**
         * Removes the message.
         */
        remove(): void;
    }

    interface ParkMessageDesc  {
        type: ParkMessageType;
        text: string;
        subject?: number;
    }

    interface Park {
        cash: number;
        rating: number;
        bankLoan: number;
        maxBankLoan: number;
        messages: ParkMessage[];

        postMessage(message: string): void;
        postMessage(message: ParkMessageDesc): void;
    }

    interface Cheats {
        allowArbitraryRideTypeChanges: boolean;
        allowTrackPlaceInvalidHeights: boolean;
        buildInPauseMode: boolean;
        disableAllBreakdowns: boolean;
        disableBrakesFailure: boolean;
        disableClearanceChecks: boolean;
        disableLittering: boolean;
        disablePlantAging: boolean;
        disableRideValueAging: boolean;
        disableSupportLimits: boolean;
        disableTrainLengthLimit: boolean;
        disableVandalism: boolean;
        enableAllDrawableTrackPieces: boolean;
        enableChainLiftOnAllTrack: boolean;
        fastLiftHill: boolean;
        freezeWeather: boolean;
        ignoreResearchStatus: boolean;
        ignoreRideIntensity: boolean;
        neverendingMarketing: boolean;
        sandboxMode: boolean;
        showAllOperatingModes: boolean;
        showVehiclesFromOtherTrackTypes: boolean;
    }

    /**
     * User Interface APIs
     * These will only be available to servers and clients that are not running headless mode.
     * Plugin writers should check if ui is available using `typeof ui !== 'undefined'`.
     */
    interface Ui {
        readonly width: number;
        readonly height: number;
        readonly windows: number;
        readonly mainViewport: Viewport;
        readonly tileSelection: TileSelection;
        readonly tool: Tool;

        getWindow(id: number): Window;
        getWindow(classification: string): Window;
        openWindow(desc: WindowDesc): Window;
        closeWindows(classification: string, id?: number): void;
        closeAllWindows(): void;

        /**
         * Show a red error box.
         * @param title The title / first line of the box.
         * @param message The message / second line of the box.
         */
        showError(title: string, message: string): void;

        /**
         * Shows a text input prompt and calls the given callback when entered.
         * @param desc The parameters for the text input window.
         */
        showTextInput(desc: TextInputDesc): void;

        /**
         * Begins a new tool session. The cursor will change to the style specified by the
         * given tool descriptor and cursor events will be provided.
         * @param tool The properties and event handlers for the tool.
         */
        activateTool(tool: ToolDesc): void;

        registerMenuItem(text: string, callback: () => void): void;
    }

    /**
     * Parameters for the text input window.
     */
    interface TextInputDesc {
        /**
         * The title of the text input window.
         */
        title: string;

        /**
         * The description to show above the text box.
         */
        description: string;

        /**
         * The current value of the text box.
         */
        initialValue?: string;

        /**
         * The maximum length the value can be.
         */
        maxLength?: number;

        /**
         * The function to call when the user has entered a new value and pressed OK.
         */
        callback: (value: string) => void;
    }

    interface TileSelection {
        range: MapRange;
        tiles: CoordsXY[];
    }

    interface Tool {
        id: string;
        cursor: CursorType;

        cancel: () => void;
    }

    interface ToolEventArgs {
        readonly isDown: boolean;
        readonly screenCoords: ScreenCoordsXY;
        readonly mapCoords?: CoordsXYZ;
        readonly tileElementIndex?: number;
        readonly entityId?: number;
    }

    /**
     * Describes the properties and event handlers for a custom tool.
     */
    interface ToolDesc {
        id: string;
        cursor?: CursorType;

        onStart: () => void;
        onDown: (e: ToolEventArgs) => void;
        onMove: (e: ToolEventArgs) => void;
        onUp: (e: ToolEventArgs) => void;
        onFinish: () => void;
    }

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

    interface Widget {
        type: WidgetType;
        x: number;
        y: number;
        width: number;
        height: number;
        name?: string;
        tooltip?: string;
        isDisabled?: boolean;
    }

    interface ButtonWidget extends Widget {
        /**
         * Whether the button has a 3D border.
         * By default, text buttons have borders and image buttons do not but it can be overridden.
         */
        border?: boolean;
        image: number;
        isPressed: boolean;
        text: string;
        onClick: () => void;
    }

    interface CheckboxWidget extends Widget {
        text: string;
        isChecked: boolean;
        onChange: (isChecked: boolean) => void;
    }

    interface DropdownWidget extends Widget {
        items: string[];
        selectedIndex: number;
        onChange: (index: number) => void;
    }

    interface LabelWidget extends Widget {
        text: string;
        onChange: (index: number) => void;
    }

    type SortOrder = "none" | "ascending" | "descending";

    type ScrollbarType = "none" | "horizontal" | "vertical" | "both";

    interface ListViewColumn {
        canSort?: boolean;
        sortOrder?: SortOrder;
        header?: string;
        headerTooltip?: string;
        width?: number;
        ratioWidth?: number;
        minWidth?: number;
        maxWidth?: number;
    }

    type ListViewItem = string[];

    interface RowColumn {
        row: number;
        column: number;
    }

    interface ListView extends Widget {
        scrollbars?: ScrollbarType;
        isStriped?: boolean;
        showColumnHeaders?: boolean;
        columns?: ListViewColumn[];
        items?: string[] | ListViewItem[];
        selectedCell?: RowColumn;
        readonly highlightedCell?: RowColumn;
        canSelect?: boolean;

        onHighlight: (item: number, column: number) => void;
        onClick: (item: number, column: number) => void;
    }

    interface SpinnerWidget extends Widget {
        text: string;
        onDecrement: () => void;
        onIncrement: () => void;
    }

    interface ViewportWidget extends Widget {
        viewport: Viewport
    }

    interface Window {
        classification: number;
        number: number;
        x: number;
        y: number;
        width: number;
        height: number;
        minWidth: number;
        maxWidth: number;
        minHeight: number;
        maxHeight: number;
        isSticky: boolean;
        colours: number[];
        title: string;
        widgets: Widget[];
        tabIndex: number;

        close(): void;
        bringToFront(): void;
        findWidget<T extends Widget>(name: string): T;
    }

    interface WindowDesc {
        classification: string;
        x?: number;
        y?: number;
        width: number;
        height: number;
        title: string;
        id?: number;
        minWidth?: number;
        minHeight?: number;
        maxWidth?: number;
        maxHeight?: number;
        widgets?: Widget[];
        colours?: number[];
        tabs?: WindowTabDesc[];

        onClose?: () => void;
        onUpdate?: () => void;
        tabChange?: () => void;
    }

    interface ImageAnimation {
        frameBase: number;
        frameCount?: number;
        frameDuration?: number;
        offset?: ScreenCoordsXY;
    }

    interface WindowTabDesc {
        image: number | ImageAnimation;
        widgets?: Widget[];
    }

    interface Viewport {
        left: number;
        top: number;
        right: number;
        bottom: number;
        rotation: number;
        zoom: number;
        visibilityFlags: number;

        getCentrePosition(): CoordsXY;
        moveTo(position: CoordsXY | CoordsXYZ): void;
        scrollTo(position: CoordsXY | CoordsXYZ): void;
    }
}
