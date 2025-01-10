/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
(async () =>
{
    await new Promise(res => window.addEventListener("DOMContentLoaded", res));
    if (!window.SharedArrayBuffer)
    {
        document.getElementById("loadingWebassembly").innerText = "Error! SharedArrayBuffer is not defined. This page required the CORP and COEP response headers.";
    }
    if (!window.WebAssembly)
    {
        document.getElementById("loadingWebassembly").innerText = "Error! This page requires WebAssembly. Please upgrade your browser or enable WebAssembly support.";
    }

    window.Module = await window.OPENRCT2_WEB(
        {
            noInitialRun: true,
            arguments: [],
            preRun: [],
            postRun: [],
            canvas: document.getElementById("canvas"),
            print: function(msg)
            {
                console.log(msg);
            },
            printErr: function(msg)
            {
                console.log(msg);
            },
            totalDependencies: 0,
            monitorRunDependencies: () => {},
            locateFile: function(fileName)
            {
                console.log("loading", fileName);
                return fileName;
            },
            funcs: {
                export: () =>
                {
                    const zip = zipFolder("/persistant/");
                    zip.generateAsync({type: "blob"}).then(blob => {
                        const a = document.createElement("a");

                        a.href = URL.createObjectURL(blob);
                        a.download = "OpenRCT2-emscripten.zip";
                        a.click();
                        setTimeout(() => URL.revokeObjectURL(a.href), 1000);
                    })
                },
                import: () =>
                {
                    if (!confirm("Are you sure? This will wipe all current data.")) return;
                    alert("Select a zip file");
                    const input = document.createElement("input");
                    input.type = "file";
                    input.addEventListener("change", async (e) => {
                        let zip = new JSZip();
                        try {
                            zip = await zip.loadAsync(e.target.files[0]);
                        } catch(e) {
                            alert("Not a zip file!");
                            return;
                        }
                        await clearDatabase("/persistant/");
                        for (const k in zip.files) {
                            const entry = zip.files[k];
                            if (entry.dir) {
                                try {
                                    Module.FS.mkdir("/"+k);
                                } catch(e) {}
                            } else {
                                Module.FS.writeFile("/"+k, await entry.async("uint8array"));
                            }
                        }
                        console.log("Database restored");
                    })
                    input.click();
                }
            }
        });

    Module.FS.mkdir("/persistant");
    Module.FS.mount(Module.FS.filesystems.IDBFS, {autoPersist: true}, '/persistant');

    Module.FS.mkdir("/RCT");
    Module.FS.mount(Module.FS.filesystems.IDBFS, {autoPersist: true}, '/RCT');

    Module.FS.mkdir("/OpenRCT2");
    Module.FS.mount(Module.FS.filesystems.IDBFS, {autoPersist: true}, '/OpenRCT2');

    await new Promise(res => Module.FS.syncfs(true, res));

    let configExists = fileExists("/persistant/config.ini");
    if (!configExists)
    {
        Module.FS.writeFile("/persistant/config.ini", `
[general]
game_path = "/RCT"
uncap_fps = true
window_scale = 1.750000
`);
    }

    await updateAssets();
    
    Module.FS.writeFile("/OpenRCT2/changelog.txt", `EMSCRIPTEN --- README

Since we're running in the web browser, we don't have direct access to the file system.
All save data is saved under the directory /persistant.

ALWAYS be sure to save to /persistant/saves when saving a game! Otherwise it will be wiped!

You can import/export the /persistant folder in the options menu.`);
    document.getElementById("loadingWebassembly").remove();

    let filesFound = fileExists("/RCT/Data/ch.dat");

    if (!filesFound)
    {
        document.getElementById("beforeLoad").style.display = "";
        await new Promise(res =>
        {
            document.getElementById("selectFile").addEventListener("change", async (e) =>
            {
                if (await extractZip(e.target.files[0], (zip) =>
                {
                    if (zip !== null)
                    {
                        if (zip.file("Data/ch.dat"))
                        {
                            document.getElementById("beforeLoad").remove();
                            return "/RCT/";
                        }
                        else if (zip.file("RCT/Data/ch.dat"))
                        {
                            document.getElementById("beforeLoad").remove();
                            return "/";
                        }
                    }
                    document.getElementById("statusMsg").innerText = "That doesn't look right. Your file should be a zip file containing Data/ch.dat. Please select your OpenRCT2 contents (zip file):";
                    return false;
                }))
                {
                    res();
                }
            });
        });
    }
    Module.canvas.style.display = "";
    Module.callMain(["--user-data-path=/persistant/", "--openrct2-data-path=/OpenRCT2/"]);
})();

