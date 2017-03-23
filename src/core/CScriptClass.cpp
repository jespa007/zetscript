#include "zs_core.h"


#define REGISTER_BASIC_TYPE(type_class, idx_class)\
	if(!register_C_Class<type_class>(STR(type_class))) return false;\
	if(vec_script_class_node->at(idx_class)->classPtrType!=typeid(type_class *).name()){\
		print_error_cr("Error initializing basic type: %s",STR(type_class));\
		return false;\
	}

vector<CScriptClass *> 				* CScriptClass::vec_script_class_node=NULL;

 //CScriptClass *  	 CScriptClass::scriptClassFactory=NULL;
 CScriptClass::tPrimitiveType CScriptClass::valid_C_PrimitiveType[MAX_C_TYPE_VALID_PRIMITIVE_VAR];


 //--obj , type convert, ---
 map<string,map<string,fntConversionType>> CScriptClass::mapTypeConversion;


int CScriptClass::getIdxClassFromIts_C_Type(const string & c_type_str){

	// 1. we have to handle primitives like void, (int *), (bool *),(float *) and (string *).
	if(STRCMP(valid_C_PrimitiveType[VOID_TYPE].type_str,==,c_type_str.c_str())){
		return IDX_CLASS_VOID;
	}else if(STRCMP(valid_C_PrimitiveType[INT_PTR_TYPE].type_str,==, c_type_str.c_str())){
		return IDX_CLASS_INTEGER;
	}else if(STRCMP(valid_C_PrimitiveType[FLOAT_PTR_TYPE].type_str,==,c_type_str.c_str())){
		return IDX_CLASS_NUMBER;
	}else if(STRCMP(valid_C_PrimitiveType[STRING_PTR_TYPE].type_str,==,c_type_str.c_str())){
		return IDX_CLASS_STRING;
	}else if(STRCMP(valid_C_PrimitiveType[BOOL_PTR_TYPE].type_str,==,c_type_str.c_str())){
		return IDX_CLASS_BOOLEAN;
	}

	 // 2. Check for rest registered C classes...
	 for(unsigned i = 0; i < vec_script_class_node->size(); i++){
		 if(vec_script_class_node->at(i)->classPtrType==c_type_str)
		 {
			 return i;
		 }
	 }

	 return -1;
 }


void CScriptClass::setVectorScriptClassNode(vector<CScriptClass *> 	* set_vec){
	vec_script_class_node = set_vec;
}


vector<CScriptClass *> 		*	CScriptClass::getVectorScriptClassNode(){
	return vec_script_class_node;
}


CScriptClass * CScriptClass::newScriptClass(const string & class_name, const string & base_class_name, PASTNode _ast){
	int index;
	CScriptClass *sci=NULL;

	if(class_name.empty()){
		print_error_cr("Class name empty");
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
		sci->metadata_info.object_info.symbol_info.idxScriptClass = vec_script_class_node->size();
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

		return sci;

	}else{
		print_error_cr("error class \"%s\" already registered at line %i!", class_name.c_str(), AST_LINE_VALUE((*vec_script_class_node)[index]->metadata_info.object_info.symbol_info.idxAstNode));
	}

	return NULL;
}


CScriptClass 	* CScriptClass::getScriptClassByIdx(int idx){
	if(idx < 0 || (unsigned)idx >= vec_script_class_node->size()){
		print_error_cr("CScriptClass node out of bound");
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
			print_error_cr("class \"%s\" doesn't exist!", class_name.c_str());
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
		print_error_cr("C class type \"%s\" is not registered", demangle(class_type).c_str());
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
		print_error_cr("class %s not registered",v.c_str());
	}

	return index;
}

bool CScriptClass::isClassRegistered(const string & v){
	return getIdxScriptClass_Internal(v) != -1;
}
//------------------------------------------------------------

fntConversionType CScriptClass::getConversionType(string objectType, string conversionType, bool show_errors){

 	if(mapTypeConversion.count(objectType) == 0){
 		print_error_cr("There's no type conversion \"%s\". Add conversion types through \"addPrimitiveTypeConversion\" function",demangle(objectType).c_str());
 		return NULL;
 	}

 	if(mapTypeConversion[objectType].count(conversionType) == 0){
 		if(show_errors){
			print_error("There's no CONVERSION from type \"%s\" to type \"%s\"",demangle(objectType).c_str(),demangle(conversionType).c_str());
			printf("\n\tAvailable types are:\n");
			for(map<string, fntConversionType>::iterator j =mapTypeConversion[objectType].begin() ; j != mapTypeConversion[objectType].end();j++){
				printf("\t\t* \"%s\"\n", demangle(j->first).c_str());
			}
 		}
 		return NULL;
 	}
 	return mapTypeConversion[objectType][conversionType];
 }

 void  print(string  *s){

 	print_info_cr("PRINT:%s",s->c_str());
 }

 void  custom_function(bool  *b){
 	print_info_cr("CUSTOM_FUNCTION B:%s",*b?"true":"false");
 }

 void  custom_function(CString  *s){
 	print_info_cr("CUSTOM_FUNCTION S:%s",s->m_strValue.c_str());
 }

 void  custom_function(CInteger  *i){
 	print_info_cr("CUSTOM_FUNCTION I:%i",i->m_intValue);
 }

 CVector *  my_new_random_vector(int * n){
 	CVector *v = new CVector();
 	return v;
 }

 class MyObject2{

	 int gg;
 public:
	 MyObject2(){
		 print_info_cr("MyObject2 HHHHHHHHHHHH!!!");
		 gg=10;
	 }

	 void print2(){
		 printf("GG:%i\n",gg);
	 }

 };

 int c_var=0;
