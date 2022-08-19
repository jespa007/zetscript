/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	bool ScriptObject::unrefAndFreeStackElementContainer(StackElement *si){
		ScriptFunction * ir_fun;
		unsigned short var_type = GET_STK_PROPERTY_TYPES(si->properties);

		switch(var_type){

			case STK_PROPERTY_BOOL:
			case STK_PROPERTY_ZS_INT:
			case STK_PROPERTY_UNDEFINED:
			case STK_PROPERTY_NULL:
			case STK_PROPERTY_ZS_FLOAT:
				break;
			case STK_PROPERTY_FUNCTION:
				 ir_fun  = (ScriptFunction *)(si->value);
				break;
			default: // properties ...

				if( (var_type & STK_PROPERTY_SCRIPT_OBJECT)
					&&(si->value != (zs_int)this) // ensure that property don't holds its same var.
					&& (si->value != 0)
				 ){ // deallocate but not if is c or this ref
					// remove property if not referenced anymore
					if(!vm_unref_shared_script_object(vm,(ScriptObject *)si->value,NULL)){
						return false;
					}
				}
				break;
		}
		free(si);
		return true;
	}

	ScriptObject::ScriptObject(){
		idx_script_type=ZS_IDX_UNDEFINED;
		shared_pointer=NULL;
		zs=NULL;
		map_builtin_properties=new std::map<std::string,StackElement *>();
		memset(&stk_this,0,sizeof(stk_this));
		stk_this.value=(zs_int)this;
		stk_this.properties=STK_PROPERTY_SCRIPT_OBJECT;
		vm=NULL;
		ref_script_objects=new std::vector<RefObject *>();
		weak_pointers=new std::vector<ScriptObjectWeakPointer *>();
	}

	void ScriptObject::init(ZetScript *_zs){
		zs=_zs;
		vm=_zs->getVirtualMachine();

		// init builtin
		if(idx_script_type >= IDX_TYPE_SCRIPT_OBJECT_STRING && idx_script_type<IDX_TYPE_SCRIPT_OBJECT_CLASS){
			ScriptType *script_type=getScriptType();
			std::vector<Symbol *> *symbol_vars=script_type->scope_script_type->symbol_variables;
			//------------------------------------------------------------------------------
			// pre-register built-in members...
			for(unsigned i = 0; i < symbol_vars->size(); i++){
				Symbol * symbol = (Symbol *)symbol_vars->at(i);
				if(symbol->properties & SYMBOL_PROPERTY_MEMBER_PROPERTY){
					addBuiltinProperty(
							symbol->name
							,{(zs_int)(new StackMemberProperty(
									this
									,(MemberProperty *)symbol->ref_ptr)
							)
							,STK_PROPERTY_MEMBER_PROPERTY}
					);
				}
			}
		}
	}

	StackElement *ScriptObject::newBuiltinSlot(){
		StackElement *stk=(StackElement *)ZS_MALLOC(sizeof(StackElement));
		stk_builtin_elements.push_back(stk);
		return stk;
	}

	// built-in only for initialized
	StackElement * ScriptObject::addBuiltinProperty(
			const std::string & symbol_value
			, StackElement stk
	){
		std::string key_value = symbol_value;

		// if ignore duplicate was true, map resets idx to the last function...
		StackElement *new_stk=newBuiltinSlot();
		*new_stk=stk;

		(*map_builtin_properties)[key_value]=new_stk;

  	    return new_stk;
	}

	size_t ScriptObject::length(){
		return 0;
	}

	const char  * ScriptObject::getTypeName(){
		return getScriptType()->str_script_type.c_str();
	}

	const char * ScriptObjectClass::getTypeNamePtr(){
		return getScriptType()->str_script_type_ptr.c_str();
	}

	bool ScriptObject::isNativeObject(){
		 return (getScriptType()->properties & SCRIPT_TYPE_PROPERTY_C_OBJECT_REF) != 0;
	}

	ScriptType * ScriptObject::getScriptType(){
		return this->zs->getScriptTypeFactory()->getScriptType(idx_script_type);
	}

	Symbol *ScriptObject::getGetter(){
		ScriptType *script_type=this->zs->getScriptTypeFactory()->getScriptType(idx_script_type);
		MetamethodMembers *metamethod_members=&script_type->metamethod_members;

		if(metamethod_members !=NULL){
			return metamethod_members->getter;
		}
		return NULL;
	}

	std::vector<StackElement *> *ScriptObject::getSetterList(ByteCodeMetamethod _byte_code_metamethod){
		ScriptType *script_type=this->zs->getScriptTypeFactory()->getScriptType(idx_script_type);
		MetamethodMembers *metamethod_members=&script_type->metamethod_members;

		if(metamethod_members !=NULL){
			MetamethodMemberSetterInfo info=metamethod_members->getSetterInfo(_byte_code_metamethod);
			return info.setters;
		}
		return NULL;
	}

	ZetScript      * ScriptObject::getZetScript() {
		return zs;
	}

	StackElement * 			ScriptObject::getBuiltinProperty(const std::string & property_name){
		if(this->map_builtin_properties->count(property_name)!=0){
			return this->map_builtin_properties->at(property_name);
		}
		return NULL;
	}

	StackElement 	* ScriptObject::getProperty(const std::string & property_name){
		return getBuiltinProperty(property_name);
	}

	Symbol 	* ScriptObject::getScriptFunctionSymbol(const std::string & _function_member_name){
		ScriptType *script_type=getScriptType();
		std::vector<Symbol *> *s=script_type->scope_script_type->symbol_functions;
		for(int i=((int)s->size())-1;i>=0;i--){
			Symbol *symbol=(Symbol *)s->at(i);
			if(symbol->name == _function_member_name){
				return symbol;
			}
		}
		return NULL;
	}

	StackElement *ScriptObject::getThisProperty(){
		return &this->stk_this;
	}

	void	*ScriptObject::getNativeObject(){
		return this;
	}

	std::vector<StackElement *> * ScriptObject::getStkBuiltinListElements(){
		return &stk_builtin_elements;
	}

	StackElement * ScriptObject::addProperty(
			const char * symbol_value
			,const std::string & error
			,StackElement * stk_element

	){
		ZS_UNUSUED_3PARAMS(symbol_value, error, stk_element);
		VM_SET_USER_ERRORF(vm,"addProperty is not implemented");
		return NULL;
	}

	StackElement * ScriptObject::getBuiltinElementAt(short idx){
		if(idx >= (int)stk_builtin_elements.size() || idx < 0){
			VM_SET_USER_ERROR(vm,"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}
		return (StackElement *)stk_builtin_elements.at(idx);
	}

	std::string ScriptObject::toString(){
		return "Object@"+std::string(getTypeName());
	}

	void ScriptObject::refWeakPointer(ScriptObjectWeakPointer *_wp){
		weak_pointers->push_back(_wp);
	}

	void ScriptObject::refObject(RefObject *_so){
		ref_script_objects->push_back(_so);
	}

	int ScriptObject::idxRefObject(RefObject  *_ref_object){
		for(unsigned i=0; i < ref_script_objects->size();i++){
			if(ref_script_objects->at(i)==_ref_object){
				return i;
			}
		}

		return ZS_IDX_UNDEFINED;
	}

	bool ScriptObject::deRefWeakPointer(){
		if(weak_pointers->size()>0){
			// get last
			ScriptObjectWeakPointer *wp=weak_pointers->at(weak_pointers->size()-1);
			weak_pointers->pop_back();
			wp->deRefObject();
			return true;
		}
		return false;
	}

	void ScriptObject::deRefObject(RefObject  *_ref_object){
		int idx=idxRefObject(_ref_object);

		if(idx==ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERRORF("internal: member function not exist");
		}

		ref_script_objects->at(idx)->deRefObject();
		ref_script_objects->erase(ref_script_objects->begin()+idx);
	}

	ScriptTypeFactory		*	ScriptObject::getScriptTypeFactory(){
		return zs->getScriptTypeFactory();
	}

	ScriptObject::~ScriptObject(){
		// deallocate built-in function member objects
		for(unsigned i=0; i< stk_builtin_elements.size(); i++){
			StackElement *stk=(StackElement *)stk_builtin_elements[i];

			if(stk->properties & STK_PROPERTY_MEMBER_PROPERTY){
				delete (StackMemberProperty *)stk->value;
			}else if(stk->properties & STK_PROPERTY_SCRIPT_OBJECT){ // is script object to be deferrenced
				if((stk->value != (zs_int)this) // ensure that property don't holds its same var.
					&& (stk->value != 0)
				  ){ // deallocate but not if is c or this ref
					// remove property if not referenced anymore
					vm_unref_shared_script_object(vm,(ScriptObject *)stk->value,NULL);
				}
			}
			free(stk);
		}
		stk_builtin_elements.clear();
		delete map_builtin_properties;

		for(unsigned i=0; i < ref_script_objects->size(); i++){
			ScriptObject **_so=(ScriptObject **)ref_script_objects->at(i);
			*_so=NULL;
		}

		for(unsigned i=0; i < ref_script_objects->size(); i++){
			RefObject *_ref_pointer=ref_script_objects->at(i);
			_ref_pointer->deRefObject();
		}

		delete ref_script_objects;

		for(unsigned i=0; i < weak_pointers->size(); i++){
			weak_pointers->at(i)->deRefObject();

		}

		delete weak_pointers;

	}
}
