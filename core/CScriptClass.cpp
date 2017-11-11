/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zs_core.h"

namespace zetscript{

	#define REGISTER_BASIC_TYPE(type_class, idx_class)\
		if(!register_C_ClassInt<type_class>(STR(type_class))) return false;\
		if(vec_script_class_node->at(idx_class)->classPtrType!=typeid(type_class *).name()){\
			zs_print_error_cr("Error initializing basic type: %s",STR(type_class));\
			return false;\
		}

	vector<CScriptClass *> 				* CScriptClass::vec_script_class_node=NULL;

	string  * CScriptClass::VOID_TYPE_STR=NULL;// 		typeid(void).name()
	string  * CScriptClass::INT_PTR_TYPE_STR=NULL;//	typeid(int *).name()
	string  * CScriptClass::FLOAT_PTR_TYPE_STR=NULL;//	typeid(float *).name()
	string  * CScriptClass::STRING_PTR_TYPE_STR=NULL;//	typeid(string *).name()
	string  * CScriptClass::CONST_CHAR_PTR_TYPE_STR=NULL;//	typeid(string *).name()
	string  * CScriptClass::BOOL_PTR_TYPE_STR=NULL;//	typeid(bool *).name()

	string  * CScriptClass::INT_TYPE_STR=NULL;//	typeid(int *).name()
	string  * CScriptClass::FLOAT_TYPE_STR=NULL;//	typeid(int *).name()
	string  * CScriptClass::BOOL_TYPE_STR=NULL;//	typeid(bool *).name()
	string  * CScriptClass::STACK_ELEMENT_PTR=NULL;//	typeid(bool *).name()

	char CScriptClass::registered_metamethod[MAX_METAMETHOD_OPERATORS][50]={{0}};
	 //CScriptClass *  	 CScriptClass::scriptClassFactory=NULL;
	 //CScriptClass::tPrimitiveType CScriptClass::valid_C_PrimitiveType[MAX_C_TYPE_VALID_PRIMITIVE_VAR];

	// register metamethods str ...
	 //--obj , type convert, ---
	 map<string,map<string,fntConversionType>> CScriptClass::mapTypeConversion;



	int CScriptClass::getIdxClassFromIts_C_TypeInternal(const string & c_type_str){

		// 1. we have to handle primitives like void, (int *), (bool *),(float *) and (string *).
		 // 2. Check for rest registered C classes...
		 for(unsigned i = 0; i < vec_script_class_node->size(); i++){
			 if(vec_script_class_node->at(i)->classPtrType==c_type_str)
			 {
				 return i;
			 }
		 }

		 return -1;
	 }

	int 			CScriptClass::getIdxClassFromIts_C_Type(const string & c_type_str){
		return CScriptClass::getIdxClassFromIts_C_TypeInternal(c_type_str);
	}

	bool 	CScriptClass::idxClassInstanceofIdxClass(int idxSrcClass, int class_idx){


		if(idxSrcClass == class_idx){
			return true;
		}

		CScriptClass * theClass = vec_script_class_node->at(idxSrcClass);

		for(unsigned i=0; i <theClass->baseClass.size(); i++){
			if(idxClassInstanceofIdxClass(theClass->baseClass[i]->idxClass,class_idx)){
				return true;
			}
		}

		return false;
	}

	tStackElement CScriptClass::C_REF_InfoVariable_2_StackElement(tInfoVariableSymbol *ir_var, void *ptr_variable){

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
			zs_print_error_cr("Variable %s is not c referenced as C symbol",ir_var->symbol_name.c_str());
		}

