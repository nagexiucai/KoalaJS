var s = ",abc,def,123,456";

print("source:\n    " + s + "\n");

print("split by ',':\n    ");
a = s.split(",");

for(i=0; i<a.length; i++) {
	if(i != 0)
		print(", ");
	print("[" + a[i] + "]");
}
print("\n");

print("replace 'c' by '--':\n    ");
s.replace(",", "--");
print(s + "\n");
