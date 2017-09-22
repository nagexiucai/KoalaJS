include('comm.js');

while(true) {
	var v = get('count');

	if(v != undefined)
		print("sub thread get : " + v);

	Thread.usleep(100000);
}
