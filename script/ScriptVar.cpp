/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void  writeError(const char *filename, int line, const  char  *string_text, ...);

	void ScriptVar::CreateSymbols(ScriptClass *ir_class){


		FunctionSymbol *si;
		StackElement *se;

		// add extra symbol this itself if is a class typedef by user...
		if(registered_class_info->idx_class >=IDX_BUILTIN_TYPE_MAX){
			this_variable.var_ref=this;
			this_variable.properties=MSK_STACK_ELEMENT_PROPERTY_IS_VAR_THIS|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR;
		}

		// Register variables...
		for ( unsigned i = 0; i < ir_class->local_variable.size(); i++){


			SymbolInfo * ir_var = &ir_class->local_variable[i];

			se=addVariableSymbol(ir_var->symbol->name);

			if(ir_var->symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) //if(IS_CLASS_C)
			{ // we know the type object so we assign the pointer ...
				// check if primitive type (only 4 no more no else)...
				void *ptr_variable = (void*) ((unsigned long long) c_object + ir_var->ref_ptr);

				*se=symbolInfoToStackElement(this->zs,ir_var,ptr_variable);
			}
		}

		// Register functions...
		for ( unsigned i = 0; i < ir_class->local_function.size(); i++){
			ScriptFunction * ir_fun  = ir_class->local_function[i];
			 si =addFunctionSymbol(
					  ir_fun->symbol_info.symbol->name,

					ir_fun

					);
			 if((ir_fun->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY_C_OBJECT_REF){ // create proxy function ...
				 // static ref only get ref function ...
				 if((ir_fun->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_STATIC_REF) == SYMBOL_INFO_PROPERTY_STATIC_REF){
					 si->proxy_ptr = ir_fun->symbol_info.ref_ptr;
				 }
				 else{ // create function member
					 si->proxy_ptr = (intptr_t)(*((std::function<void *(void *)> *)ir_fun->symbol_info.ref_ptr))(c_object);
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

	void ScriptVar::Init(ScriptClass *irv, void * _c_object){

		setup();

		this->registered_class_info = irv;
		idx_class = irv->idx_class;
		c_object = _c_object;
		
		c_scriptclass_info=NULL;

		if(c_object == NULL){ // if object == NULL, the script takes the control. Initialize c_class (c_scriptclass_info) to get needed info to destroy create the C++ object.

			if(registered_class_info->isToClass()){
					c_scriptclass_info=registered_class_info;
					created_object = (*registered_class_info->c_constructor)();
					was_created_by_constructor=true;
					c_object = created_object;
			}else {

				ScriptClass *sc=registered_class_info;
				while( sc->idx_base_class.size()>0 && c_scriptclass_info==NULL){

					sc=GET_SCRIPT_CLASS(this,sc->idx_base_class[0]); // get base class...
					if(sc->isToClass()){
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
			CreateSymbols(irv);
		}
	}

	ScriptFunction *ScriptVar::getConstructorFunction(){

		if(registered_class_info->idx_function_member_constructor != ZS_IDX_UNDEFINED){
			return registered_class_info->local_function[registered_class_info->idx_function_member_constructor];
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
		return registered_class_info->metamethod_operator[BYTE_CODE_METAMETHOD_SET].size() > 0;

	}

	void ScriptVar::setDelete_C_ObjectOnDestroy(bool _delete_on_destroy){
		created_object=NULL;
		if((this->delete_c_object = _delete_on_destroy)==true){
			created_object=c_object;
		}
	}

	StackElement * ScriptVar::addVariableSymbol(const std::string & symbol_value, const ScriptFunction *info_function,Instruction *src_instruction,StackElement * sv){
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

		//std::string symbol_ref=ScriptEval::makeSymbolRef(symbol_value,IDX_ANONYMOUSE_SCOPE);

		if(getVariableSymbol(symbol_value) != NULL){
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
		variable.push_back(si);
		variable_key.push_back(key_value);

		return &variable[variable.size()-1];
	}

	/*StackElement * ScriptVar::PropertyExist(const char *c){
		for(unsigned i = 0; i < variable_key.size(); i++){
			//ScriptVar *var = (ScriptVar *)m_variableSymbol[i].object.var_ref;
			if(variable_key[i] == std::string(c)){
				return &variable[i];
			}
		}
		return NULL;
	}*/

	StackElement * ScriptVar::getVariableSymbol(const std::string & varname){//,bool only_var_name){

		if(varname == "this"){
			return &this_variable;
		}

		for(unsigned int i = 0; i < this->variable_key.size(); i++){
			std::string symbol = this->variable_key[i];

			/*if(only_var_name){
				symbol=ScriptEval::getSymbolNameFromSymbolRef(symbol);
			}*/

			if(varname == symbol){
				return &variable[i];
			}
		}
		return NULL;
	}

	FunctionSymbol *ScriptVar::addFunctionSymbol(const std::string & symbol_value,const ScriptFunction *irv, bool ignore_duplicates){
		FunctionSymbol si;
		si.proxy_ptr=0;
		si.object = {

				(void *)irv,				// function struct pointer.
				NULL,						// no var ref releated.
				MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION  // dfine as function.
		};

		//std::string symbol_ref=ScriptEval::makeSymbolRef(symbol_value,IDX_ANONYMOUSE_SCOPE);

		if(!ignore_duplicates){
			if(getFunctionSymbol(symbol_value) != NULL){
				writeError(SFI_GET_FILE_LINE(irv,NULL), "internal:symbol already exists");
				return NULL;
			}
		}

		// get super function ...
		si.key_value = symbol_value;
		//si.idxAstNode = _idxAstNode;
		function_symbol.push_back(si);

		return &function_symbol[function_symbol.size()-1];
	}

	FunctionSymbol * ScriptVar::getFunctionSymbol(const std::string & varname){
		for(unsigned int i = 0; i < this->function_symbol.size(); i++){

			std::string symbol = this->function_symbol[i].key_value;

			/*if(only_var_name){
				symbol=ScriptEval::getSymbolNameFromSymbolRef(symbol);
			}*/

			if(varname == symbol){
				return &function_symbol[i];
			}
		}
		return NULL;
	}


	bool ScriptVar::removeVariableSymbolByIndex(unsigned int idx, bool remove_vector){//onst std::string & varname){

		StackElement *si;


		if(idx >= variable.size()){
			writeError(NULL,0,"idx out of bounds (%i>=%i)",idx,variable.size());
			return false;
		}

		si=&variable[idx];
		unsigned short var_type = GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(si->properties);

		switch(var_type){

			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION:
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
				break;
			default: // variable ...

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
			variable.erase(variable.begin()+idx);
			variable_key.erase(variable_key.begin()+idx);
		}

		return true;
	}

	std::vector<StackElement> * ScriptVar::getVectorVariable(){
		return &variable;
	}

	bool ScriptVar::removeVariableSymbolByName(const std::string & varname, const ScriptFunction *info_function){
		//std::string symbol_ref=ScriptEval::makeSymbolRef(varname,IDX_ANONYMOUSE_SCOPE);
		for(unsigned int i = 0; i < this->variable_key.size(); i++){
			if(varname == this->variable_key[i]){
				return removeVariableSymbolByIndex(i,true);
			}
		}
		writeError(SFI_GET_FILE_LINE(info_function,NULL),"symbol %s doesn't exist",varname.c_str());
		return false;
	}

	StackElement * ScriptVar::getVariableSymbolByIndex(int idx){

		if(idx==ZS_IDX_SYMBOL_THIS){
			return &this_variable;
		}


		if(idx >= (int)variable.size()){
			writeError("unknow",-1,"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}

		return &variable[idx];
	}

	FunctionSymbol * ScriptVar::getIdxScriptFunctionObjectByClassFunctionName(const std::string & varname){

		// from lat value to first to get last override function...
		for(int i = this->function_symbol.size()-1; i >= 0; i--){
			if(varname == this->function_symbol[i].key_value){
				return &function_symbol[i];
			}
		}
		return NULL;
	}

	const std::string & ScriptVar::getClassName(){
			return registered_class_info->symbol_info.symbol->name;
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



	FunctionSymbol *ScriptVar::getFunctionSymbolByIndex(unsigned int idx){
		if(idx >= function_symbol.size()){
			writeError("unknow",-1,"idx symbol index out of bounds");
			return NULL;
		}
		return &function_symbol[idx];
	}

	std::vector<FunctionSymbol> * ScriptVar::getVectorFunctionSymbol(){
		return &function_symbol;
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

		 return ((registered_class_info->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != 0);
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
		FunctionSymbol *si;

		for ( unsigned i = 0; i < variable.size(); i++){
			removeVariableSymbolByIndex(i);

		}

		// deallocate function member ...
		for ( unsigned i = 0; i < function_symbol.size(); i++){
			si = &function_symbol[i];
			ScriptFunction * ir_fun  = (ScriptFunction *)(function_symbol[i].object.stk_value);
			 if((ir_fun->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY_C_OBJECT_REF){ // create proxy function ...
				 if((ir_fun->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_STATIC_REF) != SYMBOL_INFO_PROPERTY_STATIC_REF){
					 delete ((CFunctionMemberPointer *)si->proxy_ptr);
				 }
			}
		}
	}

	ScriptVar::~ScriptVar(){

		destroy();
	}
}
