var functionDecorate = require("../jsFun/functionMagic");
functionDecorate(Function);

var list = require("../jsFun/listLike");
var assert = require("../jsFun/assert");

var constants = require("./gameConstants");
var gameMap = require("./gameMap");
var gameTrail = require("./gameTrail");

var isLegalDraculaMove = function(state, move) {

	var isLegal = true;
	var dracula = state.dracula;
	var allConnections = getDraculaConnections(state);

	var reasonDebug = "Legal";

	if (!move) {
		isLegal = false;
		reasonDebug = "Move undefined";
	} else if (move === gameMap.hospitalCityCode) {
		// Dracula can never move to the hospital
		isLegal = false;
		reasonDebug = "Illegal move: Dracula cannot move to the hospital";
	} else if (state.round === 0) {
		// first move can be anywhere on the map
		isLegal = list.contains(gameMap.locations, move);
		if (!isLegal) {
			reasonDebug = "Illegal move: " + move + "Not a map location";
		}
	} else {
		assert(dracula, "dracula undefined in isLegalDraculaMove");
		assert(dracula.location, "dracula location undefined in isLegalDraculaMove");
		assert(allConnections, "No road or sea connections from " + dracula.location);

		var pastMoves = state.draculaMoves;

		var isDraculaMovePhase = true;

		var movePhaseTrail = gameTrail.getTrailMoves(state, isDraculaMovePhase);

		if (move === constants.hideMove) {
			if (list.contains(movePhaseTrail, constants.hideMove)) {
				// Dracula can make a HIDE move if he has not already made one in his trail,
				isLegal = false;
				reasonDebug = "Illegal move: Dracula already made a hide in his trail.";
			} else if (gameMap.locationInfo[dracula.location].isSea) {
				// he cannot HIDE at sea.
				isLegal = false;
				reasonDebug = "Illegal Move: Dracula cannot hide at sea";
			}
		} else if (gameTrail.isDoublebackCode(move)) {
			isLegal = isLegalDoublebackMove(state, move);
			reasonDebug = "Illegal double-back: not adjacent or is already in trail";
		} else if (move === dracula.location || list.contains(allConnections, move)) {
			// or a LOCATION
			// [dracula can move] adjacent to or the same as
			// his current location

			// provided the MOVE is not currently in his trail
			// (he is permitted to have more than one teleport move in his trail tho
			// it is unlikely this would occur).

			if (list.contains(movePhaseTrail, move)) {
				isLegal = false;
				reasonDebug = "Location " + move + " already in trail";
			}
		} else if (move === "TP") {
			// It is illegal to teleport unless Dracula has no other available moves.
			// This is automaticaly handled elsewhere.
			isLegal = false;
			reasonDebug = "Illegal teleport: Dracula has available moves";
		} else {

			// Move isn't connected to current location, and thus disallowed.
			// Or, move does not represent a city on the current map
			assert(!list.contains(allConnections, move));
			reasonDebug = "Illegal move: Location " + move + " not adjacent to " + dracula.location;
			isLegal = false;
		}
	}
	//   if (!isLegal) {
	//     console.log(move + ' illegal');
	//    console.log(reasonDebug);
	//}
	return isLegal;
};

var isLegalHunterMove = function(state, move) {
	var hunter = state.currentPlayer;

	var isLegal = false;

	if (state.round == 0) {
		// This player's first move
		// they can choose to move to any map location
		if (list.contains(gameMap.locations, move)) {
			isLegal = true;
		}
	} else if (move === hunter.location) {
		// hunters can choose not to move (rest) and stay in the same place
		isLegal = true;
	} else if (gameMap.isConnected(hunter.location, move, "road")) {
		// Road moves: a Hunter can move to any city directly connected to the
		// current city by a road
		isLegal = true;
	} else if (gameMap.isConnected(hunter.location, move, "sea")) {
		// Sea moves: a Hunter can move from a port to an adjacent sea, or a
		// sea to an adjacent sea, or a sea to an adjacent port city.
		isLegal = true;
	} else {
		// Rail moves: The maximum distance that can be moved via rail is determined
		// by the sum of the round number and the Hunter number
		// mod 4
		railDistance = (state.round + state.currentPlayerNumber) % 4;

		var possibleRailDestinations = gameMap.expandConnections(railDistance, gameMap.railConnections, hunter.location);

		if (list.contains(possibleRailDestinations, move)) {
			isLegal = true;
		}
	}

	return isLegal;
};

