#pragma once


#define MAX_OPERANDS 32

class CScriptFunction: public CObject{

public:


	typedef struct{
		int m_line;
		CScriptFunction *m_scriptFunction;
	}tInfoRegisteredClass;


	tInfoRegisteredClass *getRegisteredClass(const string & v, bool print_msg=true);
	CScriptFunction * registerClass(const string & class_name, int line);


	CScriptFunction(CScriptFunction *_parentFunction=NULL);
	vector<CCompiler::tInfoStatementOp> * getCompiledCode();

	void 	 addArg(const string & var_name);
	void 	 addFunction(CScriptFunction *sf);
	vector<CScriptFunction *> *	 getVectorFunction();

	CObject *getArg(const string & var_name);

	CObject *getReturnValue();
	void setReturnValue(CObject *);


	CScriptFunction *getParent();

	CScope *getScope();

	/*
	 * eval: Evaluates its body.
	 */
//	bool eval(const string & s);
	//bool execute(vector<CObject *> *argv=NULL);

	~CScriptFunction();

private:

	enum ALU_TYPE{
		UNKNOW_TYPE=0,
		INTEGER_TYPE,
		NUMBER_TYPE,
		STRING_TYPE,
		BOOLEAN_TYPE,
		MAX_ALU_TYPES

	};

	class CAlu{

	public:

		CInteger *popInteger(){
			if(n_current_integer ==MAX_OPERANDS){
				print_error_cr("Max int operands");
				return NULL;
			}
			return &integer[n_current_integer++];
		}

		CBoolean *popBoolean(){
			if(n_current_boolean ==MAX_OPERANDS){
				print_error_cr("Max int operands");
				return NULL;
			}
			return &boolean[n_current_boolean++];
		}

		CNumber *popNumber(){
			if(n_current_number ==MAX_OPERANDS){
				print_error_cr("Max number operands");
				return NULL;
			}
			return &number[n_current_number++];
		}
		CString *popString(){
			if(n_current_string ==MAX_OPERANDS){
				print_error_cr("Max string operands");
				return NULL;
			}
			return &string[n_current_string++];

		}

		void resetALU(){
			n_current_integer =n_current_number=n_current_boolean=n_current_string=current_asm_instruction;
		}


	private:

		CInteger integer[MAX_OPERANDS];
		CNumber	 number[MAX_OPERANDS];
		CBoolean boolean[MAX_OPERANDS];
		CString  string[MAX_OPERANDS];
		//CVector	 * vector[MAX_OPERANDS];

		int n_current_integer;
		int n_current_number;
		int n_current_boolean;
		int n_current_string;


		int asm_instruction[MAX_ALU_TYPES*MAX_OPERANDS];
		int current_asm_instruction;


	};



	vector<CObject *> m_arg;
	vector<CScriptFunction *> m_function;


	vector<CCompiler::tInfoStatementOp>  	m_listStatements;
	map<string,tInfoRegisteredClass *>  	 m_registeredClass;
	/**
	 * Return variable is assigned at the begin as undefined and when return keyword occurs,
	 * returnVariable is assigned as the result of expression after return. By default return variable is type void.
	 */
	CObject *returnVariable;
	tInfoRegisteredClass * existRegisteredClass(const string & class_name);


	CScope	*m_scope; // base scope...
	CScriptFunction *m_parentFunction;



};
