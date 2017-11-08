a = { 
	"name": "Misa",
	"age": 1,
	"belong" : {
		"book": "Bible",
		"f": function() {
			print("function called.\n");
		},
		"numbers": [1, 3, 4, 5, 6]
	}
};

a.belong.f();

print(a);
print("\n\n");

a.name = "Yoda";
print(a);
print("\n");

