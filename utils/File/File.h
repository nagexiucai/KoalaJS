#ifndef FILE_HH
#define FILE_HH

#include <string>  
#include <fstream>  
#include <sstream>  

class File {
public:
	static std::string read(const std::string& fname);

	static std::string getcwd();

	static std::string getFullname(const std::string& path, const std::string& fname);

	static std::string getPath(const std::string& fname);
};

#endif
