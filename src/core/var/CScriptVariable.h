#pragma once



#include "core/ScriptDefinesStructs.h"
#include "core/ast/CScopeInfo.h"



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
class CVariable;
template <size_t argIdx, typename R, typename... Args>
auto getArgTypes(std::string& ref, std::vector<std::string> & params)
    -> typename std::enable_if<argIdx == 1>::type
{
    using fun = function_traits<std::function<R(Args...)> >;
    ref.append(std::to_string(argIdx)+" ");

    typename fun::template arg<argIdx-1>::type var=NULL;
    string parameter_type=typeid(var).name();



    //cout << "is same:" << std::is_base_of<CVariable ,typename fun::template arg<0>::type> << endl;
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

    //cout << "is same:" << std::is_same<CVariable *,parameter_type>::value << endl;
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
	CZG_ScriptCore::getInstance()->registerOperatorInternal(op_name,result_type,param_type, (void(*)()) getcodeptr(reg_function));\
}
*/





class CScriptVariable{//: public CVariable{

public:

	static CUndefined *UndefinedSymbol;
	static CVoid *VoidSymbol;




	static void createSingletons();
	static void destroySingletons();

	/*enum TYPE{
		SCRIPT_FUNCTION_TYPE=0, // function with args (default)...
		//C_FUNCTION_TYPE, // C Function with defined symbols ...
		CLASS_TYPE // complex script function. Includes args available for constructor and a set of functions
	};*/

	class tSymbolInfo{
		public:
		void *object; // created object. undefined by default.
		tASTNode *ast;
		tSymbolInfo(){
			this->object = CScriptVariable::UndefinedSymbol;
			this->ast = NULL;
		}

	};


	// public vars ...
	string m_strValue;

	//CScriptVariable();
	CScriptVariable(tInfoRegisteredClass *info_registered_class);

	//TYPE m_type;
	int getIdxClass(){return m_infoRegisteredClass->class_idx;}

	void addVariableSymbol(tASTNode *ast);
	tSymbolInfo * getVariableSymbol(const string & varname);
	tSymbolInfo * getVariableSymbolByIndex(unsigned idx);

	void addFunctionSymbol(tASTNode *ast,tInfoRegisteredFunctionSymbol *irv);
	tSymbolInfo * getFunctionSymbol(const string & funname);
	tSymbolInfo * getFunctionSymbolByIndex(unsigned idx);

	vector<tInfoStatementOp> * getCompiledCode(int idx_function);

	//void addArgSymbol(const string & arg_name);

	const string & getClassName(){return m_infoRegisteredClass->object_info.symbol_info.symbol_name;}
	const string & getPointer_C_ClassName(){return m_infoRegisteredClass->classPtrType;}


    virtual string toString(){
    	return m_strValue;
    }
	//tSymbolInfo * getArgSymbol(const string & var_name);
	//tSymbolInfo * getArgSymbol(unsigned index);


	//CScriptVariable(tInfoRegisteredFunctionSymbol *irv);
	//vector<tInfoStatementOp> * getCompiledCode();

	//tInfoRegisteredFunctionSymbol * getFunctionInfo(unsigned idx);


//	tInfoScopeVar * registerArgument(const string & var_name);
//	void 	 addFunction(CScriptVariable *sf);
//	vector<CScriptVariable *> *	 getVectorFunction();
//	vector<string> *	 getArgVector();


	/**
	 * Check whether is main function or not.
	 */
	//bool isMainFunction();


	//void setupAsFunctionPointer(void * _pointer_function);
	//void add_C_function_argument(string arg_type);
	//bool call_C_function(vector<CVariable *> * argv);

	//TYPE getType();


	//CScriptVariable *getParent();
	//CScopeInfo *getScope();

	//PASTNode getRootAst();
	//PASTNode * getRootAstPtr();


	/*
	 * eval: Evaluates its body.
	 */
//	bool eval(const string & s);
	//bool execute(vector<CVariable *> *argv=NULL);

	virtual ~CScriptVariable();

private:




	/**
	 * AST root
	 */
	//PASTNode m_rootAst;

	/**
	 * This variable tells whether is pointer function or not.
	 */
	//void * pointer_function;

	//vector<string> m_arg;
	//vector<string> m_c_arg; // for c function
	//vector<CScriptVariable *> m_function;
	//tInfoRegisteredFunctionSymbol *irv;
	tInfoRegisteredClass *m_infoRegisteredClass;


	void createSymbols(tInfoRegisteredClass *irv);

	vector<tSymbolInfo> m_variableSymbol;
	vector<tSymbolInfo> m_functionSymbol;
	//vector<tSymbolInfo> m_arg_symbol;
	//vector<tInfoStatementOp>  	m_listStatements;






	//CScopeInfo	*m_scope; // base scope...
	//CScriptVariable *m_parentFunction;

};
