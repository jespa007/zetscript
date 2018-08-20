/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../CZetScript.h"

namespace zetscript{


	void  writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);




	void CScriptVariable::createSymbols(CScriptClass *ir_class){


		tSymbolInfo *si;

		if(m_infoRegisteredClass->idxClass >=MAX_BASIC_CLASS_TYPES){
			this_symbol.object.varRef=this;
			this_symbol.object.properties=STK_PROPERTY_IS_THIS_VAR|STK_PROPERTY_TYPE_SCRIPTVAR;
		}

		// Register variables...
		for ( unsigned i = 0; i < ir_class->metadata_info.object_info.local_symbols.m_registeredVariable.size(); i++){


			tInfoVariableSymbol * ir_var = &ir_class->metadata_info.object_info.local_symbols.m_registeredVariable[i];

			si = addVariableSymbol(CCompiler::getSymbolNameFromSymbolRef(ir_var->symbol_ref),ir_var->idxAstNode);



			if(ir_var->properties & PROPERTY_C_OBJECT_REF) //if(IS_CLASS_C)
			{ // we know the type object so we assign the pointer ...
				// check if primitive type (only 4 no more no else)...
				void *ptr_variable = (void*) ((unsigned long long) c_object + ir_var->ref_ptr);

				si->object=CScriptClass::C_REF_InfoVariable_2_StackElement(ir_var,ptr_variable);
			}
		}


		// Register functions...
		for ( unsigned i = 0; i < ir_class->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size(); i++){
			CScriptFunctionObject * ir_fun  = GET_SCRIPT_FUNCTION_OBJECT(ir_class->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[i]);
			 si =addFunctionSymbol(
					 CCompiler::getSymbolNameFromSymbolRef( ir_fun->object_info.symbol_info.symbol_ref),
					 ir_fun->object_info.symbol_info.idxAstNode,
					ir_fun

					);
			 if((ir_fun->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF){ // create proxy function ...
				 // static ref only get ref function ...
				 if((ir_fun->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_STATIC_REF) == SYMBOL_INFO_PROPERTY::PROPERTY_STATIC_REF){
					 si->proxy_ptr = ir_fun->object_info.symbol_info.ref_ptr;
				 }
				 else{
					 si->proxy_ptr = (intptr_t)(*((std::function<void *(void *,PROXY_CREATOR)> *)ir_fun->object_info.symbol_info.ref_ptr))(c_object,PROXY_CREATOR::CREATE_FUNCTION);
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
		idxScriptClass = -1;
		aux_string ="";
		delete_c_object = false; // --> user is responsible to delete C objects!
		ast_node_new=ZS_UNDEFINED_IDX;
		memset(&this_symbol,0,sizeof(this_symbol));
	}

	CScriptVariable::CScriptVariable(){
		setup();
	}

	void CScriptVariable::init(CScriptClass *irv, void * _c_object){

		setup();

		this->m_infoRegisteredClass = irv;
		idxScriptClass = irv->idxClass;
		c_object = _c_object;
		
		c_scriptclass_info=NULL;

		if(c_object == NULL){ // if object == NULL, the script takes the control. Initialize c_class (c_scriptclass_info) to get needed info to destroy create the C++ object.

			if(m_infoRegisteredClass->is_c_class()){
					c_scriptclass_info=m_infoRegisteredClass;
					created_object = (*m_infoRegisteredClass->c_constructor)();
					was_created_by_constructor=true;
					c_object = created_object;
			}else {

				CScriptClass *sc=m_infoRegisteredClass;
				while( sc->idxBaseClass.size()>0 && c_scriptclass_info==NULL){

					sc=CScriptClass::getScriptClassByIdx(sc->idxBaseClass[0]); // get base class...
					if(sc->is_c_class()){
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

		// only create symbols if not string type to make it fast ...
		if(idxScriptClass >= MAX_CLASS_C_TYPES && idxScriptClass !=IDX_CLASS_STRING){
			createSymbols(irv);
		}
	}

	CScriptFunctionObject *CScriptVariable::getConstructorFunction(){

		if(m_infoRegisteredClass->idx_function_script_constructor != ZS_UNDEFINED_IDX){
			return GET_SCRIPT_FUNCTION_OBJECT(m_infoRegisteredClass->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[m_infoRegisteredClass->idx_function_script_constructor]);
		}

		return NULL;
	}

	bool CScriptVariable::setIdxClass(int idx){
		CScriptClass *_info_registered_class =  GET_SCRIPT_CLASS_INFO(idx);//CScriptClass::getInstance()->getRegisteredClassByIdx(idx);

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

	tSymbolInfo * CScriptVariable::addVariableSymbol(const string & symbol_value, int _idxAstNode,tStackElement * sv){
		tSymbolInfo si;
		si.proxy_ptr=0;
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
			writeErrorMsg(GET_AST_FILENAME_LINE(_idxAstNode),"invalid symbol name \"%s\". Check it doesn't start with 0-9, it has no spaces, and it has no special chars like :,;,-,(,),[,], etc.",symbol_value.c_str());
			THROW_RUNTIME_ERROR("invalid symbol name \"%s\". Check it doesn't start with 0-9, it has no spaces, and it has no special chars like :,;,-,(,),[,], etc.",symbol_value.c_str());
			return NULL;
		}

		string symbol_ref=CCompiler::makeSymbolRef(symbol_value,IDX_ANONYMOUSE_SCOPE);

		if(getVariableSymbol(symbol_ref) != NULL){
			writeErrorMsg(GET_AST_FILENAME_LINE(_idxAstNode),"symbol \"%s\" already exists",symbol_value.c_str());
			return NULL;
		}

		if(sv != NULL){
			si.object = *sv;

			// update n_refs +1
			if(sv->properties&STK_PROPERTY_TYPE_SCRIPTVAR){
				if(!CURRENT_VM->sharePointer(((CScriptVariable *)(sv->varRef))->ptr_shared_pointer_node)){
					return NULL;
				}
			}

		}else{

			si.object={
					STK_PROPERTY_TYPE_UNDEFINED,
					0,
					NULL
			};
		}

		si.idxAstNode = _idxAstNode;
		si.symbol_value = symbol_ref;
		m_variableSymbol.push_back(si);

		return &m_variableSymbol[m_variableSymbol.size()-1];
	}

	tSymbolInfo * CScriptVariable::getVariableSymbol(const string & varname,bool only_var_name){

		if(varname == "this"){
			return &this_symbol;
		}

		for(unsigned int i = 0; i < this->m_variableSymbol.size(); i++){
			string symbol = this->m_variableSymbol[i].symbol_value;

			if(only_var_name){
				symbol=CCompiler::getSymbolNameFromSymbolRef(symbol);
			}

			if(varname == symbol){
				return &m_variableSymbol[i];
			}
		}
		return NULL;
	}

	tSymbolInfo *CScriptVariable::addFunctionSymbol(const string & symbol_value,int _idxAstNode,CScriptFunctionObject *irv, bool ignore_duplicates){
		tSymbolInfo si;
		si.proxy_ptr=0;
		si.object = {
				STK_PROPERTY_TYPE_FUNCTION, // dfine as function.
				irv,						// function struct pointer.
				NULL						// no var ref releated.
		};

		string symbol_ref=CCompiler::makeSymbolRef(symbol_value,IDX_ANONYMOUSE_SCOPE);

		if(!ignore_duplicates){
			if(getFunctionSymbol(symbol_ref) != NULL){
				writeErrorMsg(GET_AST_FILENAME_LINE(_idxAstNode), "symbol already exists");
				return NULL;
			}
		}

		// get super function ...
		si.symbol_value = symbol_ref;
		si.idxAstNode = _idxAstNode;
		m_functionSymbol.push_back(si);

		return &m_functionSymbol[m_functionSymbol.size()-1];
	}

	tSymbolInfo * CScriptVariable::getFunctionSymbol(const string & varname,bool only_var_name){
		for(unsigned int i = 0; i < this->m_functionSymbol.size(); i++){

			string symbol = this->m_functionSymbol[i].symbol_value;

			if(only_var_name){
				symbol=CCompiler::getSymbolNameFromSymbolRef(symbol);
			}

			if(varname == symbol){
				return &m_functionSymbol[i];
			}
		}
		return NULL;
	}


	bool CScriptVariable::removeVariableSymbolByIndex(unsigned idx, bool remove_vector){//onst string & varname){

		tSymbolInfo *si;


		if(idx >= m_variableSymbol.size()){
			writeErrorMsg(NULL,0,"idx out of bounds (%i>=%i)",idx,m_variableSymbol.size());
			return false;
		}

		si=&m_variableSymbol[idx];
		unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(si->object.properties);

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
					if(((si->object.properties & STK_PROPERTY_IS_C_VAR) != STK_PROPERTY_IS_C_VAR)
						&& ((si->object.properties & STK_PROPERTY_IS_THIS_VAR) != STK_PROPERTY_IS_THIS_VAR)){ // deallocate but not if is c or this ref
						if(si->object.varRef != NULL){
							// remove property if not referenced anymore
							CURRENT_VM->unrefSharedScriptVar(((CScriptVariable *)(si->object.varRef))->ptr_shared_pointer_node,true);
						}
					}
				}
				break;
		}

		// remove symbol on vector ...
		if(remove_vector){
			m_variableSymbol.erase(m_variableSymbol.begin()+idx);
		}

		return true;

	}


	bool CScriptVariable::removeVariableSymbolByName(const string & varname, int idxAstNode){
		string symbol_ref=CCompiler::makeSymbolRef(varname,IDX_ANONYMOUSE_SCOPE);
		for(unsigned int i = 0; i < this->m_variableSymbol.size(); i++){
			if(symbol_ref == this->m_variableSymbol[i].symbol_value){
				return removeVariableSymbolByIndex(i,true);
			}
		}
		writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"symbol %s doesn't exist",varname.c_str());
		return false;
	}

	tSymbolInfo * CScriptVariable::getVariableSymbolByIndex(int idx){

		if(idx==ZS_THIS_IDX){
			return &this_symbol;
		}


		if(idx >= m_variableSymbol.size()){
			writeErrorMsg(GET_AST_FILENAME_LINE(ZS_UNDEFINED_IDX),"idx symbol index out of bounds (%i)",idx);
			return NULL;
		}

		return &m_variableSymbol[idx];
	}

	tSymbolInfo * CScriptVariable::getIdxScriptFunctionObjectByClassFunctionName(const string & varname){

		// from lat value to first to get last override function...
		for(int i = this->m_functionSymbol.size()-1; i >= 0; i--){
			if(varname == this->m_functionSymbol[i].symbol_value){
				return &m_functionSymbol[i];
			}
		}
		return NULL;
	}

	string CScriptVariable::getMessageMatchingFunctions(const string & varname){
		int n_candidates=0;
		string str_candidates="";
		for(int i = this->m_functionSymbol.size()-1; i>=0; i--){

			CScriptFunctionObject *irfs = (CScriptFunctionObject *)m_functionSymbol[i].object.stkValue;

			if(this->m_functionSymbol[i].symbol_value == varname){
				if(n_candidates == 0){
					str_candidates+="\t\tPossible candidates are:\n\n";
				}


				str_candidates+="\t\t-"+irfs->object_info.symbol_info.symbol_ref+"(";

				for(unsigned a = 0; a < irfs->m_arg.size(); a++){
					if(a>0){
						str_candidates+=",";
					}
					str_candidates+=demangle(GET_IDX_2_CLASS_C_STR(irfs->m_arg[a].idx_type));
				}
				str_candidates+=");\n";

				n_candidates++;
			}
		}
		return str_candidates;
	}

	const string & CScriptVariable::getClassName(){
			return m_infoRegisteredClass->metadata_info.object_info.symbol_info.symbol_ref;
		}

		const string & CScriptVariable::getPointer_C_ClassName(){
			return m_infoRegisteredClass->classPtrType;
		}

		string * CScriptVariable::toString(){
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

			writeErrorMsg(GET_AST_FILENAME_LINE(ZS_UNDEFINED_IDX)," shared ptr alrady registered");
			return false;
		}

		bool CScriptVariable::unrefSharedPtr(){
			if(ptr_shared_pointer_node!=NULL){

				CURRENT_VM->unrefSharedScriptVar(ptr_shared_pointer_node);
				return true;
			}
			else{
				writeErrorMsg(GET_AST_FILENAME_LINE(ZS_UNDEFINED_IDX),"shared ptr not registered");
			}

			return false;
		}

		intptr_t CScriptVariable::get_C_StructPtr(){
			return (intptr_t)c_object;
		}

	tSymbolInfo *CScriptVariable::getFunctionSymbolByIndex(unsigned idx){
		if(idx >= m_functionSymbol.size()){
			writeErrorMsg(GET_AST_FILENAME_LINE(ZS_UNDEFINED_IDX),"idx symbol index out of bounds");
			return NULL;
		}
		return &m_functionSymbol[idx];
	}

	vector<tSymbolInfo> * CScriptVariable::getVectorFunctionSymbol(){
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

		 return ((m_infoRegisteredClass->metadata_info.object_info.symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) != 0);
	}

	void CScriptVariable::destroy(){
		bool deallocated = false;
		if(created_object != 0){
			if((this->idxScriptClass<MAX_BASIC_CLASS_TYPES) || delete_c_object){ // only erases pointer if basic type or user/auto delete is required ...

				(*c_scriptclass_info->c_destructor)(created_object);
				deallocated=true;

			}
		}



//#ifdef __ZETSCRIPT_DEBUG__
		if(!deallocated && was_created_by_constructor){
			printf("[%s:%i] Allocated C pointer not deallocated\n",GET_AST_FILENAME_LINE(ast_node_new));
		}
//#endif

		// remove vars & fundtions if class is C...
		tSymbolInfo *si;

		for ( unsigned i = 0; i < m_variableSymbol.size(); i++){
			removeVariableSymbolByIndex(i);

		}

		// deallocate function member ...
		for ( unsigned i = 0; i < m_functionSymbol.size(); i++){
			si = &m_functionSymbol[i];
			CScriptFunctionObject * ir_fun  = (CScriptFunctionObject *)(m_functionSymbol[i].object.stkValue);
			 if((ir_fun->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF){ // create proxy function ...
				 if((ir_fun->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_STATIC_REF) != SYMBOL_INFO_PROPERTY::PROPERTY_STATIC_REF){

					 (*((std::function<void *(void *,PROXY_CREATOR)> *)ir_fun->object_info.symbol_info.ref_ptr))((void *)si->proxy_ptr,PROXY_CREATOR::DESTROY_FUNCTION);
				 }
			}
		}



	}

	CScriptVariable::~CScriptVariable(){

		destroy();

	}
}
