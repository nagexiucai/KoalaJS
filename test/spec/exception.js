function fooThrow() {
	throw "foo exception";
}

function barThrow() {
	fooThrow();
}

function fooNoThrow() {
	println("nothrow");
}

function barThrow() {
	fooThrow();
}

function barNoThrow() {
	fooNoThrow();
}

try {
	barThrow();
} catch(e) {
	println(e);
}

try {
	barNoThrow();
} catch(e) {
	println(e);
}

barThrow();
