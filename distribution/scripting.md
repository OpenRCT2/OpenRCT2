# Scripts for OpenRCT2

OpenRCT2 allows custom scripts (also known as plugins) to be written and executed in the game providing additional behaviour on top of the vanilla experience. This can range from extra windows providing information about the park to entire new multiplayer game modes.

Each script is a single physical javascript file within the `plugin` directory in your OpenRCT2 user directory. This usually `C:\Users\YourName\Documents\OpenRCT2` or `/home/YourName/.config/openrct2` on Linux. OpenRCT2 will load every single file with the extension `.js` in this directory recursively, so if you want to prevent a plugin from being used, you must move it outside this directory or rename it so the filename does not end with `.js`.

There are two types of scripts:
* Local
* Remote

Local scripts can not alter the game state. This allows each player to enable any local script for their own game without affecting or other players needing to also enable the same script. These scripts tend to provide extra tools for productivity or new windows containing information.

Remote scripts can alter the game state in certain contexts and must be executed for every player in a multiplayer game. Players can not enable or disable remote scripts for multiplayer servers they join. Instead the server will upload any remote scripts that have been enabled on the server to each player. Servers can therefore enable scripts without players needing to manually download or enable the same script on their end.

## Writing Scripts

Scripts are written in EcmaScript 5 compatible JavaScript. OpenRCT2 currently uses the duktape library to execute scripts. This does not mean you need to write your plugin in JavaScript, there are many transpilers that allow you to write in the language of your choice and then compile it to JavaScript which can then be executed by OpenRCT2. We however recommend using either JavaScript or TypeScript as that will allow you to utilise the type definition file we supply (openrct2.d.ts). If you would like to use EcmaScript 6 or above features such as the `let` keyword or classes, then you will need to use a transpiler such as `babel` or TypeScript.

The best reference for writing plugins is:
* The API: `openrct2.d.ts` distributed with OpenRCT2.
* Our collection of sample scripts: https://github.com/OpenRCT2/plugin-samples
* A TypeScript plugin comprised on multiple sources: https://github.com/IntelOrca/OpenRCT2-ParkManager

Start by copying this template script into a new file in your `plugin` directory.

```js
function main() {
    console.log("Your plugin has started!");
}

registerPlugin({
    name: 'Your Plugin',
    version: '1.0',
    authors: ['Your Name'],
    type: 'remote',
    main: main
});
```

This will log a message to stdout when you open any park. If you are on Windows, make sure to run `openrct2.com` instead of `openrct2.exe` so you can interact with the stdin / stdout console. The console will is also a JavaScript interpreter (REPL), this means you can write and test expressions similar to the console found in web browsers when you press F12. When you make changes to your script, you must exit your current game and open it again for the script to reload... unless you use the hot reload feature.

The hot reload feature can be enabled by editing your config.ini file and setting `enable_hot_reloading` to `true` under `[plugin]`. With this enabled, anytime you save your javascript file, the game will auto-reload the script in real-time. This is allows rapid development of plugins, you may even want to write temporary code for this to preview changes faster such as closing and opening a specific custom window on startup. A demonstration of this can be found here: https://www.youtube.com/watch?v=jmjWzEhmDjk

## Frequently Asked Questions

> Why was JavaScript chosen instead LUA or Python.

JavaScript is a very mature and flexible language with a large if not largest resource base available. There are arguably more transpilers, tools, and libraries for JavaScript than any other language. That and also it using a familiar C-like syntax and 0-based arrays make it more suitable than LUA and Python. Of course if you would still like to use LUA or another language, I am sure you can find a transpiler that will compile it to JavaScript for you.

