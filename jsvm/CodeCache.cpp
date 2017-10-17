#include "CodeCache.h"

BCMap CodeCache::codeCache;

void CodeCache::empty() {
	for(BCMap::iterator it = CodeCache::codeCache.begin(); it != CodeCache::codeCache.end(); ++it) {
		delete it->second;
	}
	CodeCache::codeCache.clear();
}

Bytecode* CodeCache::get(const string& index) {
	BCMap::iterator it = CodeCache::codeCache.find(index);
	if(it != CodeCache::codeCache.end()) {
		return it->second;
	}
	return NULL;
}

void CodeCache::cache(const string& index, Bytecode* bc) {
	BCMap::iterator it = CodeCache::codeCache.find(index);
	if(it != CodeCache::codeCache.end()) {
		delete it->second;
		it->second = bc;
	}
	else {
		CodeCache::codeCache.insert(BCMap::value_type(index, bc));
	}
}
