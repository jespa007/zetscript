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


#define INSTRUCTION_GET_FILE_LINE(__FUNC__,__INS__) 	((zetscript::CScriptFunction *)__FUNC__)->INSTRUCTION_getFile(__INS__), ((zetscript::CScriptFunction *)__FUNC__)->INSTRUCTION_getLine(__INS__)
#define INSTRUCTION_GET_FILE(__FUNC__,__INS__) 			((zetscript::CScriptFunction *)__FUNC__)->INSTRUCTION_getFile(__INS__)
#define INSTRUCTION_GET_LINE(__FUNC__,__INS__)			((zetscript::CScriptFunction *)__FUNC__)->INSTRUCTION_getLine(__INS__)
#define INSTRUCTION_GET_SYMBOL_NAME(__FUNC__,__INS__)	((zetscript::CScriptFunction *)__FUNC__)->INSTRUCTION_getSymbolName(__INS__)


namespace zetscript{

	class  CScriptFunction:public CScriptClassBase{


	public:



		// info related for function ONLY
		std::vector<ParamArgInfo> m_arg; // tells var arg name or var type name (in of C )
		int idx_return_type; // -1 not inicialized type return.
		PtrInstruction instruction;

		//-----------
		//  SYMBOL-INSTRUCTION
		std::map<short,OpCodeInstructionSourceInfo> instruction_info; // std::map that gives symbol with at instruction idx given
		short 		 INSTRUCTION_getLine(OpCodeInstruction * ins);
		const char * INSTRUCTION_getSymbolName(OpCodeInstruction * ins);
		const char * INSTRUCTION_getFile(OpCodeInstruction * ins);

		//  DEBUG
		//-----------


		int idxScriptFunction;
		short idxLocalFunction;


		ScopeVarInnerBlockInfo *lut_scope_symbol;
		unsigned n_lut_scope_symbols;

		 CScriptFunction(unsigned char _idxClass );

		VariableSymbolInfo *  registerVariable(const std::string & file, short line, const std::string & variable, const std::string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0);


		/**
		 * build scope variable block information
		 */
		void buildLutScopeSymbols();


		virtual ~CScriptFunction();

	private:


		inline OpCodeInstructionSourceInfo * getInstructionInfo(OpCodeInstruction *ins){
			short idx= (ins-this->instruction);///sizeof(OpCodeInstruction *);
			if(instruction_info.count(idx)==1){
				return &instruction_info[idx];
			}
			return NULL;
		}


	};

}
