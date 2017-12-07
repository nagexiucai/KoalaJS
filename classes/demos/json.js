var a = { 
	"name": "Misa",
	"age": 1,
	"belong" : {
		"book": "Bible",
		"f": function(i) {
			console.log("function called:" + i);
		},
		"numbers": [1, 3, 4, 5, 6]
	}
};

a.belong.f(123);

var s = RJSON.toString(a, "  ");
console.log(s);

a.name = "Yoda";
s = RJSON.toString(a, "  ");
console.log(s);

var b = RJSON.parse(s);
s = RJSON.toString(b, "  ");
console.log(s);
