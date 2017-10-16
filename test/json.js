a = { 
	"name": "Misa",
	"age": 1,
	"belong" : {
		"book": "Bible",
		"f": function() {
			print("function called.\n");
		} 
	}
};

a.belong.f();

print(a);
print("\n\n");

a.name = "Yoda";
print(a);
print("\n");

