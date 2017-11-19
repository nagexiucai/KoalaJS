#ifndef BC_CodeCache_HH
#define BC_CodeCache_HH

#include "Bytecode.h"
#include <map>
#include <string>

using namespace std;

typedef map<string, Bytecode*> BCMap;

class CodeCache {
	static BCMap codeCache;
public:
	static void empty();

	static Bytecode* get(const string& index);

	static void cache(const string& index, Bytecode* bc);
};

#endif
