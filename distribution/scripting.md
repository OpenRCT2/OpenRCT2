# Scripts for OpenRCT2

OpenRCT2 allows custom scripts (also known as plug-ins) to be written and executed in the game providing additional behaviour on top of the vanilla experience. This can range from extra windows providing information about the park to entire new multiplayer game modes.

Each script is a single physical javascript file within the `plugin` directory in your OpenRCT2 user directory. The user directory for each platform is usually:

- Windows: `C:\Users\YourName\Documents\OpenRCT2`
- Mac: `/Users/YourName/Library/Application Support/OpenRCT2`
- Linux: `$XDG_CONFIG_HOME/OpenRCT2` or in its absence `$HOME/.config/OpenRCT2`

OpenRCT2 will load every single file with the extension `.js` in this directory recursively. So if you want to prevent a plug-in from being used, you must move it outside this directory, or rename it so the filename does not end with `.js`.

There are three types of scripts:
* Local
* Remote
* Intransient

Local scripts can **not** alter the game state. This allows each player to enable any local script for their own game without other players needing to also enable the same script. These scripts tend to provide extra tools for productivity, or new windows containing information.

Remote scripts on the other hand can alter the game state in certain contexts, thus must be enabled for every player in a multiplayer game. Players **cannot** enable or disable remote scripts for multiplayer servers they join. Instead the server will upload any remote scripts that have been enabled on the server to each player. This allows servers to enable scripts without players needing to manually download or enable the same script on their end.

Intransient scripts are similar to local scripts, in that they can **not** alter the game state. However they are loaded at the very start of launching OpenRCT2 and remain loaded until shutdown. This allows the plugin to provide functionality across different screens such as the title screen.

