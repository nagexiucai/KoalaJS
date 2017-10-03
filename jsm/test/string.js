var s = ",abc,def,123,456";
a = s.split(",");

for(i=0; i<a.length; i++) {
	print(a[i]);
}

s.replace(",", "--");
print(s);
