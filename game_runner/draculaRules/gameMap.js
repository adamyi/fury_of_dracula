var functionDecorate = require("../jsFun/functionMagic");
functionDecorate(Function);

var list = require("../jsFun/listLike");
var assert = require("../jsFun/assert");

var constants = require("./gameConstants");

var locationDetails = [
	// Special
	{
		code: "JM",
		name: "St Joseph & St Mary",
		isHospital: true,
		isCity: true,
		isPort: false
	},
	{
		code: "CD",
		name: "Castle Dracula",
		isCastleDracula: true,
		isCity: true,
		isPort: false
	},


	// Cities
	{
		code: "AL",
		name: "Alicante",
		size: "small",
		isCity: true,
		isPort: true
	},
	{
		code: "AM",
		name: "Alicante",
		size: "large",
		isCity: true,
		isPort: true
	},
	{
		code: "AT",
		name: "Athens",
		size: "large",
		isCity: true,
		isPort: true
	},
	{
		code: "BA",
		name: "Barcelona",
		size: "large",
		isCity: true,
		isPort: true
	},
	{
		code: "BI",
		name: "Bari",
		size: "small",
		isCity: true,
		isPort: true
	},
	{
		code: "BE",
		name: "Belgrade",
		size: "small",
		isCity: true,
		isPort: false
	},
	{
		code:"BR",
		name:"Berlin",
		size:"large",
		isCity: true,
		isPort:false
	},
	{
		code:"BO",
		name:"Bordeaux",
		size:"large",
		isCity: true,
		isPort:true
	},
	{
		code:"BU",
		name:"Brussels",
		size:"small",
		isCity: true,
		isPort:false
	},
	{
		code:"BC",
		name:"Bucharest",
		size:"large",
		isCity: true,
		isPort:false
	},
	{
		code:"BD",
		name:"Budapest",
		size:"large",
		isCity: true,
		isPort:false
	},
	{
		code:"CA",
		name:"Cadiz",
		size:"large",
		isCity: true,
		isPort: true
	},
	{
		code:"CG",
		name:"Cagliari",
		size:"small",
		isCity: true,
		isPort: true
	},
	{
		code:"CF",
		name:"Clermont Ferrand",
		size:"small",
		isCity: true,
		isPort:false
	},
	{
		code:"CO",
		name:"Cologne",
		size:"large",
		isCity: true,
		isPort:false
	},
	{
		code:"CN",
		name:"Constanta",
		size:"large",
		isCity: true,
		isPort:true
	},
	{
		code:"DU",
		name:"Dublin",
		size:"small",
		isCity: true,
		isPort:true
	},
	{
		code:"ED",
		name:"Edinburgh",
		size:"large",
		isCity: true,
		isPort:true
	},
	{
		code:"FL",
		name:"Florence",
		size:"small",
		isCity: true,
		isPort:false
	},
	{
		code:"FR",
		name:"Frankfurt",
		size:"small",
		isCity: true,
		isPort:false
	},
	{
		code:"GA",
		name:"Galatz",
		size:"small",
		isCity: true,
		isPort:false
	},
	{
		code:"GW",
		name:"Galway",
		size:"small",
		isCity: true,
		isPort:true
	},
	{
		code:"GE",
		name:"Geneva",
		size:"small",
		isCity: true,
		isPort:false
	},
	{
		code:"GO",
		size:"large",
		isPort:true,
		isCity: true,
		name:"Genoa"
	},
	{
		code:"GR",
		size:"small",
		isPort:false,
		isCity: true,
		name:"Granada"
	},
	{
		code:"HA",
		size:"large",
		isPort:true,
		isCity: true,
		name:"Hamburg"
	},
	{
		code:"KL",
		size:"small",
		isCity: true,
		isPort:false,
		name:"Klausenburg"
	},
	{
		code:"LE",
		size:"small",
		isCity: true,
		isPort:true,
		name:"Le Havre"
	},
	{
		code:"LI",
		size:"large",
		isCity: true,
		isPort:false,
		name:"Leipzig"
	},
	{
		code:"LS",
		size:"large",
		isCity: true,
		isPort:true,
		name:"Lisbon"
	},
	{
		code:"LV",
		size:"large",
		isCity: true,
		isPort:true,
		name:"Liverpool"
	},
	{
		code:"LO",
		size:"large",
		isCity: true,
		isPort:true,
		name:"London"
	},
	{
		code:"MA",
		size:"large",
		isCity: true,
		isPort:false,
		name:"Madrid"
	},
	{
		code:"MN",
		size:"large",
		isCity: true,
		isPort:false,
		name:"Manchester"
	},
	{
		code:"MR",
		size:"large",
		isCity: true,
		isPort:true,
		name:"Marseilles"
	},
	{
		code:"MI",
		size:"large",
		isCity: true,
		isPort:false,
		name:"Milan"
	},
	{
		code:"MU",
		size:"large",
		isCity: true,
		isPort:false,
		name:"Munich"
	},
	{
		code:"NA",
		size:"large",
		isCity: true,
		isPort:true,
		name:"Nantes"
	},
	{
		code:"NP",
		size:"large",
		isCity: true,
		isPort:true,
		name:"Naples"
	},
	{
		code:"NU",
		size:"small",
		isCity: true,
		isPort:false,
		name:"Nuremburg"
	},
	{
		code:"PA",
		size:"large",
		isCity: true,
		isPort:false,
		name:"Paris"
	},
	{
		code:"PL",
		size:"small",
		isCity: true,
		isPort:true,
		name:"Plymouth"
	},
	{
		code:"PR",
		size:"large",
		isCity: true,
		isPort:false,
		name:"Prague"
	},
	{
		code:"RO",
		size:"large",
		isCity: true,
		isPort:true,
		name:"Rome"
	},
	{
		code:"SA",
		size:"small",
		isCity: true,
		isPort:true,
		name:"Salonica"
	},
	{
		code:"SN",
		size:"small",
		isCity: true,
		isPort:true,
		name:"Santander"
	},
	{
		code:"SR",
		size:"small",
		isCity: true,
		isPort:false,
		name:"Saragossa"
	},
	{
		code:"SJ",
		size:"small",
		isCity: true,
		isPort:false,
		name:"Sarajevo"
	},
	{
		code:"SO",
		size:"small",
		isCity: true,
		isPort:false,
		name:"Sofia"
	},
	{
		code:"ST",
		size:"small",
		isCity: true,
		isPort:false,
		name:"Strasbourg"
	},
	{
		code:"SW",
		size:"small",
		isCity: true,
		isPort:true,
		name:"Swansea"
	},
	{
		code:"SZ",
		size:"small",
		isCity: true,
		isPort:false,
		name:"Szeged"
	},
	{
		code:"TO",
		size:"small",
		isCity: true,
		isPort:false,
		name:"Toulouse"
	},
	{
		code:"VA",
		size:"small",
		isCity: true,
		isPort:true,
		name:"Valona"
	},
	{
		code:"VR",
		size:"large",
		isCity: true,
		isPort:true,
		name:"Varna"
	},
	{
		code:"VE",
		size:"small",
		isCity: true,
		isPort:true,
		name:"Venice"
	},
	{
		code:"VI",
		size:"large",
		isCity: true,
		isPort:false,
		name:"Vienna"
	},
	{
		code:"ZA",
		size:"small",
		isCity: true,
		isPort:false,
		name:"Zagreb"
	},
	{
		code:"ZU",
		size:"small",
		isCity: true,
		isPort:false,
		name:"Zurich"
	},

	// Seas
	{
		code: "NS",
		name: "North Sea",
		isSea: true
	},
	{
		code: "EC",
		name: "English Channel",
		isSea: true
	},
	{
		code: "IR",
		name: "Irish Sea",
		isSea: true
	},
	{
		code: "AO",
		name: "Atlantic Ocean",
		isSea: true
	},
	{
		code: "BB",
		name: "Bay of Biscay",
		isSea: true
	},
	{
		code: "MS",
		name: "Mediterranean Sea",
		isSea: true
	},
	{
		code: "TS",
		name: "Tyrrhenian Sea",
		isSea: true
	},
	{
		code: "IO",
		name: "Ionian Sea",
		isSea: true
	},
	{
		code: "AS",
		name: "Adriatic Sea",
		isSea: true
	},
	{
		code: "BS",
		name: "Black Sea",
		isSea: true
	}
];

