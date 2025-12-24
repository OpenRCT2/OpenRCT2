/**
 * OpenRCT2 Plugin: JSON Action Bridge
 * 
 * This plugin creates a TCP server that accepts JSON messages and converts them
 * to OpenRCT2 game actions.
 * 
 * The server listens on localhost:11752 by default.
 */

registerPlugin({
    // Fields required by OpenRCT2's plugin system
    name: "JSON Action Bridge",
    version: "1.0.0",
    authors: ["Auto-generated"],
    type: "local",
    targetApiVersion: 66,
    main: function() {
        const BRIDGE_PORT = 11752;
        
        console.log("JSON Action Bridge: Starting server on port " + BRIDGE_PORT);
        
        // Create TCP listener
        var server = network.createListener();
        
        server.on('connection', function(conn) {
            console.log("JSON Action Bridge: Client connected from " + conn.remoteAddress);
            
            var buffer = "";
            
            conn.on('data', function(data) {
                try {
                    // Accumulate data until we have a complete JSON message
                    buffer += data.toString();
                    
                    // Try to parse complete JSON messages (separated by newlines)
                    var lines = buffer.split('\n');
                    buffer = lines.pop() || ""; // Keep incomplete line in buffer
                    
                    for (var i = 0; i < lines.length; i++) {
                        var line = lines[i].trim();
                        if (line.length === 0) continue;
                        
                        handleMessage(conn, line);
                    }
                } catch (e) {
                    console.log("JSON Action Bridge: Error handling data: " + e.message);
                    sendResponse(conn, {
                        status: "error",
                        message: e.message
                    });
                }
            });
            
            conn.on('close', function() {
                console.log("JSON Action Bridge: Client disconnected");
            });
            
            conn.on('error', function(err) {
                console.log("JSON Action Bridge: Connection error: " + err);
            });
        });
        
        server.on('error', function(err) {
            console.log("JSON Action Bridge: Server error: " + err);
        });
        
        try {
            server.listen(BRIDGE_PORT);
            console.log("JSON Action Bridge: Server listening on port " + BRIDGE_PORT);
        } catch (e) {
            console.log("JSON Action Bridge: Failed to start server: " + e.message);
        }
        
        function handleMessage(conn, jsonStr) {
            try {
                var msg = JSON.parse(jsonStr);
                var action = msg.action;
                
                if (!action) {
                    sendResponse(conn, {
                        status: "error",
                        message: "Missing 'action' field"
                    });
                    return;
                }
                
                // Extract action arguments (everything except 'action')
                var args = {};
                for (var key in msg) {
                    if (key !== 'action') {
                        args[key] = msg[key];
                    }
                }
                
                console.log("JSON Action Bridge: Received action: '" + action + "' (type: " + typeof action + ")");
                
                // Handle special query actions that don't have a 1-1 mapping to game actions
                // These must be checked BEFORE calling context.executeAction
                // Trim and normalize the action string to handle any whitespace issues
                // Most "read" actions are implemented as special actions below
                var normalizedAction = String(action).trim();
                
                // Special action: Get terrain/surface height at coordinates
                if (normalizedAction === "get_surface_height") {
                    console.log("JSON Action Bridge: Handling get_surface_height query");
                    try {
                        var tileX = msg.x;
                        var tileY = msg.y;
                        if (tileX === undefined || tileY === undefined) {
                            sendResponse(conn, {
                                status: "error",
                                message: "Missing 'x' or 'y' parameter (tile coordinates)"
                            });
                            return;
                        }
                        
                        // Get the surface element at this tile
                        var tile = map.getTile(tileX, tileY);
                        if (!tile) {
                            sendResponse(conn, {
                                status: "error",
                                message: "Invalid tile coordinates"
                            });
                            return;
                        }
                        
                        var surfaceHeight = 0;
                        var waterHeight = 0;
                        for (var i = 0; i < tile.numElements; i++) {
                            var element = tile.getElement(i);
                            if (element.type === "surface") {
                                // convert to game units
                                surfaceHeight = element.baseHeight * 8;
                                waterHeight = element.waterHeight * 8;
                                break;
                            }
                        }
                        
                        sendResponse(conn, {
                            status: "ok",
                            tileX: tileX,
                            tileY: tileY,
                            gameX: tileX * 32,
                            gameY: tileY * 32,
                            surfaceHeight: surfaceHeight,
                            waterHeight: waterHeight
                        });
                        return;
                    } catch (e) {
                        console.log("JSON Action Bridge: Error getting surface height: " + e.message);
                        sendResponse(conn, {
                            status: "error",
                            message: "Failed to get surface height: " + e.message
                        });
                        return;
                    }
                }
                
                // Special action: Get all ride objects for a given ride type
                if (normalizedAction === "get_ride_objects") {
                    console.log("JSON Action Bridge: Handling get_ride_objects query (special action)");
                    try {
                        var rideType = msg.rideType;
                        if (rideType === undefined || rideType === null) {
                            sendResponse(conn, {
                                status: "error",
                                message: "Missing 'rideType' parameter"
                            });
                            return;
                        }
                        
                        var objManager = objectManager;
                        if (typeof objManager === 'undefined') {
                            sendResponse(conn, {
                                status: "error",
                                message: "ObjectManager API not available"
                            });
                            return;
                        }
                        
                        // Get all ride entries for this ride type
                        var rideEntries = objManager.getAllObjects("ride");
                        var validEntries = [];
                        
                        for (var i = 0; i < rideEntries.length; i++) {
                            var rideObj = rideEntries[i];
                            if (rideObj && rideObj.rideType && rideObj.rideType.indexOf(rideType) !== -1) {
                                validEntries.push({
                                    index: i,
                                    identifier: rideObj.identifier || "",
                                    name: rideObj.name || ""
                                });
                            }
                        }
                        
                        sendResponse(conn, {
                            status: "ok",
                            rideObjects: validEntries
                        });
                        return;
                    } catch (e) {
                        console.log("JSON Action Bridge: Error getting ride objects: " + e.message);
                        if (e.stack) {
                            console.log("JSON Action Bridge: Stack: " + e.stack);
                        }
                        sendResponse(conn, {
                            status: "error",
                            message: "Failed to get ride objects: " + e.message
                        });
                        return;
                    }
                }
                
                // Special action: Get all rides
                if (normalizedAction === "get_rides") {
                    console.log("JSON Action Bridge: Handling get_rides query (special action)");
                    try {
                        if (typeof map === 'undefined') {
                            console.log("JSON Action Bridge: Map API is undefined");
                            sendResponse(conn, {
                                status: "error",
                                message: "Map API not available"
                            });
                            return;
                        }
                        var rides = map.rides;
                        if (!rides) {
                            console.log("JSON Action Bridge: map.rides is null/undefined");
                            sendResponse(conn, {
                                status: "ok",
                                rides: []
                            });
                            return;
                        }
                        // console.log("JSON Action Bridge: Found " + rides.length + " rides");
                        var rideData = [];
                        for (var i = 0; i < rides.length; i++) {
                            var ride = rides[i];
                            var rideInfo = {
                                id: ride.id,
                                name: ride.name || "",
                                type: ride.type || 0,
                                status: ride.status || "",
                                stations: []
                            };
                            
                            // Add station positions
                            if (ride.stations && ride.stations.length > 0) {
                                for (var s = 0; s < ride.stations.length; s++) {
                                    var station = ride.stations[s];
                                    var stationInfo = {
                                        start: station.start ? {x: station.start.x, y: station.start.y, z: station.start.z} : null,
                                        entrance: station.entrance ? {x: station.entrance.x, y: station.entrance.y, z: station.entrance.z, direction: station.entrance.direction} : null,
                                        exit: station.exit ? {x: station.exit.x, y: station.exit.y, z: station.exit.z, direction: station.exit.direction} : null,
                                        length: station.length || 0
                                    };
                                    rideInfo.stations.push(stationInfo);
                                }
                            }
                            
                            rideData.push(rideInfo);
                        }
                        console.log("JSON Action Bridge: Sending response with " + rideData.length + " rides");
                        sendResponse(conn, {
                            status: "ok",
                            rides: rideData
                        });
                        console.log("JSON Action Bridge: Response sent, returning");
                        return; // CRITICAL: Must return to prevent executing as game action
                    } catch (e) {
                        console.log("JSON Action Bridge: Exception in get_rides: " + e.message);
                        if (e.stack) {
                            console.log("JSON Action Bridge: Stack: " + e.stack);
                        }
                        sendResponse(conn, {
                            status: "error",
                            message: "Failed to get rides: " + e.message
                        });
                        console.log("JSON Action Bridge: Error response sent, returning");
                        return; // CRITICAL: Must return to prevent executing as game action
                    }
                }
                
                // Execute the action (only for actual game actions)
                console.log("JSON Action Bridge: Executing game action: " + normalizedAction);
                context.executeAction(normalizedAction, args, function(result) {
                    if (result.error) {
                        sendResponse(conn, {
                            status: "error",
                            error: result.error,
                            errorTitle: result.errorTitle,
                            errorMessage: result.errorMessage
                        });
                    } else {
                        sendResponse(conn, {
                            status: "ok",
                            result: result
                        });
                    }
                });
                
            } catch (e) {
                console.log("JSON Action Bridge: Error parsing message: " + e.message);
                sendResponse(conn, {
                    status: "error",
                    message: "Failed to parse JSON: " + e.message
                });
            }
        }
        
        function sendResponse(conn, response) {
            try {
                var jsonStr = JSON.stringify(response) + "\n";
                conn.write(jsonStr);
            } catch (e) {
                console.log("JSON Action Bridge: Error sending response: " + e.message);
            }
        }
    }
});

