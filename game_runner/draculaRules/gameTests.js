var list = require("../jsFun/listLike");
var assert = require("../jsFun/assert");

var constants  = require("./gameConstants");
var gameMap = require("./gameMap");

(function testExpandConnections() {
	// Tests
	assert(
		list.isEqual(
			gameMap.expandConnections(1, gameMap.railConnections, "SO"),
			[ "BE", "SA", "VR" ]
		),
		"gameMap.expandConnections"
	);

	assert(
		list.isEqual(
			gameMap.expandConnections(2, gameMap.railConnections, "SO"),
			[ "SO", "SZ", "BE", "SA", "VR" ]
		),
		"gameMap.expandConnections"
	);

	assert(
		list.isEqual(
			gameMap.expandConnections(3, gameMap.railConnections, "SO"),
			[ "BE", "SA", "VR", "BC", "BD", "SO", "SZ" ]
		),
		"gameMap.expandConnections"
	);

	assert(
		list.isEqual(
			gameMap.expandConnections(4, gameMap.railConnections, "SO"),
			[ "SO", "SZ", "BE", "SA", "VR", "CN", "GA", "VI", "BC", "BD" ]
		),
		"expandConnections"
	);
})();
