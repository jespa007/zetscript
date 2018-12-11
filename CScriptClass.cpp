/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

namespace zetscript{

	
	#define REGISTER_BASIC_TYPE(type_class, idx_class)\
		if(!register_C_ClassInt<type_class>(STR(type_class))) return false;\
		if(vec_script_class_node->at(idx_class)->classPtrType!=typeid(type_class *).name()){\
			THROW_RUNTIME_ERROR("Error initializing basic type: %s",STR(type_class));\
			return false;\
		}

	void  writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);

	vector<CScriptClass *> 				* CScriptClass::vec_script_class_node=NULL;
	bool CScriptClass::register_c_base_symbols=false;

	string  * CScriptClass::VOID_TYPE_STR=NULL;// 		typeid(void).name()
	string  * CScriptClass::INT_PTR_TYPE_STR=NULL;//	typeid(int *).name()
	string  * CScriptClass::FLOAT_PTR_TYPE_STR=NULL;//	typeid(float *).name()
	string  * CScriptClass::STRING_PTR_TYPE_STR=NULL;//	typeid(string *).name()
	string  * CScriptClass::CONST_CHAR_PTR_TYPE_STR=NULL;//	typeid(string *).name()
	string  * CScriptClass::BOOL_PTR_TYPE_STR=NULL;//	typeid(bool *).name()

	string  * CScriptClass::INT_TYPE_STR=NULL;//	typeid(int *).name()
	string  * CScriptClass::UNSIGNED_INT_TYPE_STR=NULL;//	typeid(int *).name()
	string  * CScriptClass::INTPTR_T_TYPE_STR=NULL;//	typeid(int *).name()
	string  * CScriptClass::FLOAT_TYPE_STR=NULL;//	typeid(int *).name()
	string  * CScriptClass::BOOL_TYPE_STR=NULL;//	typeid(bool *).name()
	string  * CScriptClass::STACK_ELEMENT_PTR=NULL;//	typeid(bool *).name()


	// register metamethods str ...
	 //--obj , type convert, ---
	 map<int,map<int,fntConversionType>>  * CScriptClass::mapTypeConversion=NULL;
	void (* CScriptClass::print_out_callback)(const char *) = NULL;


	unsigned char CScriptClass::getIdxClassFromIts_C_TypeInternal(const string & c_type_str){

		// 1. we have to handle primitives like void, (int *), (bool *),(float *) and (string *).
		 // 2. Check for rest registered C classes...
		 for(unsigned i = 0; i < vec_script_class_node->size(); i++){
			 if(vec_script_class_node->at(i)->classPtrType==c_type_str)
			 {
				 return i;
			 }
		 }

		 return ZS_INVALID_CLASS;
	 }

	unsigned char 			CScriptClass::getIdxClassFromIts_C_Type(const string & c_type_str){
		return CScriptClass::getIdxClassFromIts_C_TypeInternal(c_type_str);
	}

	bool 	CScriptClass::isIdxClassInstanceOf(unsigned char idxSrcClass, unsigned char class_idx){

		if(idxSrcClass == class_idx){
			return true;
		}

		CScriptClass * theClass = vec_script_class_node->at(idxSrcClass);

		for(unsigned i=0; i < theClass->idxBaseClass.size(); i++){
			if(isIdxClassInstanceOf(theClass->idxBaseClass[i],class_idx)){
				return true;
			}
		}

		return false;
	}

	tStackElement CScriptClass::C_REF_InfoVariable_2_StackElement(tVariableSymbolInfo *ir_var, void *ptr_variable){

		if(ir_var->properties & PROPERTY_C_OBJECT_REF){

			if(*CScriptClass::INT_PTR_TYPE_STR==ir_var->c_type){
				return {
						STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_IS_C_VAR,
						0,
						ptr_variable
				};

			}else if(*CScriptClass::FLOAT_PTR_TYPE_STR==ir_var->c_type){
				return {
						STK_PROPERTY_TYPE_NUMBER|STK_PROPERTY_IS_C_VAR,
						0,
						ptr_variable
				};
			}else if(*CScriptClass::CONST_CHAR_PTR_TYPE_STR==ir_var->c_type){

				return {
						STK_PROPERTY_TYPE_STRING,
						ptr_variable,
						0
				};
			}else if(*CScriptClass::STRING_PTR_TYPE_STR==ir_var->c_type){

				return {
						STK_PROPERTY_TYPE_STRING|STK_PROPERTY_IS_C_VAR,
						(void *)((string *)ptr_variable)->c_str(),
						ptr_variable
				};
			}else if(*CScriptClass::BOOL_PTR_TYPE_STR==ir_var->c_type){
				return {
						STK_PROPERTY_TYPE_BOOLEAN|STK_PROPERTY_IS_C_VAR,
						0,
						ptr_variable
				};
			}else{
				CScriptClass *info_registered_class = GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(ir_var->c_type);//  CScriptClass::getInstance()->getRegisteredClassBy_C_ClassPtr(ir_var->c_type);

				if(info_registered_class){
					CScriptVariable *var = new CScriptVariable();
					var->init(info_registered_class,ptr_variable);

					return{
							STK_PROPERTY_TYPE_SCRIPTVAR|STK_PROPERTY_IS_C_VAR,
							NULL,
							var
					};
				}
		}
		}else{
			THROW_RUNTIME_ERROR("Variable %s is not c referenced as C symbol",ir_var->symbol_ref.c_str());
		}

		return{STK_PROPERTY_TYPE_UNDEFINED,
			0,
			NULL};
	}

	void CScriptClass::register_C_BaseSymbolsInt(bool _register){
		register_c_base_symbols = _register;
	}

	void CScriptClass::setVectorScriptClassNode(vector<CScriptClass *> 	* set_vec){
		vec_script_class_node = set_vec;
	}

	vector<CScriptClass *> 		*	CScriptClass::getVectorScriptClassNode(){
		return vec_script_class_node;
	}

	CScriptClass * CScriptClass::registerClass(short idxScope, const string & class_name, const string & base_class_name){
		unsigned char  index;
		CScriptClass *sci=NULL;


		if(vec_script_class_node->size()>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES);
			return NULL;
		}


		if(class_name.empty()){
			THROW_RUNTIME_ERROR("Class name empty");
			return NULL;
		}

		CScriptClass *base_class=NULL;
		if(base_class_name != ""){
			if((base_class = getScriptClass(base_class_name)) == NULL){
				return NULL;
			}
		}

		if((index = getIdxScriptClass_Internal(class_name))==ZS_INVALID_CLASS){ // check whether is local var registered scope ...

			// NEW SCOPE C and register ...
			unsigned char idxClass=(*vec_script_class_node).size()-1;

			sci = new CScriptClass(idxClass,idxScope);
			sci->classPtrType = TYPE_SCRIPT_VARIABLE;

			sci->symbol_info.symbol_ref = class_name;

			(*vec_script_class_node).push_back(sci);

			if(base_class != NULL){
				sci->idxBaseClass.push_back(base_class->idxClass);
			}

			return sci;

		}else{
			THROW_RUNTIME_ERROR("class \"%s\" already registered",class_name.c_str());
		}

		return NULL;
	}

	const char * CScriptClass::getMetamethod(METAMETHOD_OPERATOR op){

		switch (op) {
		case	EQU_METAMETHOD:		return  "_equ";  // ==
		case	NOT_EQU_METAMETHOD: return  "_nequ";  // !=,
		case	LT_METAMETHOD:		return  "_lt";  // <
		case	LTE_METAMETHOD:		return  "_lte";  // <=
		case	NOT_METAMETHOD:		return  "_not"; // !
		case	GT_METAMETHOD:		return  "_gt";  // >
		case	GTE_METAMETHOD:		return  "_gte"; // >=

		case	NEG_METAMETHOD:		return  "_neg"; // -a, !a
		case	ADD_METAMETHOD:		return  "_add"; // +
		case	DIV_METAMETHOD:		return  "_div"; // /
		case	MUL_METAMETHOD:		return  "_mul"; // *
		case	MOD_METAMETHOD:		return  "_mod";  // %
		case	AND_METAMETHOD:		return  "_and"; // binary and
		case	OR_METAMETHOD:		return  "_or"; //   binary or
		case	XOR_METAMETHOD:		return  "_xor"; // binary xor
		case	SHL_METAMETHOD:		return  "_shl"; // binary shift left
		case	SHR_METAMETHOD:		return  "_shr"; // binary shift right
		case	SET_METAMETHOD:		return  "_set"; // set
		default:
			return "none";
		}

		return "none";
	}

	vector<CScriptClass *> * CScriptClass::getVecScriptClassNode(){
		return vec_script_class_node;
	}

	map<int, map<int, fntConversionType>>  *	 CScriptClass::getMapTypeConversion() {
		return mapTypeConversion;
	}

	CScriptClass 	* CScriptClass::getScriptClass(unsigned char idx){
		if(idx == ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR("CScriptClass node out of bound");
			return NULL;
		}

		return vec_script_class_node->at(idx);
	}

	CScriptClass 	* CScriptClass::getScriptClass(const string & class_name){
		unsigned char index;
		if((index = getIdxScriptClass_Internal(class_name))!=ZS_INVALID_CLASS){ // check whether is local var registered scope ...
			return vec_script_class_node->at(index);
		}

		return NULL;
	}

	CScriptClass *CScriptClass::getScriptClassBy_C_ClassPtr(const string & class_type){

		for(unsigned i = 0; i < vec_script_class_node->size(); i++){
			if(class_type == vec_script_class_node->at(i)->classPtrType){//metadata_info.object_info.symbol_info.c_type){
				return vec_script_class_node->at(i);
			}
		}
		return NULL;
	}

	unsigned char CScriptClass::getIdxScriptClass_Internal(const string & class_name){

		for(unsigned i = 0; i < vec_script_class_node->size(); i++){
			if(class_name == vec_script_class_node->at(i)->symbol_info.symbol_ref){
				return i;
			}
		}
		return ZS_INVALID_CLASS;
	}

	unsigned char CScriptClass::getIdxScriptClass(const string & v){
		return getIdxScriptClass_Internal(v);
	}

	bool CScriptClass::isClassRegistered(const string & v){
		return getIdxScriptClass_Internal(v) != ZS_INVALID_CLASS;
	}

	bool CScriptClass::is_c_class(){

		 return ((symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) != 0);
	}
	//------------------------------------------------------------
	void CScriptClass::setPrintOutCallback(void (* _printout_callback)(const char *)){
		print_out_callback=_printout_callback;
	}
	 void  CScriptClass::print(const char *s){

		printf("%s\n",s);
		fflush(stdout);

		if(print_out_callback){
			print_out_callback(s);
		}
	 }

	 void  internal_print_error(const char *s){
		 CURRENT_VM->setError(s);
	 }

	 bool CScriptClass::initStaticVars(){

			if(vec_script_class_node == NULL){
				THROW_RUNTIME_ERROR("vector class factory not set");
				return false;
			}

			if(vec_script_class_node->size() != 0){ // must be 0 in order to register basic types...
				THROW_RUNTIME_ERROR("vector class factory not 0");
				return false;
			}

			CScriptClass *main_class=NULL;
			mapTypeConversion = new map<int, map<int, fntConversionType>>();

			VOID_TYPE_STR = new string(typeid(void).name());
			INT_PTR_TYPE_STR = new string(typeid(int *).name());
			FLOAT_PTR_TYPE_STR = new string(typeid(float *).name());
			CONST_CHAR_PTR_TYPE_STR = new string(typeid(const char *).name());
			STRING_PTR_TYPE_STR = new string(typeid(string *).name());
			BOOL_PTR_TYPE_STR = new string(typeid(bool *).name());

			// particular case for return ... for not to force user returning pointer (this is the only vars allowed, sorry!)
			BOOL_TYPE_STR = new string(typeid(bool).name());
			INT_TYPE_STR = new string(typeid(int).name());
			UNSIGNED_INT_TYPE_STR = new string(typeid(unsigned int).name());
			INTPTR_T_TYPE_STR = new string(typeid(intptr_t).name());
			FLOAT_TYPE_STR = new string(typeid(float).name());
			STACK_ELEMENT_PTR= new string(typeid(tStackElement *).name());

			//------------------------------------------------------------------------------------------------------------------------------------------------------------------
			//IDX_CLASS_MAIN=0, 	// Main class ...
			//IDX_CLASS_UNDEFINED,	// 1
			//IDX_CLASS_VOID,			// 2
			//IDX_CLASS_NULL,			// 3
			//IDX_CLASS_SCRIPT_VAR, 	// 4 script base that all object derive from it...
			//IDX_PRIMITIVE_INTEGER, 	  	// 5 then our basics types ...
			//IDX_PRIMITIVE_FLOAT,     	// 6
			//IDX_CLASS_STRING,     	// 7
			//IDX_CLASS_BOOLEAN,		// 8
			//IDX_CLASS_VECTOR,		// 9
			//IDX_CLASS_FUNCTOR,		// 10
			//IDX_CLASS_STRUCT,		// 11

			// register basic types...
			if((registerClass("void","",NULL)) == NULL) return false;		// 0
			vec_script_class_node->at(IDX_CLASS_VOID_C)->classPtrType=*VOID_TYPE_STR;

			if((registerClass("int *","",NULL)) == NULL) return false;		// 1
			vec_script_class_node->at(IDX_CLASS_INT_PTR_C)->classPtrType=*INT_PTR_TYPE_STR;

			if((registerClass("float *","",NULL)) == NULL) return false;		// 2
			vec_script_class_node->at(IDX_CLASS_FLOAT_PTR_C)->classPtrType=*FLOAT_PTR_TYPE_STR;


			if((registerClass("const char *","",NULL)) == NULL) return false;		// 3
			vec_script_class_node->at(IDX_CLASS_CONST_CHAR_PTR_C)->classPtrType=*CONST_CHAR_PTR_TYPE_STR;

			if((registerClass("string *","",NULL)) == NULL) return false;		// 4
			vec_script_class_node->at(IDX_CLASS_STRING_PTR_C)->classPtrType=*STRING_PTR_TYPE_STR;

			if((registerClass("bool *","",NULL)) == NULL) return false;		// 5
			vec_script_class_node->at(IDX_CLASS_BOOL_PTR_C)->classPtrType=*BOOL_PTR_TYPE_STR;

			if((registerClass("int","",NULL)) == NULL) return false;		// 6
			vec_script_class_node->at(IDX_CLASS_INT_C)->classPtrType=*INT_TYPE_STR;

			if((registerClass("unsigned int","",NULL)) == NULL) return false;		// 7
			vec_script_class_node->at(IDX_CLASS_UNSIGNED_INT_C)->classPtrType=*UNSIGNED_INT_TYPE_STR;

			if((registerClass("intptr_t","",NULL)) == NULL) return false;		// 8
			vec_script_class_node->at(IDX_CLASS_INTPTR_T_C)->classPtrType=*INTPTR_T_TYPE_STR;

			if((registerClass("float","",NULL)) == NULL) return false;		// 9
			vec_script_class_node->at(IDX_CLASS_FLOAT_C)->classPtrType=*FLOAT_TYPE_STR;

			if((registerClass("bool","",NULL)) == NULL) return false;		// 10
			vec_script_class_node->at(IDX_CLASS_BOOL_C)->classPtrType=*BOOL_TYPE_STR;


			// register basic classes...
			if((main_class=registerClass(MAIN_SCRIPT_CLASS_NAME,"",NULL)) == NULL) return false; // 11
			if((main_class->registerFunctionMember(MAIN_SCRIPT_FUNCTION_OBJECT_NAME)) == NULL) return false;

			REGISTER_BASIC_TYPE(tStackElement,IDX_STACK_ELEMENT);


			//------------------------------------------------------------------------------------------------------------------------------------------------------------------

			// rgister basic classes (Warning!!! must match idx !!! and the order is important!!!)
			REGISTER_BASIC_TYPE(CScriptVariable,IDX_CLASS_SCRIPT_VAR);
			REGISTER_BASIC_TYPE(CStringScriptVariable,IDX_CLASS_STRING);
			REGISTER_BASIC_TYPE(CVectorScriptVariable,IDX_CLASS_VECTOR);
			REGISTER_BASIC_TYPE(CFunctorScriptVariable,IDX_CLASS_FUNCTOR);
			REGISTER_BASIC_TYPE(CStructScriptVariable,IDX_CLASS_STRUCT);
			//----------------------------------------------------------------------
			// From here you defined all basic, start define hierarchy

			// register custom functions ...
			if(!class_C_baseof<CVectorScriptVariable,CScriptVariable>()) return false;
			if(!class_C_baseof<CFunctorScriptVariable,CScriptVariable>()) return false;
			if(!class_C_baseof<CStructScriptVariable,CScriptVariable>()) return false;


			//------------------------------------------------------------------------------------------------------------
			// Let's register functions,...
			// register c function's
			if(!register_C_Function("print",print)) return false;


			if(!register_C_Function("error",internal_print_error)) return false;


			if(!register_C_FunctionMember<CVectorScriptVariable>("size",&CVectorScriptVariable::size)) return false;
			if(!register_C_FunctionMember<CVectorScriptVariable>("push",static_cast<void (CVectorScriptVariable:: *)(tStackElement *)>(&CVectorScriptVariable::push))) return false;
			if(!register_C_FunctionMember<CVectorScriptVariable>("pop",&CVectorScriptVariable::pop)) return false;


			if(!register_C_FunctionMember<CStructScriptVariable>("add",&CStructScriptVariable::add_attr)) return false;
			if(!register_C_FunctionMember<CStructScriptVariable>("remove",&CStructScriptVariable::remove_attr)) return false;
			if(!register_C_FunctionMember<CStructScriptVariable>("size",&CStructScriptVariable::size)) return false;
			return true;
	 }

	 void CScriptClass::destroyStaticVars(){


		// we have to destroy all allocated constructor/destructor ...
		for (unsigned i = 0; i < vec_script_class_node->size(); i++) {

			CScriptClass *irv = vec_script_class_node->at(i);

			if ((irv->symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF) {

				//zs_print_debug_cr("* Erasing c destructor/contructor %s...", irv->classPtrType.c_str());

				if (irv->c_constructor) {
					delete irv->c_constructor;
				}

				if (irv->c_destructor) {
					delete irv->c_destructor;
				}

				// delete CScriptClass


				//
			}

			delete irv;
		}

		vec_script_class_node->clear();
		delete vec_script_class_node;
		vec_script_class_node=NULL;


		 if(VOID_TYPE_STR != NULL){
			delete VOID_TYPE_STR;
		 }

		 if(INT_PTR_TYPE_STR != NULL){
			 delete INT_PTR_TYPE_STR;
		 }

		 if(FLOAT_PTR_TYPE_STR != NULL){
			 delete FLOAT_PTR_TYPE_STR;
		 }

		 if(CONST_CHAR_PTR_TYPE_STR != NULL){
			 delete CONST_CHAR_PTR_TYPE_STR;
		 }

		 if(STRING_PTR_TYPE_STR != NULL){
			 delete STRING_PTR_TYPE_STR;
		 }

		 if(BOOL_PTR_TYPE_STR != NULL){
			 delete BOOL_PTR_TYPE_STR;
		 }

		 if (BOOL_TYPE_STR != NULL) {
			 delete BOOL_TYPE_STR;
		 }
		 
		 if (INT_TYPE_STR != NULL) {
			 delete INT_TYPE_STR;
		 }
		 
		 if (UNSIGNED_INT_TYPE_STR != NULL) {
			 delete UNSIGNED_INT_TYPE_STR;
		 }

		 if (INTPTR_T_TYPE_STR != NULL) {
			 delete INTPTR_T_TYPE_STR;
		 }

		 if (FLOAT_TYPE_STR != NULL) {
			 delete FLOAT_TYPE_STR;
		 }
		 
		 if (STACK_ELEMENT_PTR != NULL) {
			 delete STACK_ELEMENT_PTR;
		 }

		 if (mapTypeConversion != NULL) {
			 delete mapTypeConversion;
		 }

		 VOID_TYPE_STR=NULL;
		 INT_PTR_TYPE_STR=NULL;
		 FLOAT_PTR_TYPE_STR=NULL;
		 CONST_CHAR_PTR_TYPE_STR=NULL;
		 STRING_PTR_TYPE_STR=NULL;
		 BOOL_PTR_TYPE_STR=NULL;
		 BOOL_TYPE_STR = NULL;
		 INT_TYPE_STR = NULL;
		 UNSIGNED_INT_TYPE_STR = NULL;
		 INTPTR_T_TYPE_STR=NULL;
		 FLOAT_TYPE_STR = NULL;
		 STACK_ELEMENT_PTR = NULL;
		 mapTypeConversion = NULL;
	 }

	 CScriptClass::CScriptClass(unsigned char _idxClass, short _idxScope):CCommonClassFunctionData(_idxClass,_idxScope){

			classPtrType="";
			c_destructor = NULL;
			c_constructor=NULL;
			idx_function_script_constructor=ZS_INVALID_CLASS;
	}

	/*int getNumberArgsfromFunctionRefNode(PASTNode ast_node){
		if(ast_node->node_type == NODE_TYPE::FUNCTION_REF_NODE){ // function

			ast_node = AST_NODE(ast_node->idxAstParent);

			if(ast_node == NULL){
				THROW_RUNTIME_ERROR("internal error, parent null");
				return false;
			}

			if(ast_node->node_type != NODE_TYPE::CALLING_OBJECT_NODE){
				THROW_RUNTIME_ERROR("internal error, expected calling object node");
				return false;
			}

			if(ast_node->children.size()!=2){
				THROW_RUNTIME_ERROR("internal error, expected calling function 2 children");
				return false;
			}

			ast_node = AST_NODE(ast_node->children[1]);

			if(ast_node == NULL){
				THROW_RUNTIME_ERROR("internal error, expected args node");
				return false;
			}

			if(ast_node->node_type != NODE_TYPE::ARGS_PASS_NODE){
				THROW_RUNTIME_ERROR("internal error, espected ARGS PASS NODE");
				return false;
			}

			return ast_node->children.size();
		}else{
			THROW_RUNTIME_ERROR("internal error, espected FUNCTION_REF_NODE");
		}
		return -1;
	}

	int getNumberArgsfromFunctionObjectNode(PASTNode ast_node){
		if(ast_node->node_type == NODE_TYPE::FUNCTION_OBJECT_NODE){ // function

			if(ast_node->children.size()!=2){
				THROW_RUNTIME_ERROR("internal error, expected calling function 1 children");
				return false;
			}

			ast_node = AST_NODE(ast_node->children[0]);

			if(ast_node == NULL){
				THROW_RUNTIME_ERROR("internal error, expected args node");
				return false;
			}

			if(ast_node->node_type != NODE_TYPE::ARGS_DECL_NODE){
				THROW_RUNTIME_ERROR("internal error, espected ARGS PASS NODE");
				return false;
			}

			return ast_node->children.size();
		}else{
			THROW_RUNTIME_ERROR("internal error, espected FUNCTION_OBJECT_NODE");
		}

		return -1;
	}*/
#if 0
	bool CScriptClass::searchVarFunctionSymbol(CCommonClassFunctionData * scope_info, tInstruction *iao, int current_function, bool & symbol_not_found, unsigned int param_scope_type){

		int idx=0;
		symbol_not_found = true;
		char n_args_to_find =-1;
		PASTNode ast_node = AST_NODE(iao->idxAstNode);
		string symbol_to_find =ast_node->symbol_value;//CCompiler::makeSymbolRef(ast_node->symbol_value, ast_node->idxScope);

		if(ast_node->node_type == NODE_TYPE::FUNCTION_REF_NODE){ // function

			n_args_to_find = getNumberArgsfromFunctionRefNode(ast_node);
		}

		unsigned int iao_scope = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);

		if(iao_scope & INS_PROPERTY_SUPER_SCOPE){ // try deduce local/global

			if(current_function > 0){ // minimum have to have a 1

				CScriptFunction *sfo =  GET_SCRIPT_FUNCTION(scope_info->local_symbols.function[current_function]);//.object_info;
				symbol_to_find = sfo->symbol_info.symbol_ref;
				short idxScope=CCompiler::getIdxScopeFromSymbolRef(symbol_to_find);

				if(idxScope == -1){
					return false;
				}

				int idx_super=-1;
				bool is_c=false;

				for(int i = current_function-1; i >= 0 && idx_super==-1; i--){
					CScriptFunction * sfo = GET_SCRIPT_FUNCTION(scope_info->local_symbols.function[i]);
					string symbol_ref=CCompiler::getSymbolNameFromSymbolRef(sfo->symbol_info.symbol_ref);
					string symbol_ref_to_find=CCompiler::getSymbolNameFromSymbolRef(symbol_to_find);

					if((symbol_ref == symbol_ref_to_find) && ((int)sfo->m_arg.size() == n_args_to_find)){ // match name and args ...
						is_c = (sfo->symbol_info.properties & PROPERTY_C_OBJECT_REF) != 0;
						idx_super=i;
					}
				}

				if(idx_super!= -1){
					 REMOVE_SCOPES(iao->instruction_properties);
					 iao->instruction_properties |= param_scope_type;
					 iao->index_op1 = LOAD_TYPE_FUNCTION;
					 iao->index_op2 = idx_super;
					 iao->instruction_properties |= (is_c ? INS_PROPERTY_DEDUCE_C_CALL : 0);
					 //iao->idxFunction = idxFunction;
					 return true;

				}
				else{ // symbol not found ...
					return false;
				}
			}
			else{
				return false;
			}
		}
		else{

			int idx_scope=ast_node->idxScope;
			bool partial_c_class= false;

			if(info_function->symbol_info.idxClass != ZS_INVALID_CLASS){
				CScriptClass *sc=CScriptClass::getScriptClassByIdx(info_function->symbol_info.idxClass);

				if(symbol_to_find == "this" && (iao_scope & INS_PROPERTY_THIS_SCOPE)){ // trivial is the first symbol we find...
					 REMOVE_SCOPES(iao->instruction_properties);
					 iao->instruction_properties |= INS_PROPERTY_THIS_SCOPE;
					 iao->index_op1 = LOAD_TYPE_VARIABLE;
					 iao->index_op2 = ZS_THIS_IDX;
					 return true;
				}

				 partial_c_class=(info_function->symbol_info.properties&SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) !=0;
				while( sc->idxBaseClass.size()>0 && !partial_c_class){

					sc=CScriptClass::getScriptClassByIdx(sc->idxBaseClass[0]); // get base class...
					partial_c_class|=(sc->metadata_info.object_info.symbol_info.properties&SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) !=0;
				}

				if(iao_scope & INS_PROPERTY_THIS_SCOPE){ // start from class scope to find its variable/function member...
					sc=CScriptClass::getScriptClassByIdx(info_function->symbol_info.idxClass);
					idx_scope=AST_NODE(sc->metadata_info.object_info.symbol_info.idxAstNode)->idxScope;
				}
			}


			bool variable_in_main_class=SCOPE_IN_MAIN_CLASS(ast_node->idxScope);

			 bool end=false;


			 while(!end){ // we try all scopes until match symbols at right scope...

				 symbol_to_find=CCompiler::makeSymbolRef(ast_node->symbol_value,idx_scope);

				 if((idx=CScriptFunction::getIdxFunctionObject(info_function,symbol_to_find,n_args_to_find,false))!=-1){
					 REMOVE_SCOPES(iao->instruction_properties);
					 iao->instruction_properties |= param_scope_type;
					 iao->index_op1 = LOAD_TYPE_FUNCTION;
					 iao->index_op2 = idx;
					 if((GET_FUNCTION_INFO(info_function->local_symbols.function[idx])->symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF){ // set as -1 to search the right signature ...
						 iao->index_op2 = -1;
					 }
					 return true;
				 }else {

					 if((idx=CScriptFunction::getIdxVariableSymbol(info_function,symbol_to_find, false))!=-1){
						 REMOVE_SCOPES(iao->instruction_properties);
						 iao->instruction_properties |= param_scope_type;
						 iao->index_op1 = LOAD_TYPE_VARIABLE;
						 iao->index_op2 = idx;
						 return true;
					 }
				 }

			 	if(idx_scope >= 0){ // var local or class member ?
					 CScope *sc=SCOPE_NODE(idx_scope);
				 	 idx_scope=sc->getIdxParent();
			 	}

			 	if(idx_scope<0){

			 		idx_scope=ZS_UNDEFINED_IDX;

			 		if(partial_c_class){ // var in c scope ?

			 			idx_scope = IDX_C_CLASS_SCOPE;
			 			partial_c_class=false; // <-- set it false to tell that is already test

			 		}else{ // finally try global...

						 if(!variable_in_main_class){ // try global scope...
							 variable_in_main_class=true;
							 CScriptFunction *main=MAIN_SCRIPT_FUNCTION_OBJECT;
							 info_function=&main->object_info; // set main function as other...
							 idx_scope=0; // set scope global and try last...
							 param_scope_type=0; // override to global scope...
						 }
			 		}
			 	 }

			 	 end = idx_scope == ZS_UNDEFINED_IDX;


			 }
		}
		symbol_not_found = false;
		 return false;
	}

	void CScriptClass::buildScopeVariablesBlock(CScriptFunction *root_class_irfs ){
		/// PRE: base_class_irfs must be info of root class.
		 bool is_main_function = MAIN_SCRIPT_FUNCTION_OBJECT == root_class_irfs;
		 PASTNode ast = AST_NODE(root_class_irfs->object_info.symbol_info.idxAstNode);

		 if(ast!=NULL){

			 int idxScope =ast->idxScope;
			 if(!is_main_function) {// is not main function

				 if(ast->node_type == NODE_TYPE::KEYWORD_NODE){
					 if(ast->keyword_info == KEYWORD_TYPE::FUNCTION_KEYWORD ){
						 idxScope = AST_NODE(ast->children[1])->idxScope; // pass scope block ...
					 }
				 }
			 }

			 if(idxScope != -1){

				 struct tInfoVarScopeBlockRegister{
					 int idxScope;
					 vector<int> var_index;
				 };

				 vector<CScope *> *list = CScope::getVectorScopeNode();
				 vector<tVariableSymbolInfo> *vs = &root_class_irfs->object_info.local_symbols.variable;
				 vector<tInfoVarScopeBlockRegister> vec_ivsb;
				 for(unsigned i = 0;i < list->size(); i++){ // register index var per scope ...
					 tInfoVarScopeBlockRegister ivsb;

					 ivsb.idxScope = list->at(i)->idxScope;

					 for(unsigned v = 0;v < vs->size(); v++){ // register index var per scope ...

						 PASTNode ast_var = AST_NODE(vs->at(v).idxAstNode);

						 if(ast_var !=NULL){

							if(ast_var->idxScope == ivsb.idxScope){
								ivsb.var_index.push_back(v);
							}
						 }
					 }
					 vec_ivsb.push_back(ivsb);
				 }

				 root_class_irfs->scope_info.lut_scope_symbol = (tInfoVarScopeBlock*)malloc(vec_ivsb.size()*sizeof(tInfoVarScopeBlock));
				 root_class_irfs->object_info.n_lut_scope_symbols = vec_ivsb.size();

				 for(unsigned i = 0; i < vec_ivsb.size(); i++){
					 root_class_irfs->scope_info.lut_scope_symbol[i].idxScope = vec_ivsb[i].idxScope;
					 root_class_irfs->scope_info.lut_scope_symbol[i].n_var_index = (char)vec_ivsb[i].var_index.size();
					 root_class_irfs->scope_info.lut_scope_symbol[i].var_index = (int *)malloc(sizeof(int)*vec_ivsb[i].var_index.size());
					 for(unsigned j = 0; j < vec_ivsb[i].var_index.size(); j++){
						 root_class_irfs->scope_info.lut_scope_symbol[i].var_index[j] = vec_ivsb[i].var_index[j];
					 }
				 }
			 }
		 }
	}



	bool CScriptClass::updateReferenceSymbols(){


		 int idx_main_function = ((*vec_script_class_node)[IDX_START_SCRIPTVAR]->scope_info.local_symbols.function[0]);
		 CScriptFunction  *main_function = GET_SCRIPT_FUNCTION((*vec_script_class_node)[IDX_START_SCRIPTVAR]->scope_info.local_symbols.function[0]);
		 zs_print_debug_cr("DEFINED CLASSES");
		 vector<int>  mrf; // aux vector for collect function obj info.
		 bool symbol_found = false;

		 // For each class (after primitives)...
		 for(unsigned i = IDX_START_SCRIPTVAR; i < (*vec_script_class_node).size(); i++){

			 mrf.clear();
			 if(i==IDX_START_SCRIPTVAR){ // First entry (MAIN_CLASS), load global functions....
				 mrf.push_back(idx_main_function);//->object_info.local_symbols.function;
				 for(unsigned h=0; h<  main_function->scope_info.local_symbols.function.size(); h++){
					 mrf.push_back(main_function->scope_info.local_symbols.function[h]);
				 }
			 }else{ // any other class
				 for(unsigned h=0; h<  (*vec_script_class_node)[i]->scope_info.local_symbols.function.size(); h++){
					 mrf.push_back((*vec_script_class_node)[i]->scope_info.local_symbols.function[h]);
				 }
			 }

			 // For each function on the class ...
			 for(unsigned k=0; k < mrf.size();k++){


				 CScriptFunction * info_function = GET_SCRIPT_FUNCTION(mrf[k]);
				 bool is_main_class = i == IDX_CLASS_MAIN;
				 bool is_main_function = is_main_class && k==0;
				 CScriptClass * _belonging_class = (*vec_script_class_node)[i];

				 buildScopeVariablesBlock(info_function);

				 if(is_main_class){
					 if(is_main_function){
						 zs_print_debug_cr("Main Function");
					 }
					 else{
						 zs_print_debug_cr("Global function %s...",info_function->symbol_info.symbol_ref.c_str());
					 }
				 }else{
					 zs_print_debug_cr("Function %s::%s...",_belonging_class->metadata_info.symbol_info.symbol_ref.c_str(),info_function->symbol_info.symbol_ref.c_str());
				 }

				 if(info_function->object_info.asm_op != NULL){

					 int idx_op = 0;
					 for(tInstruction *iao=info_function->asm_op; iao->operator_type!=END_FUNCTION; iao++,idx_op++){

						 if(iao->operator_type==OP_CODE::LOAD){

							 string base_class = _belonging_class->symbol_info.symbol_ref;

							 CCommonClassFunctionData *sfi=NULL;
							 unsigned int scope_type = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);

							 if(scope_type & INS_PROPERTY_ACCESS_SCOPE ){
								 iao->index_op1 = LOAD_TYPE_VARIABLE;
								 iao->index_op2 = -1; // keep unknow reference for first time ..
							 }
							 else {

								 if((scope_type & INS_PROPERTY_THIS_SCOPE) | (scope_type & INS_PROPERTY_SUPER_SCOPE)){

									 sfi = &_belonging_class->scope_info;
								 }

								 if(iao->index_op1 == LOAD_TYPE_NOT_DEFINED){
									 PASTNode iao_node = AST_NODE(iao->idxAstNode);

									 string symbol_to_find =iao_node->symbol_value;

									 if(scope_type & INS_PROPERTY_ACCESS_SCOPE){

										 writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"Symbol \"%s\" will solved at run-time", AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode));

									 }else{

										// this/super required ...
										 if(sfi != NULL){
											 // make ref var from class scope ...
											 // search global...
											 if(scope_type & INS_PROPERTY_SUPER_SCOPE){
												 symbol_to_find = GET_SCRIPT_FUNCTION(sfi->local_symbols.function[k])->symbol_info.symbol_ref;
											 }

											 if(!searchVarFunctionSymbol(sfi,iao,k,symbol_found,scope_type)){
												 if(scope_type & INS_PROPERTY_SUPER_SCOPE){
													 PASTNode ast_node = AST_NODE(iao->idxAstNode);
													 int n_args_to_find = getNumberArgsfromFunctionRefNode(ast_node);
													 string arg_str="";
													 for(int i = 0; i < n_args_to_find; i++){
														 if(i>0){
															 arg_str+=",";
														 }
														 arg_str+="arg"+CZetScriptUtils::intToString(i);
													 }
													 writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"Cannot find \"super\" function for \"%s(%s)\". Has the class an inheritance or parent has the function ?", symbol_to_find.c_str(),arg_str.c_str());
												 }
												 else{
													 writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"Symbol \"%s::%s\" not found", base_class.c_str(),symbol_to_find.c_str());
												 }
												 return false;
											 }
										 }
										 else{ //normal symbol...
											 string symbol_to_find =CCompiler::makeSymbolRef(iao_node->symbol_value,iao_node->idxScope);
											 // search local...
											 if(!searchVarFunctionSymbol(&info_function->scope_info,iao,k,symbol_found,INS_PROPERTY_LOCAL_SCOPE)){

												 // search global...
												 CScriptFunction * mainFunctionInfo = MAIN_SCRIPT_FUNCTION_OBJECT;
												 if(!searchVarFunctionSymbol(&mainFunctionInfo->scope_info,iao,k,symbol_found,0)){
														PASTNode ast_node = AST_NODE(iao->idxAstNode);

														if(ast_node->node_type == NODE_TYPE::FUNCTION_REF_NODE){ // function

															if(!symbol_found){
																writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"function \"%s\" not registered", iao_node->symbol_value.c_str() );
															}
															else{
																writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"Cannot match function \"%s\" with %i args",iao_node->symbol_value.c_str(),getNumberArgsfromFunctionRefNode(ast_node) );
															}

														}
														else{
															writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"Symbol \"%s\" not found",iao_node->symbol_value.c_str());
														}
													 return false;
												 }
											 }
										}
									}
								 }
							 }

						 }else  if(iao->operator_type==OP_CODE::CALL){ // overrides variable type as function ...
							 // check whether access scope ...
							 if(info_function->asm_op[iao->index_op2].operator_type ==OP_CODE::LOAD  && // e
								(GET_INS_PROPERTY_SCOPE_TYPE(info_function->asm_op[iao->index_op2].instruction_properties) & INS_PROPERTY_ACCESS_SCOPE)){

								 info_function->asm_op[iao->index_op2].index_op1 = LOAD_TYPE_FUNCTION;
								 info_function->asm_op[iao->index_op2].index_op2 = -1;
							 }
						 }
					 }

				 }

				 if(!is_main_function){ // process all function symbols ...
					 for(unsigned m=0; m < info_function->scope_info.local_symbols.function.size(); m++){
						 if(!updateFunctionSymbols(
								 info_function->scope_info.local_symbols.function[m],
								 info_function->symbol_info.symbol_ref,m)){
							 return false;
						 }
					 }
				 }
			 }

			 // check whether class has aritmethic metamethod but it hasn't a set metamethod....
			 bool it_has_any_metamethod=false;

			 for(int m = 0; m < MAX_METAMETHOD_OPERATORS && !it_has_any_metamethod; m++){
				 if((*vec_script_class_node)[i]->metamethod_operator[m].size()>0){
					 it_has_any_metamethod=true;
				 }
			 }

			 if(it_has_any_metamethod && ((*vec_script_class_node)[i]->metamethod_operator[SET_METAMETHOD].size()==0)){
				 fprintf(stderr,"class \"%s\" it has some metamethods but it doesn't have _set metamethod (aka '='). The variable will be override on assigment.\n"
						 ,(*vec_script_class_node)[i]->symbol_info.symbol_ref.c_str()
				);
			 }
		 }

		 // update global variables, only C refs...
		 for(unsigned i = 0;i < main_function->scope_info.local_symbols.variable.size();i++){
			 if(main_function->scope_info.local_symbols.variable[i].properties & PROPERTY_C_OBJECT_REF){
				 CURRENT_VM->iniStackVar(
						 i,

						 CScriptClass::C_REF_InfoVariable_2_StackElement(
								 	&main_function->scope_info.local_symbols.variable[i],
									(void *)main_function->scope_info.local_symbols.variable[i].ref_ptr
				 ));
			 }
		 }
		 // finally update constant symbols ...
		 return true;
	}
