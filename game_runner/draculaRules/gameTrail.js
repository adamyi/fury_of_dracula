var functionDecorate = require("../jsFun/functionMagic");
functionDecorate(Function);

var list = require("../jsFun/listLike");
var assert = require("../jsFun/assert");

var constants = require("./gameConstants");

var gameMap = require("./gameMap");

var revealTrailItem = function(state, trailIndex) {
	// WTF Javascript, ensure this is a number
	trailIndex -= 0;

	assert((typeof trailIndex === "number"), "type error in revealTrailItem");

	var draculaMoves     = state.draculaMoves;
	var oldestTrailIndex = getOldestTrailIndex(state);

	var moveIndex = oldestTrailIndex + trailIndex;
	var move = draculaMoves[moveIndex];

	assert(moveIndex >= 0, "revealTrailItem: negative move index");
	assert(move, "revealTrailItem: move undefined. "
         + "\noldest: " +  oldestTrailIndex
         + "\ntrail index: " + trailIndex
         + "\nmove index: " + moveIndex
         + "\nmoves: " + draculaMoves);

	// find all the indices in the past moves
	// that this move points back to (with hides and doublebacks)
	var resolvedMoveItem = getResolvedTrailItem(move, moveIndex, draculaMoves);
	var indicesToUpdate = resolvedMoveItem.moveIndices;

	var revealIndex = function(index) {
		state.hunterKnowledge[index] = draculaMoves[index];
	};

	// reveal these moves in the hunter's knowledge
	list.each(indicesToUpdate, revealIndex);
};

var revealLocationInTrail = function(state, location) {
	// update hunterKnowledge to include information from draculaMoves

	var trailLocations = getTrailLocations(state);
	var trailIndices = [];

	// collect the indices of the trail that match this location
	var collectTrailIndices = function(trailLocation, trailIndex) {
		if (trailLocation === location) {
			trailIndices.push(trailIndex);
		}
	};
	// all players can see which MOVE(s) in the trail correspond to [this] city
	list.each(trailLocations, collectTrailIndices);

	// reveal each of these indices in the hunter knowledge
	var revealInState = revealTrailItem.partialApply(state);
	list.each(trailIndices, revealInState);
};

var isDoublebackCode = function(moveCode) {
	assert(moveCode, "moveCode undefined in isDoublebackCode");
	var doubleBackNumber = parseInt(moveCode[1]);
	return (moveCode[0] == constants.doublebackPrefix)
      && !isNaN(doubleBackNumber);
};

var getResolvedTrailItem = function(move, i, allMoves) {
	var resolvedIndices = [];

	var resolvedIndex = i;
	var resolvedMove = move;

	assert(move === allMoves[i], "argument mismatch in getResolvedTrailItem");

	var checkNext = true;
	while (checkNext) {
		checkNext = false;
		resolvedIndices.push(resolvedIndex);

		assert(resolvedMove, "resolvedMove is undefined in getResolvedTrailItem");
		if (resolvedMove === constants.hideMove) {
			// the current move is a hide move,
			// check the next one
			resolvedIndex -= 1;
			checkNext = true;
		} else if (isDoublebackCode(resolvedMove)) {
			// the current move is a double back
			// check the double-backed-to location
			resolvedIndex -= parseInt(resolvedMove[1]);
			checkNext = true;
		}

		assert(resolvedIndex >= 0, "negative resolvedIndex in getResolvedTrailItem");

		resolvedMove = allMoves[resolvedIndex];
	}

	assert(resolvedMove, "undefined resolvedMove in resolveTrailLocations");

	return { moveIndices: resolvedIndices, move: resolvedMove };
};
(function() {
	assert(getResolvedTrailItem("HI", 4, ["RO", "CD", "CF", "D2", "HI"]).move === "CD");
})();


// resolve HI, TP and Dn moves into a location
var getResolvedMoveLocation = function(move, i, allMoves) {
	var replacementMove;

	replacementMove = getResolvedTrailItem(move, i, allMoves).move;

	if (replacementMove === constants.teleportCode) {
		replacementMove = gameMap.castleDraculaCode;
	}

	assert(replacementMove, "undefined replacementMove in resolveTrailLocations");
	assert(list.contains(gameMap.locations, replacementMove),
		"invalid resolution of location " + replacementMove + " in resolveTrailLocations");

	return replacementMove;
};
(function() {
	assert(getResolvedMoveLocation("HI", 4, ["RO", "TP", "CF", "D2", "HI"]) === "CD");
})();

var getDraculaLocations = function(state) {
	return list.map(state.draculaMoves, getResolvedMoveLocation);
};
(function() {
	var tmpState = { draculaMoves: ["RO", "TP", "CF", "D2", "HI"] };
	assert(list.isEqual(getDraculaLocations(tmpState), ["RO", "CD", "CF", "CD", "CD"]));
})();

// Index in draculaMoves which is the oldest in the trail
var getOldestTrailIndex = function(state, isDraculaMovePhase) {
	var trailLength = constants.trailFullSize;

	if (isDraculaMovePhase) {
		// in dracula's move phase, the trail is one less
		// (he hasn't made a move yet)
		trailLength -= 1;
	}

	var oldestMoveIndex = state.draculaMoves.length - trailLength;

	if (oldestMoveIndex < 0) {
		oldestMoveIndex = 0;
	}

	return oldestMoveIndex;
};
(function() {
	var tmpState = { draculaMoves: ["RO", "TP", "CF", "D2", "HI"] };
	assert(getOldestTrailIndex(tmpState) == 0, "getOldestTrailIndex 1");
	tmpState = { draculaMoves: ["CF", "CF", "CF", "RO", "TP", "CF", "D2", "HI"] };
	assert(getOldestTrailIndex(tmpState) == 2, "getOldestTrailIndex 2");
})();

var getTrailLocations = function(state, isDraculaMovePhase) {
	var resolvedMoves = getDraculaLocations(state);
	return resolvedMoves.slice(getOldestTrailIndex(state, isDraculaMovePhase));
};

var getTrailMoves = function(state, isDraculaMovePhase) {
	return state.draculaMoves.slice(getOldestTrailIndex(state, isDraculaMovePhase));
};

var getTrailActions = function(state, isDraculaMovePhase) {
	return state.draculaActions.slice(getOldestTrailIndex(state, isDraculaMovePhase));
};

module.exports = {
	getTrailActions: getTrailActions,
	getTrailLocations: getTrailLocations,
	getTrailMoves: getTrailMoves,
	getDraculaLocations: getDraculaLocations,
	getOldestTrailIndex: getOldestTrailIndex,
	getResolvedMoveLocation: getResolvedMoveLocation,
	getResolvedTrailItem: getResolvedTrailItem,

	isDoublebackCode: isDoublebackCode,

	revealLocationInTrail: revealLocationInTrail,
	revealTrailItem: revealTrailItem
};

