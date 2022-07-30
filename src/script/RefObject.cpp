#include "zetscript.h"


namespace zetscript{

	RefObject::RefObject(ScriptObject *_ref_object, ScriptObject *_ref_from_object){
		ref_object=_ref_object;
		ref_from_object=_ref_from_object;
		ref_object->refObject(this);

	}

	ScriptObject *RefObject::getRefObject(){
		return ref_object;
	}

	void RefObject::deRefObject(){
		if(ref_object == NULL){
			THROW_RUNTIME_ERRORF("Already deferenced object");
		}

		ref_object->deRefObject(this);
		ref_object=NULL;
	}


	RefObject::~RefObject(){
		deRefObject();
	}
}
