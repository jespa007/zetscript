#pragma once


#define GET_FUNCTION_INFO(idx) 					CScriptFunctionObjectFactory::getFunctionInfo(idx)
#define GET_SCRIPT_FUNCTION_OBJECT(idx) 		CScriptFunctionObjectFactory::getScriptFunctionObject(idx)
#define FUNCTION_NEW_VARIABLE_SYMBOL(idx_class)		(CScriptFunctionObjectFactory::newVariableSymbol(idx_class))    // 0 is the main class

class CScriptFunctionObjectFactory{

	static vector<tScriptFunctionObject *> 	* current_vec_script_function_object_node;

public:

	static void createSingletons();
	static void set(vector<tScriptFunctionObject *> 	& set_vec);
	/**
	 * Get tInfoScriptClass Node by its idx, regarding current state.
	 */
	static vector<tScriptFunctionObject *> 		*	getVecScriptFunctionObjectNode();
	static tScriptFunctionObject 			*	newScriptFunctionObject();
	static tInfoVariableSymbol				*	newVariableSymbol(int idxFunction);
	static tFunctionInfo					* 	getFunctionInfo(int idx);
	static tScriptFunctionObject 			* 	getScriptFunctionObject(int idx);
	static int						 			getIdxFunctionObject(int idxFunction,const string & function_name, bool show_msg=true);
	static int						 			getIdxVariableSymbol(int idxFunction,const string & function_name, bool show_msg=true);

	static void destroySingletons();

};
