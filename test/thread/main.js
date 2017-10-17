// Main thread, start child thread and go dead loop to set Global value. 

include 'comm.js';
var i = 0;

RThread.run("thread.js", "thread arg: xxxx");

while(true) { 
	set('count', i);
	print("main thread set : " + i + "\n");

	i++;
	RThread.usleep(100000);
}
