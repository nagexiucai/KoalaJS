if(_args.length == 0) {
	print("Usage: jsvm dnsclient.js {domain}\n");
}
else {
	var c = RDNSC.resolv(_args[0], "A");
	print("" + c + "\n");
}

