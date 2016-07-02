#pragma once


#include "CCompiler.h"

#define MAX_PER_TYPE_OPERATIONS 32

class CScriptFunction;
class CVirtualMachine{

public:




	static CVirtualMachine * getInstance();
	static void destroySingletons();



	bool execute(CScriptFunction *fs, vector<CObject *> * argv=NULL);

private:


	class CALE{

	public:

		CALE(){
			reset();

		}

		int *pushInteger(int init_value){
			if(n_stkInteger ==MAX_PER_TYPE_OPERATIONS){
				print_error_cr("Max int operands");
				return NULL;
			}
			asm_instruction_expression[current_asm_instruction++]=CVariable::VAR_TYPE::INTEGER;

			return &stkInteger[n_stkInteger++];
		}

		bool *pushBoolean(bool init_value){
			if(n_stkBoolean ==MAX_PER_TYPE_OPERATIONS){
				print_error_cr("Reached max bool operations");
				return NULL;
			}
			asm_instruction_expression[current_asm_instruction++]=CVariable::VAR_TYPE::BOOLEAN;
			return &stkBoolean[n_stkBoolean++];
		}

		float *pushNumber(float init_value){
			if(n_stkNumber ==MAX_PER_TYPE_OPERATIONS){
				print_error_cr("Reached max number operations");
				return NULL;
			}
			asm_instruction_expression[current_asm_instruction++]=CVariable::VAR_TYPE::NUMBER;
			return &stkNumber[n_stkNumber++];
		}

		string *pushString(const string & init_value){
			if(n_stkString ==MAX_PER_TYPE_OPERATIONS){
				print_error_cr("Reached max string operations");
				return NULL;
			}
			asm_instruction_expression[current_asm_instruction++]=CVariable::VAR_TYPE::STRING;
			return &stkString[n_stkString++];

		}

		CObject **pushObject(CObject * init_value){
			if(n_stkObject ==MAX_PER_TYPE_OPERATIONS){
				print_error_cr("Reached max object operations");
				return NULL;
			}
			asm_instruction_expression[current_asm_instruction++]=CVariable::VAR_TYPE::OBJECT;
			return &stkObject[n_stkObject++];

		}

		CScriptFunction **pushFunction(CScriptFunction * init_value){
			if(n_stkFunction ==MAX_PER_TYPE_OPERATIONS){
				print_error_cr("Reached max function operations");
				return NULL;
			}
			asm_instruction_expression[current_asm_instruction++]=CVariable::VAR_TYPE::FUNCTION;
			return &stkFunction[n_stkFunction++];
		}

		CVector **pushVector(CVector * init_value){
			if(n_stkVector ==MAX_PER_TYPE_OPERATIONS){
				print_error_cr("Reached max vector operations");
				return NULL;
			}
			asm_instruction_expression[current_asm_instruction++]=CVariable::VAR_TYPE::VECTOR;
			return &stkVector[n_stkVector++];

		}

		bool loadValue(CCompiler::tInfoAsmOp *iao){

			CObject *obj = ((CObject *)iao->index_op2);
			CVariable *var=NULL;


			//sprintf(print_aux_load_value,"UNDEFINED");
			switch(iao->index_op1){
			case CCompiler::LOAD_TYPE::LOAD_TYPE_CONSTANT:

				if(obj->getObjectType() != CObject::OBJECT_TYPE::VARIABLE){
					print_error_cr("Load type %i is not variable type",obj->getObjectType() );
					return false;
				}

				var = (CVariable *)obj;

				switch(iao->variable_type){
					default:
						print_error_cr("Unexpected constant as %i",iao->variable_type);
						return false;
						break;
					case CVariable::VAR_TYPE::INTEGER:
						pushInteger(((CInteger *)var)->m_value);
						break;
					case CVariable::VAR_TYPE::BOOLEAN:
						pushBoolean(((CBoolean *)var)->m_value);
						break;
					case CVariable::VAR_TYPE::STRING:
						pushString(((CString *)var)->m_value);
						break;
					case CVariable::VAR_TYPE::NUMBER:
						pushNumber(((CNumber *)var)->m_value);
						break;
					}

				//sprintf(print_aux_load_value,"CONST(%s)",value_symbol.c_str());
				break;
			case CCompiler::LOAD_TYPE::LOAD_TYPE_VARIABLE:


				if(obj->getObjectType() == CObject::OBJECT_TYPE::VARIABLE){

					var = (CVariable *)obj;

					switch(var->getVariableType()){

						default:
						case CVariable::VAR_TYPE::OBJECT:
							pushObject(var);
							break;
						case CVariable::VAR_TYPE::INTEGER:
							pushInteger(((CInteger *)var)->m_value);
							break;
						case CVariable::VAR_TYPE::BOOLEAN:
							pushBoolean(((CBoolean *)var)->m_value);
							break;
						case CVariable::VAR_TYPE::STRING:
							pushString(((CString *)var)->m_value);
							break;
						case CVariable::VAR_TYPE::NUMBER:
							pushNumber(((CNumber *)var)->m_value);
							break;
						case CVariable::VAR_TYPE::VECTOR:
							pushVector((CVector *)var);
							break;
						case CVariable::VAR_TYPE::FUNCTION:
							pushFunction((CScriptFunction *)var);
							break;

					}
				}
				//sprintf(print_aux_load_value,"VAR(%s)",value_symbol.c_str());
				break;
			case CCompiler::LOAD_TYPE::LOAD_TYPE_ARGUMENT:
				//sprintf(print_aux_load_value,"ARG(%s)",value_symbol.c_str());
				break;
			default:
				break;

			}

			return false;
		}


