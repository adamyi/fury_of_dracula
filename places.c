////////////////////////////////////////////////////////////////////////
// COMP2521 19t0 ... the Fury of Dracula
// places.c: the places
//
// 2012-01-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-11-25	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "places.h"

/**
 * Places should appear in alphabetic order.
 * Each entry should satisfy `(places[i].id == i)`.
 * First real place must be at index `MIN_MAP_LOCATION`.
 * Last real place must be at index `MAX_MAP_LOCATION`.
 */
place PLACES[] =
{
	{"Adriatic Sea", "AS", ADRIATIC_SEA, SEA},
	{"Alicante", "AL", ALICANTE, LAND},
	{"Amsterdam", "AM", AMSTERDAM, LAND},
	{"Athens", "AT", ATHENS, LAND},
	{"Atlantic Ocean", "AO", ATLANTIC_OCEAN, SEA},
	{"Barcelona", "BA", BARCELONA, LAND},
	{"Bari", "BI", BARI, LAND},
	{"Bay of Biscay", "BB", BAY_OF_BISCAY, SEA},
	{"Belgrade", "BE", BELGRADE, LAND},
	{"Berlin", "BR", BERLIN, LAND},
	{"Black Sea", "BS", BLACK_SEA, SEA},
	{"Bordeaux", "BO", BORDEAUX, LAND},
	{"Brussels", "BU", BRUSSELS, LAND},
	{"Bucharest", "BC", BUCHAREST, LAND},
	{"Budapest", "BD", BUDAPEST, LAND},
	{"Cadiz", "CA", CADIZ, LAND},
	{"Cagliari", "CG", CAGLIARI, LAND},
	{"Castle Dracula", "CD", CASTLE_DRACULA, LAND},
	{"Clermont-Ferrand", "CF", CLERMONT_FERRAND, LAND},
	{"Cologne", "CO", COLOGNE, LAND},
	{"Constanta", "CN", CONSTANTA, LAND},
	{"Dublin", "DU", DUBLIN, LAND},
	{"Edinburgh", "ED", EDINBURGH, LAND},
	{"English Channel", "EC", ENGLISH_CHANNEL, SEA},
	{"Florence", "FL", FLORENCE, LAND},
	{"Frankfurt", "FR", FRANKFURT, LAND},
	{"Galatz", "GA", GALATZ, LAND},
	{"Galway", "GW", GALWAY, LAND},
	{"Geneva", "GE", GENEVA, LAND},
	{"Genoa", "GO", GENOA, LAND},
	{"Granada", "GR", GRANADA, LAND},
	{"Hamburg", "HA", HAMBURG, LAND},
	{"Ionian Sea", "IO", IONIAN_SEA, SEA},
	{"Irish Sea", "IR", IRISH_SEA, SEA},
	{"Klausenburg", "KL", KLAUSENBURG, LAND},
	{"Le Havre", "LE", LE_HAVRE, LAND},
	{"Leipzig", "LI", LEIPZIG, LAND},
	{"Lisbon", "LS", LISBON, LAND},
	{"Liverpool", "LV", LIVERPOOL, LAND},
	{"London", "LO", LONDON, LAND},
	{"Madrid", "MA", MADRID, LAND},
	{"Manchester", "MN", MANCHESTER, LAND},
	{"Marseilles", "MR", MARSEILLES, LAND},
	{"Mediterranean Sea", "MS", MEDITERRANEAN_SEA, SEA},
	{"Milan", "MI", MILAN, LAND},
	{"Munich", "MU", MUNICH, LAND},
	{"Nantes", "NA", NANTES, LAND},
	{"Naples", "NP", NAPLES, LAND},
	{"North Sea", "NS", NORTH_SEA, SEA},
	{"Nuremburg", "NU", NUREMBURG, LAND},
	{"Paris", "PA", PARIS, LAND},
	{"Plymouth", "PL", PLYMOUTH, LAND},
	{"Prague", "PR", PRAGUE, LAND},
	{"Rome", "RO", ROME, LAND},
	{"Salonica", "SA", SALONICA, LAND},
	{"Santander", "SN", SANTANDER, LAND},
	{"Saragossa", "SR", SARAGOSSA, LAND},
	{"Sarajevo", "SJ", SARAJEVO, LAND},
	{"Sofia", "SO", SOFIA, LAND},
	{"St Joseph and St Marys", "JM", ST_JOSEPH_AND_ST_MARYS, LAND},
	{"Strasbourg", "ST", STRASBOURG, LAND},
	{"Swansea", "SW", SWANSEA, LAND},
	{"Szeged", "SZ", SZEGED, LAND},
	{"Toulouse", "TO", TOULOUSE, LAND},
	{"Tyrrhenian Sea", "TS", TYRRHENIAN_SEA, SEA},
	{"Valona", "VA", VALONA, LAND},
	{"Varna", "VR", VARNA, LAND},
	{"Venice", "VE", VENICE, LAND},
	{"Vienna", "VI", VIENNA, LAND},
	{"Zagreb", "ZA", ZAGREB, LAND},
	{"Zurich", "ZU", ZURICH, LAND},
};

