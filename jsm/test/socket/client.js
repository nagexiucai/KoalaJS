
while(true) {
tcp = new RTCP();
tcp.connect("127.0.0.1", 8000, 3);

b = new Bytes();
b.from("hello, world!");

tcp.write(b, b.size());
b = tcp.recv(1000, 3);

if(b != undefined)
	print(b.toString());

RThread.usleep(20000);
tcp.shutdown();
tcp.close();
}
