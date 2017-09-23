// Child thread, dead loop to get global value which is set by main thread. 
while(true) {
	var v = Global.get('count');

	if(v != undefined)
		print("Sub thread get : " + v);

	Thread.usleep(100000);
}
