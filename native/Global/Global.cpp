#include "Global.h"
#include "libs/File/File.h"
#include <cstdlib>
#include <map>

using namespace std;
using namespace JSM;

static ThreadLock _locker;
static map<std::string, std::string> _kv;

void Global::get(CScriptVar *c, void *userdata) {
	CTinyJS *tinyJS = (CTinyJS *)userdata;
	std::string name = c->getParameter("name")->getString();

	_locker.lock();

	std::map<std::string, std::string>::iterator it = _kv.find(name);
  if(it == _kv.end()) {
		_locker.unlock();
    return;
	}
	std::string value = it->second;

	_locker.unlock();

	c->getReturnVar()->setString(value);
}

void Global::set(CScriptVar *c, void *userdata) {
	CTinyJS *tinyJS = (CTinyJS *)userdata;
	std::string name = c->getParameter("name")->getString();
	std::string value = c->getParameter("value")->getString();

	_locker.lock();

	std::map<std::string, std::string>::iterator it = _kv.find(name);
  if(it == _kv.end()) {
		_kv.insert(map < string, string >::value_type(name, value));
	}
	else {
		it->second = value;
	}

	_locker.unlock();
}

void Global::registerFunctions(CTinyJS* tinyJS, const std::string& className) {
	addFunction(tinyJS, className, "get(name)", get, NULL);
	addFunction(tinyJS, className, "set(name, value)", set, NULL);
}

