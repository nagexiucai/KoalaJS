// Child thread, dead loop to get global value which is set by main thread. 
include 'comm.js';

var v;
while(true) {
	v = get('count');

	if(v != undefined)
		console.log("sub thread get : " + v + ": " + _threadArg);

	RThread.usleep(100000);
}
