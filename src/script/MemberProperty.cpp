/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	MemberProperty::MemberProperty(ScriptClass *_script_class,const zs_string & _attribute_name){
		getter= NULL;
		post_inc=NULL;
		post_dec=NULL;
		pre_inc=NULL;
		pre_dec=NULL;
		attribute_name=_attribute_name;
		script_class=_script_class;
	}

	void MemberProperty::addSetter(ScriptFunction *f){

		StackElement *stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		stk->value=(zs_int)f;
		stk->properties=STK_PROPERTY_FUNCTION;
		setters.push_back((zs_int)stk);
	}

	MemberProperty::~MemberProperty(){

		for(int i=0;i < setters.count; i++){
			StackElement *stk_el=(StackElement *)setters.items[i];
			free(stk_el);
		}

		setters.clear();
		getter = NULL;
		post_inc = NULL;
		post_dec = NULL;
		pre_inc = NULL;
		pre_dec = NULL;


	}
};
