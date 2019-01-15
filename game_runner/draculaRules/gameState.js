var constants = require("./gameConstants");

var Player = function(playCode) {
	this.playCode = playCode;
   
	if (playCode === "G") {
		this.name = "Lord Godalming";
		this.type = "hunter";
		this.lifePoints = constants.hunterMaxLifePoints;
	} else if (playCode === "S") {
		this.name = "Dr Seward";
		this.type = "hunter";
		this.lifePoints = constants.hunterMaxLifePoints;
	} else if (playCode === "H") {
		this.name = "Van Helsing";
		this.type = "hunter";
		this.lifePoints = constants.hunterMaxLifePoints;
	} else if (playCode === "M") {
		this.name = "Mina Harker";
		this.type = "hunter";
		this.lifePoints = constants.hunterMaxLifePoints;
	} else if (playCode === "D") {
		this.name = "Dracula";
		this.type = "dracula";
		this.bloodPoints = constants.draculaStartBloodPoints;
	} else {
		assert("Invalid Play Code in Player");
	}
};

var State = function() {
	this.hunters = [
		new Player("G"),
		new Player("S"),
		new Player("H"),
		new Player("M")
	];
   
	this.dracula = new Player("D");
   
	this.playSequence = this.hunters.concat([this.dracula]);
   
	this.currentPlayer = this.playSequence[0];
	this.currentPlayerNumber = 0;
   
	this.messages = [];
   
   
	// all dracula's moves
	// as move strings
	this.draculaMoves = [];
   
	// all dracula's actions
	// as 'S', 'V' or 'T'
	// at sea, placed vampire, placed trap
	this.draculaActions = [];
   
	// vampire mature: 'V'
	// trap malfunction: 'M'
	// nothing: '.'
	this.roundActions = [];
   
	// move histories
	this. hunterMoves = [];
	// of
	/*
   {
      trapsEncountered: 0,
      vampiresEncountered: 0,
      draculasEncountered: 0,
      location: 'AB'
   }
   */
   
	this.hunterKnowledge = [];
   
	// lookup per city for array of round numbers when dropped
	this.trapsForCity = { };
	this.vampiresForCity = { };
   
	this.hunterRests = 0;
   
	this.score = constants.startScore;
   
	this.round = 0;
   
	this.isOver = false;
};

module.exports = State;
