#include "core/zg_core.h"



CStruct::CStruct(){//tInfoRegisteredClass *info_registered_class):CScriptVariable(info_registered_class, this){
	this->init(CScriptClassFactory::getInstance()->getRegisteredClassStruct(), (void *)this);
}
