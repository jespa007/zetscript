#pragma once


#define MAX_OPERANDS 32

/*

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
*/

/*
#define registerOperator(op_name,f){\
	auto reg_function=f;\
	typedef function_traits<decltype(reg_function)> decl;\
	std::vector<string> *param_type=getParamsFunction<decl>(0,make_index_sequence<decl::arity>{});\
	std::string result_type=typeid(decl::result_type).name();\
	CZG_Script::getInstance()->registerOperatorInternal(op_name,result_type,param_type, (void(*)()) getcodeptr(reg_function));\
}
*/

template <typename T>
struct function_traits
    : public function_traits<decltype(&T::operator())>
{};

// function pointer
template<class R, class... Args>
struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
{
    using return_type = R;

    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
    };
};

// member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&,Args...)>
{
    using return_type = R;

    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
    };
};

// const member function pointer (lambda)
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...) const> : public function_traits<R(C&,Args...)>
{
    using return_type = R;

    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
    };
};

// member object pointer
template<class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C&)>
{};

template<class R, class... Args>
struct function_traits<R(Args...)>
{
    using return_type = R;

    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
    };
};



template <std::size_t...> struct index_sequence {};
template <std::size_t N, std::size_t... Is> struct make_index_sequence : make_index_sequence<N-1, N-1, Is...> {};
template <std::size_t... Is> struct make_index_sequence<0, Is...> : index_sequence<Is...> {};

// template for last parameter (argIdx == 1)
template <size_t argIdx, typename F, typename... Args>
auto getArgTypes(std::string& ref, std::vector<std::string> & params)
    -> typename std::enable_if<argIdx == 1>::type
{
 // trivial case...
    //using fun = function_traits<std::function<R(C::*)(Args...)> >;

    //typename F::template argument<0>::type var;


    ref.append(std::to_string(argIdx)+" ");
    //ref.append(typeid(typename fun::template arg<argIdx-1>::type).name()).append(" ");
 //    typename fun::argument<argIdx-1>::type var;
    //typename fun::template argument<0>::type var=NULL;
    string parameter_type=typeid(typename F::template argument<argIdx-1>::type).name();

    printf("\nArg:%i",argIdx);


    //cout << "is same:" << std::is_base_of<CObject ,typename fun::template arg<0>::type> << endl;
    ref.append(parameter_type).append(" ");
    params.insert(params.begin()+0,parameter_type);

    // number parameter ...
   /* if(!CFactoryContainer::getInstance()->classPtrIsRegistered(parameter_type)){
    	print_error_cr("\"%s\" is not registered",parameter_type.c_str());
    }*/


}

// template when parameters are > 1
template <size_t argIdx, typename F, typename... Args>
auto getArgTypes(std::string& ref, std::vector<std::string> & params)
    -> typename std::enable_if<(argIdx > 1)>::type
{
    //using fun = function_traits<std::function<R(Args...)> >;
	//F::argument<0>::type var=NULL;



    ref.append(std::to_string(argIdx)+" ");
   // std::cout << "arg0:" << typeid(fun::argument<0>::type).name() << std::endl;
  // typename fun2::template arg<argIdx-1>::type var=NULL;

   printf("\nArg:%i ",argIdx);//,typeid(Args<argIdx-1>).name());
    //cout << "is same:" << std::is_same<CObject *,parameter_type>::value << endl;
    string parameter_type=typeid(typename F::template argument<argIdx-1>::type).name();
    ref.append(parameter_type).append(" ");
    params.insert(params.begin()+0,parameter_type);

  /*  if(!CFactoryContainer::getInstance()->classPtrIsRegistered(parameter_type)){
    	print_error_cr("\"%s\" is not registered",parameter_type.c_str());
    }*/



    getArgTypes<argIdx - 1,F, Args...>(ref, params);
}

// trivial case when parameters are 0.
template <size_t argIdx, typename F>
auto getArgTypes(std::string& ref, std::vector<std::string> & params)

{
	 printf("\nNo Args!");
}

template <typename F, std::size_t... Is>
void getParamsFunction(int i,string & returnType, std::vector<std::string> & typeParams, index_sequence<Is...>)
{
	//std::vector<std::string> *typeParams= new std::vector<std::string>();
	/*std::string*/ returnType = typeid(typename F::return_type).name();


    std::string s;

    getArgTypes<F::arity, F, typename F::template argument<Is>::type...>(s,typeParams);


    std::cout << returnType << " (" << (s) << ")";

    /*if(!CFactoryContainer::getInstance()->classPtrIsRegistered(return_type)){
    	print_error_cr("Return type \"%s\" is not registered",return_type.c_str());
    }
*/

    //return typeParams;
}





class CScriptFunction: public CObject{

public:

	enum TYPE{
		UNKNOW_TYPE=0,
		SCRIPT_FUNCTION_TYPE=1, // function with args...
		C_FUNCTION_TYPE, // C Function with defined symbols ...
		CLASS_TYPE // complex script function. Includes args available for constructor and a set of functions
	};


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
		UNKNOW_ALU_TYPE=0,
		INTEGER_ALU_TYPE,
		NUMBER_ALU_TYPE,
		STRING_ALU_TYPE,
		BOOLEAN_ALU_TYPE,
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
