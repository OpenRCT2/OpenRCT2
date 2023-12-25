const fs = require('fs');

// This is also the order expected for each version.
const ChangeTypes = ["Headline feature", "Feature", "Improved", "Change", "Removed", "Fix"];
// A list of change types that were previously used but aren't accepted anymore, will be replaced with the value in the map.
const ChangeTypeSubstitutions = {
    "Improve": "Improved",
};
const HeaderSeperator = "------------------------------------------------------------------------";

function reportError(ctx, message) {
    console.error(`Error on line ${ctx.lineIndex}: ${message}`);
    process.exit(-1);
}

function reportLineError(line, message) {
    console.error(`Error on line ${line.index + 1}: ${message}`);
    process.exit(-1);
}

function consumeLine(ctx) {
    if (ctx.lineIndex >= ctx.lines.length) {
        reportError(ctx, "Unexpected end of file");
    }
    const line = ctx.lines[ctx.lineIndex];
    ctx.lineIndex++;
    return line;
}

function peekLine(ctx) {
    if (ctx.lineIndex >= ctx.lines.length) {
        return null;
    }
    return ctx.lines[ctx.lineIndex];
}

function readVersionHeader(ctx) {
    // Extract the version number.
    const versionLine = consumeLine(ctx);
    const versionMatch = versionLine.text.match(/^((\d+\.\d+\.\d+(\.\d+)?(-\w+)?) \((\d{4}-\d{2}-\d{2}|in development)\))$/);
    if (versionMatch == null) {
        reportLineError(versionLine, "Expected a version number");
    }

    // Check the version seperator
    const versionSeperator = consumeLine(ctx);
    if (versionSeperator.text != HeaderSeperator) {
        reportLineError(versionSeperator, "Expected version seperator");
    }

    // Check for optional headline, this means the there is text not starting with - and an empty line before
    // the first entry.
    var headline = peekLine(ctx);
    if (headline != null && headline.text != "" && !headline.text.startsWith("-")) {
        consumeLine(ctx);

        // Check for empty line
        const emptyLine = peekLine(ctx);
        if (emptyLine == null || emptyLine.text != "") {
            reportError(ctx, "Expected empty line after headline");
        }

        consumeLine(ctx);
        headline = headline.text;
    } else {
        headline = "";
    }

    return {
        version: versionMatch[2],
        date: versionMatch[5],
        headline: headline,
    };
}

function isValidReference(ref) {
    const regex = /^\w*#\d+( \(partial\))?$/;
    return regex.test(ref);
}

function readVersionEntry(ctx, rawEntry, versionInfo) {
    // Check for the '- ' prefix
    var text = rawEntry.text;
    if (!text.startsWith("- ")) {
        reportLineError(rawEntry, "Expected entry prefix '- '");
    }
    text = text.substr(2);

    // Extract the change type
    const changeTypeIdx = text.indexOf(":");
    if (changeTypeIdx == -1) {
        reportLineError(rawEntry, "Expected change type");
    }
    var changeType = text.substring(0, changeTypeIdx).trim();
    text = text.substring(changeTypeIdx + 1).trim();

    // Check if changeType is valid
    if (!ChangeTypes.includes(changeType)) {
        // Check for substitution
        if (changeType in ChangeTypeSubstitutions) {
            changeType = ChangeTypeSubstitutions[changeType];
        } else {
            reportLineError(rawEntry, `Invalid change type '${changeType}', types can be ${ChangeTypes.join(", ")}`);
        }
    }

    // Extract the references.
    var refs = [];
    if (text.startsWith("[")) {
        const refsEndIdx = text.indexOf("]");
        if (refsEndIdx == -1) {
            reportLineError(rawEntry, "Open reference bracket '[' without closing bracket ']'");
        }
        refs = text.substring(1, refsEndIdx).split(",").map(ref => ref.trim());
        text = text.substring(refsEndIdx + 1).trim();
    } else {
        // Text must start with '[', allow having this missing in older versions.
        if (versionInfo.date == "in development") {
            reportLineError(rawEntry, "Expected reference [#REF]");
        }
    }

    // Check if reference is correct, can be '#123' or 'project#123'
    for (const ref of refs) {
        if(!isValidReference(ref)) {
            reportLineError(rawEntry, `Invalid reference '${ref}', must be '#123' or 'project#123'`);
        }
    }

    const result = {
        changeType: changeType,
        refs: refs,
        description: text.trim(),
    };
    return result;
}

function readVersionEntries(ctx, versionInfo) {
    var entries = [];
    while (ctx.lineIndex < ctx.lines.length) {
        const rawEntry = consumeLine(ctx);
        // Empty line means end of version block.
        if (rawEntry.text == "") {
            break;
        }
        const entry = readVersionEntry(ctx, rawEntry, versionInfo);
        entries.push(entry);
    }
    return entries;
}

