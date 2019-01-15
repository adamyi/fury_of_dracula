function AssertException( message ) {
	if ( message ) {
		this.message = message;
	} else {
		this.message = "No other info";
	}
}

AssertException.prototype.toString = function () {
	return "Assert Failed in " + arguments.callee.caller.name + ": " + this.message;
};

function assert( condition, messageOrError ) {
	if ( !condition ) {
		if ( typeof messageOrError == "object" ) {
			throw messageOrError;
		} else {
			throw new AssertException( messageOrError );
		}
	}
}

if ( module ) {
	module.exports = assert;
}