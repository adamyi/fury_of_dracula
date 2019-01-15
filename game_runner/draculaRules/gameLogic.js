var functionDecorate = require("../jsFun/functionMagic");
functionDecorate(Function);

var list = require("../jsFun/listLike");
var assert = require("../jsFun/assert");

var constants = require("./gameConstants");

var gameMap = require("./gameMap");
var gameEncounters = require("./gameEncounters");
var gameMoves = require("./gameMoves");
var gameTrail = require("./gameTrail");

function doHunterMove(state, newLocation) {
	var railDistance, isValid, hunter;
	var moveMade;
	var trapsEncountered, vampiresEncountered, draculasEncountered;
	var locationInfo;

	assert(gameMoves.isLegalHunterMove(state, newLocation), "illegal move in doHunterMove");

	hunter = state.currentPlayer;
	assert(hunter.type === "hunter", "hunter move, but not hunter: " + hunter.type + " in doHunterMove");


	/* ACTION PHASE */
	trapsEncountered = 0;
	vampiresEncountered = 0;
	draculasEncountered = 0;

	locationInfo = gameMap.locationInfo[newLocation];

	assert(locationInfo, "no information for location " + newLocation);

	if (locationInfo.isCity) {
		// If the Hunter is in any city in Dracula's trail then all players can
		// see which MOVE(s) in the trail correspond to that city

		if (list.contains(gameTrail.getTrailLocations(state), newLocation)) {
			gameTrail.revealLocationInTrail(state, newLocation);
		}
	}

	// the Hunter encounters, in sequence,
	// all Traps for that city which are in the current trail,
	// then any immature Vampires,
	// and then at last confronts Dracula himself if he is also in that city
	trapsEncountered = gameEncounters.doHunterTrapEncounters(state, newLocation);
	vampiresEncountered = gameEncounters.doHunterVampireEncounters(state, newLocation);
	draculasEncountered = gameEncounters.doHunterDraculaEncounter(state, newLocation);

	assert(trapsEncountered <= 3, "traps encountered: " + trapsEncountered + " at: " + newLocation);
	assert(vampiresEncountered <= 1, "vampires encountered: " + vampiresEncountered + " at: " + newLocation);
	assert(draculasEncountered <= 1, "draculas encountered: " + draculasEncountered + " at: " + newLocation);

	if (hunter.lifePoints <= 0) {
		// Whenever a Hunter is reduced to 0 or less life points they
		// are instantly automagically teleported to the Hospital
		// of St Joseph & St Mary
		// (and can do nothing else for the remainder of their turn.)
		state.currentPlayer.location = gameMap.hospitalCityCode;
		hunter.lifePoints = constants.hunterMaxLifePoints;

		// The score decreases by 6 each time a Hunter
		// is teleported to the hospital.
		state.score -= constants.hospitalTeleportScorePenalty;
	} else {

		// If the Hunter is in the same city or sea they were in last turn
		if (hunter.location === newLocation) {
			// A Hunter gains 3 life points each time they rest in a city
			hunter.lifePoints += constants.hunterRestReward;

			// Hunters are not permitted to exceed 9 life points
			if (hunter.lifePoints > constants.hunterMaxLifePoints) {
				hunter.lifePoints = constants.hunterMaxLifePoints;
			}

			// count consecutive rests
			state.hunterRests += 1;

			assert(state.hunterRests <= 4, "too many rests");
		} else {
			state.hunterRests = 0;
		}


		// save the new location of the hunter
		state.currentPlayer.location = newLocation;
	}

	// store past moves
	var pastMoves = state.hunterMoves[state.currentPlayerNumber];

	if (!pastMoves) {
		// create an array for this player
		state.hunterMoves[state.currentPlayerNumber] = [];
		pastMoves = state.hunterMoves[state.currentPlayerNumber];
	}

	pastMoves.push({
		location: newLocation,
		trapsEncountered: trapsEncountered,
		vampiresEncountered: vampiresEncountered,
		draculasEncountered: draculasEncountered
	});

	assert(hunter.location === gameMap.hospitalCityCode || hunter.location === newLocation,
		"Hunter's location invalid. in doHunterMove: "
         + hunter.location + " != " + newLocation);
	assert(state.hunterMoves[state.currentPlayerNumber].length != 0, "Zero length player's moves. doHunterMove");
}

