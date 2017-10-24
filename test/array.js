var arr = new Array();

for(i=0; i<10; i++) {
	arr[i] = "item " + i;
}

print("array1:\n" + arr + "\n-------------\n");
print("size: " + arr.length + "\n\n");


arr = [0, 1, 2, 3, 4, "5", "6", "7", arr[8]];

print("array2:\n" + arr + "\n-------------\n");
print("size: " + arr.length + "\n");


