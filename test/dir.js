if(_args.length == 0) {
	print("Usage: jsvm dir.js {dirname}\n");
}
else {
	var dir = new RDir();

	dir.open(_args[0]);
	var count = 0;
	while(true) {
		var s = dir.read();

		if(s.length == 0)
			break;

		print("[" + s + "]\n");
		count++;
	}

	dir.close();
	print("--------------------\nTotal: " + count + "\n");
}
