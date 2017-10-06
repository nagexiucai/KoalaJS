var a=1, s="fff";
const c=1.232;

if(b == 0) {
	print(a);
}
else if(b == 1) {
	print(b);
}

function f1(j) {
	while(j) {
		j = 0;
		if(true) 
			continue;
		else
			break;
	}
}

function f2(i, j) {
	f1(j);
	return i;
}

function f3(i, j) {
	return f2(i, j);
}


a.b = f3(1, 3);
