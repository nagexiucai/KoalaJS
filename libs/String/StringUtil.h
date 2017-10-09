#ifndef STRING_UTIL_HH
#define STRING_UTIL_HH

#include <string>
#include <vector>

using namespace std;

class StringUtil {
public:
	inline static void split(const std::string& s, char c, vector<std::string>& res) {
		string str = s;	
		res.clear();
		while(true) {
			int i = str.find(c);
			if(i == std::string::npos) {
				res.push_back(str);
				break;
			}
			res.push_back(str.substr(0, i));
			str = str.substr(i+1);
		}
	}
};


#endif
