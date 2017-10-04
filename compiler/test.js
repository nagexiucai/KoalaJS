var a.b=1, s="fff";
const c=1.232;

function f1(j) {
	j = 0;
}

function f2(i, j) {
	f1(j);
	return i;
}

function f3(i, j) {
	return f2(i, j);
}


f3(1, 3);