		return{STK_PROPERTY_TYPE_UNDEFINED,
			0,
			NULL};
	}


	void CScriptClass::setVectorScriptClassNode(vector<CScriptClass *> 	* set_vec){
		vec_script_class_node = set_vec;
	}

	vector<CScriptClass *> 		*	CScriptClass::getVectorScriptClassNode(){
		return vec_script_class_node;
	}

	CScriptClass * CScriptClass::registerClass(const string & class_name, const string & base_class_name, PASTNode _ast){
		int index;
		CScriptClass *sci=NULL;

		if(class_name.empty()){
			zs_print_error_cr("Class name empty");
			return NULL;
		}

		CScriptClass *base_class=NULL;
		if(base_class_name != ""){
			if((base_class = getScriptClassByName(base_class_name)) == NULL){
				return NULL;
			}
		}

		if((index = getIdxScriptClass_Internal(class_name))==-1){ // check whether is local var registered scope ...

			//CScriptClass 	*registered_class=new CScriptClass;
			sci = new CScriptClass;
			sci->metadata_info.object_info.symbol_info.idxScriptClass = (short)vec_script_class_node->size();
			sci->classPtrType = TYPE_SCRIPT_VARIABLE;

			if(base_class != NULL){
				sci->baseClass.push_back(base_class);
			}

			sci->metadata_info.object_info.symbol_info.symbol_name = class_name;
			sci->metadata_info.object_info.symbol_info.idxAstNode=-1;

			if(_ast != NULL){
				sci->metadata_info.object_info.symbol_info.idxAstNode=_ast->idxAstNode;
			}

			(*vec_script_class_node).push_back(sci);
			sci->idxClass=(*vec_script_class_node).size()-1;

			return sci;

		}else{
			zs_print_error_cr("error class \"%s\" already registered at line %i!", class_name.c_str(), AST_LINE((*vec_script_class_node)[index]->metadata_info.object_info.symbol_info.idxAstNode));
		}

		return NULL;
	}

	CScriptClass 	* CScriptClass::getScriptClassByIdx(int idx){
		if(idx < 0 || (unsigned)idx >= vec_script_class_node->size()){
			zs_print_error_cr("CScriptClass node out of bound");
			return NULL;
		}

		return vec_script_class_node->at(idx);
	}

	CScriptClass 	* CScriptClass::getScriptClassByName(const string & class_name, bool print_msg){
		int index;
		if((index = getIdxScriptClass_Internal(class_name))!=-1){ // check whether is local var registered scope ...

			return vec_script_class_node->at(index);
		}else{
			if(print_msg){
				zs_print_error_cr("class \"%s\" doesn't exist!", class_name.c_str());
			}
		}

		return NULL;
	}

	CScriptClass *CScriptClass::getScriptClassBy_C_ClassPtr(const string & class_type, bool print_msg){

		for(unsigned i = 0; i < vec_script_class_node->size(); i++){
			if(class_type == vec_script_class_node->at(i)->metadata_info.object_info.symbol_info.c_type){
				return vec_script_class_node->at(i);
			}
		}

		if(print_msg){
			zs_print_error_cr("C class type \"%s\" is not registered", demangle(class_type).c_str());
		}

		return NULL;
	}

	int CScriptClass::getIdxScriptClass_Internal(const string & class_name){

		for(unsigned i = 0; i < vec_script_class_node->size(); i++){
			if(class_name == vec_script_class_node->at(i)->metadata_info.object_info.symbol_info.symbol_name){
				return i;
			}
		}
		return -1;
	}

	int CScriptClass::getIdxScriptClass(const string & v, bool print_msg){
		int index= getIdxScriptClass_Internal(v);

		if(index == -1 && print_msg){
			zs_print_error_cr("class %s not registered",v.c_str());
		}

		return index;
	}

	bool CScriptClass::isClassRegistered(const string & v){
		return getIdxScriptClass_Internal(v) != -1;
	}

	bool CScriptClass::is_c_class(){

		 return ((metadata_info.object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) != 0);
	}
	//------------------------------------------------------------
	fntConversionType CScriptClass::getConversionType(string objectType, string conversionType, bool show_errors){

		if(mapTypeConversion.count(objectType) == 0){
			if(show_errors){
				zs_print_error_cr("There's no type conversion \"%s\". Add conversion types through \"addPrimitiveTypeConversion\" function",demangle(objectType).c_str());
			}
			return NULL;
		}

		if(mapTypeConversion[objectType].count(conversionType) == 0){
			if(show_errors){
				zs_print_error("There's no CONVERSION from type \"%s\" to type \"%s\"",demangle(objectType).c_str(),demangle(conversionType).c_str());
				printf("\n\tAvailable types are:\n");
				for(map<string, fntConversionType>::iterator j =mapTypeConversion[objectType].begin() ; j != mapTypeConversion[objectType].end();j++){
					printf("\t\t* \"%s\"\n", demangle(j->first).c_str());
				}
			}
			return NULL;
		}
		return mapTypeConversion[objectType][conversionType];
	 }

	 void  print(const char *s){

		printf("%s\n",s);
		fflush(stdout);
	 }

	 void  internal_print_error(const char *s){
		 CURRENT_VM->setError(s);
		//printf("%s\n",s);
		//fflush(stderr);
	 }

	 bool CScriptClass::init(){

			if(vec_script_class_node == NULL){
				zs_print_error_cr("vector class factory not set");
				return false;
			}

			if(vec_script_class_node->size() != 0){ // must be 0 in order to register basic types...
				zs_print_error_cr("vector class factory not 0!");
				return false;
			}

			VOID_TYPE_STR = new string(typeid(void).name());
			INT_PTR_TYPE_STR = new string(typeid(int *).name());
			FLOAT_PTR_TYPE_STR = new string(typeid(float *).name());
			CONST_CHAR_PTR_TYPE_STR = new string(typeid(const char *).name());
			STRING_PTR_TYPE_STR = new string(typeid(string *).name());
			BOOL_PTR_TYPE_STR = new string(typeid(bool *).name());

			// particular case for return ... for not to force user returning pointer (this is the only vars allowed, sorry!)
			BOOL_TYPE_STR = new string(typeid(bool).name());
			INT_TYPE_STR = new string(typeid(int).name());
			FLOAT_TYPE_STR = new string(typeid(float).name());
			STACK_ELEMENT_PTR= new string(typeid(tStackElement *).name());

			// register metamethods ...
			strcpy(registered_metamethod[EQU_METAMETHOD],"_equ");  // ==
			strcpy(registered_metamethod[NOT_EQU_METAMETHOD],"_nequ");  // !=,
			strcpy(registered_metamethod[LT_METAMETHOD],"_lt");  // <
			strcpy(registered_metamethod[LTE_METAMETHOD],"_lte");  // <=
			strcpy(registered_metamethod[NOT_METAMETHOD],"_not"); // !
			strcpy(registered_metamethod[GT_METAMETHOD],"_gt");  // >
			strcpy(registered_metamethod[GTE_METAMETHOD],"_gte"); // >=

			strcpy(registered_metamethod[NEG_METAMETHOD],"_neg"); // -a, !a
			strcpy(registered_metamethod[ADD_METAMETHOD],"_add"); // +
			strcpy(registered_metamethod[DIV_METAMETHOD],"_div"); // /
			strcpy(registered_metamethod[MUL_METAMETHOD],"_mul"); // *
			strcpy(registered_metamethod[MOD_METAMETHOD],"_mod");  // %

			strcpy(registered_metamethod[AND_METAMETHOD],"_and"); // binary and
			strcpy(registered_metamethod[OR_METAMETHOD],"_or"); //   binary or
			strcpy(registered_metamethod[XOR_METAMETHOD],"_xor"); // binary xor
			strcpy(registered_metamethod[SHL_METAMETHOD],"_shl"); // binary shift left
			strcpy(registered_metamethod[SHR_METAMETHOD],"_shr"); // binary shift right
			strcpy(registered_metamethod[SET_METAMETHOD],"_set"); // set
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

			if((registerClass("float","",NULL)) == NULL) return false;		// 7
			vec_script_class_node->at(IDX_CLASS_FLOAT_C)->classPtrType=*FLOAT_TYPE_STR;


			if((registerClass("bool","",NULL)) == NULL) return false;		// 8
			vec_script_class_node->at(IDX_CLASS_BOOL_C)->classPtrType=*BOOL_TYPE_STR;


			// register basic classes...
			if((registerClass(MAIN_SCRIPT_CLASS_NAME,"",NULL)) == NULL) return false; // 7
			if((registerFunctionSymbol(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME,IDX_MAIN_AST_NODE)) == NULL) return false;

			REGISTER_BASIC_TYPE(tStackElement,IDX_STACK_ELEMENT);



			//------------------------------------------------------------------------------------------------------------------------------------------------------------------

			// rgister basic classes (Warning!!! must match idx !!! and the order is important!!!)
			REGISTER_BASIC_TYPE(CScriptVariable,IDX_CLASS_SCRIPT_VAR);
			REGISTER_BASIC_TYPE(CStringScriptVariable,IDX_CLASS_STRING);
			REGISTER_BASIC_TYPE(CVectorScriptVariable,IDX_CLASS_VECTOR);
			REGISTER_BASIC_TYPE(CFunctorScriptVariable,IDX_CLASS_FUNCTOR);
			REGISTER_BASIC_TYPE(CStructScriptVariable,IDX_CLASS_STRUCT);



			//-----------------------
			// Conversion from object types to primitive types (move into factory) ...
			if(!addPrimitiveTypeConversion<CStringScriptVariable *,string *>( [] (CScriptVariable *obj){
				return (intptr_t)(((CStringScriptVariable *)obj)->m_value);
			})) return false;
			//----------------------------------------------------------------------
			// From here you defined all basic, start define hierarchy

			// register custom functions ...
			if(!register_C_FunctionMember("toString",&CScriptVariable::toString)) return false;

			if(!class_C_baseof<CVectorScriptVariable,CScriptVariable>()) return false;
			if(!class_C_baseof<CFunctorScriptVariable,CScriptVariable>()) return false;
			if(!class_C_baseof<CStructScriptVariable,CScriptVariable>()) return false;


			//------------------------------------------------------------------------------------------------------------
			// Let's register functions,...
			// register c function's
			if(!register_C_Function("print",print)) return false;
			if(!register_C_Function("error",internal_print_error)) return false;


			if(!register_C_FunctionMember("size",&CVectorScriptVariable::size)) return false;
			if(!register_C_FunctionMember("add",&CVectorScriptVariable::add)) return false;
			if(!register_C_FunctionMember("pop",&CVectorScriptVariable::pop)) return false;


			if(!register_C_FunctionMember("size",&CStructScriptVariable::size)) return false;
			return true;
	 }


	 void CScriptClass::destroySingletons(){
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

		 VOID_TYPE_STR=NULL;
		 INT_PTR_TYPE_STR=NULL;
		 FLOAT_PTR_TYPE_STR=NULL;
		 CONST_CHAR_PTR_TYPE_STR=NULL;
		 STRING_PTR_TYPE_STR=NULL;
		 BOOL_PTR_TYPE_STR=NULL;
	 }

	 CScriptClass::CScriptClass(){
			classPtrType="";
			idxClass=-1;
			c_destructor = NULL;
			c_constructor=NULL;
			idx_function_script_constructor=-1;
	}

	int getNumberArgsfromFunctionRefNode(PASTNode ast_node){
		if(ast_node->node_type == NODE_TYPE::FUNCTION_REF_NODE){ // function


			ast_node = AST_NODE(ast_node->idxAstParent);

			if(ast_node == NULL){
				zs_print_error_cr("internal error, parent null");
				return false;
			}

			if(ast_node->node_type != NODE_TYPE::CALLING_OBJECT_NODE){
				zs_print_error_cr("internal error, expected calling object node");
				return false;
			}

			if(ast_node->children.size()!=2){
				zs_print_error_cr("internal error, expected calling function 2 children");
				return false;
			}

			ast_node = AST_NODE(ast_node->children[1]);

			if(ast_node == NULL){
				zs_print_error_cr("internal error, expected args node");
				return false;
			}

			if(ast_node->node_type != NODE_TYPE::ARGS_PASS_NODE){
				zs_print_error_cr("internal error, espected ARGS PASS NODE");
				return false;
			}

			return ast_node->children.size();
		}else{
			zs_print_error_cr("internal error, espected FUNCTION_REF_NODE");
		}

		return -1;
	}

	int getNumberArgsfromFunctionObjectNode(PASTNode ast_node){
		if(ast_node->node_type == NODE_TYPE::FUNCTION_OBJECT_NODE){ // function

			if(ast_node->children.size()!=2){
				zs_print_error_cr("internal error, expected calling function 1 children");
				return false;
			}

			ast_node = AST_NODE(ast_node->children[0]);

			if(ast_node == NULL){
				zs_print_error_cr("internal error, expected args node");
				return false;
			}

			if(ast_node->node_type != NODE_TYPE::ARGS_DECL_NODE){
				zs_print_error_cr("internal error, espected ARGS PASS NODE");
				return false;
			}

			return ast_node->children.size();
		}else{
			zs_print_error_cr("internal error, espected FUNCTION_OBJECT_NODE");
		}

		return -1;
	}

	bool CScriptClass::searchVarFunctionSymbol(tFunctionInfo * info_function, tInfoAsmOp *iao, int current_function, bool & symbol_not_found, unsigned int param_scope_type){

		int idx=0;
		symbol_not_found = true;
		char n_args_to_find =-1;
		PASTNode ast_node = AST_NODE(iao->idxAstNode);
		string symbol_to_find =ast_node->symbol_value;

		if(ast_node->node_type == NODE_TYPE::FUNCTION_REF_NODE){ // function

			n_args_to_find = getNumberArgsfromFunctionRefNode(ast_node);
		}

		unsigned int iao_scope = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);

		if(iao_scope & INS_PROPERTY_SUPER_SCOPE){ // try deduce local/global

			if(current_function > 0){ // minimum have to have a 1

				tFunctionInfo *irfs =  GET_FUNCTION_INFO(info_function->local_symbols.vec_idx_registeredFunction[current_function]);//.object_info;
				symbol_to_find = irfs->symbol_info.symbol_name;

				int idx_super=-1;
				bool is_c=false;

				for(int i = current_function-1; i >= 0 && idx_super==-1; i--){
					CScriptFunctionObject * sfo = GET_SCRIPT_FUNCTION_OBJECT(info_function->local_symbols.vec_idx_registeredFunction[i]);
					if((sfo->object_info.symbol_info.symbol_name == symbol_to_find) && ((int)sfo->m_arg.size() == n_args_to_find)){ // match name and args ...
						is_c = (sfo->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) != 0;
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

			 if((idx=CScriptFunctionObject::getIdxFunctionObject(info_function,symbol_to_find,n_args_to_find,false))!=-1){
				 REMOVE_SCOPES(iao->instruction_properties);
				 iao->instruction_properties |= param_scope_type;
				 iao->index_op1 = LOAD_TYPE_FUNCTION;
				 iao->index_op2 = idx;
				 if((GET_FUNCTION_INFO(info_function->local_symbols.vec_idx_registeredFunction[idx])->symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // set as -1 to search the right signature ...
					 iao->index_op2 = -1;
				 }
				 return true;
			 }else {
				 if((idx=CScriptFunctionObject::getIdxVariableSymbol(info_function,symbol_to_find, false))!=-1){
					 REMOVE_SCOPES(iao->instruction_properties);
					 iao->instruction_properties |= param_scope_type;
					 iao->index_op1 = LOAD_TYPE_VARIABLE;
					 iao->index_op2 = idx;
					 return true;
				 }
			 }
		}
		symbol_not_found = false;
		 return false;
	}

	void CScriptClass::buildScopeVariablesBlock(CScriptFunctionObject *root_class_irfs ){
		/// PRE: base_class_irfs must be info of root class.
		 bool is_main_function = GET_MAIN_FUNCTION_OBJECT == root_class_irfs;
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
				 vector<tInfoVariableSymbol> *vs = &root_class_irfs->object_info.local_symbols.m_registeredVariable;
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

				 root_class_irfs->object_info.info_var_scope = (tInfoVarScopeBlock*)malloc(vec_ivsb.size()*sizeof(tInfoVarScopeBlock));
				 root_class_irfs->object_info.n_info_var_scope = vec_ivsb.size();

				 for(unsigned i = 0; i < vec_ivsb.size(); i++){
					 root_class_irfs->object_info.info_var_scope[i].idxScope = vec_ivsb[i].idxScope;
					 root_class_irfs->object_info.info_var_scope[i].n_var_index = (char)vec_ivsb[i].var_index.size();
					 root_class_irfs->object_info.info_var_scope[i].var_index = (int *)malloc(sizeof(int)*vec_ivsb[i].var_index.size());
					 for(unsigned j = 0; j < vec_ivsb[i].var_index.size(); j++){
						 root_class_irfs->object_info.info_var_scope[i].var_index[j] = vec_ivsb[i].var_index[j];
					 }
				 }
			 }
		 }
	}

	bool CScriptClass::updateFunctionSymbols(int idxScriptFunctionObject, const string & parent_symbol, int n_function){

		CScriptFunctionObject * sfo = GET_SCRIPT_FUNCTION_OBJECT(idxScriptFunctionObject);
		tFunctionInfo * info_function = &sfo->object_info;
		bool symbol_found=false;

		zs_print_info_cr("processing function %s -> %s",parent_symbol.c_str(),info_function->symbol_info.symbol_name.c_str());

		buildScopeVariablesBlock(sfo);

		 for(PtrStatment stat = info_function->statment_op; *stat != NULL;stat++){
			 int idx_op=0;
			 for(tInfoAsmOp *iao=*stat; iao->operator_type!=END_STATMENT ; iao++,idx_op++){

				 unsigned int scope_type = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);

				 if(iao->operator_type==ASM_OPERATOR::LOAD){

					 if(scope_type & INS_PROPERTY_ACCESS_SCOPE ){
						 iao->index_op1 = LOAD_TYPE_VARIABLE;
						 iao->index_op2 = idx_op-1;
					 }
					 else {

						 if(iao->index_op1 == LOAD_TYPE_NOT_DEFINED){

									 string symbol_to_find =AST_SYMBOL_VALUE(iao->idxAstNode);

									 if(scope_type & INS_PROPERTY_ACCESS_SCOPE){

										 ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(iao->idxAstNode),"Symbol defined \"%s\" will solved at run-time", symbol_to_find.c_str());
									 }
									 else{
										 // search local...
										 if(!searchVarFunctionSymbol(info_function,iao,n_function,symbol_found,INS_PROPERTY_LOCAL_SCOPE)){

											 // search global...
											 if(!searchVarFunctionSymbol(&GET_MAIN_FUNCTION_OBJECT->object_info,iao,n_function,symbol_found,0)){
													PASTNode ast_node = AST_NODE(iao->idxAstNode);

													if(ast_node->node_type == NODE_TYPE::FUNCTION_REF_NODE){ // function

														if(!symbol_found){
															ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(iao->idxAstNode),"function \"%s\" not registered", symbol_to_find.c_str() );
														}
														else{
															ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(iao->idxAstNode),"Cannot match function \"%s\" with %i args",symbol_to_find.c_str(),getNumberArgsfromFunctionRefNode(ast_node) );
														}

													}
													else{
														ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(iao->idxAstNode),"Symbol defined \"%s\"not found", symbol_to_find.c_str());
													}
												 return false;
											 }

										 }
								 }
						 }
					 }

				 }else  if(iao->operator_type==ASM_OPERATOR::CALL){ // overrides variable type as function ...
					 // check whether access scope ...
					 if((*stat)[iao->index_op1].operator_type ==ASM_OPERATOR::LOAD  && // e
							 (GET_INS_PROPERTY_SCOPE_TYPE((*stat)[iao->index_op1].instruction_properties) & INS_PROPERTY_ACCESS_SCOPE)){

						 (*stat)[iao->index_op1].index_op1 = LOAD_TYPE_FUNCTION;
						 (*stat)[iao->index_op1].index_op2 = iao->index_op1-1;
						// iao->instruction_properties |= INS_PROPERTY_CALLING_OBJECT;
					 }

				 }
			 }
		 }

		 vector<int> *mrf = &info_function->local_symbols.vec_idx_registeredFunction;

		 for(unsigned k=0; k < mrf->size();k++){

			 if(!updateFunctionSymbols(mrf->at(k), info_function->symbol_info.symbol_name, k)){
				 return false;
			 }
		 }

		return true;
	}

	bool CScriptClass::updateReferenceSymbols(){


		 int idx_main_function = ((*vec_script_class_node)[IDX_START_SCRIPTVAR]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[0]);
		 CScriptFunctionObject  *main_function = GET_SCRIPT_FUNCTION_OBJECT((*vec_script_class_node)[IDX_START_SCRIPTVAR]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[0]);
		 zs_print_debug_cr("DEFINED CLASSES");
		 vector<int>  mrf; // aux vector for collect function obj info.
		 bool symbol_found = false;

		 // For each class (after primitives)...
		 for(unsigned i = IDX_START_SCRIPTVAR; i < (*vec_script_class_node).size(); i++){

			 mrf.clear();
			 if(i==IDX_START_SCRIPTVAR){ // First entry (MAIN_CLASS), load global functions....
				 mrf.push_back(idx_main_function);//->object_info.local_symbols.vec_idx_registeredFunction;
				 for(unsigned h=0; h<  main_function->object_info.local_symbols.vec_idx_registeredFunction.size(); h++){
					 mrf.push_back(main_function->object_info.local_symbols.vec_idx_registeredFunction[h]);
				 }
			 }else{ // any other class
				 for(unsigned h=0; h<  (*vec_script_class_node)[i]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size(); h++){
					 mrf.push_back((*vec_script_class_node)[i]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[h]);
				 }
			 }

			 // For each function on the class ...
			 for(unsigned k=0; k < mrf.size();k++){


				 CScriptFunctionObject * info_function = GET_SCRIPT_FUNCTION_OBJECT(mrf[k]);
				 bool is_main_class = i == IDX_CLASS_MAIN;
				 bool is_main_function = is_main_class && k==0;
				 CScriptClass * _belonging_class = (*vec_script_class_node)[i];

				 buildScopeVariablesBlock(info_function);

				 if(is_main_class){
					 if(is_main_function){
						 zs_print_debug_cr("Main Function");
					 }
					 else{
						 zs_print_debug_cr("Global function %s...",info_function->object_info.symbol_info.symbol_name.c_str());
					 }
				 }else{
					 zs_print_debug_cr("Function %s::%s...",_belonging_class->metadata_info.object_info.symbol_info.symbol_name.c_str(),info_function->object_info.symbol_info.symbol_name.c_str());
				 }

				 if(info_function->object_info.statment_op != NULL){
					 for(PtrStatment stat = info_function->object_info.statment_op; *stat!=NULL;stat++){
						 int idx_op = 0;
						 for(tInfoAsmOp *iao=*stat; iao->operator_type!=END_STATMENT; iao++,idx_op++){

							 if(iao->operator_type==ASM_OPERATOR::LOAD){

								 string base_class = _belonging_class->metadata_info.object_info.symbol_info.symbol_name;

								 tFunctionInfo *sfi=NULL;
								 unsigned int scope_type = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);

								 if(scope_type & INS_PROPERTY_ACCESS_SCOPE ){
									 iao->index_op1 = LOAD_TYPE_VARIABLE;
									 iao->index_op2 = -1; // keep unknow reference for first time ..
								 }
								 else {

									 if((scope_type & INS_PROPERTY_THIS_SCOPE) | (scope_type & INS_PROPERTY_SUPER_SCOPE)){

										 sfi = &_belonging_class->metadata_info.object_info;
									 }

									 if(iao->index_op1 == LOAD_TYPE_NOT_DEFINED){

										 string symbol_to_find =AST_SYMBOL_VALUE(iao->idxAstNode);

										 if(scope_type & INS_PROPERTY_ACCESS_SCOPE){

											 ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(iao->idxAstNode),"Symbol \"%s\" will solved at run-time", AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode));
										 }
										 else{

											// this/super required ...
											 if(sfi != NULL){
												 // search global...
												 if(scope_type & INS_PROPERTY_SUPER_SCOPE){
													 symbol_to_find = GET_SCRIPT_FUNCTION_OBJECT(sfi->local_symbols.vec_idx_registeredFunction[k])->object_info.symbol_info.symbol_name;
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
															 arg_str+="arg"+CStringUtils::intToString(i);
														 }
														 ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(iao->idxAstNode),"Cannot find ancestor function for \"%s(%s)\". Is registered ?", symbol_to_find.c_str(),arg_str.c_str());
													 }
													 else{
														 ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(iao->idxAstNode),"Symbol \"%s::%s\"not found", base_class.c_str(),symbol_to_find.c_str());
													 }
													 return false;
												 }
											 }
											 else{ //normal symbol...
												 // search local...
												 if(!searchVarFunctionSymbol(&info_function->object_info,iao,k,symbol_found,INS_PROPERTY_LOCAL_SCOPE)){

													 // search global...
													 CScriptFunctionObject * mainFunctionInfo = GET_MAIN_FUNCTION_OBJECT;// getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME);
													 if(!searchVarFunctionSymbol(&mainFunctionInfo->object_info,iao,k,symbol_found,0)){
															PASTNode ast_node = AST_NODE(iao->idxAstNode);

															if(ast_node->node_type == NODE_TYPE::FUNCTION_REF_NODE){ // function

																if(!symbol_found){
																	ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(iao->idxAstNode),"function \"%s\" not registered", symbol_to_find.c_str() );
																}
																else{
																	ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(iao->idxAstNode),"Cannot match function \"%s\" with %i args",symbol_to_find.c_str(),getNumberArgsfromFunctionRefNode(ast_node) );
																}

															}
															else{
																ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(iao->idxAstNode),"Symbol \"%s\"not found",symbol_to_find.c_str());
															}
														 return false;
													 }
												 }
											}
										}
									 }
								 }

							 }else  if(iao->operator_type==ASM_OPERATOR::CALL){ // overrides variable type as function ...
								 // check whether access scope ...
								 if((*stat)[iao->index_op1].operator_type ==ASM_OPERATOR::LOAD  && // e
									(GET_INS_PROPERTY_SCOPE_TYPE((*stat)[iao->index_op1].instruction_properties) & INS_PROPERTY_ACCESS_SCOPE)){

									 (*stat)[iao->index_op1].index_op1 = LOAD_TYPE_FUNCTION;
									 (*stat)[iao->index_op1].index_op2 = -1;
									// iao->instruction_properties |= INS_PROPERTY_CALLING_OBJECT;
								 }
							 }
						 }
					 }
				 }

				 if(!is_main_function){ // process all function symbols ...
					 for(unsigned m=0; m < info_function->object_info.local_symbols.vec_idx_registeredFunction.size(); m++){
						 if(!updateFunctionSymbols(
								 info_function->object_info.local_symbols.vec_idx_registeredFunction[m],
								 info_function->object_info.symbol_info.symbol_name,m)){
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
						 ,(*vec_script_class_node)[i]->metadata_info.object_info.symbol_info.symbol_name.c_str()
				);
			 }



		 }

		 // update global variables, only C refs...
		 for(unsigned i = 0;i < main_function->object_info.local_symbols.m_registeredVariable.size();i++){
			 if(main_function->object_info.local_symbols.m_registeredVariable[i].properties & PROPERTY_C_OBJECT_REF){
				 CURRENT_VM->iniStackVar(
						 i,

						 CScriptClass::C_REF_InfoVariable_2_StackElement(
								 	&main_function->object_info.local_symbols.m_registeredVariable[i],
									(void *)main_function->object_info.local_symbols.m_registeredVariable[i].ref_ptr
				 ));
			 }
		 }
		 // finally update constant symbols ...
		 return true;
	}


	CScriptVariable *		CScriptClass::instanceScriptVariableByClassName(const string & class_name){

		 // 0. Search class info ...
		 CScriptClass * rc = getScriptClassByName(class_name);

		 if(rc != NULL){
			 return instanceScriptVariableByIdx(rc->metadata_info.object_info.symbol_info.idxScriptClass);
		 }

		 return NULL;
	 }

	 CScriptVariable 		 * CScriptClass::instanceScriptVariableByIdx(int idx_class, void * value_object){

		 CScriptVariable *class_object=NULL;

		 // 0. Search class info ...
		 CScriptClass *rc = getScriptClassByIdx(idx_class);

		 if(rc != NULL){

			 // Is a primitive ?
			 switch(rc->metadata_info.object_info.symbol_info.idxScriptClass){

			 case IDX_CLASS_VOID_C:
			 case IDX_CLASS_INT_PTR_C:
			 case IDX_CLASS_FLOAT_PTR_C:
			 case IDX_CLASS_STRING_PTR_C:
			 case IDX_CLASS_BOOL_PTR_C:
				 zs_print_error_cr("Internal error!");
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

	 CScriptVariable 		 * CScriptClass::getScriptVariableByIdx(int idx_class, int idx_var){


		 if(idx_var == ZS_UNDEFINED_IDX){
			 zs_print_error_cr("invalid index");
			 return NULL;
		 }
		 // 0. Search class info ...
		 CScriptClass *rc = getScriptClassByIdx(idx_class);

		 if(rc != NULL){

			 if(idx_var < (int)rc->metadata_info.object_info.local_symbols.m_registeredVariable.size()){
				 return (CScriptVariable  *)rc->metadata_info.object_info.local_symbols.m_registeredVariable[idx_var].ref_ptr;
			 }
			 else{
				 zs_print_error_cr("index out of bounds");
			 }
		 }

		 return NULL;
	 }

	/**
	 * Register C variable
	 */
	bool  CScriptClass::register_C_VariableInt(const string & var_name,void * var_ptr, const string & var_type)
	{
		//CScope *scope;
		tInfoVariableSymbol *irs;

		if(var_ptr==NULL){
			zs_print_error_cr("cannot register var \"%s\" with NULL reference value", var_name.c_str());
			return false;
		}

		int idxMainFunctionInfo = getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME);

		if(idxMainFunctionInfo == -1){
			zs_print_error_cr("main function is not created");
			return false;
		}

		if(getIdxClassFromIts_C_Type(var_type) == -1){
			zs_print_error_cr("%s has not valid type (%s)",var_name.c_str(),var_type.c_str());
			return false;
		}

		if(!MAIN_SCOPE_NODE->registerSymbol(var_name)){
			return false;
		}

		if((irs = FUNCTION_NEW_VARIABLE_SYMBOL(idxMainFunctionInfo)) != NULL){

			// init struct...
			irs->properties = ::PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF;
			irs->symbol_name = var_name;
			irs->ref_ptr=(intptr_t)var_ptr;
			irs->c_type=var_type;

			zs_print_debug_cr("Registered variable name: %s",var_name.c_str());


			return true;
		}

		return false;
	}

	int CScriptClass::getIdx_C_RegisteredClass(const string & str_classPtr, bool print_msg){
			// ok check c_type
			for(unsigned i = 0; i < (*vec_script_class_node).size(); i++){
				if((*vec_script_class_node)[i]->classPtrType == str_classPtr){
					return i;
				}
			}

			if(print_msg){
				zs_print_error_cr("C class %s is not registered",str_classPtr.c_str());
			}

			return -1;
	}

	int CScriptClass::getIdx_C_RegisteredFunctionMemberClass(const string & str_classPtr, const string & str_functionName, bool print_msg){

			int index_class = getIdx_C_RegisteredClass(str_classPtr,print_msg);

			if(index_class == -1){
				return -1;
			}

			vector<int> * vec_irfs = &(*vec_script_class_node)[index_class]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction;

			// ok check c_type
			for(unsigned i = 0; i < vec_irfs->size(); i++){
				CScriptFunctionObject *sfo = GET_SCRIPT_FUNCTION_OBJECT(vec_irfs->at(i));
				if(AST_SYMBOL_VALUE(sfo->object_info.symbol_info.idxAstNode) == str_classPtr){
					return i;
				}
			}

			if(print_msg){
				zs_print_error_cr("C class %s is not registered",str_classPtr.c_str());
			}

			return -1;
	}


	tInfoVariableSymbol * CScriptClass::registerVariableSymbol(const string & class_name,const string & var_name,short  idxAstNode){

		CScriptClass *rc = GET_SCRIPT_CLASS_INFO_BY_NAME(class_name);// getIdxRegisteredClass(class_name);

		if(rc != NULL){//idxScriptClass != -1){

			tFunctionInfo *object_info=&rc->metadata_info.object_info;

			tInfoVariableSymbol info_var;
			info_var.idxScriptClass = rc->metadata_info.object_info.symbol_info.idxScriptClass;
			info_var.idxAstNode = idxAstNode;
			info_var.symbol_name =var_name;
			info_var.idxSymbol = (short)object_info->local_symbols.m_registeredVariable.size();
			object_info->local_symbols.m_registeredVariable.push_back(info_var);

			return &object_info->local_symbols.m_registeredVariable[object_info->local_symbols.m_registeredVariable.size()-1];
		}else{
			zs_print_error_cr("class \"%s\" not exist!",class_name.c_str());
			return NULL;
		}

		return NULL;
	}

	tInfoVariableSymbol *  CScriptClass::getRegisteredVariableSymbol(const string & class_name,const string & function_name){

		CScriptClass *rc = GET_SCRIPT_CLASS_INFO_BY_NAME(class_name);

		if(rc != NULL){
			tFunctionInfo *object_info=&rc->metadata_info.object_info;

			for(unsigned i = 0; i < object_info->local_symbols.m_registeredVariable.size(); i++){
				if(object_info->local_symbols.m_registeredVariable[i].symbol_name == function_name){
					return &object_info->local_symbols.m_registeredVariable[i];
				}
			}
		}
		zs_print_error_cr("variable member %s::%s doesn't exist",class_name.c_str(),function_name.c_str());
		return NULL;
	}

	int CScriptClass::getIdxRegisteredVariableSymbol(tFunctionInfo *idxFunction ,const string & function_name, bool show_msg){

		for(unsigned i = 0; i < idxFunction->local_symbols.m_registeredVariable.size(); i++){
			if(idxFunction->local_symbols.m_registeredVariable[i].symbol_name == function_name){
				return i;
			}
		}

		if(show_msg){
			zs_print_error_cr("variable member %s::%s doesn't exist",idxFunction->symbol_info.symbol_name.c_str(),function_name.c_str());
		}
		return -1;
	}

	tFunctionInfo *  CScriptClass::getSuperClass(CScriptClass *irc, const string & fun_name){


		if(irc == NULL){ // trivial case ...
			return NULL;
		}

		for(unsigned i = 0; i < irc->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size(); i++){
			CScriptFunctionObject *sfo = GET_SCRIPT_FUNCTION_OBJECT(irc->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[i]);
			if(sfo->object_info.symbol_info.symbol_name == fun_name){
				return &irc->metadata_info.object_info;
			}
		}

		CScriptClass *base = NULL;
		if(irc->baseClass.size() > 0){
			base = irc->baseClass[0];
		}

		return getSuperClass(base,fun_name);
	}
	//-------
	CScriptFunctionObject * CScriptClass::registerFunctionSymbol(const string & class_name, const string & fun_name, short  idxAstNode){

		CScriptClass *rc = GET_SCRIPT_CLASS_INFO_BY_NAME(class_name);

		if(rc != NULL){

			tFunctionInfo *object_info=&rc->metadata_info.object_info;

			CScriptFunctionObject *irs = CScriptFunctionObject::newScriptFunctionObject();

			irs->object_info.symbol_info.idxScriptClass = object_info->symbol_info.idxScriptClass;
			irs->object_info.symbol_info.symbol_name = fun_name;
			irs->object_info.symbol_info.idxAstNode = idxAstNode;

			if(fun_name == class_name){
				rc->idx_function_script_constructor = object_info->local_symbols.vec_idx_registeredFunction.size();
			}
			irs->object_info.symbol_info.idxSymbol = (short)object_info->local_symbols.vec_idx_registeredFunction.size();
			object_info->local_symbols.vec_idx_registeredFunction.push_back(irs->object_info.idxScriptFunctionObject);

			// check if metamethod...
			for(int i = 0; i < MAX_METAMETHOD_OPERATORS; i++){
				if(STRCMP(registered_metamethod[i],==,fun_name.c_str())){

					rc->metamethod_operator[i].push_back(irs->object_info.idxScriptFunctionObject);

					zs_print_debug_cr("Registered metamethod %s::%s",class_name.c_str(), fun_name.c_str());
					break;
				}
			}
			return irs;
		}
		return NULL;
	}

	int CScriptClass::getIdxScriptFunctionObjectByClassFunctionName_Internal(CScriptClass *rc,const string & function_name, bool show_errors){

		if(rc != NULL){

			tFunctionInfo *object_info=&rc->metadata_info.object_info;

			// from lat value to first to get last override function...
			for(int i = object_info->local_symbols.vec_idx_registeredFunction.size()-1; i >= 0 ; i--){
				tFunctionInfo * function_info = GET_FUNCTION_INFO(object_info->local_symbols.vec_idx_registeredFunction[i]);
				if(function_info->symbol_info.symbol_name == function_name){
					return object_info->local_symbols.vec_idx_registeredFunction[i];
				}
			}

			if(show_errors){
				zs_print_error_cr("function member %s::%s doesn't exist",rc->metadata_info.object_info.symbol_info.symbol_name.c_str(),function_name.c_str());
			}
		}
		return -1;
	}

	int CScriptClass::getIdxScriptFunctionObjectByClassFunctionName(const string & class_name,const string & function_name, bool show_errors){
		return getIdxScriptFunctionObjectByClassFunctionName_Internal(GET_SCRIPT_CLASS_INFO_BY_NAME(class_name),function_name,show_errors);
	}

	CScriptFunctionObject  * CScriptClass::getScriptFunctionObjectByClassFunctionName(const string & class_name,const string & function_name, bool show_errors){

		int idx=getIdxScriptFunctionObjectByClassFunctionName(class_name,function_name, show_errors);

		if(idx != ZS_UNDEFINED_IDX){
			return GET_SCRIPT_FUNCTION_OBJECT(idx);
		}
		return NULL;
	}

	CScriptFunctionObject  * CScriptClass::getScriptFunctionObjectByClassIdxFunctionName(int idxClassName,const string & function_name, bool show_errors){

		int idx=getIdxScriptFunctionObjectByClassFunctionName_Internal(GET_SCRIPT_CLASS_INFO(idxClassName),function_name, show_errors);

		if(idx != ZS_UNDEFINED_IDX){
			return GET_SCRIPT_FUNCTION_OBJECT(idx);
		}
		return NULL;
	}

	//-----

	bool CScriptClass::addArgumentFunctionSymbol(const string & class_name,const string & function_name, const string & arg_name){

		int idxScriptFunctionObject = getIdxScriptFunctionObjectByClassFunctionName(class_name,function_name);

		if(idxScriptFunctionObject!=-1){

			int idx_type = CScriptClass::getIdx_C_RegisteredClass(arg_name);
			if(idx_type == -1){
				zs_print_error_cr("internal error");
				return false;
			}
			GET_SCRIPT_FUNCTION_OBJECT(idxScriptFunctionObject)->m_arg.push_back(idx_type);
		}else{
			//zs_print_error_cr("object info NULL");
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

	CScriptClass *  CScriptClass::getRegisteredClassVector(){
		return (*vec_script_class_node)[IDX_CLASS_VECTOR];
	}

	CScriptClass *  CScriptClass::getRegisteredClassFunctor(){
		return (*vec_script_class_node)[IDX_CLASS_FUNCTOR];
	}

	const char * CScriptClass::getNameRegisteredClassByIdx(int idx){
		if(idx != -1){
			return (*vec_script_class_node)[idx]->metadata_info.object_info.symbol_info.symbol_name.c_str();
		}
		 return "unknow";
	}
}



