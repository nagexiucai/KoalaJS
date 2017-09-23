// Main thread, start child thread and go dead loop to set Global value. 

include('comm.js');
var i = 0;

Thread.run("thread.js");

var i = 0;

while(true) { 
	i++;

	print("main thread set : " + i);
	set('count', ""+i);

	Thread.usleep(100000);
}
