/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectIterator::ScriptObjectIterator(ZetScript *_zs):ScriptObject(_zs){
		n_it_elements=0;
		memset(stk_it_elements,0,sizeof(stk_it_elements));
		init(SCRIPT_CLASS_STRING(this), (void *)this);
		object=NULL;
	}

	void   ScriptObjectIterator::pushSf(ScriptObjectIterator *si,StackElement  * stk){
		si->push(stk);
	}

	void ScriptObjectIterator::push(StackElement  * _stk){
		if(n_it_elements < IDX_ITERATOR_ELEMENT_MAX){
			switch(n_it_elements){
			case IDX_ITERATOR_ELEMENT_OBJECT:
				if((_stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)
						||
					(_stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT)){

				}
				break;
			case IDX_ITERATOR_ELEMENT_KEY:
				break;
			case IDX_ITERATOR_ELEMENT_VALUE:
				break;
			}
			//stk_it_elements[n_it_elements++]=_stk;
		}
	}

	void	 ScriptObjectIterator::nextSf(ScriptObjectIterator *si){
		si->next();
	}

	void	 ScriptObjectIterator::next(){

	}


	bool	 ScriptObjectIterator::endSf(ScriptObjectIterator *si){
		return si->end();
	}

	bool	 ScriptObjectIterator::end(){
		return false;
	}

}
