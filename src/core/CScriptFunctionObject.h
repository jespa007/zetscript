#pragma once

#define NEW_SCRIPT_FUNCTION_OBJECT				CScriptFunctionObject::newScriptFunctionObject()
#define GET_FUNCTION_INFO(idx) 					CScriptFunctionObject::getFunctionInfo(idx)
#define GET_SCRIPT_FUNCTION_OBJECT(idx) 		CScriptFunctionObject::getScriptFunctionObject(idx)
#define FUNCTION_NEW_VARIABLE_SYMBOL(idx_class)	(CScriptFunctionObject::newVariableSymbol(idx_class))    // 0 is the main class

class CScriptFunctionObject{



public:

	tFunctionInfo	object_info;

	//CScriptFunctionObject *virtual_function;

	// var for function ...
	vector<std::string> m_arg; // tells var arg name or var type name (in of C )
	int idx_return_type; // -1 not inicialized type return.

	CScriptFunctionObject(){
		//virtual_function = NULL;
		idx_return_type = -1;
		object_info.idxScriptFunctionObject = -1;
	}

	static void set(vector<CScriptFunctionObject *> 	* set_vec);
	/**
	 * Get CScriptClass Node by its idx, regarding current state.
	 */
	static vector<CScriptFunctionObject *> 	*	getVecScriptFunctionObjectNode();
	static CScriptFunctionObject 			*	newScriptFunctionObject();
	static tInfoVariableSymbol				*	newVariableSymbol(int idxFunction);
	static tFunctionInfo					* 	getFunctionInfo(int idx);
	static CScriptFunctionObject 			* 	getScriptFunctionObject(int idx);
	static int						 			getIdxFunctionObject(int idxFunction,const string & function_name, bool show_msg=true);
	static int						 			getIdxVariableSymbol(int idxFunction,const string & function_name, bool show_msg=true);

private:
	static vector<CScriptFunctionObject *> 	* current_vec_script_function_object_node;

};
