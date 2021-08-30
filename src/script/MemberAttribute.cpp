#include "zetscript.h"

namespace zetscript{

	MemberAttribute::MemberAttribute(const std::string & _attribute_name){
		getter= NULL;
		post_inc=NULL;
		post_dec=NULL;
		pre_inc=NULL;
		pre_dec=NULL;
		attribute_name=_attribute_name;
	}

	void MemberAttribute::addSetter(ScriptFunction *f){

		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		stk->value=(zs_int)f;
		stk->properties=STK_PROPERTY_FUNCTION;
		setters.push_back(stk);
	}

	MemberAttribute::~MemberAttribute(){

		for(unsigned i=0;i < setters.size(); i++){
			StackElement *stk_el=(StackElement *)setters.at(i);
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
