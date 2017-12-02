var s = ",abc,def,123,456";

print("source:\n    " + s + "\n");

print("split by ',':\n    ");
var a = s.split(",");

for(var i=0; i<a.length; i++) {
	if(i != 0)
		print(", ");
	print("[" + a[i] + "]");
}
print("\n");

print("replace ',' by '--':\n    ");
s.replace(",", "--");
println(s);


var i = (0x80<<24) | (0x70<<16) | (0x60<<8) | 0x50;
println("HEX: " + String.format("0x%08X", i) + ": " + i);
