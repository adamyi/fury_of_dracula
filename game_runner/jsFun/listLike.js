//"use strict";

var assert = require( "./assert" );

var listFuncs = { };

function getType( inObject ) {
	if ( inObject instanceof Array ) {
		return Array;
	} else if ( inObject instanceof String ) {
		return String;
	} else {
		return Object;
	}
}

function numKeys( inObject ) {
	var count, i;
   
	if ( inObject.length ) {
		return inObject.length;
	} else {
		count = 0;
		for ( i in inObject ) {
			if ( inObject.hasOwnProperty( i ) ) {
				count += 1;
			}
		}
		return count;
	}
}

function getValues( inObject ) {
	var i, values;
	if ( inObject.length ) {
		return inObject;
	} else {
		values = [];
		for ( i in inObject ) {
			if ( inObject.hasOwnProperty( i ) ) {
				values.push( inObject[i] );
			}
		}
		return values;
	}
}

function newObject( type ) {
	if ( type === String ) {
		return "";
	} else if ( type == Array ) {
		return [];
	} else {
		return { };
	}
}

// N.B. Impure iterator functions
listFuncs.selfMap = function( inObject, delegate, context ) {
	var i;
	assert( typeof delegate === "function", new TypeError( "Delegate not a function" ) );

	for ( i in inObject ) {
		if ( inObject.hasOwnProperty( i ) ) {
			inObject[i] = delegate.call( context, inObject[i], i, inObject );
		}
	}
   
	return inObject;
};

listFuncs.doWhile = function( delegate, context ) {
	var i = 0;
	while ( delegate.call( context, i ) ) {
		i++;
	}
};

listFuncs.rollWhile = function( howMany, delegate, context ) {
	var i;
	for ( i = 0; i < howMany; i++ ) {
		if ( !delegate.call( context, i ) ) {
			break;
		}
	}
};

listFuncs.roll = function( howMany, delegate, context ) {
	var i;
	for ( i = 0; i < howMany; i++ ) {
		delegate.call( context, i );
	}
};

// side-effects over an object
listFuncs.each = function( inObject, delegate, context ) {
	var i;
	assert( typeof delegate === "function", new TypeError( "Delegate not a function" ) );

	for ( i in inObject ) {
		if ( inObject.hasOwnProperty( i ) ) {
			delegate.call( context, inObject[i], i, inObject );
		}
	}
   
	return inObject;
};

// early exit each
listFuncs.eachWhile = function( inObject, delegate, context ) {
	var i;
	assert( typeof delegate === "function", new TypeError( "Delegate not a function" ) );
   
	for ( i in inObject ) {
		if ( inObject.hasOwnProperty( i ) ) {
			if ( !delegate.call( context, inObject[i], i, inObject ) ) {
				break;
			}
		}
	}
   
	return inObject;
};

listFuncs.make = function( type, seedValueOrGenerator, howMany, keyPrefix, context ) {
	var i, newCollection, seedType, seedValue, seedDelegate;
   
	if ( typeof seedValueOrGenerator === "function" ) {
		seedDelegate = seedValueOrGenerator;
	} else {
		seedDelegate = false;
		seedValue = seedValueOrGenerator;
	}
   
	newCollection = newObject( type );
   
	for ( i = 0; i < howMany; i++ ) {
		if ( seedDelegate ) {
			seedValue = seedDelegate.call( context, i, howMany );
		}
		if ( keyPrefix ) {
			newCollection[keyPrefix + i] = seedValue;
		} else {
			if ( type === String ) {
				newCollection += seedValue;
			} else if ( type === Array ) {
				newCollection.push( seedValue );
			} else {
				newCollection[i] = seedValue;
			}
		}
	}
   
	return newCollection;
};

listFuncs.take = function( inObject, howMany ) {
	var i, count, newObject;
   
	newObject = newObject( getType( inObject ) );
   
	count = 0;
	for ( i in inObject ) {
		if ( count < howMany ) {
			newObject[i] = inObject[i];
		} else {
			break;
		}
		count += 1;
	}
   
	return newObject;
};