/**
 * Connections have no particular order required.
 * For convenience, they're grouped into road, rail, and boat
 * connections, and sorted alphabetically within those groups.
 */
connection CONNECTIONS[] = {
	//### ROAD Connections ###
	{ALICANTE, GRANADA, ROAD},
	{ALICANTE, MADRID, ROAD},
	{ALICANTE, SARAGOSSA, ROAD},
	{AMSTERDAM, BRUSSELS, ROAD},
	{AMSTERDAM, COLOGNE, ROAD},
	{ATHENS, VALONA, ROAD},
	{BARCELONA, SARAGOSSA, ROAD},
	{BARCELONA, TOULOUSE, ROAD},
	{BARI, NAPLES, ROAD},
	{BARI, ROME, ROAD},
	{BELGRADE, BUCHAREST, ROAD},
	{BELGRADE, KLAUSENBURG, ROAD},
	{BELGRADE, SARAJEVO, ROAD},
	{BELGRADE, SOFIA, ROAD},
	{BELGRADE, ST_JOSEPH_AND_ST_MARYS, ROAD},
	{BELGRADE, SZEGED, ROAD},
	{BERLIN, HAMBURG, ROAD},
	{BERLIN, LEIPZIG, ROAD},
	{BERLIN, PRAGUE, ROAD},
	{BORDEAUX, CLERMONT_FERRAND, ROAD},
	{BORDEAUX, NANTES, ROAD},
	{BORDEAUX, SARAGOSSA, ROAD},
	{BORDEAUX, TOULOUSE, ROAD},
	{BRUSSELS, COLOGNE, ROAD},
	{BRUSSELS, LE_HAVRE, ROAD},
	{BRUSSELS, PARIS, ROAD},
	{BRUSSELS, STRASBOURG, ROAD},
	{BUCHAREST, CONSTANTA, ROAD},
	{BUCHAREST, GALATZ, ROAD},
	{BUCHAREST, KLAUSENBURG, ROAD},
	{BUCHAREST, SOFIA, ROAD},
	{BUDAPEST, KLAUSENBURG, ROAD},
	{BUDAPEST, SZEGED, ROAD},
	{BUDAPEST, VIENNA, ROAD},
	{BUDAPEST, ZAGREB, ROAD},
	{CADIZ, GRANADA, ROAD},
	{CADIZ, LISBON, ROAD},
	{CADIZ, MADRID, ROAD},
	{CASTLE_DRACULA, GALATZ, ROAD},
	{CASTLE_DRACULA, KLAUSENBURG, ROAD},
	{CLERMONT_FERRAND, GENEVA, ROAD},
	{CLERMONT_FERRAND, MARSEILLES, ROAD},
	{CLERMONT_FERRAND, NANTES, ROAD},
	{CLERMONT_FERRAND, PARIS, ROAD},
	{CLERMONT_FERRAND, TOULOUSE, ROAD},
	{COLOGNE, FRANKFURT, ROAD},
	{COLOGNE, HAMBURG, ROAD},
	{COLOGNE, LEIPZIG, ROAD},
	{COLOGNE, STRASBOURG, ROAD},
	{CONSTANTA, GALATZ, ROAD},
	{CONSTANTA, VARNA, ROAD},
	{DUBLIN, GALWAY, ROAD},
	{EDINBURGH, MANCHESTER, ROAD},
	{FLORENCE, GENOA, ROAD},
	{FLORENCE, ROME, ROAD},
	{FLORENCE, VENICE, ROAD},
	{FRANKFURT, LEIPZIG, ROAD},
	{FRANKFURT, NUREMBURG, ROAD},
	{FRANKFURT, STRASBOURG, ROAD},
	{GALATZ, KLAUSENBURG, ROAD},
	{GENEVA, MARSEILLES, ROAD},
	{GENEVA, PARIS, ROAD},
	{GENEVA, STRASBOURG, ROAD},
	{GENEVA, ZURICH, ROAD},
	{GENOA, MARSEILLES, ROAD},
	{GENOA, MILAN, ROAD},
	{GENOA, VENICE, ROAD},
	{GRANADA, MADRID, ROAD},
	{HAMBURG, LEIPZIG, ROAD},
	{KLAUSENBURG, SZEGED, ROAD},
	{LEIPZIG, NUREMBURG, ROAD},
	{LE_HAVRE, NANTES, ROAD},
	{LE_HAVRE, PARIS, ROAD},
	{LISBON, MADRID, ROAD},
	{LISBON, SANTANDER, ROAD},
	{LIVERPOOL, MANCHESTER, ROAD},
	{LIVERPOOL, SWANSEA, ROAD},
	{LONDON, MANCHESTER, ROAD},
	{LONDON, PLYMOUTH, ROAD},
	{LONDON, SWANSEA, ROAD},
	{MADRID, SANTANDER, ROAD},
	{MADRID, SARAGOSSA, ROAD},
	{MARSEILLES, MILAN, ROAD},
	{MARSEILLES, TOULOUSE, ROAD},
	{MARSEILLES, ZURICH, ROAD},
	{MILAN, MUNICH, ROAD},
	{MILAN, VENICE, ROAD},
	{MILAN, ZURICH, ROAD},
	{MUNICH, NUREMBURG, ROAD},
	{MUNICH, STRASBOURG, ROAD},
	{MUNICH, VENICE, ROAD},
	{MUNICH, VIENNA, ROAD},
	{MUNICH, ZAGREB, ROAD},
	{MUNICH, ZURICH, ROAD},
	{NANTES, PARIS, ROAD},
	{NAPLES, ROME, ROAD},
	{NUREMBURG, PRAGUE, ROAD},
	{NUREMBURG, STRASBOURG, ROAD},
	{PARIS, STRASBOURG, ROAD},
	{PRAGUE, VIENNA, ROAD},
	{SALONICA, SOFIA, ROAD},
	{SALONICA, VALONA, ROAD},
	{SANTANDER, SARAGOSSA, ROAD},
	{SARAGOSSA, TOULOUSE, ROAD},
	{SARAJEVO, SOFIA, ROAD},
	{SARAJEVO, ST_JOSEPH_AND_ST_MARYS, ROAD},
	{SARAJEVO, VALONA, ROAD},
	{SARAJEVO, ZAGREB, ROAD},
	{SOFIA, VALONA, ROAD},
	{SOFIA, VARNA, ROAD},
	{STRASBOURG, ZURICH, ROAD},
	{ST_JOSEPH_AND_ST_MARYS, SZEGED, ROAD},
	{ST_JOSEPH_AND_ST_MARYS, ZAGREB, ROAD},
	{SZEGED, ZAGREB, ROAD},
	{VIENNA, ZAGREB, ROAD},

	//### RAIL Connections ###
	{ALICANTE, BARCELONA, RAIL},
	{ALICANTE, MADRID, RAIL},
	{BARCELONA, SARAGOSSA, RAIL},
	{BARI, NAPLES, RAIL},
	{BELGRADE, SOFIA, RAIL},
	{BELGRADE, SZEGED, RAIL},
	{BERLIN, HAMBURG, RAIL},
	{BERLIN, LEIPZIG, RAIL},
	{BERLIN, PRAGUE, RAIL},
	{BORDEAUX, PARIS, RAIL},
	{BORDEAUX, SARAGOSSA, RAIL},
	{BRUSSELS, COLOGNE, RAIL},
	{BRUSSELS, PARIS, RAIL},
	{BUCHAREST, CONSTANTA, RAIL},
	{BUCHAREST, GALATZ, RAIL},
	{BUCHAREST, SZEGED, RAIL},
	{BUDAPEST, SZEGED, RAIL},
	{BUDAPEST, VIENNA, RAIL},
	{COLOGNE, FRANKFURT, RAIL},
	{EDINBURGH, MANCHESTER, RAIL},
	{FLORENCE, MILAN, RAIL},
	{FLORENCE, ROME, RAIL},
	{FRANKFURT, LEIPZIG, RAIL},
	{FRANKFURT, STRASBOURG, RAIL},
	{GENEVA, MILAN, RAIL},
	{GENOA, MILAN, RAIL},
	{LEIPZIG, NUREMBURG, RAIL},
	{LE_HAVRE, PARIS, RAIL},
	{LISBON, MADRID, RAIL},
	{LIVERPOOL, MANCHESTER, RAIL},
	{LONDON, MANCHESTER, RAIL},
	{LONDON, SWANSEA, RAIL},
	{MADRID, SANTANDER, RAIL},
	{MADRID, SARAGOSSA, RAIL},
	{MARSEILLES, PARIS, RAIL},
	{MILAN, ZURICH, RAIL},
	{MUNICH, NUREMBURG, RAIL},
	{NAPLES, ROME, RAIL},
	{PRAGUE, VIENNA, RAIL},
	{SALONICA, SOFIA, RAIL},
	{SOFIA, VARNA, RAIL},
	{STRASBOURG, ZURICH, RAIL},
	{VENICE, VIENNA, RAIL},

	//### BOAT Connections ###
	{ADRIATIC_SEA, BARI, BOAT},
	{ADRIATIC_SEA, IONIAN_SEA, BOAT},
	{ADRIATIC_SEA, VENICE, BOAT},
	{ALICANTE, MEDITERRANEAN_SEA, BOAT},
	{AMSTERDAM, NORTH_SEA, BOAT},
	{ATHENS, IONIAN_SEA, BOAT},
	{ATLANTIC_OCEAN, BAY_OF_BISCAY, BOAT},
	{ATLANTIC_OCEAN, CADIZ, BOAT},
	{ATLANTIC_OCEAN, ENGLISH_CHANNEL, BOAT},
	{ATLANTIC_OCEAN, GALWAY, BOAT},
	{ATLANTIC_OCEAN, IRISH_SEA, BOAT},
	{ATLANTIC_OCEAN, LISBON, BOAT},
	{ATLANTIC_OCEAN, MEDITERRANEAN_SEA, BOAT},
	{ATLANTIC_OCEAN, NORTH_SEA, BOAT},
	{BARCELONA, MEDITERRANEAN_SEA, BOAT},
	{BAY_OF_BISCAY, BORDEAUX, BOAT},
	{BAY_OF_BISCAY, NANTES, BOAT},
	{BAY_OF_BISCAY, SANTANDER, BOAT},
	{BLACK_SEA, CONSTANTA, BOAT},
	{BLACK_SEA, IONIAN_SEA, BOAT},
	{BLACK_SEA, VARNA, BOAT},
	{CAGLIARI, MEDITERRANEAN_SEA, BOAT},
	{CAGLIARI, TYRRHENIAN_SEA, BOAT},
	{DUBLIN, IRISH_SEA, BOAT},
	{EDINBURGH, NORTH_SEA, BOAT},
	{ENGLISH_CHANNEL, LE_HAVRE, BOAT},
	{ENGLISH_CHANNEL, LONDON, BOAT},
	{ENGLISH_CHANNEL, NORTH_SEA, BOAT},
	{ENGLISH_CHANNEL, PLYMOUTH, BOAT},
	{GENOA, TYRRHENIAN_SEA, BOAT},
	{HAMBURG, NORTH_SEA, BOAT},
	{IONIAN_SEA, SALONICA, BOAT},
	{IONIAN_SEA, TYRRHENIAN_SEA, BOAT},
	{IONIAN_SEA, VALONA, BOAT},
	{IRISH_SEA, LIVERPOOL, BOAT},
	{IRISH_SEA, SWANSEA, BOAT},
	{MARSEILLES, MEDITERRANEAN_SEA, BOAT},
	{MEDITERRANEAN_SEA, TYRRHENIAN_SEA, BOAT},
	{NAPLES, TYRRHENIAN_SEA, BOAT},
	{ROME, TYRRHENIAN_SEA, BOAT},

	// Sentinel
	{-1, -1, ANY}
};

