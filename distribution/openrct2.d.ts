/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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

export type PluginType = "server" | "client" | "server_client";

export interface PluginMetadata {
    name: string;
    version: string;
    authors: string | string[];
    type: PluginType;
    minApiVersion: number;
    main: () => void;
}

export interface Console {
    clear(): void;
    log(message?: any, ...optionalParams: any[]): void;
}

export interface Configuration {
    getAll(pattern: string): { [name: string]: any };
    get<T>(key: string): T | undefined;
    get<T>(key: string, defaultValue: T): T;
    set<T>(key: string, value: T): void;
    has(key: string): boolean;
}

export interface Coord2 {
    x: number;
    y: number;
}

export interface Coord3 {
    x: number;
    y: number;
    z: number;
}

export type HookType =
    "interval.tick" | "interval.day" |
    "network.chat" | "network.action" | "network.join" | "network.leave";

export type ExpenditureType =
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

export interface GameActionResult {
    error: string;
    errorTitle: string;
    errorMessage: string;
    position: Coord3;
    cost: number;
    expenditureType: ExpenditureType;
}

export interface GameActionDesc {
    id: string;
    query: (args: object) => GameActionResult;
    execute: (args: object) => GameActionResult;
}

export interface NetworkEventArgs {
    readonly player: number;
}

export interface NetworkActionEventArgs extends NetworkEventArgs {
    readonly type: string;
    result: GameActionResult;
}

export interface NetworkChatEventArgs extends NetworkEventArgs {
    message: string;
}

export interface Context {
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

export interface IntentDesc
{
    key: string;
    title?: string;
    shortcut?: string;
    action: Function;
}

export interface IDisposable {
    dispose(): void;
}

export type TileElementType =
    "surface" | "footpath" | "track" | "small_scenery" | "entrance" | "large_scenery" | "banner";

export interface BaseTileElement {
    type: TileElementType;
    baseHeight: number;
    clearanceHeight: number;
    broken: boolean;
}

// export interface TileElement extends BaseTileElement {
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

export type TileElement = SurfaceElement | FootpathElement | TrackElement;

export interface SurfaceElement extends BaseTileElement {
    slope: number;
    terrain: number;
    waterHeight: number;
    grassLength: number;
    ownership: number;
    parkFences: number;
}

export interface FootpathAdditionStatus extends BaseTileElement {
    north: number;
    east: number;
    south: number;
    west: number;
}

export interface FootpathAddition extends BaseTileElement {
    type: number;
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

export interface FootpathElement extends BaseTileElement {
    footpathType: number;
    isSloped: boolean;
    isQueue: boolean;
    addition: FootpathAddition;
    edges: number;
    corners: number;
    rideIndex: number;
}

export interface TrackElement extends BaseTileElement {
}

export interface SmallSceneryElement extends BaseTileElement {
}

export interface EntranceElement extends BaseTileElement {
}

export interface WallElement extends BaseTileElement {
}

export interface LargeSceneryElement extends BaseTileElement {
}

export interface BannerElement extends BaseTileElement {
}

export interface CorruptElement extends BaseTileElement {
}

export interface Tile {
    readonly x: number;
    readonly y: number;
    elements: TileElement[];

    getElement(index: number): TileElement;
}

export interface Object {
    readonly identifier: string;
    readonly name: string;
}

export interface RideObject extends Object {
    readonly description: string;
    readonly capacity: string;
}

export interface Ride {
    readonly object: RideObject;
    readonly id: number;
    type: string;
    name: string;
    excitement: number;
    intensity: number;
    nausea: number;
    totalCustomers: number;
}

export type ThingType =
    "car" | "duck" | "peep";

export interface Thing {
    readonly type: ThingType;
    x: number;
    y: number;
    z: number;

    asPeep(): Thing | null;
}

export interface Peep extends Thing {
    tshirt: number;
    trousers: number;
}

export interface Map {
    readonly size: { x: number; y: number; };
    readonly rides: number;
    readonly things: number;

    getRide(id: number): Ride;
    getTile(x: number, y: number): Tile;
    getThing(id: number): Thing;
}

export type ParkMessageType =
    "attraction" | "peep_on_attraction" | "peep" | "money" | "blank" | "research" | "guests" | "award" | "chart";

export interface ParkMessage {
    type: ParkMessageType;
    text: string;
}

export interface Park {
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
export type WidgetType =
    "button" | "checkbox" | "dropdown" | "groupbox" | "label" | "tabview" | "viewport";

export interface Widget {
    type: WidgetType;
    x: number;
    y: number;
    width: number;
    height: number;
    isDisabled: boolean;
}

export interface ButtonWidget extends Widget {
    text: string;
    onClick: () => void;
}

export interface CheckboxWidget extends Widget {
    text: string;
    isChecked: number;
    onChanged: (isChecked: boolean) => void;
}

export interface DropdownWidget extends Widget {
    items: string[];
    selectedIndex: number;
    onChanged: (index: number) => void;
}

export interface LabelWidget extends Widget {
    text: string;
}

export interface ViewportWidget extends Widget {
    target: null | number | Coord3;
    zoom: number;
}

export interface Tab {
    image: number;
    tooltip: string;
    widgets: Widget[];
}

export interface Window {
    id: number;
    classification: string;
    x: number;
    y: number;
    width: number;
    height: number;
    isSticky: boolean;
    colours: number[];
    title: string;
    widgets: Widget[];

    close(): void;
}

export interface TabbedWindow extends Window {
    tabs: Tab[];
    activeTabIndex: number;

    onTabChanged: (index: number) => void;
}

export interface WindowDesc {
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
}

export type ToolCallbackType = "move" | "press" | "release";

export interface ToolCallback {
    type: ToolCallbackType;
    tiles: Coord2[];
}

export type CursorType =
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

export interface ToolDesc {
    id: string;
    cursor: CursorType;
    width: number;
    height: number;
    callback: (e: ToolCallback) => void;
}

export interface Viewport {
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

export interface Ui {
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
export interface Player {
    readonly id: number;
    readonly name: string;
    group: number;
    readonly ping: number;
    readonly commandsRan: number;
    readonly moneySpent: number;
}

export type PermissionType =
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

export interface PlayerGroup {
    readonly id: number;
    name: string;
    permissions: PermissionType[];
}

export interface ServerInfo {
    readonly name: string;
    readonly description: string;
    readonly greeting: string;
    readonly providerName: string;
    readonly providerEmail: string;
    readonly providerWebsite: string;
}

export type NetworkMode = "none" | "server" | "client";

export interface Network {
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

/**
 * Global context for accessing all other APIs.
 */
declare global {
    var console: Console;
    var context: Context;
    var map: Map;
    var network: Network;
    var park: Park;
    var ui: Ui;
}