class MyObject{

public:
	int i;
	MyObject(){
		i=0;
		print_info_cr("MyObject built!!!");
	}

	MyObject(MyObject2 *pp){
		i=0;
		print_info_cr("MyObject built2!!!");
	}

	void print(){
		print_info_cr("My object v:%i",i);
	}

	~MyObject(){
		print_info_cr("MyObject destroyed!!!");
	}
};

 bool CScriptClass::init(){

	 vector<CASTNode *> *vec_ast = CASTNode::getVectorASTNode();

	    if(vec_script_class_node == NULL){
	    	print_error_cr("vector class factory not set");
	    	return false;
	    }

	    if(vec_script_class_node->size() != 0){ // must be 0 in order to register basic types...
	    	print_error_cr("vector class factory not 0!");
	    	return false;
	    }

	 	valid_C_PrimitiveType[VOID_TYPE]={typeid(void).name(),VOID_TYPE};
	 	//valid_C_PrimitiveType[INT_TYPE]={typeid(int).name(),INT_TYPE};
	 	valid_C_PrimitiveType[INT_PTR_TYPE]={typeid(int *).name(),INT_PTR_TYPE};
	 	valid_C_PrimitiveType[FLOAT_PTR_TYPE]={typeid(float *).name(),FLOAT_PTR_TYPE};
	 	valid_C_PrimitiveType[STRING_PTR_TYPE]={typeid(string *).name(),STRING_PTR_TYPE};
	 	valid_C_PrimitiveType[BOOL_PTR_TYPE]={typeid(bool *).name(),BOOL_PTR_TYPE};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//IDX_CLASS_MAIN=0, 	// Main class ...
		//IDX_CLASS_UNDEFINED,	// 1
		//IDX_CLASS_VOID,			// 2
		//IDX_CLASS_NULL,			// 3
		//IDX_CLASS_SCRIPT_VAR, 	// 4 script base that all object derive from it...
		//IDX_CLASS_INTEGER, 	  	// 5 then our basics types ...
		//IDX_CLASS_NUMBER,     	// 6
		//IDX_CLASS_STRING,     	// 7
		//IDX_CLASS_BOOLEAN,		// 8
		//IDX_CLASS_VECTOR,		// 9
		//IDX_CLASS_FUNCTOR,		// 10
		//IDX_CLASS_STRUCT,		// 11

		// MAIN CLASS (IDX==0)! Is the first entry before any other one   (this order is important!...
		if((newScriptClass(MAIN_SCRIPT_CLASS_NAME,"",NULL)) == NULL) return false; // 0
		if((registerFunctionSymbol(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME,MAIN_AST_NODE)) == NULL) return false;



		if((newScriptClass("CUndefined","",NULL)) == NULL) return false;		// 1
		vec_script_class_node->at(IDX_CLASS_UNDEFINED)->classPtrType=typeid(CUndefined *).name();

		if((newScriptClass("CVoid","",NULL)) == NULL) return false;				// 2
		vec_script_class_node->at(IDX_CLASS_VOID)->classPtrType=typeid(CVoid *).name();

		if((newScriptClass("CNull","",NULL)) == NULL) return false;				// 3
		vec_script_class_node->at(IDX_CLASS_NULL)->classPtrType=typeid(CNull *).name();


		//------------------------------------------------------------------------------------------------------------------------------------------------------------------

		// rgister basic classes (Warning!!! must match idx !!! and the order is important!!!)
		REGISTER_BASIC_TYPE(CScriptVariable,IDX_CLASS_SCRIPT_VAR);
		REGISTER_BASIC_TYPE(CInteger,IDX_CLASS_INTEGER);
		REGISTER_BASIC_TYPE(CNumber,IDX_CLASS_NUMBER);
		REGISTER_BASIC_TYPE(CString,IDX_CLASS_STRING);
		REGISTER_BASIC_TYPE(CBoolean,IDX_CLASS_BOOLEAN);
		REGISTER_BASIC_TYPE(CVector,IDX_CLASS_VECTOR);
		REGISTER_BASIC_TYPE(CFunctor,IDX_CLASS_FUNCTOR);
		REGISTER_BASIC_TYPE(CStruct,IDX_CLASS_STRUCT);



		//-----------------------
		// Conversion from object types to primitive types (move into factory) ...
		//addPrimitiveTypeConversion<CInteger *,int>( [] (CScriptVariable *obj){return *((int *)((CInteger *)obj)->m_value);});
		if(!addPrimitiveTypeConversion<CInteger *,int *>( [] (CScriptVariable *obj){
			return (int)((CInteger *)obj)->m_value;
		})) return false;

		if(!addPrimitiveTypeConversion<CInteger *,string *>( [] (CScriptVariable *obj){
			obj->m_strValue=CStringUtils::intToString(*((int *)((CInteger*)obj)->m_value));
			return (int)&obj->m_strValue;
		})) return false;

		if(!addPrimitiveTypeConversion<CNumber *,float *>( [] (CScriptVariable *obj){
			return (int)(((CNumber *)obj)->m_value);
		})) return false;

		if(!addPrimitiveTypeConversion<CNumber *,string *>( [] (CScriptVariable *obj){
			obj->m_strValue=CStringUtils::floatToString(*((float *)((CNumber*)obj)->m_value));
			return (int)&obj->m_strValue;
		})) return false;

		if(!addPrimitiveTypeConversion<CBoolean *,bool *>( [] (CScriptVariable *obj){
			return (int)((CBoolean *)obj)->m_value;
		})) return false;

		if(!addPrimitiveTypeConversion<CBoolean *,string *>( [] (CScriptVariable *obj){
			obj->toString();
			return (int)&obj->m_strValue;
		})) return false;

		if(!addPrimitiveTypeConversion<CString *,string *>( [] (CScriptVariable *obj){
			return (int)(((CString *)obj)->m_value);
		})) return false;


		//----------------------------------------------------------------------
		// From here you defined all basic, start define hierarchy

		// register custom functions ...
		if(!register_C_FunctionMember(CScriptVariable,toString)) return false;
		if(!register_C_FunctionMemberInt<CScriptVariable>("fun1",static_cast<void (CVector::*)(string * )>(&CScriptVariable::fun1))) return false;
		if(!register_C_FunctionMemberInt<CScriptVariable>("fun1",static_cast<void (CVector::*)(int * )>(&CScriptVariable::fun1))) return false;


		if(!class_C_baseof<CVoid,CScriptVariable>()) return false;
		if(!class_C_baseof<CUndefined,CScriptVariable>()) return false;
		if(!class_C_baseof<CInteger,CScriptVariable>()) return false;
		if(!class_C_baseof<CNumber,CScriptVariable>()) return false;
		if(!class_C_baseof<CBoolean,CScriptVariable>()) return false;
		if(!class_C_baseof<CString,CScriptVariable>()) return false;
		if(!class_C_baseof<CVector,CScriptVariable>()) return false;
		if(!class_C_baseof<CFunctor,CScriptVariable>()) return false;
		if(!class_C_baseof<CStruct,CScriptVariable>()) return false;


		//------------------------------------------------------------------------------------------------------------
		// Let's register functions,...

		// register c function's
		//m_chai->add(static_cast<void (*)(string * )>(&print),"print");
		//m_chai->add(static_cast<void (*)(int * )>(&print),"print");
		if(!register_C_Function(print)) return false;
		//if(!register_C_Function(print)) return false;
		//if(!register_C_Function(print)) return false;
		//CScriptClass::register_C_FunctionInt("custom_function",static_cast<void (*)(string * )>(&custom_function));
		//CScriptClass::register_C_FunctionInt("custom_function",static_cast<void (*)(bool * )>(&custom_function));

		if(!CScriptClass::register_C_FunctionInt("custom_function",static_cast<void (*)(CString * )>(&custom_function))) return false;
		if(!CScriptClass::register_C_FunctionInt("custom_function",static_cast<void (*)(CInteger * )>(&custom_function))) return false;

		if(!register_C_Variable(c_var)) return false;


		if(!register_C_FunctionMember(CVector,size)) return false;
		//if(!register_C_FunctionMember(CVector,add)) return false;

		if(!register_C_FunctionMemberInt<CVector>("add",static_cast<void (CVector::*)(CScriptVariable * )>(&CVector::add))) return false;




		// test user custom object
		if(!register_C_Class<MyObject2>("MyObject2")) return false;


		if(!register_C_Class<MyObject>("MyObject")) return false;
		if(!register_C_FunctionMember(MyObject,print)) return false;
		if(!register_C_VariableMember(MyObject,i)) return false;
		//offsetof(MyObject,i);


		if(!CScriptClass::register_C_Function(my_new_random_vector)) return false;

		// init constants vars
		UNDEFINED_SYMBOL;
		NULL_SYMBOL;


		return true;
 }


 CScriptClass::CScriptClass(){

		//idxScriptClass=-1;
		classPtrType="";
		c_destructor = NULL;
		c_constructor=NULL;
		idx_function_script_constructor=-1;
}



 CScriptClass::tPrimitiveType *CScriptClass::getPrimitiveTypeFromStr(const string & str){

 	for(unsigned i=0; i < MAX_C_TYPE_VALID_PRIMITIVE_VAR; i++){
 		if(valid_C_PrimitiveType[i].type_str == str){
 			return &valid_C_PrimitiveType[i];
 		}
 	}

 	print_error_cr("type \"%s\" is not registered",str.c_str());

 	return NULL;
 }


