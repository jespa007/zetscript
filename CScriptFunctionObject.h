/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

/*
//	 _____           _       _    ______                _   _
//	/  ___|         (_)     | |   |  ___|              | | (_)
//	\ `--.  ___ _ __ _ _ __ | |_  | |_ _   _ _ __   ___| |_ _  ___  _ __
//	 `--. \/ __| '__| | '_ \| __| |  _| | | | '_ \ / __| __| |/ _ \| '_ \
//	/\__/ / (__| |  | | |_) | |_  | | | |_| | | | | (__| |_| | (_) | | | |
//	\____/ \___|_|  |_| .__/ \__| \_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|
//                    | |
//                    |_|
// 	_________________________________________________
//  	__________________________________
//
*/

#define NEW_SCRIPT_FUNCTION_OBJECT(idxScope,idxScripClass)				CScriptFunction::newScriptFunctionObject(idxScope,idxScripClass)

#define GET_SCRIPT_FUNCTION_OBJECT(idx) 		CScriptFunction::getScriptFunctionObject(idx)
//#define FUNCTION_NEW_VARIABLE_SYMBOL(idx_class)	(CScriptFunction::newVariableSymbol(idx_class))    // 0 is the main class

#define MAIN_FUNCTION_OBJECT					GET_SCRIPT_FUNCTION_OBJECT(0)

namespace zetscript{

	class  CScriptFunction{

	public:

		tScopeInfo			scope_info;
		tVariableSymbolInfo symbol_info;

		// info related for function ONLY
		vector<tArgumentInfo> m_arg; // tells var arg name or var type name (in of C )
		int idx_return_type; // -1 not inicialized type return.
		PtrInstruction instruction;
		int idxScriptFunctionObject;


		CScriptFunction( short _idxScope,  short _idxScriptClass ){
			//virtual_function = NULL;
			idx_return_type = ZS_UNDEFINED_IDX;
			idxScriptFunctionObject = ZS_UNDEFINED_IDX;
			instruction=NULL;
			scope_info.info_var_scope=NULL;
			scope_info.idxScope=_idxScope;
			symbol_info.idxScriptClass = _idxScriptClass;

		}

		// new/get function/variable it returns the idx vector element on symbol_info.scope_info.[vRegisteredFunction/vRegisteredVariables]
		int						 				registerFunction(short idxLocalScope, const string & function_name, vector<tArgumentInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);


		virtual int				 				registerLocalFunction(const string & function_name, vector<tArgumentInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);
		int 									registerLocalVariable(const string & variable,const string & variable_ref, const string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0);
		int						 				getLocalFunction(const string & function_ref,char n_args=0);
		int						 				getLocalVariable(const string & variable_ref);


		/**
		 * Set/Get CScriptClass Node by its idx, regarding current state.
		 */
		static void 								setVectorScriptFunctionObjectNode(vector<CScriptFunction *> 	* set_vec);
		static vector<CScriptFunction *> 	*	getVectorScriptFunctionObjectNode();

		ZETSCRIPT_MODULE_EXPORT static CScriptFunction 			*	newScriptFunctionObject(short idxScope, short idxScriptClass);
		ZETSCRIPT_MODULE_EXPORT static bool									checkCanRegister_C_Function(const char *f);
		//static tVariableSymbolInfo				*	newVariableSymbol(int idxFunction);

		ZETSCRIPT_MODULE_EXPORT static CScriptFunction 			* 	getScriptFunctionObject(int idx);

	private:
		static vector<CScriptFunction *> 	* current_vec_script_function_object_node;

	};

}
