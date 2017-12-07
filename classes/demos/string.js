var s = ",abc,def,123,456";

console.log("source:\n    " + s);

console.log("split by ',':");
var a = s.split(",");

for(var i=0; i<a.length; i++) {
	if(i != 0)
		console.log(", " + "[" + a[i] + "]");
}

console.log("replace ',' by '--':");
s.replace(",", "--");
console.log(s);


var i = (0x80<<24) | (0x70<<16) | (0x60<<8) | 0x50;
console.log("HEX: " + String.format("0x%08X", i) + ": " + i);
