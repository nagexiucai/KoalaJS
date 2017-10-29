#include "JSFile.h"
#include "utils/File/File.h"
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


using namespace std;
using namespace JSM;

static int getFID(CScriptVar* var) {
	BCVar* thisV = var->getParameter("this");
	if(thisV == NULL)
		return -1;
	
	BCNode* n = thisV->getChild("fid");
	if(n == NULL)
		return -1;
	return n->var->getInt();
}

static BCVar* setFID(CScriptVar* var, int fid) {
	BCVar* thisV = var->getParameter("this");
	if(thisV == NULL)
		return NULL;
	
	BCNode* n = thisV->getChildOrCreate("fid");
	BCVar* v = new BCVar();
	v->setInt(fid);
	n->replace(v);
	return thisV;
}


void JSFile::size(CScriptVar* var, void* data) {
	CScriptVar * r = var->getReturnVar();
	r->setInt(-1);
	
	int fid = getFID(var);
	if(fid < 0)
		return;

	struct stat st;
	fstat(fid, &st);
	
	r->setInt(st.st_size);
}

void JSFile::close(CScriptVar* var, void* data) {
	int fid = getFID(var);
	if(fid >= 0)
		::close(fid);

	setFID(var, fid);
}

void JSFile::seek(CScriptVar* var, void* data) {
	int pos = var->getParameter("pos")->getInt();
	int fid = getFID(var);
	if(pos <= 0 || fid < 0)
		return;

	pos = ::lseek(fid, pos, SEEK_SET);
	
	CScriptVar* v = var->getReturnVar();
  v->setInt(pos);	
}


void JSFile::write(CScriptVar* var, void* data) {
	CScriptVar* v = var->getParameter("buf");
	CScriptVar* sv = var->getParameter("size");
	int fid = getFID(var);
	if(fid < 0)
		return;
	
	int size = 0;	
	if(sv != NULL)
		size = sv->getInt();

	unsigned char* p = NULL;
	if(v->isBytes()) {
		p = (unsigned char*)v->getPoint();
		if(size == 0 || size > v->getInt())
			size = v->getInt();
	}
	else {
		std::string s = v->getString();
		p = (unsigned char*)s.c_str();
		if(size == 0 || size > s.length())
			size = s.length();
	}

	v = var->getReturnVar();
 	v->setInt(-1);	

	if(p != NULL) {
		size = ::write(fid, p, size);
		v->setInt(size);
	}
}


void JSFile::read(CScriptVar* var, void* data) {
	int size = var->getParameter("size")->getInt();
	KoalaJS* js = (KoalaJS*)data;
	int fid = getFID(var);

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
	
	CScriptVar* v = js->newObject("Bytes");
	v->setPoint(buf, size, NULL, true);
	var->setReturnVar(v);
}


void JSFile::open(CScriptVar* var, void* data) {
	std::string fname = var->getParameter("fname")->getString();
	CScriptVar* mv = var->getParameter("mode");
	std::string mode = "";

	if(mv != NULL)
		mv->getString();

	int fd = -1;
	if(fname == "STDIN") { fd = 0; }
	else if(fname == "STDOUT") { fd = 1; }
	else if(fname == "STDERR") { fd = 2; }
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
	CScriptVar* v = setFID(var, fd);
	var->setReturnVar(v);
}
