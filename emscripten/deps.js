/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
var EmscriptenDeps = {
    ExportPersistentData: () =>
    {
        if (!window.JSZip)
        {
            alert("JSZip library not found. Aborting");
            return;
        }
        const zipFolder = (folder) =>
        {
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
        const zip = zipFolder("/persistent/");
        zip.generateAsync({type: "blob"}).then(blob => {
            const a = document.createElement("a");

            a.href = URL.createObjectURL(blob);
            a.download = "OpenRCT2-emscripten.zip";
            a.click();
            setTimeout(() => URL.revokeObjectURL(a.href), 1000);
        })
    },
    ImportPersistentData: () =>
    {
        if (!window.JSZip)
        {
            alert("JSZip library not found. Aborting");
            return;
        }
        const clearDatabase = async(dir) => {
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
        };
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
            await clearDatabase("/persistent/");
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
};

mergeInto(LibraryManager.library, EmscriptenDeps);
