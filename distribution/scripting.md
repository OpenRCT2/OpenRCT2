# Scripts for OpenRCT2

OpenRCT2 allows custom scripts (also known as plugins) to be written and executed in the game providing additional behaviour on top of the vanilla experience. This can range from extra windows providing information about the park to entire new multiplayer game modes.

Each script is a single physical javascript file within the `plugin` directory in your OpenRCT2 user directory. The user directory for each platform is usually:

- Windows: `C:\Users\YourName\Documents\OpenRCT2`
- Mac: `/Users/YourName/Library/Application Support/OpenRCT2`
- Linux: `$XDG_CONFIG_HOME/OpenRCT2` or in its absence `$HOME/.config/OpenRCT2`

You can also find the user directory via the “Open custom content folder” option under the red toolbox button in the game's title screen.

OpenRCT2 will load every single file with the extension `.js` in this directory recursively. So if you want to prevent a plugin from being used, you must move it outside this directory, or rename it so the filename does not end with `.js`.

## Writing Scripts

Scripts are written in ECMAScript 5 compatible JavaScript. OpenRCT2 currently uses the [duktape](https://duktape.org) library to execute scripts. This however does not mean you need to write your plugin in JavaScript, there are many transpilers that allow you to write in a language of your choice and then compile it to JavaScript allowing it to be executed by OpenRCT2. JavaScript or [TypeScript](https://www.typescriptlang.org) is recommended however, as that will allow you to utilise the type definition file we supply (`openrct2.d.ts`). If you would like to use ECMAScript 6 or later which contain features such as arrow functions, the `let` keyword or classes, then you will need to use a transpiler such as [Babel](https://babeljs.io) or [TypeScript](https://www.typescriptlang.org).

Official references for writing plugins are:
* The API: `openrct2.d.ts` distributed with OpenRCT2.
* Our collection of sample scripts: [OpenRCT2/plugin-samples](https://github.com/OpenRCT2/plugin-samples)
* A TypeScript plugin comprised of multiple sources: [IntelOrca/OpenRCT2-ParkManager](https://github.com/IntelOrca/OpenRCT2-ParkManager)

Start by copying this template script into a new file in your `plugin` directory:
```js
function main() {
    console.log("Your plugin has started!");
}

registerPlugin({
    name: 'Your Plugin',
    version: '1.0',
    authors: ['Your Name'],
    type: 'remote',
    licence: 'MIT',
    targetApiVersion: 34,
    minApiVersion: 10,
    main: main
});
```

This will log a message to the terminal screen (`stdout`) when you open any park. If you are on Windows, make sure to run `openrct2.com` instead of `openrct2.exe` so you can interact with the `stdin` / `stdout` console. The console is a JavaScript interpreter (REPL), this means you can write and test expressions similar to the console found in web browsers when you press `F12`. When you make changes to your script, you must exit your current game and open it again for the script to reload... unless you use the hot reload feature.

### Hot reload

The hot reload feature can be enabled by editing your `config.ini` file and setting `enable_hot_reloading` to `true` under `[plugin]`. When this is enabled, the game will auto-reload the script in real-time whenever you save your JavaScript file. This allows rapid development of plugins as you can write code and quickly preview your changes, such as closing and opening a specific custom window on startup. A demonstration of this can be found on YouTube: [OpenRCT2 plugin hot-reload demo](https://www.youtube.com/watch?v=jmjWzEhmDjk)

### Plugin licence

The authors must also define a licence for the plugin, making it clear to the community whether that plugin can be altered, copied, etc. A good reference material is listed on [ChooseALlicense](https://choosealicense.com/appendix/), try to pick one of them and use its corresponding identifier, as listed on [SPDX](https://spdx.org/licenses/).

### Script types

There are three types of scripts that can influence how the game loads the script:

* Local
* Remote
* Intransient

#### Singleplayer

In singleplayer all plugin types are unrestricted. Local and remote plugins reload each time a scenario or a save file is opened. Plugins of both types can alter game state directly or execute either built-in game actions or custom game actions.

Intransient plugins load when the game starts and stays active until the game is shutdown.

#### Multiplayer

In multiplayer local and remote plugins are different in when they are loaded and how they can alter the game state.

**Local plugins** will load on any client in multiplayer that has the plugin installed, which will allow each player to bring any plugin they have with them into the multiplayer server without other players in the server needing to have the same plugin installed.

Local plugins **cannot** alter the game state directly, but they **can** interact with the game by mimicking a player's actions through the built-in “game actions”. Local plugins may also use custom actions, but clients that do not have the plugin will not execute the registered callbacks, which may lead to a desync.

**Remote plugins** will only load in multiplayer when installed on the server. When players join a server, the remote script will be downloaded to the client and will stay active for the duration of the multiplayer session. Any remote plugins installed on the client will not load in multiplayer.

Remote plugins **can** interact with the game by mimicking a player's actions through the built-in “game actions”. Remote scripts **can** also alter the game state directly, but only within the “execute” context of custom game actions. With custom actions, OpenRCT2 can ensure that the game state mutations are executed synchronously across all connected clients.

**Intransient plugins** in multiplayer have the same restrictions as local plugins. They will stay loaded and only interact with the game through game actions.

#### Example usage

- Local plugins tend to provide extra tools for productivity with the built-in game actions, or new windows containing information about aspects of the game or the park.
- Remote plugins can be tools that have to mutate game state more directly using either built-in or custom game actions, or multiplayer server functionality like welcome messages, anti-spam, or kick/ban tools.
- Intransient plugins could provide functionality across different screens such as the title screen, and can be used for plugins like the title sequence editor or global dashboards.

## Game actions

Game actions are actions that players can invoke in games. For every action a player can perform in the game that alters the park or scenario in some way, there is a built-in game action.

Here is an example flow of a game action such as placing a small scenery item (like a tree):

1. Player attempts to perform action (places tree) via the game's interface.
2. The query method of the game action “place small scenery” is called and may check:
   - Can the player afford to place the item?
   - Is the placement tile owned by the park?
   - Is the placement location blocked by another tile element?
3. If the query fails, show an error message.
4. If the query succeeds and the game is singleplayer: the execute function for “place small scenery” is called.
5. A tree is placed at the location specified by the game action.

In multiplayer, there are some additional steps after step 3:

4. If the query succeeds and the game is multiplayer: game action is send to the server.
5. Server checks if the player has permissions to perform the game action.
6. Server calls the query method again.
7. If the permission check or the query fails, the server sends an error message back to the client.
8. If the perission check and query succeed: send action to all players to execute action at a specified tick.
9. All player clients receive the game action and execute it at the specified tick.
10. A tree is placed at the specified location on all clients that are connected to the server.

This sequence of steps ensures that every player client executes the game action exactly in the same way on the exact same game tick (synchronized execution). Furthermore, it allows the server to validate that the action is allowed and does not fail due to permission or another player executing a conflicting action just before it. This is why there is a noticeable delay when constructing in multiplayer games, as the client has to wait for the server to acknowledge the action and reply with the tick number to execute it on.

Plugins can also use game actions to alter the game in the same way players can. Game actions are the **recommended approach** for mutating game state, as it automatically ensures multiplayer-safety and various sanity checks for money, clearance checks, and others.

Besides built-in game actions, remote scripts can also register their own game actions (“custom game actions”) with query and execute functions written in JavaScript. These custom actions allow custom game state mutations to be executed in sync for all players on the server. Beware that the plugin will also be responsible for proper permission checks, as custom game actions have no permission checks by default.

## Breaking changes

As of version 34 there are a few breaking API changes:

- **Version 34:** `Entity.type` will now return `"guest"` or `"staff"` instead of `"peep"`.
- **Version 63:** Accessing G2 sprites by id directly is now deprecated in favor of a future-proof implementation using `IconName` and/or `context.getIcon()`.
- **Version 68:** Custom game actions registered through `context.registerAction()` now wrap the callback arguments in a `GameActionEventArgs`, similar to `context.subscribe()` callbacks.
- **Version 77:** Network APIs that take player index and group index now take player ID and group ID instead.

## Frequently Asked Questions

> Why was JavaScript chosen instead of LUA or Python.

JavaScript is a very mature and flexible language with a large, if not the largest, resource base available. There are arguably more transpilers, tools, and libraries for JavaScript than any other language. That and also it using a familiar C-like syntax and 0-indexed arrays make it more suitable than LUA and Python. Of course if you would still like to use LUA or another language, there will likely be a JavaScript transpiler for it.

Another benefit of using JavaScript is that you get rich editor features such as completion and API documentation by using a TypeScript definition file which works for both TypeScript and JavaScript. [Visual Studio Code](https://code.visualstudio.com) is recommended, as that supports the workflow very well. See the [OpenRCT2 plugin hot-reload demo](https://www.youtube.com/watch?v=jmjWzEhmDjk) video for a demonstration of the editor functionality.

---

> How safe are scripts?

Scripts are executed in a sandbox container with no direct access to your computer. Scripts can only use the APIs we provide which only consist of interactions to OpenRCT2 and a limited API for storing data. It is technically possible for a script to freeze the game, or fill your disc up with data, but these aren't particularly severe issues and are noticed quite quickly.

The [duktape](https://duktape.org) library is used to execute scripts, it is a very mature library for executing scripts, but no library can promise 100% security. If any security vulnerabilities are found, they will likely be fixed promptly and OpenRCT2 can then be updated to use the new version of `duktape`.

---

> Is it possible for someone to run a bitcoin miner in my game?

Yes, but the performance would be so dire that it would be a waste of their time. This is probably the least of your worries.

---

> What are the limits?

Scripts can consist of any behaviour and have a large memory pool available to them. The speed will vary depending on the hardware and system executing them. The scripts are interpreted, so do not expect anywhere close to the performance of native code. In most scenarios this should be satisfactory, but a random map generator, or genetic algorithm for building roller coasters might struggle. Like any language, there will be tricks to optimising JavaScript and the use of the OpenRCT2 APIs. [Duktape also provides some engine-specific performance tips](https://wiki.duktape.org/performance).

The APIs for OpenRCT2 try to provide access to the game’s data structures as much as possible but we can only add so many at a time. The best way to grow the plugin system is to add APIs on-demand. So if you find an API is missing, please raise an issue for it on GitHub and also feel free to discuss it on our Discord and to submit a pull request afterwards.

---

> What is `targetApiVersion`?

In case there are breaking API changes, plugins can use this to keep the old API behaviour. For example in version 34 `Entity.type` would no longer return `peep` for guests and staff, instead it would return either `guest` or `staff`, so if your plugin expects `peep` you can specify the version 33 to keep the old behaviour. See the [list of breaking changes](#breaking-changes). If this is not specified it will default to version 33 and the plugin will log an error on startup, it is recommended to specify the current API version.

---

> How do I prevent my script from running on older versions of OpenRCT2 that do not support all the APIs I require?

When registering your plugin, you can specify a minimum required API version like below:

```js
registerPlugin({
    name: 'Your Plugin',
    version: '1.2', // Your plugin version
    authors: ['Your Name'],
    type: 'remote',
    licence: 'MIT',
    minApiVersion: 7, // OpenRCT2 plugin API v7 or higher is required to run your plugin
    main: main
});
```

This will prevent the script from loading at all on OpenRCT2 versions before API v7.

When new APIs are introduced, or the behaviour of current APIs change, a new version number will be issued which you can find in the OpenRCT2 [source code](https://github.com/OpenRCT2/OpenRCT2/blob/develop/src/openrct2/scripting/ScriptEngine.h) or changelog.

---

> How do I debug my script?

Debugging has not yet been implemented, but is planned. In the meantime, you can use `console.log` to print useful information for diagnosing problems.

---

> Which plugin type should I pick if I only care about singleplayer?

If you are making a plugin that only uses built-in game actions to mutate the game, or is not mutating the game at all (like dashboards or info-windows), then it's best to go with a **local plugin**. You will get multiplayer support out-of-the-box without any extra work.

If you are making a plugin that mutates the game more directly via any of the various APIs, then it is possible to make it a **remote plugin** to prevent it from loading when a player joins a server as a client. If the plugin happens to be installed on the server, it will be distributed to all joining clients though. Therefor it is **highly recommended** to always wrap any direct game state mutations in a [custom game action](#game-actions). If your plugin alters game state incorrectly in a multiplayer session, **it will lead to desyncs!**

Use **transient plugins** only if you need the plugin to remain loaded across multiple park/scenario sessions or in the title screen, when using `context.sharedStorage` or `context.getParkStorage` for data persistency is not sufficient enough.

---

> What does the error “Game state is not mutable in this context” mean?

This means you are attempting to modify the game state (e.g. change the park, map or guests etc.) in a context where you should not be doing so. This might be because your script is defined as `local` or `intransient` and altering game state in a multiplayer session. Or your script is defined as `remote` and altering game state outside of a safe function callback in a multiplayer session.

In multiplayer, any changes to the game state must be synchronised across all players to ensure that the same changes happen on the same tick for every player. This prevents the game going out of sync. To do this you must only change game state in a compatible hook such as `interval.day` or in the execute method of a custom game action. [Game actions](#game-actions) allow players to make specific changes to the game providing they have the correct permissions and the server allows it.

Whilst OpenRCT2 tries to prevent desynchronisation from happening, it still requires careful coding to ensure the behaviour is deterministic across all clients. Any attempt to use local specific or non-deterministic data to change the game state will cause a desync. This can be as easy as using `ui.windows`, a variable that can be different for every player.

---

> What are hooks?

Hooks allow a script to subscribe to certain events that occur in the game. For example if the script was to award cash to the park every day, it would subscribe to the `interval.day` event as follows:

```js
context.subscribe('interval.day', function() {
    park.cash += 10000;
});
```

Other hooks include receiving a chat message in multiplayer, or recalculation of a ride’s ratings.

---

> Can I run code specifically for servers or clients?

Yep, there is an API to get the current network mode.

```js
if (network.mode == "server") {
    console.log("This is a server...");
} else if (network.mode == "client") {
    console.log("This is a client...");
} else {
    console.log("This is single player...");
}
```

---

> Can I run code only if the user interface is available?

Yes, it is good practice, particularly if writing scripts for servers to check if the game is running in headless mode before attempting to use any UI APIs. The `ui` namespace is not available in headless mode, so make sure you check it, otherwise an error will be thrown.

```js
if (typeof ui !== 'undefined') {
    console.log("OpenRCT2 is not running in headless, UI is available!");
    ui.registerMenuItem('My window', function() {
        // ...
    });
}
```

---

> Can I modify the widgets on built-in windows?

Not yet. A lot of the internal code for these windows relies on the widgets existing, and being in a specific order, as well as hard resizing them to certain positions. Modifying them would likely crash the game until the window system is improved to handle such changes.

---

> Can servers add additional user interface elements to players?

Yes, remote scripts are uploaded to every client and run as-is. Even if the server is running in headless mode, the script can still contain UI calls for the benefit of the players that are not running in headless mode. Be sure to check if the UI is available first before executing UI calls.

---

> Can plugins persist data across multiple OpenRCT2 instances?

Yes, use `context.sharedStorage` to read or write data to an external JSON file: `plugin.store.json` in the OpenRCT2 user directory. It is recommended that you namespace all your plugin's data under a common name.

```js
var h = context.sharedStorage.get('IntelOrca.MagicLift.Height');
if (!h) {
    context.sharedStorage.set('IntelOrca.MagicLift.Height', 2);
}
```

All plugins have access to the same shared storage.

If you want to only store data specific to the current park that is loaded, use `context.getParkStorage`. Any data stored here will be written to the .park file.

---

> Can plugins communicate with other processes, or the internet?

There is a socket API (based on net.Server and net.Socket from node.js) available for listening and communicating across TCP streams. For security purposes, plugins can only listen and connect to localhost. If you want to extend the communication further, you will need to provide your own separate reverse proxy. What port you can listen on is subject to your operating system, and how elevated the OpenRCT2 process is.

```js
var server = network.createListener();
server.on('connection', function (conn) {
    conn.on('data', function(data) {
        console.log("Received data: ", data);
        conn.write("Reply data");
    });
});
server.listen(8080);
```

---

> Can I use third party JavaScript libraries?

Absolutely, as long as you embed the library in your JavaScript file. There are a number of tools to help you do this. Raw module statements like `import` and `require` are not supported unless transpiled by other bundling tools.

---

> Can I share code across multiple scripts?

Yes, and there are two ways this can be done. If you are just sharing helper routines or libraries, the best thing is to embed it in each plugin. There are a number of tools for JavaScript to help you do this. Raw module statements like `import` and `require` are not supported unless transpiled by other bundling tools.

If you would like two plugins to communicate with each other, perhaps to share data, then you can do use custom actions, or use the shared storage APIs.

---

> What do I do if there is no API for ...?

No problem, if there is something you need, post a new issue on the issue tracker for OpenRCT2 asking for the API you require: https://github.com/OpenRCT2/OpenRCT2/issues

Adding new APIs is most of time straight forward, so they can be added to new development builds quickly. Or if you are capable, make the change yourself to the OpenRCT2 source stream and create a pull request.

---

> Why do array/string functions like `find` and `includes` give me an error?

OpenRCT2 uses Duktape as the script engine and Duktape only has full support for ES5 Javascript and limited support for ES6+ features. Functions like `find` and `includes` are ES6 functions and not yet supported by Duktape. As an alternative you can implement these yourself, or use a polyfill.

- [List of object and functions in ES5](https://github.com/microsoft/TypeScript/blob/main/src/lib/es5.d.ts)
- [List of ES6+ features that are implemented in Duktape.](https://wiki.duktape.org/postes5features)

Some ES6 features that are not supported: arrow functions, classes, `let` keyword, async/await, spread operator, destructuring, template literals, default parameters. To use these features anyway, it is recommended to use a transpiler and/or polyfills.

---

> Where shall I keep the code for my script?

We recommend [GitHub](https://github.com) (where OpenRCT2 is hosted), or another source control host such as [BitBucket](https://bitbucket.org) or [GitLab](https://gitlab.com). All of them offer private repositories if you want to keep your code private, or public repositories which allow others to easily contribute to your script.

---

> What licence should I use for my script?

This is up to you. The OpenRCT2 licence does not enforce any licence requirement for content that is loaded into it. MIT is recommended as it is the most permissive. You also have to consider what licence any third party libraries you are using allow.

---

> Is there a good place to distribute my script to other players?

The recommendation is to upload releases of your script on GitHub alongside your source code (if public). There is a community driven repository for sharing plugins available at https://openrct2plugins.org/.
