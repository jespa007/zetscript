#pragma once

#include "system/zg_system.h"
#include "utils/zg_utils.h"
#include "object/zg_object.h"
#include "factory/zg_factory.h"

#include "ast.h"




template <typename T>
struct function_traits
    : public function_traits<decltype(&T::operator())>
{};
// For generic types, directly use the result of the signature of its 'operator()'

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
// we specialize for pointers to member function
{
    enum { arity = sizeof...(Args) };
    // arity is the number of arguments.

    typedef ReturnType result_type;

    template <size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
        // the i-th argument is equivalent to the i-th tuple element of a tuple
        // composed of those arguments.
    };
};

template <std::size_t...> struct index_sequence {};
template <std::size_t N, std::size_t... Is> struct make_index_sequence : make_index_sequence<N-1, N-1, Is...> {};
template <std::size_t... Is> struct make_index_sequence<0, Is...> : index_sequence<Is...> {};
class CObject;
template <size_t argIdx, typename R, typename... Args>
auto getArgTypes(std::string& ref, std::vector<std::string> & params)
    -> typename std::enable_if<argIdx == 1>::type
{
    using fun = function_traits<std::function<R(Args...)> >;
    ref.append(std::to_string(argIdx)+" ");

    typename fun::template arg<argIdx-1>::type var=NULL;
    string parameter_type=typeid(var).name();



    //cout << "is same:" << std::is_base_of<CObject ,typename fun::template arg<0>::type> << endl;
    ref.append(parameter_type).append(" ");
    params.insert(params.begin()+0,parameter_type);

    // number parameter ...
    if(!CFactoryContainer::getInstance()->classPtrIsRegistered(parameter_type)){
    	print_error_cr("\"%s\" is not registered",parameter_type.c_str());
    }


}

template <size_t argIdx, typename R, typename... Args>
auto getArgTypes(std::string& ref, std::vector<std::string> & params)
    -> typename std::enable_if<argIdx != 1>::type
{
    using fun = function_traits<std::function<R(Args...)> >;

    ref.append(std::to_string(argIdx)+" ");

   typename fun::template arg<argIdx-1>::type var=NULL;

    //cout << "is same:" << std::is_same<CObject *,parameter_type>::value << endl;
    string parameter_type=typeid(var).name();
    ref.append(parameter_type).append(" ");
    params.insert(params.begin()+0,parameter_type);

    if(!CFactoryContainer::getInstance()->classPtrIsRegistered(parameter_type)){
    	print_error_cr("\"%s\" is not registered",parameter_type.c_str());
    }



    getArgTypes<argIdx - 1,R, Args...>(ref, params);
}


template <typename F, std::size_t... Is>
std::vector<std::string> * getParamsFunction(int i, index_sequence<Is...>)
{
	std::vector<std::string> *typeParams= new std::vector<std::string>();
	std::string return_type = typeid(typename F::result_type).name();
    std::string s;
    getArgTypes<F::arity, typename F::result_type, typename F::template arg<Is>::type...>(s,*typeParams);

    std::cout << return_type << " (" << (s) << ")";

    if(!CFactoryContainer::getInstance()->classPtrIsRegistered(return_type)){
    	print_error_cr("Return type \"%s\" is not registered",return_type.c_str());
    }


    return typeParams;
}

template<class FunctorT>
void* getcodeptr(const FunctorT& f) {
  auto ptr = &FunctorT::operator();
  return *(void**)&ptr;
}

/*
#define registerOperator(op_name,f){\
	auto reg_function=f;\
	typedef function_traits<decltype(reg_function)> decl;\
	std::vector<string> *param_type=getParamsFunction<decl>(0,make_index_sequence<decl::arity>{});\
	std::string result_type=typeid(decl::result_type).name();\
	CZG_Script::getInstance()->registerOperatorInternal(op_name,result_type,param_type, (void(*)()) getcodeptr(reg_function));\
}
*/


class CZG_Script{

public:
	enum TYPE{
		UNKNOW=0,
		BOOL,
		INTEGER,
		NUMBER,
		STRING,
		VAR
	};


private:

	/*enum TYPE_{
		LOAD_VALUE=1,
		MOV_VAR,
		OPERATOR
	};*/



	/*typedef struct{
		string result_type;
		vector <string> * param_type;
		void (* fun_ptr)();
	}tInfoObjectOperator;*/

	CUndefined *m_defaultVar;



	class tInfoAsmOp{

	public:


	     //int type_op;
	     //tInfoObjectOperator *funOp;
	     //CObject *left_var_obj;
	     void *result_obj; // can be float/bool/string or variable.
	     //string type_res;

	     //------------------
	     TYPE result_type;
	     ASM_OPERATOR operator_type;
	     void *ptr_value; // can be float, bool or string.
	     //------------------

	     int index_left,index_right;
	    // bool (* isconvertable)(int value);

		tInfoAsmOp(){
			result_type=TYPE::UNKNOW;
			operator_type=ASM_OPERATOR::UNKNOW;
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


	vector<tInfoStatementOp> statement_op;


	static CZG_Script * m_instance;

	CZG_Script();

	~CZG_Script();


	//map<string,vector<tInfoObjectOperator> *> m_mapContainerOperators;
	map<string,CObject *> m_registeredVariable;


	//bool existOperatorSignature(const string & op,const string & result, vector<string> * param);
	bool existRegisteredVariable(const string & var_name);


	//tInfoObjectOperator * getOperatorInfo(const string & op, string * type_op1, string * type_op2=NULL);

	void unregisterOperators();
	void insertNewStatment();


	ASM_OPERATOR getNumberOperatorId_TwoOps(const string & op,TYPE & result_type);
	ASM_OPERATOR getIntegerOperatorId_TwoOps(const string & op,TYPE & result_type);
	ASM_OPERATOR getBoleanOperatorId_TwoOps(const string & op,TYPE & result_type);
	ASM_OPERATOR getStringOperatorId_TwoOps(const string & op,TYPE & result_type);

	ASM_OPERATOR getNumberOperatorId_OneOp(const string & op);
	ASM_OPERATOR getBoleanOperatorId_OneOp(const string & op);
	ASM_OPERATOR getStringOperatorId_OneOp(const string & op);

	TYPE getTypeAsmResult(int index);
	bool isVarDeclarationStatment(const char *statment, bool & error,char **eval_expression, int & m_line);
public:

	//---------------------------------
	// Register functions
	CObject *getRegisteredVariable(const string & v, bool print_msg=true);
	bool registerVariable(const string & var_name);
	bool defineVariable(const string & var_name, CObject *obj);
	//---------------------------------

	static CZG_Script * getInstance();

	bool eval(const string & s);
	//bool registerOperatorInternal(const string & _op_name, const string &  result_type,vector<string> * param_type, void(*fun_ptr)());

	bool insertLoadValueInstruction(const string & v, string & type_ptr);
	bool insertMovVarInstruction(CObject *var, int right);

	bool insertOperatorInstruction(const string &op, int left, int right=-1);
	string getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction);




	void execute();

	void init();

	static void destroy();
};