////////////////////////////////////////////////////////////////////////

location_t location_find_by_name (char *name)
{
	// binary search
	location_t lo = MIN_MAP_LOCATION, hi = MAX_MAP_LOCATION;
	while (lo <= hi) {
		location_t mid = (hi + lo) / 2;
		int ord = strcmp (name, PLACES[mid].name);
		if (ord == 0) return PLACES[mid].id;
		if (ord <  0) hi = mid - 1;
		if (ord  > 0) lo = mid + 1;
	}

	return NOWHERE;
}

location_t location_find_by_abbrev (char *abbrev)
{
	// an attempt to optimise a linear search
	struct place *first = &PLACES[MIN_MAP_LOCATION];
	struct place *last = &PLACES[MAX_MAP_LOCATION];

	for (struct place *p = first; p <= last; p++)
		if (
			p->abbrev[0] == abbrev[0] &&
			p->abbrev[1] == abbrev[1] &&
			p->abbrev[2] == '\0'
		)
			return p->id;

	return NOWHERE;
}

//////////////////////////////////////////////////////////////////////////////////

place_type_t location_get_type (location_t p)
{
	assert (valid_location_p (p));
	return PLACES[p].type;
}

// Clang warns us about the `switch' statements, some of which don't
// handle some cases, or which overspecify cases.  We intentionally
// suppress those warnings in this section.
#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wswitch"
# pragma clang diagnostic ignored "-Wswitch-enum"
# pragma clang diagnostic ignored "-Wcovered-switch-default"
#endif

