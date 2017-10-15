// Main thread, start child thread and go dead loop to set Global value. 

include 'comm.js';
var i = 0;

RThread.run("thread.js", "thread arg: xxxx");

var i = 0;

while(true) { 
	i++;
	set('count', i);
	print("main thread set : " + i + "\n");

	RThread.usleep(100000);
}
