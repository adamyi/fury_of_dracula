////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// Game.h: global game definitions for "Fury of Dracula"
//
// 2012-01-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-11-25	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
// // // // // // // // DO NOT CHANGE THIS FILE! // // // // // // // //

#ifndef FOD__GAME_H_
#define FOD__GAME_H_

/** Players */
enum player {
	PLAYER_LORD_GODALMING,	/**< Lord Godalming, hunter */
	PLAYER_DR_SEWARD,		/**< Dr Seward, hunter */
	PLAYER_VAN_HELSING,		/**< Van Helsing, hunter */
	PLAYER_MINA_HARKER,		/**< Mina Harker, hunter */
	PLAYER_DRACULA			/**< Dracula, the vampire */
};
enum { NUM_PLAYERS = 5 };

/** Game scoring.
 * Dracula wants the game score to be low.
 * Hunters want the game score to be high. */
enum {
	/** The initial game score. */
	GAME_START_SCORE			= 366,
	/** The game score lost when Dracula makes a turn. */
	SCORE_LOSS_DRACULA_TURN		= 1,
	/** The game score lost when a hunter 'dies',
	 * and is teleported to hospital. */
	SCORE_LOSS_HUNTER_HOSPITAL	= 6,
	/** The game score lost when one of Dracula's minions matures. */
	SCORE_LOSS_VAMPIRE_MATURES	= 13
};

/** Dracula's health, in 'blood points'. */
enum {
	/** Dracula's initial health. */
	GAME_START_BLOOD_POINTS			= 40,
	/** Blood points lost when Dracula encounters a hunter. */
	LIFE_LOSS_HUNTER_ENCOUNTER		= 10,
	/** Blood points lost when Dracula is at sea. */
	LIFE_LOSS_SEA					= 2,
	/** Blood points gained when Dracula is at Castle Dracula. */
	LIFE_GAIN_CASTLE_DRACULA		= 10,
};

/** Hunter health, in 'life points'. */
enum {
	/** Initial hunter health */
	GAME_START_HUNTER_LIFE_POINTS	= 9,
	/** Life points lost when hunter encounters a trap. */
	LIFE_LOSS_TRAP_ENCOUNTER		= 2,
	/** Life points lost when hunter encounters Dracula. */
	LIFE_LOSS_DRACULA_ENCOUNTER		= 4,
	/** Life points gained when hunter rests. */
	LIFE_GAIN_REST					= 3,
};

/** The length of a trail. */
enum { TRAIL_SIZE = 6 };

enum { TURN_LIMIT_MSECS = 1500 };
enum { MESSAGE_SIZE = 100 };

/** a type to contain a message (or taunt) provided by a player */
typedef char player_message[MESSAGE_SIZE];

/** The round number we're up to. */
typedef int round_t;

/**
 * Register your best move (so far) with the game engine.
 * This overrides any previous moves provided by your player.
 *
 * The array `message` will be stored in the game history;
 * it must be a well-formed string.
 */
void register_best_play (char *play, player_message message);

#endif // !defined(FOD__GAME_H_)
