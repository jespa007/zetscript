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



bool CVirtualMachine::execute(CScriptFunction *sf, vector<CObject *> * argv){

	vector<CCompiler::tInfoStatementOp> * m_listStatements = sf->getCompiledCode();
	bool conditional_jmp=false;

	if(argv != NULL){

		if((*argv).size() != sf->getArgVector()->size()){
			print_error_cr("calling function s from line . Expected %i but passed %i",sf->getArgVector()->size(),(*argv).size());
			return false;
		}

		print_info_cr("assign local symbol the passing args...");
		for(unsigned i = 0; i < (*argv).size(); i++){
			print_info_cr("%s=%s <cr>",sf->getArgVector()->at(i).c_str(),(*argv).at(i)->getPointerClassStr().c_str());
			(*sf->getArg(i))=(*argv).at(i);
		}
	}


	CALE ALE; // new ale ?

	for(unsigned s = 0; s < (*m_listStatements).size();){

		conditional_jmp = false;
		if((*m_listStatements)[s].asm_op.size()>0){

			// clear previous ALE information stack..
			ALE.reset();

			vector<CCompiler::tInfoAsmOp *> * asm_op_statment = &(*m_listStatements)[s].asm_op;
			print_vm_cr("executing code...");


			//CCompiler::tInfoAsmOp * instruction=NULL;
			CCompiler::tInfoStatementOp * current_statment = &(*m_listStatements)[s];
			for(unsigned i = 0; i  <  asm_op_statment->size() && !conditional_jmp; i++){ // for each code-instruction execute it.
				print_vm_cr("executing instruction %s [%02i:%02i]...",sf->getName().c_str(), s,i);

				if( s==2 && i==3){
					int hhh=0;
					hhh++;
				}

				int jmp_to_statment = -1;

				ALE.performInstruction(i,current_statment->asm_op[i],sf,jmp_to_statment);


				//previous_instruction = instruction;
			}

			// if not conditional jmp increase next statment...
			if(!conditional_jmp){
				s++;
			}

		}
	}

	return true;

}

CVirtualMachine::~CVirtualMachine(){

}
