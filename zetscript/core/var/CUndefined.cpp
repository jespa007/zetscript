#include "../../CZetScript.h"



CUndefined::CUndefined(){ //tInfoRegisteredClass *info_registered_class):CScriptVariable(info_registered_class, this){
	this->init(CScriptClassFactory::getInstance()->getRegisteredClassUndefined(), (void *)this);
}
