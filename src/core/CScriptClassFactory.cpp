#include "zg_core.h"

 CScriptClassFactory *  	 CScriptClassFactory::scriptClassFactory=NULL;
 CScriptClassFactory::tPrimitiveType CScriptClassFactory::primitiveType[MAX_VAR_C_TYPES];


 //--obj , type convert, ---
 map<string,map<string,fntConversionType>> mapTypeConversion;

 template<typename _S, typename _D, typename _F>
 bool addTypeConversion(_F f){

 	bool valid_type = false;

 	// check if any entry is int, *float, *bool , *string, *int or any from factory. Anyelese will be no allowed!
 	valid_type|=CScriptClassFactory::primitiveType[CScriptClassFactory::VOID_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(void).name(),"void",VOID_TYPE};
 	valid_type|=CScriptClassFactory::primitiveType[CScriptClassFactory::INT_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(int).name(),"int",INT_TYPE};
 	valid_type|=CScriptClassFactory::primitiveType[CScriptClassFactory::INT_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(int *).name(),"int *",INT_PTR_TYPE};
 	valid_type|=CScriptClassFactory::primitiveType[CScriptClassFactory::FLOAT_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(float *).name(),"float *",FLOAT_PTR_TYPE};
 	valid_type|=CScriptClassFactory::primitiveType[CScriptClassFactory::STRING_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(string *).name(),"string *",STRING_PTR_TYPE};
 	valid_type|=CScriptClassFactory::primitiveType[CScriptClassFactory::BOOL_PTR_TYPE].type_str==string(typeid(_D).name()); ;//={typeid(bool *).name(),"bool *",BOOL_PTR_TYPE};

 	if(!valid_type){
 		print_error_cr("Conversion type \"%s\" not valid",typeid(_D).name());
 		return false;
 	}



 	if(mapTypeConversion.count(typeid(_S).name()) == 1){ // create new map...
 		if(mapTypeConversion[typeid(_S).name()].count(typeid(_D).name())==1){
 			print_error_cr("type conversion \"%s\" to \"%s\" already inserted",typeid(_S).name(),typeid(_D).name());
 			return false;
 		}
 	}

 	mapTypeConversion[typeid(_S).name()][typeid(_D).name()]=f;

 	return true;
 	//typeConversion["P7CNumber"]["Ss"](&n);
 }



 fntConversionType CScriptClassFactory::getConversionType(string objectType, string conversionType){

 	if(mapTypeConversion.count(objectType) == 0){
 		print_error_cr("There's no type conversion \"%s\". Add conversion types through \"addTypeConversion\" function",objectType.c_str());
 		return NULL;
 	}

 	if(mapTypeConversion[objectType].count(conversionType) == 0){
 		print_error("There's no CONVERSION from type \"%s\" to type \"%s\"",objectType.c_str(),conversionType.c_str());
 		printf("\n\tAvailable types are:");
 		for(map<string, fntConversionType>::iterator j =mapTypeConversion[objectType].begin() ; j != mapTypeConversion[objectType].end();j++){
 			printf("\n\t\t* \"%s\"", j->first.c_str());
 		}

 		return NULL;
 	}

 	return mapTypeConversion[objectType][conversionType];


 }

 void CScriptClassFactory::registerPrimitiveTypes(){

 	primitiveType[VOID_TYPE]={typeid(void).name(),"void",VOID_TYPE};
 	primitiveType[INT_TYPE]={typeid(int).name(),"int",INT_TYPE};
 	primitiveType[INT_PTR_TYPE]={typeid(int *).name(),"int *",INT_PTR_TYPE};
 	primitiveType[FLOAT_TYPE]={typeid(float).name(),"float",FLOAT_TYPE};
 	primitiveType[FLOAT_PTR_TYPE]={typeid(float *).name(),"float *",FLOAT_PTR_TYPE};
 	primitiveType[STRING_TYPE]={typeid(string).name(),"string",STRING_TYPE};
 	primitiveType[STRING_PTR_TYPE]={typeid(string *).name(),"string *",STRING_PTR_TYPE};
 	primitiveType[BOOL_TYPE]={typeid(bool).name(),"bool",BOOL_TYPE};
 	primitiveType[BOOL_PTR_TYPE]={typeid(bool *).name(),"bool *",BOOL_PTR_TYPE};
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


 CScriptClassFactory::CScriptClassFactory(){

 }

 CScriptClassFactory::tPrimitiveType *CScriptClassFactory::getPrimitiveTypeFromStr(const string & str){

 	for(unsigned i=0; i < MAX_VAR_C_TYPES; i++){
 		if(primitiveType[i].type_str == str){
 			return &primitiveType[i];
 		}
 	}

 	print_error_cr("type \"%s\" is not registered",str.c_str());

 	return NULL;
 }

 tInfoRegisteredClass * CScriptClassFactory::registerScriptClass(const string & class_name){
	int index;
	tInfoRegisteredClass *irv=NULL;

	if(class_name.empty()){
		print_error_cr("Class name empty");
		return NULL;
	}

	if((index = getIdxRegisteredClass_Internal(class_name))==-1){ // check whether is local var registered scope ...

		irv = new tInfoRegisteredClass;
		irv->baseClass=NULL;
		irv->object_info.symbol_info.symbol_name = class_name;
		//irv->ast=ast;
		m_registeredClass.push_back(irv);

		return irv;

	}else{
		print_error_cr("error class \"%s\" already registered at line %i!", class_name.c_str(), m_registeredClass[index]->object_info.symbol_info.ast->definedValueline);
	}

	return NULL;
}


 CScriptClass 		 * CScriptClassFactory::newClass(const string & class_name){

	 CScriptClass *class_object=NULL;

	 // 0. Search class info ...
	 tInfoRegisteredClass *rc = getRegisteredClass(class_name);

	 if(rc != NULL){

		 // 1. Create the object ...
		 class_object = new CScriptClass(rc);


	 }

	 return class_object;

 }


/**
 * Register C variable
 */
void CScriptClassFactory::register_C_Variable(const string & class_name, const string & var_str,void * var_ptr, const string & var_type)
{
	CScopeInfo *scope;

	if(var_ptr==NULL){
		print_error_cr("cannot register var \"%s\" with NULL reference value", var_str.c_str());
		return;
	}

	tInfoRegisteredClass *rc = getRegisteredClass(class_name);

	if(rc != NULL){
		return;
	}

	if((scope = rc->object_info.symbol_info.ast->scope_info_ptr) == NULL){
		print_error_cr("scope null");
		return;
	}


	//CZG_ScriptCore::tPrimitiveType *type;
	//tInfoScopeVar *irv;
	//string human_str;


}




tInfoRegisteredVariableSymbol * CScriptClassFactory::registerVariableSymbol(const string & class_name,const string & var_name,PASTNode  ast){

	tInfoRegisteredClass *rc = getRegisteredClass(class_name);


	if(rc != NULL){

		tScriptFunctionInfo *object_info=&rc->object_info;

		tInfoRegisteredVariableSymbol info_var;
		info_var.ast = ast;
		info_var.symbol_name =var_name;
		object_info->local_symbols.m_registeredVariable.push_back(info_var);

		return &object_info->local_symbols.m_registeredVariable[object_info->local_symbols.m_registeredVariable.size()-1];
	}else{
		print_error_cr("object_info null!");
		return NULL;
	}

	return NULL;
}

tInfoRegisteredVariableSymbol *  CScriptClassFactory::getRegisteredVariableSymbol(const string & class_name,const string & function_name){

	tInfoRegisteredClass *rc = getRegisteredClass(class_name);

	if(rc != NULL){

		tScriptFunctionInfo *object_info=&rc->object_info;

		for(unsigned i = 0; i < object_info->local_symbols.m_registeredVariable.size(); i++){
			if(object_info->local_symbols.m_registeredVariable[i].symbol_name == function_name){
				return &object_info->local_symbols.m_registeredVariable[i];
			}
		}
	/*if((index = getIdxRegisteredFunctionSymbol_Internal(class_name))!=-1){ // check whether is local var registered scope ...

		return m_registeredClass[index];
	}else{
		if(print_msg){
			print_error_cr("class \"%s\" doesn't exist!", class_name.c_str());
		}*/
	}

	print_error_cr("variable member %s::%s doesn't exist",class_name.c_str(),function_name.c_str());

	return NULL;
}

tInfoRegisteredFunctionSymbol * CScriptClassFactory::registerFunctionSymbol(const string & class_name, const string & fun_name, PASTNode  ast){




	tInfoRegisteredClass *rc = getRegisteredClass(class_name);


	if(rc != NULL){
		if(getRegisteredFunctionSymbol(class_name,fun_name, false)==NULL){


			tScriptFunctionInfo *object_info=&rc->object_info;

			tInfoRegisteredFunctionSymbol irs;

			irs.object_info.symbol_info.symbol_name = fun_name;
			irs.object_info.symbol_info.ast = ast;
			irs.object_info.symbol_info.properties = 0;
			object_info->local_symbols.m_registeredFunction.push_back(irs);

			return &object_info->local_symbols.m_registeredFunction[object_info->local_symbols.m_registeredFunction.size()-1];
		}
		else{
			print_error_cr("function member %s::%s already registered",class_name.c_str(),fun_name.c_str());
		}
	}else{
		//print_error_cr("object info NULL");
	}

	return NULL;
}




tInfoRegisteredFunctionSymbol * CScriptClassFactory::getRegisteredFunctionSymbol(const string & class_name,const string & function_name, bool show_errors){

	tInfoRegisteredClass *rc = getRegisteredClass(class_name);

	if(rc != NULL){

		tScriptFunctionInfo *object_info=&rc->object_info;

		for(unsigned i = 0; i < object_info->local_symbols.m_registeredFunction.size(); i++){
			if(object_info->local_symbols.m_registeredFunction[i].object_info.symbol_info.symbol_name == function_name){
				return &object_info->local_symbols.m_registeredFunction[i];
			}
		}
	/*if((index = getIdxRegisteredFunctionSymbol_Internal(class_name))!=-1){ // check whether is local var registered scope ...

		return m_registeredClass[index];
	}else{
		if(print_msg){
			print_error_cr("class \"%s\" doesn't exist!", class_name.c_str());
		}*/
	}

	if(show_errors){
		print_error_cr("function member %s::%s doesn't exist",class_name.c_str(),function_name.c_str());
	}

	return NULL;
}


bool CScriptClassFactory::addArgumentFunctionSymbol(const string & class_name,const string & function_name, const string & arg_name){


	tInfoRegisteredFunctionSymbol *object_info = getRegisteredFunctionSymbol(class_name,function_name);


	if(object_info!=NULL){

		object_info->m_arg.push_back(arg_name);

	}else{
		//print_error_cr("object info NULL");
	}

	return false;
}

int CScriptClassFactory::getIdxRegisteredClass_Internal(const string & class_name){

	for(unsigned i = 0; i < m_registeredClass.size(); i++){
		if(class_name == m_registeredClass[i]->object_info.symbol_info.symbol_name){
			return i;
		}
	}

	return -1;
}

tInfoRegisteredClass *CScriptClassFactory::getRegisteredClass(const string & class_name, bool print_msg){

	int index;
	if((index = getIdxRegisteredClass_Internal(class_name))!=-1){ // check whether is local var registered scope ...

		return m_registeredClass[index];
	}else{
		if(print_msg){
			print_error_cr("class \"%s\" doesn't exist!", class_name.c_str());
		}
	}

	return NULL;

}



int CScriptClassFactory::getIdxRegisteredClass(const string & v){
	int index= getIdxRegisteredClass_Internal(v);

	if(index == -1){
		print_error_cr("class %s not registered",v.c_str());
	}

	return index;
}

bool CScriptClassFactory::isClassRegistered(const string & v){
	return getIdxRegisteredClass_Internal(v) != -1;
}

const char * CScriptClassFactory::getNameRegisteredClassByIdx(int idx){
	if(idx != -1){
		return m_registeredClass[idx]->object_info.symbol_info.ast->value_symbol.c_str();
	}
	 return "unknow";

}

CScriptClassFactory::~CScriptClassFactory() {

	for(unsigned i = 0;i<m_registeredClass.size();i++){

			tInfoRegisteredClass *irv = m_registeredClass[i];
		    for(unsigned i = 0; i < irv->object_info.local_symbols.m_registeredFunction.size();i++){
		    	for(unsigned j = 0; j < irv->object_info.local_symbols.m_registeredFunction[i].object_info.statment_op.size(); j++){
		    		for(unsigned a = 0; a  <irv->object_info.local_symbols.m_registeredFunction[i].object_info.statment_op[j].asm_op.size(); a++){

		    			delete irv->object_info.local_symbols.m_registeredFunction[i].object_info.statment_op[j].asm_op[a];
		    		}

		    	}
		    }


		    // delete tInfoRegisteredClass
			delete irv;
	}
}