var routes = [
	{ from:"AL", to:"GR", type:"road" },
	{ from:"AL", to:"MA", type:"road" },
	{ from:"AL", to:"SR", type:"road" },
	{ from:"AM", to:"BU", type:"road" },
	{ from:"AM", to:"CO", type:"road" },
	{ from:"AT", to:"VA", type:"road" },

	{ from:"BA", to:"SR", type:"road" },
	{ from:"BA", to:"TO", type:"road" },
	{ from:"BI", to:"NP", type:"road" },
	{ from:"BI", to:"RO", type:"road" },
	{ from:"BE", to:"BC", type:"road" },
	{ from:"BE", to:"JM", type:"road" },
	{ from:"BE", to:"KL", type:"road" },
	{ from:"BE", to:"SJ", type:"road" },
	{ from:"BE", to:"SO", type:"road" },
	{ from:"BE", to:"SZ", type:"road" },
	{ from:"BR", to:"HA", type:"road" },
	{ from:"BR", to:"LI", type:"road" },
	{ from:"BR", to:"PR", type:"road" },

	{ from:"BO", to:"CF", type:"road" },
	{ from:"BO", to:"NA", type:"road" },
	{ from:"BO", to:"SR", type:"road" },
	{ from:"BO", to:"TO", type:"road" },
	{ from:"BU", to:"CO", type:"road" },
	{ from:"BU", to:"LE", type:"road" },
	{ from:"BU", to:"PA", type:"road" },
	{ from:"BU", to:"ST", type:"road" },
	{ from:"BC", to:"CN", type:"road" },
	{ from:"BC", to:"GA", type:"road" },
	{ from:"BC", to:"KL", type:"road" },
	{ from:"BC", to:"SO", type:"road" },
	{ from:"BD", to:"KL", type:"road" },
	{ from:"BD", to:"SZ", type:"road" },
	{ from:"BD", to:"VI", type:"road" },
	{ from:"BD", to:"ZA", type:"road" },

	{ from:"CA", to:"GR", type:"road" },
	{ from:"CA", to:"LS", type:"road" },
	{ from:"CA", to:"MA", type:"road" },
	{ from:"CD", to:"GA", type:"road" },
	{ from:"CD", to:"KL", type:"road" },
	{ from:"CF", to:"GE", type:"road" },
	{ from:"CF", to:"MR", type:"road" },
	{ from:"CF", to:"NA", type:"road" },
	{ from:"CF", to:"PA", type:"road" },
	{ from:"CF", to:"TO", type:"road" },
	{ from:"CO", to:"FR", type:"road" },
	{ from:"CO", to:"HA", type:"road" },
	{ from:"CO", to:"LI", type:"road" },
	{ from:"CO", to:"ST", type:"road" },
	{ from:"CN", to:"GA", type:"road" },
	{ from:"CN", to:"VR", type:"road" },

	{ from:"DU", to:"GW", type:"road" },

	{ from:"ED", to:"MN", type:"road" },

	{ from:"FL", to:"GO", type:"road" },
	{ from:"FL", to:"RO", type:"road" },
	{ from:"FL", to:"VE", type:"road" },
	{ from:"FR", to:"LI", type:"road" },
	{ from:"FR", to:"NU", type:"road" },
	{ from:"FR", to:"ST", type:"road" },

	{ from:"GA", to:"KL", type:"road" },
	{ from:"GE", to:"MR", type:"road" },
	{ from:"GE", to:"PA", type:"road" },
	{ from:"GE", to:"ST", type:"road" },
	{ from:"GE", to:"ZU", type:"road" },
	{ from:"GO", to:"MR", type:"road" },
	{ from:"GO", to:"MI", type:"road" },
	{ from:"GO", to:"VE", type:"road" },
	{ from:"GR", to:"MA", type:"road" },

	{ from:"HA", to:"LI", type:"road" },

	{ from:"JM", to:"SJ", type:"road" },
	{ from:"JM", to:"SZ", type:"road" },
	{ from:"JM", to:"ZA", type:"road" },

	{ from:"KL", to:"SZ", type:"road" },

	{ from:"LE", to:"NA", type:"road" },
	{ from:"LE", to:"PA", type:"road" },
	{ from:"LI", to:"NU", type:"road" },
	{ from:"LS", to:"MA", type:"road" },
	{ from:"LS", to:"SN", type:"road" },
	{ from:"LV", to:"MN", type:"road" },
	{ from:"LV", to:"SW", type:"road" },
	{ from:"LO", to:"MN", type:"road" },
	{ from:"LO", to:"PL", type:"road" },
	{ from:"LO", to:"SW", type:"road" },

	{ from:"MA", to:"SN", type:"road" },
	{ from:"MA", to:"SR", type:"road" },
	{ from:"MR", to:"MI", type:"road" },
	{ from:"MR", to:"TO", type:"road" },
	{ from:"MR", to:"ZU", type:"road" },
	{ from:"MI", to:"MU", type:"road" },
	{ from:"MI", to:"VE", type:"road" },
	{ from:"MI", to:"ZU", type:"road" },
	{ from:"MU", to:"NU", type:"road" },
	{ from:"MU", to:"ST", type:"road" },
	{ from:"MU", to:"VE", type:"road" },
	{ from:"MU", to:"VI", type:"road" },
	{ from:"MU", to:"ZA", type:"road" },
	{ from:"MU", to:"ZU", type:"road" },

	{ from:"NA", to:"PA", type:"road" },
	{ from:"NP", to:"RO", type:"road" },
	{ from:"NU", to:"PR", type:"road" },
	{ from:"NU", to:"ST", type:"road" },

	{ from:"PA", to:"ST", type:"road" },
	{ from:"PR", to:"VI", type:"road" },

	{ from:"SA", to:"SO", type:"road" },
	{ from:"SA", to:"VA", type:"road" },
	{ from:"SN", to:"SR", type:"road" },
	{ from:"SR", to:"TO", type:"road" },
	{ from:"SJ", to:"SO", type:"road" },
	{ from:"SJ", to:"VA", type:"road" },
	{ from:"SJ", to:"ZA", type:"road" },
	{ from:"SO", to:"VR", type:"road" },
	{ from:"SO", to:"VA", type:"road" },
	{ from:"ST", to:"ZU", type:"road" },
	{ from:"SZ", to:"ZA", type:"road" },

	{ from:"VI", to:"ZA", type:"road" },

	{ from:"AL", to:"BA", type:"rail" },
	{ from:"AL", to:"MA", type:"rail" },
	{ from:"BA", to:"SR", type:"rail" },
	{ from:"BI", to:"NP", type:"rail" },
	{ from:"BE", to:"SO", type:"rail" },
	{ from:"BE", to:"SZ", type:"rail" },
	{ from:"BR", to:"HA", type:"rail" },
	{ from:"BR", to:"LI", type:"rail" },
	{ from:"BR", to:"PR", type:"rail" },
	{ from:"BO", to:"PA", type:"rail" },
	{ from:"BO", to:"SR", type:"rail" },
	{ from:"BU", to:"CO", type:"rail" },
	{ from:"BU", to:"PA", type:"rail" },
	{ from:"BC", to:"CN", type:"rail" },
	{ from:"BC", to:"GA", type:"rail" },
	{ from:"BC", to:"SZ", type:"rail" },
	{ from:"BD", to:"VI", type:"rail" },
	{ from:"BD", to:"SZ", type:"rail" },
	{ from:"CO", to:"FR", type:"rail" },
	{ from:"ED", to:"MN", type:"rail" },
	{ from:"FL", to:"MI", type:"rail" },
	{ from:"FL", to:"RO", type:"rail" },
	{ from:"FR", to:"LI", type:"rail" },
	{ from:"FR", to:"ST", type:"rail" },
	{ from:"GE", to:"MI", type:"rail" },
	{ from:"GO", to:"MI", type:"rail" },
	{ from:"LE", to:"PA", type:"rail" },
	{ from:"LI", to:"NU", type:"rail" },
	{ from:"LS", to:"MA", type:"rail" },
	{ from:"LV", to:"MN", type:"rail" },
	{ from:"LO", to:"MN", type:"rail" },
	{ from:"LO", to:"SW", type:"rail" },
	{ from:"MA", to:"SN", type:"rail" },
	{ from:"MA", to:"SR", type:"rail" },
	{ from:"MR", to:"PA", type:"rail" },
	{ from:"MI", to:"ZU", type:"rail" },
	{ from:"MU", to:"NU", type:"rail" },
	{ from:"NP", to:"RO", type:"rail" },
	{ from:"PR", to:"VI", type:"rail" },
	{ from:"SA", to:"SO", type:"rail" },
	{ from:"SO", to:"VR", type:"rail" },
	{ from:"ST", to:"ZU", type:"rail" },
	{ from:"VE", to:"VI", type:"rail" },

	{ from:"AL", to:"MS", type:"sea" },
	{ from:"AM", to:"NS", type:"sea" },
	{ from:"AT", to:"IO", type:"sea" },
	{ from:"BA", to:"MS", type:"sea" },
	{ from:"BI", to:"AS", type:"sea" },
	{ from:"BO", to:"BB", type:"sea" },
	{ from:"CA", to:"AO", type:"sea" },
	{ from:"CG", to:"MS", type:"sea" },
	{ from:"CG", to:"TS", type:"sea" },
	{ from:"CN", to:"BS", type:"sea" },
	{ from:"DU", to:"IR", type:"sea" },
	{ from:"ED", to:"NS", type:"sea" },
	{ from:"GW", to:"AO", type:"sea" },
	{ from:"GO", to:"TS", type:"sea" },
	{ from:"HA", to:"NS", type:"sea" },
	{ from:"LE", to:"EC", type:"sea" },
	{ from:"LS", to:"AO", type:"sea" },
	{ from:"LV", to:"IR", type:"sea" },
	{ from:"LO", to:"EC", type:"sea" },
	{ from:"MR", to:"MS", type:"sea" },
	{ from:"NA", to:"BB", type:"sea" },
	{ from:"NP", to:"TS", type:"sea" },
	{ from:"PL", to:"EC", type:"sea" },
	{ from:"RO", to:"TS", type:"sea" },
	{ from:"SA", to:"IO", type:"sea" },
	{ from:"SN", to:"BB", type:"sea" },
	{ from:"SW", to:"IR", type:"sea" },
	{ from:"VA", to:"IO", type:"sea" },
	{ from:"VR", to:"BS", type:"sea" },
	{ from:"VE", to:"AS", type:"sea" },

	{ from:"NS", to:"EC", type:"sea" },
	{ from:"NS", to:"AO", type:"sea" },
	{ from:"EC", to:"AO", type:"sea" },
	{ from:"IR", to:"AO", type:"sea" },
	{ from:"AO", to:"BB", type:"sea" },
	{ from:"AO", to:"MS", type:"sea" },
	{ from:"MS", to:"TS", type:"sea" },
	{ from:"TS", to:"IO", type:"sea" },
	{ from:"IO", to:"AS", type:"sea" },
	{ from:"IO", to:"BS", type:"sea" }
];

