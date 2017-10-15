//echo task
var cs = _threadArg;

b = cs.read(100);

if(b != undefined) 
	cs.write(b, b.size()); 

print("closed");
cs.close();
