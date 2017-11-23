var a = 1;
print("before: " + a);

RVM.exec("a = 0;", true);

print(", after: " + a + "\n");
