#include "zs_factory.h"


vector<tInfoRegisteredClass *> 				* vec_registered_class_node=NULL;

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
		vec_registered_class_node = new vector<tInfoRegisteredClass *>();
	}
}

void CScriptClassFactory::destroySingletons(){
	if(vec_registered_class_node!=NULL){
		delete vec_registered_class_node;
		vec_registered_class_node=NULL;
	}
}

void CScriptClassFactory::set(vector<tInfoRegisteredClass *> 	& set_vec){
	*vec_registered_class_node = set_vec;
}


vector<tInfoRegisteredClass *> 		*	CScriptClassFactory::getVecScriptClassNode(){
	return vec_registered_class_node;
}


tInfoRegisteredClass *	CScriptClassFactory::newScriptClass(){
	tInfoRegisteredClass 	*registered_class=new tInfoRegisteredClass;
	vec_registered_class_node->push_back(registered_class);
	registered_class->idxClass = vec_registered_class_node->size()-1;
	return registered_class;
}



tInfoRegisteredClass 	* CScriptClassFactory::getRegisteredClassNodeByIdx(int idx){
	if(idx < 0 || (unsigned)idx >= vec_registered_class_node->size()){
		print_error_cr("tScopeVar node out of bound");
		return NULL;
	}

	return vec_registered_class_node->at(idx);
}

tInfoRegisteredClass 	* CScriptClassFactory::getRegisteredClassNodeByName(const string & class_name, bool print_msg){
	int index;
	if((index = getIdxRegisteredClassNode_Internal(class_name))!=-1){ // check whether is local var registered scope ...

		return vec_registered_class_node->at(index);
	}else{
		if(print_msg){
			print_error_cr("class \"%s\" doesn't exist!", class_name.c_str());
		}
	}

	return NULL;
}




tInfoRegisteredClass *CScriptClassFactory::getRegisteredClassNodeBy_C_ClassPtr(const string & class_type, bool print_msg){

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

int CScriptClassFactory::getIdxRegisteredClassNode_Internal(const string & class_name){

	for(unsigned i = 0; i < vec_registered_class_node->size(); i++){
		if(class_name == vec_registered_class_node->at(i)->metadata_info.object_info.symbol_info.symbol_name){
			return i;
		}
	}

	return -1;
}


int CScriptClassFactory::getIdxRegisteredClassNode(const string & v, bool print_msg){
	int index= getIdxRegisteredClassNode_Internal(v);

	if(index == -1 && print_msg){
		print_error_cr("class %s not registered",v.c_str());
	}

	return index;
}

bool CScriptClassFactory::isClassRegistered(const string & v){
	return getIdxRegisteredClassNode_Internal(v) != -1;
}
