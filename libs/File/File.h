#ifndef FILE_HH
#define FILE_HH

#include <string>  
#include <fstream>  
#include <sstream>  

class FReader {
public:
	static std::string read(const std::string& fname) {
		std::ifstream t(fname.c_str());
		if(!t)
			return "";

		std::stringstream buffer;  
		buffer << t.rdbuf();  
		t.close();
		return buffer.str();
	}
};

#endif
