// Main thread, start child thread and go dead loop to set Global value. 

include('comm.js');
var i = 0;

R.Thread.run("thread.js");

var i = 0;

while(true) { 
	i++;

	print("main thread set : " + i);
	set('count', ""+i);

	R.Thread.usleep(100000);
}
