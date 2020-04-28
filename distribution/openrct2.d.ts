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
     * A coordinate within the game.
     * Each in-game tile is a size of 32x32.
     */
    interface Coord2 {
        x: number;
        y: number;
    }

    /**
     * A coordinate within the game.
     * Each in-game tile is a size of 32x32.
     * The z-coordinate raises 16 per land increment. A full-height wall is 32 in height.
     */
    interface Coord3 extends Coord2 {
        z: number;
    }

    /**
     * Represents information about the plugin such as type, name, author and version.
     * It also includes the entry point.
     */
    interface PluginMetadata {
        name: string;
        version: string;
        authors: string | string[];
        type: PluginType;
        minApiVersion?: number;
        main: () => void;
    }

    /**
     * Console APIs
     * Currently interact with stdout.
     */
    interface Console {
        clear(): void;
        log(message?: any, ...optionalParams: any[]): void;
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
         * Gets the loaded object at the given index.
         * @param type The object type.
         * @param index The index.
         */
        getObject(type: ObjectType, index: number): Object;
        getObject(type: "ride", index: number): RideObject;
        getObject(type: "small_scenery", index: number): SmallSceneryObject;

        getAllObjects(type: ObjectType): Object[];
        getAllObjects(type: "ride"): RideObject[];

        /**
         * Gets a random integer within the specified range using the game's pseudo-
         * random number generator. This is part of the game state and shared across
         * all clients, you therefore must be in a context that can mutate the game
         * state. Use this to generate random numbers instead of Math.Random during
         * game logic routines such as hooks and game actions.
         * @param min The minimum value inclusive.
         * @param max The maximum value exclusive.
         */
        getRandom(min: number, max: number): number;

        /**
         * Registers a new game action that allows clients to interact with the game.
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
         * @param action The name of the action.
         * @param args The action parameters.
         * @param callback The function to be called with the result of the action.
         */
        queryAction(action: string, args: object, callback: (result: GameActionResult) => void): void;

        /**
         * Executes a game action. In a network game, this will send a request to the server and wait
         * for the server to reply.
         * @param action The name of the action.
         * @param args The action parameters.
         * @param callback The function to be called with the result of the action.
         */
        executeAction(action: string, args: object, callback: (result: GameActionResult) => void): void;

        /**
         * Subscribes to the given hook.
         */
        subscribe(hook: HookType, callback: Function): IDisposable;

        subscribe(hook: "action.query", callback: (e: GameActionEventArgs) => void): IDisposable;
        subscribe(hook: "action.execute", callback: (e: GameActionEventArgs) => void): IDisposable;
        subscribe(hook: "interval.tick", callback: () => void): IDisposable;
        subscribe(hook: "interval.day", callback: () => void): IDisposable;
        subscribe(hook: "network.chat", callback: (e: NetworkChatEventArgs) => void): IDisposable;
        subscribe(hook: "network.authenticate", callback: (e: NetworkAuthenticateEventArgs) => void): IDisposable;
        subscribe(hook: "network.join", callback: (e: NetworkEventArgs) => void): IDisposable;
        subscribe(hook: "network.leave", callback: (e: NetworkEventArgs) => void): IDisposable;
    }

    interface Configuration {
        getAll(namespace: string): { [name: string]: any };
        get<T>(key: string): T | undefined;
        get<T>(key: string, defaultValue: T): T;
        set<T>(key: string, value: T): void;
        has(key: string): boolean;
    }

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

    type HookType =
        "interval.tick" | "interval.day" |
        "network.chat" | "network.action" | "network.join" | "network.leave";

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

    interface GameActionEventArgs {
        readonly player: number;
        readonly type: string;
        readonly isClientOnly: boolean;
        result: GameActionResult;
    }

    interface GameActionResult {
        error?: string;
        errorTitle?: string;
        errorMessage?: string;
        position?: Coord3;
        cost?: number;
        expenditureType?: ExpenditureType;
    }

    interface RideCreateGameActionResult extends GameActionResult {
        readonly ride: number;
    }

    interface RideCreateActionEventArgs extends GameActionEventArgs {
        readonly rideType: number;
        readonly rideObject: number;
        result: RideCreateGameActionResult;
    }

    interface NetworkEventArgs {
        readonly player: number;
    }

    interface NetworkChatEventArgs extends NetworkEventArgs {
        message: string;
    }

    interface NetworkAuthenticateEventArgs {
        readonly name: number;
        readonly ipAddress: string;
        readonly publicKeyHash: string;
        cancel: boolean;
    }

    /**
     * APIs for the in-game date.
     */
    interface GameDate {
        /**
         * The total number of ticks that have elapsed since the beginning of the game / scenario. This
         * should never reset.
         */
        readonly ticksElapsed: number;
        /**
         * The total number of months that have elapsed. This will equate to 16 on 1st March, Year 2.
         * Note: this represents the current date and may be reset by cheats or scripts.
         */
        monthsElapsed: number;
        /**
         * The total number of years that have elapsed. This always equates to (monthsElapsed / 8).
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
        readonly size: Coord2;
        readonly numRides: number;
        readonly numEntities: number;
        readonly rides: Ride[];

        getRide(id: number): Ride;
        getTile(x: number, y: number): Tile;
        getEntity(id: number): Entity;
        getAllEntities(type: EntityType);
    }

    type TileElementType =
        "surface" | "footpath" | "track" | "small_scenery" | "wall" | "entrance" | "large_scenery" | "banner";

    interface BaseTileElement {
        type: TileElementType;
        baseHeight: number;
        clearanceHeight: number;
    }

    interface SurfaceElement extends BaseTileElement {
        slope: number;
        surfaceStyle: number;
        edgeStyle: number;
        waterHeight: number;
        grassLength: number;
        ownership: number;
        parkFences: number;

        readonly hasOwnership: boolean;
        readonly hasConstructionRights: boolean;
    }

    interface FootpathElement extends BaseTileElement {
        footpathType: number;
        edgesAndCorners: number;
        slopeDirection: number | null;
        isBlockedByVehicle: boolean;
        isWide: boolean;

        isQueue: boolean;
        queueBannerDirection: number | null;
        ride: number;
        station: number;

        addition: number | null;
        isAdditionBroken: boolean;
    }

    interface TrackElement extends BaseTileElement {
        trackType: number;
        sequence: number;
        ride: number;
        station: number;
        hasChainLift: boolean;
    }

    interface SmallSceneryElement extends BaseTileElement {
        object: number;
        primaryColour: number;
        secondaryColour: number;
    }

    interface EntranceElement extends BaseTileElement {
        object: number;
        sequence: number;
        ride: number;
        station: number;
    }

    interface WallElement extends BaseTileElement {
        object: number;
    }

    interface LargeSceneryElement extends BaseTileElement {
        object: number;
        primaryColour: number;
        secondaryColour: number;
    }

    interface BannerElement extends BaseTileElement {
    }

    interface CorruptElement extends BaseTileElement {
    }

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
        /** Gets an array of all the tile elements on this tile. */
        readonly elements: TileElement[];
        /** Gets the number of tile elements on this tile. */
        readonly numElements: number;
        /**
         * Gets or sets the raw data for this tile.
         * This can provide more control and efficiency for tile manipulation but requires
         * knowledge of tile element structures and may change between versions of OpenRCT2.
         */
        data: Uint8Array;

        /** Gets the tile element at the given index on this tile. */
        getElement(index: number): TileElement;
        /** Gets the tile element at the given index on this tile. */
        getElement<T extends BaseTileElement>(index: number): T;
        /** Inserts a new tile element at the given index on this tile. */
        insertElement(index: number): TileElement;
        /** Removes the tile element at the given index from this tile. */
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
         * The unique name identifier of the object, e.g. "BURGB   ".
         * This may have trailing spaces if the name is shorter than 8 characters.
         */
        readonly identifier: string;
        /**
         * The name in the user's current language.
         */
        readonly name: string;
    }

    /**
     * Represents the object definition of a ride or stall.
     */
    interface RideObject extends Object {
        /**
         * The description of the ride / stall in the player's current language.
         */
        readonly description: string;
        /**
         * A text description describing the capacity of the ride in the player's current language.
         */
        readonly capacity: string;
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
        /**
         * The object metadata for this ride.
         */
        readonly object: RideObject;
        /**
         * The unique ID / index of the ride.
         */
        readonly id: number;
        /**
         * The type of the ride represented as the internal built-in ride type ID.
         */
        type: number;
        /**
         * The generated or custom name of the ride.
         */
        name: string;
        /**
         * The excitement metric of the ride represented as a 2 decimal point fixed integer.
         * For example, `652` equates to `6.52`.
         */
        excitement: number;
        /**
         * The intensity metric of the ride represented as a 2 decimal point fixed integer.
         * For example, `652` equates to `6.52`.
         */
        intensity: number;
        /**
         * The nausea metric of the ride represented as a 2 decimal point fixed integer.
         * For example, `652` equates to `6.52`.
         */
        nausea: number;
        /**
         * The total number of customers the ride has served since it was built.
         */
        totalCustomers: number;
    }

    type EntityType =
        "car" | "duck" | "peep";

    /**
     * Represents an object "entity" on the map that can typically moves and has a sub-tile coordinate.
     */
    interface Entity {
        /**
         * The type of entity, e.g. car, duck, litter, or peep.
         */
        readonly type: EntityType;
        /**
         * The x-coordinate of the entity in game units.
         */
        x: number;
        /**
         * The y-coordinate of the entity in game units.
         */
        y: number;
        /**
         * The z-coordinate of the entity in game units.
         */
        z: number;
    }

    /**
     * Represents a guest or staff member.
     */
    interface Peep extends Entity {
        /**
         * Colour of the peep's t-shirt.
         */
        tshirtColour: number;
        /**
         * Colour of the peep's trousers.
         */
        trousersColour: number;
    }

    /**
     * Network APIs
     * Use `network.status` to determine whether the current game is a client, server or in single player mode.
     */
    interface Network {
        readonly mode: NetworkMode;
        readonly numGroups: number;
        readonly numPlayers: number;
        readonly groups: PlayerGroup[];
        readonly players: Player[];
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
        type: ParkMessageType;
        text: string;
    }

    interface Park {
        cash: number;
        rating: number;
        bankLoan: number;
        maxBankLoan: number;

        postMessage(message: string): void;
        postMessage(message: ParkMessage): void;
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

        getWindow(id: number): Window;
        getWindow(classification: string): Window;
        openWindow(desc: WindowDesc): Window;
        closeWindows(classification: string, id?: number): void;
        closeAllWindows(): void;

        registerMenuItem(text: string, callback: () => void): void;
    }

    /**
     * Represents the type of a widget, e.g. button or label.
     */
    type WidgetType =
        "button" | "checkbox" | "dropdown" | "groupbox" | "label" | "spinner" | "tabview" | "viewport";

    interface Widget {
        type: WidgetType;
        x: number;
        y: number;
        width: number;
        height: number;
        name?: string;
        isDisabled?: boolean;
    }

    interface ButtonWidget extends Widget {
        image: number;
        text: string;
        onClick: () => void;
    }

    interface CheckboxWidget extends Widget {
        text: string;
        isChecked: boolean;
        onChanged: (isChecked: boolean) => void;
    }

    interface DropdownWidget extends Widget {
        items: string[];
        selectedIndex: number;
        onChanged: (index: number) => void;
    }

    interface LabelWidget extends Widget {
        text: string;
        onChanged: (index: number) => void;
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
        isSticky: boolean;
        colours: number[];
        title: string;
        widgets: Widget[];

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
        widgets?: Widget[];
        colours?: number[];

        onClose?: () => void;
    }

    interface Viewport {
        left: number;
        top: number;
        right: number;
        bottom: number;
        rotation: number;
        zoom: number;
        visibilityFlags: number;

        getCentrePosition(): Coord2;
        moveTo(position: Coord2 | Coord3): void;
        scrollTo(position: Coord2 | Coord3): void;
    }
}
