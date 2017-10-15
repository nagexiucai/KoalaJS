function f() {
	b = new Bytes();
	b.from("xxx");


	c = new Bytes();
	c.from("-xxx");

	b.append(c);
	b.append(".xxx");

	c = RBase64.decode(RBase64.encode(b)).toString();
	print(c + "\n");
}

f();
