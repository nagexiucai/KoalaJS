//echo task
var cs = _threadArg;

while(true) {
	b = cs.read(100);

	if(b == undefined || b.size() == 0) 
		break;

	cs.write(b, b.size()); 
}

print("closed");
cs.close();
