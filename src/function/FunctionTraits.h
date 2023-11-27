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
	auto getArgTypes( Vector<zs_int> & params)
		-> typename std::enable_if<ArgIdx == 1>::type
	{
		const char *parameter_type=typeid(typename F::template Argument<ArgIdx-1>::type).name();
		params.insert(0,(zs_int)parameter_type);
	}

	// template when parameters ArgIdx > 1
	template <size_t ArgIdx, typename F, typename... Args>
	auto getArgTypes(Vector<zs_int> & params)
		-> typename std::enable_if<(ArgIdx > 1)>::type
	{

		const char *parameter_type=typeid(typename F::template Argument<ArgIdx-1>::type).name();
		params.insert(0,(zs_int)parameter_type);
		getArgTypes<ArgIdx - 1,F, Args...>( params);
	}


	// trivial case when parameters (ArgIdx == 0).
	template <size_t ArgIdx, typename F>
	auto getArgTypes(Vector<zs_int> & params)
	-> typename std::enable_if<(ArgIdx == 0)>::type
	{
		ZS_UNUSUED_PARAM(params);
		 // NO ARGS CASE
	}

	template <typename _F, std::size_t... Is>
	auto getParamsFunction(const char ** return_type, Vector<zs_int> & type_params, IndexSequence<Is...>)
	-> typename std::enable_if<(_F::arity > 0)>::type
	{
		*return_type = typeid(typename _F::return_type).name();
		getArgTypes<_F::arity, _F, typename _F::template Argument<Is>::type...>(type_params);
	}

	template <typename _F, std::size_t... Is>
	auto getParamsFunction(const char ** return_type, Vector<zs_int> & type_params, IndexSequence<Is...>)
	-> typename std::enable_if<(_F::arity == 0)>::type
	{
		*return_type = typeid(typename _F::return_type).name();
		getArgTypes<0, _F>(type_params);
	}

	template <typename F>
	TypeId getNativeFunctionRetArgsTypes(
			 TypeFactory *_script_class_factory
			,Type * _type
			,F _ptr_function
			,FunctionParam **_params
			,int *_params_len
			, const char **_str_return_type=NULL
	){
		TypeId return_type_id=TYPE_ID_INVALID;
		const char * return_type;
		Vector<zs_int> args;
		String error="";
		//String name=_type==NULL?_bind_function_description:String(_type->name)+"::"+_bind_function_description;
		// 1. check all parameters ok.
		using Traits3 = FunctionTraits<decltype(_ptr_function)>;
		getParamsFunction<Traits3>(&return_type, args, MakeIndexSequence<Traits3::arity>{});

		if(args.size()>ZS_MAX_NATIVE_FUNCTION_ARGS){
			ZS_THROW_RUNTIME_ERROR(
				"Max arguments reached (max:'%i')"
				,ZS_MAX_NATIVE_FUNCTION_ARGS
			);
		}

		// check valid parameters ...
		if((return_type_id=_script_class_factory->getTypeIdFromTypeNamePtr(return_type)) == -1){
			ZS_THROW_RUNTIME_ERROR(
				"Return type '%s' not registered"
				,Rtti::demangle(return_type).toConstChar()
			);
		}

		if(_params != NULL){
			*_params=new FunctionParam[args.size()];
			*_params_len=args.size();

			if(args.size()==0){
				ZS_THROW_RUNTIME_ERRORF(
					"Function to bind has to have 'ZetScript *' as FIRST parameter"
				);
			}

			if(args.size()==1 && _type != NULL){
				error=String::format(
					"Function to bind has to have '%s' as SECOND parameter for object member reference"
					,Rtti::demangle(_type->native_name.toConstChar()).toConstChar()
				);

			}

			for(int i = 0; i < args.size(); i++){
				const char *str_param=(const char *)args.get(i);
				TypeId type_id = _script_class_factory->getTypeIdFromTypeNamePtr(str_param);

				if(i==0){
					if(type_id!=TYPE_ID_OBJECT_ZETSCRIPT){
						ZS_THROW_RUNTIME_ERROR(
							"Expected FIRST parameter as 'ZetScript *' but it was '%s'"
							,Rtti::demangle(str_param).toConstChar()
						);
					}
				}

				if(i==1 && _type!=NULL){
					if(strcmp(str_param,_type->native_name.toConstChar())!=0){
						error=String::format(
							"SECOND parameter, as object member reference, has to be type '%s' but it was '%s'"
							,Rtti::demangle(_type->native_name.toConstChar()).toConstChar()
							,Rtti::demangle(str_param).toConstChar()
						);
						goto exit_function_traits;
					}
				}

				// exception: These variables are registered but not allowed to pass throught parameter
				if(type_id==TYPE_ID_FLOAT_C || type_id==TYPE_ID_BOOL_C || type_id == TYPE_ID_ZS_STRING_C){
					error=String::format("Argument %i type '%s' is not supported as parameter, you should use pointer instead (i.e '%s *')"
							,i+1
							,Rtti::demangle(str_param).toConstChar()
							,Rtti::demangle(str_param).toConstChar());
					goto exit_function_traits;
				}

				if(type_id==TYPE_ID_INVALID){

					error=String::format("Argument %i type '%s' not registered"
						,i+1
						,Rtti::demangle(str_param).toConstChar()
					);
					goto exit_function_traits;
				}

				(*_params)[i]=FunctionParam(type_id,str_param);
			}
		}

exit_function_traits:

		if(String::isEmpty(error)==false){

			if(*_params !=NULL){
				delete [] *_params;
			}

			 *_params=NULL;

			ZS_THROW_RUNTIME_ERRORF(
					error.toConstChar()
			);
		}

		if(_str_return_type != NULL){
			*_str_return_type=return_type;
		}

		return return_type_id;

	}


}
