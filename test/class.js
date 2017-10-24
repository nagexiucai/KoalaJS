// javascript class demo
function Jedi(name, age) {
	this.name = name;
	this.age = age;
	this.say = function() {
		print("name: " + name);
		print(", age: " + age);
		println(", May the Force be with you!");
	};
}

jedi = new Jedi("Misa", 25);
jedi.say();

jedi.name = "Yoda";
jedi.age = 1800;
jedi.say();

//class definition
class Hello {
	say(word) {
		println(word);
	}

	get word() {
		return "Rule the Galaxy!";
	}
}

h = new Hello();
h.say(h.word);

