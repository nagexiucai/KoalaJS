#include "Dir.h"
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


using namespace std;
using namespace JSM;


JSDirNative::~JSDirNative() {
	if(dir != NULL)
		::closedir(dir);

	dir = NULL; 
}

JSDirNative::JSDirNative(void* data) {
	dir = NULL;
}

void JSDirNative::close(CScriptVar* var, void* data) {
	if(dir != NULL)
		::closedir(dir);

	dir = NULL;
}

void JSDirNative::read(CScriptVar* var, void* data) {
	var->getReturnVar()->setString("");
	if(dir == NULL)
		return;

	struct dirent* dp = ::readdir(dir);
	if(dp == NULL)
		return;
	
	var->getReturnVar()->setString(dp->d_name);
}


void JSDirNative::open(CScriptVar* var, void* data) {
	std::string name = var->getParameter("name")->getString();

	var->getReturnVar()->setInt(0);
	
	if(name.length() == 0)
		return;

	DIR* d = ::opendir(name.c_str());
	if(d == NULL)
		return;

	dir = d;
	var->getReturnVar()->setInt(1);
}

