/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */


#include 	"script/script_type/ScriptTypeFactory.tcc"
#include 	"script/script_object/ArrayScriptObject.tcc"
#include 	"script/script_object/ObjectScriptObject.tcc"
#include 	"script/script_object/ClassScriptObject.tcc"



namespace zetscript{

		template<typename T>
		ClassScriptObject * ZetScript::newClassScriptObject(T *_instance){
			//return ClassScriptObject::newShareableClassScriptObject<>(this);
			const char * native_name = typeid(T *).name();
			int id=script_type_factory->getScriptTypeIdFromTypeNamePtr(native_name);

			if(id<ScriptTypeId::SCRIPT_TYPE_ID_MAX){
				ZS_THROW_RUNTIME_ERROR(
				"Internal ScriptObject type '%s' is not instanciable as ClassScriptObject"
				,zs_rtti::demangle(typeid(T *).name()).c_str()
				);
			}

			if(id==ZS_UNDEFINED_IDX){
				ZS_THROW_RUNTIME_ERROR(
				"Cannot instance script object as native type '%s' because is not registered"
				,zs_rtti::demangle(typeid(T *).name()).c_str()
				);
			}


			auto so_script=ClassScriptObject::newClassScriptObject(this,id,(void *)_instance);
			so_script->deleteNativeObjectOnDestroy(true); // <-- this destroy the native pointer on destroy script class
			return so_script;
		}

		template<typename _C>
		_C ZetScript::stackElementTo(StackElement * _stk){

			zs_string error;
			_C ptr_var;
			zs_string native_name = typeid(_C).name();
			ScriptTypeFactory *_script_factory=this->getScriptTypeFactory();
			ScriptType *script_type = _script_factory->getScriptTypeFromTypeNamePtr(native_name);

			if(script_type == NULL){
				ZS_THROW_RUNTIME_ERROR("Type '%s' not registered",zs_rtti::demangle(native_name.c_str()).c_str());
			}

			if(this->stackElementTo(_stk, script_type->id, (zs_int *)&ptr_var,error)==false){
				ZS_THROW_RUNTIME_ERROR("Error converting StackElement to '%s': %s"
						,zs_rtti::demangle(native_name.c_str()).c_str()
						,error.c_str()
				);
			}
			return ptr_var;
		}

		template<typename _C>
		bool ZetScript::canStackElementCastTo(
			StackElement * _stack_element
		){

			zs_string error;
			zs_string native_name = typeid(_C).name();
			ScriptTypeFactory *_script_factory=this->getScriptTypeFactory();
			ScriptType *script_type = _script_factory->getScriptTypeFromTypeNamePtr(native_name);

			if(script_type == NULL){
				ZS_THROW_RUNTIME_ERROR("Type '%s' not registered",zs_rtti::demangle(native_name.c_str()).c_str());
			}

			return this->canStackElementCastTo(
				_stack_element
				,script_type->id
			);
		}	

		template<typename _C>
		bool ZetScript::stackElementInstanceOf(
			StackElement * _stack_element
		){

			zs_string error;
			zs_string native_name = typeid(_C).name();
			ScriptTypeFactory *_script_factory=this->getScriptTypeFactory();
			ScriptType *script_type = _script_factory->getScriptTypeFromTypeNamePtr(native_name);

			if(script_type == NULL){
				ZS_THROW_RUNTIME_ERROR("Type '%s' not registered",zs_rtti::demangle(native_name.c_str()).c_str());
			}

			return this->canStackElementCastTo(
				_stack_element
				,script_type->id
				,true
			);
		}			

		template<typename _C>
		_C ZetScript::stackElementTo(StackElement   _stk){
			return stackElementTo<_C>(&_stk);
		}

		template<typename R>
		R ZetScript::checkAndConvertStackElement(StackElement *_stk, int _idx_return){
			zetscript::zs_string str_error;
			R ret_value;

			// returning instanced types it cannot be C++ types due it can become memory leaks
			if((_idx_return >= ScriptTypeId::SCRIPT_TYPE_ID_MAX) && (_stk->properties & STACK_ELEMENT_PROPERTY_SCRIPT_OBJECT)){
				ClassScriptObject *class_script_object=(ClassScriptObject *)_stk->value;
				if(class_script_object->was_created_by_constructor == true){
					ZS_THROW_RUNTIME_ERROR("run-time converting result value to '%s'. Returning registered class type objects, return type from bind function signature should be set as 'Class ScriptObject * to be dereferenced after its use. (i.e bindScriptFunction<ClassScriptObject *(_type1, _type2, ...)> )"
						,zetscript::zs_rtti::demangle(class_script_object->getScriptType()->native_name.c_str()).c_str()
					);
				}
			}

			if(!this->stackElementTo(_stk,_idx_return, (zs_int*)(&ret_value),str_error)){
				ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.c_str());
			}

			// particular case return type is string and stk is string script object
			if(_idx_return==ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(_stk)){
				ScriptObject *so_string=(ScriptObject *)_stk->value;
				this->unrefLifetimeObject(so_string);
			}


