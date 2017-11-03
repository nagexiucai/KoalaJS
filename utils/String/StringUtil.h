#ifndef STRING_UTIL_HH
#define STRING_UTIL_HH

#include <string>
#include <vector>
#include <sstream>  

using namespace std;

class StringUtil {
public:
	inline static void split(const std::string& s, char c, vector<std::string>& res) {
		string str = s;	
		res.clear();
		while(true) {
			int i = (int)str.find(c);
			if(i == std::string::npos) {
				res.push_back(str);
				break;
			}
			res.push_back(str.substr(0, i));
			str = str.substr(i+1);
		}
	}

	inline static string trim(const string& s) {
		string ret = StringUtil::trimLeft(s);
		return StringUtil::trimRight(ret);
	}

	inline static string trimLeft(const string& s) {
		size_t l = s.length();

		for(size_t i=0; i<l; ++i) {
			char b = s.at(i);
			if(b == ' ' ||
					b == '\t' ||
					b == '\r' ||
					b == '\n') {
				continue;
			}
			return s.substr(i);
		}
		return "";
	}

	inline static string trimRight(const string& s) {
		for(int i=(int)s.length()-1; i>=0; --i) {
			char b = s.at(i);
			if(b == ' ' ||
					b == '\t' ||
					b == '\r' ||
					b == '\n') {
				continue;
			}
			return s.substr(0, i+1);
		}
		return "";
	}

	inline static string from(int i) {
		ostringstream ostr;
		ostr << i;
		return ostr.str();
	}

	inline static string from(double f) {
		ostringstream ostr;
		ostr << f;
		return ostr.str();
	}
};


#endif
