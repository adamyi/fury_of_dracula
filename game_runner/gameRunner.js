var assert = require( "./jsFun/assert" );
var events = require( "events" );

var GameRunner = function( ) {
	events.EventEmitter.call( this );
};

GameRunner.super_ = events.EventEmitter;
GameRunner.prototype = Object.create( events.EventEmitter.prototype, {
	constructor: {
		value: GameRunner,
		enumerable: false
	}
} );

GameRunner.prototype.setRules = function( rules ) {
	assert( rules.state );
	assert( rules.numPlayers );
	assert( rules.getPlayerInput );
	assert( rules.getGameOutput );
	assert( rules.playMove );
	assert( rules.endTurn );
	assert( rules.endRound );
	assert( rules.isGameOver );
   
	this.rules = rules;
	this.state = new rules.state( );
   
	this.round = 0;
	this.turn = 0;
	this.playerIndex = 0;
   
	this.isDisqualified = { };
   
	this.initialised = true;
};

GameRunner.prototype.isLegalMove = function( move ) {
	return this.rules.isLegalMove( this.state, move );
};

GameRunner.prototype.playMove = function( move ) {
	if ( !move ) {
		// play a default move if one isn't provided
		this.rules.playDefaultMove( this.state );
	} else if ( !this.rules.isLegalMove( this.state, move ) ) {
		// move is illegal, disqualify player
		this.rules.dracMoves(this.state);
		this.isDisqualified[this.playerIndex] = true;
		this.emit( "playerDisqualified", this.playerIndex, move );
		this.rules.playDefaultMove( this.state );
	} else {
		this.rules.playMove( this.state, move );
		this.emit( "turnFinished", move, this.playerIndex );
	}
   
	this.turn++;
	this.playerIndex = (this.playerIndex+ 1) % this.rules.numPlayers;
	this.round  = Math.floor( this.turn / this.rules.numPlayers );
   
	this.rules.endTurn( this.state );
	if ( this.playerIndex == 0 ) {
		this.rules.endRound( this.state );
	}
   
	this.getNextMove( );
};

GameRunner.prototype.isPlayersTurn = function( player ) {
	return (player == this.playerIndex);
};

GameRunner.prototype.getPlayerInput = function( ) {
	return this.rules.getPlayerInput( this.state );
};

GameRunner.prototype.getNextMove = function( ) {
	console.log("--------------------------------------------------------------------------");
	if ( this.rules.isGameOver( this.state ) ) {
		this.emit( "gameOver" );
	} else if ( this.isDisqualified[this.playerIndex] ) {
		console.log("Player is currently disqualified. Making a move on their behalf." + this.playerIndex);
		this.playMove( );
	} else {
		assert( this.playerIndex !== undefined, "playerIndex is undefined. in getNextMove( )" );
		this.emit( "playerTurn", this.playerIndex, this.rules.getPlayerInput( this.state ) );
	}
};

GameRunner.prototype.getOutput = function( ) {
	return this.rules.getGameOutput( this.state );
};

GameRunner.prototype.run = function( ) {
	assert( this.initialised, "Game rules not set. in run( )" );
	this.getNextMove( );
};

module.exports = GameRunner;
