if(_args.length == 0) {
	print("Usage: jsvm file.js {filename}\n");
}
else {
	f = new RFile();

	f.open(_args[0], "r");

	b = f.read(f.size());
	print("file: " + _args[0] + "; size: " + b.size() + "\n------\n" + b.toString() + "\n-------\n");

	f.close();
}
