#pragma once


#define GET_FUNCTION_INFO(idx) 					CScriptFunctionObjectFactory::getFunctionInfo(idx)
#define GET_SCRIPT_FUNCTION_OBJECT(idx) 		CScriptFunctionObjectFactory::getScriptFunctionObject(idx)


class CScriptFunctionObjectFactory{

	static vector<tScriptFunctionObject *> 	* current_vec_script_function_object_node;

public:

	static void createSingletons();
	static void set(vector<tScriptFunctionObject *> 	& set_vec);
	/**
	 * Get tInfoRegisteredClass Node by its idx, regarding current state.
	 */
	static vector<tScriptFunctionObject *> 		*	getVecScriptFunctionObjectNode();
	static tScriptFunctionObject 			*	newFunctionSymbol();
	static tFunctionInfo					* 	getFunctionInfo(int idx);
	static tScriptFunctionObject 			* 	getScriptFunctionObject(int idx);

	static void destroySingletons();

};
