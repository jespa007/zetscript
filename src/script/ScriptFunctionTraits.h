/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

namespace zetscript{


	template <typename T>
	struct FunctionTraits
		: public FunctionTraits<decltype(&T::operator())>
	{};


	//------------------------------------------------------------
	// Function traits for typedef function

	// function without args ...
	template<class R>
	struct FunctionTraits<R()>
	{
		using return_type = R;
		static constexpr std::size_t arity = 0;
		struct Argument
		{

		};

	};


	// with args  ...
	template<class R, class... Args>
	struct FunctionTraits<R(Args...)>
	{
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template <std::size_t N>
		struct Argument
		{
			static_assert(N < arity, "error:  Member function OBJECT invalid parameter index.");
			using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
		};
	};


	//------------------------------------------------------------
	// Function traits typedef C pointer function
	template<class R, class... Args>
	struct FunctionTraits<R(*)(Args...)> : public FunctionTraits<R(Args...)>
	{
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template <std::size_t N>
		struct Argument
		{
			static_assert(N < arity, "error: C function invalid parameter index.");
			using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
		};
	};

	//------------------------------------------------------------
	// Function traits member object pointer
	template<class C, class R>
	struct FunctionTraits<R(C::*)> : public FunctionTraits<R(C&)>
	{};

	//------------------------------------------------------------
	// Function traits typedef function member pointer function

	template<class C, class R, class... Args>
	struct FunctionTraits<R(C::*)(Args...)> : public FunctionTraits<R(C&,Args...)>
	{
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template <std::size_t N>
		struct Argument
		{
			static_assert(N < arity, "error: C function invalid parameter index.");
			using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
		};
	};

	// const member function pointer (lambda)
	template<class C, class R, class... Args>
	struct FunctionTraits<R(C::*)(Args...) const> : public FunctionTraits<R(C&,Args...)>
	{
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template <std::size_t N>
		struct Argument
		{
			static_assert(N < arity, "error: Member function invalid parameter index.");
			using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
		};
	};



	// unpack function helper
	template <std::size_t...> struct IndexSequence {};
	template <std::size_t N, std::size_t... Is> struct MakeIndexSequence : MakeIndexSequence<N-1, N-1, Is...> {};
	template <std::size_t... Is> struct MakeIndexSequence<0, Is...> : IndexSequence<Is...> {};


	// Dynaminc unpack parameter function ...
	// template for last parameter ArgIdx == 1
	template <size_t ArgIdx, typename F, typename... Args>
	auto getArgTypes( zs_vector & params)
		-> typename std::enable_if<ArgIdx == 1>::type
	{
		const char *parameter_type=typeid(typename F::template Argument<ArgIdx-1>::type).name();
		params.insert(0,(zs_int)parameter_type);
	}

	// template when parameters ArgIdx > 1
	template <size_t ArgIdx, typename F, typename... Args>
	auto getArgTypes(zs_vector & params)
		-> typename std::enable_if<(ArgIdx > 1)>::type
	{

		const char *parameter_type=typeid(typename F::template Argument<ArgIdx-1>::type).name();
		params.insert(0,(zs_int)parameter_type);
		getArgTypes<ArgIdx - 1,F, Args...>( params);
	}


	// trivial case when parameters (ArgIdx == 0).
	template <size_t ArgIdx, typename F>
	auto getArgTypes(zs_vector & params)
	-> typename std::enable_if<(ArgIdx == 0)>::type
	{
		 // NO ARGS CASE
	}

	template <typename _F, std::size_t... Is>
	auto getParamsFunction(int i,const char ** return_type, zs_vector & type_params, IndexSequence<Is...>)
	-> typename std::enable_if<(_F::arity > 0)>::type
	{
		*return_type = typeid(typename _F::return_type).name();
		getArgTypes<_F::arity, _F, typename _F::template Argument<Is>::type...>(type_params);
	}

	template <typename _F, std::size_t... Is>
	auto getParamsFunction(int i,const char ** return_type, zs_vector & type_params, IndexSequence<Is...>)
	-> typename std::enable_if<(_F::arity == 0)>::type
	{
		*return_type = typeid(typename _F::return_type).name();
		getArgTypes<0, _F>(type_params);
	}

	template <typename F>
	int getNativeMemberFunctionRetArgsTypes(
			 ScriptClassFactory *script_class_factory
			,const zs_string & function_name
			,F ptr_function
			,ScriptFunctionParam **params
			,size_t *params_len
			, const char **str_return_type=NULL
	){
		int idx_return_type=-1;
		const char * return_type;
		zs_vector args;
		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(ptr_function)>;
		getParamsFunction<Traits3>(0, &return_type, args, MakeIndexSequence<Traits3::arity>{});

		if(args.count>MAX_NATIVE_FUNCTION_ARGS){
			THROW_RUNTIME_ERROR("Max arguments reached");
		}

		// check valid parameters ...
		if((idx_return_type=script_class_factory->getIdxClassFromItsNativeType(return_type)) == -1){
			THROW_RUNTIME_ERROR("Return type \"%s\" for function \"%s\" not registered",zs_rtti::demangle(return_type),function_name);
		}

		*params=(ScriptFunctionParam *)malloc(sizeof(ScriptFunctionParam)*args.count);
		*params_len=args.count;

		for(unsigned int i = 0; i < args.count; i++){
			const char *param=(const char *)args.items[i];
			int idx_type = script_class_factory->getIdxClassFromItsNativeType(param);
			// exception: These variables are registered but not allowed to pass throught parameter
			if(idx_type==IDX_BUILTIN_TYPE_ZS_FLOAT_C || idx_type==IDX_BUILTIN_TYPE_BOOL_C || idx_type == IDX_BUILTIN_TYPE_STRING_C){
				THROW_RUNTIME_ERROR("Argument %i type \"%s\" for function \"%s\" is not supported as parameter, you should use pointer instead (i.e %s *)"
						,i+1
						,zs_rtti::demangle(param)
						,function_name,zs_rtti::demangle(param)
				);
			}

			if(idx_type==ZS_IDX_UNDEFINED){
				THROW_RUNTIME_ERROR("Argument %i type \"%s\" for function \"%s\" not registered"
						,i+1
						,zs_rtti::demangle(param)
						,function_name);
			}

			(*params)[i]=ScriptFunctionParam(idx_type,param);
		}

		if(str_return_type != NULL){
			*str_return_type=return_type;
		}

		return idx_return_type;
	}


}
