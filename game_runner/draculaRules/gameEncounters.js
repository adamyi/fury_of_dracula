var functionDecorate = require("../jsFun/functionMagic");
functionDecorate(Function);

var list = require("../jsFun/listLike");
var assert = require("../jsFun/assert");

var constants = require("./gameConstants");
var gameMap = require("./gameMap");

function doHunterTrapEncounters(state, newLocation){
	var hunter = state.currentPlayer;
	var numEncounters = 0;
   
	if (!state.trapsForCity[newLocation]){
		// nothing to see here
		return 0;
	}

	// the Hunter encounters the Traps one at a time until either
	// the Hunter is reduced to 0 or less life points,
	// or until all the Traps are encountered, whichever occurs first.
	while (state.trapsForCity[newLocation].length != 0 && hunter.lifePoints > 0){
		// When a Trap is encountered the Hunter instantly loses 2 life points
		// and the Trap is destroyed.
		hunter.lifePoints -= constants.hunterTrapPenalty;
      
		numEncounters += 1;
      
		// kill the oldest trap
		state.trapsForCity[newLocation].shift();
	}
   
	return numEncounters;
}

function doHunterVampireEncounters(state, newLocation){
	var hunter = state.currentPlayer;
	var numEncounters = 0;
   
	if (!state.vampiresForCity[newLocation]){
		return 0;
	}
   
	while (state.vampiresForCity[newLocation].length != 0 && hunter.lifePoints > 0){
		// When an immature Vampire is encountered
		// it is killed instantly with no effect to the Hunter.  (well done!)
		numEncounters += 1;
      
		// kill the oldest vampire in that city
		state.vampiresForCity[newLocation].shift();
	}
   
	return numEncounters;
}

function doHunterDraculaEncounter(state, newLocation){
	var numEncounters = 0;
	var hunter = state.currentPlayer;
   
	// Unfortunately Hunters can never encounter Dracula while they are at sea
	var areInSameCity = (state.dracula.location === newLocation)
      && gameMap.locationInfo[newLocation].isCity;
   
	if (hunter.lifePoints > 0){
		if (areInSameCity){
			// If Dracula is encountered then he
			// instantly loses 10 blood points
			state.dracula.bloodPoints -= constants.draculaHunterBloodPenalty;
			// and the Hunter instantly loses 4 life points.
			hunter.lifePoints -= constants.hunterDraculaLifePenalty;
         
			numEncounters = 1;
		}
	}
   
	return numEncounters;
}

module.exports = {
	doHunterTrapEncounters: doHunterTrapEncounters,
	doHunterVampireEncounters: doHunterVampireEncounters,
	doHunterDraculaEncounter: doHunterDraculaEncounter
};
