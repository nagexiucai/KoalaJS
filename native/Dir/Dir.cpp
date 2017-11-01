#include "Dir.h"
#include <dirent.h>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


using namespace std;
using namespace JSM;


static void destroyDir(void* data) {
	DIR* dir = (DIR*)data;
	if(dir != NULL)
		::closedir(dir);
}

static DIR* getDir(BCVar* var) {
	BCVar* thisV = var->getParameter("this");
	if(thisV == NULL)
		return NULL;
	
	BCNode* n = thisV->getChild("dir");
	if(n == NULL)
		return NULL;
	return (DIR*)(n->var->getPoint());
}

static BCVar* setDir(BCVar* var, DIR* dir) {
	BCVar* thisV = var->getParameter("this");
	if(thisV == NULL)
		return NULL;
	
	BCNode* n = thisV->getChildOrCreate("dir");
	BCVar* v = new BCVar();
	v->setPoint(dir, NO_BYTES, destroyDir, true);
	n->replace(v);
	return thisV;
}

void JSDir::close(KoalaJS* js, BCVar* var, void* data) {
	setDir(var, NULL);
}

void JSDir::read(KoalaJS* js, BCVar* var, void* data) {
	var->getReturnVar()->setString("");
	DIR* dir = getDir(var);
	if(dir == NULL)
		return;

	struct dirent* dp = ::readdir(dir);
	if(dp == NULL)
		return;
	
	var->getReturnVar()->setString(dp->d_name);
}


void JSDir::open(KoalaJS* js, BCVar* var, void* data) {
	std::string name = var->getParameter("name")->getString();

	var->getReturnVar()->setInt(0);
	
	if(name.length() == 0)
		return;

	DIR* d = ::opendir(name.c_str());
	if(d == NULL)
		return;

	setDir(var, d);
	var->getReturnVar()->setInt(1);
}

