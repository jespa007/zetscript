/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void CScriptFunction::buildLutScopeSymbols(){

		if(symbol_info.symbol->idxScope < 0){ // it could be undefined or C
			return;
		}

		if(m_variable.size() == 0){ // no elements...
			return;
		}

		if(lut_scope_symbol != NULL){ // free if already allocated.
			free(lut_scope_symbol);
			lut_scope_symbol=NULL;
			n_lut_scope_symbols=0;
		}

		/// PRE: base_class_irfs must be info of root class.
		 bool is_main_function = symbol_info.symbol->idxScope == IDX_GLOBAL_SCOPE;


		 struct tInfoVarScopeBlockRegister{
			 int idxScope;
			 std::vector<int> var_index;
		 };

		 std::vector<CScope *> *list = CScopeFactory::getInstance()->getVectorScopeNode();
		 std::vector<tInfoVarScopeBlockRegister> vec_ivsb;
		 std::map<short,tInfoVarScopeBlockRegister> map_scope_register;

		 for(unsigned idx_var = 0;idx_var < m_variable.size(); idx_var++){ // register index var per scope ...

			map_scope_register[m_variable[idx_var].symbol->idxScope].idxScope=m_variable[idx_var].symbol->idxScope;
			map_scope_register[m_variable[idx_var].symbol->idxScope].var_index.push_back(idx_var);
		 }


		 lut_scope_symbol = (tInfoVarScopeBlock*)malloc(map_scope_register.size()*sizeof(tInfoVarScopeBlock));
		 n_lut_scope_symbols =map_scope_register.size();

		 int i=0;
		 for(std::map<short,tInfoVarScopeBlockRegister>::iterator e = map_scope_register.begin(); e != map_scope_register.end(); e++){

			 tInfoVarScopeBlockRegister ivs = map_scope_register[e->first];

			 lut_scope_symbol[i].idxScope = ivs.idxScope;
			 lut_scope_symbol[i].n_var_index = (char)ivs.var_index.size();
			 lut_scope_symbol[i].var_index = (int *)malloc(sizeof(int)*ivs.var_index.size());
			 for(unsigned j = 0; j < ivs.var_index.size(); j++){
				 lut_scope_symbol[i].var_index[j] = ivs.var_index[j];
			 }
			 i++;
		 }

	}


	CScriptFunction::CScriptFunction(unsigned char _idxClass):CScriptClassBase(_idxClass){
		idx_return_type = ZS_UNDEFINED_IDX;
		idxScriptFunction = ZS_UNDEFINED_IDX;
		instruction=NULL;
		lut_scope_symbol=NULL;
		n_lut_scope_symbols=0;
		idxLocalFunction=ZS_UNDEFINED_IDX;

	}


	short 		 CScriptFunction::INSTRUCTION_getLine(tInstruction * ins){
		tInstructionInfo *info=getInstructionInfo(ins);

		if(info!=NULL){
			return info->line;
		}
		return -1;
	}

	const char *  CScriptFunction::INSTRUCTION_getSymbolName(tInstruction * ins){
		tInstructionInfo *info=getInstructionInfo(ins);

		if(info!=NULL){
			return info->symbol_name->c_str();
		}
		return "unknown";
	}

	const char * CScriptFunction::INSTRUCTION_getFile(tInstruction * ins){

		tInstructionInfo *info=getInstructionInfo(ins);

		if(info!=NULL){
			return info->file;
		}
		return "unknown";
	}


	tVariableSymbolInfo *	CScriptFunction::registerVariable(const std::string & file, short line, const std::string & variable_name, const std::string & c_type, intptr_t ref_ptr, unsigned short properties)
	{
		tVariableSymbolInfo *vsi=CScriptClassBase::registerVariable(file,line,this->symbol_info.symbol->idxScope,  variable_name,  c_type,  ref_ptr,   properties);

		tStackElement se = {0,0,STK_PROPERTY_TYPE_UNDEFINED};

		if(properties &  PROPERTY_C_OBJECT_REF) // convert c ref var into stack element. This should be consistent in the whole execution.
			se=CScriptClassFactory::C_REF_InfoVariable_2_StackElement(
										 	vsi,
											(void *)vsi->ref_ptr);

		CURRENT_VM->addGlobalVar(se);

		return vsi;

	}

	CScriptFunction::~CScriptFunction(){

	}

}