listFuncs.map = function( inObject, delegate, context ) {
	var i, copy, resultItem;
	assert( typeof delegate === "function", new TypeError( "Delegate not a function" ) );
   
	copy = newObject( getType( inObject ) );
	for ( i in inObject ) {
		if ( inObject.hasOwnProperty( i ) ) {
			resultItem = delegate.call( context, inObject[i], i, inObject, copy  );
			if ( copy instanceof String ) {
				copy += resultItem;
			} else if ( copy instanceof Array ) {
				copy.push( resultItem );
			} else {
				copy[i] = resultItem;
			}
		}
	}
   
	return copy;
};

listFuncs.foldl = function( inObject, delegate, seed, context ) {
	var i, result, first;
   
	assert( typeof delegate === "function", new TypeError( "Delegate not a function" ) );
   
	result = seed;
	first = true;
   
	for ( i in inObject ) {
		if ( inObject.hasOwnProperty( i ) ) {
			if ( !seed && first ) {
				result = inObject[i];
				first = false;
			} else {
				result = delegate.call( context, result, inObject[i], i, inObject );
			}
		}
	}
   
	return result;
};

listFuncs.reduce = listFuncs.foldl;

listFuncs.foldr = function( inObject, delegate, seed, context ) {
	var i, result, first, revKeys, keyIndex, key;
	assert( typeof delegate === "function", new TypeError( "Delegate not a function" ) );
   
	result = seed;
	first = true;
   
	revKeys = [];
	for ( i in inObject ) {
		if ( inObject.hasOwnProperty( i ) ) {
			revKeys.push( i );
		}
	}
   
	revKeys.reverse( );
   
	for ( keyIndex in revKeys ) {
		if ( revKeys.hasOwnProperty( keyIndex ) ) {
			key = revKeys[keyIndex];
			if ( !seed && first ) {
				result = inObject[key];
				first = false;
			} else {
				result = delegate.call( context, inObject[key], result, key, inObject );
			}
		}
	}
   
	return result;
};

listFuncs.zipToObject = function( inObject, otherObject ) {
	var i, result;
	assert( numKeys( inObject ) === numKeys( otherObject ), "zipToObject inputs must be of same length" );
	// assumes identical keysets
   
	result = { };
   
	for ( i in inObject ) {
		if ( inObject.hasOwnProperty( i ) && otherObject.hasOwnProperty( i ) ) {
			result[inObject[i]] = otherObject[i];
		}
	}
   
	return result;
};

listFuncs.zip = function( inObject, otherObject, delegate, context ) {
	var i, result, resultItem;
	if ( !delegate || (typeof delegate !== "function") ) {
		delegate = function( a, b ) {
			return [a, b];
		};
	}
   
	assert( numKeys( inObject ) === numKeys( otherObject ), "zipWith inputs must be of same length" );
   
	result = newObject( getType( inObject ) );
   
	for ( i in inObject ) {
		resultItem = delegate.call( context, inObject[i], otherObject[i], i, inObject, otherObject );
		if ( inObject.hasOwnProperty( i ) && otherObject.hasOwnProperty( i ) ) {
			if ( result instanceof String ) {
				result += resultItem;
			} else if ( result instanceof Array ) {
				result.push( resultItem );
			} else {
				result[i] = resultItem;
			}
		}
	}
   
	return result;
};

listFuncs.contains = function( inObject, needle, comparisonDelegate, context ) {
	var i, comparison;
	if ( !comparisonDelegate || (typeof comparisonDelegate !== "function") ) {
		comparisonDelegate = function( a, b ) {
			return a === b;
		};
	}
   
	for ( i in inObject ) {
		if ( inObject.hasOwnProperty( i ) ) {
			comparison = comparisonDelegate.call( context, needle, inObject[i], i, inObject );
         
			if ( comparison ) {
				return true;   
			}
		}
	}
   
	return false;
};

