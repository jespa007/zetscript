#include "zs_factory.h"

 //CScriptClassFactory *  	 CScriptClassFactory::scriptClassFactory=NULL;
 CScriptClassFactory::tPrimitiveType CScriptClassFactory::valid_C_PrimitiveType[MAX_C_TYPE_VALID_PRIMITIVE_VAR];


 //--obj , type convert, ---
 map<string,map<string,fntConversionType>> CScriptClassFactory::mapTypeConversion;
vector<tInfoScriptClass *> 				* vec_registered_class_node=NULL;

/*
//	______           _     _                    _   _____ _
//	| ___ \         (_)   | |                  | | /  __ \ |
//	| |_/ /___  __ _ _ ___| |_ ___ _ __ ___  __| | | /  \/ | __ _ ___ ___
//	|    // _ \/ _` | / __| __/ _ \ '__/ _ \/ _` | | |   | |/ _` / __/ __|
//	| |\ \  __/ (_| | \__ \ ||  __/ | |  __/ (_| | | \__/\ | (_| \__ \__ \
//	\_| \_\___|\__, |_|___/\__\___|_|  \___|\__,_|  \____/_|\__,_|___/___/
//            __/ |
//           |___/
// _________________________________________________
//  __________________________________
*/


void CScriptClassFactory::createSingletons(){
	if(vec_registered_class_node==NULL){
		vec_registered_class_node = new vector<tInfoScriptClass *>();
	}
}

int CScriptClassFactory::getIdxClassFromIts_C_Type(const string & c_type_str){

	 // 1. check primitives ...
	 /*for(unsigned i = 0; i < MAX_C_TYPE_VALID_PRIMITIVE_VAR;i++){
		 if(CScriptClass::valid_C_PrimitiveType[i].type_str==type_str){
			 return true;
		 }
	 }*/
	 /*
	 if(CScriptClass::valid_C_PrimitiveType[i].type_str==type_str){
			 return true;
	 }*/
	 //int , , , , IDX_CLASS_VECTOR, IDX_CLASS_FUNCTOR, IDX_CLASS_UNDEFINED, ;
		/*if(CScriptClass::valid_C_PrimitiveType[CScriptClass::VOID_TYPE].type_str==c_type_str){
			return IDX_CLASS_VOID;
		}else if(CScriptClass::valid_C_PrimitiveType[CScriptClass::INT_PTR_TYPE].type_str== c_type_str){
			return IDX_CLASS_INTEGER;
		}else if(CScriptClass::valid_C_PrimitiveType[CScriptClass::FLOAT_PTR_TYPE].type_str==c_type_str){
			return IDX_CLASS_NUMBER;
		}else if(CScriptClass::valid_C_PrimitiveType[CScriptClass::STRING_PTR_TYPE].type_str==c_type_str){
			return IDX_CLASS_STRING;
		}else if(CScriptClass::valid_C_PrimitiveType[CScriptClass::BOOL_PTR_TYPE].type_str==c_type_str){
			return IDX_CLASS_BOOLEAN;
		}*/



	 // for all registered C classes...
	 for(unsigned i = 0; i < vec_registered_class_node->size(); i++){
		 if(vec_registered_class_node->at(i)->classPtrType==c_type_str)
		 {
			 return i;
		 }
	 }


	 return -1;

 }

void CScriptClassFactory::destroySingletons(){
	if(vec_registered_class_node!=NULL){
		delete vec_registered_class_node;
		vec_registered_class_node=NULL;
	}
}

void CScriptClassFactory::set(vector<tInfoScriptClass *> 	& set_vec){
	*vec_registered_class_node = set_vec;
}


vector<tInfoScriptClass *> 		*	CScriptClassFactory::getVectorInfoScriptClassList(){
	return vec_registered_class_node;
}


tInfoScriptClass *	CScriptClassFactory::newInfoScriptClass(){
	tInfoScriptClass 	*registered_class=new tInfoScriptClass;
	vec_registered_class_node->push_back(registered_class);
	registered_class->idxClass = vec_registered_class_node->size()-1;
	return registered_class;
}



tInfoScriptClass 	* CScriptClassFactory::getInfoScriptClassByIdx(int idx){
	if(idx < 0 || (unsigned)idx >= vec_registered_class_node->size()){
		print_error_cr("tScopeVar node out of bound");
		return NULL;
	}

	return vec_registered_class_node->at(idx);
}

tInfoScriptClass 	* CScriptClassFactory::getInfoScriptClassByName(const string & class_name, bool print_msg){
	int index;
	if((index = getIdxInfoScriptClass_Internal(class_name))!=-1){ // check whether is local var registered scope ...

		return vec_registered_class_node->at(index);
	}else{
		if(print_msg){
			print_error_cr("class \"%s\" doesn't exist!", class_name.c_str());
		}
	}

	return NULL;
}





tInfoScriptClass *CScriptClassFactory::getInfoScriptClassBy_C_ClassPtr(const string & class_type, bool print_msg){

	for(unsigned i = 0; i < vec_registered_class_node->size(); i++){
		if(class_type == vec_registered_class_node->at(i)->metadata_info.object_info.symbol_info.c_type){
			return vec_registered_class_node->at(i);
		}
	}


	if(print_msg){
		print_error_cr("C class type \"%s\" is not registered", demangle(class_type).c_str());
	}

	return NULL;

}

