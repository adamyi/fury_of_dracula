var gameState = require("./gameState");
var gameLogic = require("./gameLogic");
var gameMoves = require("./gameMoves");
var gameMap   = require("./gameMap");

var gameSerialiser = require("./gameSerialiser");


var assert = require("../jsFun/assert");

var rules = {
	state: gameState,

	numPlayers: 5,

	getGameOutput: function(state) {
		return {
			score: state.score,
			revealedPastPlays: gameSerialiser.generatePastPlays(state, "dracula"),
			messages: JSON.stringify(state.messages)
		};
	},

	getPlayerInput: function(state) {
		return {
			past_plays: gameSerialiser.generatePastPlays(state),
			messages: state.messages
		};
	},

	isGameOver: function(state) {
		return state.isOver;
	},

	playDefaultMove: function(state) {
		var move;

		var randomMessages = [
			"===== Generating default move...player disqualified... ====="
		];

		var randomIndex = Math.floor(Math.random() * randomMessages.length);
		var message = randomMessages[randomIndex];
		state.messages.push(message);

		if (state.currentPlayer.type === "hunter") {
			//console.log("generating default hunter move");
			if (state.round === 0) {
				move = gameMap.hospitalCityCode;
			} else {
				move = state.currentPlayer.location;
			}

			//console.log("> Moving from: " + state.currentPlayer.location);
			//console.log("> Life Points: " + state.currentPlayer.lifePoints);
			console.log("Default Hunter Move " + JSON.stringify(move));
			gameLogic.doHunterMove(state, move);


		} else {
			//console.log("generating default dracula move");
			var legalDraculaMoves = gameMoves.getValidDraculaMoves(state);

			if (legalDraculaMoves.length === 0) {
				move = constants.teleportCode;
			} else {
				var randomIndex = Math.floor(Math.random() * legalDraculaMoves.length);
				move = legalDraculaMoves[randomIndex];
			}



			//console.log("> Dracula move: ");
			//console.log("> +++");
			//console.log("  move: " + move);
			//console.log("  message: " + message);
			//console.log("> ---");

			//console.log("> Moving from: " + state.currentPlayer.location);
			//console.log("> Blood Points: " + state.currentPlayer.bloodPoints);

			console.log("Default Dracula Move: " + JSON.stringify(move));
			gameLogic.doDraculaMove(state, move);
		}
	},

	isLegalMove: function(state, playerResult) {
		var move = playerResult.move;

		if (state.currentPlayer.type === "hunter") {
			return gameMoves.isLegalHunterMove(state, move);
		} else {
			if (!gameMoves.canDraculaMove(state)) {
				return true; // any move is legal if dracula can't move
			} else {
				return gameMoves.isLegalDraculaMove(state, move);
			}
		}
	},

	playMove: function(state, playerResult) {
		var move, message;

		move = playerResult.move;
		message = playerResult.message;

		if (!message) {
			message = "";
		}

		// add the message to the state
		state.messages.push(message);

		console.log("> Game score: " + state.score);
		console.log("> Moving from: " + state.currentPlayer.location);

		if (state.currentPlayer.type === "hunter") {
			console.log("> Life Points: " + state.currentPlayer.lifePoints);
		} else {
			console.log("> Blood Points: " + state.currentPlayer.bloodPoints);
			this.dracMoves(state);
		}

		if (state.currentPlayer.type == "hunter") {
			assert(gameMoves.isLegalHunterMove(state, move));
			gameLogic.doHunterMove(state, move);
		} else if (state.currentPlayer.type == "dracula") {
			if (gameMoves.canDraculaMove(state)) {
				assert(gameMoves.isLegalDraculaMove(state, move));
			}
			gameLogic.doDraculaMove(state, move);
		}
	},

	dracMoves: function(state) {
		if (state.currentPlayer.type == "dracula") {
			var ms = gameMoves.getValidDraculaMoves(state);
			if (ms.length < 10) {
				m = "> Legal Drac moves:";
				for (i = 0; i < ms.length; i++) m += " "+ms[i];
				console.log(m);
			}
		}
	},

	endTurn: function(state) {
		if (state.dracula.bloodPoints <= 0) {
			// When Dracula has no blood points left the game is over
			state.isOver = true;
		} else if (state.score <= 0) {
			// if [the score] ever reaches 0 we say Dracula has utterly won,
			// the world is doomed, and the game stops at once. We all look
			// accusingly at the Hunters...
			state.isOver = true;
		}
		var util = require("util");

		// do research
		if (state.hunterRests === 4) {
			//console.log(util.inspect(state.hunterKnowledge));
			gameLogic.doResearch(state);
			//console.log(util.inspect(state.hunterKnowledge));
			state.hunterRests = 3;
		}

		// next player
		state.currentPlayerNumber = (state.currentPlayerNumber + 1) % rules.numPlayers;
		state.currentPlayer = state.playSequence[state.currentPlayerNumber];
	},

	endRound: function(state) {
		console.log("> End of Round: ", state.round);

		state.round++;

		console.log("> Starting Round: ", state.round);
	}
};

module.exports = rules;
