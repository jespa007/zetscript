#pragma once

class CScriptClassFactory{

	static vector<tInfoRegisteredClass *> 			* vec_registered_class_node;
public:

	static void createSingletons();

	/**
	 * Get tInfoRegisteredClass Node by its idx, regarding current state.
	 */
	static int 				  		newScriptClass(tInfoRegisteredClass 	*registered_class);

	static vector<tInfoRegisteredClass *> 		*	getVecRegisteredClassNode();

	static tInfoRegisteredClass 	* getRegisteredClassNodeByIdx(int idx);
	static tInfoRegisteredClass 	* getRegisteredClassNodeByName(const string & name, bool print_msg=true);
	static tInfoRegisteredClass 	* getRegisteredClassNodeBy_C_ClassPtr(const string & class_type, bool print_msg=true);
	static int 						getIdxRegisteredClassNode_Internal(const string & class_name);
	static int 						getIdxRegisteredClassNode(const string & v, bool print_msg=true);
	static bool 					isClassRegistered(const string & v);

	static void destroySingletons();

};