		bool performInstruction(CCompiler::tInfoAsmOp * instruction){

			int index_op2 = instruction->index_op2;
			int index_op1 = instruction->index_op1;


			switch(instruction->operator_type){
			default:
				print_error_cr("operator type(%s) not implemented",CCompiler::def_operator[instruction->operator_type].op_str);
				break;
			case CCompiler::NOP: // ignore ...
				break;
			case CCompiler::LOAD: // load value in function of value/constant ...

				loadValue(instruction);

				break;

			case CCompiler::ADD: // check both operants type number / integer.

				// get indexes and check whether is possible or not its calculation.
				// check indexes
				if(!(index_op1 >= 0 && index_op1 <=current_asm_instruction)) { print_error_cr("instruction 1 out of bounds");}
				if(!(index_op2 >= 0 && index_op2 <=current_asm_instruction)) { print_error_cr("instruction 2 out of bounds");}
				if(!(index_op2 >= index_op1 )) { print_error_cr("invalid indexes");}
				// check types ...
				if((asm_instruction_expression[index_op1] == CVariable::VAR_TYPE::INTEGER || asm_instruction_expression[index_op1] == CVariable::VAR_TYPE::NUMBER) &&
				   (asm_instruction_expression[index_op2] == CVariable::VAR_TYPE::INTEGER || asm_instruction_expression[index_op2] == CVariable::VAR_TYPE::NUMBER)
				){
					// this is the combination for number operations:
					//
					// op1 | op2 |  R
					// ----+-----+----
					//  i  |  i  |  i
					//  i  |  f  |  i
					//  f  |  i  |  f
					//  f  |  f  |  f

				}

				break;
			}

			return true;
		}

		void reset(){
			n_stkInteger=
			n_stkNumber=
			n_stkBoolean=
			n_stkString=
			n_stkObject=
			n_stkVector=
			n_stkFunction=
			current_asm_instruction=0;
		}


	private:

		enum{
			MAX_OPERATIONS_PER_EXPRESSION=(MAX_PER_TYPE_OPERATIONS+1)*CVariable::MAX_VAR_TYPES // +1 because
		};

		int 	stkInteger[MAX_PER_TYPE_OPERATIONS];
		float	stkNumber[MAX_PER_TYPE_OPERATIONS];
		bool 	stkBoolean[MAX_PER_TYPE_OPERATIONS];
		string  stkString[MAX_PER_TYPE_OPERATIONS];
		CVector  *stkVector[MAX_PER_TYPE_OPERATIONS];
		CScriptFunction *stkFunction[MAX_PER_TYPE_OPERATIONS];
		CObject  *stkObject[MAX_PER_TYPE_OPERATIONS];
		//CVector	 * vector[MAX_OPERANDS];

		int n_stkInteger;
		int n_stkNumber;
		int n_stkBoolean;
		int n_stkString;
		int n_stkObject;
		int n_stkVector;
		int n_stkFunction;



		CVariable::VAR_TYPE asm_instruction_expression[MAX_OPERATIONS_PER_EXPRESSION];
		int current_asm_instruction;


	};

	static vector<CALE> ALE; // 1 push per function

	static CVirtualMachine *m_virtualMachine;
	CVirtualMachine();
	~CVirtualMachine();
};
