#pragma once

class CScriptFunction;
class CVirtualMachine{

public:

	enum VAR_TYPE{
		NOT_DEFINED=0,
		BOOL,
		INTEGER,
		NUMBER,
		STRING,
		OBJ,

		// function
		FUNCTION,

		// vector (to get int primitives)
		VECTOR1D_INT,
		VECTOR1D_BOOL,
		VECTOR1D_FLOAT,
		VECTOR1D_OBJ,
		VECTOR2D_INT,
		VECTOR2D_BOOL,
		VECTOR2D_FLOAT,
		VECTOR2D_OBJ,
		VECTOR3D_INT,
		VECTOR3D_BOOL,
		VECTOR3D_FLOAT,
		VECTOR3D_OBJ
	};

	enum ASM_OPERATOR{
		INVALID_OP=-1,
		NOP=0,
		MOV=1, // mov expression to var
		LOAD, // primitive value like number/string or boolean...
		EQU,  // ==
		LT,  // <
		LTE,  // <=
		NOT, // !
		GT,  // >
		GTE, // >=
		NEG, // !
		ADD, // +
		PRE_INC, // ++
		POST_INC, // ++
		PRE_DEC, // ++
		POST_DEC, // ++
		LOGIC_AND, // &&
		LOGIC_OR,  // ||
		DIV, // /
		MUL, // *
		MOD,  // %
		CAT,  // str+str
		AND, // bitwise logic and
		OR, // bitwise logic or
		XOR, // logic xor
		SHL, // shift left
		SHR, // shift right
		PUSH_SCOPE,
		POP_SCOPE,
		JMP,
		JNT, // goto if not true ... goes end to conditional.
		JT // goto if true ... goes end to conditional.


	};


	class tInfoAsmOp{

	public:


	     //int type_op;
	     //tInfoObjectOperator *funOp;
	     //CObject *left_var_obj;
	     void *result_obj; // can be float/bool/string or variable.
	     //string type_res;

	     //------------------
	     VAR_TYPE result_type;
	     ASM_OPERATOR operator_type;
	     void *ptr_value; // can be float, bool or string.
	     //------------------

	     int index_left,index_right;
	    // bool (* isconvertable)(int value);

		tInfoAsmOp(){
			result_type=VAR_TYPE::NOT_DEFINED;
			operator_type=ASM_OPERATOR::INVALID_OP;
			//isconvertable=NULL;
			//left_var_obj=NULL;
		  //   type_op=0;
		   //  funOp=NULL;
			result_obj=NULL; // must be created before.

		   // type_res="none";
		    index_left=index_right=-1;
		    ptr_value=NULL;
		}

	};

	typedef struct{

	    vector<tInfoAsmOp *> asm_op;
	}tInfoStatementOp;


	static CVirtualMachine * getInstance();
	static void destroySingletons();

	bool execute(CScriptFunction *fs, vector<CObject *> * argv=NULL);

private:
	static CVirtualMachine *m_virtualMachine;
	CVirtualMachine();
	~CVirtualMachine();
};
