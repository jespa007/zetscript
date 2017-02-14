#include "../../CZetScript.h"



CUndefined::CUndefined(){ //tInfoRegisteredClass *info_registered_class):CScriptVariable(info_registered_class, this){
	this->init(CScriptClass::getInstance()->getRegisteredClassUndefined(), (void *)this);
}
