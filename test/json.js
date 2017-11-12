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

var s = RJSON.toString(a, "  ");
println(s);
print("\n");

a.name = "Yoda";
s = RJSON.toString(a, "  ");
println(s);
print("\n");

var b = RJSON.parse(s);
s = RJSON.toString(b, "  ");
println(s);
