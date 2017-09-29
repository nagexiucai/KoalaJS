#ifndef FILE_HH
#define FILE_HH

#include <string>  
#include <fstream>  
#include <sstream>  

class File {
public:
	static std::string read(const std::string& fname);

	static std::string getcwd();

};

#endif