const char *location_get_name (location_t p)
{
	if (valid_location_p (p))
		return PLACES[p].name;

	switch (p) {
	case CITY_UNKNOWN:	return "Unknown City";
	case SEA_UNKNOWN:	return "Unknown Sea";
	case HIDE:			return "Hide";
	case DOUBLE_BACK_1:	return "Double Back 1";
	case DOUBLE_BACK_2:	return "Double Back 2";
	case DOUBLE_BACK_3:	return "Double Back 3";
	case DOUBLE_BACK_4:	return "Double Back 4";
	case DOUBLE_BACK_5:	return "Double Back 5";
	case TELEPORT:		return "Teleport";
	case UNKNOWN_LOCATION: return "Unknown Location";
	}

	__builtin_unreachable ();
}

const char *location_get_abbrev (location_t p)
{
	if (valid_location_p (p))
		return PLACES[p].abbrev;

	switch (p) {
	case CITY_UNKNOWN:	return "C?";
	case SEA_UNKNOWN:	return "S?";
	case HIDE:			return "HI";
	case DOUBLE_BACK_1:	return "D1";
	case DOUBLE_BACK_2:	return "D2";
	case DOUBLE_BACK_3:	return "D3";
	case DOUBLE_BACK_4:	return "D4";
	case DOUBLE_BACK_5:	return "D5";
	case TELEPORT:		return "TP";
	case UNKNOWN_LOCATION: return "??";
	}

	__builtin_unreachable ();
}

const char *transport_to_s (transport_t t)
{
	switch (t) {
	case ROAD: return "road";
	case RAIL: return "rail";
	case BOAT: return "boat";
	case NONE:
	case ANY:
	default:   return "????";
	}
}

#ifdef __clang__
# pragma clang diagnostic pop
#endif
