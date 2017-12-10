var a = 1;
console.log("before: " + a);

RVM.exec("a = 0;", true);

console.log("after: " + a);
