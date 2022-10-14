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
	auto getArgTypes( zs_vector<zs_int> & params)
		-> typename std::enable_if<ArgIdx == 1>::type
	{
		const char *parameter_type=typeid(typename F::template Argument<ArgIdx-1>::type).name();
		params.insert(0,(zs_int)parameter_type);
	}

	// template when parameters ArgIdx > 1
	template <size_t ArgIdx, typename F, typename... Args>
	auto getArgTypes(zs_vector<zs_int> & params)
		-> typename std::enable_if<(ArgIdx > 1)>::type
	{

		const char *parameter_type=typeid(typename F::template Argument<ArgIdx-1>::type).name();
		params.insert(0,(zs_int)parameter_type);
		getArgTypes<ArgIdx - 1,F, Args...>( params);
	}


	// trivial case when parameters (ArgIdx == 0).
	template <size_t ArgIdx, typename F>
	auto getArgTypes(zs_vector<zs_int> & params)
	-> typename std::enable_if<(ArgIdx == 0)>::type
	{
		ZS_UNUSUED_PARAM(params);
		 // NO ARGS CASE
	}

	template <typename _F, std::size_t... Is>
	auto getParamsFunction(const char ** return_type, zs_vector<zs_int> & type_params, IndexSequence<Is...>)
	-> typename std::enable_if<(_F::arity > 0)>::type
	{
		*return_type = typeid(typename _F::return_type).name();
		getArgTypes<_F::arity, _F, typename _F::template Argument<Is>::type...>(type_params);
	}

	template <typename _F, std::size_t... Is>
	auto getParamsFunction(const char ** return_type, zs_vector<zs_int> & type_params, IndexSequence<Is...>)
	-> typename std::enable_if<(_F::arity == 0)>::type
	{
		*return_type = typeid(typename _F::return_type).name();
		getArgTypes<0, _F>(type_params);
	}

	template <typename F>
	int getNativeFunctionRetArgsTypes(
			 ScriptTypeFactory *_script_class_factory
			,ScriptType * _script_type
			,F _ptr_function
			,ScriptFunctionParam **_params
			,int *_params_len
			, const char **_str_return_type=NULL
	){
		int idx_script_type_return=-1;
		const char * return_type;
		zs_vector<zs_int> args;
		zs_string error="";
		//zs_string name_script_function=_script_type==NULL?_bind_function_description:zs_string(_script_type->str_script_type)+"::"+_bind_function_description;
		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(_ptr_function)>;
		getParamsFunction<Traits3>(&return_type, args, MakeIndexSequence<Traits3::arity>{});

		if(args.size()>MAX_NATIVE_FUNCTION_ARGS){
			THROW_RUNTIME_ERROR(
				"Max arguments reached (max:'%i')"
				,MAX_NATIVE_FUNCTION_ARGS
			);
		}

		// check valid parameters ...
		if((idx_script_type_return=_script_class_factory->getIdxScriptTypeFromTypeNamePtr(return_type)) == -1){
			THROW_RUNTIME_ERROR(
				"Return type '%s' not registered"
				,zs_rtti::demangle(return_type).c_str()
			);
		}

		if(_params != NULL){
			*_params=new ScriptFunctionParam[args.size()];
			*_params_len=args.size();

			if(args.size()==0){
				THROW_RUNTIME_ERRORF(
					"Function to bind has to have 'ZetScript *' as FIRST parameter"
				);
			}

			if(args.size()==1 && _script_type != NULL){
				error=zs_strutils::format(
					"Function to bind has to have '%s' as SECOND parameter for object member reference"
					,zs_rtti::demangle(_script_type->str_script_type_ptr.c_str()).c_str()
				);

			}

			for(int i = 0; i < args.size(); i++){
				const char *str_param=(const char *)args.items[i];
				int idx_script_type = _script_class_factory->getIdxScriptTypeFromTypeNamePtr(str_param);

				if(i==0){
					if(idx_script_type!=IDX_TYPE_SCRIPT_OBJECT_ZETSCRIPT){
						THROW_RUNTIME_ERROR(
							"Expected FIRST parameter as 'ZetScript *' but it was '%s'"
							,zs_rtti::demangle(str_param).c_str()
						);
					}
				}

				if(i==1 && _script_type!=NULL){
					if(strcmp(str_param,_script_type->str_script_type_ptr.c_str())!=0){
						error=zs_strutils::format(
							"SECOND parameter, as object member reference, has to be type '%s' but it was '%s'"
							,zs_rtti::demangle(_script_type->str_script_type_ptr.c_str()).c_str()
							,zs_rtti::demangle(str_param).c_str()
						);
						goto exit_function_traits;
					}
				}

				// exception: These variables are registered but not allowed to pass throught parameter
				if(idx_script_type==IDX_TYPE_ZS_FLOAT_C || idx_script_type==IDX_TYPE_BOOL_C || idx_script_type == IDX_TYPE_ZS_STRING_C){
					error=zs_strutils::format("Argument %i type '%s' is not supported as parameter, you should use pointer instead (i.e '%s *')"
							,i+1
							,zs_rtti::demangle(str_param).c_str()
							,zs_rtti::demangle(str_param).c_str());
					goto exit_function_traits;
				}

				if(idx_script_type==ZS_IDX_UNDEFINED){

					error=zs_strutils::format("Argument %i type '%s' not registered"
						,i+1
						,zs_rtti::demangle(str_param).c_str()
					);
					goto exit_function_traits;
				}

				(*_params)[i]=ScriptFunctionParam(idx_script_type,str_param);
			}
		}

exit_function_traits:

		if(zs_strutils::is_empty(error)==false){

			if(*_params !=NULL){
				delete [] *_params;
			}

			 *_params=NULL;

			THROW_RUNTIME_ERRORF(
					error.c_str()
			);
		}

		if(_str_return_type != NULL){
			*_str_return_type=return_type;
		}

		return idx_script_type_return;

	}


}
