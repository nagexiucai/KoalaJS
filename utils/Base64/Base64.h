#ifndef BASE64_H
#define BASE64_H

#include <string>

class Base64 {
public:
	static std::string encode(unsigned char const* , unsigned int len);

	static std::string decode(std::string const& s);

	static unsigned char* decode(std::string const& s, size_t& size);
	
	static size_t decodeLen(std::string const& s);
};

#endif
