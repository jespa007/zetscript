/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

namespace zs{


	void  writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);




	void CScriptVariable::createSymbols(CScriptClass *ir_class){


		tFunctionSymbol *si;
		tStackElement *se;

		// add extra symbol this itself if is a class typedef by user...
		if(m_infoRegisteredClass->idxClass >=MAX_BUILT_IN_TYPES){
			this_variable.varRef=this;
			this_variable.properties=STK_PROPERTY_IS_THIS_VAR|STK_PROPERTY_TYPE_SCRIPTVAR;
		}

		// Register variables...
		for ( unsigned i = 0; i < ir_class->m_variable.size(); i++){


			tVariableSymbolInfo * ir_var = &ir_class->m_variable[i];

			se=addVariableSymbol(ir_var->symbol->name);

			if(ir_var->properties & PROPERTY_C_OBJECT_REF) //if(IS_CLASS_C)
			{ // we know the type object so we assign the pointer ...
				// check if primitive type (only 4 no more no else)...
				void *ptr_variable = (void*) ((unsigned long long) c_object + ir_var->ref_ptr);

				*se=CScriptClassFactory::C_REF_InfoVariable_2_StackElement(ir_var,ptr_variable);
			}
		}

		// Register functions...
		for ( unsigned i = 0; i < ir_class->m_function.size(); i++){
			CScriptFunction * ir_fun  = ir_class->m_function[i];
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

	void CScriptVariable::setup(){

		ptr_shared_pointer_node = NULL;

		m_infoRegisteredClass = NULL;
		c_object = NULL;
		created_object = NULL;
		m_value = NULL;
		was_created_by_constructor=false;
		c_scriptclass_info=NULL;
		idxClass = -1;
		aux_string ="";
		delete_c_object = false; // --> user is responsible to delete C objects!
		info_function_new=NULL;
		instruction_new=NULL;
		memset(&this_variable,0,sizeof(this_variable));
	}

	CScriptVariable::CScriptVariable(){
		setup();
	}

	void CScriptVariable::init(CScriptClass *irv, void * _c_object){

		setup();

		this->m_infoRegisteredClass = irv;
		idxClass = irv->idxClass;
		c_object = _c_object;
		
		c_scriptclass_info=NULL;

		if(c_object == NULL){ // if object == NULL, the script takes the control. Initialize c_class (c_scriptclass_info) to get needed info to destroy create the C++ object.

			if(m_infoRegisteredClass->isToClass()){
					c_scriptclass_info=m_infoRegisteredClass;
					created_object = (*m_infoRegisteredClass->c_constructor)();
					was_created_by_constructor=true;
					c_object = created_object;
			}else {

				CScriptClass *sc=m_infoRegisteredClass;
				while( sc->idxBaseClass.size()>0 && c_scriptclass_info==NULL){

					sc=GET_SCRIPT_CLASS(sc->idxBaseClass[0]); // get base class...
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
		if(idxClass >= BUILT_IN_TYPE::MAX_BUILT_IN_TYPES && idxClass !=IDX_CLASS_STRING){
			createSymbols(irv);
		}
	}

	CScriptFunction *CScriptVariable::getConstructorFunction(){

		if(m_infoRegisteredClass->idx_function_member_constructor != ZS_UNDEFINED_IDX){
			return m_infoRegisteredClass->m_function[m_infoRegisteredClass->idx_function_member_constructor];
		}

		return NULL;
	}

	bool CScriptVariable::setIdxClass(unsigned char idx){
		CScriptClass *_info_registered_class =  GET_SCRIPT_CLASS(idx);//CScriptClass::getInstance()->getRegisteredClassByIdx(idx);

		if(_info_registered_class == NULL){
			return false;
		}

		m_infoRegisteredClass = _info_registered_class;
		return true;
	}

	bool CScriptVariable::itHasSetMetamethod(){
		return m_infoRegisteredClass->metamethod_operator[SET_METAMETHOD].size() > 0;

	}

	void CScriptVariable::setDelete_C_ObjectOnDestroy(bool _delete_on_destroy){
		created_object=NULL;
		if((this->delete_c_object = _delete_on_destroy)==true){
			created_object=c_object;
		}
	}

	tStackElement * CScriptVariable::addVariableSymbol(const std::string & symbol_value, const CScriptFunction *info_function,tInstruction *src_instruction,tStackElement * sv){
		tStackElement si;

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
			//writeErrorMsg(CURRENT_PARSING_FILENAME,m_line," Symbol name cannot begin with %c", *aux_p);
			//return NULL;
		}

		if(error_symbol){
			writeErrorMsg(INSTRUCTION_GET_FILE_LINE(info_function, src_instruction),"invalid symbol name \"%s\". Check it doesn't start with 0-9, it has no spaces, and it has no special chars like :,;,-,(,),[,], etc.",symbol_value.c_str());
			THROW_RUNTIME_ERROR(CZetScriptUtils::sformat("invalid symbol name \"%s\". Check it doesn't start with 0-9, it has no spaces, and it has no special chars like :,;,-,(,),[,], etc.",symbol_value.c_str()));
			return NULL;
		}

		//std::string symbol_ref=CEval::makeSymbolRef(symbol_value,IDX_ANONYMOUSE_SCOPE);

		if(getVariableSymbol(symbol_value) != NULL){
			writeErrorMsg(INSTRUCTION_GET_FILE_LINE(info_function,src_instruction),"internal:symbol \"%s\" already exists",symbol_value.c_str());
			return NULL;
		}

		if(sv != NULL){
			si = *sv;

			// update n_refs +1
			if(sv->properties&STK_PROPERTY_TYPE_SCRIPTVAR){
				if(!CURRENT_VM->sharePointer(((CScriptVariable *)(sv->varRef))->ptr_shared_pointer_node)){
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
		m_variable.push_back(si);
		m_variableKey.push_back(key_value);

		return &m_variable[m_variable.size()-1];
	}

	tStackElement * CScriptVariable::exist(const char *c){
		for(unsigned i = 0; i < m_variableKey.size(); i++){
			//CScriptVariable *var = (CScriptVariable *)m_variableSymbol[i].object.varRef;
			if(m_variableKey[i] == std::string(c)){
				return &m_variable[i];
			}

		}

		return NULL;
	}

	tStackElement * CScriptVariable::getVariableSymbol(const std::string & varname){//,bool only_var_name){

		if(varname == "this"){
			return &this_variable;
		}

		for(unsigned int i = 0; i < this->m_variableKey.size(); i++){
			std::string symbol = this->m_variableKey[i];

			/*if(only_var_name){
				symbol=CEval::getSymbolNameFromSymbolRef(symbol);
			}*/

			if(varname == symbol){
				return &m_variable[i];
			}
		}
		return NULL;
	}

	tFunctionSymbol *CScriptVariable::addFunctionSymbol(const std::string & symbol_value,const CScriptFunction *irv, bool ignore_duplicates){
		tFunctionSymbol si;
		si.proxy_ptr=0;
		si.object = {

				(void *)irv,				// function struct pointer.
				NULL,						// no var ref releated.
				STK_PROPERTY_TYPE_FUNCTION  // dfine as function.
		};

		//std::string symbol_ref=CEval::makeSymbolRef(symbol_value,IDX_ANONYMOUSE_SCOPE);

		if(!ignore_duplicates){
			if(getFunctionSymbol(symbol_value) != NULL){
				writeErrorMsg(INSTRUCTION_GET_FILE_LINE(irv,NULL), "internal:symbol already exists");
				return NULL;
			}
		}

		// get super function ...
		si.key_value = symbol_value;
		//si.idxAstNode = _idxAstNode;
		m_functionSymbol.push_back(si);

		return &m_functionSymbol[m_functionSymbol.size()-1];
	}

	tFunctionSymbol * CScriptVariable::getFunctionSymbol(const std::string & varname){
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


	bool CScriptVariable::removeVariableSymbolByIndex(unsigned int idx, bool remove_vector){//onst std::string & varname){

		tStackElement *si;


		if(idx >= m_variable.size()){
			writeErrorMsg(NULL,0,"idx out of bounds (%i>=%i)",idx,m_variable.size());
			return false;
		}

		si=&m_variable[idx];
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
							CURRENT_VM->unrefSharedScriptVar(((CScriptVariable *)(si->varRef))->ptr_shared_pointer_node,true);
						}
					}
				}
				break;
		}

		// remove symbol on std::vector ...
		if(remove_vector){
			m_variable.erase(m_variable.begin()+idx);
			m_variableKey.erase(m_variableKey.begin()+idx);
		}

		return true;
	}

	std::vector<tStackElement> * CScriptVariable::getVectorVariable(){
		return &m_variable;
	}

	bool CScriptVariable::removeVariableSymbolByName(const std::string & varname, const CScriptFunction *info_function){
		//std::string symbol_ref=CEval::makeSymbolRef(varname,IDX_ANONYMOUSE_SCOPE);
		for(unsigned int i = 0; i < this->m_variableKey.size(); i++){
			if(varname == this->m_variableKey[i]){
				return removeVariableSymbolByIndex(i,true);
			}
		}
		writeErrorMsg(INSTRUCTION_GET_FILE_LINE(info_function,NULL),"symbol %s doesn't exist",varname.c_str());
		return false;
	}

	tStackElement * CScriptVariable::getVariableSymbolByIndex(unsigned int idx){

		if(idx==ZS_THIS_IDX){
			return &this_variable;
		}


		if(idx >= m_variable.size()){
			writeErrorMsg("unknow",-1,"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}

		return &m_variable[idx];
	}

	tFunctionSymbol * CScriptVariable::getIdxScriptFunctionObjectByClassFunctionName(const std::string & varname){

		// from lat value to first to get last override function...
		for(int i = this->m_functionSymbol.size()-1; i >= 0; i--){
			if(varname == this->m_functionSymbol[i].key_value){
				return &m_functionSymbol[i];
			}
		}
		return NULL;
	}

	const std::string & CScriptVariable::getClassName(){
			return m_infoRegisteredClass->symbol_info.symbol->name;
		}

		const std::string & CScriptVariable::getPointer_C_ClassName(){
			return m_infoRegisteredClass->classPtrType;
		}

		std::string * CScriptVariable::toString(){
			return &m_strValue;
		}

		bool CScriptVariable::initSharedPtr(bool is_assigned){

			if(ptr_shared_pointer_node == NULL){
				ptr_shared_pointer_node = CURRENT_VM->newSharedPointer(this);

				if(is_assigned){ // increment number shared pointers...
					if(!CURRENT_VM->sharePointer(ptr_shared_pointer_node)){
						return NULL;
					}
				}
				return true;
			}

			writeErrorMsg("unknow",-1," shared ptr alrady registered");
			return false;
		}

		bool CScriptVariable::unrefSharedPtr(){
			if(ptr_shared_pointer_node!=NULL){

				CURRENT_VM->unrefSharedScriptVar(ptr_shared_pointer_node);
				return true;
			}
			else{
				writeErrorMsg("unknow",-1,"shared ptr not registered");
			}

			return false;
		}

		intptr_t CScriptVariable::get_C_StructPtr(){
			return (intptr_t)c_object;
		}

	tFunctionSymbol *CScriptVariable::getFunctionSymbolByIndex(unsigned int idx){
		if(idx >= m_functionSymbol.size()){
			writeErrorMsg("unknow",-1,"idx symbol index out of bounds");
			return NULL;
		}
		return &m_functionSymbol[idx];
	}

	std::vector<tFunctionSymbol> * CScriptVariable::getVectorFunctionSymbol(){
		return &m_functionSymbol;
	}


	void * CScriptVariable::get_C_Object(){
		return c_object;
	}

	bool CScriptVariable::isCreatedByContructor(){
		return was_created_by_constructor;
	}

	CScriptClass * CScriptVariable::get_C_Class(){

		 return c_scriptclass_info;
	}

	bool CScriptVariable::is_c_object(){

		 return ((m_infoRegisteredClass->symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) != 0);
	}

	void CScriptVariable::destroy(){
		bool deallocated = false;
		if(created_object != 0){
			if((this->idxClass<MAX_BUILT_IN_TYPES) || delete_c_object){ // only erases pointer if basic type or user/auto delete is required ...

				(*c_scriptclass_info->c_destructor)(created_object);
				deallocated=true;

			}
		}

//#ifdef __ZETSCRIPT_DEBUG__
		if(!deallocated && was_created_by_constructor){
			printf("[%s:%i] Allocated C pointer not deallocated\n",INSTRUCTION_GET_FILE_LINE(info_function_new, instruction_new));
		}
//#endif

		// remove vars & fundtions if class is C...
		tFunctionSymbol *si;

		for ( unsigned i = 0; i < m_variable.size(); i++){
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

	CScriptVariable::~CScriptVariable(){

		destroy();
	}
}
