var cs = _threadArg;

b = new Bytes();
b.from("Hello, world! "); 
cs.send(b, b.size(), 1); 

cs.shutdown(); 
cs.close();
