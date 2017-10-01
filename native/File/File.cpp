#include "File.h"
#include "libs/File/File.h"
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


using namespace std;
using namespace JSM;


JSFileNative::~JSFileNative() {
	if(fid >= 0)
		::close(fid);

	fid = -1;
}

JSFileNative::JSFileNative(void* data) {
	fid = -1;
}

void JSFileNative::size(CScriptVar* var, void* data) {
}

void JSFileNative::close(CScriptVar* var, void* data) {
	if(fid >= 0)
		::close(fid);

	fid = -1;
}

void JSFileNative::open(CScriptVar* var, void* data) {
	std::string fname = var->getParameter("fname")->getString();
	std::string mode = var->getParameter("mode")->getString();

	var->getReturnVar()->setInt(0);
	
	if(fname.length() == 0)
		return;

	int m = 0;

	if(mode.find("rw") != string::npos)
		m = O_RDWR;
	else if(mode.find('r') != string::npos)
		m = O_RDONLY;
	else if(mode.find('w') != string::npos)
		m = O_WRONLY;

	if(mode.find('c') != string::npos) 
		m |= O_CREAT;
	if(mode.find('t') != string::npos) 
		m |= O_TRUNC;
	if(mode.find('a') != string::npos) 
		m |= O_APPEND;

	int f = ::open(fname.c_str(), m); 
	if(f < 0)
		return;

	fid = f;
	var->getReturnVar()->setInt(f);
}