function doDraculaMove(state, move) {
	var dracula = state.dracula;
	var oldestInTrail;

	// Teleport dracula when nowhere to go
	if (!gameMoves.canDraculaMove(state)) {
		// dracula can't move
		move = constants.teleportCode;
	} else {
		assert(gameMoves.isLegalDraculaMove(state, move), "illegal move " + move + " in doDraculaMove");
	}

	/* MOVE PHASE */
	var trailActions = gameTrail.getTrailActions(state);
	var trailLocations = gameTrail.getTrailLocations(state);
	var trailMoves = gameTrail.getTrailMoves(state);

	var roundAction = constants.roundActionOther;

	if (trailLocations.length === constants.trailFullSize) {
		// If the trail is already full (ie has 6 moves in it)
		// then the oldest MOVE in the trail is first removed from the trail.

		oldestActionInTrail = trailActions[0];
		oldestLocationInTrail = trailLocations[0];

		if (oldestActionInTrail === constants.draculaActionPlacedTrap) {
			// if a trap was placed here

			var traps = state.trapsForCity[oldestLocationInTrail];

			// is the trap still there
			if (traps && (state.round - constants.trailFullSize) === traps[0]) {

				// Remove malfunctioned trap from city
				state.trapsForCity[oldestLocationInTrail].shift();

				roundAction = constants.roundActionTrapMalfunction;
			}
		} else if (oldestActionInTrail === constants.draculaActionPlacedVampire) {
			// if a vampire was placed here

			var vampires = state.vampiresForCity[oldestLocationInTrail];

			// vampire hasn't been killed
			if (vampires && (state.round - constants.trailFullSize) === vampires[0]) {
				// remove the vampire
				state.vampiresForCity[oldestLocationInTrail].shift();

				// The score decreases by 13 each time a Vampire matures (i.e. falls off the trail)
				state.score -= constants.vampireMatureScorePenalty;
				roundAction = constants.roundActionVampireMatured;
			}
		}

	}


	// Dracula's current MOVE is placed in the trail.
	assert(move, "move is undefined");
	state.draculaMoves.push(move);



	/* ACTION PHASE */

	state.roundActions.push(roundAction);

	// Resolve locations
	var newTrailLocations = gameTrail.getTrailLocations(state);
	var mostRecentTrailIndex = newTrailLocations.length-1;

	// get dracula's location
	// dracula is now at the most recent location in the trail
	dracula.location = newTrailLocations[mostRecentTrailIndex];

	var locationInfo = gameMap.locationInfo[dracula.location];
	assert(locationInfo, "no location info for: " + dracula.location + ". in doDraculaAction");

	var draculaAction;

	if (locationInfo.isSea) {
		// If Dracula is at sea he loses two blood points.
		draculaAction = constants.draculaActionAtSea;
		dracula.bloodPoints -= constants.draculaSeaPenalty;
	} else if (locationInfo.isCity) {
		// If he is in a city Dracula places an Encounter in that city.


		// Dracula cannot place an Encounter in a city if there are already 3 Encounters in that city.
		var numEncounters = 0;

		if (state.vampiresForCity[dracula.location]) {
			numEncounters += state.vampiresForCity[dracula.location].length;
		}
		if (state.trapsForCity[dracula.location]) {
			numEncounters += state.trapsForCity[dracula.location].length;
		}

		if (numEncounters < 3) {

			// Dracula places an immature Vampire if he is in a city
			// AND if the round whose number is divisible by 13 (ie in round 0, then in round 13, then 26, and so on).
			var isVampireDrop = ((state.round % constants.vampireInterval) == 0);
			if (isVampireDrop) {
				draculaAction = constants.draculaActionPlacedVampire;
				// add vampire to the current city
				if (!state.vampiresForCity[dracula.location]) {
					state.vampiresForCity[dracula.location] = [];
				}

				state.vampiresForCity[dracula.location].push(state.round);
			} else {
				// Otherwise he places a Trap.
				draculaAction = constants.draculaActionPlacedTrap;
				// add trap to the current city
				if (!state.trapsForCity[dracula.location]) {
					state.trapsForCity[dracula.location] = [];
				}
				state.trapsForCity[dracula.location].push(state.round);
			}
		} else {
			console.log("Dracula does not place an Encounter this turn...");
			if (!state.vampiresForCity[dracula.location]) {
				console.log("City contains " + state.trapsForCity[dracula.location].length +" traps already.");
			} else {
				console.log("City contains " + state.vampiresForCity[dracula.location].length + " vampires and " +
                  state.trapsForCity[dracula.location].length +" traps already.");
			}
		}
	}

	if (locationInfo.isCastleDracula && !state.isOver) {
		// Dracula regains 10 blood points if he is in Castle Dracula at the end of his turn
		// and has not yet been defeated
		state.dracula.bloodPoints += constants.draculaCastleReward;
	}

	// The score is reduced by 1 point
	state.score -= 1;

	// save dracula's action
	state.draculaActions.push(draculaAction);

	var knowledgeItem;

	// Update hunter knowledge of this move
	if (gameTrail.isDoublebackCode(move)) {
		// Hunters know he has made a DOUBLE_BACK
		// and which item in the trail he has doubled back to
		// but not the location
		knowledgeItem = move;
	} else if (locationInfo.isCity) {
		// otherwise if dracula's in a city, unknown city
		knowledgeItem = constants.unknownCityCode;
	} else if (locationInfo.isSea) {
		// unknown sea
		knowledgeItem = constants.unknownSeaCode;
	}

	assert(knowledgeItem, "knowledgeItem undefined");
	state.hunterKnowledge.push(knowledgeItem);

	if ((isHunterInLocation(state, dracula.location) && locationInfo.isCity)
         || dracula.location === gameMap.castleDraculaCode) {
		// The Hunters can see Dracula's location whenever he ends his turn in his
		// castle, or in a city currently occupied by a Hunter.
		gameTrail.revealTrailItem(state, mostRecentTrailIndex);
	}

	// The Hunters can see whenever the sixth move in the trail is a HIDE move.
	// This does not mean that they learn the nature of the move to which it relates


	var newTrailMoves = gameTrail.getTrailMoves(state);
	if (newTrailMoves[0] === constants.hideMove) {
		var oldestTrailMoveIndex = gameTrail.getOldestTrailIndex(state);
		state.hunterKnowledge[oldestTrailMoveIndex] = state.draculaMoves[oldestTrailMoveIndex];
	}
}

function isHunterInLocation(state, location) {
	var hunters = state.hunters;

	var extractLocation = function(hunter) {
		return hunter.location;
	};

	var hunterLocations = list.map(hunters, extractLocation);

	return list.contains(hunterLocations, location);
}

function doResearch(state) {
	// If a Hunter rests, and each of the other Hunters rested in their most recent turn,
	// then we say they have been doing collaborative research,
	// and the 6th move in Dracula's trail is immediately revealed.
	// nothing is revealed if there are not yet 6 moves in the trail
	if (state.round > 5) {
		// reveal the oldest item in the trail
		gameTrail.revealTrailItem(state, 0);
	}
}

module.exports = {
	doHunterMove: doHunterMove,
	doDraculaMove: doDraculaMove,
	doResearch: doResearch
};
