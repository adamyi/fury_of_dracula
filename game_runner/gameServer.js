var http = require("http");
var url = require("url");


//var address = "108.166.126.128";
var address = "0.0.0.0";
var port = 9024;

var gameRunner = require( "./gameRunner" );

var gameRules = require( "./draculaRules/gameRules" );

var games = { };

var maxGameTimechecks = 10;
var checkInterval = 60*1000; // 60 seconds

var freeIds = [
	"aboldes",
	"morsons",
	"lintast",
	"compets",
	"consity",
	"sembers",
	"blarety",
	"knetted",
	"involly",
	"tanquad",
	"weslant",
	"oblitee",
	"leaksum",
	"quadrol",
	"medange",
	"lureina",
	"werises",
	"squenta",
	"sultion",
	"pinvous"
];

function checkGames( ) {
	// console.log( "Checking games" );
	for ( var gameId in games ) {
		if ( games.hasOwnProperty( gameId ) ) {
			// console.log( "Game " + gameId + " interval: " + games[gameId].timeChecks );
			if ( games[gameId].timeChecks >= maxGameTimechecks ) {
				// game id is allowed to be recycled if needed
				endGame( gameId );
				console.log( gameId + " Expired" );
			} else {
				games[gameId].timeChecks += 1;
			}
		}
	}
}

var startGame = function( password ) {
	if ( freeIds.length > 0 ) {
		var gameId = freeIds.shift( );
      
		console.log( "Game " + gameId + " created with password " + password );
      
		var game = new gameRunner( );
      
		game.setRules( gameRules );
      
		games[gameId] = {
			password: password,
			game: game,
			timeChecks: 0,
			isOver: false
		};
      
		game.on( "playerTurn", function( playerIndex, playerInput ) {
			if ( playerIndex == 4 ) {
				game.playMove( );
			} else {
				console.log( "Waiting for player " + playerIndex + " turn on game " + gameId );
			}
		} );
      
		game.on( "playerDisqualified", function( playerIndex, move ) {
			console.log( "Player " + playerIndex + " disqualified on game " + gameId );
		} );
      
		game.on( "gameOver", function( ) {
			endGame( gameId );
		} );
      
		game.run( );
      
		return gameId;
	}
   
	return null;
};

var endGame = function( gameId ) {
	games[gameId].isOver = true;
	freeIds.push( gameId );
};


/**
 * Protocol:
 *    GET query strings for action:
 *         action=newGame:
 *              requires password=myPasswordHere
 *              creates a game with the given password
 *              can return:
 *                Ok
 *                <gameId> (this is the assigned gameId)
 *              or:
 *                NoFreeGames
 *         action=request:
 *              requires gameId=myGameId, password=myGamePassword, player=myPlayerIndex (0-3)
 *              can return:
 *                Ok
 *                <PastPlays>
 *                <Message>
 *                <Message>... etc.
 *              or:
 *                Disqualified
 *                (should never happen on this server)
 *         or:
 *              WaitTurn
 *              (if it's not your turn yet)
 *    action=play:
 *         requires:
 *              gameId=myGameId
 *              password=myGamePassword
 *              player=myPlayerIndex
 *              move=myPlayerMove
 *              message=myPlayerMessage
 *         can return:
 *              MoveRequired
 *              (no move given)
 *         or:
 *              IllegalMove
 *              (move isn't legal)
 *         or:
 *              NotYourTurn
 *              (not your turn in the game yet)
 *         or:
 *              Ok
 *              (it worked)
 *    action=who:
 *         requires:
 *              gameId=myGameId
 *              password=myGamePassword
 *         returns:
 *              Ok
 *              <playerIndex>
 *              (the player index for the current turn)
 *    action=cancel:
 *         requires:
 *              gameId=myGameId
 *              password=myGamePassword
 *         can return:
 *              GameCancelled
 *              (game id is now free to be used for a new game)
 *
 * You may get:
 *    BadPassword
 *    if the password for the given game is invalid
 *
 * When a game ends, the game Id is recycled. All subsequent actions will get:
 *    GameOver
 *
 *
 * Adding a javascript=true to the query string will return the above,
 * but wrapped in a javascript function call to allow making GUIs off-domain
 * e.g.
 *    gameResponse( "output here" );
 *
 * (this can be loaded by appending a <script> tag to the DOM)
 *
 *
 * 
 **/

var respond = function( response, output, isJavascript ) {
	if ( isJavascript ) {
		response.write( "gameResponse( " + JSON.stringify( output ) + " );" );
	} else {
		response.write( output );
	}
	response.end( );
};


var handlePlayer = function( response, input, game ) {
	var output = "";
	if ( input.action == "who" ) {
      
		output += "Ok\n";
		output += game.playerIndex + "\n";
		respond( response, output, input.javascript );
	} else if ( input.action == "request" ) {
		if ( game.isPlayersTurn( input.player ) ) {
			output += "Ok\n";
         
			var playerInput = game.getPlayerInput( );
			output += playerInput.pastPlays + "\n";
         
			for ( var i in playerInput.messages ) {
				output += playerInput.messages[i] + "\n";
			}
         
			respond( response, output, input.javascript );
		} else if ( game.isDisqualified[ input.player ] ) {
			respond( response, "Disqualified\n", input.javascript );
		} else {
			respond( response, "WaitTurn\n", input.javascript );
		}
	} else if ( input.action == "play" ) {
		if ( game.isPlayersTurn( input.player ) ) {
			if ( input.move && input.message ) {
				var move = { move: input.move, message: input.message };
				if ( game.isLegalMove( move ) ) {
					game.playMove( move );
					respond( response, "Ok\n", input.javascript );
				} else {
					respond( response, "IllegalMove\n", input.javascript );
				}
			} else {
				respond( response, "MoveRequired\n", input.javascript );
			}
		} else {
			respond( response, "NotYourTurn\n", input.javascript );
		}
	} else {
		respond( response, "InvalidAction\n", input.javascript );
	}
};

http.createServer( function ( request, response ) {
	input = url.parse( request.url, true ).query;
   
	response.writeHead(
		200,
		{
			"Content-Type": "text/plain",
			"Access-Control-Allow-Origin": "*"
		}
	);
   
	if ( input.action == "newGame" && input.password ) {
		// make a new game
		var gameId = startGame( input.password );
		if ( gameId ) {
			// a free slot
			respond( response, "Ok\n" + gameId + "\n", input.javascript );
		} else {
			// no free slot
			respond( response, "NoFreeGames\n", input.javascript );
		}
	} else if ( input.gameId ) {
      
		// check password
		if ( games[input.gameId] && games[input.gameId].password === input.password ) {
         
			// make alive
			games[input.gameId].checkInterval = 0;
         
			if ( games[input.gameId].isOver ) {
				var gameOut = games[input.gameId].game.getOutput( );
				respond( response, "GameOver\n", input.javascript );
			} else if ( input.action == "cancel" ) {
				endGame( input.gameId );
				respond( response, "GameCancelled\n", input.javascript );
			} else {
				// do your thang
				handlePlayer( response, input, games[input.gameId].game );
			}
		} else {
			respond( response, "BadPassword\n", input.javascript );
		}
	} else {
		respond( response, "Dracula Server!\n", input.javascript );
	}
} ).listen( port, address );

setInterval( checkGames, checkInterval );

console.log( "Dracula Server running at " + address + ":" + port );

