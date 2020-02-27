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
    interface PluginMetadata {
        name: string;
        version: string;
        authors: string | string[];
        type: PluginType;
        minApiVersion?: number;
        main: () => void;
    }

    interface Console {
        clear(): void;
        log(message?: any, ...optionalParams: any[]): void;
    }

    interface Configuration {
        getAll(pattern: string): { [name: string]: any };
        get<T>(key: string): T | undefined;
        get<T>(key: string, defaultValue: T): T;
        set<T>(key: string, value: T): void;
        has(key: string): boolean;
    }

    interface Coord2 {
        x: number;
        y: number;
    }

    interface Coord3 {
        x: number;
        y: number;
        z: number;
    }

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

    interface GameActionResult {
        error: string;
        errorTitle: string;
        errorMessage: string;
        position: Coord3;
        cost: number;
        expenditureType: ExpenditureType;
    }

    interface GameActionDesc {
        id: string;
        query: (args: object) => GameActionResult;
        execute: (args: object) => GameActionResult;
    }

    interface NetworkEventArgs {
        readonly player: number;
    }

    interface NetworkActionEventArgs extends NetworkEventArgs {
        readonly type: string;
        result: GameActionResult;
    }

    interface NetworkChatEventArgs extends NetworkEventArgs {
        message: string;
    }

    interface Context {
        /**
         * The user's current configuration.
         */
        configuration: Configuration;

        /**
         * Shared generic storage for all plugins.
         */
        sharedStorage: Configuration;

        /**
         * Local generic storage for a each plugin.
         */
        localStorage: Configuration;

        /**
         * Registers a new intent (command) that can be mapped to a shortcut.
         */
        registerIntent(desc: IntentDesc): void;

        /**
         * Registers a new game action that allows clients to interact with the game.
         */
        registerGameAction(desc: GameActionDesc): void;

        /**
         * Subscribes to the given hook.
         */
        subscribe(hook: HookType, callback: Function): IDisposable;

        subscribe(hook: "interval.tick", callback: () => void): IDisposable;
        subscribe(hook: "interval.day", callback: () => void): IDisposable;
        subscribe(hook: "network.action", callback: (e: NetworkActionEventArgs) => void): IDisposable;
        subscribe(hook: "network.chat", callback: (e: NetworkChatEventArgs) => void): IDisposable;
        subscribe(hook: "network.join", callback: (e: NetworkEventArgs) => void): IDisposable;
        subscribe(hook: "network.leave", callback: (e: NetworkEventArgs) => void): IDisposable;
    }

    interface IntentDesc
    {
        key: string;
        title?: string;
        shortcut?: string;
        action: Function;
    }

    interface IDisposable {
        dispose(): void;
    }

    type TileElementType =
        "surface" | "footpath" | "track" | "small_scenery" | "entrance" | "large_scenery" | "banner";

    interface BaseTileElement {
        type: TileElementType;
        baseHeight: number;
        clearanceHeight: number;
        broken: boolean;
    }

    // interface TileElement extends BaseTileElement {
    //     /**
    //      * Gets the element as a specific type to access its properties
    //      */
    //     asSurface(): SurfaceElement | null;
    //     asFootpath(): FootpathElement | null;
    //     asTrack(): TrackElement | null;
    //     asSmallScenery(): SmallSceneryElement | null;
    //     asEntrance(): EntranceElement | null;
    //     asWall(): WallElement | null;
    //     asLargeScenery(): LargeSceneryElement | null;
    //     asBanner(): BannerElement | null;
    //     asCorruptElement(): CorruptElement | null;
    // }

    type TileElement = SurfaceElement | FootpathElement | TrackElement;

    interface SurfaceElement extends BaseTileElement {
        slope: number;
        terrain: number;
        waterHeight: number;
        grassLength: number;
        ownership: number;
        parkFences: number;
    }

    interface FootpathAdditionStatus extends BaseTileElement {
        north: number;
        east: number;
        south: number;
        west: number;
    }

    interface FootpathAddition extends BaseTileElement {
        isBin: boolean;
        isBench: boolean;
        isLamp: boolean;
        isBreakable: boolean;
        isJumpingFountainWater: boolean;
        isJumpingFountainSnow: boolean;
        allowedOnQueue: boolean;
        allowedOnSlope: boolean;
        isQueueScreen: boolean;
        status: FootpathAdditionStatus;

        /**
         * Remove the path addition
         */
        remove(): void;
    }

    interface FootpathElement extends BaseTileElement {
        footpathType: number;
        isSloped: boolean;
        isQueue: boolean;
        addition: FootpathAddition;
        edges: number;
        corners: number;
        rideIndex: number;
    }

    interface TrackElement extends BaseTileElement {
    }

    interface SmallSceneryElement extends BaseTileElement {
    }

    interface EntranceElement extends BaseTileElement {
    }

    interface WallElement extends BaseTileElement {
    }

    interface LargeSceneryElement extends BaseTileElement {
    }

    interface BannerElement extends BaseTileElement {
    }

    interface CorruptElement extends BaseTileElement {
    }

    interface Tile {
        readonly x: number;
        readonly y: number;
        elements: TileElement[];

        getElement(index: number): TileElement;
    }

    interface Object {
        readonly identifier: string;
        readonly name: string;
    }

    interface RideObject extends Object {
        readonly description: string;
        readonly capacity: string;
    }

    interface Ride {
        readonly object: RideObject;
        readonly id: number;
        type: number;
        name: string;
        excitement: number;
        intensity: number;
        nausea: number;
        totalCustomers: number;
    }

    type ThingType =
        "car" | "duck" | "peep";

    interface Thing {
        readonly type: ThingType;
        x: number;
        y: number;
        z: number;

        asPeep(): Thing | null;
    }

    interface Peep extends Thing {
        tshirt: number;
        trousers: number;
    }

    interface GameMap {
        readonly size: { x: number; y: number; };
        readonly rides: number;
        readonly things: number;

        getRide(id: number): Ride;
        getTile(x: number, y: number): Tile;
        getThing(id: number): Thing;
    }

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

    /**
     * Represents the type of a widget, e.g. button or label.
     */
    type WidgetType =
        "button" | "checkbox" | "dropdown" | "groupbox" | "label" | "tabview" | "viewport";

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
        isChecked: number;
        onChanged: (isChecked: boolean) => void;
    }

    interface DropdownWidget extends Widget {
        items: string[];
        selectedIndex: number;
        onChanged: (index: number) => void;
    }

    interface LabelWidget extends Widget {
        text: string;
    }

    interface ViewportWidget extends Widget {
        viewport: Viewport
    }

    interface Tab {
        image: number;
        tooltip: string;
        widgets: Widget[];
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

    interface TabbedWindow extends Window {
        tabs: Tab[];
        activeTabIndex: number;

        onTabChanged: (index: number) => void;
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
        tabs?: Tab[];

        onClose?: () => void;
    }

    type ToolCallbackType = "move" | "press" | "release";

    interface ToolCallback {
        type: ToolCallbackType;
        tiles: Coord2[];
    }

    type CursorType =
        "arrow" |
        "blank" |
        "up_arrow" |
        "up_down_arrow" |
        "hand_point" |
        "zzz" |
        "diagonal_arrows" |
        "picker" |
        "tree_down" |
        "fountain_down" |
        "statue_down" |
        "bench_down" |
        "cross_hair" |
        "bin_down" |
        "lamppost_down" |
        "fence_down" |
        "flower_down" |
        "path_down" |
        "dig_down" |
        "water_down" |
        "house_down" |
        "volcano_down" |
        "walk_down" |
        "paint_down" |
        "entrance_down" |
        "hand_open" |
        "hand_closed";

    interface ToolDesc {
        id: string;
        cursor: CursorType;
        width: number;
        height: number;
        callback: (e: ToolCallback) => void;
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

    interface Ui {
        readonly width: number;
        readonly height: number;
        readonly windows: number;
        readonly mainViewport: Viewport;

        getWindow(id: number): Window;
        getWindow(classification: string, id?: number): Window;
        openWindow(desc: WindowDesc): Window;
        closeWindows(classification: string, id?: number): void;
        closeAllWindows(): void;

        activateTool(options: ToolDesc): IDisposable;
        registerMenuItem(text: string, callback: () => void): void;
    }

    /**
     * Network APIs
     * Use `network.status` to determine whether the current game is a client, server or in single player mode.
     */

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

    type NetworkMode = "none" | "server" | "client";

    interface Network {
        readonly mode: NetworkMode;
        readonly groups: number;
        readonly players: number;
        defaultGroup: number;

        getServerInfo(): ServerInfo;
        getGroup(index: number): PlayerGroup;
        setGroups(groups: PlayerGroup[]): void;
        getPlayer(index: number): Player;
        kickPlayer(index: number): void;
        sendMessage(message: string): void;
        sendMessage(message: string, players: number[]): void;
        sendQueryAction(action: string, args: object, callback: (result: GameActionResult) => void): void;
        sendExecuteAction(action: string, args: object, callback: (result: GameActionResult) => void): void;
    }

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
     * Global context for accessing all other APIs.
     */
    var console: Console;
    var context: Context;
    var date: GameDate;
    var map: GameMap;
    var network: Network;
    var park: Park;
    var ui: Ui;

    /**
     * Registers the plugin. This only only be called once.
     * @param metadata Information about the plugin and the entry point.
     */
    function registerPlugin(metadata: PluginMetadata): void;
}
