////////////////////////////////////////////////////////////////////////
// COMP1927/2521 ... the Fury of Dracula!
//
// Game engine.
//
// ....-..-..    David Collien <dcollien@>
// ....-..-..    Geneveive Carter <gac@>
// 2018-01-17    Jashank Jeremy <jashankj@>
//     Cleaned up code style and commentary.

var fs = require ("fs");
var path = require ("path");
var cp = require ("child_process");
var spawn = cp.spawn;

var list = require ("./jsFun/listLike");
var assert = require ("./jsFun/assert");

var gameRunner = require ("./gameRunner");
var gameRules = require ("./draculaRules/gameRules");

var game = new gameRunner ();

function log_banner () {
	console.log ("   The FURY of DRACULA   ");
	console.log ("          _..._          ");
	console.log ("        .'     '.        ");
	console.log ("       ; __   __ ;       ");
	console.log ("       |/  \\ /  \\|       ");
	console.log ("     |\\| -- ' -- |/|     ");
	console.log ("     |(| \\o| |o/ |)|     ");
	console.log ("     _\\|    U    |/_     ");
	console.log ("  .-'  | ,.___., |  '-.  ");
	console.log ("  \\    ;  V'-'V  ;    /  ");
	console.log ("   `\\   \\       /   /`   ");
	console.log ("     `\\  '-...-'  /`     ");
	console.log ("       `\\  / \\  /`       ");
	console.log ("         `\\\\_//`         ");
	console.log ("...........................");
}

function log_vk (value, key) {
	console.log ("  " + key + ": " + value);
}

function runGame (playerPrograms) {
	log_banner();

	game.setRules (gameRules);

	game.on ("playerTurn", function (playerIndex, playerInput) {
		 console.log ("> ");
		 console.log ("> ----------------------------------------- <");
		 console.log ("> Input for player " + playerIndex + ":");

		 console.log ("> +++");
		 list.each (playerInput, function (value, key) {
		 	if (key === "messages") return; // DEBUG
		 	console.log ("  " + key + ": ");
		 	console.log (value);
		 });
		 console.log ("> ---");

		var programToRun = playerPrograms[playerIndex];
		var args = [];
		// console.log (playerPrograms[playerIndex]);
		if (programToRun.indexOf (" ") != -1) {
			args = programToRun.split (" ");
			programToRun = args.shift ();
		}

		if (!programToRun) {
			game.playMove ();
		} else {
			console.log ("> Running player " + playerIndex + "... ");
			runPlayer (programToRun, playerInput, function (playerMove, playerOutput) {
				// spit out the player's output
				// console.log ("# ---   Player Output   ---");
				// console.log (playerOutput);
				// console.log ("# --- End Player Output ---");

				// play the player's move
				game.playMove (playerMove);
			}, args);
		}
	});

	game.on ("playerDisqualified", function (playerIndex, move) {
		console.log ("> Player " + playerIndex + " disqualified for illegal move.");
		console.log ("> +++ ");
		list.each (move, log_vk);
		console.log ("> --- ");
	});

	game.on ("turnFinished", function (moveMade, playerIndex) {
		console.log ("> Player " + playerIndex + " made move:");

		console.log ("> +++");
		list.each (moveMade, log_vk);
		console.log ("> ---");

	});

	game.on ("gameOver", function () {
		console.log ("> End of game: ");
		console.log ("> +++");
		list.each (game.getOutput(), log_vk);
		console.log ("> ---");

		console.log ("> Game Over");
		console.log (game.state.score);
		console.log (game.round);
		for (var i in this.isDisqualified) {
			console.log ("Player " + i + " was disqualified? " + this.isDisqualified[i]);
		}

	});

	game.run ();
}

function runPlayer (playerProgram, programInput, callback, args) {
	//console.log (playerProgram +"   " + args[0]);

	// This is currently disabled due to the amount of log space it requires
	console.log ("Running with input: \"past_plays\":", JSON.stringify (programInput["past_plays"], null, 2));
	var killed = false;

    // var posix = require ('posix');
    // posix.setrlimit ('core', { soft: 1000000, hard: null });

	assert (playerProgram, "playerProgram undefined. in runPlayer");
	var playerProcess;
	if (args[0] == "valgrind") {
		playerProcess = spawn ("valgrind", ["--tool=memcheck", playerProgram]);
	} else {
		playerProcess = spawn ("timeout", ["10s", playerProgram], args);

	}

	// store stdout
	var stdout = "";
	playerProcess.stdout.on ("data", function (data) {
		if (data) stdout += data.toString ();
	});

	// store stderr
	var stderr = "";
	playerProcess.stderr.on ("data", function (data) {
		if (data) stderr += data.toString ();
	});

	// var playerProcess = spawn (playerProgram, args);
	// var playerProcess = spawn ("valgrind", ["--tool=memcheck", "--read-var-info=yes", playerProgram]);
	// var playerProcess = spawn ("valgrind", ["--tool=memcheck", playerProgram]);

	playerProcess.on ("exit", function (code, signal) {
		if (code !== 0) {
			if (code == 42) {
				console.log ("Program was terminated after maximum allowed time.");
			} else {
				console.log ("Program exited with code: " + code);
			}
		}

		if (signal !== null) {
			console.log ("Received signal: " + signal.toString ());

            // var exec = require ('child_process').execSync;
            // exec ('sh ~/15s2.dracula/scripts/getBacktrace.sh ' + playerProgram + " core", function (error, stdout, stderr) {
			// 	console.log ("testing things");
			// 	console.log (' ', stdout);
			// 	console.log (' ', stderr);
			// 	if (error !== null) {
			// 		console.log ('exec error: ', error);
			// 	}
            // });

		}
	});

	playerProcess.on ("close", function (code, signal) {
		var outputObject;
		// expect JSON output on stdout, normal output on stderr
		console.log ("==============================================================");
		var trimmed_stderr = stderr.substring (0, 20000);
		var trim_len = stderr.length;
		console.log ("Program Output (" + trim_len + "): " + trimmed_stderr + "");
		if (trim_len > 20000)
			console.log ("...output truncated due to length");

		// console.log ("Program Output: REMOVED DUE TO 300mb");
		console.log ("==============================================================");

		try {
			outputObject = JSON.parse (stdout);
		} catch (e) {
			console.log ("Move data parse error: \""+ stdout + "\"");
			console.log (e);
			outputObject = { };
			console.log ("This usually means your program crashed before making a move.");
		}

		console.log ("Move: ", JSON.stringify (outputObject, null, 2));

		callback (outputObject, stderr);
	});

	playerProcess.on ("error", function (err) {
		console.log ("Error: " + err.toString ());
	});

	// sent programInput object to subprocess as stdin
	playerProcess.stdin.write (JSON.stringify (programInput));
	playerProcess.stdin.end ();
}

// ignore first two arguments  ("node" and script name)
runGame (process.argv.slice (2));

