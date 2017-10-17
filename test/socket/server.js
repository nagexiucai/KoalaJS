function run(port) {
	tcp = new RTCP();

	if(!tcp.bind("", port)) {
		print("binded failed.\n");
		return;
	}

	if(!tcp.listen(1000)) {
		print("listening failed!\n");
		return false;
	}

	while(true) {
		c = tcp.accept();
		if(c == undefined) {
			print("accept failed!\n");
			break;
		}
		else {
			print("accepted.\n");
			RThread.run("task.js", c);
		}
	}

	tcp.close();
}

run(8000);
