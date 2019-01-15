var functionDecorate = require("../jsFun/functionMagic");
functionDecorate(Function);

var assert = require("../jsFun/assert");
var list = require("../jsFun/listLike");

var constants  = require("./gameConstants");
var gameMap = require("./gameMap");

var hunterSuffix = function(turnData) {
	// Hunter Play
	var play = "";
      
	// 2 uppercase characters representing the new location of the hunter.
	assert(turnData.location, "playerMoves location is undefined. in hunterSuffix");
	play = play.concat(turnData.location);
   
	// 4 letters representing, in order, the encounters that occurred:
	var traps = list.make(String, "T", turnData.trapsEncountered);
	var vampires = list.make(String, "V", turnData.vampiresEncountered);
	var draculas = list.make(String, "D", turnData.draculasEncountered);
   
	play = play.concat(traps + vampires + draculas);
   
	// then '.'s for the remaining 0..4 characters
	var numDots = 4 - (traps.length + vampires.length + draculas.length);
	var dots = "" + list.make(String, ".", numDots);
   
	play = play.concat(dots);
   
	return play;
};

var generatePlay = function(state, playerType, turnNumber) {
	var isDracula = (playerType === "dracula");
   
	var numPlayers = state.playSequence.length;
	var playerNumber = turnNumber % numPlayers;
	var roundNumber = Math.floor(turnNumber / numPlayers);
   
	var turnData;
	var playerKnowledge;
	var draculaAction;
	var roundAction;
   
	var play = "";
   
	if (playerNumber === constants.draculaPlayerNumber) {
		// Make dracula play
      
		if (isDracula) {
			playerKnowledge = state.draculaMoves[roundNumber];
		} else {
			playerKnowledge = state.hunterKnowledge[roundNumber];
		}
      
		draculaAction = state.draculaActions[roundNumber];
		roundAction = state.roundActions[roundNumber];
      
		play = play.concat(state.dracula.playCode);
      
		assert(playerKnowledge, "player knowledge is undefined. drac moves:\n"
         + state.draculaMoves + " hunter knowledge:\n"
         + state.hunterKnowledge + "\n round number: " + roundNumber);
      
		// 2 uppercase characters representing the new location of Dracula
		play = play.concat(playerKnowledge);
      
		// 2 characters representing the encounter Dracula placed
		// 'T' if a Trap was placed, otherwise '.'
		if (draculaAction === constants.draculaActionPlacedTrap) {
			play = play.concat("T");
		} else {
			play = play.concat(".");
		}
      
		// 'V' if an immature Vampire was placed, otherwise '.'
		if (draculaAction === constants.draculaActionPlacedVampire) {
			play = play.concat("V");
		} else {
			play = play.concat(".");
		}
      
		// 1 character representing the action phase of Dracula's turn
		// N.B. "action phase" is a bit misleading, I call it 'round action'
		if (roundAction === constants.roundActionTrapMalfunction) {
			// 'M' if a Trap has left the trail (malfunctions due to old age)
			play = play.concat("M");
		} else if (roundAction === constants.roundActionVampireMatured) {
			// 'V' if a Vampire has matured. (eek!)
			play = play.concat("V");
		} else {
			// '.' = Nothing has occurred (eg early in game)
			play = play.concat(".");
		}
      
		// a single '.' character
		play = play.concat(".");
	} else {
		// Make hunter play
		turnData = state.hunterMoves[playerNumber][roundNumber];
		assert(turnData, "generateHunterPlay");
      
		play = play.concat(state.playSequence[playerNumber].playCode);
		play = play.concat(hunterSuffix(turnData));
	}
   
	assert(play.length == 7, "Each play is 7 characters long: \"" + play + "\" = " + play.length + ". in generatePlay");
   
	return play;
};

module.exports = {
	generatePastPlays: function(state, playerType) {
		if (!playerType) {
			playerType = state.currentPlayer.type;
		}
      
		var numPlayers = state.playSequence.length;
		var currentTurn = (state.round * numPlayers) + state.currentPlayerNumber;
      
		var playMaker = generatePlay.partialApply(state, playerType);
      
		var pastPlays = list.make(Array, playMaker, currentTurn);
      
		assert(pastPlays.length === currentTurn, "pastPlays of incorrect length. in generatePastPlays");
      
		return pastPlays.join(" ");
	}
};
