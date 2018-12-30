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


namespace zetscript{

	class  CScriptFunction:public CCommonClassFunctionBase{





		string cfile; // file where function was compiled.


	public:

		typedef struct {
				string value;
		}tInfoInstruction;

		// info related for function ONLY

		vector<tArgumentInfo> m_arg; // tells var arg name or var type name (in of C )
		int idx_return_type; // -1 not inicialized type return.
		PtrInstruction instruction;

		//-----------
		//  DEBUG
		std::map<short,string> value_symbol; // map that gives info about current instruction
		int 		 getLine(tInstruction * ins);
		const char * getSymbol(tInstruction * ins);
		const char * getFile();

		//  DEBUG
		//-----------


		int idxScriptFunction;
		short idxLocalFunction;


		tInfoVarScopeBlock *lut_scope_symbol;
		unsigned n_lut_scope_symbols;

		//static void initStaticVars();
		//static void destroyStaticVars();

		ZETSCRIPT_MODULE_EXPORT CScriptFunction(unsigned char _idxClass,short _idxScope );

		tVariableSymbolInfo *  registerVariable(const string & variable, const string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0);


		/**
		 * build scope variable block information
		 */
		void buildLutScopeSymbols();


		virtual ~CScriptFunction();


	};

}
