// Main thread, start child thread and go dead loop to set Global value. 

import 'comm.js';
var i = 0;

RThread.run("thread.js", "thread arg: xxxx");

while(true) { 
	set('count', i);
	console.log("main thread set : " + i);

	i++;
	RThread.usleep(100000);
}
