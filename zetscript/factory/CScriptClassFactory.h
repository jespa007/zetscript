#pragma once



#define REGISTERED_CLASS_NODE(idx) 				CScriptClassFactory::getRegisteredClassNodeByIdx(idx)
#define MAIN_CLASS_NODE							(CScriptClassFactory::getRegisteredClassNodeByIdx(0))    // 0 is the main class
#define GET_CLASS(idx)							(CScriptClassFactory::getRegisteredClassNodeByIdx(idx))    // 0 is the main class
#define GET_CLASS_BY_NAME(s)					(CScriptClassFactory::getRegisteredClassNodeByName(s))    // 0 is the main class
#define GET_CLASS_C_PTR_NAME(s)					(CScriptClassFactory::getRegisteredClassNodeBy_C_ClassPtr(s))    // 0 is the main class



class CScriptClassFactory{

	static vector<tInfoRegisteredClass *> 			* vec_registered_class_node;
public:

	static void createSingletons();
	static void set(vector<tInfoRegisteredClass *> 	& set_vec);

	/**
	 * Get tInfoRegisteredClass Node by its idx, regarding current state.
	 */
	static tInfoRegisteredClass 				*	newScriptClass();
	static vector<tInfoRegisteredClass *> 		*	getVecScriptClassNode();

	static tInfoRegisteredClass 				* 	getRegisteredClassNodeByIdx(int idx);
	static tInfoRegisteredClass 				* 	getRegisteredClassNodeByName(const string & name, bool print_msg=true);
	static tInfoRegisteredClass 				* 	getRegisteredClassNodeBy_C_ClassPtr(const string & class_type, bool print_msg=true);
	static int 										getIdxRegisteredClassNode_Internal(const string & class_name);
	static int 										getIdxRegisteredClassNode(const string & v, bool print_msg=true);
	static int 										getIdxClassFromIts_C_Type(const string & c_type_str);
	static bool 									isClassRegistered(const string & v);

	static void destroySingletons();

};