function readVersionBlock(ctx) {
    const versionInfo = readVersionHeader(ctx);
    const entries = readVersionEntries(ctx, versionInfo);
    return {
        version: versionInfo.version,
        date: versionInfo.date,
        headline: versionInfo.headline,
        entries: entries
    };
}

function parseChangelog(changelog) {
    // Split the changelog into lines but also keep the line numbers.
    const lines = changelog.split('\n').map((text, index) => ({ text, index }));

    // Remove duplicate empty lines.
    for (var i = 0; i < lines.length; i++) {
        if (lines[i].text == "" && (i == 0 || lines[i - 1].text == "")) {
            lines.splice(i, 1);
            i--;
        }
    }

    var ctx = {
        lines,
        lineIndex: 0,
        versions: []
    };

    while (ctx.lineIndex < ctx.lines.length) {
        const versionBlock = readVersionBlock(ctx);
        ctx.versions.push(versionBlock);
    }

    return ctx.versions;
}

function referenceSort(a, b) {
    // Extract the number from the reference
    const numA = parseInt(a.replace(/\D/g, ''), 10);
    const numB = parseInt(b.replace(/\D/g, ''), 10);

    // Check if both are direct references or subproject references
    const isDirectA = a.startsWith('#');
    const isDirectB = b.startsWith('#');

    if (isDirectA && isDirectB) {
        // If both are direct references, sort by the number
        return numA - numB;
    } else if (!isDirectA && !isDirectB) {
        // If both are subproject references, sort by the name and then the number
        const nameA = a.split('#')[0];
        const nameB = b.split('#')[0];
        if (nameA.toLowerCase() < nameB.toLowerCase()) return -1;
        if (nameA.toLowerCase() > nameB.toLowerCase()) return 1;
        return numA - numB;
    } else {
        // If one is a direct reference and the other is a subproject reference,
        // the direct reference should come first
        return isDirectA ? -1 : 1;
    }
}

function reorderGroups(a, b) {
    const changeTypeDiff = ChangeTypes.indexOf(a.changeType) - ChangeTypes.indexOf(b.changeType);
    if (changeTypeDiff != 0) {
        return changeTypeDiff;
    }
    // If changeType is identical, sort based on reference
    const hasMainRefA = a.refs.length > 0 && a.refs[0].startsWith("#");
    const hasMainRefB = b.refs.length > 0 && b.refs[0].startsWith("#");
    // Sort by main reference number.
    if (hasMainRefA && hasMainRefB) {
        const numA = parseInt(a.refs[0].replace(/\D/g, ''), 10);
        const numB = parseInt(b.refs[0].replace(/\D/g, ''), 10);
        return numA - numB;
    } else if (hasMainRefA && !hasMainRefB) {
        return -1;
    } else if (!hasMainRefA && hasMainRefB) {
        return 1;
    } else {
        // Sort by reference name.
        const nameA = a.refs.length > 0 ? a.refs[0].split('#')[0] : "";
        const nameB = b.refs.length > 0 ? b.refs[0].split('#')[0] : "";
        if (nameA.toLowerCase() < nameB.toLowerCase()) return -1;
        if (nameA.toLowerCase() > nameB.toLowerCase()) return 1;
        return 0;
    }
}

function cleanupVersionBlock(version) {
    // Sort the references first, numbers come before sub project refs.
    version.entries.forEach(entry => entry.refs.sort((a, b) => referenceSort(a, b)));

    // Sort the entries by change type.
    version.entries.sort((a, b) => reorderGroups(a, b));

    // Ensure entries end with a dot.
    version.entries.forEach(entry => {
        if (!entry.description.endsWith(".")) {
            entry.description += ".";
        }
    });
}

function cleanupVersions(versions) {
    for (const version of versions) {
        cleanupVersionBlock(version);
    }
}

function printChangelog(versions) {
    for (const version of versions) {
        console.log(`${version.version} (${version.date})`);
        console.log(HeaderSeperator);
        if (version.headline != "") {
            console.log(version.headline);
            console.log();
        }
        for (const entry of version.entries) {
            var line = `- ${entry.changeType}: `;
            if (entry.refs.length > 0) {
                line += `[${entry.refs.join(", ")}] `;
            }
            line += `${entry.description}`;
            console.log(line);
        }
        console.log();
    }
}

function checkLineEndings(changelog) {
    if (changelog.indexOf("\r\n") != -1) {
        console.error("Changelog contains Windows line endings, please convert to Unix line endings.");
        process.exit(-1);
    }
}

const filePath = process.argv[2];
fs.readFile(filePath, 'utf8', (err, data) => {
    if (err) {
        console.error('Error reading the file:', err);
        process.exit(-1);
    }

    checkLineEndings(data);

    var versions = parseChangelog(data);

    cleanupVersions(versions);

    printChangelog(versions);
});