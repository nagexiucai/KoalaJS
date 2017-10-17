var s = ",abc,def,123,456";
a = s.split(",");

for(i=0; i<a.length; i++) {
	if(i != 0)
		print(", ");
	print("[" + a[i] + "]");
}
print("\n");

s.replace(",", "--");
print(s + "\n");