var locationInfo = { };

// store each location detail in locationInfo as a lookup
list.each(
	locationDetails,
	function(locationDetail) {
		this[locationDetail.code] = locationDetail;
	},
	locationInfo
);


var makeConnectionsDict = function(type) {
	// make a dictionary of a list of connections
	var result = { };

	var storeRoute = function(route) {
		// bi-directional (to and from are somewhat misnomers)
		if (route.type == type) {
			if (result[route.from]) {
				result[route.from].push(route.to);
			} else {
				result[route.from] = [route.to];
			}

			if (result[route.to]) {
				result[route.to].push(route.from);
			} else {
				result[route.to]   = [route.from];
			}
		}
	};

	list.each(routes, storeRoute);

	return result;
};

var roadConnections = makeConnectionsDict("road");
var railConnections = makeConnectionsDict("rail");
var seaConnections  = makeConnectionsDict("sea");

var connections = {
	road: roadConnections,
	rail: railConnections,
	sea: seaConnections
};

var isConnected = function(from, to, type) {
	// are there any connections of this type
	return list.contains(connections[type][from], to);
};

var expandConnections = function(distance, connections, fromCode) {
	var adjacentLocations = connections[fromCode];

	if (distance > 0 && adjacentLocations) {
		// apply a lesser distance for the next hop
		var getNextConnections = expandConnections.partialApply(distance-1, connections);

		// get the next list of connections for each of the adjacent locations
		var nextConnections = list.map(adjacentLocations, getNextConnections);

		// reduce these lists to the union over them all
		return list.reduce(nextConnections.concat(adjacentLocations), list.union, []);
	} else {
		// no connections for 0 distance, or no adjacent locations
		return [];
	}
};


var extractCode = function(locationDetail) {
	return locationDetail.code;
};

var locations = list.map(locationDetails, extractCode);



module.exports = {
	locations: locations,
	routes: routes,
	isConnected: isConnected,
	expandConnections: expandConnections,
	roadConnections: roadConnections,
	railConnections: railConnections,
	seaConnections: seaConnections,
	locationInfo: locationInfo,
	hospitalCityCode: "JM",
	castleDraculaCode: "CD"
};
