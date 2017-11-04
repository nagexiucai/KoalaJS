#include "JSFile.h"
#include "utils/File/File.h"
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


using namespace std;
using namespace JSM;

static int getFID(BCVar* var) {
	BCVar* thisV = var->getParameter("this");
	if(thisV == NULL)
		return -1;
	
	BCNode* n = thisV->getChild("fid");
	if(n == NULL)
		return -1;
	return n->var->getInt();
}

static BCVar* setFID(BCVar* var, int fid) {
	BCVar* thisV = var->getParameter("this");
	if(thisV == NULL)
		return NULL;
	
	BCNode* n = thisV->getChildOrCreate("fid");
	BCVar* v = new BCVar();
	v->setInt(fid);
	n->replace(v);
	return thisV;
}


void JSFile::size(KoalaJS* js, BCVar* var, void* data) {
	BCVar * r = var->getReturnVar();
	r->setInt(-1);
	
	int fid = getFID(var);
	if(fid < 0)
		return;

	struct stat st;
	fstat(fid, &st);
	
	r->setInt((int)st.st_size);
}

void JSFile::close(KoalaJS* js, BCVar* var, void* data) {
	int fid = getFID(var);
	if(fid >= 0)
		::close(fid);

	setFID(var, fid);
}

void JSFile::seek(KoalaJS* js, BCVar* var, void* data) {
	int pos = var->getParameter("pos")->getInt();
	int fid = getFID(var);
	if(pos <= 0 || fid < 0)
		return;

	pos = (int)::lseek(fid, pos, SEEK_SET);
	
	BCVar* v = var->getReturnVar();
  v->setInt(pos);	
}


void JSFile::write(KoalaJS* js, BCVar* var, void* data) {
	BCVar* v = var->getParameter("buf");
	BCVar* sv = var->getParameter("size");
	int fid = getFID(var);
	if(fid < 0)
		return;
	
	int size = 0;	
	if(sv != NULL)
		size = sv->getInt();

	unsigned char* p = NULL;
	std::string s;
	if(v->isBytes()) {
		p = (unsigned char*)v->getPoint();
		if(size == 0 || size > v->getInt())
			size = v->getInt();
	}
	else {
		s = v->getString();
		p = (unsigned char*)s.c_str();
		if(size == 0 || size > (int)s.length())
			size = (int)s.length();
	}

	v = var->getReturnVar();
 	v->setInt(-1);	

	if(p != NULL) {
		size = (int)::write(fid, p, size);
		v->setInt(size);
	}
}


void JSFile::read(KoalaJS* js, BCVar* var, void* data) {
	int size = var->getParameter("size")->getInt();
	int fid = getFID(var);

	if(size <= 0 || fid < 0)
		return;
	
	char* buf = new char[size];
	if(buf == NULL)
		return;

	size = (int)::read(fid, buf, size);
	if(size < 0) {
		delete []buf;
		return;
	}
	
	BCVar* v = js->newObject("Bytes");
	v->setPoint(buf, size, NULL, true);
	var->setReturnVar(v);
}


void JSFile::open(KoalaJS* js, BCVar* var, void* data) {
	std::string fname = var->getParameter("fname")->getString();
	BCVar* mv = var->getParameter("mode");
	std::string mode = "";

	if(mv != NULL)
		mv->getString();

	int fd = -1;
	if(fname == STDIN) { fd = 0; }
	else if(fname == STDOUT) { fd = 1; }
	else if(fname == STDERR) { fd = 2; }
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
	BCVar* v = setFID(var, fd);
	var->setReturnVar(v);
}
