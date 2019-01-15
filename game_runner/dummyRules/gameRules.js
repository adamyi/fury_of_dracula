var rules = {
	state: {
		number: 0,
		messages: []
	},
   
	numPlayers: 5,
   
	getGameOutput: function(state) {
		return state.number;
	},
   
	getPlayerInput: function(state) {
		return { pastPlays: "Hello!", messages: state.messages };
	},
   
	isGameOver: function(state) {
		return state.number >= 420;
	},
   
	isPlayerDisqualified: function(state) {
		return false;
	},
   
	isLegalMove: function(state, move) {
		return true;
	},
   
	playDefaultMove: function(state) {
      
	},
   
	playMove: function(state, move) {
		if (!move.move) {
			move.move = 0;
			move.message = "Invalid Move";
		}
      
		state.messages.push(move.message);
      
		var number = parseInt(move.move);
      
		if (!isNaN(number)) {
			state.number += number;
		}
	},
   
	endTurn: function(state) {
      
	},
   
	endRound: function(state) {
      
	}
};

module.exports = rules;