The authors must also define a licence for the plug-in, making it clear to the community whether that plug-in can be altered, copied, etc. A good reference material is listed on [ChooseALlicense](https://choosealicense.com/appendix/), try to pick one of them and use its corresponding identifier, as listed on [SPDX](https://spdx.org/licenses/).

## Writing Scripts

Scripts are written in ECMAScript 5 compatible JavaScript. OpenRCT2 currently uses the [duktape](https://duktape.org) library to execute scripts. This however does not mean you need to write your plug-in in JavaScript, there are many transpilers that allow you to write in a language of your choice and then compile it to JavaScript allowing it to be executed by OpenRCT2. JavaScript or [TypeScript](https://www.typescriptlang.org) is recommended however, as that will allow you to utilise the type definition file we supply (`openrct2.d.ts`). If you would like to use ECMAScript 6 or later which contain features such as the `let` keyword or classes, then you will need to use a transpiler such as [Babel](https://babeljs.io) or [TypeScript](https://www.typescriptlang.org).

Official references for writing plug-ins are:
* The API: `openrct2.d.ts` distributed with OpenRCT2.
* Our collection of sample scripts: [OpenRCT2/plugin-samples](https://github.com/OpenRCT2/plugin-samples)
* A TypeScript plug-in comprised of multiple sources: [IntelOrca/OpenRCT2-ParkManager](https://github.com/IntelOrca/OpenRCT2-ParkManager)

Start by copying this template script into a new file in your `plugin` directory:
```js
function main() {
    console.log("Your plug-in has started!");
}

registerPlugin({
    name: 'Your Plugin',
    version: '1.0',
    authors: ['Your Name'],
    type: 'remote',
    licence: 'MIT',
    targetApiVersion: 34,
    main: main
});
```

This will log a message to the terminal screen (`stdout`) when you open any park. If you are on Windows, make sure to run `openrct2.com` instead of `openrct2.exe` so you can interact with the `stdin` / `stdout` console. The console is a JavaScript interpreter (REPL), this means you can write and test expressions similar to the console found in web browsers when you press `F12`. When you make changes to your script, you must exit your current game and open it again for the script to reload... unless you use the hot reload feature.

The hot reload feature can be enabled by editing your `config.ini` file and setting `enable_hot_reloading` to `true` under `[plugin]`. When this is enabled, the game will auto-reload the script in real-time whenever you save your JavaScript file. This allows rapid development of plug-ins as you can write code and quickly preview your changes, such as closing and opening a specific custom window on startup. A demonstration of this can be found on YouTube: [OpenRCT2 plugin hot-reload demo](https://www.youtube.com/watch?v=jmjWzEhmDjk)

## Breaking changes
As of version 34 there are breaking Api changes.

- **Version 34:** `Entity.type` will now return `"guest"` or `"staff"` instead of `"peep"`.
- **Version 63:** Accessing G2 sprites by id directly is now deprecated in favor of a future-proof implementation using `IconName` and/or `context.getIcon()`.
- **Version 68:** Custom game actions registered through `context.registerAction()` now wrap the callback arguments in a `GameActionEventArgs`, similar to `context.subscribe()` callbacks.
- **Version 77:** Network APIs that take player index and group index now take player ID and group ID instead.

## Frequently Asked Questions

> Why was JavaScript chosen instead of LUA or Python.

JavaScript is a very mature and flexible language with a large, if not the largest, resource base available. There are arguably more transpilers, tools, and libraries for JavaScript than any other language. That and also it using a familiar C-like syntax and 0-indexed arrays make it more suitable than LUA and Python. Of course if you would still like to use LUA or another language, there will likely be a JavaScript transpiler for it.

Another benefit of using JavaScript is that you get rich editor features such as completion and API documentation by using a TypeScript definition file which works for both TypeScript and JavaScript. [Visual Studio Code](https://code.visualstudio.com) is recommended, as that supports the workflow very well. See the [OpenRCT2 plugin hot-reload demo](https://www.youtube.com/watch?v=jmjWzEhmDjk) video for a demonstration of the editor functionality.

> How safe are scripts?

Scripts are executed in a sandbox container with no direct access to your computer. Scripts can only use the APIs we provide which only consist of interactions to OpenRCT2 and a limited API for storing data. It is technically possible for a script to freeze the game, or fill your disc up with data, but these aren't particularly severe issues and are noticed quite quickly.

The [duktape](https://duktape.org) library is used to execute scripts, it is a very mature library for executing scripts, but no library can promise 100% security. If any security vulnerabilities are found, they will likely be fixed promptly and OpenRCT2 can then be updated to use the new version of `duktape`.

> Is it possible for someone to run a bitcoin miner in my game?

Yes, but the performance would be so dire that it would be a waste of their time. This is probably the least of your worries.

> What are the limits?

Scripts can consist of any behaviour and have a large memory pool available to them. The speed will vary depending on the hardware and system executing them. The scripts are interpreted, so do not expect anywhere close to the performance of native code. In most scenarios this should be satisfactory, but a random map generator, or genetic algorithm for building roller coasters might struggle. Like any language, there will be tricks to optimising JavaScript and the use of the OpenRCT2 APIs.

The APIs for OpenRCT2 try to mimic the internal data structures as close as possible but we can only add so many at a time. The best way to grow the plug-in system is to add APIs on-demand. So if you find an API is missing, please raise an issue for it on GitHub and also feel free to submit a pull request afterwards.

> What is ```targetApiVersion```

In case there are breaking Api changes plugins can use this to keep the old Api behavior, as an example in version 34 ```Entity.Type``` would no longer return "peep" for
guests and staff, instead it would return either "guest" or "staff", so if your plugin expects "peep" you can specify the version 33 to keep the old behavior. See the list
of breaking changes. If this is not specified it will default to version 33, it is recommended to specify the current api version.

> How do I debug my script?

Debugging has not yet been implemented, but is planned. In the meantime, you can use `console.log` to print useful information for diagnosing problems.

> What does the error 'Game state is not mutable in this context' mean?

This means you are attempting to modify the game state (e.g. change the park, map or guests etc.) in a context where you should not be doing so. This might be because your script is defined as `local` or `intransient`, meaning it must work independently of other players, not having the script enabled, or a remote script attempting to modify the game in the main function or a user interface event.

Any changes to the game state must be synchronised across all players so that the same changes happen on the same tick for every player. This prevents the game going out of sync. To do this you must only change game state in a compatible hook such as `interval.day` or in the execute method of a game action. Game actions allow players to make specific changes to the game providing they have the correct permissions and the server allows it.

Whilst OpenRCT2 tries to prevent desynchronisation from happening, it still requires careful coding to ensure the behaviour is deterministic across all clients. Any attempt to use local specific or non-deterministic data to change the game state will cause a desync. This can be as easy as using `ui.windows`, a variable that can be different for every player.

> What are hooks?

Hooks allow a script to subscribe to certain events that occur in the game. For example if the script was to award cash to the park every day, it would subscribe to the `interval.day` event as follows:

```js
context.subscribe('interval.day', function() {
    park.cash += 10000;
});
```

Other hooks include receiving a chat message in multiplayer, or a ride breaking-down.

> What are game actions?

Game actions allow you to define new actions (with a new permission slot) that players can invoke in games. Here is an example flow of a game action such as opening the park:

1. Player executes action (open park) via UI.
2. Game action (open park), query method is called.
3. If query succeeds, send action to server, else show error message.
4. Server receives action and query method is called on server.
5. If query or permission fails, send error message back to player, else:
6. Send action to all player to execute action at tick ###.
7. All players execute action at tick ###.

This sequence of actions ensures that every player execute the action exactly in the same way on the exact same game tick. It also allows the server to validate that the action is allowed and does not fail due to permission or another player executing a conflicting action just before it. This is why there is a noticeable delay when constructing in multiplayer games, as the client has to wait for the server to acknowledge the action and reply with the tick number to execute it on, since we do not yet have any rollback support.

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

> Can I modify the widgets on built-in windows?

Not yet. A lot of the internal code for these windows relies on the widgets existing, and being in a specific order, as well as hard resizing them to certain positions. Modifying them would likely crash the game until the window system is improved to handle such changes.

> Can servers add additional user interface elements to players?

Yes, remote scripts are uploaded to every client and run as-is. Even if the server is running in headless mode, the script can still contain UI calls for the benefit of the players that are not running in headless mode. Be sure to check if the UI is available first before executing UI calls.

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

> Can plugins communicate with other processes, or the internet?

There is a socket API (based on net.Server and net.Socket from node.js) available for listening and communicating across TCP streams. For security purposes, plugins can only listen and connect to localhost. If you want to extend the communication further, you will need to provide your own separate reverse proxy. What port you can listen on is subject to your operating system, and how elevated the OpenRCT2 process is.

```js
var server = network.createServer();
server.on('connection', function (conn) {
    conn.on('data', function(data) {
        console.log("Received data: ", data);
        conn.write("Reply data");
    });
});
server.listen(8080);
```

> Can I use third party JavaScript libraries?

Absolutely, just embed the library in your JavaScript file. There are a number of tools to help you do this.

> Can I share code across multiple scripts?

Yes, and there are two ways this can be done. If you are just sharing helper routines or libraries, the best thing is to embed it in each plug-in. There are a number of tools for JavaScript to help you do this. If you would like two plug-ins to communicate with each other, perhaps to share data, then you can do this by declaring a variable or function in global scope which is available to all plug-ins, or use the shared storage APIs.

> What do I do if there is no API for ...?

No problem, if there is something you need, post a new issue on the issue tracker for OpenRCT2 asking for the API you require: https://github.com/OpenRCT2/OpenRCT2/issues

Adding new APIs is most of time straight forward, so they can be added to new development builds quickly. Or if you are capable, make the change yourself to the OpenRCT2 source stream and create a pull request.

> How do I prevent my script from running on older versions of OpenRCT2 that do not support all the APIs I require?

When registering your plugin, you can specify a minimum required API version like below:
```js
registerPlugin({
    name: 'Your Plugin',
    version: '1.2', // Your plugin version
    authors: ['Your Name'],
    type: 'remote',
    minApiVersion: 7, // OpenRCT2 plugin API v7 or higher is required to run your plugin
    main: main
});
```

When new APIs are introduced, or the behaviour of current APIs change, a new version number will be issued which you can find in the OpenRCT2 source code or changelog.

> Where shall I keep the code for my script?

We recommend [GitHub](https://github.com) (where OpenRCT2 is hosted), or another source control host such as [BitBucket](https://bitbucket.org) or [GitLab](https://gitlab.com). All of them offer private repositories if you want to keep your code private, or public repositories which allow others to easily contribute to your script.

> What licence should I use for my script?

This is up to you. The OpenRCT2 licence does not enforce any licence requirement for content that is loaded into it. MIT is recommended as it is the most permissive. You also have to consider what licence any third party libraries you are using allow.

> Is there a good place to distribute my script to other players?

The recommendation is to upload releases of your script on GitHub alongside your source code (if public). There is a community driven repository for sharing plugins available at https://openrct2plugins.org/.
