var http = require("http");

var args = process.argv.slice( 2 );

var startClient = function(host, port) {
	var input = "";

	process.stdin.on("data", function(chunk) {
		input += chunk.toString();
		//process.stdout.write(chunk);
	});

	process.stdin.on("end", function() {
		var parts = input.trim().split(/ /g);

		if (parts.length < 2) {
			console.log("Unknown action");
		} else { 
			var client = http.request({
				host: host,
				path: "/?" + parts.join("&"),
				port: port
			}, function(response) {
				response.on("data", function(c) {
					process.stdout.write(c);
				});

				response.on("end", function() {
					console.log("\n");
				});
			});

			client.end();
		}
	});
};

if (args.length < 2) {
	console.log("Requires hostname and port as arguments");
} else {
	startClient(args[0], parseInt(args[1], 10));
}