int CScriptClassFactory::getIdxInfoScriptClass_Internal(const string & class_name){

	for(unsigned i = 0; i < vec_registered_class_node->size(); i++){
		if(class_name == vec_registered_class_node->at(i)->metadata_info.object_info.symbol_info.symbol_name){
			return i;
		}
	}

	return -1;
}


int CScriptClassFactory::getIdxInfoScriptClass(const string & v, bool print_msg){
	int index= getIdxInfoScriptClass_Internal(v);

	if(index == -1 && print_msg){
		print_error_cr("class %s not registered",v.c_str());
	}

	return index;
}

bool CScriptClassFactory::isClassRegistered(const string & v){
	return getIdxInfoScriptClass_Internal(v) != -1;
}

//------------------------------------------------------------


fntConversionType CScriptClassFactory::getConversionType(string objectType, string conversionType, bool show_errors){


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

 void CScriptClassFactory::registerPrimitiveTypes(){


 }


 CScriptClassFactory*  CScriptClassFactory::getInstance(){

	 if(scriptClassFactory == NULL){
		 scriptClassFactory = new CScriptClassFactory();
	 }

	 return scriptClassFactory;

 }

 void CScriptClassFactory::destroySingletons() {
	 if(scriptClassFactory != NULL){
		 delete scriptClassFactory;
	 }

	 scriptClassFactory=NULL;

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

 	/*for(int i = 0; i < *n; i++){
 		v->add(new CInteger(random()%10000));
 	}

 	v->add(CZetScript::getInstance()->script_call("rec1",{}));*/

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


 bool CScriptClassFactory::registerBase(){


	 	valid_C_PrimitiveType[VOID_TYPE]={typeid(void).name(),VOID_TYPE};
	 	valid_C_PrimitiveType[INT_TYPE]={typeid(int).name(),INT_TYPE};
	 	valid_C_PrimitiveType[INT_PTR_TYPE]={typeid(int *).name(),INT_PTR_TYPE};
	 	valid_C_PrimitiveType[FLOAT_PTR_TYPE]={typeid(float *).name(),FLOAT_PTR_TYPE};
	 	valid_C_PrimitiveType[STRING_PTR_TYPE]={typeid(string *).name(),STRING_PTR_TYPE};
	 	valid_C_PrimitiveType[BOOL_PTR_TYPE]={typeid(bool *).name(),BOOL_PTR_TYPE};


		//------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// MAIN CLASS! Is the first entry before any other one   (this order is important!...
		if((registerScriptClass(MAIN_SCRIPT_CLASS_NAME,"",NULL)) == NULL) return false;
		if((registerFunctionSymbol(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_NAME,CAst::getInstance()->getMainAstNode())) == NULL) return false;

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------


		// register internal classes ...
		if(!register_C_Class<CScriptVariable>("CScriptVariable")) return false; // 0
		if(!register_C_Class<CVoid>("CVoid")) return false; // 1
		if(!register_C_Class<CNull>("CNull")) return false; // 2
		if(!register_C_Class<CUndefined>("CUndefined")) return false; // 3
		if(!register_C_Class<CInteger>("CInteger")) return false; // 4
		if(!register_C_Class<CNumber>("CNumber")) return false; // 5
		if(!register_C_Class<CBoolean>("CBoolean")) return false; // 6
		if(!register_C_Class<CString>("CString")) return false; // 7
		if(!register_C_Class<CVector>("CVector")) return false; // 8
		if(!register_C_Class<CFunctor>("CFunctor")) return false; // 9
		if(!register_C_Class<CStruct>("CStruct")) return false; // 10


		// register primitive classes first ...
		if((IDX_CLASS_VOID = getIdxRegisteredClass("CVoid"))==-1) return false;
		if((IDX_CLASS_UNDEFINED = getIdxRegisteredClass("CUndefined"))==-1) return false;
		if((IDX_CLASS_NULL = getIdxRegisteredClass("CNull"))==-1) return false;
		if((IDX_CLASS_INTEGER = getIdxRegisteredClass("CInteger"))==-1) return false;
		if((IDX_CLASS_NUMBER = getIdxRegisteredClass("CNumber"))==-1) return false;
		if((IDX_CLASS_BOOLEAN = getIdxRegisteredClass("CBoolean"))==-1) return false;
		if((IDX_CLASS_VECTOR = getIdxRegisteredClass("CVector"))==-1) return false;
		if((IDX_CLASS_STRING = getIdxRegisteredClass("CString"))==-1) return false;
		if((IDX_CLASS_FUNCTOR = getIdxRegisteredClass("CFunctor"))==-1) return false;
		if((IDX_CLASS_STRUCT = getIdxRegisteredClass("CStruct"))==-1) return false;

		//===> MOVE INTO CScriptClassFactory !!!! ====================================================>

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
		//CScriptClassFactory::register_C_FunctionInt("custom_function",static_cast<void (*)(string * )>(&custom_function));
		//CScriptClassFactory::register_C_FunctionInt("custom_function",static_cast<void (*)(bool * )>(&custom_function));

		if(!CScriptClassFactory::register_C_FunctionInt("custom_function",static_cast<void (*)(CString * )>(&custom_function))) return false;
		if(!CScriptClassFactory::register_C_FunctionInt("custom_function",static_cast<void (*)(CInteger * )>(&custom_function))) return false;

		if(!register_C_Variable(c_var)) return false;


		if(!register_C_FunctionMember(CVector,size)) return false;
		//if(!register_C_FunctionMember(CVector,add)) return false;

		if(!register_C_FunctionMemberInt<CVector>("add",static_cast<void (CVector::*)(CScriptVariable * )>(&CVector::add))) return false;




		// test user custom object
		if(!register_C_Class<MyObject2>("MyObject2")) return false;


		if(!register_C_Class<MyObject>("MyObject")) return false;
		if(!register_C_FunctionMember(MyObject,print)) return false;
		if(!register_C_VariableMember(MyObject,i)) return false;


		if(!CScriptClassFactory::register_C_Function(my_new_random_vector)) return false;



		return true;
 }


 CScriptClassFactory::CScriptClassFactory(){

	 IDX_CLASS_NULL = IDX_CLASS_INTEGER = IDX_CLASS_NUMBER = IDX_CLASS_STRING = IDX_CLASS_BOOLEAN = IDX_CLASS_VECTOR = IDX_CLASS_FUNCTOR = IDX_CLASS_UNDEFINED= IDX_CLASS_VOID = IDX_CLASS_STRUCT = -1;


 }

 int CScriptClassFactory::saveState(){
	 // 1. For all current registered classes:
	 // 1.1. Clone its ast...
	 // 1.2. Clone its scopeinfo...
	 // 1.3. Clone registered classes...
	 //m_registeredClass
	 return -1;
 }


 bool CScriptClassFactory::restoreState(int index){
 	 //m_registeredClass
	 return false;
  }


 CScriptClassFactory::tPrimitiveType *CScriptClassFactory::getPrimitiveTypeFromStr(const string & str){

 	for(unsigned i=0; i < MAX_C_TYPE_VALID_PRIMITIVE_VAR; i++){
 		if(valid_C_PrimitiveType[i].type_str == str){
 			return &valid_C_PrimitiveType[i];
 		}
 	}

 	print_error_cr("type \"%s\" is not registered",str.c_str());

 	return NULL;
 }


bool CScriptClassFactory::searchVarFunctionSymbol(int idxFunction, tInfoAsmOp *iao, int current_function, SCOPE_TYPE scope_type){

	int idx=0;
	string symbol_to_find = AST_SYMBOL_VALUE(iao->idxAstNode);
	//CScope * scope_node = iao->ast_node->scope_info_ptr;

	//scope_type = iao->scope_type;


	tFunctionInfo * info_function= GET_FUNCTION_INFO(idxFunction);


	if(iao->scope_type == SCOPE_TYPE::SUPER_SCOPE){ // try deduce local/global

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

				 iao->scope_type = scope_type;
				 iao->index_op1 = LOAD_TYPE_FUNCTION;
				 iao->index_op2 = idx_super;
				 iao->idxFunction = idxFunction;
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

		if(scope_type == SCOPE_TYPE::UNKNOWN_SCOPE){
			print_error_cr("scope type not defined");
			return false;
		}
		/*if(scope_type == SCOPE_TYPE::UNKNOWN_SCOPE){ // try deduce local/global

			if(scope_node == CAst::getInstance()->getRootScope()){
				scope_type = SCOPE_TYPE::GLOBAL_SCOPE;
			}else{
				scope_type = SCOPE_TYPE::LOCAL_SCOPE;
			}
		}*/

		 if((idx=CScriptFunctionObjectFactory::getIdxFunctionObject(idxFunction,symbol_to_find,false))!=-1){
			 iao->scope_type = scope_type;
			 iao->index_op1 = LOAD_TYPE_FUNCTION;
			 iao->index_op2 = idx;
			 if((GET_FUNCTION_INFO(info_function->local_symbols.vec_idx_registeredFunction[idx])->symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF){ // set as -1 to search the right signature ...
				 iao->index_op2 = -1;
				 iao->scope_type = SCOPE_TYPE::GLOBAL_SCOPE; // all C functions are globals !
			 }

			 iao->idxFunction = idxFunction;
			 return true;
		 }else {
			 if((idx=CScriptFunctionObjectFactory::getIdxVariableSymbol(idxFunction,symbol_to_find, false))!=-1){
				 iao->scope_type = scope_type;
				 iao->index_op1 = LOAD_TYPE_VARIABLE;
				 iao->index_op2 = idx;
				 iao->idxFunction = idxFunction;
				 return true;
			 }
		 }
	}




	 return false;
}



bool CScriptClassFactory::buildScopeVariablesBlock(tScriptFunctionObject *root_class_irfs ){

	/// PRE: base_class_irfs must be info of root class.


	/* print_info_cr("======================");
	 print_info_cr("scopes for function %s",irfs->object_info.symbol_info.symbol_name.c_str());
	 print_info_cr("======================");
*/



	 bool is_main_function = CZetScript::getInstance()->getMainStructInfo() == root_class_irfs;
	 PASTNode ast = AST_NODE(root_class_irfs->object_info.symbol_info.idxAstNode);

	 if(ast!=NULL){

		 int idxScope =ast->idxScope;
		 if(!is_main_function) {// is not main function

			 if(ast->node_type == NODE_TYPE::KEYWORD_NODE){
				 if(ast->keyword_info->id == KEYWORD_TYPE::FUNCTION_KEYWORD){
					 idxScope = ast->children[1]->idxScope; // pass scope block ...
				 }
			 }
		 }

		 if(idxScope != -1){

			 CScope *scp = SCOPE_INFO_NODE(idxScope);

			/* if(scp->getParent() != NULL){
				 print_error_cr("parent must be null!");
				 return false;
			 }*/

			 vector<CScope *> *list = scp->getVecIndexScopeNode();
			 //scp->generateScopeList(list);
			 vector<tInfoVariableSymbol> *vs = &root_class_irfs->object_info.local_symbols.m_registeredVariable;
			 for(unsigned i = 0;i < list->size(); i++){ // register index var per scope ...
				 tInfoVarScopeBlock ivsb;

				 ivsb.idxScope = list->at(i)->getIndex();

				 for(unsigned v = 0;v < vs->size(); v++){ // register index var per scope ...

					 PASTNode ast_var = AST_NODE(vs->at(v).idxAstNode);

					 if(ast_var !=NULL){

						if(ast_var->idxScope == ivsb.idxScope){
							//print_info_cr("Scope[%i] Symbol %s",ivsb.scope_ptr->getIndex(),vs->at(v).symbol_name.c_str());
							ivsb.var_index.push_back(v);
						}
					 }
				 }

				 root_class_irfs->object_info.info_var_scope.push_back(ivsb);
			 }
		 }
	 }

	 return true;
}


bool CScriptClassFactory::updateFunctionSymbols(int idxScriptFunctionObject, const string & parent_symbol, int n_function){


	tFunctionInfo * info_function = GET_FUNCTION_INFO(idxScriptFunctionObject);

	print_info_cr("processing function %s -> %s",parent_symbol.c_str(),info_function->symbol_info.symbol_name.c_str());

	if(buildScopeVariablesBlock(info_function)){
		return false;
	}

	 vector<tInfoStatementOp> *stat = &info_function->statment_op;
	 for(unsigned h=0; h < stat->size();h++){
		 for(unsigned idx_op=0; idx_op < stat->at(h).asm_op.size();idx_op++){
			 tInfoAsmOp *iao=stat->at(h).asm_op[idx_op];

			 if(iao->operator_type==ASM_OPERATOR::LOAD){

				 if(iao->scope_type == SCOPE_TYPE::ACCESS_SCOPE ){
					 iao->index_op1 = LOAD_TYPE_VARIABLE;
					 iao->index_op2 = idx_op-1;
				 }
				 else {

					 if(iao->index_op1 == LOAD_TYPE_NOT_DEFINED){

								 string symbol_to_find =AST_SYMBOL_VALUE(iao->idxAstNode);

								 if(iao->scope_type ==SCOPE_TYPE::ACCESS_SCOPE){

									 print_info_cr("Symbol defined at line %i \"%s\" will solved at run-time",AST_LINE_VALUE(iao->idxAstNode), symbol_to_find.c_str());
								 }
								 else{


									 // search local...
									 if(!searchVarFunctionSymbol(&info_function->iao,n_function,SCOPE_TYPE::LOCAL_SCOPE)){

										 // search global...
										 tScriptFunctionObject * mainFunctionInfo = getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_NAME);
										 if(!searchVarFunctionSymbol(&mainFunctionInfo->object_info,iao,n_function,SCOPE_TYPE::GLOBAL_SCOPE)){
											 print_error_cr("Symbol defined at line %i \"%s\"not found",AST_LINE_VALUE(iao->idxAstNode), symbol_to_find.c_str());
											 return false;
										 }

									 }

							 }
					 }
			 	 }

			 }else  if(iao->operator_type==ASM_OPERATOR::CALL){ // overrides variable type as function ...
				 // check whether access scope ...
				 if(stat->at(h).asm_op[iao->index_op1]->operator_type ==ASM_OPERATOR::LOAD  && // e
				    stat->at(h).asm_op[iao->index_op1]->scope_type == SCOPE_TYPE::ACCESS_SCOPE){

					 stat->at(h).asm_op[iao->index_op1]->index_op1 = LOAD_TYPE_FUNCTION;
					 stat->at(h).asm_op[iao->index_op1]->index_op2 = iao->index_op1-1;
					 iao->asm_properties = ASM_PROPERTY_CALLING_OBJECT;
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

bool CScriptClassFactory::updateReferenceSymbols(){



	 tScriptFunctionObject  *main_function = GET_SCRIPT_FUNCTION_OBJECT(m_registeredClass[0]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[0]);
	 print_info_cr("DEFINED CLASSES");
	 vector<tScriptFunctionObject *>  mrf;

	 // For each class...
	 for(unsigned i = 0; i < m_registeredClass.size(); i++){

		 mrf.clear();
		 if(i==0){ // First entry (MAIN_CLASS), load global functions....
			 mrf.push_back(main_function);//->object_info.local_symbols.vec_idx_registeredFunction;
			 for(unsigned h=0; h<  main_function->object_info.local_symbols.vec_idx_registeredFunction.size(); h++){
				 mrf.push_back(&main_function->object_info.local_symbols.vec_idx_registeredFunction[h]);
			 }
		 }else{ // any other class
			 for(unsigned h=0; h<  m_registeredClass[i]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size(); h++){
				 mrf.push_back(&m_registeredClass[i]->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[h]);
			 }
		 }

		 // For each function on the class ...
		 for(unsigned k=0; k < mrf.size();k++){


			 tScriptFunctionObject * info_function = mrf[k];
			 bool is_main_class = i == 0;
			 bool is_main_function = is_main_class && k==0;
			 tInfoScriptClass * _belonging_class = m_registeredClass[i];

			 if(!buildScopeVariablesBlock(info_function)){
				 return false;
			 }

			 if(is_main_class){
				 if(is_main_function){
					 print_info_cr("Main Function");
				 }
				 else{
					 print_info_cr("Global function %s...",info_function->object_info.symbol_info.symbol_name.c_str());
				 }
			 }else{
				 print_info_cr("Function %s::%s...",_belonging_class->metadata_info.object_info.symbol_info.symbol_name.c_str(),info_function->object_info.symbol_info.symbol_name.c_str());
			 }



			 /*if(i==0){

				 if(k==0){

				 }else{

				 }
			 }else{
				 print_info_cr("Function %s::%s...",m_registeredClass[i]->metadata_info.object_info.symbol_info.symbol_name.c_str(),mrf[k]->object_info.symbol_info.symbol_name.c_str());
			 }*/
			 vector<tInfoStatementOp> *stat = &info_function->object_info.statment_op;
			 for(unsigned h=0; h < stat->size();h++){
				 for(unsigned idx_op=0; idx_op < stat->at(h).asm_op.size();idx_op++){
					 tInfoAsmOp *iao=stat->at(h).asm_op[idx_op];


					 if(iao->operator_type==ASM_OPERATOR::LOAD){

						 string base_class = _belonging_class->metadata_info.object_info.symbol_info.symbol_name;

						 tFunctionInfo *sfi=NULL;

						 if(iao->scope_type == SCOPE_TYPE::ACCESS_SCOPE ){
							 iao->index_op1 = LOAD_TYPE_VARIABLE;
							 iao->index_op2 = idx_op-1;
						 }
						 else {

							 if(iao->scope_type == SCOPE_TYPE::THIS_SCOPE || iao->scope_type == SCOPE_TYPE::SUPER_SCOPE){

								 sfi = &_belonging_class->metadata_info.object_info;

							 }

							 if(iao->index_op1 == LOAD_TYPE_NOT_DEFINED){



										 string symbol_to_find =AST_SYMBOL_VALUE(iao->idxAstNode);

										 if(iao->scope_type ==SCOPE_TYPE::ACCESS_SCOPE){

											 print_info_cr("Symbol defined at line %i \"%s\" will solved at run-time",AST_LINE_VALUE(iao->idxAstNode), AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode));
										 }
										 else{

											// this/super required ...
											 if(sfi != NULL){
												 // search global...
												 if(iao->scope_type == SCOPE_TYPE::SUPER_SCOPE){
													 symbol_to_find = sfi->local_symbols.vec_idx_registeredFunction[k].object_info.symbol_info.symbol_name;
												 }


												 if(!searchVarFunctionSymbol(sfi,iao,k,iao->scope_type)){
													 if(iao->scope_type == SCOPE_TYPE::SUPER_SCOPE){
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
												 if(!searchVarFunctionSymbol(&info_function->object_info,iao,k,SCOPE_TYPE::LOCAL_SCOPE)){

													 // search global...
													 tScriptFunctionObject * mainFunctionInfo = getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_NAME);
													 if(!searchVarFunctionSymbol(&mainFunctionInfo->object_info,iao,k,SCOPE_TYPE::GLOBAL_SCOPE)){
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
						 if(stat->at(h).asm_op[iao->index_op1]->operator_type ==ASM_OPERATOR::LOAD  && // e
						    stat->at(h).asm_op[iao->index_op1]->scope_type == SCOPE_TYPE::ACCESS_SCOPE){

							 stat->at(h).asm_op[iao->index_op1]->index_op1 = LOAD_TYPE_FUNCTION;
							 stat->at(h).asm_op[iao->index_op1]->index_op2 = iao->index_op1-1;
							 iao->asm_properties = ASM_PROPERTY_CALLING_OBJECT;
						 }

					 }
				 }

			 }



			 /*vector<tScriptFunctionObject> *mrf = &info_function->object_info.local_symbols.vec_idx_registeredFunction;

			 for(unsigned k=0; k < mrf->size();k++){

				 if(!updateFunctionSymbols(&mrf->at(k), info_function->object_info.symbol_info.symbol_name, n_class,k)){
					 return false;
				 }


			 }	*/

			 if(!is_main_function){ // process all function symbols ...
				 for(unsigned m=0; m < info_function->object_info.local_symbols.vec_idx_registeredFunction.size(); m++){
					 if(!updateFunctionSymbols(&info_function->object_info.local_symbols.vec_idx_registeredFunction[m], info_function->object_info.symbol_info.symbol_name,m)){
						 return false;
					 }
				 }
			 }


		 }
	 }

	 return true;
}


 tInfoScriptClass * CScriptClassFactory::registerScriptClass(const string & class_name, const string & base_class_name, PASTNode _ast){
	int index;
	tInfoScriptClass *irv=NULL;

	if(class_name.empty()){
		print_error_cr("Class name empty");
		return NULL;
	}

	tInfoScriptClass *base_class=NULL;

	if(base_class_name != ""){
		if((base_class = this->getRegisteredClass(base_class_name)) == NULL){
			return NULL;
		}
	}


	if((index = getIdxRegisteredClass_Internal(class_name))==-1){ // check whether is local var registered scope ...


		irv = new tInfoScriptClass;
		irv->idxClass=m_registeredClass.size();
		irv->classPtrType = TYPE_SCRIPT_VARIABLE;
		irv->baseClass.push_back(base_class);
		irv->metadata_info.object_info.symbol_info.symbol_name = class_name;
		irv->metadata_info.object_info.symbol_info.idxAstNode=-1;

		if(_ast != NULL){
			irv->metadata_info.object_info.symbol_info.idxAstNode=_ast->idxAstNode;
		}

		irv->metadata_info.object_info.symbol_info.index = m_registeredClass.size();
		m_registeredClass.push_back(irv);

		return irv;

	}else{
		print_error_cr("error class \"%s\" already registered at line %i!", class_name.c_str(), AST_LINE_VALUE(m_registeredClass[index]->metadata_info.object_info.symbol_info.idxAstNode));
	}

	return NULL;
}


 CScriptVariable 		 * CScriptClassFactory::newScriptVariableByName(const string & class_name){

	 // 0. Search class info ...
	 tInfoScriptClass *rc = getRegisteredClass(class_name);


	 if(rc != NULL){
		 return newScriptVariableByIdx(rc->idxClass);
	 }

	 return NULL;

 }


 CScriptVariable 		 * CScriptClassFactory::newScriptVariableByIdx(unsigned idx, void * value_object){

	 CScriptVariable *class_object=NULL;

	 // 0. Search class info ...
	 tInfoScriptClass *rc = getRegisteredClassByIdx(idx);

	 if(rc != NULL){

		 // Is a primitive ?
		 if(rc->idxClass == IDX_CLASS_INTEGER){
			 class_object = NEW_INTEGER_VAR;
			 if(value_object!=NULL){
				 ((CInteger *)class_object)->m_intValue = *((int *)value_object);
			 }
		 }else if(rc->idxClass == IDX_CLASS_NUMBER){
			 class_object = NEW_NUMBER_VAR;
			 if(value_object!=NULL){
				 ((CNumber *)class_object)->m_floatValue = *((float *)value_object);
			 }
		 }else if(rc->idxClass ==IDX_CLASS_BOOLEAN){
			 class_object = NEW_BOOLEAN_VAR;
			 if(value_object!=NULL){
				 ((CBoolean *)class_object)->m_boolValue = *((bool *)value_object);
			 }
	 	 }else if(rc->idxClass ==IDX_CLASS_STRING){
			 class_object = NEW_STRING_VAR;
			 if(value_object!=NULL){
				 ((CString *)class_object)->m_strValue = *((string *)value_object);
			 }
	 	 }else if( // check internal class type
	 			   rc->idxClass ==IDX_CLASS_VECTOR
	 			|| rc->idxClass ==IDX_CLASS_STRUCT
				|| rc->idxClass ==IDX_CLASS_UNDEFINED
				|| rc->idxClass ==IDX_CLASS_NULL
				|| rc->idxClass ==IDX_CLASS_VOID
	 			 ){
			 class_object = (CScriptVariable *)value_object;
 	 	 }else{ // create C++ class ...
			 class_object = new CScriptVariable();
			 class_object->init(rc, value_object);
		 }


	 }

	 return class_object;

 }

/**
 * Register C variable
 */
bool  CScriptClassFactory::register_C_VariableInt(const string & var_name,void * var_ptr, const string & var_type)
{
	//CScope *scope;
	tInfoVariableSymbol *irs;

	if(var_ptr==NULL){
		print_error_cr("cannot register var \"%s\" with NULL reference value", var_name.c_str());
		return false;
	}

	int idxMainFunctionInfo = getIdxScriptFunctionObjectByClassFunctionName(MAIN_SCRIPT_CLASS_NAME,MAIN_SCRIPT_FUNCTION_NAME);

	if(idxMainFunctionInfo == -1){
		print_error_cr("main function is not created");
		return false;
	}

	if(getIdxClassFromIts_C_Type(var_type) == -1){
		print_error_cr("%s has not valid type (%s)",var_name.c_str(),var_type.c_str());
		return false;
	}



	if(!MAIN_AST_ROOT->registerSymbol(var_name)){
		return false;
	}

	//if((irs.idxScopeVar=CAst::getInstance()->getRootScope()->registerSymbol(var_name))==-1){
	//	return false;
	//}

	if((irs = FUNCTION_NEW_VARIABLE_SYMBOL(idxMainFunctionInfo)) != NULL){

		// init struct...
		irs->properties = ::PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF;
		irs->symbol_name = var_name;
		irs->ref_ptr=(int)var_ptr;

		print_info_cr("Registered function name: %s",var_name.c_str());


		return true;
	}

	return false;

}




tInfoVariableSymbol * CScriptClassFactory::registerVariableSymbol(const string & class_name,const string & var_name,PASTNode  ast){

	//tInfoScriptClass *rc = getRegisteredClass(class_name);
	//int idxClass
	tInfoScriptClass *rc = GET_CLASS_BY_NAME(class_name);// getIdxRegisteredClass(class_name);


	if(rc != NULL){//idxClass != -1){


		//tInfoScriptClass *rc = getRegisteredClassByIdx(idxClass);
		tFunctionInfo *object_info=&rc->metadata_info.object_info;

		tInfoVariableSymbol info_var;
		info_var.idxClassInfo = rc->idxClass;
		info_var.idxAstNode = ast->idxAstNode;
		info_var.symbol_name =var_name;
		info_var.index = object_info->local_symbols.m_registeredVariable.size();
		object_info->local_symbols.m_registeredVariable.push_back(info_var);

		return &object_info->local_symbols.m_registeredVariable[object_info->local_symbols.m_registeredVariable.size()-1];
	}else{
		print_error_cr("class \"%s\" not exist!",class_name.c_str());
		return NULL;
	}

	return NULL;
}

tInfoVariableSymbol *  CScriptClassFactory::getRegisteredVariableSymbol(const string & class_name,const string & function_name){

	tInfoScriptClass *rc = GET_CLASS_BY_NAME(class_name);

	if(rc != NULL){

		tFunctionInfo *object_info=&rc->metadata_info.object_info;

		for(unsigned i = 0; i < object_info->local_symbols.m_registeredVariable.size(); i++){
			if(object_info->local_symbols.m_registeredVariable[i].symbol_name == function_name){
				return &object_info->local_symbols.m_registeredVariable[i];
			}
		}
	/*if((index = getIdxFunctionSymbol_Internal(class_name))!=-1){ // check whether is local var registered scope ...

		return m_registeredClass[index];
	}else{
		if(print_msg){
			print_error_cr("class \"%s\" doesn't exist!", class_name.c_str());
		}*/
	}

	print_error_cr("variable member %s::%s doesn't exist",class_name.c_str(),function_name.c_str());

	return NULL;
}

int CScriptClassFactory::getIdxRegisteredVariableSymbol(tFunctionInfo *idxFunction ,const string & function_name, bool show_msg){

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




tFunctionInfo *  CScriptClassFactory::getSuperClass(tInfoScriptClass *irc, const string & fun_name){


	if(irc == NULL){ // trivial case ...
		return NULL;
	}

	for(unsigned i = 0; i < irc->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size(); i++){
		tScriptFunctionObject *sfo = GET_SCRIPT_FUNCTION_OBJECT(irc->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[i]);
		if(sfo->object_info.symbol_info.symbol_name == fun_name){
			return &irc->metadata_info.object_info;
		}
	}

	tInfoScriptClass *base = NULL;
	if(irc->baseClass.size() > 0){
		base = irc->baseClass[0];
	}

	return getSuperClass(base,fun_name);
}


//-------

tScriptFunctionObject * CScriptClassFactory::registerFunctionSymbol(const string & class_name, const string & fun_name, PASTNode  ast){

	//int idxClass =getIdxRegisteredClass(class_name);
	tInfoScriptClass *rc = GET_CLASS_BY_NAME(class_name);

	//if(idxClass != -1){
	if(rc != NULL){


		//tInfoScriptClass *rc = getRegisteredClassByIdx(idxClass);

		tFunctionInfo *object_info=&rc->metadata_info.object_info;

		tScriptFunctionObject *irs = CScriptFunctionObjectFactory::newScriptFunctionObject();

		irs->object_info.symbol_info.idxClassInfo = rc->idxClass;
		irs->object_info.symbol_info.symbol_name = fun_name;

		irs->object_info.symbol_info.idxAstNode = -1;
		if(ast != NULL){
			irs->object_info.symbol_info.idxAstNode = ast->idxAstNode;
		}


		if(fun_name == class_name){
			rc->idx_function_script_constructor = object_info->local_symbols.vec_idx_registeredFunction.size();
		}


		irs->object_info.symbol_info.index = object_info->local_symbols.vec_idx_registeredFunction.size();
		object_info->local_symbols.vec_idx_registeredFunction.push_back(irs->object_info.idxFunctionSymbol);

		return irs;
		//return &object_info->local_symbols.vec_idx_registeredFunction[object_info->local_symbols.vec_idx_registeredFunction.size()-1];
	}

	return NULL;
}

int CScriptClassFactory::getIdxScriptFunctionObjectByClassFunctionName(const string & class_name,const string & function_name, bool show_errors){

	tInfoScriptClass *rc =GET_CLASS_BY_NAME(class_name);

	if(rc != NULL){

		tFunctionInfo *object_info=&rc->metadata_info.object_info;

		// from lat value to first to get last override function...
		for(int i = object_info->local_symbols.vec_idx_registeredFunction.size()-1; i >= 0 ; i--){
			tFunctionInfo * object_info = GET_FUNCTION_INFO(object_info->local_symbols.vec_idx_registeredFunction[i]);
			if(object_info->symbol_info.symbol_name == function_name){
				return object_info->local_symbols.vec_idx_registeredFunction[i];
			}
		}
	}

	if(show_errors){
		print_error_cr("function member %s::%s doesn't exist",class_name.c_str(),function_name.c_str());
	}

	return -1;
}




//-----

bool CScriptClassFactory::addArgumentFunctionSymbol(const string & class_name,const string & function_name, const string & arg_name){


	int idxScriptFunctionObject = getIdxScriptFunctionObjectByClassFunctionName(class_name,function_name);


	if(idxScriptFunctionObject!=-1){

		GET_SCRIPT_FUNCTION_OBJECT(idxScriptFunctionObject)->m_arg.push_back(arg_name);

	}else{
		//print_error_cr("object info NULL");
	}

	return false;
}



const char * CScriptClassFactory::getNameRegisteredClassByIdx(int idx){
	if(idx != -1){
		return m_registeredClass[idx]->metadata_info.object_info.symbol_info.symbol_name.c_str();
	}
	 return "unknow";

}

void CScriptClassFactory::unloadRecursiveFunctions(tScriptFunctionObject * info_function){


	if((info_function->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){
		return;
	}

	print_info_cr("unloading local function %s...",info_function->object_info.symbol_info.symbol_name.c_str());
	for(unsigned k = 0; k < info_function->object_info.statment_op.size(); k++){

		for(unsigned a = 0; a  <info_function->object_info.statment_op[k].asm_op.size(); a++){

			delete info_function->object_info.statment_op[k].asm_op[a];
		}



	}

	for(unsigned f = 0; f < info_function->object_info.local_symbols.vec_idx_registeredFunction.size(); f++){
		unloadRecursiveFunctions(&info_function->object_info.local_symbols.vec_idx_registeredFunction[f]);
	}
}

CScriptClassFactory::~CScriptClassFactory() {

	for(unsigned i = 0;i<m_registeredClass.size();i++){

		tInfoScriptClass *irv = m_registeredClass[i];
		for(unsigned j = 0; j < irv->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size();j++){

			tScriptFunctionObject * info_function = &irv->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[j];

			print_info_cr("unloading %s::%s...",m_registeredClass[i]->metadata_info.object_info.symbol_info.symbol_name.c_str(), info_function->object_info.symbol_info.symbol_name.c_str());

			// C related functions ...
			if(i==0 && j==0){ // MAIN FUNCTION (C functions)

				// check external C functions defined inside main function ...
				for(unsigned f=0; f < info_function->object_info.local_symbols.vec_idx_registeredFunction.size();f++){

					tScriptFunctionObject * irv_main_function =  &info_function->object_info.local_symbols.vec_idx_registeredFunction[f];

					if((irv_main_function->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){

						/*if(irv_main_function->idx_return_type == getIdxClassVoid()){
							delete_proxy_function<void>(irv_main_function->m_arg.size(),(void *)irv_main_function->object_info.symbol_info.ref_ptr);//=(int)new_proxy_function<void>(irs.m_arg.size(),function_ptr))==0){//(int)function_ptr;

						}
						else{
							delete_proxy_function<int>(irv_main_function->m_arg.size(),(void *)irv_main_function->object_info.symbol_info.ref_ptr);
						}*/
					}
				}
			}
			else{ // normal class function member ...
				/*if(((info_function->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF)
				 &&((info_function->object_info.symbol_info.properties & PROPERTY_IS_DERIVATED) != PROPERTY_IS_DERIVATED)
				 ){

					delete ((std::function<void *(void *,PROXY_CREATOR)> *)	info_function->object_info.symbol_info.ref_ptr);
				}*/
			}

			info_function->object_info.symbol_info.ref_ptr = 0;


			unloadRecursiveFunctions(info_function);//->object_info.local_symbols.vec_idx_registeredFunction[f]);


			//bool is_main_function = i==0&&j==0;

			//if(!is_main_function){

			//}

		}



		if(irv->c_constructor != NULL){
			delete irv->c_constructor;
		}

		if(irv->c_destructor != NULL){
			delete irv->c_destructor;
		}

		if((irv->metadata_info.object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){ //ast created for compatibility reasons, we need to deallocate

			//delete irv->metadata_info.object_info.symbol_info.ast;
		}
		else{

			if(i==0){ // MAIN CLASS...


				//delete_proxy_function


			}else{ // generic class ...

				CScope *scope_info_ptr = AST_SCOPE_INFO(irv->metadata_info.object_info.symbol_info.idxAstNode);

				if(scope_info_ptr != NULL){

					scope_info_ptr->destroyScopes();
					delete scope_info_ptr;
				}
				//delete irv->metadata_info.object_info.symbol_info.ast;
			}
		}


		// delete tInfoScriptClass
		delete irv;
	}
}


