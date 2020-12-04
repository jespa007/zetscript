/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void ScriptObject::createSymbols(ScriptClass *ir_class){
		//FunctionSymbol *si;
		StackElement *se;
		std::string error;

		// special var this
		memset(&stk_this,0,sizeof(stk_this));
		stk_this.var_ref=this;
		stk_this.properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT;

		// special var count
		memset(&stk_count,0,sizeof(stk_count));
		stk_count.stk_value=0;//this->countUserProperties();
		stk_count.var_ref=0;
		stk_count.properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT;
		//}

		// Register c vars...
		for ( unsigned i = 0; i < ir_class->symbol_members->count; i++){

			Symbol * symbol = (Symbol *)ir_class->symbol_members->items[i];
			bool is_script_function=symbol->properties & SYMBOL_PROPERTY_FUNCTION;
			bool ignore_duplicates=is_script_function==false; // we ignore duplicates in case of script function, to allow super operation work.

			// we add symbol as property. In it will have the same idx as when were evaluated declared symbols on each class
			if((se=addPropertyBuiltIn(
				symbol->name
			))==NULL){
				return;
			}

			if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){

				ScriptFunction * ir_fun  = (ScriptFunction *)symbol->ref_ptr;
				se->stk_value=this;
				se->var_ref=ir_fun;
				se->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION; // tell stack element that is a function
			}
			else{ // var... should be native in principle ?

				if(symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF) //if(IS_CLASS_C)
				{
					// we know the type object so we assign the pointer ...
					void *ptr_variable=(void *)((zs_int)this->c_object + symbol->ref_ptr);

					*se=convertSymbolToStackElement(this->zs,symbol,ptr_variable);
				}else if(symbol->properties & (SYMBOL_PROPERTY_CONST)){ // stack element
					se->stk_value=NULL;
					se->var_ref=(void *)symbol->ref_ptr;
					se->properties=MSK_STACK_ELEMENT_PROPERTY_PTR_STK;
				}else{
					VM_SET_USER_ERROR(this->virtual_machine,"internal error: symbol should be const or native var");
					return;
				}
			}
		}

		// register custom built-in vars
		/*if((se=addPropertyBuiltIn(
			"length"
		))==NULL){
			return;
		}

		se->var_ref=&lenght_user_properties;
		se->properties=(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C|MSK_STACK_ELEMENT_PROPERTY_READ_ONLY);*/

		// start property idx starts  from last built-in property...
		idx_start_user_properties=stk_properties->count;
		//lenght_user_properties=0;
	}

	void ScriptObject::setup(){

		shared_pointer = NULL;

		script_class = NULL;
		c_object = NULL;
		created_object = NULL;
		value = NULL;
		was_created_by_constructor=false;
		script_class_native=NULL;
		idx_class = ZS_IDX_UNDEFINED;
		//aux_string ="";
		delete_c_object = false; // --> user is responsible to delete C objects!
		info_function_new=NULL;
		instruction_new=NULL;
		memset(&stk_this,0,sizeof(stk_this));
		stk_properties=new zs_vector();
		map_property_keys=new zs_map();
		idx_start_user_properties = 0;
	}

	ScriptObject::ScriptObject(){
		setup();
	}

	ScriptObject::ScriptObject(ZetScript	*_zs){
		setup();

		zs=_zs;
		virtual_machine = zs->getVirtualMachine();
		script_class_factory = zs->getScriptClassFactory();
	}

	void ScriptObject::init(ScriptClass *irv, void * _c_object){

		this->script_class = irv;
		idx_class = irv->idx_class;
		c_object = _c_object;
		
		script_class_native=NULL;

		if(c_object == NULL){ // if object == NULL, the script takes the control. Initialize c_class (script_class_native) to get needed info to destroy create the C++ object.
			if(script_class->isNativeClass()){
				script_class_native=script_class;
				created_object = (*script_class->c_constructor)();
				was_created_by_constructor=true;
				c_object = created_object;
			}else {
				ScriptClass *sc=script_class;
				// get first class with c inheritance...

				while((sc->idx_base_classes->count>0) && (script_class_native==NULL)){
					sc=GET_SCRIPT_CLASS(this,sc->idx_base_classes->items[0]); // get base class (only first in script because has single inheritance)...
					if(sc->isNativeClass()){ // we found the native script class!
						script_class_native=sc;
						if(sc->c_constructor!=NULL){ // if not null is class, else is singleton or static class already created
							created_object = (*sc->c_constructor)();
							was_created_by_constructor=true;
							c_object = created_object;
						}
					}
				}

			}

		}else{ // pass the pointer reference but in principle is cannot be deleted when the scriptvar is deleted...
			script_class_native=irv;
		}

		// only create symbols if not std::string or std::vector type to make it fast ...
		if(idx_class >= IDX_BUILTIN_TYPE_MAX
			|| idx_class == IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_STRING
			|| idx_class == IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_VECTOR
			|| idx_class == IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT
		){
			createSymbols(irv);
		}
	}

	ScriptFunction *ScriptObject::getConstructorFunction(){

		if(script_class->idx_function_member_constructor != ZS_IDX_UNDEFINED){
			return (ScriptFunction *)script_class->symbol_members->items[script_class->idx_function_member_constructor];
		}

		return NULL;
	}

	bool ScriptObject::setIdxClass(unsigned char idx){
		ScriptClass *_info_registered_class =  GET_SCRIPT_CLASS(this,idx);//ScriptClass::getInstance()->getRegisteredClassByIdx(idx);

		if(_info_registered_class == NULL){
			return false;
		}

		script_class = _info_registered_class;
		return true;
	}

	bool ScriptObject::itHasSetMetamethod(){
		return getProperty(ByteCodeMetamethodToSymbolStr(BYTE_CODE_METAMETHOD_SET),NULL) != NULL;
		/*metamethod_operator[BYTE_CODE_METAMETHOD_SET]!=NULL){
			return script_class->metamethod_operator[BYTE_CODE_METAMETHOD_SET]->count > 0;
		}
		return false;*/
	}

	void ScriptObject::deleteNativeObjectOnDestroy(bool _delete_on_destroy){
		created_object=NULL;
		if((this->delete_c_object = _delete_on_destroy)==true){
			created_object=c_object;
		}
	}

	StackElement *ScriptObject::newSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk={NULL,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		stk_properties->push_back((zs_int)stk);
		return stk;
	}

	StackElement *ScriptObject::popUserProperty(){
		if(stk_properties->count<=idx_start_user_properties){
			VM_SET_USER_ERROR(this->virtual_machine,"pop(): error stack already empty");
			return NULL;
		}

		StackElement *stk_element=((StackElement *)stk_properties->items[stk_properties->count-1]);
		if(!eraseProperty(stk_properties->count-1)){
			return NULL;
		}
		return stk_element;
	}

	// built-in only for initialized
	StackElement * ScriptObject::addPropertyBuiltIn(const std::string & symbol_value){
		std::string key_value = symbol_value;

		if(idx_start_user_properties != 0){
			VM_SET_USER_ERROR(this->virtual_machine,"addPropertyBuiltIn should be used within ScriptObject::createSymbols");
			return NULL;
		}

		// if ignore duplicate was true, map resets idx to the last function...
		map_property_keys->set(key_value.c_str(),stk_properties->count);


		StackElement *new_stk=newSlot();
		*new_stk={
				0,
				0,
				MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED
			}; //assign var

  	    return new_stk;
	}

	StackElement * ScriptObject::addProperty(
			const std::string & symbol_value
			,std::string & error
			//, const ScriptFunction *info_function
			//,Instruction *src_instruction
			,StackElement * sv
			,int * idx_stk_element

		){
		StackElement si;
		bool error_symbol=false;

		char *aux_p=(char *)symbol_value.c_str();
		if(
			   ('a' <= *aux_p && *aux_p <='z') ||
			   ('A' <= *aux_p && *aux_p <='Z')


			){ // let's see it has right chars...
				aux_p++;
				while((*aux_p!=0) && (
					  ('a' <= *aux_p && *aux_p <='z') ||
					  ('0' <= *aux_p && *aux_p <='9') ||
					  (*aux_p=='_') ||
					  ('A' <= *aux_p && *aux_p <='Z'))){
					aux_p++;
				}

				if((*aux_p!=0)){
					error_symbol=true;
				}
		}else{
			error_symbol=true;
		}

		if(error_symbol){
			error=zs_strutils::format("invalid symbol name \"%s\". Check it doesn't start with 0-9, it has no spaces, and it has no special chars like :,;,-,(,),[,], etc.",symbol_value.c_str());
			return NULL;
		}

		if(map_property_keys->exist(symbol_value.c_str())){
			error=zs_strutils::format("\"%s\" symbol already exists",symbol_value.c_str());
			return NULL;
		}

		if(sv != NULL){
			si = *sv;

			// update n_refs +1
			if(sv->properties&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
				if(virtual_machine->sharePointer(((ScriptObject *)(sv->var_ref))->shared_pointer) == false){
					return NULL;
				}
			}

		}else{

			si={
				0,
				0,
				MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED
			};
		}

		if(idx_stk_element != NULL){
			*idx_stk_element=stk_properties->count;
		}

		std::string key_value = symbol_value;
		map_property_keys->set(key_value.c_str(),stk_properties->count);
		StackElement *new_stk=newSlot();
		*new_stk=si; //assign var
		return new_stk;
	}

	int  ScriptObject::getPropertyIdx(const std::string & property_name){//,bool only_var_name){

		bool exists;
		int idx_stk_element=this->map_property_keys->get(property_name.c_str(),exists);
		if(exists){
			return idx_stk_element;
		}
		return ZS_IDX_UNDEFINED;
	}

	Symbol * ScriptObject::getSymbolMemberByIdx(int idx){

		return (Symbol *)this->script_class->symbol_members->items[idx];
	}

	StackElement * ScriptObject::getProperty(const std::string & property_name, int * idx){//,bool only_var_name){

		// special properties
		if(property_name == "count"){
			stk_count.stk_value=(void *)this->countUserProperties();
			return &stk_count;
		}

		bool exists;
		zs_int idx_stk_element=this->map_property_keys->get(property_name.c_str(),exists);
		if(exists){
			if(idx!=NULL){
				*idx=idx_stk_element;
			}

			return (StackElement *)stk_properties->items[idx_stk_element];
		}
		return NULL;
	}

	StackElement * ScriptObject::getProperty(short idx){

		if(idx==ZS_IDX_SCRIPTVAR_PROPERTY_IS_THIS){
			return &stk_this;
		}

		if(idx >= (int)stk_properties->count){
			VM_SET_USER_ERROR(this->virtual_machine,"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}

		return (StackElement *)stk_properties->items[idx];
	}

	bool ScriptObject::eraseProperty(short idx){//onst std::string & varname){

		StackElement *si;
		ScriptFunction * ir_fun;

		if(idx >= stk_properties->count){
			VM_SET_USER_ERROR(this->virtual_machine,"idx out of bounds (%i>=%i)",idx,stk_properties->count);
			return false;
		}

		si=(StackElement *)stk_properties->items[idx];
		unsigned short var_type = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(si->properties);

		switch(var_type){

			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION:
				 ir_fun  = (ScriptFunction *)(si->var_ref);
				break;
			default: // properties ...

				if(var_type & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT){
					if(((si->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C) != MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C)
						&& (si->var_ref != this) // ensure that property don't holds its same var.
						&& (si->var_ref != 0)
					  ){ // deallocate but not if is c or this ref

						// remove property if not referenced anymore
						if(!virtual_machine->unrefSharedScriptObject(((ScriptObject *)(si->var_ref))->shared_pointer,true)){
							return false;
						}

					}
				}
				break;
		}

		// remove symbol on std::vector ...
		free((void *)stk_properties->items[idx]);
		stk_properties->erase(idx);

		/*if(stk_properties->count<idx_start_user_properties){
			lenght_user_properties=0; // invalidate any user property
		}
		else{
			lenght_user_properties=stk_properties->count-idx_start_user_properties;
		}*/

		return true;
	}

	StackElement * ScriptObject::getUserProperty(int v_index){ // return list of stack elements

		// check indexes ...
		if(v_index < 0){
			VM_SET_USER_ERROR(this->virtual_machine,"Negative index std::vector (%i)",v_index);
			return NULL;
		}

		if(v_index >= this->countUserProperties()){
			VM_SET_USER_ERROR(this->virtual_machine,"Index std::vector out of bounds (%i)",v_index);
			return NULL;
		}

		return (StackElement *)this->stk_properties->items[v_index+this->idx_start_user_properties];
	}

	StackElement *ScriptObject::getThisProperty(){
		return &this->stk_this;
	}

	zs_vector * ScriptObject::getAllProperties(){ // return list of stack elements
		return stk_properties;
	}

	bool ScriptObject::eraseProperty(const std::string & property_name, const ScriptFunction *info_function){
		bool exists=false;
		zs_int idx_property = map_property_keys->get(property_name.c_str(),exists);
		if(!exists){
			VM_SET_USER_ERROR(this->virtual_machine,"Property %s not exist",property_name.c_str());
			return false;
		}
		if(!eraseProperty(idx_property)){
			return false;
		}
		map_property_keys->erase(property_name.c_str()); // erase also property key

		return true;
	}

	zs_int ScriptObject::countUserProperties(){

		zs_int count=this->stk_properties->count-this->idx_start_user_properties;
		if(this->script_class->idx_class == IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_STRING){
			return ((std::string *)this->value)->size();
		}
		return count;
	}

	StackElement **ScriptObject::getUserProperties(){
		return (StackElement **)&stk_properties->items[this->idx_start_user_properties];
	}

	ScriptClass * ScriptObject::getScriptClass(){
		return script_class;
	}

	const std::string & ScriptObject::getClassName(){
		return script_class->symbol_class.name;
	}

	const std::string & ScriptObject::getNativePointerClassName(){
		return script_class->str_class_ptr_type;
	}

	std::string ScriptObject::toString(){
		// check whether toString is implemented...
		StackElement *stk_function=getProperty(ByteCodeMetamethodToSymbolStr(BYTE_CODE_METAMETHOD_TO_STRING),NULL);

		if(stk_function != NULL){ // get first element
			if(stk_function->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION){
				ScriptFunction *ptr_function=(ScriptFunction *)stk_function->var_ref;
				if((ptr_function->symbol.properties & SYMBOL_PROPERTY_STATIC) == 0){

					StackElement result=VM_EXECUTE(
							virtual_machine
							,this
							,ptr_function
							,NULL
							,0
					);
					if(result.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){
						ScriptObject *so=(ScriptObjectString *)result.var_ref;
						// capture string...
						std::string aux=so->toString();
						// ... destroy lifetime object we don't need anymore
						virtual_machine->destroyLifetimeObject(so);
						// return
						return aux;
					}
				}
			}
		}

		return "@Class:"+script_class->symbol_class.name;
	}

	bool ScriptObject::initSharedPtr(){
		// is assigned means that when is created is assigned immediately ?

		if(shared_pointer == NULL){
			if((shared_pointer = virtual_machine->newSharedPointer(this))==NULL){
				return false;
			}
		}
		else{
			VM_SET_USER_ERROR(this->virtual_machine," shared ptr already registered");
			return false;
		}

		return true;
	}

	bool ScriptObject::unrefSharedPtr(int _idx_current_call){
		if(shared_pointer!=NULL){
			if(!virtual_machine->unrefSharedScriptObject(shared_pointer,_idx_current_call)){
				return false;
			}
		}
		else{
			VM_SET_USER_ERROR(virtual_machine,"shared ptr not registered");
			return false;
		}
		return true;
	}

	void * ScriptObject::getNativeObject(){
		return c_object;
	}

	bool ScriptObject::isCreatedByContructor(){
		return was_created_by_constructor;
	}

	ScriptClass * ScriptObject::getNativeClass(){
		 return script_class_native;
	}

	bool ScriptObject::isNativeObject(){
		 return ((script_class->symbol_class.properties & SYMBOL_PROPERTY_C_OBJECT_REF) != 0);
	}

	bool ScriptObject::destroy(){

		bool deallocated = false;
		if(created_object != 0){
			if((this->idx_class<IDX_BUILTIN_TYPE_MAX) || delete_c_object){ // only erases pointer if basic type or user/auto delete is required ...
				(*script_class_native->c_destructor)(created_object);
				deallocated=true;
			}
		}

//#ifdef __DEBUG__
		if(!deallocated && was_created_by_constructor){
			fprintf(stderr,"%s"
					,zs_strutils::format("[%s:%i] Allocated C pointer not deallocated"
							,SFI_GET_FILE_LINE(info_function_new, instruction_new)
					).c_str()
				);
			//return false;
		}
//#endif
		// remove vars & fundtions if class is C...
		while ( stk_properties->count!=0){
			if(!eraseProperty(stk_properties->count-1)){
				return false;
			}
		}

		stk_properties->clear();
		return true;
	}

	ScriptObject::~ScriptObject(){
		destroy();
		delete stk_properties;
		delete map_property_keys;
	}
}
