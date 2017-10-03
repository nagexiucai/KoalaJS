i=0;
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
			print("accept ." + i);
			i++;
			RThread.run("task.js", c);
			//RThread.exec(' var cs = _threadArg; b = new Bytes(); b.from("Hello, world! "); cs.send(b, b.size(), 1); print("task end."); cs.shutdown(); cs.close();', c);
		}
	}

	tcp.close();
}

run(8000);
