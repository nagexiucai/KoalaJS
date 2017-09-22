include('comm.js');

Thread.run("thread.js");

var i = 0;

while(true) { 
	i++;

	print("main thread set : " + i);
	set('count', ""+i);

	Thread.usleep(100000);
}
