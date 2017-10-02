function run(port) {
	tcp = new RTCP();

	if(!tcp.bind("", port)) {
		print("binded failed.");
		return;
	}

	if(!tcp.listen(10)) {
		print("listening failed.");
		return false;
	}

	b = new Bytes();
	b.from("Hello, world!\n");

	while(true) {
		c = tcp.accept();
		if(c == undefined) {
			break;
		}
		else {
			print("accept." + c);
			c.send(b, b.size(), 0);
			c.close();
		}
	}

	tcp.close();
}

run(8000);