var getDraculaConnections = function(state) {
	var dracula = state.dracula;

	var roadConnections = gameMap.roadConnections[dracula.location];
	var seaConnections  = gameMap.seaConnections[dracula.location];
	var allConnections;

	if (roadConnections  && seaConnections) {
		allConnections = roadConnections.concat(seaConnections);
	} else if (roadConnections) {
		allConnections = roadConnections;
	} else {
		allConnections = seaConnections;
	}

	return allConnections;
};

var getCandidateDraculaMoves = function(state) {
	var dracula = state.dracula;

	var allConnections = getDraculaConnections(state);

	// gather special moves dracula can do
	var specialMoves = constants.doublebackMoves.concat([constants.hideMove]);

	// candidate moves are the adjacent locations and the special moves
	var candidateMoves = allConnections.concat(specialMoves);

	return candidateMoves;
};

var canDraculaMove = function(state) {
	if (state.round == 0) {
		// Dracula can always move on the first turn
		return true;
	}

	var candidateMoves = getCandidateDraculaMoves(state);
	var isLegal = isLegalDraculaMove.partialApply(state);

	// are any candidate moves legal
	return list.any(candidateMoves, isLegal);

};

var getValidDraculaMoves = function(state) {
	var moves;
	if (state.round == 0) {
		// first move

		var isNotHospital = function(location) {
			return location != gameMap.hospitalCityCode;
		};

		// Dracula can't start at the hospital
		moves = list.filter(gameMap.locations, isNotHospital);
	} else if (!canDraculaMove(state)) {
		moves = [constants.teleportCode];
	} else {
		var candidateMoves = getCandidateDraculaMoves(state);

		// is legal with this state
		var isLegal = isLegalDraculaMove.partialApply(state);

		moves = list.filter(candidateMoves, isLegal);
	}
	return moves;
};


var isLegalDoublebackMove = function(state, move) {
	assert(move, "move is undefined. in isDoublebackMove: called from " + arguments.callee.caller.name);

	var isDoubleback = true;

	if (!gameTrail.isDoublebackCode(move)) {
		// move isn't a doubleback code
		// a double back move has a prefix and a digit
		isDoubleback = false;
	} else {
		var doubleBackNumber = parseInt(move[1]);

		// pass in isDraculaMovePhase = true (as there are 5 trail items)
		var trailLocations = gameTrail.getTrailLocations(state, true);
		var trailMoves = gameTrail.getTrailMoves(state, true);
		var doubleBackIndex = trailLocations.length-doubleBackNumber;

		if (doubleBackIndex < 0) {
			// not doubling back to trail
			isDoubleback = false;
		} else {

			var doubleBackLocation = trailLocations[doubleBackIndex];

			var allConnections = getDraculaConnections(state);
			var isAdjacentTrailMove = list.contains(allConnections, doubleBackLocation);

			// is adjacent to or the same as dracula's current location
			isAdjacentTrailMove = isAdjacentTrailMove || (doubleBackLocation === state.dracula.location);

			if (!isAdjacentTrailMove) {
				// A DOUBLE_BACK move is a move to a location adjacent to or the
				// same as his current location
				isDoubleback = false;
			} else if (list.any(trailMoves, gameTrail.isDoublebackCode)) {
				// Dracula can (only) make a DOUBLE_BACK move if he has not already
				// made a DOUBLE_BACK move in his trail.
				isDoubleback = false;
			}
		}
	}

	return isDoubleback;
};

module.exports = {
	canDraculaMove: canDraculaMove,
	getValidDraculaMoves: getValidDraculaMoves,
	isLegalHunterMove: isLegalHunterMove,
	isLegalDraculaMove: isLegalDraculaMove
};
