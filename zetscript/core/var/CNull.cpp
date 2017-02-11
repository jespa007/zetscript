#include "../../CZetScript.h"


CNull::CNull(){

    //m_classStr="undefined";
   // m_pointerClassStr="undefined";
	this->init(CScriptClassFactory::getInstance()->getRegisteredClassNull(), (void *)this);

}
