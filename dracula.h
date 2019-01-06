////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// dracula.h: the interface to your "Fury of Dracula" Dracula AI
//
// 2012-01-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
// // // // // // // // DO NOT CHANGE THIS FILE! // // // // // // // //

#ifndef FOD__DRACULA_H_
#define FOD__DRACULA_H_

#include "dracula_view.h"

/**
 * For a given view of the game, decide what the best next move is for
 * Dracula by calling `register_best_play`.
 */
void decide_dracula_move (DraculaView state);

#endif // !defined(FOD__DRACULA_H_)
