#include "../../CZetScript.h"


//CUndefined *CScriptVariable::UndefinedSymbol=NULL;
//CVoid *CScriptVariable::VoidSymbol=NULL;
//CNull *CScriptVariable::NullSymbol=NULL;


tSymbolInfo *CScriptVariable::addFunctionSymbol(const string & symbol_value,int _idxAstNode,CScriptFunctionObject *irv){
	tSymbolInfo si;
	si.proxy_ptr=0;
	si.object = {
			INS_PROPERTY_TYPE_FUNCTION, // dfine as function.
			irv,						// function struct pointer.
			NULL						// no var ref releated.
	};

	// get super function ...
	si.super_function = getIdxScriptFunctionObjectByClassFunctionName(symbol_value);

	si.symbol_value = symbol_value;
	si.idxAstNode = _idxAstNode;
	m_functionSymbol.push_back(si);


	return &m_functionSymbol[m_functionSymbol.size()-1];
}

void CScriptVariable::createSymbols(CScriptClass *ir_class){
	tSymbolInfo *si;

	//if(ir_class == this->m_infoRegisteredClass)
	{ // only register vars from higher class because the heredited symbols were created at CCompiler::gacClass

		for ( unsigned i = 0; i < ir_class->metadata_info.object_info.local_symbols.m_registeredVariable.size(); i++){


			tInfoVariableSymbol * ir_var = &ir_class->metadata_info.object_info.local_symbols.m_registeredVariable[i];

			si = addVariableSymbol(ir_var->symbol_name,ir_var->idxAstNode);

			// Warning if you put any var for primitives (i.e CInteger, CNumber, etc will crash in recursive manner)
			if(IS_CLASS_C){ // we know the type object so we assign the pointer ...
				// check if primitive type (only 4 no more no else)...
				void *ptr_variable = (void*) ((unsigned long long) c_object + ir_var->ref_ptr);

				if(*CScriptClass::INT_PTR_TYPE_STR==ir_var->c_type){//={typeid(int *).name(),"int *",IDX_CLASS_INT_PTR_C};
					si->object={
							INS_PROPERTY_TYPE_INTEGER|INS_PROPERTY_IS_C_VAR,
							ptr_variable,
							NULL
					};

				}else if(*CScriptClass::FLOAT_PTR_TYPE_STR==ir_var->c_type){//={typeid(float *).name(),"float *",IDX_CLASS_FLOAT_PTR_C};
					si->object={
							INS_PROPERTY_TYPE_NUMBER|INS_PROPERTY_IS_C_VAR,
							ptr_variable,
							NULL
					};


				}else if(*CScriptClass::STRING_PTR_TYPE_STR==ir_var->c_type){//={typeid(string *).name(),"string *",IDX_CLASS_STRING_PTR_C};

					si->object={
							INS_PROPERTY_TYPE_STRING|INS_PROPERTY_IS_C_VAR,
							ptr_variable,
							NULL
					};

				}else if(*CScriptClass::BOOL_PTR_TYPE_STR==ir_var->c_type){//={typeid(bool *).name(),"bool *",IDX_CLASS_BOOL_PTR_C};
					si->object={
							INS_PROPERTY_TYPE_BOOLEAN|INS_PROPERTY_IS_C_VAR,
							ptr_variable,
							NULL
					};
				}else{
					CScriptClass *info_registered_class = GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(ir_var->c_type);//  CScriptClass::getInstance()->getRegisteredClassBy_C_ClassPtr(ir_var->c_type);

					if(info_registered_class){
						CScriptVariable *var = new CScriptVariable();
						var->init(info_registered_class,ptr_variable);

						si->object={
								INS_PROPERTY_TYPE_SCRIPTVAR|INS_PROPERTY_IS_C_VAR,
								NULL,
								var
						};
					}

				}
			}

		}

	}


	// Register even for primitives (if appropiate)
	for ( unsigned i = 0; i < ir_class->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size(); i++){
		CScriptFunctionObject * ir_fun  = GET_SCRIPT_FUNCTION_OBJECT(ir_class->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[i]);
		 si =addFunctionSymbol(
				 ir_fun->object_info.symbol_info.symbol_name,
				 ir_fun->object_info.symbol_info.idxAstNode,
				ir_fun

				);
		 if((ir_fun->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // create proxy function ...
			si->proxy_ptr = (*((std::function<void *(void *,PROXY_CREATOR)> *)ir_fun->object_info.symbol_info.ref_ptr))(c_object,PROXY_CREATOR::CREATE_FUNCTION);
		}
		//addSymbol(m_infoRegisteredClass->object_info.local_symbols.m_registeredVariable[i].ast);
	}

}


void CScriptVariable::fun1(int *i){
	print_info_cr("FUN1 (I): %i",*i);
}

void CScriptVariable::fun1(int *i, int *j){
	print_info_cr("FUN1 (I): %i %j",*i,*j);
}

void CScriptVariable::fun1(string *s){
	print_info_cr("FUN1 (S): %s",s->c_str());
}

CScriptVariable * CScriptVariable::_add(CScriptVariable *v1){
	// 1. perform operations ...

	// 2. create var variable (after the operation the VM will add pointer into MemoryManager)...
	return new CScriptVariable;
}

CScriptVariable * CScriptVariable::_add(CScriptVariable *v1, CScriptVariable *v2){
	return new CScriptVariable;
}

void CScriptVariable::setup(){

	ptr_shared_pointer_node = NULL;

	m_infoRegisteredClass = NULL;
	c_object = NULL;
	created_object = NULL;
	m_value = NULL;
	idxScriptClass = -1;
	aux_string ="";

}

CScriptVariable::CScriptVariable(){

	setup();

}
/*
CScriptVariable::CScriptVariable(CScriptClass *irv, void *_c_object){

	init(irv,_c_object);

}*/

void CScriptVariable::init(CScriptClass *irv, void *_c_object){
	setup();


	this->m_infoRegisteredClass = irv;
	idxScriptClass = irv->metadata_info.object_info.symbol_info.idxScriptClass;
	//m_rootAst=NULL;
	//m_registeredVariable = NULL;
	//m_type = TYPE::SCRIPT_FUNCTION_TYPE;
	//pointer_function = NULL;
	//m_scope = scope;
	//m_parentFunction = _parentFunction;

	if(IS_CLASS_C){
		if(_c_object == NULL){
			created_object = (*m_infoRegisteredClass->c_constructor)();
			c_object = created_object;
		}else{
			c_object = _c_object;
		}
	}else{
		c_object = this;
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

/*
int CScriptVariable::getIdxClass(){
	return m_infoRegisteredClass->metadata_info.object_info.symbol_info.idxScriptClass;
}*/

bool CScriptVariable::setIdxClass(int idx){
	CScriptClass *_info_registered_class =  GET_SCRIPT_CLASS_INFO(idx);//CScriptClass::getInstance()->getRegisteredClassByIdx(idx);

	if(_info_registered_class == NULL){
		return false;
	}

	m_infoRegisteredClass = _info_registered_class;
	return true;
}


tSymbolInfo * CScriptVariable::addVariableSymbol(const string & symbol_value, int _idxAstNode,tStackElement * sv){
	tSymbolInfo si;
	si.proxy_ptr=0;

	if(sv != NULL){
		si.object = *sv;
	}else{

		si.object={
				INS_PROPERTY_TYPE_UNDEFINED,
				0,
				NULL
		};
	}

	si.idxAstNode = _idxAstNode;
	si.symbol_value = symbol_value;
	m_variableSymbol.push_back(si);

	return &m_variableSymbol[m_variableSymbol.size()-1];
}




tSymbolInfo * CScriptVariable::getVariableSymbol(const string & varname){
	for(unsigned int i = 0; i < this->m_variableSymbol.size(); i++){
		if(varname == this->m_variableSymbol[i].symbol_value){
			return &m_variableSymbol[i];
		}
	}

	return NULL;
}

tSymbolInfo * CScriptVariable::getVariableSymbolByIndex(unsigned idx){
	if(idx >= m_variableSymbol.size()){
		print_error_cr("idx symbol index out of bounds (%i)",idx);
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


int CScriptVariable::getidxScriptFunctionObjectWithMatchArgs(const string & varname, tStackElement * ptrArg, unsigned char n_args, bool match_signature){

	// from lat value to first to get last override function...
	bool all_check=false;


	for(int i = this->m_functionSymbol.size()-1; i>=0; i--){

		CScriptFunctionObject *irfs = (CScriptFunctionObject *)m_functionSymbol[i].object.stkValue;

		if(this->m_functionSymbol[i].symbol_value == varname && (irfs->m_arg.size() == n_args)){
			all_check=true;
			// convert parameters script to c...
			for( unsigned char k = 0; k < n_args && all_check;k++){
				//converted_param[i]= (int)(argv->at(i));


				unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(ptrArg[k].properties);
					switch(var_type){
					default:
						print_error_cr("unknow variable");
						return ZS_UNDEFINED_IDX;
						break;
					case INS_PROPERTY_TYPE_INTEGER:
						aux_string=*CScriptClass::INT_PTR_TYPE_STR;
						break;
					case INS_PROPERTY_TYPE_NUMBER:
						aux_string=*CScriptClass::FLOAT_PTR_TYPE_STR;
						break;
					case INS_PROPERTY_TYPE_BOOLEAN:
						aux_string=*CScriptClass::BOOL_PTR_TYPE_STR;
						break;
					case INS_PROPERTY_TYPE_STRING:
						aux_string=*CScriptClass::STRING_PTR_TYPE_STR;

						break;
					case INS_PROPERTY_TYPE_NULL:
					case INS_PROPERTY_TYPE_UNDEFINED:
					case INS_PROPERTY_TYPE_SCRIPTVAR:
					case INS_PROPERTY_TYPE_SCRIPTVAR|INS_PROPERTY_TYPE_STRING:
						aux_string = ((CScriptVariable *)ptrArg[k].varRef)->getPointer_C_ClassName();
						break;
					}

				bool is_script_var = aux_string == TYPE_SCRIPT_VARIABLE; // if C_ClassName is void means that is a ScriptClass...
				if(match_signature){
					all_check = (aux_string==irfs->m_arg[k]) || (is_script_var);
				}
				else{
					if(aux_string!=irfs->m_arg[k] && !(is_script_var)){
						all_check =(CScriptClass::getConversionType(aux_string,irfs->m_arg[k], false)!=NULL);
					}
				}
			}

			if(all_check){ // we found the right function ...
				return i;
			}
		}
	}
	return ZS_UNDEFINED_IDX;
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


			str_candidates+="\t\t-"+irfs->object_info.symbol_info.symbol_name+"(";

			for(unsigned a = 0; a < irfs->m_arg.size(); a++){
				if(a>0){
					str_candidates+=",";
				}
				str_candidates+=demangle(irfs->m_arg[a]);
			}

			str_candidates+=");\n";


			n_candidates++;
		}
	}
	return str_candidates;
}

const string & CScriptVariable::getClassName(){
		return m_infoRegisteredClass->metadata_info.object_info.symbol_info.symbol_name;
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
				CURRENT_VM->sharePointer(ptr_shared_pointer_node);
			}

			return true;
    	}

    	print_error_cr(" shared ptr alrady registered");
    	return false;
    }

    bool CScriptVariable::unrefSharedPtr(){
    	if(ptr_shared_pointer_node!=NULL){
    		/*if(CURRENT_VM->getNumShares(idx_shared_ptr) > 1){
    			print_error_cr("shared pointer more than once");
    			return false;
    		}

    		int index_0_share_idx = CURRENT_VM->getIdx0Shares(idx_shared_ptr);
    		if(index_0_share_idx!=ZS_UNDEFINED_IDX){
    			CURRENT_VM->remove0Shares(index_0_share_idx);
    		}
    		idx_shared_ptr = ZS_UNDEFINED_IDX;*/
    		CURRENT_VM->unrefSharedScriptVar(ptr_shared_pointer_node);
    		return true;

    	}
    	else{
    		print_error_cr("shared ptr not registered");
    	}

    	return false;
    }

    intptr_t CScriptVariable::get_C_StructPtr(){
		return (intptr_t)c_object;
	}


tSymbolInfo *CScriptVariable::getFunctionSymbolByIndex(unsigned idx){
	if(idx >= m_functionSymbol.size()){
		print_error_cr("idx symbol index out of bounds");
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

CScriptVariable::~CScriptVariable(){

	if(created_object != NULL){
		 (*m_infoRegisteredClass->c_destructor)(created_object);
	}


	// remove vars & fundtions if class is C...
	tSymbolInfo *si;

	//if((this->m_infoRegisteredClass->metadata_info.object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == 0){ // script class
		for ( unsigned i = 0; i < m_variableSymbol.size(); i++){
			si = &m_variableSymbol[i];
			unsigned short var_type = GET_INS_PROPERTY_VAR_TYPE(m_variableSymbol[i].object.properties);

			switch(var_type){

				case INS_PROPERTY_TYPE_BOOLEAN:
				case INS_PROPERTY_TYPE_INTEGER:
				case INS_PROPERTY_TYPE_UNDEFINED:
				case INS_PROPERTY_TYPE_NULL:
				case INS_PROPERTY_TYPE_FUNCTION:
				case INS_PROPERTY_TYPE_NUMBER:
					break;

				default: // variable ...
					if(m_variableSymbol[i].object.properties & INS_PROPERTY_IS_C_VAR){ // deallocate but not if is native string
						if(m_variableSymbol[i].object.varRef != NULL){
							delete ((CScriptVariable *)(m_variableSymbol[i].object.varRef));
						}
					}
					break;
			}

		}
	//}


	// Register even for primitives (if appropiate)
	for ( unsigned i = 0; i < m_functionSymbol.size(); i++){
		si = &m_functionSymbol[i];
		CScriptFunctionObject * ir_fun  = (CScriptFunctionObject *)(m_functionSymbol[i].object.stkValue);
		 if((ir_fun->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // create proxy function ...
			 (*((std::function<void *(void *,PROXY_CREATOR)> *)ir_fun->object_info.symbol_info.ref_ptr))(si->proxy_ptr,PROXY_CREATOR::DESTROY_FUNCTION);

		}
		//addSymbol(m_infoRegisteredClass->object_info.local_symbols.m_registeredVariable[i].ast);
	}

}
