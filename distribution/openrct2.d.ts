/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

// OpenRCT2 Scripting API definition file

export interface Console {
    log(message?: any, ...optionalParams: any[]): void;
}

export interface Configuration {
    getAll(pattern: string): { [name: string]: any };
    get<T>(key: string): T | undefined;
    get<T>(key: string, defaultValue: T): T;
    set<T>(key: string, value: T): void;
    has(key: string): boolean;
}

export interface Context {
    /**
     * The user's current configuration.
     */
    configuration: Configuration;
    /**
     * Subscribes to the given hook.
     */
    subscribe: (hook: string, callback: Function) => void;
}

export interface Ride {
    name: string;
    excitement: number;
    intensity: number;
    nausea: number;
    totalCustomers: number;
}

export interface Map {
    getRide(id: number): Ride;
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

    postMessage(message: ParkMessage): void;
}

export interface Window {
    id: number;
    classification: string;
    x: number;
    y: number;
    width: number;
    height: number;
    title: string;
}

export interface WindowDesc {
    classification: string;
    x?: number;
    y?: number;
    width: number;
    height: number;
    title: string;
    id?: number;
}

export interface Ui {
    openWindow(desc: WindowDesc): Window;
    closeWindow(window: Window);
    closeWindows(classification: string, id?: number): void;
    closeAllWindows(): void;
}

declare var context: Context;
declare var map: Map;
declare var park: Park;
declare var ui: Ui;
