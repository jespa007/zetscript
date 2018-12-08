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

#define NEW_SCRIPT_FUNCTION(idxScope,idxScripClass)				CScriptFunction::newScriptFunctionObject(idxScope,idxScripClass)
#define GET_SCRIPT_FUNCTION(idx) 								CScriptFunction::getScriptFunctionObject(idx)
#define MAIN_FUNCTION											GET_SCRIPT_FUNCTION(0)

namespace zetscript{

	class  CScriptFunction:public CCommonClassFunctionData{


		/**
		 * build scope variable block information
		 */
		void buildLutScopeSymbols();

	public:

		typedef struct {
				string value;
		}tInfoInstruction;

		// info related for function ONLY

		vector<tArgumentInfo> m_arg; // tells var arg name or var type name (in of C )
		int idx_return_type; // -1 not inicialized type return.
		PtrInstruction instruction;

		std::map<short,tInfoInstruction> debug_info; // map that gives info about current instruction
		int idxScriptFunctionObject;


		tInfoVarScopeBlock *lut_scope_symbol;
		unsigned n_lut_scope_symbols;

		static void initStaticVars();
		static void destroyStaticVars();

		ZETSCRIPT_MODULE_EXPORT CScriptFunction(unsigned char _idxClass,short _idxScope );


		/**
		 * Set/Get CScriptClass Node by its idx, regarding current state.
		 */
		//static void 								setVectorScriptFunctionObjectNode(vector<CScriptFunction *> 	* set_vec);
		static vector<CScriptFunction *> 	*	getVectorScriptFunctionObjectNode();

		ZETSCRIPT_MODULE_EXPORT static CScriptFunction 			*	newScriptFunctionObject(  short idxClass
																							, short idxScope
																							, const string & function_name
																							, vector<tArgumentInfo> args={}
																							, int idx_return_type=ZS_UNDEFINED_IDX
																							, intptr_t ref_ptr=0
																							, unsigned short properties=0);

		ZETSCRIPT_MODULE_EXPORT static bool									checkCanRegister_C_Function(const char *f);

		ZETSCRIPT_MODULE_EXPORT static CScriptFunction 			* 	getScriptFunctionObject(int idx);

		virtual ~CScriptFunction();

	private:
		static vector<CScriptFunction *> 	* vec_script_function_node;

	};

}
