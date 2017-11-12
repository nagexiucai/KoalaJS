var a = { 
	"name": "Misa",
	"age": 1,
	"belong" : {
		"book": "Bible",
		"f": function(i) {
			print("function called:" + i + "\n");
		},
		"numbers": [1, 3, 4, 5, 6]
	}
};

a.belong.f(123);

print(a);
print("\n\n");


a.name = "Yoda";
print(a);
print("\n");

var s = "" + a;
var b = RJSON.parse(s);
println(b);
