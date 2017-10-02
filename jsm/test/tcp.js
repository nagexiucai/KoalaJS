tcp = new RTCP();

tcp.connect("t.cn", 80, 0);

s = new Bytes();
s.from("GET / HTTP/1.0\r\n\r\n");

//tcp.send(s, s.size(), 10);
tcp.write(s, s.size());

//b = tcp.recv(1000, 10);
b = tcp.read(1000);

if(b != undefined)
	print(b.toString());

tcp.shutdown();
tcp.close();
