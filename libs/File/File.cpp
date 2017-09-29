#include "File.h"
#include <unistd.h>  

std::string File::read(const std::string& fname) {
	std::ifstream t(fname.c_str());
	if(!t)
		return "";

	std::stringstream buffer;  
	buffer << t.rdbuf();  
	t.close();
	return buffer.str();
}

#define MAX_PATH_LEN 1024
std::string File::getcwd() {
	char ret[MAX_PATH_LEN+1];
	::getcwd(ret, MAX_PATH_LEN);
	return ret;	
}

