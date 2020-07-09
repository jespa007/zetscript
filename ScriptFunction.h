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


#define INSTRUCTION_GET_FILE_LINE(__FUNC__,__INS__) 	((ZetScript::ScriptFunction *)__FUNC__)->GetInstructionSourceFile(__INS__), ((ZetScript::ScriptFunction *)__FUNC__)->GetInstructionLine(__INS__)
#define INSTRUCTION_GET_FILE(__FUNC__,__INS__) 			((ZetScript::ScriptFunction *)__FUNC__)->GetInstructionSourceFile(__INS__)
#define INSTRUCTION_GET_LINE(__FUNC__,__INS__)			((ZetScript::ScriptFunction *)__FUNC__)->GetInstructionLine(__INS__)
#define INSTRUCTION_GET_SYMBOL_NAME(__FUNC__,__INS__)	((ZetScript::ScriptFunction *)__FUNC__)->GetInstructionSymbolName(__INS__)


namespace zetscript{

	class  ScriptFunction:public ScriptClassBase{


	public:
		static void PrintGeneratedCode(ScriptFunction *sfo);


		// info related for function ONLY
		std::vector<ParamArgInfo> arg_info; // tells var arg name or var type name (in of C )
		int idx_return_type; // -1 not inicialized type return.
		PtrInstruction instruction;

		int idx_script_function;
		short idx_local_function;
		ScopeVarInnerBlockInfo *lut_scope_symbol;
		unsigned n_lut_scope_symbols;


		//-----------
		//  SYMBOL-INSTRUCTION
		std::map<short,OpCodeInstructionSourceInfo> instruction_info; // std::map that gives symbol with at instruction idx given
		short 		 GetInstructionLine(Instruction * ins);
		const char * GetInstructionSymbolName(Instruction * ins);
		const char * GetInstructionSourceFile(Instruction * ins);

		//  DEBUG
		//-----------

		ScriptFunction(ZetScript * _zs,unsigned char _idxClass );
		SymbolInfo *  RegisterVariable(const std::string & file, short line, const std::string & variable, const std::string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0);


		/**
		 * build scope variable block information
		 */
		void BuildLutScopeSymbols();


		virtual ~ScriptFunction();

	private:

		inline OpCodeInstructionSourceInfo * GetInstructionInfo(Instruction *ins){
			short idx= (ins-this->instruction);///sizeof(Instruction *);
			if(instruction_info.count(idx)==1){
				return &instruction_info[idx];
			}
			return NULL;
		}


	};

}