async function updateAssets() {
    let currentVersion = "";
    try {
        currentVersion = Module.FS.readFile("/OpenRCT2/version", {encoding: "utf8"});
        console.log("Found asset version", currentVersion);
    } catch(e) {
        console.log("No asset version found");
    };
    let assetsVersion = "DEBUG";
    try {
        assetsVersion = Module.ccall("GetVersion", "string");
    } catch(e) {
        console.warn("Could not call 'GetVersion'! Is it added to EXPORTED_FUNCTIONS? Is ccall added to EXPORTED_RUNTIME_METHODS?");
    };

    //Always pull assets on a debug build
    if (currentVersion !== assetsVersion || assetsVersion.includes("DEBUG"))
    {
        console.log("Updating assets to", assetsVersion);
        document.getElementById("loadingWebassembly").innerText = "Asset update found. Downloading...";
        await clearDatabase("/OpenRCT2/");
        await extractZip(await (await fetch("assets.zip")).blob(), () =>
        {
            return "/OpenRCT2/";
        });
        Module.FS.writeFile("/OpenRCT2/version", assetsVersion.toString());
    }
}

async function extractZip(data, checkZip) {
    let zip = new JSZip();
    let contents;
    try {
        contents = await zip.loadAsync(data);
    } catch(e) {
        if (typeof checkZip === "function")
        {
            checkZip(null);
        }
        throw e;
    }
    let base = "/";
    if (typeof checkZip === "function")
    {
        const cont = checkZip(contents);
        if (cont === false) return false;
        base = cont;
    }
    for (const k in contents.files) {
        const entry = contents.files[k];
        if (entry.dir)
        {
            try {
                Module.FS.mkdir(base+k);
            } catch(e) {}
        }
        else
        {
            Module.FS.writeFile(base+k, await entry.async("uint8array"));
        }
    }
    return true;
}
async function clearDatabase(dir) {
    await new Promise(res => Module.FS.syncfs(false, res));
    const processFolder = (path) => {
        let contents;
        try {
            contents = Module.FS.readdir(path);
        } catch(e) {
            return;
        }
        contents.forEach((entry) => {
            if ([".", ".."].includes(entry)) return;
            try {
                Module.FS.readFile(path + entry);
                Module.FS.unlink(path + entry);
            } catch(e) {
                processFolder(path + entry + "/");
            }
        })
        if (path === dir) return;
        try {
            Module.FS.rmdir(path, {recursive: true});
        } catch(e) {
            console.log("Could not remove:", path);
        }
    }
    processFolder(dir);
    await new Promise(res => Module.FS.syncfs(false, res));
}
function zipFolder(folder) {
    let zip = new JSZip();
    const processFolder = (name) => {
        let contents;
        try {
            contents = Module.FS.readdir(name);
        } catch(e) {
            return;
        }
        contents.forEach((entry) => {
            if ([".", ".."].includes(entry)) return;
            try {
                Module.FS.readFile(name + entry);
                processFile(name + entry);
            } catch(e) {
                processFolder(name + entry + "/");
            }
        })
    }
    const processFile = (name) => {
        zip.file(name, Module.FS.readFile(name));
    }
    processFolder(folder);
    return zip;
}
function fileExists(path) {
    try {
        Module.FS.readFile(path);
        return true;
    } catch(e) {};
    return false;
}
