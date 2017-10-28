#include "JSFile.h"
#include "utils/File/File.h"
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
	CScriptVar * r = var->getReturnVar();
	r->setInt(-1);

	if(fid < 0)
		return;

	struct stat st;
	fstat(fid, &st);
	
	r->setInt(st.st_size);
}

void JSFileNative::close(CScriptVar* var, void* data) {
	if(fid >= 0)
		::close(fid);

	fid = -1;
}

void JSFileNative::seek(CScriptVar* var, void* data) {
	int pos = var->getParameter("pos")->getInt();
	if(pos <= 0 || fid < 0)
		return;

	pos = ::lseek(fid, pos, SEEK_SET);
	
	CScriptVar* v = var->getReturnVar();
  v->setInt(pos);	
}


void JSFileNative::write(CScriptVar* var, void* data) {
	CScriptVar* v = var->getParameter("buf");
	int size = var->getParameter("size")->getInt();
	if(size <= 0 || fid < 0 || !v->isBytes())
		return;

	if(size > v->getInt())
		size = v->getInt();

	char* p = (char*)v->getPoint();

	v = var->getReturnVar();
 	v->setInt(-1);	

	if(p != NULL) {
		size = ::write(fid, p, size);
		v->setInt(size);
	}
}


void JSFileNative::read(CScriptVar* var, void* data) {
	int size = var->getParameter("size")->getInt();
	CTinyJS* tinyJS = (CTinyJS*)data;

	if(size <= 0 || fid < 0)
		return;
	
	char* buf = new char[size];
	if(buf == NULL)
		return;

	size = ::read(fid, buf, size);
	if(size < 0) {
		delete []buf;
		return;
	}
	
	CScriptVar* v = tinyJS->newObject("Bytes"); 
	v->setPoint(buf, size, NULL, true);
	var->setReturnVar(v);
}


void JSFileNative::open(CScriptVar* var, void* data) {
	std::string fname = var->getParameter("fname")->getString();
	std::string mode = var->getParameter("mode")->getString();

	int fd = -1;
	if(fname.length() == 0) { //no name for stdin fd
		if(mode.find('w') != string::npos)
			fd = 1;
		else
			fd = 0;
	}
	else {
		int m = O_RDONLY;
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

		fd = ::open(fname.c_str(), m, S_IRUSR | S_IWUSR | S_IRGRP); 
		if(fd < 0)
			return;
	}
	fid = fd;
	var->getReturnVar()->setInt(fd);
}