// returns an array of values from inObject that satisfy the conditionDelegate
listFuncs.filter = function( inObject, conditionDelegate, context ) {
	var i, filtered, filteredIndex;
	assert( typeof conditionDelegate === "function", new TypeError( "Delegate not a function" ) );
   
	filtered = new Array( numKeys( inObject ) );
	filteredIndex = 0;
   
	for ( i in inObject ) {
		if ( inObject.hasOwnProperty( i ) ) {
			if ( conditionDelegate.call( context, inObject[i], i, inObject, filteredIndex, filtered ) ) {
				filtered[filteredIndex] = inObject[i];
				filteredIndex += 1;
			}
		}
	}
   
	return filtered.slice( 0, filteredIndex );
};


// Array of all elements in inObject are filtered by whether they are contained in listrray
listFuncs.intersection = function( inObject, array, comparisonDelegate, context ) {
	var self, intersectionFilter;
   
	intersectionFilter = function( item ) {
		return listFuncs.contains( array, item, comparisonDelegate, this );
	};
   
	return listFuncs.filter( inObject, intersectionFilter, context );
};

listFuncs.nub = function( inObject, comparisonDelegate, context ) {
	var self, nubFilter;
   
	if ( !comparisonDelegate || (typeof comparisonDelegate !== "function") ) {
		comparisonDelegate = function( a, b ) {
			return a === b;
		};
	}
   
	nubFilter = function( item, i, original, filteredIndex, filtered ) {
		return !listFuncs.contains( filtered, item, comparisonDelegate, this );
	};
   
	return listFuncs.filter( inObject, nubFilter, context );
};

// Array of values common to two objects
listFuncs.union = function( inObject, otherObject, comparisonDelegate, context ) {
	var concat = getValues( inObject ).concat( getValues( otherObject ) );
	return listFuncs.nub( concat, context );
};

listFuncs.any = function( inObject, conditionDelegate, context ) {
	var i;
	assert( typeof conditionDelegate === "function", new TypeError( "Delegate not a function" ) );
   
	for ( i in inObject ) {
		if ( inObject.hasOwnProperty( i ) ) {
			if ( conditionDelegate.call( context, inObject[i], i, inObject ) ) {
				return true;
			}
		}
	}
   
	return false;
};

listFuncs.all = function( inObject, conditionDelegate, context ) {
	var i;
   
	assert( typeof conditionDelegate === "function", new TypeError( "Delegate not a function" ) );
   
	for ( i in inObject ) {
		if ( inObject.hasOwnProperty( i ) ) {
			if ( !conditionDelegate.call( context, inObject[i], i, inObject ) ) {
				return false;
			}
		}
	}
   
	return true;
};

// keys and values match
listFuncs.isEqual = function( inObject, otherObject, comparisonDelegate, context ) {
	var i, comparison, itemsChecked;
   
	if ( !comparisonDelegate || (typeof comparisonDelegate !== "function") ) {
		comparisonDelegate = function( a, b ) {
			return a === b;
		};
	}
   
	assert( typeof inObject === typeof otherObject );
   
	if ( numKeys( otherObject ) !== numKeys( inObject ) ) {
		return false; // key mismatch
	}
   
	itemsChecked = 0;
	for ( i in inObject ) {
		if ( inObject.hasOwnProperty( i ) ) {
			comparison = comparisonDelegate.call( context, inObject[i], otherObject[i], i, inObject, otherObject );
         
			if ( !comparison ) {
				return false;  
			}
         
			itemsChecked += 1;
		}
	}
   
	return true;
};


