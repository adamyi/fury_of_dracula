////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// map.c: an implementation of a Map type
// You can change this as much as you want!
//
// 2017-11-30	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "map.h"
#include "places.h"

typedef struct vNode *VList;
struct vNode {
	enum location_id  v;    // ALICANTE, etc
	enum transport_type type; // ROAD, RAIL, BOAT
	VList       next; // link to next node
};

struct map {
	int nV; // #vertices
	int nE; // #edges
	VList connections[NUM_MAP_LOCATIONS]; // array of lists
};

static void addConnections (Map m);
static inline bool is_sentinel_edge (connection);

// Create a new empty graph (for a map)
// #Vertices always same as NUM_PLACES
Map
newMap (void)
{
	Map g = malloc (sizeof *g);
	if (g == NULL) err (EX_OSERR, "couldn't allocate Map");

	(*g) = (struct map){
		.nV = NUM_MAP_LOCATIONS,
		.nE = 0,
		.connections = { NULL }
	};

	addConnections (g);
	return g;
}

// Remove an existing graph
void
disposeMap (Map g)
{
	// wait, what?
	if (g == NULL) return;

	for (int i = 0; i < g->nV; i++) {
		VList curr = g->connections[i];
		while (curr != NULL) {
			VList next = curr->next;
			free (curr);
			curr = next;
		}
	}
	free (g);
}

static VList
insertVList (VList L, enum location_id v, enum transport_type type)
{
	VList new = malloc (sizeof(struct vNode));
	if (new == NULL) err (EX_OSERR, "couldn't allocate vNode");

	(*new) = (struct vNode){
		.v = v,
		.type = type,
		.next = L
	};
	return new;
}

static int
inVList (VList L, enum location_id v, enum transport_type type)
{
	for (VList cur = L; cur != NULL; cur = cur->next)
		if (cur->v == v && cur->type == type)
			return 1;

	return 0;
}

// Add a new edge to the Map/Graph
static void
addLink (Map g, enum location_id start, enum location_id end, enum transport_type type)
{
	assert (g != NULL);

// don't add edges twice
	if (inVList (g->connections[start], end, type)) return;

	g->connections[start] = insertVList(g->connections[start],end,type);
	g->connections[end] = insertVList(g->connections[end],start,type);
	g->nE++;
}

static const char *
typeToString (enum transport_type t)
{
	switch (t) {
	case ROAD: return "road";
	case RAIL: return "rail";
	case BOAT: return "boat";
	default:   return "????";
	}
}

// Display content of Map/Graph
void
showMap (Map g)
{
	assert (g != NULL);

	printf ("V=%d, E=%d\n", g->nV, g->nE);
	for (int i = 0; i < g->nV; i++)
		for (VList n = g->connections[i]; n != NULL; n = n->next)
			printf ("%s connects to %s by %s\n",
					idToName (i), idToName (n->v), typeToString (n->type));
}

// Return count of nodes
int
numV (Map g)
{
	assert (g != NULL);
	return g->nV;
}

// Return count of edges of a particular type
int
numE (Map g, enum transport_type type)
{
	assert (g != NULL);
	assert (0 <= type && type <= ANY);

	int nE = 0;
	for (int i = 0; i < g->nV; i++)
		for (VList n = g->connections[i]; n != NULL; n = n->next)
			if (n->type == type || type == ANY)
				nE++;

	return nE;
}

// Add edges to Graph representing map of Europe
static void
addConnections (Map g)
{
	for (int i = 0; ! is_sentinel_edge (CONNECTIONS[i]); i++)
		addLink (g, CONNECTIONS[i].v, CONNECTIONS[i].w, CONNECTIONS[i].t);
}

static inline bool is_sentinel_edge (connection x)
{
	return x.v == -1 && x.w == -1 && x.t == ANY;
}
