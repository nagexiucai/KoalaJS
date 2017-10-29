#include "Global.h"
#include <cstdlib>
#include <stdio.h>
#include <map>

using namespace std;
using namespace JSM;

static ThreadLock _locker;

static map<std::string, BasicValueT> _kv;

void Global::get(CScriptVar *c, void *userdata) {

	KoalaJS *js = (KoalaJS *)userdata;
	std::string name = c->getParameter("name")->getString();

	_locker.lock();
	std::map<std::string, BasicValueT>::iterator it = _kv.find(name);
  if(it == _kv.end()) {
		_locker.unlock();
    return;
	}

	const BasicValueT& value = it->second;
	
	switch(value.type) {
		case BasicValueT::INT:
			c->getReturnVar()->setInt(value.v.intV);
			break;
		case BasicValueT::DOUBLE:
			c->getReturnVar()->setDouble(value.v.doubleV);
			break;
		case BasicValueT::STRING:
			c->getReturnVar()->setString(value.v.stringV);
			break;
		case BasicValueT::BYTES:
			unsigned char* p = NULL;
			int size = 0;
			if(value.v.bytesV != NULL && value.v.intV > 0) {
				p = new unsigned char[value.v.intV];
				if(p != NULL) {
					memcpy(p, value.v.bytesV, value.v.intV);
					size = value.v.intV;
				}
			}
				
			c->getReturnVar()->setPoint(p, size, NULL, true);
			break;
	}

	_locker.unlock();
}

void Global::set(CScriptVar *c, void *userdata) {

	KoalaJS *js = (KoalaJS *)userdata;
	std::string name = c->getParameter("name")->getString();
	CScriptVar* v = c->getParameter("value");

	BasicValueT value;

	if(v->isInt()) {
		value.type = BasicValueT::INT;
		value.v.intV = v->getInt();
	} 
	else if(v->isDouble()) {
		value.type = BasicValueT::DOUBLE;
		value.v.doubleV = v->getDouble();
	}
	else if(v->isString()) {
		value.type = BasicValueT::STRING;
		value.v.stringV = v->getString();
	}
	else if(v->isBytes()) {
		value.type = BasicValueT::BYTES;
		int size = v->getInt();
		unsigned char* p = (unsigned char*)v->getPoint();
		
		if(size > 0 && p != NULL) {
			value.v.bytesV = new unsigned char[size];
			value.v.intV = size;
			memcpy(value.v.bytesV, p, size);
		}
	}
	else {
		TRACE("Only can set global parameter with basic type.\n");
		value.type = BasicValueT::UNDEF;
	}

	_locker.lock();

	std::map<std::string, BasicValueT>::iterator it = _kv.find(name);
  if(it == _kv.end()) {
		_kv.insert(map <string,  BasicValueT>::value_type(name, value));
	}
	else {
		if(it->second.v.bytesV != NULL) {
			delete it->second.v.bytesV;
		}
		it->second = value;
	}

	_locker.unlock();
}


