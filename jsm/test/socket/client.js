while(true) {

tcp = new RTCP();
tcp.connect("127.0.0.1", 8000, 0);

b = tcp.read(1000);

if(b != undefined)
	print(b.toString());

tcp.shutdown();
tcp.close();

}
