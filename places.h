////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// places.h: an interface to the Places
//
// 2012-01-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-11-25	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
// // // // // // // // DO NOT CHANGE THIS FILE! // // // // // // // //

#include <stdbool.h>
#include <stdlib.h>

#ifndef FOD__PLACES_H_
#define FOD__PLACES_H_

// Types of places
typedef enum place_type {
	UNKNOWN,
	LAND,
	SEA,
} place_type_t;

// Types of connections between places
typedef enum transport {
	NONE,
	ROAD,
	RAIL,
	BOAT,
	ANY
} transport_t;

#define MIN_TRANSPORT (transport_t) ROAD
#define MAX_TRANSPORT (transport_t) BOAT

// Places
typedef enum location {
	ADRIATIC_SEA,
	ALICANTE,
	AMSTERDAM,
	ATHENS,
	ATLANTIC_OCEAN,
	BARCELONA,
	BARI,
	BAY_OF_BISCAY,
	BELGRADE,
	BERLIN,
	BLACK_SEA,
	BORDEAUX,
	BRUSSELS,
	BUCHAREST,
	BUDAPEST,
	CADIZ,
	CAGLIARI,
	CASTLE_DRACULA,
	CLERMONT_FERRAND,
	COLOGNE,
	CONSTANTA,
	DUBLIN,
	EDINBURGH,
	ENGLISH_CHANNEL,
	FLORENCE,
	FRANKFURT,
	GALATZ,
	GALWAY,
	GENEVA,
	GENOA,
	GRANADA,
	HAMBURG,
	IONIAN_SEA,
	IRISH_SEA,
	KLAUSENBURG,
	LE_HAVRE,
	LEIPZIG,
	LISBON,
	LIVERPOOL,
	LONDON,
	MADRID,
	MANCHESTER,
	MARSEILLES,
	MEDITERRANEAN_SEA,
	MILAN,
	MUNICH,
	NANTES,
	NAPLES,
	NORTH_SEA,
	NUREMBURG,
	PARIS,
	PLYMOUTH,
	PRAGUE,
	ROME,
	SALONICA,
	SANTANDER,
	SARAGOSSA,
	SARAJEVO,
	SOFIA,
	ST_JOSEPH_AND_ST_MARYS,
	STRASBOURG,
	SWANSEA,
	SZEGED,
	TOULOUSE,
	TYRRHENIAN_SEA,
	VALONA,
	VARNA,
	VENICE,
	VIENNA,
	ZAGREB,
	ZURICH,

	// Other "locations"
	CITY_UNKNOWN = 100,
	SEA_UNKNOWN = 101,
	HIDE = 102,
	DOUBLE_BACK_1 = 103,
	DOUBLE_BACK_2 = 104,
	DOUBLE_BACK_3 = 105,
	DOUBLE_BACK_4 = 106,
	DOUBLE_BACK_5 = 107,
	TELEPORT = 108,
	UNKNOWN_LOCATION = -1,
	NOWHERE = UNKNOWN_LOCATION,
} location_t;

#define MIN_MAP_LOCATION	(location_t) ADRIATIC_SEA
#define HOSPITAL_LOCATION	(location_t) ST_JOSEPH_AND_ST_MARYS
#define MAX_MAP_LOCATION	(location_t) ZURICH
#define NUM_MAP_LOCATIONS	((int) ZURICH + 1)

typedef struct place place;
extern struct place {
	char *name, *abbrev;
	location_t id;
	place_type_t type;
} PLACES[];

typedef struct connection connection;
extern struct connection {
	location_t v, w;
	transport_t t;
} CONNECTIONS[];

/** given a place number, return its name */
const char *location_get_name (location_t place);

/** given a place number, return its two-char code */
const char *location_get_abbrev (location_t place);

/** given a place number, return its type */
place_type_t location_get_type (location_t place);

/** given a place name, return its ID number */
location_t location_find_by_name (char *name);

/** given a place abbreviation, return its ID number */
location_t location_find_by_abbrev (char *abbrev);

/** given a type of transport, return a string representation. */
const char *transport_to_s (transport_t t);

/** given a location, is it land? */
static inline bool land_p (location_t loc)
{
	return location_get_type (loc) == LAND;
}

/** given a location, is it a sea? */
static inline bool sea_p (location_t loc)
{
	return location_get_type (loc) == SEA;
}

/** is the given location valid? */
static inline bool valid_location_p (location_t pid)
{
	return MIN_MAP_LOCATION <= pid
		&& pid <= MAX_MAP_LOCATION;
}

/** is the given mode of transport valid? */
static inline bool valid_transport_p (transport_t tid)
{
	return ROAD <= tid
		&& tid <= BOAT;
}


#endif // !defined (FOD__PLACES_H_)
