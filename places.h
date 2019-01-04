////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// places.h: an interface to the Places
//
// 2012-01-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-11-25	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
// // // // // // // // DO NOT CHANGE THIS FILE! // // // // // // // //

#ifndef FOD__PLACES_H_
#define FOD__PLACES_H_

// Types of places
enum place_type {
	UNKNOWN,
	LAND,
	SEA,
};

// Types of connections between places
enum transport_type {
	NONE,
	ROAD,
	RAIL,
	BOAT,
	ANY
};

#define MIN_TRANSPORT (enum transport_id)ROAD
#define MAX_TRANSPORT (enum transport_id)BOAT

// Places
enum location_id {
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
};

#define MIN_MAP_LOCATION	(enum location_id) ADRIATIC_SEA
#define HOSPITAL_LOCATION	(enum location_id) ST_JOSEPH_AND_ST_MARYS
#define MAX_MAP_LOCATION	(enum location_id) ZURICH
#define NUM_MAP_LOCATIONS	((int) ZURICH + 1)

typedef struct place place;
extern struct place {
	char *name, *abbrev;
	enum location_id id;
	enum place_type type;
} PLACES[];

typedef struct connection connection;
extern struct connection {
	enum location_id v, w;
	enum transport_type t;
} CONNECTIONS[];

// given a Place number, return its name
char *idToName (enum location_id place);

// given a Place number, return its two-char code
char *idToAbbrev (enum location_id place);

// given a Place number, return its type
enum place_type idToType (enum location_id place);

// given a Place name, return its ID number
enum location_id nameToID (char *name);

// given a Place abbreviation, return its ID number
enum location_id abbrevToID (char *abbrev);

#define isLand(place) (idToType (place) == LAND)
#define isSea(place)  (idToType (place) == SEA)
#define validPlace(pid) \
	(MIN_MAP_LOCATION <= (pid) && (pid) <= MAX_MAP_LOCATION)

#endif // !defined (FOD__PLACES_H_)
