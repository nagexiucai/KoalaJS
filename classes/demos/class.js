// javascript class demo
function Jedi(name, age) {
	this.name = name;
	this.age = age;
	this.say = function() {
		console.log("name: " + name +
			", age: " + age +
			", May the Force be with you!");
	};
}

var jedi = new Jedi("Misa", 25);
jedi.say();

jedi.name = "Yoda";
jedi.age = 1800;
jedi.say();

//class definition
print("\n");
class Hello {
	constructor() {
		this.name = "";
	}

	say(word) {
		console.log(name + ": " + word);
	}

	get word() {
		console.log("Get-function word");
		return "Rule the Galaxy!";
	}
}

var h = new Hello();
h.name = "Misa";
h.say(h.word);

