////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// hunter.h: the interface to your "Fury of Dracula" Hunter AI
//
// 2012-01-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
// // // // // // // // DO NOT CHANGE THIS FILE! // // // // // // // //

#ifndef FOD__HUNTER_H_
#define FOD__HUNTER_H_

#include "hunter_view.h"

/**
 * For a given view of the game, decide what the best next move is for
 * the current hunter by calling `register_best_play`.
 */
void decide_hunter_move (HunterView state);

#endif // !defined(FOD__HUNTER_H_)
