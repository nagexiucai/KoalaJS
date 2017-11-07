function run(port, task) {
	var tcp = new RTCP();

	if(!tcp.bind("", port)) {
		print("binded failed.\n");
		return;
	}

	if(!tcp.listen(1000)) {
		print("listening failed!\n");
		return false;
	}

	while(true) {
		var c = tcp.accept();
		if(c == undefined) {
			break;
		}
		else {
			RThread.run(task, c);
		}
	}

	tcp.close();
}

function start() {
	if(_args.length != 2) {
		println("Usage: koala server.js {port} {client task file}.");
		println("example: koala server.js 8000 echo.js");
		return;
	}

	var port = _args[0].toInt();
	var task = _args[1];

	run(port, task);
}

start();

