/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

namespace zetscript{


	void  write_error(const char *filename, int line, const  char  *string_text, ...);




	void CScriptVar::createSymbols(CScriptClass *ir_class){


		FunctionSymbol *si;
		StackElement *se;

		// add extra symbol this itself if is a class typedef by user...
		if(registered_class_info->idx_class >=MAX_BUILT_IN_TYPES){
			this_variable.varRef=this;
			this_variable.properties=STK_PROPERTY_IS_THIS_VAR|STK_PROPERTY_TYPE_SCRIPTVAR;
		}

		// Register variables...
		for ( unsigned i = 0; i < ir_class->local_variable.size(); i++){


			VariableSymbolInfo * ir_var = &ir_class->local_variable[i];

			se=addVariableSymbol(ir_var->symbol->name);

			if(ir_var->properties & PROPERTY_C_OBJECT_REF) //if(IS_CLASS_C)
			{ // we know the type object so we assign the pointer ...
				// check if primitive type (only 4 no more no else)...
				void *ptr_variable = (void*) ((unsigned long long) c_object + ir_var->ref_ptr);

				*se=zs->C_REF_InfoVariable_2_StackElement(ir_var,ptr_variable);
			}
		}

		// Register functions...
		for ( unsigned i = 0; i < ir_class->local_function.size(); i++){
			CScriptFunction * ir_fun  = ir_class->local_function[i];
			 si =addFunctionSymbol(
					  ir_fun->symbol_info.symbol->name,

					ir_fun

					);
			 if((ir_fun->symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF){ // create proxy function ...
				 // static ref only get ref function ...
				 if((ir_fun->symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_STATIC_REF) == SYMBOL_INFO_PROPERTY::PROPERTY_STATIC_REF){
					 si->proxy_ptr = ir_fun->symbol_info.ref_ptr;
				 }
				 else{
					 si->proxy_ptr = (intptr_t)(*((std::function<void *(void *,PROXY_CREATOR)> *)ir_fun->symbol_info.ref_ptr))(c_object,PROXY_CREATOR::CREATE_FUNCTION);
				 }
			}
		}
	}

	void CScriptVar::setup(){

		ptr_shared_pointer_node = NULL;

		registered_class_info = NULL;
		c_object = NULL;
		created_object = NULL;
		m_value = NULL;
		was_created_by_constructor=false;
		c_scriptclass_info=NULL;
		idx_class = -1;
		aux_string ="";
		delete_c_object = false; // --> user is responsible to delete C objects!
		info_function_new=NULL;
		instruction_new=NULL;
		memset(&this_variable,0,sizeof(this_variable));
	}

	CScriptVar::CScriptVar(){
		setup();
	}

	CScriptVar::CScriptVar(CZetScript	*_zs){
		setup();

		zs=_zs;
		virtual_machine = zs->getVirtualMachine();
		script_class_factory = zs->getScriptClassFactory();
	}

	void CScriptVar::init(CScriptClass *irv, void * _c_object){

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

				CScriptClass *sc=registered_class_info;
				while( sc->idx_base_class.size()>0 && c_scriptclass_info==NULL){

					sc=GET_SCRIPT_CLASS(sc->idx_base_class[0]); // get base class...
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
		if(idx_class >= MAX_BUILT_IN_TYPES && idx_class != IDX_CLASS_STRING){
			createSymbols(irv);
		}
	}

	CScriptFunction *CScriptVar::getConstructorFunction(){

		if(registered_class_info->idx_function_member_constructor != ZS_UNDEFINED_IDX){
			return registered_class_info->local_function[registered_class_info->idx_function_member_constructor];
		}

		return NULL;
	}

	bool CScriptVar::setIdxClass(unsigned char idx){
		CScriptClass *_info_registered_class =  GET_SCRIPT_CLASS(idx);//CScriptClass::getInstance()->getRegisteredClassByIdx(idx);

		if(_info_registered_class == NULL){
			return false;
		}

		registered_class_info = _info_registered_class;
		return true;
	}

	bool CScriptVar::itHasSetMetamethod(){
		return registered_class_info->metamethod_operator[SET_METAMETHOD].size() > 0;

	}

	void CScriptVar::setDelete_C_ObjectOnDestroy(bool _delete_on_destroy){
		created_object=NULL;
		if((this->delete_c_object = _delete_on_destroy)==true){
			created_object=c_object;
		}
	}

	StackElement * CScriptVar::addVariableSymbol(const std::string & symbol_value, const CScriptFunction *info_function,CByteCode *src_instruction,StackElement * sv){
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
			//write_error(CURRENT_PARSING_FILENAME,m_line," Symbol name cannot begin with %c", *aux_p);
			//return NULL;
		}

		if(error_symbol){
			write_error(INSTRUCTION_GET_FILE_LINE(info_function, src_instruction),"invalid symbol name \"%s\". Check it doesn't start with 0-9, it has no spaces, and it has no special chars like :,;,-,(,),[,], etc.",symbol_value.c_str());
			THROW_RUNTIME_ERROR(string::sformat("invalid symbol name \"%s\". Check it doesn't start with 0-9, it has no spaces, and it has no special chars like :,;,-,(,),[,], etc.",symbol_value.c_str()));
			return NULL;
		}

		//std::string symbol_ref=CEval::makeSymbolRef(symbol_value,IDX_ANONYMOUSE_SCOPE);

		if(getVariableSymbol(symbol_value) != NULL){
			write_error(INSTRUCTION_GET_FILE_LINE(info_function,src_instruction),"internal:symbol \"%s\" already exists",symbol_value.c_str());
			return NULL;
		}

		if(sv != NULL){
			si = *sv;

			// update n_refs +1
			if(sv->properties&STK_PROPERTY_TYPE_SCRIPTVAR){
				if(!virtual_machine->sharePointer(((CScriptVar *)(sv->varRef))->ptr_shared_pointer_node)){
					return NULL;
				}
			}

		}else{

			si={
				0,
				0,
				STK_PROPERTY_TYPE_UNDEFINED
			};
		}

		//si.idxAstNode = _idxAstNode;
		std::string key_value = symbol_value;
		variable.push_back(si);
		m_variableKey.push_back(key_value);

		return &variable[variable.size()-1];
	}

	StackElement * CScriptVar::exist(const char *c){
		for(unsigned i = 0; i < m_variableKey.size(); i++){
			//CScriptVar *var = (CScriptVar *)m_variableSymbol[i].object.varRef;
			if(m_variableKey[i] == std::string(c)){
				return &variable[i];
			}

		}

		return NULL;
	}

	StackElement * CScriptVar::getVariableSymbol(const std::string & varname){//,bool only_var_name){

		if(varname == "this"){
			return &this_variable;
		}

		for(unsigned int i = 0; i < this->m_variableKey.size(); i++){
			std::string symbol = this->m_variableKey[i];

			/*if(only_var_name){
				symbol=CEval::getSymbolNameFromSymbolRef(symbol);
			}*/

			if(varname == symbol){
				return &variable[i];
			}
		}
		return NULL;
	}

	FunctionSymbol *CScriptVar::addFunctionSymbol(const std::string & symbol_value,const CScriptFunction *irv, bool ignore_duplicates){
		FunctionSymbol si;
		si.proxy_ptr=0;
		si.object = {

				(void *)irv,				// function struct pointer.
				NULL,						// no var ref releated.
				STK_PROPERTY_TYPE_FUNCTION  // dfine as function.
		};

		//std::string symbol_ref=CEval::makeSymbolRef(symbol_value,IDX_ANONYMOUSE_SCOPE);

		if(!ignore_duplicates){
			if(getFunctionSymbol(symbol_value) != NULL){
				write_error(INSTRUCTION_GET_FILE_LINE(irv,NULL), "internal:symbol already exists");
				return NULL;
			}
		}

		// get super function ...
		si.key_value = symbol_value;
		//si.idxAstNode = _idxAstNode;
		m_functionSymbol.push_back(si);

		return &m_functionSymbol[m_functionSymbol.size()-1];
	}

	FunctionSymbol * CScriptVar::getFunctionSymbol(const std::string & varname){
		for(unsigned int i = 0; i < this->m_functionSymbol.size(); i++){

			std::string symbol = this->m_functionSymbol[i].key_value;

			/*if(only_var_name){
				symbol=CEval::getSymbolNameFromSymbolRef(symbol);
			}*/

			if(varname == symbol){
				return &m_functionSymbol[i];
			}
		}
		return NULL;
	}


	bool CScriptVar::removeVariableSymbolByIndex(unsigned int idx, bool remove_vector){//onst std::string & varname){

		StackElement *si;


		if(idx >= variable.size()){
			write_error(NULL,0,"idx out of bounds (%i>=%i)",idx,variable.size());
			return false;
		}

		si=&variable[idx];
		unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(si->properties);

		switch(var_type){

			case STK_PROPERTY_TYPE_BOOLEAN:
			case STK_PROPERTY_TYPE_INTEGER:
			case STK_PROPERTY_TYPE_UNDEFINED:
			case STK_PROPERTY_TYPE_NULL:
			case STK_PROPERTY_TYPE_FUNCTION:
			case STK_PROPERTY_TYPE_NUMBER:
				break;
			default: // variable ...

				if(var_type & STK_PROPERTY_TYPE_SCRIPTVAR){
					if(((si->properties & STK_PROPERTY_IS_C_VAR) != STK_PROPERTY_IS_C_VAR)
						&& ((si->properties & STK_PROPERTY_IS_THIS_VAR) != STK_PROPERTY_IS_THIS_VAR)){ // deallocate but not if is c or this ref
						if(si->varRef != NULL){
							// remove property if not referenced anymore
							virtual_machine->unrefSharedScriptVar(((CScriptVar *)(si->varRef))->ptr_shared_pointer_node,true);
						}
					}
				}
				break;
		}

		// remove symbol on std::vector ...
		if(remove_vector){
			variable.erase(variable.begin()+idx);
			m_variableKey.erase(m_variableKey.begin()+idx);
		}

		return true;
	}

	std::vector<StackElement> * CScriptVar::getVectorVariable(){
		return &variable;
	}

	bool CScriptVar::removeVariableSymbolByName(const std::string & varname, const CScriptFunction *info_function){
		//std::string symbol_ref=CEval::makeSymbolRef(varname,IDX_ANONYMOUSE_SCOPE);
		for(unsigned int i = 0; i < this->m_variableKey.size(); i++){
			if(varname == this->m_variableKey[i]){
				return removeVariableSymbolByIndex(i,true);
			}
		}
		write_error(INSTRUCTION_GET_FILE_LINE(info_function,NULL),"symbol %s doesn't exist",varname.c_str());
		return false;
	}

	StackElement * CScriptVar::getVariableSymbolByIndex(unsigned int idx){

		if(idx==ZS_THIS_IDX){
			return &this_variable;
		}


		if(idx >= variable.size()){
			write_error("unknow",-1,"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}

		return &variable[idx];
	}

	FunctionSymbol * CScriptVar::getIdxScriptFunctionObjectByClassFunctionName(const std::string & varname){

		// from lat value to first to get last override function...
		for(int i = this->m_functionSymbol.size()-1; i >= 0; i--){
			if(varname == this->m_functionSymbol[i].key_value){
				return &m_functionSymbol[i];
			}
		}
		return NULL;
	}

	const std::string & CScriptVar::getClassName(){
			return registered_class_info->symbol_info.symbol->name;
		}

		const std::string & CScriptVar::getPointer_C_ClassName(){
			return registered_class_info->str_class_ptr_type;
		}

		std::string * CScriptVar::toString(){
			return &m_strValue;
		}

		bool CScriptVar::initSharedPtr(bool is_assigned){

			if(ptr_shared_pointer_node == NULL){
				ptr_shared_pointer_node = virtual_machine->newSharedPointer(this);

				if(is_assigned){ // increment number shared pointers...
					if(!virtual_machine->sharePointer(ptr_shared_pointer_node)){
						return NULL;
					}
				}
				return true;
			}

			write_error("unknow",-1," shared ptr alrady registered");
			return false;
		}

		bool CScriptVar::unrefSharedPtr(){
			if(ptr_shared_pointer_node!=NULL){

				virtual_machine->unrefSharedScriptVar(ptr_shared_pointer_node);
				return true;
			}
			else{
				write_error("unknow",-1,"shared ptr not registered");
			}

			return false;
		}

		intptr_t CScriptVar::get_C_StructPtr(){
			return (intptr_t)c_object;
		}

	FunctionSymbol *CScriptVar::getFunctionSymbolByIndex(unsigned int idx){
		if(idx >= m_functionSymbol.size()){
			write_error("unknow",-1,"idx symbol index out of bounds");
			return NULL;
		}
		return &m_functionSymbol[idx];
	}

	std::vector<FunctionSymbol> * CScriptVar::getVectorFunctionSymbol(){
		return &m_functionSymbol;
	}


	void * CScriptVar::get_C_Object(){
		return c_object;
	}

	bool CScriptVar::isCreatedByContructor(){
		return was_created_by_constructor;
	}

	CScriptClass * CScriptVar::get_C_Class(){

		 return c_scriptclass_info;
	}

	bool CScriptVar::is_c_object(){

		 return ((registered_class_info->symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) != 0);
	}

	void CScriptVar::destroy(){
		bool deallocated = false;
		if(created_object != 0){
			if((this->idx_class<MAX_BUILT_IN_TYPES) || delete_c_object){ // only erases pointer if basic type or user/auto delete is required ...

				(*c_scriptclass_info->c_destructor)(created_object);
				deallocated=true;

			}
		}

//#ifdef __DEBUG__
		if(!deallocated && was_created_by_constructor){
			printf("[%s:%i] Allocated C pointer not deallocated\n",INSTRUCTION_GET_FILE_LINE(info_function_new, instruction_new));
		}
//#endif

		// remove vars & fundtions if class is C...
		FunctionSymbol *si;

		for ( unsigned i = 0; i < variable.size(); i++){
			removeVariableSymbolByIndex(i);

		}

		// deallocate function member ...
		for ( unsigned i = 0; i < m_functionSymbol.size(); i++){
			si = &m_functionSymbol[i];
			CScriptFunction * ir_fun  = (CScriptFunction *)(m_functionSymbol[i].object.stkValue);
			 if((ir_fun->symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF){ // create proxy function ...
				 if((ir_fun->symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_STATIC_REF) != SYMBOL_INFO_PROPERTY::PROPERTY_STATIC_REF){

					 (*((std::function<void *(void *,PROXY_CREATOR)> *)ir_fun->symbol_info.ref_ptr))((void *)si->proxy_ptr,PROXY_CREATOR::DESTROY_FUNCTION);
				 }
			}
		}
	}

	CScriptVar::~CScriptVar(){

		destroy();
	}
}