bool CScriptClass::searchVarFunctionSymbol(tFunctionInfo * info_function, tInfoAsmOp *iao, int current_function, unsigned int param_scope_type){

	int idx=0;
	string symbol_to_find = AST_SYMBOL_VALUE(iao->idxAstNode);
	//CScope * scope_node = iao->ast_node->scope_info_ptr;

	//scope_type = iao->scope_type;


	//tFunctionInfo * info_function= GET_FUNCTION_INFO(idxFunction);
	unsigned int iao_scope = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);

	if(iao_scope & INS_PROPERTY_SUPER_SCOPE){ // try deduce local/global

		if(current_function > 0){ // minimum have to have a 1

			tFunctionInfo *irfs =  GET_FUNCTION_INFO(info_function->local_symbols.vec_idx_registeredFunction[current_function]);//.object_info;



			symbol_to_find = irfs->symbol_info.symbol_name;


			int idx_super=-1;

			for(int i = current_function-1; i >= 0 && idx_super==-1; i--){

				if(GET_FUNCTION_INFO(info_function->local_symbols.vec_idx_registeredFunction[i])->symbol_info.symbol_name == symbol_to_find){
					idx_super=i;
				}
			}


			if(idx_super!= -1){


				 REMOVE_SCOPES(iao->instruction_properties);
				 iao->instruction_properties |= param_scope_type;
				 iao->index_op1 = LOAD_TYPE_FUNCTION;
				 iao->index_op2 = idx_super;
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

		/*if(param_scope_type & INS_PROPERTY_UNKNOWN_SCOPE){
			print_error_cr("scope type not defined");
			return false;
		}*/
		/*if(scope_type == INS_PROPERTY_UNKNOWN_SCOPE){ // try deduce local/global

			if(scope_node == CASTNode::getInstance()->getRootScope()){
				scope_type = INS_PROPERTY_GLOBAL_SCOPE;
			}else{
				scope_type = INS_PROPERTY_LOCAL_SCOPE;
			}
		}*/

		 if((idx=CScriptFunctionObject::getIdxFunctionObject(info_function,symbol_to_find,false))!=-1){
			 REMOVE_SCOPES(iao->instruction_properties);
			 iao->instruction_properties |= param_scope_type;
			 iao->index_op1 = LOAD_TYPE_FUNCTION;
			 iao->index_op2 = idx;
			 if((GET_FUNCTION_INFO(info_function->local_symbols.vec_idx_registeredFunction[idx])->symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // set as -1 to search the right signature ...
				 iao->index_op2 = -1;
				 //iao->instruction_properties = INS_PROPERTY_GLOBAL_SCOPE; // all C functions are globals !
			 }

			 //iao->idxFunction = idxFunction;
			 return true;
		 }else {
			 if((idx=CScriptFunctionObject::getIdxVariableSymbol(info_function,symbol_to_find, false))!=-1){
				 REMOVE_SCOPES(iao->instruction_properties);
				 iao->instruction_properties |= param_scope_type;
				 iao->index_op1 = LOAD_TYPE_VARIABLE;
				 iao->index_op2 = idx;
				 //iao->idxFunction = idxFunction;
				 return true;
			 }
		 }
	}




	 return false;
}



bool CScriptClass::buildScopeVariablesBlock(CScriptFunctionObject *root_class_irfs ){

	/// PRE: base_class_irfs must be info of root class.

	 bool is_main_function = GET_MAIN_FUNCTION_OBJECT == root_class_irfs;
	 PASTNode ast = AST_NODE(root_class_irfs->object_info.symbol_info.idxAstNode);

	 if(ast!=NULL){

		 int idxScope =ast->idxScope;
		 if(!is_main_function) {// is not main function

			 if(ast->node_type == NODE_TYPE::KEYWORD_NODE){
				 if(ast->keyword_info->id == KEYWORD_TYPE::FUNCTION_KEYWORD || ast->keyword_info->id == KEYWORD_TYPE::OPERATOR_KEYWORD){
					 idxScope = ast->children[1]->idxScope; // pass scope block ...
				 }
			 }
		 }

		 if(idxScope != -1){

			 struct tInfoVarScopeBlockRegister{
				 int idxScope;
				 vector<int> var_index;
			 };

			 vector<CScope *> *list = CScope::getVectorScopeNode();
			 //scp->generateScopeList(list);
			 vector<tInfoVariableSymbol> *vs = &root_class_irfs->object_info.local_symbols.m_registeredVariable;
			 vector<tInfoVarScopeBlockRegister> vec_ivsb;
			 for(unsigned i = 0;i < list->size(); i++){ // register index var per scope ...
				 tInfoVarScopeBlockRegister ivsb;

				 ivsb.idxScope = list->at(i)->idxScope;

				 for(unsigned v = 0;v < vs->size(); v++){ // register index var per scope ...

					 PASTNode ast_var = AST_NODE(vs->at(v).idxAstNode);

					 if(ast_var !=NULL){

						if(ast_var->idxScope == ivsb.idxScope){
							//print_info_cr("Scope[%i] Symbol %s",ivsb.scope_ptr->getIndex(),vs->at(v).symbol_name.c_str());
							ivsb.var_index.push_back(v);
						}
					 }
				 }

				 //root_class_irfs->object_info.info_var_scope.push_back(ivsb);
				 vec_ivsb.push_back(ivsb);
			 }

			 root_class_irfs->object_info.info_var_scope = (tInfoVarScopeBlock*)malloc(vec_ivsb.size()*sizeof(tInfoVarScopeBlock));
			 root_class_irfs->object_info.n_info_var_scope = vec_ivsb.size();

			 for(unsigned i = 0; i < vec_ivsb.size(); i++){
				 root_class_irfs->object_info.info_var_scope[i].idxScope = vec_ivsb[i].idxScope;
				 root_class_irfs->object_info.info_var_scope[i].n_var_index = vec_ivsb[i].var_index.size();
				 root_class_irfs->object_info.info_var_scope[i].var_index = (int *)malloc(sizeof(int)*vec_ivsb[i].var_index.size());
				 for(unsigned j = 0; j < vec_ivsb[i].var_index.size(); j++){
					 root_class_irfs->object_info.info_var_scope[i].var_index[j] = vec_ivsb[i].var_index[j];
				 }
			 }
		 }
	 }

	 return true;
}


bool CScriptClass::updateFunctionSymbols(int idxScriptFunctionObject, const string & parent_symbol, int n_function){


	CScriptFunctionObject * sfo = GET_SCRIPT_FUNCTION_OBJECT(idxScriptFunctionObject);
	tFunctionInfo * info_function = &sfo->object_info;

	print_info_cr("processing function %s -> %s",parent_symbol.c_str(),info_function->symbol_info.symbol_name.c_str());

	if(buildScopeVariablesBlock(sfo)){
		return false;
	}



	 for(PtrStatment stat = info_function->statment_op; *stat != NULL;stat++){
		 int idx_op=0;
		 for(tInfoAsmOp *iao=*stat; iao->operator_type!=INVALID_OP ; iao++,idx_op++){

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

									 print_info_cr("Symbol defined at line %i \"%s\" will solved at run-time",AST_LINE_VALUE(iao->idxAstNode), symbol_to_find.c_str());
								 }
								 else{


									 // search local...
									 if(!searchVarFunctionSymbol(info_function,iao,n_function,INS_PROPERTY_LOCAL_SCOPE)){

										 // search global...
										 //CScriptFunctionObject * mainFunctionInfo = GET_MAIN_FUNCTION_OBJECT;//getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME);
										 if(!searchVarFunctionSymbol(&GET_MAIN_FUNCTION_OBJECT->object_info,iao,n_function,0)){
											 print_error_cr("Symbol defined at line %i \"%s\"not found",AST_LINE_VALUE(iao->idxAstNode), symbol_to_find.c_str());
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
					 iao->instruction_properties |= INS_PROPERTY_CALLING_OBJECT;
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


	 int idx_main_function = ((*vec_script_class_node)[0]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[0]);
	 CScriptFunctionObject  *main_function = GET_SCRIPT_FUNCTION_OBJECT((*vec_script_class_node)[0]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[0]);
	 print_info_cr("DEFINED CLASSES");
	 vector<int>  mrf;

	 // For each class...
	 for(unsigned i = 0; i < (*vec_script_class_node).size(); i++){

		 mrf.clear();
		 if(i==0){ // First entry (MAIN_CLASS), load global functions....
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
			 bool is_main_class = i == 0;
			 bool is_main_function = is_main_class && k==0;
			 CScriptClass * _belonging_class = (*vec_script_class_node)[i];

			 if(!buildScopeVariablesBlock(info_function)){
				 return false;
			 }

			 if(is_main_class){
				 if(is_main_function){
					 print_debug_cr("Main Function");
				 }
				 else{
					 print_debug_cr("Global function %s...",info_function->object_info.symbol_info.symbol_name.c_str());
				 }
			 }else{
				 print_debug_cr("Function %s::%s...",_belonging_class->metadata_info.object_info.symbol_info.symbol_name.c_str(),info_function->object_info.symbol_info.symbol_name.c_str());
			 }


			 if(info_function->object_info.statment_op != NULL){
				 for(PtrStatment stat = info_function->object_info.statment_op; *stat!=NULL;stat++){
					 int idx_op = 0;
					 for(tInfoAsmOp *iao=*stat; iao->operator_type!=INVALID_OP; iao++,idx_op++){

						 if(iao->operator_type==ASM_OPERATOR::LOAD){

							 string base_class = _belonging_class->metadata_info.object_info.symbol_info.symbol_name;

							 tFunctionInfo *sfi=NULL;
							 unsigned int scope_type = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);

							 if(scope_type & INS_PROPERTY_ACCESS_SCOPE ){
								 iao->index_op1 = LOAD_TYPE_VARIABLE;
								 iao->index_op2 = idx_op-1;
							 }
							 else {

								 if((scope_type & INS_PROPERTY_THIS_SCOPE) | (scope_type & INS_PROPERTY_SUPER_SCOPE)){

									 sfi = &_belonging_class->metadata_info.object_info;

								 }

								 if(iao->index_op1 == LOAD_TYPE_NOT_DEFINED){


									 string symbol_to_find =AST_SYMBOL_VALUE(iao->idxAstNode);

									 if(scope_type & INS_PROPERTY_ACCESS_SCOPE){

										 print_info_cr("Symbol defined at line %i \"%s\" will solved at run-time",AST_LINE_VALUE(iao->idxAstNode), AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode));
									 }
									 else{

										// this/super required ...
										 if(sfi != NULL){
											 // search global...
											 if(scope_type & INS_PROPERTY_SUPER_SCOPE){
												 symbol_to_find = GET_SCRIPT_FUNCTION_OBJECT(sfi->local_symbols.vec_idx_registeredFunction[k])->object_info.symbol_info.symbol_name;
											 }


											 if(!searchVarFunctionSymbol(sfi,iao,k,scope_type)){
												 if(scope_type & INS_PROPERTY_SUPER_SCOPE){
													 print_error_cr("line %i: Cannot find ancestor function for \"%s()\"",AST_LINE_VALUE(iao->idxAstNode), symbol_to_find.c_str());
												 }
												 else{
													 print_error_cr("Symbol defined at line %i \"%s::%s\"not found",AST_LINE_VALUE(iao->idxAstNode), base_class.c_str(),symbol_to_find.c_str());
												 }
												 return false;
											 }
										 }
										 else{ //normal symbol...
											 // search local...
											 if(!searchVarFunctionSymbol(&info_function->object_info,iao,k,INS_PROPERTY_LOCAL_SCOPE)){

												 // search global...
												 CScriptFunctionObject * mainFunctionInfo = GET_MAIN_FUNCTION_OBJECT;// getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME);
												 if(!searchVarFunctionSymbol(&mainFunctionInfo->object_info,iao,k,0)){
													 print_error_cr("Symbol defined at line %i \"%s\"not found",AST_LINE_VALUE(iao->idxAstNode), symbol_to_find.c_str());
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
								 (*stat)[iao->index_op1].index_op2 = iao->index_op1-1;
								 iao->instruction_properties |= INS_PROPERTY_CALLING_OBJECT;
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
	 }

	 return true;
}




CScriptVariable *		CScriptClass::newScriptVariableByName(const string & class_name){

	 // 0. Search class info ...
	 CScriptClass * rc = getScriptClassByName(class_name);


	 if(rc != NULL){
		 return newScriptVariableByIdx(rc->metadata_info.object_info.symbol_info.idxScriptClass);
	 }

	 return NULL;

 }


 CScriptVariable 		 * CScriptClass::newScriptVariableByIdx(int idx_class, void * value_object){

	 CScriptVariable *class_object=NULL;

	 // 0. Search class info ...
	 CScriptClass *rc = getScriptClassByIdx(idx_class);

	 if(rc != NULL){

		 // Is a primitive ?
		 switch(rc->metadata_info.object_info.symbol_info.idxScriptClass){
		 case IDX_CLASS_INTEGER:
			 class_object = NEW_INTEGER_VAR;
			 if(value_object!=NULL){
				 ((CInteger *)class_object)->m_intValue = *((int *)value_object);
			 }
			 break;
		 case IDX_CLASS_NUMBER:
			 class_object = NEW_NUMBER_VAR;
			 if(value_object!=NULL){
				 ((CNumber *)class_object)->m_floatValue = *((float *)value_object);
			 }
			 break;

		 case IDX_CLASS_BOOLEAN:
			 class_object = NEW_BOOLEAN_VAR;
			 if(value_object!=NULL){
				 ((CBoolean *)class_object)->m_boolValue = *((bool *)value_object);
			 }
			 break;

		 case IDX_CLASS_STRING:
			 class_object = NEW_STRING_VAR;
			 if(value_object!=NULL){
				 ((CString *)class_object)->m_strValue = *((string *)value_object);
			 }
			 break;

		 case IDX_CLASS_VECTOR:
		 case IDX_CLASS_STRUCT:
		 case IDX_CLASS_UNDEFINED:
		 case IDX_CLASS_NULL:
		 case IDX_CLASS_VOID:
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
		 print_error_cr("invalid index");
		 return NULL;
	 }
	 // 0. Search class info ...
	 CScriptClass *rc = getScriptClassByIdx(idx_class);

	 if(rc != NULL){

		 if(idx_var < (int)rc->metadata_info.object_info.local_symbols.m_registeredVariable.size()){
			 return (CScriptVariable  *)rc->metadata_info.object_info.local_symbols.m_registeredVariable[idx_var].ref_ptr;
		 }
		 else{
			 print_error_cr("index out of bounds");
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
		print_error_cr("cannot register var \"%s\" with NULL reference value", var_name.c_str());
		return false;
	}

	int idxMainFunctionInfo = getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_OBJECT_NAME);

	if(idxMainFunctionInfo == -1){
		print_error_cr("main function is not created");
		return false;
	}

	if(getIdxClassFromIts_C_Type(var_type) == -1){
		print_error_cr("%s has not valid type (%s)",var_name.c_str(),var_type.c_str());
		return false;
	}

	CScope *scp = MAIN_SCOPE_ROOT;

	if(!MAIN_SCOPE_ROOT->registerSymbol(var_name)){
		return false;
	}

	//if((irs.idxScopeVar=CASTNode::getInstance()->getRootScope()->registerSymbol(var_name))==-1){
	//	return false;
	//}

	if((irs = FUNCTION_NEW_VARIABLE_SYMBOL(idxMainFunctionInfo)) != NULL){

		// init struct...
		irs->properties = ::PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF;
		irs->symbol_name = var_name;
		irs->ref_ptr=(int)var_ptr;

		print_debug_cr("Registered function name: %s",var_name.c_str());


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
			print_error_cr("C class %s is not registered",str_classPtr.c_str());
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
			print_error_cr("C class %s is not registered",str_classPtr.c_str());
		}

		return -1;
}



tInfoVariableSymbol * CScriptClass::registerVariableSymbol(const string & class_name,const string & var_name,PASTNode  ast){

	//CScriptClass *rc = getRegisteredClass(class_name);
	//int idxScriptClass
	CScriptClass *rc = GET_SCRIPT_CLASS_INFO_BY_NAME(class_name);// getIdxRegisteredClass(class_name);


	if(rc != NULL){//idxScriptClass != -1){


		//CScriptClass *rc = getRegisteredClassByIdx(idxScriptClass);
		tFunctionInfo *object_info=&rc->metadata_info.object_info;

		tInfoVariableSymbol info_var;
		info_var.idxScriptClass = rc->metadata_info.object_info.symbol_info.idxScriptClass;
		info_var.idxAstNode = ast->idxAstNode;
		info_var.symbol_name =var_name;
		info_var.idxSymbol = object_info->local_symbols.m_registeredVariable.size();
		object_info->local_symbols.m_registeredVariable.push_back(info_var);

		return &object_info->local_symbols.m_registeredVariable[object_info->local_symbols.m_registeredVariable.size()-1];
	}else{
		print_error_cr("class \"%s\" not exist!",class_name.c_str());
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
	/*if((index = getidxScriptFunctionObject_Internal(class_name))!=-1){ // check whether is local var registered scope ...

		return (*vec_script_class_node)[index];
	}else{
		if(print_msg){
			print_error_cr("class \"%s\" doesn't exist!", class_name.c_str());
		}*/
	}

	print_error_cr("variable member %s::%s doesn't exist",class_name.c_str(),function_name.c_str());

	return NULL;
}

int CScriptClass::getIdxRegisteredVariableSymbol(tFunctionInfo *idxFunction ,const string & function_name, bool show_msg){

	for(unsigned i = 0; i < idxFunction->local_symbols.m_registeredVariable.size(); i++){
		if(idxFunction->local_symbols.m_registeredVariable[i].symbol_name == function_name){
			return i;
		}
	}

	if(show_msg){
		print_error_cr("variable member %s::%s doesn't exist",idxFunction->symbol_info.symbol_name.c_str(),function_name.c_str());
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

CScriptFunctionObject * CScriptClass::registerFunctionSymbol(const string & class_name, const string & fun_name, PASTNode  ast){

	CScriptClass *rc = GET_SCRIPT_CLASS_INFO_BY_NAME(class_name);

	if(rc != NULL){

		tFunctionInfo *object_info=&rc->metadata_info.object_info;

		CScriptFunctionObject *irs = CScriptFunctionObject::newScriptFunctionObject();

		irs->object_info.symbol_info.idxScriptClass = object_info->symbol_info.idxScriptClass;
		irs->object_info.symbol_info.symbol_name = fun_name;

		irs->object_info.symbol_info.idxAstNode = -1;
		if(ast != NULL){
			irs->object_info.symbol_info.idxAstNode = ast->idxAstNode;
		}


		if(fun_name == class_name){
			rc->idx_function_script_constructor = object_info->local_symbols.vec_idx_registeredFunction.size();
		}
		irs->object_info.symbol_info.idxSymbol = object_info->local_symbols.vec_idx_registeredFunction.size();
		object_info->local_symbols.vec_idx_registeredFunction.push_back(irs->object_info.idxScriptFunctionObject);

		return irs;
	}

	return NULL;
}

int CScriptClass::getIdxScriptFunctionObjectByClassFunctionName(const string & class_name,const string & function_name, bool show_errors){

	CScriptClass *rc =GET_SCRIPT_CLASS_INFO_BY_NAME(class_name);

	if(rc != NULL){

		tFunctionInfo *object_info=&rc->metadata_info.object_info;

		// from lat value to first to get last override function...
		for(int i = object_info->local_symbols.vec_idx_registeredFunction.size()-1; i >= 0 ; i--){
			tFunctionInfo * function_info = GET_FUNCTION_INFO(object_info->local_symbols.vec_idx_registeredFunction[i]);
			if(function_info->symbol_info.symbol_name == function_name){
				return object_info->local_symbols.vec_idx_registeredFunction[i];
			}
		}
	}

	if(show_errors){
		print_error_cr("function member %s::%s doesn't exist",class_name.c_str(),function_name.c_str());
	}

	return -1;
}

CScriptFunctionObject  * CScriptClass::getScriptFunctionObjectByClassFunctionName(const string & class_name,const string & function_name, bool show_errors){

	int idx=getIdxScriptFunctionObjectByClassFunctionName(class_name,function_name, show_errors);

	if(idx != ZS_UNDEFINED_IDX){
		return GET_SCRIPT_FUNCTION_OBJECT(idx);
	}

	return NULL;
}


//-----

bool CScriptClass::addArgumentFunctionSymbol(const string & class_name,const string & function_name, const string & arg_name){


	int idxScriptFunctionObject = getIdxScriptFunctionObjectByClassFunctionName(class_name,function_name);


	if(idxScriptFunctionObject!=-1){

		GET_SCRIPT_FUNCTION_OBJECT(idxScriptFunctionObject)->m_arg.push_back(arg_name);

	}else{
		//print_error_cr("object info NULL");
	}

	return false;
}

// internal var types ...
CScriptClass *  CScriptClass::getRegisteredClassVoid(){
	return (*vec_script_class_node)[IDX_CLASS_VOID];
}

CScriptClass *  CScriptClass::getRegisteredClassUndefined(){
	return (*vec_script_class_node)[IDX_CLASS_UNDEFINED];
}

CScriptClass *  CScriptClass::getRegisteredClassInteger(){
	return (*vec_script_class_node)[IDX_CLASS_INTEGER];
}

CScriptClass *  CScriptClass::getRegisteredClassNumber(){
	return (*vec_script_class_node)[IDX_CLASS_NUMBER];
}

CScriptClass *  CScriptClass::getRegisteredClassStruct(){
	return (*vec_script_class_node)[IDX_CLASS_STRUCT];
}

CScriptClass *  CScriptClass::getRegisteredClassString(){
	return (*vec_script_class_node)[IDX_CLASS_STRING];
}

CScriptClass *  CScriptClass::getRegisteredClassBoolean(){
	return (*vec_script_class_node)[IDX_CLASS_BOOLEAN];
}

CScriptClass *  CScriptClass::getRegisteredClassVector(){
	return (*vec_script_class_node)[IDX_CLASS_VECTOR];
}

CScriptClass *  CScriptClass::getRegisteredClassFunctor(){
	return (*vec_script_class_node)[IDX_CLASS_FUNCTOR];
}

CScriptClass *  CScriptClass::getRegisteredClassNull(){
	return (*vec_script_class_node)[IDX_CLASS_NULL];
}

const char * CScriptClass::getNameRegisteredClassByIdx(int idx){
	if(idx != -1){
		return (*vec_script_class_node)[idx]->metadata_info.object_info.symbol_info.symbol_name.c_str();
	}
	 return "unknow";

}