			return ret_value;
		}

		template<typename _C>
		StackElement	ZetScript::toStackElement( _C _val){
			zs_string error;
			zs_string native_name = typeid(_C).name();
			ScriptTypeFactory *_script_factory=this->getScriptTypeFactory();
			ScriptType *script_type = _script_factory->getScriptTypeFromTypeNamePtr(native_name);

			if(script_type == NULL){
				ZS_THROW_RUNTIME_ERROR("Type '%s' not registered",zs_rtti::demangle(native_name.c_str()).c_str());
			}

			// particular case for zs_float
			if(script_type->id==ScriptTypeId::SCRIPT_TYPE_ID_FLOAT_C){
				return this->toStackElement((zs_int)&_val,ScriptTypeId::SCRIPT_TYPE_ID_FLOAT_PTR_C);
			}
			return this->toStackElement((zs_int)_val,script_type->id);
		}


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<std::is_same<R,void>::value>::type
		{


			*ptr_fun=((void *)(new std::function<void ()>(
				[&,file,line,calling_obj,fun_obj](){
					vm_execute(
						virtual_machine
						,calling_obj
						,fun_obj
						,NULL
						,0
						,ZS_VM_PROPERTY_CALL_FROM_NATIVE
						,file
						,line
					);

				}
			)));
		}

		template <typename R,typename T>
		auto ZetScript::bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type
		{
			int idx_return = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(R).name());

			*ptr_fun=((void *)(new std::function<R ()>(
				[&,file,line,calling_obj,fun_obj,idx_return](){
						//zs_string str_error;
						//R ret_value;

						StackElement stk = vm_execute(
								virtual_machine
								,calling_obj
								,fun_obj
								,NULL
								,0
								,ZS_VM_PROPERTY_CALL_FROM_NATIVE
								,file
								,line
						);

						/*if(!this->stackElementTo(&stk, idx_return, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.c_str());
						}

						// particular case return type is string and stk is string script object
						if(idx_return==ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&stk)){
							ScriptObject *so_string=(ScriptObject *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, idx_return);//;ret_value;
				}
			)));

		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 1 PARAMS
		//
		// template for last parameter argIdx == 1
		template<typename R,typename T,  typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			//return NULL;

			using Param1 = typename T::template Argument<0>::type;
			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());

			*ptr_fun=((void *)(new std::function<void (Param1)>(
				[&,file,line,calling_obj,fun_obj, idx_param1](Param1 p1){

					StackElement args[1]={
							 this->toStackElement((zs_int)p1,idx_param1)
					};

					vm_execute(
								virtual_machine
								,calling_obj
								,fun_obj
								,args
								,1
								,ZS_VM_PROPERTY_CALL_FROM_NATIVE
								,file
								,line);
				}
			)));
		}

		template<typename R,typename T,  typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			using Param1 = typename T::template Argument<0>::type;

			int idx_return = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(R).name());
			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());


			*ptr_fun=((void *)(new std::function<R (Param1)>(
				[&,file,line,calling_obj,fun_obj,idx_return, idx_param1](Param1 p1){

						//R ret_value;
						//zs_string str_error;

						StackElement args[1]={
								this->toStackElement((zs_int)p1,idx_param1)
						};

						StackElement stk = vm_execute(
								virtual_machine
								,calling_obj
								,fun_obj
								,args
								,1
								,ZS_VM_PROPERTY_CALL_FROM_NATIVE
								,file
								,line);

						/*if(!this->stackElementTo(&stk, idx_return, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.c_str());
						}

						// particular case return type is string and stk is string script object
						if(idx_return==ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&stk)){
							ScriptObject *so_string=(ScriptObject *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, idx_return);//;ret_value;
				}
			)));
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 2 PARAMS
		//
		// template when parameters argIdx == 2
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;


			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());

			*ptr_fun=((void *)(new std::function<void (Param1,Param2)>(
				[&,file,line,calling_obj,fun_obj, idx_param1, idx_param2](Param1 p1,Param2 p2){

					StackElement args[2]={
							 this->toStackElement((zs_int)p1,idx_param1)
							,this->toStackElement((zs_int)p2,idx_param2)

					};

					vm_execute(
						virtual_machine
						,calling_obj
						,fun_obj
						,args
						,2
						,ZS_VM_PROPERTY_CALL_FROM_NATIVE
						,file
						,line
					);

				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;


			int idx_return = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(R).name());
			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());

			*ptr_fun=((void *)(new std::function<R (Param1,Param2)>(
				[&,file,line,calling_obj,fun_obj,idx_return, idx_param1, idx_param2](Param1 p1,Param2 p2){

						//R ret_value;
						//zs_string str_error;

						StackElement args[2]={
								 this->toStackElement((zs_int)p1,idx_param1)
								,this->toStackElement((zs_int)p2,idx_param2)

						};

						StackElement stk = vm_execute(
							virtual_machine
							,calling_obj
							,fun_obj
							,args
							,2
							,ZS_VM_PROPERTY_CALL_FROM_NATIVE
							,file
							,line
						);

						/*if(!this->stackElementTo(&stk, idx_return, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.c_str());
						}

						// particular case return type is string and stk is string script object
						if(idx_return==ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&stk)){
							ScriptObject *so_string=(ScriptObject *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, idx_return);//;ret_value;
				}
			)));

			//return NULL;
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 3 PARAMS
		//
		// template when parameters argIdx == 3
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;


			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());


			*ptr_fun=((void *)(new std::function<void (Param1,Param2,Param3)>(
				[&,file,line,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3](Param1 p1,Param2 p2,Param3 p3){


					StackElement args[3]={
							 this->toStackElement((zs_int)p1,idx_param1)
							,this->toStackElement((zs_int)p2,idx_param2)
							,this->toStackElement((zs_int)p3,idx_param3)
					};

					vm_execute(
							virtual_machine
							,calling_obj
							,fun_obj
							,args
							,3
							,ZS_VM_PROPERTY_CALL_FROM_NATIVE
							,file
							,line
					);

				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;

			int idx_return = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(R).name());
			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());

			*ptr_fun=((void *)(new std::function<R (Param1,Param2,Param3)>(
				[&,file,line,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3](Param1 p1,Param2 p2,Param3 p3){
					//R ret_value;
					//zs_string str_error;

					StackElement args[3]={
							 this->toStackElement((zs_int)p1,idx_param1)
							,this->toStackElement((zs_int)p2,idx_param2)
							,this->toStackElement((zs_int)p3,idx_param3)
					};

					StackElement stk = vm_execute(
						virtual_machine
						,calling_obj
						,fun_obj
						,args
						,3
						,ZS_VM_PROPERTY_CALL_FROM_NATIVE
						,file
						,line
					);

					/*if(!this->stackElementTo(&stk, idx_return, (zs_int *)(&ret_value),str_error)){
						ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.c_str());
					}

					// particular case return type is string and stk is string script object
					if(idx_return==ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&stk)){
						ScriptObject *so_string=(ScriptObject *)stk.value;
						this->unrefLifetimeObject(so_string);
					}*/


					return this->checkAndConvertStackElement<R>(&stk, idx_return);//;ret_value;
				}
			)));
		}


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 4 PARAMS
		//
		// template when parameters argIdx == 4
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;

			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());


			*ptr_fun=((void *)(new std::function<void (Param1,Param2,Param3,Param4)>(
				[&,file,line,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4](Param1 p1,Param2 p2,Param3 p3,Param4 p4){

					StackElement args[4]={
							 this->toStackElement((zs_int)p1,idx_param1)
							,this->toStackElement((zs_int)p2,idx_param2)
							,this->toStackElement((zs_int)p3,idx_param3)
							,this->toStackElement((zs_int)p4,idx_param4)
					};

					vm_execute(
						virtual_machine
						,calling_obj
						,fun_obj
						,args
						,4
						,ZS_VM_PROPERTY_CALL_FROM_NATIVE
						,file
						,line
					);

				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;

			int idx_return = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(R).name());
			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());

			*ptr_fun=((void *)(new std::function<R (Param1,Param2,Param3,Param4)>(
				[&,file,line,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4](Param1 p1,Param2 p2,Param3 p3,Param4 p4){
						//R ret_value;
						//zs_string str_error;

						StackElement args[4]={
								 this->toStackElement((zs_int)p1,idx_param1)
								,this->toStackElement((zs_int)p2,idx_param2)
								,this->toStackElement((zs_int)p3,idx_param3)
								,this->toStackElement((zs_int)p4,idx_param4)
						};

						StackElement stk = vm_execute(
								virtual_machine
								,calling_obj
								,fun_obj
								,args
								,4
								,ZS_VM_PROPERTY_CALL_FROM_NATIVE
								,file
								,line
								);

						/*if(!this->stackElementTo(&stk, idx_return, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.c_str());
						}

						// particular case return type is string and stk is string script object
						if(idx_return==ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&stk)){
							ScriptObject *so_string=(ScriptObject *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, idx_return);//;ret_value;
				}
			)));

		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 5 PARAMS
		//
		// template when parameters argIdx == 5
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;



			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());
			int idx_param5 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param5).name());


			*ptr_fun=((void *)(new std::function<void (Param1,Param2,Param3,Param4,Param5)>(
				[&,file,line,calling_obj,fun_obj,idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5){

					StackElement args[5]={
							 this->toStackElement((zs_int)p1,idx_param1)
							,this->toStackElement((zs_int)p2,idx_param2)
							,this->toStackElement((zs_int)p3,idx_param3)
							,this->toStackElement((zs_int)p4,idx_param4)
							,this->toStackElement((zs_int)p5,idx_param5)

					};

					vm_execute(
							virtual_machine
							,calling_obj
							,fun_obj
							,args
							,5
							,ZS_VM_PROPERTY_CALL_FROM_NATIVE
							,file
							,line);
				}

			)));
		}


		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;

			int idx_return = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(R).name());
			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());
			int idx_param5 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param5).name());

			*ptr_fun=((void *)(new std::function<R (Param1,Param2,Param3,Param4,Param5)>(
				[&,file,line,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5){

					//R ret_value;
					//zs_string str_error;

					StackElement args[5]={
							 this->toStackElement((zs_int)p1,idx_param1)
							,this->toStackElement((zs_int)p2,idx_param2)
							,this->toStackElement((zs_int)p3,idx_param3)
							,this->toStackElement((zs_int)p4,idx_param4)
							,this->toStackElement((zs_int)p5,idx_param5)

					};

					StackElement stk = vm_execute(
							virtual_machine
							,calling_obj
							,fun_obj
							,args
							,5
							,ZS_VM_PROPERTY_CALL_FROM_NATIVE
							,file
							,line);

					/*if(!this->stackElementTo(&stk, idx_return, (zs_int *)(&ret_value),str_error)){
						ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.c_str());
					}

					// particular case return type is string and stk is string script object
					if(idx_return==ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&stk)){
						ScriptObject *so_string=(ScriptObject *)stk.value;
						this->unrefLifetimeObject(so_string);
					}*/


					return this->checkAndConvertStackElement<R>(&stk, idx_return);//;ret_value;
				}
			)));
		}


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 6 PARAMS
		//
		// template when parameters argIdx == 6
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;


			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());
			int idx_param5 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param5).name());
			int idx_param6 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param6).name());




			*ptr_fun=((void *)(new std::function<void (Param1,Param2,Param3,Param4,Param5,Param6)>(
				[&,file,line,calling_obj,fun_obj, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5,Param6 p6){

					StackElement args[6]={
							 this->toStackElement((zs_int)p1,idx_param1)
							,this->toStackElement((zs_int)p2,idx_param2)
							,this->toStackElement((zs_int)p3,idx_param3)
							,this->toStackElement((zs_int)p4,idx_param4)
							,this->toStackElement((zs_int)p5,idx_param5)
							,this->toStackElement((zs_int)p6,idx_param6)
					};

					vm_execute(
							 virtual_machine
							,calling_obj
							,fun_obj
							,args
							,6
							,ZS_VM_PROPERTY_CALL_FROM_NATIVE
							,file
							,line);
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;

			int idx_return = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(R).name());
			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());
			int idx_param5 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param5).name());
			int idx_param6 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param6).name());

			*ptr_fun=((void *)(new std::function<R (Param1,Param2,Param3,Param4,Param5,Param6)>(
				[&,file,line,calling_obj,fun_obj,idx_return, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5,Param6 p6){

						//R ret_value;
						//zs_string str_error;

						StackElement args[6]={
								 this->toStackElement((zs_int)p1,idx_param1)
								,this->toStackElement((zs_int)p2,idx_param2)
								,this->toStackElement((zs_int)p3,idx_param3)
								,this->toStackElement((zs_int)p4,idx_param4)
								,this->toStackElement((zs_int)p5,idx_param5)
								,this->toStackElement((zs_int)p6,idx_param6)
						};

						StackElement stk = vm_execute(
								virtual_machine,
								calling_obj,
								fun_obj,
								args,
								6
								,ZS_VM_PROPERTY_CALL_FROM_NATIVE
								,file
								,line);

						/*if(!this->stackElementTo(&stk, idx_return, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.c_str());
						}

						// particular case return type is string and stk is string script object
						if(idx_return==ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&stk)){
							ScriptObject *so_string=(ScriptObject *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, idx_return);//;ret_value;

				}
			)));
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 7 PARAMS
		//
		// template when parameters argIdx == 7
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 7)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;
			using Param7 = typename T::template Argument<6>::type;


			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());
			int idx_param5 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param5).name());
			int idx_param6 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param6).name());
			int idx_param7 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param7).name());

			*ptr_fun=((void *)(new std::function<void (
					Param1
					,Param2
					,Param3
					,Param4
					,Param5
					,Param6
					,Param7
			)>(
			[
			 	 &
				 ,file
				 ,line
				 ,calling_obj
				 ,fun_obj
				 , idx_param1
				 , idx_param2
				 , idx_param3
				 , idx_param4
				 , idx_param5
				 , idx_param6
				 , idx_param7
			](
				  Param1 p1
				 ,Param2 p2
				 ,Param3 p3
				 ,Param4 p4
				 ,Param5 p5
				 ,Param6 p6
				 ,Param7 p7
				){

					StackElement args[7]={
						 this->toStackElement((zs_int)p1,idx_param1)
						,this->toStackElement((zs_int)p2,idx_param2)
						,this->toStackElement((zs_int)p3,idx_param3)
						,this->toStackElement((zs_int)p4,idx_param4)
						,this->toStackElement((zs_int)p5,idx_param5)
						,this->toStackElement((zs_int)p6,idx_param6)
						,this->toStackElement((zs_int)p7,idx_param7)
					};

					vm_execute(
							 virtual_machine
							,calling_obj
							,fun_obj
							,args
							,7
							,ZS_VM_PROPERTY_CALL_FROM_NATIVE
							,file
							,line);
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 7)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;
			using Param7 = typename T::template Argument<6>::type;

			int idx_return = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(R).name());
			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());
			int idx_param5 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param5).name());
			int idx_param6 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param6).name());
			int idx_param7 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param7).name());

			*ptr_fun=((void *)(new std::function<R (
					Param1
					,Param2
					,Param3
					,Param4
					,Param5
					,Param6
					,Param7
			)>(
				[
					 &
					 ,file
					 ,line
					 ,calling_obj
					 ,fun_obj
					 ,idx_return
					 , idx_param1
					 , idx_param2
					 , idx_param3
					 , idx_param4
					 , idx_param5
					 , idx_param6
					 , idx_param7
				 ](
					 Param1 p1
					 ,Param2 p2
					 ,Param3 p3
					 ,Param4 p4
					 ,Param5 p5
					 ,Param6 p6
					 ,Param7 p7
				){

						//R ret_value;
						//zs_string str_error;

						StackElement args[7]={
								 this->toStackElement((zs_int)p1,idx_param1)
								,this->toStackElement((zs_int)p2,idx_param2)
								,this->toStackElement((zs_int)p3,idx_param3)
								,this->toStackElement((zs_int)p4,idx_param4)
								,this->toStackElement((zs_int)p5,idx_param5)
								,this->toStackElement((zs_int)p6,idx_param6)
								,this->toStackElement((zs_int)p7,idx_param7)
						};

						StackElement stk = vm_execute(
								virtual_machine,
								calling_obj,
								fun_obj,
								args,
								7
								,ZS_VM_PROPERTY_CALL_FROM_NATIVE
								,file
								,line);

						/*if(!this->stackElementTo(&stk, idx_return, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.c_str());
						}

						// particular case return type is string and stk is string script object
						if(idx_return==ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&stk)){
							ScriptObject *so_string=(ScriptObject *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, idx_return);//;ret_value;

				}
			)));
		}
		//--------------------------------------------------------------------------------------------------------------------
		//
		// 8 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 8)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;
			using Param7 = typename T::template Argument<6>::type;
			using Param8 = typename T::template Argument<7>::type;


			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());
			int idx_param5 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param5).name());
			int idx_param6 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param6).name());
			int idx_param7 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param7).name());
			int idx_param8 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param8).name());

			*ptr_fun=((void *)(new std::function<void (
					Param1
					,Param2
					,Param3
					,Param4
					,Param5
					,Param6
					,Param7
					,Param8
			)>(
			[
			 	 &
				 ,file
				 ,line
				 ,calling_obj
				 ,fun_obj
				 , idx_param1
				 , idx_param2
				 , idx_param3
				 , idx_param4
				 , idx_param5
				 , idx_param6
				 , idx_param7
				 , idx_param8
			](
				  Param1 p1
				 ,Param2 p2
				 ,Param3 p3
				 ,Param4 p4
				 ,Param5 p5
				 ,Param6 p6
				 ,Param7 p7
				 ,Param8 p8
				){

					StackElement args[8]={
						 this->toStackElement((zs_int)p1,idx_param1)
						,this->toStackElement((zs_int)p2,idx_param2)
						,this->toStackElement((zs_int)p3,idx_param3)
						,this->toStackElement((zs_int)p4,idx_param4)
						,this->toStackElement((zs_int)p5,idx_param5)
						,this->toStackElement((zs_int)p6,idx_param6)
						,this->toStackElement((zs_int)p7,idx_param7)
						,this->toStackElement((zs_int)p8,idx_param8)
					};

					vm_execute(
							 virtual_machine
							,calling_obj
							,fun_obj
							,args
							,8
							,ZS_VM_PROPERTY_CALL_FROM_NATIVE
							,file
							,line);
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 8)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;
			using Param7 = typename T::template Argument<6>::type;
			using Param8 = typename T::template Argument<7>::type;

			int idx_return = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(R).name());
			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());
			int idx_param5 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param5).name());
			int idx_param6 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param6).name());
			int idx_param7 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param7).name());
			int idx_param8 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param8).name());

			*ptr_fun=((void *)(new std::function<R (
					Param1
					,Param2
					,Param3
					,Param4
					,Param5
					,Param6
					,Param7
					,Param8
			)>(
				[
					 &
					 ,file
					 ,line
					 ,calling_obj
					 ,fun_obj
					 ,idx_return
					 , idx_param1
					 , idx_param2
					 , idx_param3
					 , idx_param4
					 , idx_param5
					 , idx_param6
					 , idx_param7
					 , idx_param8
				 ](
					 Param1 p1
					 ,Param2 p2
					 ,Param3 p3
					 ,Param4 p4
					 ,Param5 p5
					 ,Param6 p6
					 ,Param7 p7
					 ,Param8 p8
				){

						//R ret_value;
						//zs_string str_error;

						StackElement args[8]={
								 this->toStackElement((zs_int)p1,idx_param1)
								,this->toStackElement((zs_int)p2,idx_param2)
								,this->toStackElement((zs_int)p3,idx_param3)
								,this->toStackElement((zs_int)p4,idx_param4)
								,this->toStackElement((zs_int)p5,idx_param5)
								,this->toStackElement((zs_int)p6,idx_param6)
								,this->toStackElement((zs_int)p7,idx_param7)
								,this->toStackElement((zs_int)p8,idx_param8)
						};

						StackElement stk = vm_execute(
								virtual_machine,
								calling_obj,
								fun_obj,
								args,
								8
								,ZS_VM_PROPERTY_CALL_FROM_NATIVE
								,file
								,line);

						/*if(!this->stackElementTo(&stk, idx_return, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.c_str());
						}

						// particular case return type is string and stk is string script object
						if(idx_return==ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&stk)){
							ScriptObject *so_string=(ScriptObject *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, idx_return);//;ret_value;

				}
			)));
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 9 PARAMS
		//
		// template when parameters argIdx == 9
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 9)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;
			using Param7 = typename T::template Argument<6>::type;
			using Param8 = typename T::template Argument<7>::type;
			using Param9 = typename T::template Argument<8>::type;


			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());
			int idx_param5 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param5).name());
			int idx_param6 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param6).name());
			int idx_param7 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param7).name());
			int idx_param8 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param8).name());
			int idx_param9 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param9).name());

			*ptr_fun=((void *)(new std::function<void (
					Param1
					,Param2
					,Param3
					,Param4
					,Param5
					,Param6
					,Param7
					,Param8
					,Param9
			)>(
			[
			 	 &
				 ,file
				 ,line
				 ,calling_obj
				 ,fun_obj
				 , idx_param1
				 , idx_param2
				 , idx_param3
				 , idx_param4
				 , idx_param5
				 , idx_param6
				 , idx_param7
				 , idx_param8
				 , idx_param9
			](
				  Param1 p1
				 ,Param2 p2
				 ,Param3 p3
				 ,Param4 p4
				 ,Param5 p5
				 ,Param6 p6
				 ,Param7 p7
				 ,Param8 p8
				 ,Param9 p9
				){

					StackElement args[9]={
						 this->toStackElement((zs_int)p1,idx_param1)
						,this->toStackElement((zs_int)p2,idx_param2)
						,this->toStackElement((zs_int)p3,idx_param3)
						,this->toStackElement((zs_int)p4,idx_param4)
						,this->toStackElement((zs_int)p5,idx_param5)
						,this->toStackElement((zs_int)p6,idx_param6)
						,this->toStackElement((zs_int)p7,idx_param7)
						,this->toStackElement((zs_int)p8,idx_param8)
						,this->toStackElement((zs_int)p9,idx_param9)
					};

					vm_execute(
							 virtual_machine
							,calling_obj
							,fun_obj
							,args
							,9
							,ZS_VM_PROPERTY_CALL_FROM_NATIVE
							,file
							,line);
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 9)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;
			using Param7 = typename T::template Argument<6>::type;
			using Param8 = typename T::template Argument<7>::type;
			using Param9 = typename T::template Argument<8>::type;

			int idx_return = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(R).name());
			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());
			int idx_param5 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param5).name());
			int idx_param6 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param6).name());
			int idx_param7 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param7).name());
			int idx_param8 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param8).name());
			int idx_param9 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param9).name());

			*ptr_fun=((void *)(new std::function<R (
					Param1
					,Param2
					,Param3
					,Param4
					,Param5
					,Param6
					,Param7
					,Param8
					,Param9
			)>(
				[
					 &
					 ,file
					 ,line
					 ,calling_obj
					 ,fun_obj
					 ,idx_return
					 , idx_param1
					 , idx_param2
					 , idx_param3
					 , idx_param4
					 , idx_param5
					 , idx_param6
					 , idx_param7
					 , idx_param8
					 , idx_param9
				 ](
					 Param1 p1
					 ,Param2 p2
					 ,Param3 p3
					 ,Param4 p4
					 ,Param5 p5
					 ,Param6 p6
					 ,Param7 p7
					 ,Param8 p8
					 ,Param9 p9
				){

						//R ret_value;
						//zs_string str_error;

						StackElement args[9]={
								 this->toStackElement((zs_int)p1,idx_param1)
								,this->toStackElement((zs_int)p2,idx_param2)
								,this->toStackElement((zs_int)p3,idx_param3)
								,this->toStackElement((zs_int)p4,idx_param4)
								,this->toStackElement((zs_int)p5,idx_param5)
								,this->toStackElement((zs_int)p6,idx_param6)
								,this->toStackElement((zs_int)p7,idx_param7)
								,this->toStackElement((zs_int)p8,idx_param8)
								,this->toStackElement((zs_int)p9,idx_param9)
						};

						StackElement stk = vm_execute(
								virtual_machine,
								calling_obj,
								fun_obj,
								args,
								9
								,ZS_VM_PROPERTY_CALL_FROM_NATIVE
								,file
								,line);

						/*if(!this->stackElementTo(&stk, idx_return, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.c_str());
						}

						// particular case return type is string and stk is string script object
						if(idx_return==ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&stk)){
							ScriptObject *so_string=(ScriptObject *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, idx_return);//;ret_value;

				}
			)));
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 10 PARAMS
		//
		// template when parameters argIdx == 9
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 10)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;
			using Param7 = typename T::template Argument<6>::type;
			using Param8 = typename T::template Argument<7>::type;
			using Param9 = typename T::template Argument<8>::type;
			using Param10 = typename T::template Argument<9>::type;


			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());
			int idx_param5 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param5).name());
			int idx_param6 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param6).name());
			int idx_param7 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param7).name());
			int idx_param8 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param8).name());
			int idx_param9 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param9).name());
			int idx_param10 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param10).name());

			*ptr_fun=((void *)(new std::function<void (
					Param1
					,Param2
					,Param3
					,Param4
					,Param5
					,Param6
					,Param7
					,Param8
					,Param9
					,Param10
			)>(
			[
			 	 &
				 ,file
				 ,line
				 ,calling_obj
				 ,fun_obj
				 , idx_param1
				 , idx_param2
				 , idx_param3
				 , idx_param4
				 , idx_param5
				 , idx_param6
				 , idx_param7
				 , idx_param8
				 , idx_param9
				 , idx_param10
			](
				  Param1 p1
				 ,Param2 p2
				 ,Param3 p3
				 ,Param4 p4
				 ,Param5 p5
				 ,Param6 p6
				 ,Param7 p7
				 ,Param8 p8
				 ,Param9 p9
				 ,Param10 p10
				){

					StackElement args[10]={
						 this->toStackElement((zs_int)p1,idx_param1)
						,this->toStackElement((zs_int)p2,idx_param2)
						,this->toStackElement((zs_int)p3,idx_param3)
						,this->toStackElement((zs_int)p4,idx_param4)
						,this->toStackElement((zs_int)p5,idx_param5)
						,this->toStackElement((zs_int)p6,idx_param6)
						,this->toStackElement((zs_int)p7,idx_param7)
						,this->toStackElement((zs_int)p8,idx_param8)
						,this->toStackElement((zs_int)p9,idx_param9)
						,this->toStackElement((zs_int)p10,idx_param10)
					};

					vm_execute(
							 virtual_machine
							,calling_obj
							,fun_obj
							,args
							,10
							,ZS_VM_PROPERTY_CALL_FROM_NATIVE
							,file
							,line);
				}

			)));
		}

		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindScriptFunctionBuilder(const char *file, int line,void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 10)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;
			using Param7 = typename T::template Argument<6>::type;
			using Param8 = typename T::template Argument<7>::type;
			using Param9 = typename T::template Argument<8>::type;
			using Param10 = typename T::template Argument<9>::type;

			int idx_return = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(R).name());
			int idx_param1 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param1).name());
			int idx_param2 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param2).name());
			int idx_param3 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param3).name());
			int idx_param4 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param4).name());
			int idx_param5 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param5).name());
			int idx_param6 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param6).name());
			int idx_param7 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param7).name());
			int idx_param8 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param8).name());
			int idx_param9 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param9).name());
			int idx_param10 = script_type_factory->getScriptTypeIdFromTypeNamePtr(typeid(Param10).name());

			*ptr_fun=((void *)(new std::function<R (
					Param1
					,Param2
					,Param3
					,Param4
					,Param5
					,Param6
					,Param7
					,Param8
					,Param9
					,Param10
			)>(
				[
					 &
					 ,file
					 ,line
					 ,calling_obj
					 ,fun_obj
					 ,idx_return
					 , idx_param1
					 , idx_param2
					 , idx_param3
					 , idx_param4
					 , idx_param5
					 , idx_param6
					 , idx_param7
					 , idx_param8
					 , idx_param9
					 , idx_param10
				 ](
					 Param1 p1
					 ,Param2 p2
					 ,Param3 p3
					 ,Param4 p4
					 ,Param5 p5
					 ,Param6 p6
					 ,Param7 p7
					 ,Param8 p8
					 ,Param9 p9
					 ,Param10 p10
				){

						//R ret_value;
						//zs_string str_error;

						StackElement args[10]={
								 this->toStackElement((zs_int)p1,idx_param1)
								,this->toStackElement((zs_int)p2,idx_param2)
								,this->toStackElement((zs_int)p3,idx_param3)
								,this->toStackElement((zs_int)p4,idx_param4)
								,this->toStackElement((zs_int)p5,idx_param5)
								,this->toStackElement((zs_int)p6,idx_param6)
								,this->toStackElement((zs_int)p7,idx_param7)
								,this->toStackElement((zs_int)p8,idx_param8)
								,this->toStackElement((zs_int)p9,idx_param9)
								,this->toStackElement((zs_int)p10,idx_param10)
						};

						StackElement stk = vm_execute(
								virtual_machine,
								calling_obj,
								fun_obj,
								args,
								10
								,ZS_VM_PROPERTY_CALL_FROM_NATIVE
								,file
								,line);

						/*if(!this->stackElementTo(&stk, idx_return, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.c_str());
						}

						// particular case return type is string and stk is string script object
						if(idx_return==ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C && STACK_ELEMENT_IS_STRING_SCRIPT_OBJECT(&stk)){
							ScriptObject *so_string=(ScriptObject *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, idx_return);//;ret_value;

				}
			)));
		}

		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename F, std::size_t... Is>
		 auto ZetScript::bindScriptFunctionBuilderBase(const char *file, int line,void **ptr_fun, ScriptObject *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type
		{
			 bindScriptFunctionBuilder<typename F::return_type, F,  typename F::template Argument<Is>::type...>(file,line,ptr_fun,calling_obj,fun_obj);
		}



		 template <typename F, std::size_t... Is>
		 auto ZetScript::bindScriptFunctionBuilderBase(const char *file, int line, void **ptr_fun, ScriptObject *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type
		{
			 bindScriptFunctionBuilder<typename F::return_type, F>(file,line,ptr_fun,calling_obj,fun_obj);
		}

		template <  typename F>
		std::function<F> ZetScript::bindScriptFunction(ScriptFunction *fun,ScriptObject *calling_obj, const char *file, int line){

			const char *return_type;
			zs_vector<zs_int> params;
			int return_script_type_id=-1;
			void *ptr=NULL;


			// 1. check all parameters ok.
			using Traits3 = FunctionTraits<F>;
			getParamsFunction<Traits3>(&return_type, params, MakeIndexSequence<Traits3::arity>{});

			// 2. check valid parameters ...
			if((return_script_type_id=script_type_factory->getScriptTypeIdFromTypeNamePtr(return_type)) == -1){
				ZS_THROW_RUNTIME_ERROR("Return type '%s' for bind function not registered",zs_rtti::demangle(return_type).c_str());
				return NULL;
			}

			if(
					return_script_type_id == ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_PTR_C
					|| return_script_type_id == ScriptTypeId::SCRIPT_TYPE_ID_CONST_CHAR_PTR_C
			){
				ZS_THROW_RUNTIME_ERROR("Return type '%s' is not supported",zs_rtti::demangle(return_type).c_str());
				return NULL;
			}

			for(int i = 0; i < params.size(); i++){
				char *str_param=(char *)params.get(i);
				zs_int id=script_type_factory->getScriptTypeIdFromTypeNamePtr(str_param);

				// exception: These variables are registered but not allowed to pass throught parameter
				if(
						id==ScriptTypeId::SCRIPT_TYPE_ID_FLOAT_C
						|| id==ScriptTypeId::SCRIPT_TYPE_ID_BOOL_C
						|| id == ScriptTypeId::SCRIPT_TYPE_ID_ZS_STRING_C
				){
					ZS_THROW_RUNTIME_ERROR("Argument %i type '%s' is not supported as parameter, you should use pointer instead (i.e '%s *')"
							,i+1
							,zs_rtti::demangle(str_param).c_str()
							,zs_rtti::demangle(str_param).c_str());
					return NULL;
				}

				if(id==-1){
					ZS_THROW_RUNTIME_ERROR("Argument %i type '%s' for bind function not registered"
							,i+1
							,zs_rtti::demangle(str_param).c_str());
					return NULL;
				}
			}

			// 3. build custom function in function of parameter number ...
			bindScriptFunctionBuilderBase<Traits3>(file,line,&ptr,calling_obj,fun,MakeIndexSequence<Traits3::arity>{});

			// copy f and deallocate
			auto f=*((std::function<F> *)ptr);
			delete (std::function<F> *)ptr;

			return f;

		}

		template <  typename F>
		std::function<F> ZetScript::bindScriptFunction(MemberFunctionScriptObject *fun, const char *file, int line){
			return bindScriptFunction<F>(fun->sf_ref,fun->getRefObject(), file, line);
		}

		template <  typename F>
		std::function<F> ZetScript::bindScriptFunction(ScriptFunction *fun, const char *file, int line){
			return bindScriptFunction<F>(fun,NULL, file, line);
		}


		template <  typename F>
		std::function<F> ZetScript::bindScriptFunction(const zs_string & function_access, const char *file, int line)
		{
			std::function<F> return_function=NULL;
			ScriptFunction * fun_obj=NULL;
			ScriptObject *calling_obj=NULL;
			zs_vector<zs_string> access_var = zs_strutils::split(function_access,'.');
			ScriptFunction * main_function = script_type_factory->getMainFunction();
			StackElement *se=NULL;
			Symbol *symbol_sfm=NULL;


			// 1. some variable in main function ...
			if(access_var.size()>1){
				for(int i=0; i < access_var.size()-1; i++){
					const char *symbol_to_find=access_var.get(i).c_str();
					if(i==0){ // get variable through main_class.main_function (global element)
						zs_vector<Symbol *> *list_variables=main_function->scope->symbol_variables;
						for(int j = 0; j < list_variables->size() && calling_obj==NULL; j++){
							Symbol * registered_symbol=(Symbol *)list_variables->get(j);
							if(registered_symbol->name==symbol_to_find
							&& registered_symbol->scope == ZS_MAIN_SCOPE(this)){
								StackElement *stk = vm_get_stack_element_at(virtual_machine,j); // main_function->object_info.local_symbols.variable[j].
								if(stk!=NULL){
									if(stk->properties & STACK_ELEMENT_PROPERTY_SCRIPT_OBJECT){
										calling_obj=(ScriptObject *)stk->value;
									}
								}
								else{
									ZS_THROW_EXCEPTION_FILE_LINE(
											file
											,line
											,"Cannot bind script function '%s': cannot access i (%i)"
											,function_access.c_str()
											,j
									);
								}
							}
						}

						if(calling_obj == NULL){
							ZS_THROW_EXCEPTION_FILE_LINE(
									file
									,line
									,"Cannot bind script function '%s': Variable name '%s' doesn't exist"
									,function_access.c_str()
									,symbol_to_find
							);
						}

					}else{ // we have got the calling_obj from last iteration ...
						se = calling_obj->getStackElementByKeyName(symbol_to_find);
						if(se!=NULL){
							if(se->properties & STACK_ELEMENT_PROPERTY_SCRIPT_OBJECT){
								calling_obj=(ScriptObject *)se->value;
							}else{
								ZS_THROW_EXCEPTION_FILE_LINE(
										file
										,line
										,"Cannot bind script function '%s': Variable name '%s' not script variable"
										,function_access.c_str()
										,symbol_to_find
								);
							}
						}
						else{
							ZS_THROW_EXCEPTION_FILE_LINE(
								file
								,line
								,"Cannot bind script function '%s': Variable name '%s' doesn't exist"
								,function_access.c_str()
								,symbol_to_find
							);
						}
					}
				}

				symbol_sfm=calling_obj->getScriptType()->getSymbolMemberFunction(access_var.get(access_var.size()-1).c_str());
				if(symbol_sfm!=NULL){
					ScriptFunction *test_fun=NULL;
					if(symbol_sfm->properties & ZS_SYMBOL_PROPERTY_FUNCTION){
						test_fun=(ScriptFunction *)symbol_sfm->ref_ptr;
					}

					if(test_fun!=NULL && (test_fun->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)){
						fun_obj=test_fun;
					}
				}else{
					ZS_THROW_EXCEPTION_FILE_LINE(
							file
							,line
							,"Cannot bind script function '%s': Cannot find function '%s'"
							,function_access.c_str()
							,access_var.get(access_var.size()-1).c_str()
					);
				}

			}else{ // some function in main function
				zs_string symbol_to_find=access_var.get(0);
				zs_vector<Symbol *> *list_functions=main_function->scope->symbol_functions;

				for(int i = 0; i < list_functions->size() && fun_obj==NULL; i++){
					Symbol *symbol=(Symbol *)list_functions->get(i);
					ScriptFunction *aux_fun_obj=(ScriptFunction *)symbol->ref_ptr;
					if(	aux_fun_obj->name == symbol_to_find){
						fun_obj=aux_fun_obj;
					}

				}
			}

			if(fun_obj==NULL){
				ZS_THROW_EXCEPTION_FILE_LINE(
						file
						,line
						,"Cannot bind script function '%s': Variable name '%s' is not found or not function type"
						,function_access.c_str()
						,access_var.get(access_var.size()-1).c_str()
				);
			}
			try{
				return_function=bindScriptFunction<F>(fun_obj,calling_obj,file,line);
			}catch(std::exception & ex){
				ZS_THROW_EXCEPTION_FILE_LINE(file,line,"Cannot bind script function '%s': %s",function_access.c_str(),ex.what());
			}


			return return_function;

		}
}
