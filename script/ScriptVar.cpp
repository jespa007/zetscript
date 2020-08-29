/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void ScriptVar::createSymbols(ScriptClass *ir_class){


		//FunctionSymbol *si;
		StackElement *se;

		// add extra symbol this itself if is a class typedef by user...
		if(registered_class_info->idx_class >=IDX_BUILTIN_TYPE_MAX){
			this_variable.var_ref=this;
			this_variable.properties=MSK_STACK_ELEMENT_PROPERTY_IS_VAR_THIS|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR;
		}

		// Register c vars...
		for ( unsigned i = 0; i < ir_class->symbol_members->count; i++){

			Symbol * symbol = (Symbol *)ir_class->symbol_members->items[i];
			bool is_script_function=symbol->symbol_properties & SYMBOL_PROPERTY_IS_SCRIPT_FUNCTION;
			bool ignore_duplicates=is_script_function==false; // we ignore duplicates in case of script function, to allow super operation work.

			// we add symbol as property. In it will have the same idx as when were evaluated declared symbols on each class
			se=addPropertyBuiltIn(
					symbol->name
				);

			if(symbol->symbol_properties & SYMBOL_PROPERTY_IS_SCRIPT_FUNCTION){

				ScriptFunction * ir_fun  = (ScriptFunction *)symbol->ref_ptr;
				se->stk_value=ir_fun;
				se->properties=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION; // tell stack element that is a function


				 if((ir_fun->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF){ // create proxy function ...
					 // static ref only get ref function ...
					 if((ir_fun->symbol.symbol_properties & SYMBOL_PROPERTY_STATIC_REF) == SYMBOL_PROPERTY_STATIC_REF){
						 se->var_ref = (void *)ir_fun->ref_native_function_ptr;
					 }
					 else{ // hard way: create function member ptr through proxy
						 se->var_ref = (*((std::function<void *(void *)> *)ir_fun->ref_native_function_ptr))(c_object);
					 }
				}
			}
			else{ // var... should be native in principle ?

				if(symbol->symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) //if(IS_CLASS_C)
				{ // we know the type object so we assign the pointer ...
					// check if primitive type (only 4 no more no else)...
					void *ptr_variable = (void*) ((unsigned long long) c_object + symbol->ref_ptr);

					*se=convertSymbolToStackElement(this->zs,symbol,ptr_variable);
				}else{
					THROW_RUNTIME_ERROR("symbol should be c var");
				}
			}
		}

		// Register functions...
		/*for ( unsigned i = 0; i < ir_class->function_members->count; i++){
			ScriptFunction * ir_fun  = (ScriptFunction *)ir_class->function_members->items[i];
			 si =addFunction(
					  ir_fun->symbol.name,

					ir_fun

					);
			 if((ir_fun->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF){ // create proxy function ...
				 // static ref only get ref function ...
				 if((ir_fun->symbol.symbol_properties & SYMBOL_PROPERTY_STATIC_REF) == SYMBOL_PROPERTY_STATIC_REF){
					 si->proxy_ptr = ir_fun->symbol.ref_ptr;
				 }
				 else{ // create function member
					 si->proxy_ptr = (intptr_t)(*((std::function<void *(void *)> *)ir_fun->symbol.ref_ptr))(c_object);
				 }
			}
		}*/
	}

	void ScriptVar::setup(){

		ptr_shared_pointer_node = NULL;

		registered_class_info = NULL;
		c_object = NULL;
		created_object = NULL;
		value = NULL;
		was_created_by_constructor=false;
		c_scriptclass_info=NULL;
		idx_class = -1;
		aux_string ="";
		delete_c_object = false; // --> user is responsible to delete C objects!
		info_function_new=NULL;
		instruction_new=NULL;
		memset(&this_variable,0,sizeof(this_variable));
		stk_properties=new zs_vector();
		properties_keys=new zs_map();

		stk_properties_built_in=new zs_vector();
		properties_keys_built_in=new zs_map();


	}

	ScriptVar::ScriptVar(){
		setup();
	}

	ScriptVar::ScriptVar(ZetScript	*_zs){
		setup();

		zs=_zs;
		virtual_machine = zs->getVirtualMachine();
		script_class_factory = zs->getScriptClassFactory();
	}

	void ScriptVar::init(ScriptClass *irv, void * _c_object){

		this->registered_class_info = irv;
		idx_class = irv->idx_class;
		c_object = _c_object;
		
		c_scriptclass_info=NULL;

		if(c_object == NULL){ // if object == NULL, the script takes the control. Initialize c_class (c_scriptclass_info) to get needed info to destroy create the C++ object.
			if(registered_class_info->is_C_Class()){
				c_scriptclass_info=registered_class_info;
				created_object = (*registered_class_info->c_constructor)();
				was_created_by_constructor=true;
				c_object = created_object;
			}else {
				ScriptClass *sc=registered_class_info;
				while( sc->idx_base_classes->count>0 && c_scriptclass_info==NULL){
					sc=GET_SCRIPT_CLASS(this,sc->idx_base_classes->items[0]); // get base class...
					if(sc->is_C_Class()){
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
			|| idx_class != IDX_BUILTIN_TYPE_CLASS_STRING
			|| idx_class != IDX_BUILTIN_TYPE_CLASS_VECTOR){
			createSymbols(irv);
		}
	}

	ScriptFunction *ScriptVar::getConstructorFunction(){

		if(registered_class_info->idx_function_member_constructor != ZS_IDX_UNDEFINED){
			return (ScriptFunction *)registered_class_info->symbol_members->items[registered_class_info->idx_function_member_constructor];
		}

		return NULL;
	}

	bool ScriptVar::setIdxClass(unsigned char idx){
		ScriptClass *_info_registered_class =  GET_SCRIPT_CLASS(this,idx);//ScriptClass::getInstance()->getRegisteredClassByIdx(idx);

		if(_info_registered_class == NULL){
			return false;
		}

		registered_class_info = _info_registered_class;
		return true;
	}

	bool ScriptVar::itHasSetMetamethod(){
		return registered_class_info->metamethod_operator[BYTE_CODE_METAMETHOD_SET]->count > 0;

	}

	void ScriptVar::setDelete_C_ObjectOnDestroy(bool _delete_on_destroy){
		created_object=NULL;
		if((this->delete_c_object = _delete_on_destroy)==true){
			created_object=c_object;
		}
	}

	StackElement * ScriptVar::addPropertyBuiltIn(const std::string & symbol_value){
		std::string key_value = symbol_value;
		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		*new_stk={
				0,
				0,
				MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED
			}; //assign var

		// if ignore duplicate was true, map resets idx to the last function...
		properties_keys_built_in->set(key_value.c_str(),(intptr_t)stk_properties->count);

		stk_properties_built_in->push_back((intptr_t)new_stk);

  	    return new_stk;
	}

	StackElement * ScriptVar::addProperty(
			const std::string & symbol_value
			, const ScriptFunction *info_function
			,Instruction *src_instruction
			,StackElement * sv
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

		if(sv != NULL){
			si = *sv;

			// update n_refs +1
			if(sv->properties&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
				virtual_machine->sharePointer(((ScriptVar *)(sv->var_ref))->ptr_shared_pointer_node);
			}

		}else{

			si={
				0,
				0,
				MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED
			};
		}

		//si.idxAstNode = _idxAstNode;
		std::string key_value = symbol_value;
		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		*new_stk=si; //assign var

		// if ignore duplicate was true, map resets idx to the last function...
		properties_keys->set(key_value.c_str(),(intptr_t)stk_properties->count);

		stk_properties->push_back((intptr_t)new_stk);


		return new_stk;
	}

	/*StackElement * ScriptVar::PropertyExist(const char *c){
		for(unsigned i = 0; i < properties_keys.size(); i++){
			//ScriptVar *var = (ScriptVar *)m_variableSymbol[i].object.var_ref;
			if(properties_keys[i] == std::string(c)){
				return &properties[i];
			}
		}
		return NULL;
	}*/

	StackElement * ScriptVar::getPropertyBuiltIn(const std::string & property_name){
		bool exists;
		if(property_name == "this"){
			return &this_variable;
		}


		intptr_t idx_property=this->properties_keys_built_in->get(property_name.c_str(),exists);
		if(exists){
			return (StackElement *)this->stk_properties_built_in->items[idx_property];
		}

		return NULL;
	}

	StackElement * ScriptVar::getProperty(const std::string & property_name){//,bool only_var_name){

		bool exists;


		intptr_t idx_property=this->properties_keys->get(property_name.c_str(),exists);
		if(exists){
			return (StackElement *)this->stk_properties->items[idx_property];
		}

		return NULL;
		/*for(unsigned int i = 0; i < this->properties_keys->count; i++){
			if(strcmp(property_name.c_str(),(const char *)this->properties_keys->items[i])==0){
				return (StackElement *)stk_properties->items[i];
			}
		}
		return NULL;*/
	}

	/*FunctionSymbol *ScriptVar::addFunction(const std::string & symbol_value,const ScriptFunction *irv, bool ignore_duplicates){
		FunctionSymbol *si=new FunctionSymbol();
		si->proxy_ptr=0;
		si->object = {
			(void *)irv,				// function struct pointer.
			NULL,						// no var ref releated.
			MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION  // dfine as function.
		};

		if(!ignore_duplicates){
			if(getFunction(symbol_value) != NULL){
				THROW_SCRIPT_ERROR(SFI_GET_FILE_LINE(irv,NULL), "internal:symbol already exists");
			}
		}

		// get super function ...
		si->key_value=symbol_value;
		//si.idxAstNode = _idxAstNode;
		functions->push_back((intptr_t)si);

		return si;
	}

	FunctionSymbol * ScriptVar::getFunction(const std::string & varname){
		for(unsigned int i = 0; i < this->functions->count; i++){

			FunctionSymbol *fs=(FunctionSymbol *)this->functions->items[i];
			std::string symbol = fs->key_value;

			if(varname == symbol){
				return (FunctionSymbol * )functions->items[i];
			}
		}
		return NULL;
	}*/


	void ScriptVar::eraseProperty(short idx, bool remove_vector){//onst std::string & varname){

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

				 ir_fun  = (ScriptFunction *)(si->stk_value);
				 if((ir_fun->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF){ // create proxy function ...
					 if((ir_fun->symbol.symbol_properties & SYMBOL_PROPERTY_STATIC_REF) != SYMBOL_PROPERTY_STATIC_REF){
						 delete ((CFunctionMemberPointer *)si->var_ref);
					 }
				}
				break;
			default: // properties ...

				if(var_type & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
					if(((si->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C) != MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C)
						&& ((si->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_THIS) != MSK_STACK_ELEMENT_PROPERTY_IS_VAR_THIS)){ // deallocate but not if is c or this ref
						if(si->var_ref != 0){
							// remove property if not referenced anymore
							virtual_machine->unrefSharedScriptVar(((ScriptVar *)(si->var_ref))->ptr_shared_pointer_node,true);
						}
					}
				}
				break;
		}

		// remove symbol on std::vector ...
		if(remove_vector){
			stk_properties->erase(idx);
		}

	}

	zs_vector * ScriptVar::getProperties(){ // return list of stack elements
		return stk_properties;
	}

	void ScriptVar::eraseProperty(const std::string & property_name, const ScriptFunction *info_function){
		//try{
		bool exists=false;
		intptr_t idx_property = properties_keys->get(property_name.c_str(),exists);
		if(!exists){
			THROW_RUNTIME_ERROR("Property %s not exist",property_name.c_str());
		}
		eraseProperty(idx_property,true);
		properties_keys->erase(property_name.c_str()); // erase also property key

		 	// return true;

		/*}catch(std::exception & ex){
			writeError(SFI_GET_FILE_LINE(info_function,NULL),"%s",ex.what());

		}*/

		//return false;
		/*for(unsigned int i = 0; i < this->properties_keys->count; i++){
			if(strcmp(property_name.c_str(),(const char *)this->properties_keys->items[i])==0){
				return eraseProperty(i,true);
			}
		}*/
		//writeError(SFI_GET_FILE_LINE(info_function,NULL),"symbol %s doesn't exist",varname.c_str());
		//return false;
	}

	StackElement * ScriptVar::getPropertyBuiltIn(short idx){

		if(idx==ZS_IDX_SYMBOL_THIS){
			return &this_variable;
		}

		if(idx >= (int)stk_properties_built_in->count){
			THROW_RUNTIME_ERROR("idx symbol index out of bounds (%i)",idx);
		}

		return (StackElement *)stk_properties_built_in->items[idx];
	}

	StackElement * ScriptVar::getProperty(short idx){

		if(idx==ZS_IDX_SYMBOL_THIS){
			return &this_variable;
		}

		if(idx >= (int)stk_properties->count){
			THROW_RUNTIME_ERROR("idx symbol index out of bounds (%i)",idx);
		}

		return (StackElement *)stk_properties->items[idx];
	}

	const std::string & ScriptVar::getClassName(){
		return registered_class_info->symbol.name;
	}

	const std::string & ScriptVar::getNativePointerClassName(){
		return registered_class_info->str_class_ptr_type;
	}

	std::string * ScriptVar::toString(){
		return &str_value;
	}

	void ScriptVar::initSharedPtr(bool is_assigned){

		if(ptr_shared_pointer_node == NULL){
			ptr_shared_pointer_node = virtual_machine->newSharedPointer(this);
			if(is_assigned){ // increment number shared pointers...
				virtual_machine->sharePointer(ptr_shared_pointer_node);
			}
		}
		else{
			THROW_RUNTIME_ERROR(" shared ptr alrady registered");
		}
	}

	void ScriptVar::unrefSharedPtr(){
		if(ptr_shared_pointer_node!=NULL){
			virtual_machine->unrefSharedScriptVar(ptr_shared_pointer_node);
		}
		else{
			THROW_RUNTIME_ERROR("shared ptr not registered");
		}
	}

	/*FunctionSymbol *ScriptVar::getFunction(unsigned int idx){
		if(idx >= functions->count){
			THROW_RUNTIME_ERROR("idx symbol index out of bounds");
		}
		return (FunctionSymbol *)functions->items[idx];
	}

	zs_vector * ScriptVar::getFunctions(){ // return list of functions
		return functions;
	}*/

	void * ScriptVar::getNativeObject(){
		return c_object;
	}

	bool ScriptVar::isCreatedByContructor(){
		return was_created_by_constructor;
	}

	ScriptClass * ScriptVar::getNativeClass(){

		 return c_scriptclass_info;
	}

	bool ScriptVar::isNativeObject(){
		 return ((registered_class_info->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) != 0);
	}

	void ScriptVar::destroy(){
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


		for ( unsigned i = 0; i < stk_properties->count; i++){
			eraseProperty(i);
		}
		stk_properties->clear();

		// deallocate function member ...
		/*for ( unsigned i = 0; i < functions->count; i++){
			FunctionSymbol *si = (FunctionSymbol *)functions->items[i];
			ScriptFunction * ir_fun  = (ScriptFunction *)(si->object.stk_value);
			 if((ir_fun->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF){ // create proxy function ...
				 if((ir_fun->symbol.symbol_properties & SYMBOL_PROPERTY_STATIC_REF) != SYMBOL_PROPERTY_STATIC_REF){
					 delete ((CFunctionMemberPointer *)si->proxy_ptr);
				 }
			}
		}*/
	}

	ScriptVar::~ScriptVar(){

		destroy();

		//-----------------------------------
		// BUILT-IN destroy built-in elements...
		for(unsigned i =0;i < stk_properties_built_in->count; i++){
			free((void *)stk_properties_built_in->items[i]);
		}

		delete stk_properties_built_in;
		delete properties_keys_built_in;
		//-----------------------------------
		// USER delete all free items and clear();
		for(unsigned i =0;i < stk_properties->count; i++){
			free((void *)stk_properties->items[i]);
		}

		delete stk_properties;
		delete properties_keys;
		//-----------------------------------


		/*for(unsigned i=0; i < functions->count;i++){
			delete ((FunctionSymbol *)functions->items[i]);
		}
		functions->clear();
		delete functions;*/
	}
}
