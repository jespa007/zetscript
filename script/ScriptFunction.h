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


#define SFI_GET_FILE_LINE(__FUNC__,__INS__) 	((zetscript::ScriptFunction *)__FUNC__)->getInstructionSourceFile(__INS__), ((zetscript::ScriptFunction *)__FUNC__)->getInstructionLine(__INS__)
#define SFI_GET_FILE(__FUNC__,__INS__) 			((zetscript::ScriptFunction *)__FUNC__)->getInstructionSourceFile(__INS__)
#define SFI_GET_LINE(__FUNC__,__INS__)			((zetscript::ScriptFunction *)__FUNC__)->getInstructionLine(__INS__)
#define SFI_GET_SYMBOL_NAME(__FUNC__,__INS__)	((zetscript::ScriptFunction *)__FUNC__)->getInstructionSymbolName(__INS__)


namespace zetscript{

	class  ScriptFunction:public ScriptClassBase{

	public:

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
		std::map<short,InstructionSourceInfo> instruction_source_info; // std::map that gives symbol with at instruction idx given

		short 		 getInstructionLine(Instruction * ins);
		const char * getInstructionSymbolName(Instruction * ins);
		const char * getInstructionSourceFile(Instruction * ins);

		//  DEBUG
		//-----------

		static void printGeneratedCode(ScriptFunction *sfo);

		ScriptFunction(ZetScript * _zs,unsigned char _idxClass );


		SymbolInfo *  registerVariable(
				const std::string & file
				, short line
				, const std::string & variable
				, const std::string & c_type=""
				, intptr_t ref_ptr=0
				, unsigned short properties=0
		);

		/**
		 * build scope variable block information
		 */
		void buildLutScopeSymbols();


		virtual ~ScriptFunction();

	private:
		static std::string formatInstructionLoadType(ScriptFunction *current_function,PtrInstruction list_statements, int current_instruction);
		inline InstructionSourceInfo * getInstructionInfo(Instruction *ins){
			short idx= (ins-this->instruction);///sizeof(Instruction *);
			if(instruction_source_info.count(idx)==1){
				return &instruction_source_info[idx];
			}
			return NULL;
		}
	};

}
