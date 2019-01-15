var overloadFunction = function( functionObject ) {
	// partially applies the specified arguments to a function, returning a new function
	functionObject.prototype.partialApply = function( ) {
		var func = this;
		var slice = Array.prototype.slice;
		var appliedArgs = slice.call( arguments, 0 );
      
		return function( ) {
			var leftoverArgs = slice.call( arguments, 0 );
			return func.apply( this, appliedArgs.concat( leftoverArgs ) );
		};
	};
   
	// flips the first two arguments of a function
	functionObject.prototype.flip = function( ) {
		var func = this;
		return function( ) {
			var first = arguments[0];
			var second = arguments[1];
			var rest = Array.prototype.slice.call( arguments, 2 );
			var newArgs = [second, first].concat( rest );
         
			return func.apply( this, newArgs );
		};
	};
};

/*
e.g.

var foo = function( a, b, c, d ) { console.log( a, b, c, d ); }
var iAmA = foo.partialApply( "I", "am", "a" );
iAmA( "Donkey" );
-> I am a Donkey

var bah = foo.flip( );
bah( 1, 2, 3, 4 );
-> 2 1 3 4
*/

if ( module ) {
	module.exports = overloadFunction;
}