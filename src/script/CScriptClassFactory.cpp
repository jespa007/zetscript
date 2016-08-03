#include "zg_script.h"

 map<string,tInfoRegisteredClass *>  	 CScriptClassFactory::m_registeredClass;
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

 CScriptClassFactory::tPrimitiveType *CScriptClassFactory::getPrimitiveTypeFromStr(const string & str){

 	for(unsigned i=0; i < MAX_VAR_C_TYPES; i++){
 		if(primitiveType[i].type_str == str){
 			return &primitiveType[i];
 		}
 	}

 	print_error_cr("type \"%s\" is not registered",str.c_str());

 	return NULL;
 }

 tInfoRegisteredClass * CScriptClassFactory::registerClass(const string & class_name){
	tInfoRegisteredClass * irv;
	if((irv = registeredClassExists(class_name))==NULL){ // check whether is local var registered scope ...

		irv = new tInfoRegisteredClass;
		//irv->ast=ast;
		m_registeredClass[class_name]=irv;

		return irv;

	}else{
		print_error_cr("error class \"%s\" already registered at line %i!", class_name.c_str(), irv->object_info.symbol_info.ast->definedValueline);
	}

	return NULL;
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


	//CZG_Script::tPrimitiveType *type;
	tInfoScopeVar *irv;
	string human_str;

	if((irv=scope->registerSymbol(var_str))!= NULL){ // registered symbol var...


			if(CFactoryContainer::getInstance()->classPtrIsRegistered(var_type)){ // type exists ...
				//irv->m_obj = (CObject *)var_ptr;

				human_str = var_type;
			}else{
				/*type = getPrimitiveTypeFromStr(var_type);
				if(type != NULL){

					human_str = type->human_description_str;

				switch(type->id){
				// 1. first try primitives ...
				default:
					print_error_cr("Cannot know how to set \"%s\" as \"%s\" type variable.",var_str,type->human_description_str.c_str());
					return;
					break;
				case INT_PTR_TYPE:
					irv->m_obj=NEW_INTEGER();
					((CVariable *)irv->m_obj)->setPtr(var_ptr);
					break;
				case FLOAT_PTR_TYPE:
					irv->m_obj=NEW_NUMBER();
					((CVariable *)irv->m_obj)->setPtr(var_ptr);
					break;
				case STRING_PTR_TYPE:
					irv->m_obj=NEW_STRING();
					((CVariable *)irv->m_obj)->setPtr(var_ptr);
					break;
				case BOOL_PTR_TYPE:
					irv->m_obj=NEW_BOOLEAN();
					((CVariable *)irv->m_obj)->setPtr(var_ptr);
					break;

				}
				}*/

			}



		/*if(irv->m_obj != NULL){
			print_info_cr("Registered \"%s\" as \"%s\"",var_str, human_str.c_str());
		}*/
	}
}

tInfoRegisteredFunctionSymbol * CScriptClassFactory::registerFunctionSymbol(tBaseObjectInfo *object_info,PASTNode  ast, unsigned int properties){



	if(object_info != NULL){

		tInfoRegisteredFunctionSymbol irs;

		irs.object_info.symbol_info.ast = ast;
		irs.object_info.symbol_info.properties = properties;
		object_info->member_data.m_registeredFunction.push_back(irs);

		return &object_info->member_data.m_registeredFunction[object_info->member_data.m_registeredFunction.size()-1];
	}else{
		print_error_cr("object info NULL");
	}

	return NULL;
}


tInfoRegisteredVariableSymbol * CScriptClassFactory::registerVariableSymbol(tBaseObjectInfo *object_info,PASTNode  ast){

	//tInfoRegisteredClass *rc = getRegisteredClass(class_name);

	if(object_info != NULL){

		tInfoRegisteredVariableSymbol irs;
		irs.ast = ast;
		object_info->member_data.m_registeredSymbol.push_back(irs);

		return &object_info->member_data.m_registeredSymbol[object_info->member_data.m_registeredSymbol.size()-1];
	}else{
		print_error_cr("object_info null!");
		return NULL;
	}

	return NULL;
}

tInfoRegisteredFunctionSymbol *  CScriptClassFactory::getRegisteredFunctionSymbol(tBaseObjectInfo *object_info,unsigned idx){


	if(object_info != NULL){
		if(idx<object_info->member_data.m_registeredFunction.size()){
			return &object_info->member_data.m_registeredFunction[idx];
		}else{
			print_error_cr("Function index out of bounds");
		}
	}else{
		print_error_cr("object info NULL");
	}

	return NULL;
}

bool CScriptClassFactory::addArgumentFunctionSymbol(tBaseObjectInfo *object_info,unsigned idx, const string & var_name){



	if(object_info != NULL){

		if(idx<object_info->member_data.m_registeredFunction.size()){
			object_info->member_data.m_registeredFunction[idx].m_arg.push_back(var_name);
			return true;
		}else{
			print_error_cr("Function index out of bounds");
		}
	}else{
		print_error_cr("object info NULL");
	}

	return false;
}

tInfoRegisteredClass * CScriptClassFactory::registeredClassExists(const string & class_name){
	if(m_registeredClass.count(class_name)==1){ // not exit but we will deepth through parents ...
		return m_registeredClass[class_name];
	}

	return NULL;
}

tInfoRegisteredClass *CScriptClassFactory::getRegisteredClass(const string & class_name, bool print_msg){
	tInfoRegisteredClass * irv;
	if((irv = registeredClassExists(class_name))!=NULL){ // check whether is local var registered scope ...

		return irv;
	}else{
		if(print_msg){
			print_error_cr("class \"%s\" doesn't exist!", class_name.c_str());
		}
	}

	return NULL;

}

void CScriptClassFactory::destroySingletons() {

	for(map<string,tInfoRegisteredClass *>::iterator it = m_registeredClass.begin();it!= m_registeredClass.end();it++){

		    for(unsigned i = 0; i < it->second->object_info.member_data.m_registeredFunction.size();i++){
		    	for(unsigned j = 0; j < it->second->object_info.member_data.m_registeredFunction[i].object_info.statment_op.size(); j++){
		    		for(unsigned a = 0; a  <it->second->object_info.member_data.m_registeredFunction[i].object_info.statment_op[j].asm_op.size(); a++){

		    			delete it->second->object_info.member_data.m_registeredFunction[i].object_info.statment_op[j].asm_op[a];
		    		}

		    	}
		    }


		    // delete tInfoRegisteredClass
			delete it->second;
	}
}
