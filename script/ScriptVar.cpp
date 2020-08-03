/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void  writeError(const char *filename, int line, const  char  *string_text, ...);
	void  writeWarning(const char *filename, int line, const  char  *string_text, ...);

	void ScriptVar::createSymbols(ScriptClass *ir_class){


		FunctionSymbol *si;
		StackElement *se;

		// add extra symbol this itself if is a class typedef by user...
		if(registered_class_info->idx_class >=IDX_BUILTIN_TYPE_MAX){
			this_variable.var_ref=this;
			this_variable.properties=MSK_STACK_ELEMENT_PROPERTY_IS_VAR_THIS|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR;
		}

		// Register c vars...
		for ( unsigned i = 0; i < ir_class->symbol_native_variable_members->count; i++){


			Symbol * symbol = (Symbol *)ir_class->symbol_native_variable_members->items[i];

			se=addProperty(symbol->name);

			if(symbol->symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) //if(IS_CLASS_C)
			{ // we know the type object so we assign the pointer ...
				// check if primitive type (only 4 no more no else)...
				void *ptr_variable = (void*) ((unsigned long long) c_object + symbol->ref_ptr);

				*se=convertSymbolToStackElement(this->zs,symbol,ptr_variable);
			}else{
				THROW_RUNTIME_ERROR("symbol should be c var");
			}
		}

		// Register functions...
		for ( unsigned i = 0; i < ir_class->function_members->count; i++){
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
		}
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
		functions=new zs_vector();
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

		setup();

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

		// only create symbols if not std::string type to make it fast ...
		if(idx_class >= IDX_BUILTIN_TYPE_MAX && idx_class != IDX_BUILTIN_TYPE_CLASS_STRING){
			createSymbols(irv);
		}
	}

	ScriptFunction *ScriptVar::getConstructorFunction(){

		if(registered_class_info->idx_function_member_constructor != ZS_IDX_UNDEFINED){
			return (ScriptFunction *)registered_class_info->function_members->items[registered_class_info->idx_function_member_constructor];
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
			//writeError(CURRENT_PARSING_FILENAME,m_line," Symbol name cannot begin with %c", *aux_p);
			//return NULL;
		}

		if(error_symbol){
			writeError(SFI_GET_FILE_LINE(info_function, src_instruction),"invalid symbol name \"%s\". Check it doesn't start with 0-9, it has no spaces, and it has no special chars like :,;,-,(,),[,], etc.",symbol_value.c_str());
			THROW_RUNTIME_ERROR(zs_strutils::format("invalid symbol name \"%s\". Check it doesn't start with 0-9, it has no spaces, and it has no special chars like :,;,-,(,),[,], etc.",symbol_value.c_str()));
			return NULL;
		}

		if(getProperty(symbol_value) != NULL){
			writeError(SFI_GET_FILE_LINE(info_function,src_instruction),"internal:symbol \"%s\" already exists",symbol_value.c_str());
			return NULL;
		}

		if(sv != NULL){
			si = *sv;

			// update n_refs +1
			if(sv->properties&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
				if(!virtual_machine->sharePointer(((ScriptVar *)(sv->var_ref))->ptr_shared_pointer_node)){
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

		//si.idxAstNode = _idxAstNode;
		std::string key_value = symbol_value;
		StackElement *new_stk=(StackElement *)malloc(sizeof(StackElement));
		*new_stk=si; //assign var

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

	StackElement * ScriptVar::getProperty(const std::string & property_name){//,bool only_var_name){

		if(property_name == "this"){
			return &this_variable;
		}

		intptr_t idx_property=this->properties_keys->get(property_name.c_str());
		return (StackElement *)this->stk_properties->items[idx_property];
		/*for(unsigned int i = 0; i < this->properties_keys->count; i++){
			if(strcmp(property_name.c_str(),(const char *)this->properties_keys->items[i])==0){
				return (StackElement *)stk_properties->items[i];
			}
		}
		return NULL;*/
	}

	FunctionSymbol *ScriptVar::addFunction(const std::string & symbol_value,const ScriptFunction *irv, bool ignore_duplicates){
		FunctionSymbol *si=new FunctionSymbol();
		si->proxy_ptr=0;
		si->object = {
			(void *)irv,				// function struct pointer.
			NULL,						// no var ref releated.
			MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION  // dfine as function.
		};

		if(!ignore_duplicates){
			if(getFunction(symbol_value) != NULL){
				writeError(SFI_GET_FILE_LINE(irv,NULL), "internal:symbol already exists");
				return NULL;
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
	}


	bool ScriptVar::eraseProperty(short idx, bool remove_vector){//onst std::string & varname){

		StackElement *si;


		if(idx >= stk_properties->count){
			writeError(NULL,0,"idx out of bounds (%i>=%i)",idx,stk_properties->count);
			return false;
		}

		si=(StackElement *)stk_properties->items[idx];
		unsigned short var_type = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(si->properties);

		switch(var_type){

			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
				break;
			default: // properties ...

				if(var_type & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
					if(((si->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C) != MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C)
						&& ((si->properties & MSK_STACK_ELEMENT_PROPERTY_IS_VAR_THIS) != MSK_STACK_ELEMENT_PROPERTY_IS_VAR_THIS)){ // deallocate but not if is c or this ref
						if(si->var_ref != NULL){
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

		return true;
	}

	zs_vector * ScriptVar::getProperties(){ // return list of stack elements
		return stk_properties;
	}

	bool ScriptVar::eraseProperty(const std::string & property_name, const ScriptFunction *info_function){
		try{
			intptr_t idx_property = properties_keys->get(property_name.c_str());
		 	 eraseProperty(idx_property,true);
		 	 properties_keys->erase(property_name.c_str()); // erase also property key
		 	 return true;

		}catch(std::exception & ex){
			writeError(SFI_GET_FILE_LINE(info_function,NULL),"%s",ex.what());

		}

		return false;
		/*for(unsigned int i = 0; i < this->properties_keys->count; i++){
			if(strcmp(property_name.c_str(),(const char *)this->properties_keys->items[i])==0){
				return eraseProperty(i,true);
			}
		}*/
		//writeError(SFI_GET_FILE_LINE(info_function,NULL),"symbol %s doesn't exist",varname.c_str());
		//return false;
	}

	StackElement * ScriptVar::getProperty(short idx){

		if(idx==ZS_IDX_SYMBOL_THIS){
			return &this_variable;
		}

		if(idx >= (int)stk_properties->count){
			writeError("unknow",-1,"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}

		return (StackElement *)stk_properties->items[idx];
	}

	/*FunctionSymbol * ScriptVar::getIdxScriptFunctionObjectByClassFunctionName(const std::string & varname){

		// from lat value to first to get last override function...
		for(unsigned i = this->functions->count-1; i >= 0; i--){
			FunctionSymbol *si=(FunctionSymbol *)this->function_symbols->items[i];
			if(varname == si->key_value){
				return si;
			}
		}
		return NULL;
	}*/

	const std::string & ScriptVar::getClassName(){
		return registered_class_info->symbol.name;
	}

	const std::string & ScriptVar::getPointer_C_ClassName(){
		return registered_class_info->str_class_ptr_type;
	}

	std::string * ScriptVar::toString(){
		return &str_value;
	}

	bool ScriptVar::initSharedPtr(bool is_assigned){

		if(ptr_shared_pointer_node == NULL){
			ptr_shared_pointer_node = virtual_machine->newSharedPointer(this);

			if(is_assigned){ // increment number shared pointers...
				if(!virtual_machine->sharePointer(ptr_shared_pointer_node)){
					return NULL;
				}
			}
			return true;
		}

		writeError("unknow",-1," shared ptr alrady registered");
		return false;
	}

	bool ScriptVar::unrefSharedPtr(){
		if(ptr_shared_pointer_node!=NULL){

			virtual_machine->unrefSharedScriptVar(ptr_shared_pointer_node);
			return true;
		}
		else{
			writeError("unknow",-1,"shared ptr not registered");
		}

		return false;
	}

	FunctionSymbol *ScriptVar::getFunction(unsigned int idx){
		if(idx >= functions->count){
			writeError("unknow",-1,"idx symbol index out of bounds");
			return NULL;
		}
		return (FunctionSymbol *)functions->items[idx];
	}

	zs_vector * ScriptVar::getFunctions(){ // return list of functions
		return functions;
	}

	void * ScriptVar::Get_C_Object(){
		return c_object;
	}

	bool ScriptVar::isCreatedByContructor(){
		return was_created_by_constructor;
	}

	ScriptClass * ScriptVar::Get_C_Class(){

		 return c_scriptclass_info;
	}

	bool ScriptVar::Is_C_Object(){
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
		for ( unsigned i = 0; i < functions->count; i++){
			FunctionSymbol *si = (FunctionSymbol *)functions->items[i];
			ScriptFunction * ir_fun  = (ScriptFunction *)(si->object.stk_value);
			 if((ir_fun->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) == SYMBOL_PROPERTY_C_OBJECT_REF){ // create proxy function ...
				 if((ir_fun->symbol.symbol_properties & SYMBOL_PROPERTY_STATIC_REF) != SYMBOL_PROPERTY_STATIC_REF){
					 delete ((CFunctionMemberPointer *)si->proxy_ptr);
				 }
			}
		}
	}

	ScriptVar::~ScriptVar(){

		destroy();

		// delete all free items and clear();
		stk_properties->free_all_items_and_clear();
		delete stk_properties;

		// delete search support...
		delete properties_keys;


		for(unsigned i=0; i < functions->count;i++){
			delete ((FunctionSymbol *)functions->items[i]);
		}
		functions->clear();
	}
}
