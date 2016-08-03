#include "zg_script.h"

CVirtualMachine * CVirtualMachine::m_virtualMachine = NULL;
//vector<CVirtualMachine::CALE> CVirtualMachine::ALE;


// static: only defined in this module...

CVirtualMachine * CVirtualMachine::getInstance(){
	if(m_virtualMachine == NULL){
		m_virtualMachine = new CVirtualMachine();
	}

	return m_virtualMachine;
}

void CVirtualMachine::destroySingletons(){
	if(m_virtualMachine != NULL){
		delete m_virtualMachine;
		m_virtualMachine = NULL;
	}


}

CVirtualMachine::CVirtualMachine(){

}


#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
//#define __VERBOSE_MESSAGE__
#endif



#ifdef  __VERBOSE_MESSAGE__

#define print_vm_cr print_info_cr
#else
#define print_vm_cr(s,...)
#endif



bool CVirtualMachine::execute(CScriptFunction *sf, int stk,vector<CObject *> * argv){

	tInfoRegisteredFunctionSymbol *irsf=sf->getFunctionInfo();

	if((sf->getFunctionInfo()->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::C_OBJECT_REF){ // C-Call

			return CZG_Script::call_C_function(sf->getFunctionInfo(),argv);
	}


	vector<tInfoStatementOp> * m_listStatements = sf->getCompiledCode();
	bool conditional_jmp=false;
	int jmp_to_statment = -1;

	if(argv != NULL){

		if((*argv).size() != irsf->m_arg.size()){
			print_error_cr("calling function s from line . Expected %i but passed %i",irsf->m_arg.size(),(*argv).size());
			return false;
		}

		//print_info_cr("assign local symbol the passing args...");
		for(unsigned i = 0; i < (*argv).size(); i++){
			//print_info_cr("%s=%s <cr>",sf->getArgVector()->at(i).c_str(),(*argv).at(i)->getPointerClassStr().c_str());
			(*sf->getArgSymbol(i)).object=(*argv).at(i);
		}
	}



	CALE ALE; // new ale ?

	unsigned n_stats=(*m_listStatements).size();


	for(unsigned s = 0; s < n_stats;){

		conditional_jmp = false;
		jmp_to_statment = -1;
		tInfoStatementOp * current_statment = &(*m_listStatements)[s];
		vector<tInfoAsmOp *> * asm_op_statment = &current_statment->asm_op;
		unsigned n_asm_op= asm_op_statment->size();

		//if(stk==2){
		//	s++;
		//	continue;
		//}

		if(n_asm_op>0){

			// clear previous ALE information stack..
			//if(stk!=2){
			ALE.reset();
			//}

			//vector<CCompiler::tInfoAsmOp *> * asm_op_statment = &(*m_listStatements)[s].asm_op;
			//print_vm_cr("executing code...");


			//CCompiler::tInfoAsmOp * instruction=NULL;


			for(unsigned i = 0; i  <  n_asm_op && (jmp_to_statment==-1); i++){ // for each code-instruction execute it.
				//print_vm_cr("executing instruction %s [%02i:%02i]...",sf->getName().c_str(), s,i);

			/*	if( s==1 && i==0){
					int hhh=0;
					hhh++;
				}
*/


				//if(stk!=2){

					//return true;


					if(!ALE.performInstruction(i,asm_op_statment->at(i),sf,jmp_to_statment,stk)){
						return false;
					}

					if(asm_op_statment->at(i)->operator_type == ASM_OPERATOR::RET){ // return...
						return true;
					}

				//}


				//previous_instruction = instruction;
			}

			if(jmp_to_statment != -1){
				s=jmp_to_statment;
			}
			else{ // next statment ...
				s++;
			}


		}
	}

	return true;

}

CVirtualMachine::~CVirtualMachine(){

}
