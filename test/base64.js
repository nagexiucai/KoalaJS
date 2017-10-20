b = new Bytes();
b.from("Test Base64");

print("source:\n    \"" + b.toString() + "\"\n");

c = RBase64.encode(b);
print("encoded:\n    \"" + c + "\"\n");

s = RBase64.decode(c).toString();
print("decoded:\n    \"" + s + "\"\n");

