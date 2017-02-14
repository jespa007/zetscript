#pragma once

class CScriptFunctionObjectFactory{

	static vector<tScriptFunctionObject *> 	* current_vec_script_function_object_node;

public:

	static void createSingletons();

	/**
	 * Get tInfoRegisteredClass Node by its idx, regarding current state.
	 */
	static vector<tScriptFunctionObject *> *	getCurrentVecInfoRegisteredFunctionFunctionNode();
	static int 				  			newFunctionSymbol(tScriptFunctionObject *script_function);
	static tFunctionInfo				* getScriptFunctionNodeByIdx(int idx);
	static tScriptFunctionObject 	* getFunctionSymbolNodeByIdx(int idx);

	static void destroySingletons();

};
