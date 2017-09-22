var i = 0;

Thread.run("thread.js");

while(true) { 
	var a = "" + i;
	i++;

	Global.set('count', a);

	print("main thread set : " + a);
	Thread.usleep(100000);
}