#endif

	CScriptVariable *		CScriptClass::instanceScriptVariableByClassName(const string & class_name){

		 // 0. Search class info ...
		 CScriptClass * rc = getScriptClass(class_name);

		 if(rc != NULL){
			 return instanceScriptVariable(rc->idxClass);
		 }

		 return NULL;
	 }

	 CScriptVariable 		 * CScriptClass::instanceScriptVariableByIdx(unsigned char idx_class, void * value_object){

		 CScriptVariable *class_object=NULL;

		 // 0. Search class info ...
		 CScriptClass *rc = getScriptClass(idx_class);

		 if(rc != NULL){

			 // Is a primitive ?
			 switch(rc->idxClass){

			 case IDX_CLASS_VOID_C:
			 case IDX_CLASS_INT_PTR_C:
			 case IDX_CLASS_FLOAT_PTR_C:
			 case IDX_CLASS_STRING_PTR_C:
			 case IDX_CLASS_BOOL_PTR_C:
				 THROW_RUNTIME_ERROR("Internal error");
				 return NULL;
				 break;

			 case IDX_CLASS_VECTOR:
			 case IDX_CLASS_STRUCT:
				 class_object = (CScriptVariable *)value_object;
				 break;
			 default:
				 class_object = new CScriptVariable();
				 class_object->init(rc, value_object);
				 break;
			 }
		 }
		 return class_object;
	 }

	/**
	 * Register C variable
	 */
	 tVariableSymbolInfo *  CScriptClass::register_C_VariableInt(const string & var_name,void * var_ptr, const string & var_type)
	{
		//CScope *scope;
		tVariableSymbolInfo *irs;
		//int idxVariable;

		if(var_ptr==NULL){
			THROW_RUNTIME_ERROR("cannot register var \"%s\" with NULL reference value", var_name.c_str());
			return false;
		}

		CScriptFunction *main_function=MAIN_FUNCTION;

		if(main_function == NULL){
			THROW_RUNTIME_ERROR("main function is not created");
			return false;
		}

		if(getIdxClassFromIts_C_Type(var_type) == ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR("%s has not valid type (%s)",var_name.c_str(),var_type.c_str());
			return false;
		}

		if(!MAIN_SCOPE_NODE->registerSymbol(var_name)){
			return false;
		}

		if((irs = main_function->registerVariable(var_name,var_type,(intptr_t)var_ptr,::PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF)) != NULL){

			zs_print_debug_cr("Registered variable name: %s",var_name.c_str());

			return irs;
		}

		return NULL;
	}

	unsigned char CScriptClass::getIdx_C_RegisteredClass(const string & str_classPtr, bool throw_if_not_found){
			// ok check c_type
			for(unsigned i = 0; i < (*vec_script_class_node).size(); i++){
				if((*vec_script_class_node)[i]->classPtrType == str_classPtr){
					return i;
				}
			}

			if(throw_if_not_found){
				THROW_RUNTIME_ERROR("C class %s is not registered",str_classPtr.c_str());
			}

			return ZS_INVALID_CLASS;
	}

	/*tVariableSymbolInfo * CScriptClass::registerVariableSymbol(const string & class_name,const string & var_name){


		CScriptClass *rc = GET_SCRIPT_CLASS(class_name);// getIdxRegisteredClass(class_name);

		if(rc != NULL){//idxClass != -1){

			string symbol_ref = CEval::makeSymbolRef(var_name,rc->idxScope);

			if(rc->getVariable(symbol_ref) != NULL){
				writeErrorMsg(GET_INSTRUCTION_FILE_LINE(idxAstNode),"symbol variable \"%s::%s\" already exist",class_name.c_str(),var_name.c_str());
				return NULL;
			}


			CCommonClassFunctionData *scope_info=&rc->scope_info;

			tVariableSymbolInfo info_var;
			info_var.idxClass = rc->symbol_info.idxClass;
			info_var.idxAstNode = idxAstNode;
			info_var.symbol_ref =var_name;
			info_var.idxSymbol = (short)scope_info->local_symbols.variable.size();
			scope_info->local_symbols.variable.push_back(info_var);

			return &scope_info->local_symbols.variable[scope_info->local_symbols.variable.size()-1];

			return rc->registerVariable(var_name,)
		}else{
			writeErrorMsg(GET_INSTRUCTION_FILE_LINE(idxAstNode),"class \"%s\" not exist",class_name.c_str());
			return NULL;
		}

		return NULL;
	}*/


	CScriptFunction * CScriptClass::registerFunctionMember(const string & function_name, vector<tArgumentInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short properties){

		CScriptFunction * sf = registerFunction( function_name,  args,  idx_return_type, ref_ptr,  properties);
		string class_name=symbol_info.symbol_ref;

		if(function_name == class_name){
			idx_function_script_constructor = m_function.size();
		}


		// check if metamethod...
		for(int i = 0; i < MAX_METAMETHOD_OPERATORS; i++){
			if(STRCMP(getMetamethod((METAMETHOD_OPERATOR)i),==,function_name.c_str())){

				metamethod_operator[i].push_back(sf->idxScriptFunction);

				zs_print_debug_cr("Registered metamethod %s::%s",class_name.c_str(), function_name.c_str());
				break;
			}
		}
		return sf;

	}

	/*bool  CScriptClass::existFunctionMember(const string & function_name, int n_params){

		return CScriptFunction::getFunction(function_name, n_params) != ZS_UNDEFINED_IDX;
	}*/

	//-----

	bool CScriptClass::addArgumentFunctionSymbol(const string & class_name,const string & function_name, const string & arg_name){

		int idxScriptFunction = getIdxScriptFunctionObjectByClassFunctionName(class_name,function_name);

		if(idxScriptFunction!=-1){

			GET_SCRIPT_FUNCTION(idxScriptFunction)->m_arg.push_back({ZS_UNDEFINED_IDX,arg_name});
		}
		return false;
	}

	// internal var types ...
	CScriptClass *  CScriptClass::getRegisteredClassStruct(){
		return (*vec_script_class_node)[IDX_CLASS_STRUCT];
	}

	CScriptClass *  CScriptClass::getRegisteredClassString(){
		return (*vec_script_class_node)[IDX_CLASS_STRING];
	}

	intptr_t CScriptClass::doCast(intptr_t obj, unsigned char idx_src_class, unsigned char idx_convert_class){//c_class->idxClass,idx_return_type){

		CScriptClass *src_class = CScriptClass::getScriptClassx(idx_src_class);
		CScriptClass *convert_class = CScriptClass::getScriptClass(idx_convert_class);

		//local_map_type_conversion
		if(mapTypeConversion->count(idx_src_class) == 0){
			THROW_RUNTIME_ERROR("There's no type src conversion class \"%s\".",demangle(src_class->classPtrType).c_str());
			return 0;
		}

		if((*mapTypeConversion)[idx_src_class].count(idx_convert_class) == 0){
			THROW_RUNTIME_ERROR("There's no dest conversion class \"%s\".",demangle(convert_class->classPtrType).c_str());
			return 0;
		}

		return (*mapTypeConversion)[idx_src_class][idx_convert_class](obj);
	}

	CScriptClass *  CScriptClass::getRegisteredClassVector(){
		return (*vec_script_class_node)[IDX_CLASS_VECTOR];
	}

	CScriptClass *  CScriptClass::getRegisteredClassFunctor(){
		return (*vec_script_class_node)[IDX_CLASS_FUNCTOR];
	}

	const char * CScriptClass::getNameRegisteredClassByIdx(unsigned char idx){
		if(idx != ZS_INVALID_CLASS){
			return (*vec_script_class_node)[idx]->symbol_info.symbol_ref.c_str();
		}
		 return "class_unknow";
	}
}



