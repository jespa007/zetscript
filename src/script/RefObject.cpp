#include "zetscript.h"


namespace zetscript{

	RefObject::RefObject(ScriptObject *_target_obj, ScriptObject *_link_obj){
		target_obj=_target_obj;
		link_obj=_link_obj;
		target_obj->refObject(this);

	}

	ScriptObject *RefObject::getTargetObject(){
		return target_obj;
	}

	void RefObject::deRefObject(){
		if(target_obj != NULL){
			target_obj->deRefObject(this);
			target_obj=NULL;
		}
	}


	RefObject::~RefObject(){
		deRefObject();
	}
}
