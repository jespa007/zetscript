/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void ScriptObject::createSymbols(ScriptClass *ir_class){
		//FunctionSymbol *si;
		StackElement *se;

		memset(&this_variable,0,sizeof(this_variable));

		//this_variable.properties|=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED;

		// add extra symbol this itself if is a class typedef by user...
		//if(registered_class_info->idx_class >=IDX_BUILTIN_TYPE_MAX){ // put as read only and cannot assign
		this_variable.var_ref=this;
		this_variable.properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_OBJECT;
		//}

		// Register c vars...
		for ( unsigned i = 0; i < ir_class->symbol_members->count; i++){

			Symbol * symbol = (Symbol *)ir_class->symbol_members->items[i];
			bool is_script_function=symbol->properties & SYMBOL_PROPERTY_IS_FUNCTION;
			bool ignore_duplicates=is_script_function==false; // we ignore duplicates in case of script function, to allow super operation work.

			// we add symbol as property. In it will have the same idx as when were evaluated declared symbols on each class
			se=addPropertyBuiltIn(
				symbol->name
			);

			if(symbol->properties & SYMBOL_PROPERTY_IS_FUNCTION){

				ScriptFunction * ir_fun  = (ScriptFunction *)symbol->ref_ptr;
				se->stk_value=this;
				se->var_ref=ir_fun;
				se->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION; // tell stack element that is a function
			}
			else{ // var... should be native in principle ?

				if(symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF) //if(IS_CLASS_C)
				{
					// we know the type object so we assign the pointer ...
					void *ptr_variable=(void *)((intptr_t)this->c_object + symbol->ref_ptr);

					*se=convertSymbolToStackElement(this->zs,symbol,ptr_variable);
				}else{
					THROW_RUNTIME_ERROR("symbol should be c var");
				}
			}
		}

		// register custom built-in vars
		se=addPropertyBuiltIn(
			"length"
		);

		se->var_ref=&lenght_user_properties;
		se->properties=(MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C|MSK_STACK_ELEMENT_PROPERTY_READ_ONLY);

		// start property idx starts  from last built-in property...
		idx_start_user_properties=stk_properties->count;
		lenght_user_properties=0;
	}

	void ScriptObject::setup(){

		ptr_shared_pointer_node = NULL;

		registered_class_info = NULL;
		c_object = NULL;
		created_object = NULL;
		value = NULL;
		was_created_by_constructor=false;
		c_scriptclass_info=NULL;
		idx_class = ZS_INVALID_CLASS;
		aux_string ="";
		delete_c_object = false; // --> user is responsible to delete C objects!
		info_function_new=NULL;
		instruction_new=NULL;
		memset(&this_variable,0,sizeof(this_variable));
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

		this->registered_class_info = irv;
		idx_class = irv->idx_class;
		c_object = _c_object;
		
		c_scriptclass_info=NULL;

		if(c_object == NULL){ // if object == NULL, the script takes the control. Initialize c_class (c_scriptclass_info) to get needed info to destroy create the C++ object.
			if(registered_class_info->isNativeClass()){
				c_scriptclass_info=registered_class_info;
				created_object = (*registered_class_info->c_constructor)();
				was_created_by_constructor=true;
				c_object = created_object;
			}else {
				ScriptClass *sc=registered_class_info;
				// get first class with c inheritance...

				while((sc->idx_base_classes->count>0) && (c_scriptclass_info==NULL)){
					sc=GET_SCRIPT_CLASS(this,sc->idx_base_classes->items[0]); // get base class (only first in script because has single inheritance)...
					if(sc->isNativeClass()){ // we found the native script class!
						c_scriptclass_info=sc;
						created_object = (*sc->c_constructor)();
						was_created_by_constructor=true;
						c_object = created_object;
					}
				}

			}

		}else{ // pass the pointer reference but in principle is cannot be deleted when the scriptvar is deleted...
			c_scriptclass_info=irv;
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

		if(registered_class_info->idx_function_member_constructor != ZS_IDX_UNDEFINED){
			return (ScriptFunction *)registered_class_info->symbol_members->items[registered_class_info->idx_function_member_constructor];
		}

		return NULL;
	}

	bool ScriptObject::setIdxClass(unsigned char idx){
		ScriptClass *_info_registered_class =  GET_SCRIPT_CLASS(this,idx);//ScriptClass::getInstance()->getRegisteredClassByIdx(idx);

		if(_info_registered_class == NULL){
			return false;
		}

		registered_class_info = _info_registered_class;
		return true;
	}

	bool ScriptObject::itHasSetMetamethod(){
		return registered_class_info->metamethod_operator[BYTE_CODE_METAMETHOD_SET]->count > 0;
	}

	void ScriptObject::setDelete_C_ObjectOnDestroy(bool _delete_on_destroy){
		created_object=NULL;
		if((this->delete_c_object = _delete_on_destroy)==true){
			created_object=c_object;
		}
	}

	StackElement *ScriptObject::newSlot(){
		StackElement *stk=(StackElement *)malloc(sizeof(StackElement));
		*stk={NULL,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};
		stk_properties->push_back((intptr_t)stk);
		lenght_user_properties=(int)stk_properties->count-idx_start_user_properties;
		return stk;
	}

	StackElement *ScriptObject::popUserProperty(){
		if(stk_properties->count<=idx_start_user_properties){
			THROW_RUNTIME_ERROR("pop(): error stack already empty");
		}

		StackElement *stk_element=((StackElement *)stk_properties->items[stk_properties->count-1]);
		eraseProperty(stk_properties->count-1);
		return stk_element;
	}

	// built-in only for initialized
	StackElement * ScriptObject::addPropertyBuiltIn(const std::string & symbol_value){
		std::string key_value = symbol_value;

		if(idx_start_user_properties != 0){
			THROW_RUNTIME_ERROR("internal error: addPropertyBuiltIn should be used within ScriptObject::createSymbols");
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
			, const ScriptFunction *info_function
			,Instruction *src_instruction
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
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(info_function, src_instruction),"invalid symbol name \"%s\". Check it doesn't start with 0-9, it has no spaces, and it has no special chars like :,;,-,(,),[,], etc.",symbol_value.c_str());
		}

		if(map_property_keys->exist(symbol_value.c_str())){
			THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(info_function, src_instruction),"\"%s\" symbol already exists",symbol_value.c_str());
		}

		if(sv != NULL){
			si = *sv;

			// update n_refs +1
			if(sv->properties&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_OBJECT){
				virtual_machine->sharePointer(((ScriptObject *)(sv->var_ref))->ptr_shared_pointer_node);
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

	StackElement * ScriptObject::getProperty(const std::string & property_name, int * idx){//,bool only_var_name){

		bool exists;
		intptr_t idx_stk_element=this->map_property_keys->get(property_name.c_str(),exists);
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
			return &this_variable;
		}

		if(idx >= (int)stk_properties->count){
			THROW_RUNTIME_ERROR("idx symbol index out of bounds (%i)",idx);
		}

		return (StackElement *)stk_properties->items[idx];
	}

	void ScriptObject::eraseProperty(short idx){//onst std::string & varname){

		StackElement *si;
		ScriptFunction * ir_fun;

		if(idx >= stk_properties->count){
			THROW_RUNTIME_ERROR("idx out of bounds (%i>=%i)",idx,stk_properties->count);
		}

		si=(StackElement *)stk_properties->items[idx];
		unsigned short var_type = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(si->properties);

		switch(var_type){

			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION:

				 ir_fun  = (ScriptFunction *)(si->var_ref);
				 /*if((ir_fun->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF){ // create proxy function ...
					 if((ir_fun->symbol.properties & SYMBOL_PROPERTY_C_STATIC_REF) != SYMBOL_PROPERTY_C_STATIC_REF){ // delete function pointer
						 StackElement *stk_element=(StackElement *)si->stk_value;
						 delete ((CMemberFunctionPointer *)stk_element->stk_value);
						 free(si->stk_value);
					 }
				}*/
				break;
			default: // properties ...

				if(var_type & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_OBJECT){
					if(((si->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C) != MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C)
						&& (si->var_ref != this) // ensure that property don't holds its same var.
						&& (si->var_ref != 0)
					  ){ // deallocate but not if is c or this ref

						// remove property if not referenced anymore
						virtual_machine->unrefSharedScriptObject(((ScriptObject *)(si->var_ref))->ptr_shared_pointer_node,true);

					}
				}
				break;
		}

		// remove symbol on std::vector ...
		free((void *)stk_properties->items[idx]);
		stk_properties->erase(idx);

		if(stk_properties->count<idx_start_user_properties){
			lenght_user_properties=0; // invalidate any user property
		}
		else{
			lenght_user_properties=stk_properties->count-idx_start_user_properties;
		}
	}

	StackElement * ScriptObject::getUserProperty(int v_index){ // return list of stack elements

		// check indexes ...
		if(v_index < 0){
			THROW_RUNTIME_ERROR("Negative index std::vector (%i)",v_index);
		}

		if(v_index >= this->lenght_user_properties){
			THROW_RUNTIME_ERROR("Index std::vector out of bounds (%i)",v_index);
		}

		return (StackElement *)this->stk_properties->items[v_index+this->idx_start_user_properties];
	}

	StackElement *ScriptObject::getThisProperty(){
		return &this->this_variable;
	}

	zs_vector * ScriptObject::getAllProperties(){ // return list of stack elements
		return stk_properties;
	}

	void ScriptObject::eraseProperty(const std::string & property_name, const ScriptFunction *info_function){
		bool exists=false;
		intptr_t idx_property = map_property_keys->get(property_name.c_str(),exists);
		if(!exists){
			THROW_RUNTIME_ERROR("Property %s not exist",property_name.c_str());
		}
		eraseProperty(idx_property);
		map_property_keys->erase(property_name.c_str()); // erase also property key

	}

	ScriptClass * ScriptObject::getScriptClass(){
		return registered_class_info;
	}

	const std::string & ScriptObject::getClassName(){
		return registered_class_info->symbol.name;
	}

	const std::string & ScriptObject::getNativePointerClassName(){
		return registered_class_info->str_class_ptr_type;
	}

	std::string ScriptObject::toString(){
		return "@Class:"+registered_class_info->symbol.name;
	}

	void ScriptObject::initSharedPtr(){
		// is assigned means that when is created is assigned immediately ?

		if(ptr_shared_pointer_node == NULL){
			ptr_shared_pointer_node = virtual_machine->newSharedPointer(this);
		}
		else{
			THROW_RUNTIME_ERROR(" shared ptr already registered");
		}
	}

	void ScriptObject::unrefSharedPtr(){
		if(ptr_shared_pointer_node!=NULL){
			virtual_machine->unrefSharedScriptObject(ptr_shared_pointer_node);
		}
		else{
			THROW_RUNTIME_ERROR("shared ptr not registered");
		}
	}

	void * ScriptObject::getNativeObject(){
		return c_object;
	}

	bool ScriptObject::isCreatedByContructor(){
		return was_created_by_constructor;
	}

	ScriptClass * ScriptObject::getNativeClass(){
		 return c_scriptclass_info;
	}

	bool ScriptObject::isNativeObject(){
		 return ((registered_class_info->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) != 0);
	}

	void ScriptObject::destroy(){

		bool deallocated = false;
		if(created_object != 0){
			if((this->idx_class<IDX_BUILTIN_TYPE_MAX) || delete_c_object){ // only erases pointer if basic type or user/auto delete is required ...
				(*c_scriptclass_info->c_destructor)(created_object);
				deallocated=true;
			}
		}

//#ifdef __DEBUG__
		if(!deallocated && was_created_by_constructor){
			printf("[%s:%i] Allocated C pointer not deallocated\n",SFI_GET_FILE_LINE(info_function_new, instruction_new));
		}
//#endif
		// remove vars & fundtions if class is C...
		while ( stk_properties->count!=0){
			eraseProperty(stk_properties->count-1);
		}

		stk_properties->clear();
	}

	ScriptObject::~ScriptObject(){
		destroy();
		delete stk_properties;
		delete map_property_keys;
	}
}
