function run(port) {
	tcp = new RTCP();

	if(!tcp.bind("", port)) {
		print("binded failed.");
		return;
	}

	if(!tcp.listen(1000)) {
		print("listening failed.");
		return false;
	}

	while(true) {
		c = tcp.accept();
		if(c == undefined) {
			print("accept failed.");
		}
		else {
			RThread.run("task.js", c);
		}
	}

	tcp.close();
}

run(8000);