Another benefit of JavaScript being used is that you get rich editor features such as completion and API documentation using a typescript definition file which works for both JavaScript and TypeScript. [Visual Studo Code](https://code.visualstudio.com) is recommended as that supports the workflow very well. See this video for a demonstration of the editor functionality: https://www.youtube.com/watch?v=jmjWzEhmDjk

> How safe are scripts?

Scripts are executed in a sandbox container with no direct access to your computer. Scripts can only use the APIs we provide which only consist of interactions to OpenRCT2 and a limited API for storing data. It is technically possible for a script to freeze the game, fill your disc up with data, but these aren't particular severe issues and are noticed very quickly.

We use duktape to execute scripts, it is a very mature library for executing scripts, but no library can promise 100% security. If any security vulnerabilities are found, they will likely be fixed quickly and we will update duktape accordingly for OpenRCT2.

We would not provide the scripts feature if we were not confident they were safe, and they are much safe than the alternative which would be native code loaded from DLLs.

> Is it possible for someone to run a bitcoin miner in my game?

Yes, but the performance would be so dire that it would be a waste of their time. This probably the least of your worries.

> What are the limits?

Scripts can conist of any behaviour and have a large memory pool available to them. The speed will vary depending on the computer executing them. The scripts are interpreted, so do not expect anywhere near the performance of native code. In most scenarios this should be good enough, but your random map generator or genetic algorithm for building roller coasters might struggle. Like any other language, there will be tricks to optimising JavaScript and the use of OpenRCT2's APIs.

The APIs for OpenRCT2 try to mimic the internal data structures as close as possible but we can only add so many at a time. The best way to grow the plugin system is to add APIs on demand, so if you find an API is missing, please raise an issue for it on GitHub.

> What does the error 'Game state is not mutable in this context' mean?

This means you are attempting to modify the game state (e.g. change the park, map or guests etc.) in a context where you should not be doing so. This might be because your script is defined as `local` meaning it must work independently of other players not having the script, or a remote script attempting to modify the game in the main function or a user interface event.

Any changes to the game state must be synchronised across all players so that the same changes happen on the same tick for every player. This prevents the game going out of sync. To do this you must only change game state in a compatible hook such as `interval.day` or in the execute method of a game action. Game actions allow players to make specific changes to the game providing they have the correct permissions and the server allows it.

Whilst we try to prevent desynchronision from happening, it still requires careful coding from the script writer. Any attempt to use local specific or non-deterministic data to change the game state will cause a desync. This can be as easy as using `ui.windows`, a variable that will different for every player.

> What are hooks?

Hooks allow a script to subscribe to certain events that occur in the game. For example if the script wants to award cash to the park every day, it can subscribe to the `interval.day` event as follows:

```js
context.subscribe('interval.day', function() {
    park.cash += 10000;
});
```

Other hooks including receiving a chat message in multiplayer or a ride breaking down.

> What are game actions?

Game actions allow you to define new actions players can invoke in games which have their own permission. The flow of a game action is:

1. Player executes action (open park) via UI.
2. Game action (open park), query method is called.
3. If query succeeds, send action to server, else show error message.
4. Server receives action and query method is called on server.
5. If query or permission fails, send error message back to player, else:
6. Send action to all player to execute action at tick ###.
7. All players execute action at tick ###.

This sequence of actions ensure that every player execute the action exactly the same way on the exact same game tick. It also allows the server to validate that the action is allowed and does not fail due to permission or another player winning with a conflicting action. This is why there is a noticable delay when constructing in multiplayer games as the client has to wait for the server to acknowledge the action and reply with the tick number to execute it on, since we do not yet have any rollback support.

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

Yes, it is good practice, particularly if writing scripts for servers to check if the game is running in headless mode before attempting to use any UI APIs. The `ui` namespace is not available in headless mode, so make sure you check it otherwise an error will be thrown.

```js
if (typeof ui !== 'undefined') {
    console.log("OpenRCT2 is not running in headless, UI is available!");
    ui.registerMenuItem('My window', function() {
        // ...
    });
}
```

> Can I use third party JavaScript libraries?

Absolutely, just embed the library in your JavaScript file. There are a number of tools to help you do this.

> Can I share code across multiple scripts?

There are two ways this can be done. If you are just sharing helper routines or libraries, the best thing is to embed it in each plugin. There are a number of tools for JavaScript to help you do this. If you would like two plugins to communicate to each other, perhaps to share data then you can do this by declaring a variable or function in global scope which makes it available to all plugins or use the shared storage APIs.

> What do I do if there is no API for ...?

No problem, if there is something you need, post a new issue on our issue tracker asking for the API you require: https://github.com/OpenRCT2/OpenRCT2/issues

Adding new APIs is most of time straight forward, so they can be added to new development builds quickly. Or if you capable, make the change yourself to the OpenRCT2 source stream and create a pull request.

> Where shall I keep the code for my script?

We recommend GitHub (where OpenRCT2 is hosted), or another source control host such as BitBucket or GitLab. All of them offer private repositories if you want to keep your code private or public repositories which allow others to contribute to your script.

> What licence should I use for my script?

This is up to you. The OpenRCT2 licence does not enforce any licence requirement for content that is loaded into it. MIT is recommended as is the most permissive. You also have to consider what licence any third party libraries you use require.

> Is there a good place to distribute my script to other players?

We not currently host any content database for this. We recommend for now uploading releases of your script on GitHub alongside your source code (if public). Some people like to make a GitHub repository that just consists of a list of content (scripts in this case) which anyone can add to via pull requests.
