f = new RFile();

f.open("jsm.cpp", "r");

b = f.read(f.size());
print("" + b.size() + ": " + b.toString());

f.close();


f.open("tt", "wc");
f.write(b, b.size());
f.close();
