#include "../../CZetScript.h"


CNull::CNull(){

    //m_classStr="undefined";
   // m_pointerClassStr="undefined";
	this->init(CScriptClass::getInstance()->getRegisteredClassNull(), (void *)this);

}