(function test( ) {
   
	var inData, inData2, outData, list;
   
	list = listFuncs;
   
	// isEqual
	inData = {
		"a": 1,
		"b": 2,
		"c": 3,
		"d": 4,
		"e": 5
	};
   
	inData2 = {
		"a": 1,
		"c": 3,
		"d": 4,
		"e": 5,
		"f": 6
	};
   
	assert( !list.isEqual( inData, inData2 ), "isEqual" );
   
	inData["f"] = 6;
	inData2["b"] = 2;
   
	assert( list.isEqual( inData, inData2 ), "isEqual" );
   
	assert( list.isEqual( [], [] ) );
	assert( !list.isEqual( [1], [] ) );
	assert( !list.isEqual( [], [1] ) );
   
	// selfMap
	inData = [1,2,3,4,5];
	outData = list.selfMap( inData, function( x, i ) { return x + parseInt( i ); } );
	assert( list.isEqual( outData, [1,3,5,7,9] ),outData.toString( ) );
	assert( list.isEqual( inData, [1,3,5,7,9] ), inData.toString( ) );
   
	// map
	inData = [1,2,3,4,5];
	outData = list.map( inData, function( x, i ) { return x + parseInt( i ); } );
	assert( list.isEqual( outData, [1,3,5,7,9] ),outData.toString( ) );
	assert( list.isEqual( inData, [1,2,3,4,5] ), inData.toString( ) );
   
	// foldl
	inData = [1,2,3,4,5];
	outData = list.foldl( inData, function( a, b ) { return [b].concat( a ); }, [] );
	assert( list.isEqual( outData, [5,4,3,2,1] ), outData.toString( ) );
	assert( list.isEqual( inData, [1,2,3,4,5] ), inData.toString( ) );
   
	inData = [1,2,3,4,5];
	outData = list.foldl( inData, function( a, b ) { return a + b; } );
	assert( outData == 15, outData );
   
	// foldr
	inData = [1,2,3,4,5];
	outData = list.foldr( inData, function( a, b ) { return b.concat( [a] ); }, [] );
	assert( list.isEqual( outData, [5,4,3,2,1] ), outData.toString( ) );
	assert( list.isEqual( inData, [1,2,3,4,5] ), inData.toString( ) );
   
	// zip
	inData =  [1,2,3,4,5];
	inData2 = [-1,-2,-3,-4,-5];
	outData = list.zip( inData, inData2 );
   
	var test = list.isEqual( outData, [[1,-1], [2,-2], [3,-3], [4,-4], [5,-5]], function( a, b ) { return list.isEqual( a, b ); } );
	assert( test, outData.toString( ) );
   
	// zipToObject
	inData =  ["a", "b", "c", "d", "e"];
	inData2 = [-1,-2,-3,-4,-5];
	outData = list.zipToObject( inData, inData2 );
	assert( outData.a == -1, outData.a );
	assert( outData.b == -2, outData.b );
	assert( outData.c == -3, outData.c );
	assert( outData.d == -4, outData.d );
	assert( outData.e == -5, outData.e );
   
	// contains
	inData =  [1,2,3,4,5];
	assert( list.contains( inData, 4 ), inData );
   
	// filter
	inData = [1,2,3,4,5];
	outData = list.filter( inData, function( x ) { return x == 2 || x == 3; } );
	assert( list.isEqual( outData, [2,3] ), outData );
	assert( list.isEqual( inData, [1,2,3,4,5] ), inData.toString( ) );
   
	// union
	inData = [1,2,3,4,5];
	inData2 = [3,4,5,6,7];
	outData = list.union( inData, inData2 );
	assert( list.isEqual( outData, [1,2,3,4,5,6,7] ), outData );
   
	// intersection
	inData = [1,2,3,4,5];
	inData2 = [3,4,5,6,7];
	outData = list.intersection( inData, inData2 );
	assert( list.isEqual( outData, [3,4,5] ), outData );
   
	// nub
	inData = [1,1,2,2,1,2,1,1,1,1,1,2,2,1,1,2];
	outData = list.nub( inData );
	assert( list.isEqual( outData, [1,2] ) );
   
	// any
	inData = [2,3,4,5];
	assert( list.any( inData, function( x ) { return x < 5; } ) );
	assert( !list.any( inData, function( x ) { return x < 2; } ) );
   
	// all
	inData = [2,3,4,5];
	assert( list.all( inData, function( x ) { return x <= 5; } ) );
	assert( !list.all( inData, function( x ) { return x < 2; } ) );
	assert( !list.all( inData, function( x ) { return x < 5; } ) );
   
	// make
	assert( list.make( String, "A", 5 ) === "AAAAA" );
   
	inData = ["a","b","c","d","e"];
	var maker = function( i ) {
		return inData[i];
	};
	var outData = list.make( Array, maker, 5 );
	assert( list.isEqual( inData, outData ), outData );
   
})( );

if ( module ) {
	module.exports = listFuncs;
}