#include "File.h"
#include <unistd.h>  
#include <vector>  

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

std::string File::getFullname(const std::string& path, const std::string& fname) {
	if(fname.length() == 0)
    return "";

  std::string full;

  if(fname[0] == '/') {
    full = fname;
  }
  else {
    if(path.length() == 0)
      full = File::getcwd() + '/' + fname;
    else
      full = path + fname;

		std::vector<std::string> a;
		std::string name, rest;
		rest = full;
		size_t i = 0;

		while(true) {	
			i = rest.rfind('/');
			if(i == std::string::npos)
				break;

			name = rest.substr(i+1);
			rest = rest.substr(0, i);
			if(name.length() > 0)
				a.insert(a.end(), name);
		}

		size_t size = a.size();
		if(size == 0)
			return "";
		
		size_t up = 0;
		full = "";

		for(i=0; i<size; i++) {
			name = a.at(i);	
			if(name.length() == 0 || name == ".") {
				continue;
			}
			else if(name == "..") {
				up++;
				continue;
			}
			else {
				if(up > 0) {
					up--;
					continue;
				}
				full = "/" + name + full;
			}
		}	
  }

  return full;
}

std::string File::getPath(const std::string& fname) {
	
	if(fname.length() == 0)
    return "";

  size_t i = fname.rfind('/');
  if(i == std::string::npos)
    return "";
  return fname.substr(0, i+1);
}
