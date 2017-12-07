var arr = new Array();

for(var i=0; i<10; i++) {
	arr[i] = "item " + i;
}

console.log("array1:\n" + arr + "\n-------------");
console.log("size: " + arr.length + "\n");


arr = [0, 1, 2, 3, 4, "5", "6", "7", arr[8]];

console.log("array2:\n" + arr + "\n-------------");
console.log("size: " + arr.length);


