i = 0;

while(true) {
tcp = new RTCP();
tcp.connect("127.0.0.1", 8000, 3);

b = tcp.recv(1000, 3);

if(b != undefined)
	print("" + (i++) + ": " + b.toString());

tcp.shutdown();
tcp.close();
RThread.usleep(200000);

}
