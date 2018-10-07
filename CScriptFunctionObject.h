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

#define NEW_SCRIPT_FUNCTION_OBJECT				CScriptFunctionObject::newScriptFunctionObject()

#define GET_SCRIPT_FUNCTION_OBJECT(idx) 		CScriptFunctionObject::getScriptFunctionObject(idx)
#define FUNCTION_NEW_VARIABLE_SYMBOL(idx_class)	(CScriptFunctionObject::newVariableSymbol(idx_class))    // 0 is the main class


namespace zetscript{

	class  CScriptFunctionObject{



	public:

		tScopeInfo			scope_info;
		tVariableSymbolInfo symbol_info;

		//CScriptFunctionObject *virtual_function;

		// var for function ...
		vector<tArgumentInfo> m_arg; // tells var arg name or var type name (in of C )
		int idx_return_type; // -1 not inicialized type return.
		PtrAsmOp asm_op;

		int idxScriptFunctionObject;
		short idxScope;


		CScriptFunctionObject(){
			//virtual_function = NULL;
			idx_return_type = -1;
			idxScriptFunctionObject = ZS_UNDEFINED_IDX;
			asm_op=NULL;
			scope_info.info_var_scope=NULL;
			idxScope=IDX_INVALID_SCOPE;
		}

		/**
		 * Set/Get CScriptClass Node by its idx, regarding current state.
		 */
		static void 								setVectorScriptFunctionObjectNode(vector<CScriptFunctionObject *> 	* set_vec);
		static vector<CScriptFunctionObject *> 	*	getVectorScriptFunctionObjectNode();

		ZETSCRIPT_MODULE_EXPORT static CScriptFunctionObject 			*	newScriptFunctionObject();
		ZETSCRIPT_MODULE_EXPORT static bool									checkCanRegister_C_Function(const char *f);
		static tVariableSymbolInfo				*	newVariableSymbol(int idxFunction);

		ZETSCRIPT_MODULE_EXPORT static CScriptFunctionObject 			* 	getScriptFunctionObject(int idx);
		static int						 			getIdxFunctionObject(CScriptFunctionObject * in_sfo,const string & function_name,char n_args=-1, bool show_msg=true);
		static int						 			getIdxVariableSymbol(CScriptFunctionObject * in_sfo,const string & function_name, bool show_msg=true);

	private:
		static vector<CScriptFunctionObject *> 	* current_vec_script_function_object_node;

	};

}
