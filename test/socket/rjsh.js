var f = _threadArg;
Debug.setFD(f.getFD());

//show help message
function help() {
	f.write("[Help] JSM Javascript engine, you can input javascript code here.\n" + 
					"commands:\n" + 
					"\thelp       : show this help.\n" + 
					"\trun        : run the new input javascript, will reset runtime first.\n" + 
					"\texit       : quit shell.\n");
}
help();

f.write("jsh:> ");

var cmd = true;
var input = "";

while(true) {
	var ln = f.read(1000); //read line
	var sln = ln.toString().trim();
	if(sln.length == 0) {
		if(cmd)
			cmd = false;
		else {
			cmd = true;
		}
	}
	else {
		if(cmd) { 
			if(sln == "exit") {
				break;
			}
			else if(sln == "help") {
				help();
			}
			else if(sln == "run") {
				RVM.exec(input);
				input = "";
			}
			else {
				input = input + sln;
				cmd = false;
			}
		}
		else {
			input = input + sln;
			cmd = false;
		}
	}
	
	if(cmd)	
		f.write("jsh:> ");	
}

f.write("Byte!\n");
f.close();
