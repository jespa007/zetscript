#include "../../CZetScript.h"


CVoid::CVoid(){//tInfoRegisteredClass *info_registered_class):CScriptVariable(info_registered_class, this){
	this->init(CScriptClass::getInstance()->getRegisteredClassVoid(), (void *)this);
}
