////////////////////////////////////////////////////////////////////////
// Runs a player's game turn ...
//
// Can produce either a Hunter player or a Dracula player
// depending on the setting of the I_AM_DRACULA #define
//
// This is a dummy version of the real player.c used when you submit
// your AIs. It is provided so that you can test whether your code is
// likely to compile ...
//
// Note that it used to drive both Hunter and Dracula AIs.  It first
// creates an appropriate view, and then invokes the relevant
// decide-my-move function, which should use the register_best_play()
// function to send the move back.
//
// The real player.c applies a timeout, so that it will halt the AI
// after a fixed amount of time, if it doesn't finish first.  The last
// move registered will be the one used by the game engine.  This
// version of player.c won't stop your decide-my-move function if it
// goes into an infinite loop.  Sort that out before you submit.
//
// Based on the program by David Collien, written in 2012
//
// 2017-12-04	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v1.2	Team Dracula <cs2521@cse.unsw.edu.au>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#ifdef I_AM_DRACULA
# include "dracula.h"
# include "dracula_view.h"
#else
# include "hunter.h"
# include "hunter_view.h"
#endif

// moves given by register_best_play are this long (including terminator)
#define MOVE_SIZE 3

// The minimum static globals I can get away with
static char latest_play[MOVE_SIZE] = "";
static char latest_message[MESSAGE_SIZE] = "";

// A pseudo-generic interface, which defines
// - a type `view_t',
// - functions `view_new', `decide_move', `view_drop',
// - a trail `xtrail', and a message buffer `xmsgs'.
#ifdef I_AM_DRACULA
typedef DraculaView view_t;
# define view_new dv_new
# define decide_move decide_dracula_move
# define view_drop dv_drop

# define xtrail "GZA.... SED.... HZU.... MZU...."
# define xmsgs { "", "", "", "" }
#else
typedef HunterView view_t;
# define view_new hv_new
# define decide_move decide_hunter_move
# define view_drop hv_drop

# define xtrail "GZA.... SED.... HZU...."
# define xmsgs { "", "", "" }
#endif

int main (void)
{
	char *trail = xtrail;
	player_message msgs[] = xmsgs;

	view_t state = view_new (trail, msgs);
	decide_move (state);
	view_drop (state);

	printf ("Move: %s, Message: %s\n", latest_play, latest_message);
	return EXIT_SUCCESS;
}

// Saves characters from play (and appends a terminator)
// and saves characters from message (and appends a terminator)
void register_best_play (char *play, player_message message)
{
	strncpy (latest_play, play, MOVE_SIZE - 1);
	latest_play[MOVE_SIZE - 1] = '\0';

	strncpy (latest_message, message, MESSAGE_SIZE);
	latest_message[MESSAGE_SIZE - 1] = '\0';
}
