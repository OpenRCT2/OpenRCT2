/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

var EmscriptenDeps = {
    $SAVE: {
        filehandle: null,

        readFileData: (isTrackDesign) => {
            const filePath = isTrackDesign ? "/save.park.td6" : "/save.park";
            let data;
            try {
                data = FS.readFile(filePath);
                FS.unlink(filePath);
            } catch(e) {};
            return data;
        }
    },
    EmscriptenSaveGame: async (isTrackDesign, isAutosave, type) => {
        isTrackDesign = isTrackDesign === 1;
        isAutosave = isAutosave === 1;
        const data = SAVE.readFileData(isTrackDesign);
        try {
            if (window.showSaveFilePicker) {
                if (SAVE.filehandle === null) {
                    SAVE.filehandle = null;
                    const file = await showSaveFilePicker({
                        id: "OpenRCT2-" + type,
                        types: [
                            {
                                description: isTrackDesign ? "Track Design File" : "Park File",
                                accept: {
                                    "application/octet-stream": [
                                        isTrackDesign ? ".td6" : ".park"
                                    ]
                                }
                            }
                        ]
                    });
                    if (!file) throw new Error("Could not get a file as RW!");
                    SAVE.filehandle = file;
                }
                const writable = await SAVE.filehandle.createWritable();
                await writable.write(data);
                await writable.close();
                return;
            }
        } catch(e) {
            // Don't return - fallback to the old save dialog.
            console.warn("Failed to use showSaveFilePicker with error message:", e);

            // User requested abort -- return here.
            if (e.name === "AbortError") {
                return;
            }
        }
        if (!isAutosave) {
            const blob = new Blob([data]);
            const a = document.createElement("a");
            a.href = URL.createObjectURL(blob);
            a.download = "download.park";
            a.click();
            setTimeout(() => URL.revokeObjectURL(a.href), 1000);
        }
    },
    EmscriptenResetAutosave: () => {
        SAVE.filehandle = null;
    },
    EmscriptenLoadGame: async (type) => {
        try {
            if (window.showOpenFilePicker) {
                const [file] = await showOpenFilePicker({
                    id: "OpenRCT2-" + type,
                    types: [
                        {
                            description: "Park File",
                            accept: {
                                "application/octet-stream": [
                                    ".park"
                                ]
                            }
                        },
                        {
                            description: "Track Design File",
                            accept: {
                                "application/octet-stream": [
                                    ".td6"
                                ]
                            }
                        }
                    ],
                    multiple: false
                });
                if (!file) throw new Error("Could not get a file as RW!");
                SAVE.filehandle = file;
                const fileInstance = await SAVE.filehandle.getFile();
                const name = SAVE.filehandle.name;
                const data = await fileInstance.arrayBuffer();
                FS.writeFile(name, new Uint8Array(data));
                _LoadGameCallback(stringToNewUTF8(name), type);
                FS.unlink(name);
                return;
            }
        } catch(e) {
            // Don't return - fallback to the old save dialog.
            console.warn("Failed to use showOpenFilePicker with error message:", e);

            // User requested abort -- return here.
            if (e.name === "AbortError") {
                return;
            }
        }
        const input = document.createElement("input");
        input.type = "file";
        input.addEventListener("change", async () => {
            const file = input.files[0];
            if (!file) return;
            const name = file.name;
            const data = await file.arrayBuffer();
            FS.writeFile(name, new Uint8Array(data));
            _LoadGameCallback(stringToNewUTF8(name), type);
            FS.unlink(name);
        });
        input.click();
    },
    ExportPersistentData: () => {
        if (!window.JSZip) {
            alert("JSZip library not found. Aborting");
            return;
        }
        const zipFolder = (folder) => {
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
    ImportPersistentData: () => {
        if (!window.JSZip) {
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
        const input = document.createElement("input");
        input.type = "file";
        input.accept = ".zip";
        input.addEventListener("change", async (e) => {
            if (!confirm("Are you sure? This will wipe all current data.")) return;
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
    },
    SelectFilesToUpload: () => {
        const input = document.createElement("input");
        input.type = "file";
        input.multiple = true;
        input.accept = ".park, .sv4, .sv6";
        input.addEventListener("change", async (e) => {
            console.log(input);
            if (!input.files || input.files.length === 0) return;
            let success = 0;
            for (let i=0; i<input.files.length; i++) {
                const file = input.files[i];
                const name = file.name;
                const ab = await file.arrayBuffer();
                const byteArray = new Uint8Array(ab);
                const outputDirectory = "/persistent/save/uploads/";
                try {
                    FS.mkdir(outputDirectory);
                } catch(e) {};
                try {
                    FS.writeFile(outputDirectory + name, byteArray);
                    success++;
                } catch(e) {};
            }
            alert("Successfully uploaded " + success + " files");
        });
        input.click();
    },
    GetLocaleCurrencyCode: () => {
        // There is no way to get this directly from javascript! Assume based off of user country
        const localeToCurrencyMap = {AD:"EUR",AE:"AED",AF:"AFN",AG:"XCD",AI:"XCD",AL:"ALL",AM:"AMD",AN:"ANG",AO:"AOA",AQ:"USD",AR:"ARS",AS:"USD",AT:"EUR",AU:"AUD",AW:"AWG",AX:"EUR",AZ:"AZN",BA:"BAM",BB:"BBD",BD:"BDT",BE:"EUR",BF:"XOF",BG:"BGN",BH:"BHD",BI:"BIF",BJ:"XOF",BL:"EUR",BM:"BMD",BN:"BND",BO:"BOB",BQ:"USD",BR:"BRL",BS:"BSD",BT:"BTN",BV:"NOK",BW:"BWP",BY:"BYN",BZ:"BZD",CA:"CAD",CC:"AUD",CD:"CDF",CF:"XAF",CG:"XAF",CH:"CHF",CI:"XOF",CK:"NZD",CL:"CLP",CM:"XAF",CN:"CNY",CO:"COP",CR:"CRC",CU:"CUP",CV:"CVE",CW:"ANG",CX:"AUD",CY:"EUR",CZ:"CZK",DE:"EUR",DJ:"DJF",DK:"DKK",DM:"XCD",DO:"DOP",DZ:"DZD",EC:"USD",EE:"EUR",EG:"EGP",EH:"MAD",ER:"ERN",ES:"EUR",ET:"ETB",FI:"EUR",FJ:"FJD",FK:"FKP",FM:"USD",FO:"DKK",FR:"EUR",GA:"XAF",GB:"GBP",GD:"XCD",GE:"GEL",GF:"EUR",GG:"GBP",GH:"GHS",GI:"GIP",GL:"DKK",GM:"GMD",GN:"GNF",GP:"EUR",GQ:"XAF",GR:"EUR",GS:"FKP",GT:"GTQ",GU:"USD",GW:"XOF",GY:"GYD",HK:"HKD",HM:"AUD",HN:"HNL",HR:"EUR",HT:"HTG",HU:"HUF",ID:"IDR",IE:"EUR",IL:"ILS",IM:"GBP",IN:"INR",IO:"USD",IQ:"IQD",IR:"IRR",IS:"ISK",IT:"EUR",JE:"GBP",JM:"JMD",JO:"JOD",JP:"JPY",KE:"KES",KG:"KGS",KH:"KHR",KI:"AUD",KM:"KMF",KN:"XCD",KP:"KPW",KR:"KRW",KW:"KWD",KY:"KYD",KZ:"KZT",LA:"LAK",LB:"LBP",LC:"XCD",LI:"CHF",LK:"LKR",LR:"LRD",LS:"LSL",LT:"EUR",LU:"EUR",LV:"EUR",LY:"LYD",MA:"MAD",MC:"EUR",MD:"MDL",ME:"EUR",MF:"EUR",MG:"MGA",MH:"USD",MK:"MKD",ML:"XOF",MM:"MMK",MN:"MNT",MO:"MOP",MP:"USD",MQ:"EUR",MR:"MRU",MS:"XCD",MT:"EUR",MU:"MUR",MV:"MVR",MW:"MWK",MX:"MXN",MY:"MYR",MZ:"MZN",NA:"NAD",NC:"XPF",NE:"XOF",NF:"AUD",NG:"NGN",NI:"NIO",NL:"EUR",NO:"NOK",NP:"NPR",NR:"AUD",NU:"NZD",NZ:"NZD",OM:"OMR",PA:"PAB",PE:"PEN",PF:"XPF",PG:"PGK",PH:"PHP",PK:"PKR",PL:"PLN",PM:"EUR",PN:"NZD",PR:"USD",PS:"ILS",PT:"EUR",PW:"USD",PY:"PYG",QA:"QAR",RE:"EUR",RO:"RON",RS:"RSD",RU:"RUB",RW:"RWF",SA:"SAR",SB:"SBD",SC:"SCR",SD:"SDG",SE:"SEK",SG:"SGD",SH:"SHP",SI:"EUR",SJ:"NOK",SK:"EUR",SL:"SLE",SM:"EUR",SN:"XOF",SO:"SOS",SR:"SRD",SS:"SSP",ST:"STN",SV:"USD",SX:"ANG",SY:"SYP",SZ:"SZL",TC:"USD",TD:"XAF",TF:"EUR",TG:"XOF",TH:"THB",TJ:"TJS",TK:"NZD",TL:"USD",TM:"TMT",TN:"TND",TO:"TOP",TR:"TRY",TT:"TTD",TV:"AUD",TW:"TWD",TZ:"TZS",UA:"UAH",UG:"UGX",UM:"USD",US:"USD",UY:"UYU",UZ:"UZS",VA:"EUR",VC:"XCD",VE:"VED",VG:"USD",VI:"USD",VN:"VND",VU:"VUV",WF:"XPF",WS:"WST",XK:"EUR",YE:"YER",YT:"EUR",ZA:"ZAR",ZM:"ZMW",ZW:"ZWG"};
        const country = Intl.DateTimeFormat().resolvedOptions().locale.split("-").pop();
        const code = localeToCurrencyMap[country];
        return stringToNewUTF8(code);
    }
};

autoAddDeps(EmscriptenDeps, '$SAVE');
addToLibrary(EmscriptenDeps);
