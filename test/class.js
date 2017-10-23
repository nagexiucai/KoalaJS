// class demo
function Man(name, age) {
	this.name = name;
	this.age = age;
	this.show = function() {
		println("name: " + name);
		println("age: " + age);
	};
}

man = new Man("Misa", 25);
man.show();

man.name = "Yoda";
man.age = 1800;
man.show();
