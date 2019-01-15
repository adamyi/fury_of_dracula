module.exports = {
	hunterMaxLifePoints: 9,
	hideMove: "HI",
	doublebackPrefix: "D",
	doublebackMoves: [
		"D1",
		"D2",
		"D3",
		"D4",
		"D5"
	],
   
	draculaPlayerNumber: 4,
   
	// The game score starts at 366 points.
	startScore: 366,
   
	unknownCityCode: "C?",
   
	unknownSeaCode: "S?",
   
	teleportCode: "TP",
   
	draculaActionAtSea: "S",
	draculaActionPlacedTrap: "T",
	draculaActionPlacedVampire: "V",
   
	roundActionTrapMalfunction: "M",
	roundActionVampireMatured: "V",
	roundActionOther: ".",
   
	// The score decreases by 6 each time a Hunter
	// is teleported to the hospital.
	hospitalTeleportScorePenalty: 6,
   
	// A Hunter gains 3 life points each time they rest in a city
	hunterRestReward: 3,
   
	// If Dracula is at sea he loses two blood points.
	draculaSeaPenalty: 2,
   
	// Dracula places an immature Vampire if he is in a city
	// AND if the round whose number is divisible by 13
	// (ie in round 0, then in round 13, then 26, and so on).
	vampireInterval: 13,

	// When a Trap is encountered the Hunter instantly loses 2 life points
	hunterTrapPenalty: 2,
   
	// If Dracula is encountered then he
	// instantly loses 10 blood points
	draculaHunterBloodPenalty: 10,
   
	// and the Hunter instantly loses 4 life points.
	hunterDraculaLifePenalty: 4,

	// Dracula regains 10 blood points if he is in
	// Castle Dracula at the end of his turn
	// and has not yet been defeated
	draculaCastleReward: 10,
   
	// The score decreases by 13 each time a Vampire matures
	// (i.e. falls off the trail)
	vampireMatureScorePenalty: 13,
   
	// the trail is already full (ie has 6 moves in it)
	trailFullSize: 6,
   
	// Dracula starts with 40 blood points.
	draculaStartBloodPoints: 40
};