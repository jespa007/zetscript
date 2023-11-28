/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */


#include "core/object/ArrayObject.tcc"
#include "core/object/ClassObject.tcc"
#include "core/object/DictionaryObject.tcc"
#include "core/type/TypeFactory.tcc"



namespace zetscript{

		template<typename T>
		ClassObject * ZetScript::newClassObject(T *_instance){
			//return ClassObject::newShareableClassObject<>(this);
			const char * native_name = typeid(T *).name();
			TypeId type_id=type_factory->getTypeIdFromTypeNamePtr(native_name);

			if(type_id<TYPE_ID_MAX){
				ZS_THROW_RUNTIME_ERROR(
				"Internal Object type '%s' is not instanciable as ClassObject"
				,Rtti::demangle(typeid(T *).name()).toConstChar()
				);
			}

			if(type_id==TYPE_ID_INVALID){
				ZS_THROW_RUNTIME_ERROR(
				"Cannot instance script object as native type '%s' because is not registered"
				,Rtti::demangle(typeid(T *).name()).toConstChar()
				);
			}


			auto so_script=ClassObject::newClassObject(this,type_id,(void *)_instance);
			so_script->deleteNativeObjectOnDestroy(true); // <-- this destroy the native pointer on destroy script class
			return so_script;
		}

		template<typename _C>
		_C ZetScript::stackElementTo(StackElement * _stk){

			String error;
			_C ptr_var;
			String native_name = typeid(_C).name();
			TypeFactory *_script_factory=this->getTypeFactory();
			Type *type = _script_factory->getTypeFromTypeNamePtr(native_name);

			if(type == NULL){
				ZS_THROW_RUNTIME_ERROR("Type '%s' not registered",Rtti::demangle(native_name.toConstChar()).toConstChar());
			}

			if(this->stackElementTo(_stk, type->id, (zs_int *)&ptr_var,error) == false){
				ZS_THROW_RUNTIME_ERROR("Error converting StackElement to '%s': %s"
						,Rtti::demangle(native_name.toConstChar()).toConstChar()
						,error.toConstChar()
				);
			}
			return ptr_var;
		}

		template<typename _C>
		bool ZetScript::canStackElementCastTo(
			StackElement * _stack_element
		){

			String error;
			String native_name = typeid(_C).name();
			TypeFactory *_script_factory=this->getTypeFactory();
			Type *type = _script_factory->getTypeFromTypeNamePtr(native_name);

			if(type == NULL){
				ZS_THROW_RUNTIME_ERROR("Type '%s' not registered",Rtti::demangle(native_name.toConstChar()).toConstChar());
			}

			return this->canStackElementCastTo(
				_stack_element
				,type->id
			);
		}	

		template<typename _C>
		bool ZetScript::stackElementInstanceOf(
			StackElement * _stack_element
		){

			String error;
			String native_name = typeid(_C).name();
			TypeFactory *_script_factory=this->getTypeFactory();
			Type *type = _script_factory->getTypeFromTypeNamePtr(native_name);

			if(type == NULL){
				ZS_THROW_RUNTIME_ERROR("Type '%s' not registered",Rtti::demangle(native_name.toConstChar()).toConstChar());
			}

			return this->canStackElementCastTo(
				_stack_element
				,type->id
				,true
			);
		}			

		template<typename _C>
		_C ZetScript::stackElementTo(StackElement   _stk){
			return stackElementTo<_C>(&_stk);
		}

		template<typename R>
		R ZetScript::checkAndConvertStackElement(StackElement *_stk, TypeId _return_type_id){
			zetscript::String str_error;
			R ret_value;

			// returning instanced types it cannot be C++ types due it can become memory leaks
			if((_return_type_id >= TYPE_ID_MAX) && (_stk->properties & STACK_ELEMENT_PROPERTY_OBJECT)){
				ClassObject *class_object=(ClassObject *)_stk->value;
				if(class_object->was_created_by_constructor == true){
					ZS_THROW_RUNTIME_ERROR("run-time converting result value to '%s'. Returning registered class type objects, return type from bind function signature should be set as 'Class Object * to be dereferenced after its use. (i.e bindFunction<ClassObject *(_type1, _type2, ...)> )"
						,zetscript::Rtti::demangle(class_object->getType()->native_name.toConstChar()).toConstChar()
					);
				}
			}

			if(!this->stackElementTo(_stk,_return_type_id, (zs_int*)(&ret_value),str_error)){
				ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.toConstChar());
			}

			// particular case return type is string and stk is string script object
			if(_return_type_id==TYPE_ID_STRING_C && STACK_ELEMENT_IS_STRING_OBJECT(_stk)){
				Object *so_string=(Object *)_stk->value;
				this->unrefLifetimeObject(so_string);
			}


			return ret_value;
		}

		template<typename _C>
		StackElement	ZetScript::toStackElement( _C _val){
			String error;
			String native_name = typeid(_C).name();
			TypeFactory *_script_factory=this->getTypeFactory();
			Type *type = _script_factory->getTypeFromTypeNamePtr(native_name);

			if(type == NULL){
				ZS_THROW_RUNTIME_ERROR("Type '%s' not registered",Rtti::demangle(native_name.toConstChar()).toConstChar());
			}

			// particular case for zs_float
			if(type->id == TYPE_ID_FLOAT_C){
				return this->toStackElement((zs_int)&_val,TYPE_ID_FLOAT_PTR_C);
			}
			return this->toStackElement((zs_int)_val,type->id);
		}


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
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
		auto ZetScript::bindFunctionBuilder(const char *file,int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type
		{
			TypeId return_type_id = type_factory->getTypeIdFromTypeNamePtr(typeid(R).name());

			*ptr_fun=((void *)(new std::function<R ()>(
				[&,file,line,calling_obj,fun_obj,return_type_id](){
						//String str_error;
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

						/*if(!this->stackElementTo(&stk, return_type_id, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.toConstChar());
						}

						// particular case return type is string and stk is string script object
						if(return_type_id==TypeId::TYPE_ID_STRING_C && STACK_ELEMENT_IS_STRING_OBJECT(&stk)){
							Object *so_string=(Object *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, return_type_id);//;ret_value;
				}
			)));

		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 1 PARAMS
		//
		// template for last parameter argIdx == 1
		template<typename R,typename T,  typename... ArgTypes>
		auto ZetScript::bindFunctionBuilder(const char *file,int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			//return NULL;

			using Param1 = typename T::template Argument<0>::type;
			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());

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
		auto ZetScript::bindFunctionBuilder(const char *file,int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type
		{
			using Param1 = typename T::template Argument<0>::type;

			TypeId return_type_id = type_factory->getTypeIdFromTypeNamePtr(typeid(R).name());
			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());


			*ptr_fun=((void *)(new std::function<R (Param1)>(
				[&,file,line,calling_obj,fun_obj,return_type_id, idx_param1](Param1 p1){

						//R ret_value;
						//String str_error;

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

						/*if(!this->stackElementTo(&stk, return_type_id, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.toConstChar());
						}

						// particular case return type is string and stk is string script object
						if(return_type_id==TypeId::TYPE_ID_STRING_C && STACK_ELEMENT_IS_STRING_OBJECT(&stk)){
							Object *so_string=(Object *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, return_type_id);//;ret_value;
				}
			)));
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 2 PARAMS
		//
		// template when parameters argIdx == 2
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindFunctionBuilder(const char *file,int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;


			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());

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
		auto ZetScript::bindFunctionBuilder(const char *file,int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;


			TypeId return_type_id = type_factory->getTypeIdFromTypeNamePtr(typeid(R).name());
			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());

			*ptr_fun=((void *)(new std::function<R (Param1,Param2)>(
				[&,file,line,calling_obj,fun_obj,return_type_id, idx_param1, idx_param2](Param1 p1,Param2 p2){

						//R ret_value;
						//String str_error;

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

						/*if(!this->stackElementTo(&stk, return_type_id, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.toConstChar());
						}

						// particular case return type is string and stk is string script object
						if(return_type_id==TypeId::TYPE_ID_STRING_C && STACK_ELEMENT_IS_STRING_OBJECT(&stk)){
							Object *so_string=(Object *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, return_type_id);//;ret_value;
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
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;


			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());


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
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;

			TypeId return_type_id = type_factory->getTypeIdFromTypeNamePtr(typeid(R).name());
			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());

			*ptr_fun=((void *)(new std::function<R (Param1,Param2,Param3)>(
				[&,file,line,calling_obj,fun_obj,return_type_id, idx_param1, idx_param2, idx_param3](Param1 p1,Param2 p2,Param3 p3){
					//R ret_value;
					//String str_error;

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

					/*if(!this->stackElementTo(&stk, return_type_id, (zs_int *)(&ret_value),str_error)){
						ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.toConstChar());
					}

					// particular case return type is string and stk is string script object
					if(return_type_id==TypeId::TYPE_ID_STRING_C && STACK_ELEMENT_IS_STRING_OBJECT(&stk)){
						Object *so_string=(Object *)stk.value;
						this->unrefLifetimeObject(so_string);
					}*/


					return this->checkAndConvertStackElement<R>(&stk, return_type_id);//;ret_value;
				}
			)));
		}


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 4 PARAMS
		//
		// template when parameters argIdx == 4
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;

			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());


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
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;

			TypeId return_type_id = type_factory->getTypeIdFromTypeNamePtr(typeid(R).name());
			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());

			*ptr_fun=((void *)(new std::function<R (Param1,Param2,Param3,Param4)>(
				[&,file,line,calling_obj,fun_obj,return_type_id, idx_param1, idx_param2, idx_param3, idx_param4](Param1 p1,Param2 p2,Param3 p3,Param4 p4){
						//R ret_value;
						//String str_error;

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

						/*if(!this->stackElementTo(&stk, return_type_id, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.toConstChar());
						}

						// particular case return type is string and stk is string script object
						if(return_type_id==TypeId::TYPE_ID_STRING_C && STACK_ELEMENT_IS_STRING_OBJECT(&stk)){
							Object *so_string=(Object *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, return_type_id);//;ret_value;
				}
			)));

		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 5 PARAMS
		//
		// template when parameters argIdx == 5
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;



			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());
			TypeId idx_param5 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param5).name());


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
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;

			TypeId return_type_id = type_factory->getTypeIdFromTypeNamePtr(typeid(R).name());
			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());
			TypeId idx_param5 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param5).name());

			*ptr_fun=((void *)(new std::function<R (Param1,Param2,Param3,Param4,Param5)>(
				[&,file,line,calling_obj,fun_obj,return_type_id, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5){

					//R ret_value;
					//String str_error;

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

					/*if(!this->stackElementTo(&stk, return_type_id, (zs_int *)(&ret_value),str_error)){
						ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.toConstChar());
					}

					// particular case return type is string and stk is string script object
					if(return_type_id==TypeId::TYPE_ID_STRING_C && STACK_ELEMENT_IS_STRING_OBJECT(&stk)){
						Object *so_string=(Object *)stk.value;
						this->unrefLifetimeObject(so_string);
					}*/


					return this->checkAndConvertStackElement<R>(&stk, return_type_id);//;ret_value;
				}
			)));
		}


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 6 PARAMS
		//
		// template when parameters argIdx == 6
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;


			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());
			TypeId idx_param5 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param5).name());
			TypeId idx_param6 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param6).name());




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
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;

			TypeId return_type_id = type_factory->getTypeIdFromTypeNamePtr(typeid(R).name());
			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());
			TypeId idx_param5 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param5).name());
			TypeId idx_param6 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param6).name());

			*ptr_fun=((void *)(new std::function<R (Param1,Param2,Param3,Param4,Param5,Param6)>(
				[&,file,line,calling_obj,fun_obj,return_type_id, idx_param1, idx_param2, idx_param3, idx_param4, idx_param5, idx_param6](Param1 p1,Param2 p2,Param3 p3,Param4 p4,Param5 p5,Param6 p6){

						//R ret_value;
						//String str_error;

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

						/*if(!this->stackElementTo(&stk, return_type_id, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.toConstChar());
						}

						// particular case return type is string and stk is string script object
						if(return_type_id==TypeId::TYPE_ID_STRING_C && STACK_ELEMENT_IS_STRING_OBJECT(&stk)){
							Object *so_string=(Object *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, return_type_id);//;ret_value;

				}
			)));
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 7 PARAMS
		//
		// template when parameters argIdx == 7
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 7)>::type
		{

			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;
			using Param7 = typename T::template Argument<6>::type;


			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());
			TypeId idx_param5 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param5).name());
			TypeId idx_param6 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param6).name());
			TypeId idx_param7 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param7).name());

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
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 7)>::type
		{
			using Param1 = typename T::template Argument<0>::type;
			using Param2 = typename T::template Argument<1>::type;
			using Param3 = typename T::template Argument<2>::type;
			using Param4 = typename T::template Argument<3>::type;
			using Param5 = typename T::template Argument<4>::type;
			using Param6 = typename T::template Argument<5>::type;
			using Param7 = typename T::template Argument<6>::type;

			TypeId return_type_id = type_factory->getTypeIdFromTypeNamePtr(typeid(R).name());
			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());
			TypeId idx_param5 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param5).name());
			TypeId idx_param6 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param6).name());
			TypeId idx_param7 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param7).name());

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
					 ,return_type_id
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
						//String str_error;

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

						/*if(!this->stackElementTo(&stk, return_type_id, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.toConstChar());
						}

						// particular case return type is string and stk is string script object
						if(return_type_id==TypeId::TYPE_ID_STRING_C && STACK_ELEMENT_IS_STRING_OBJECT(&stk)){
							Object *so_string=(Object *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, return_type_id);//;ret_value;

				}
			)));
		}
		//--------------------------------------------------------------------------------------------------------------------
		//
		// 8 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
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


			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());
			TypeId idx_param5 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param5).name());
			TypeId idx_param6 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param6).name());
			TypeId idx_param7 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param7).name());
			TypeId idx_param8 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param8).name());

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
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
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

			TypeId return_type_id = type_factory->getTypeIdFromTypeNamePtr(typeid(R).name());
			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());
			TypeId idx_param5 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param5).name());
			TypeId idx_param6 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param6).name());
			TypeId idx_param7 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param7).name());
			TypeId idx_param8 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param8).name());

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
					 ,return_type_id
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
						//String str_error;

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

						/*if(!this->stackElementTo(&stk, return_type_id, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.toConstChar());
						}

						// particular case return type is string and stk is string script object
						if(return_type_id==TypeId::TYPE_ID_STRING_C && STACK_ELEMENT_IS_STRING_OBJECT(&stk)){
							Object *so_string=(Object *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, return_type_id);//;ret_value;

				}
			)));
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 9 PARAMS
		//
		// template when parameters argIdx == 9
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
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


			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());
			TypeId idx_param5 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param5).name());
			TypeId idx_param6 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param6).name());
			TypeId idx_param7 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param7).name());
			TypeId idx_param8 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param8).name());
			TypeId idx_param9 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param9).name());

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
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
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

			TypeId return_type_id = type_factory->getTypeIdFromTypeNamePtr(typeid(R).name());
			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());
			TypeId idx_param5 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param5).name());
			TypeId idx_param6 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param6).name());
			TypeId idx_param7 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param7).name());
			TypeId idx_param8 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param8).name());
			TypeId idx_param9 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param9).name());

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
					 ,return_type_id
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
						//String str_error;

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

						/*if(!this->stackElementTo(&stk, return_type_id, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.toConstChar());
						}

						// particular case return type is string and stk is string script object
						if(return_type_id==TypeId::TYPE_ID_STRING_C && STACK_ELEMENT_IS_STRING_OBJECT(&stk)){
							Object *so_string=(Object *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, return_type_id);//;ret_value;

				}
			)));
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 10 PARAMS
		//
		// template when parameters argIdx == 9
		template <typename R,typename T, typename... ArgTypes>
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
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


			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());
			TypeId idx_param5 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param5).name());
			TypeId idx_param6 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param6).name());
			TypeId idx_param7 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param7).name());
			TypeId idx_param8 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param8).name());
			TypeId idx_param9 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param9).name());
			TypeId idx_param10 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param10).name());

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
		auto ZetScript::bindFunctionBuilder(const char *file, int line,void **ptr_fun,Object *calling_obj,Function *fun_obj)
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

			TypeId return_type_id = type_factory->getTypeIdFromTypeNamePtr(typeid(R).name());
			TypeId idx_param1 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param1).name());
			TypeId idx_param2 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param2).name());
			TypeId idx_param3 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param3).name());
			TypeId idx_param4 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param4).name());
			TypeId idx_param5 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param5).name());
			TypeId idx_param6 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param6).name());
			TypeId idx_param7 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param7).name());
			TypeId idx_param8 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param8).name());
			TypeId idx_param9 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param9).name());
			TypeId idx_param10 = type_factory->getTypeIdFromTypeNamePtr(typeid(Param10).name());

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
					 ,return_type_id
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
						//String str_error;

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

						/*if(!this->stackElementTo(&stk, return_type_id, (zs_int *)(&ret_value),str_error)){
							ZS_THROW_RUNTIME_ERROR("run-time error converting result value:%s",str_error.toConstChar());
						}

						// particular case return type is string and stk is string script object
						if(return_type_id==TypeId::TYPE_ID_STRING_C && STACK_ELEMENT_IS_STRING_OBJECT(&stk)){
							Object *so_string=(Object *)stk.value;
							this->unrefLifetimeObject(so_string);
						}*/


						return this->checkAndConvertStackElement<R>(&stk, return_type_id);//;ret_value;

				}
			)));
		}

		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename F, std::size_t... Is>
		 auto ZetScript::bindFunctionBuilderBase(const char *file, int line,void **ptr_fun, Object *calling_obj,Function *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type
		{
			 bindFunctionBuilder<typename F::return_type, F,  typename F::template Argument<Is>::type...>(file,line,ptr_fun,calling_obj,fun_obj);
		}



		 template <typename F, std::size_t... Is>
		 auto ZetScript::bindFunctionBuilderBase(const char *file, int line, void **ptr_fun, Object *calling_obj,Function *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type
		{
			 bindFunctionBuilder<typename F::return_type, F>(file,line,ptr_fun,calling_obj,fun_obj);
		}

		template <  typename F>
		std::function<F> ZetScript::bindFunction(Function *fun,Object *calling_obj, const char *file, int line){

			const char *return_type;
			Vector<zs_int> params;
			TypeId return_type_id=TYPE_ID_INVALID;
			void *ptr=NULL;


			// 1. check all parameters ok.
			using Traits3 = FunctionTraits<F>;
			getParamsFunction<Traits3>(&return_type, params, MakeIndexSequence<Traits3::arity>{});

			// 2. check valid parameters ...
			if((return_type_id=type_factory->getTypeIdFromTypeNamePtr(return_type)) == -1){
				ZS_THROW_RUNTIME_ERROR("Return type '%s' for bind function not registered",Rtti::demangle(return_type).toConstChar());
				return NULL;
			}

			if(
					return_type_id == TYPE_ID_STRING_PTR_C
					|| return_type_id == TYPE_ID_CONST_CHAR_PTR_C
			){
				ZS_THROW_RUNTIME_ERROR("Return type '%s' is not supported",Rtti::demangle(return_type).toConstChar());
				return NULL;
			}

			for(int i = 0; i < params.length(); i++){
				char *str_param=(char *)params.get(i);
				TypeId type_id=type_factory->getTypeIdFromTypeNamePtr(str_param);

				// exception: These variables are registered but not allowed to pass throught parameter
				if(
							type_id == TYPE_ID_FLOAT_C
						||  type_id == TYPE_ID_BOOL_C
						||  type_id == TYPE_ID_STRING_C
				){
					ZS_THROW_RUNTIME_ERROR("Argument %i type '%s' is not supported as parameter, you should use pointer instead (i.e '%s *')"
							,i+1
							,Rtti::demangle(str_param).toConstChar()
							,Rtti::demangle(str_param).toConstChar());
					return NULL;
				}

				if(type_id==TYPE_ID_INVALID){
					ZS_THROW_RUNTIME_ERROR("Argument %i type '%s' for bind function not registered"
							,i+1
							,Rtti::demangle(str_param).toConstChar());
					return NULL;
				}
			}

			// 3. build custom function in function of parameter number ...
			bindFunctionBuilderBase<Traits3>(file,line,&ptr,calling_obj,fun,MakeIndexSequence<Traits3::arity>{});

			// copy f and deallocate
			auto f=*((std::function<F> *)ptr);
			delete (std::function<F> *)ptr;

			return f;

		}

		template <  typename F>
		std::function<F> ZetScript::bindFunction(MemberFunctionObject *fun, const char *file, int line){
			return bindFunction<F>(fun->sf_ref,fun->getRefObject(), file, line);
		}

		template <  typename F>
		std::function<F> ZetScript::bindFunction(Function *fun, const char *file, int line){
			return bindFunction<F>(fun,NULL, file, line);
		}


		template <  typename F>
		std::function<F> ZetScript::bindFunction(const String & function_access, const char *file, int line)
		{
			std::function<F> return_function=NULL;
			Function * fun_obj=NULL;
			Object *calling_obj=NULL;
			Vector<String> access_var = StringUtils::split(function_access,'.');
			Function * main_function = type_factory->getMainFunction();
			StackElement *se=NULL;
			Symbol *symbol_sfm=NULL;


			// 1. some variable in main function ...
			if(access_var.length()>1){
				for(int i=0; i < access_var.length()-1; i++){
					const char *symbol_to_find=access_var.get(i).toConstChar();
					if(i==0){ // get variable through main_class.main_function (global element)
						Vector<Symbol *> *list_variables=main_function->scope->symbol_variables;
						for(int j = 0; j < list_variables->length() && calling_obj==NULL; j++){
							Symbol * registered_symbol=(Symbol *)list_variables->get(j);
							if(registered_symbol->name==symbol_to_find
							&& registered_symbol->scope == ZS_MAIN_SCOPE(this)){
								StackElement *stk = vm_get_stack_element_at(virtual_machine,j); // main_function->object_info.local_symbols.variable[j].
								if(stk!=NULL){
									if(stk->properties & STACK_ELEMENT_PROPERTY_OBJECT){
										calling_obj=(Object *)stk->value;
									}
								}
								else{
									ZS_THROW_EXCEPTION_FILE_LINE(
											file
											,line
											,"Cannot bind script function '%s': cannot access i (%i)"
											,function_access.toConstChar()
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
									,function_access.toConstChar()
									,symbol_to_find
							);
						}

					}else{ // we have got the calling_obj from last iteration ...
						se = calling_obj->getStackElementByKeyName(symbol_to_find);
						if(se!=NULL){
							if(se->properties & STACK_ELEMENT_PROPERTY_OBJECT){
								calling_obj=(Object *)se->value;
							}else{
								ZS_THROW_EXCEPTION_FILE_LINE(
										file
										,line
										,"Cannot bind script function '%s': Variable name '%s' not script variable"
										,function_access.toConstChar()
										,symbol_to_find
								);
							}
						}
						else{
							ZS_THROW_EXCEPTION_FILE_LINE(
								file
								,line
								,"Cannot bind script function '%s': Variable name '%s' doesn't exist"
								,function_access.toConstChar()
								,symbol_to_find
							);
						}
					}
				}

				symbol_sfm=calling_obj->getType()->getSymbolMemberFunction(access_var.get(access_var.length()-1).toConstChar());
				if(symbol_sfm!=NULL){
					Function *test_fun=NULL;
					if(symbol_sfm->properties & SYMBOL_PROPERTY_FUNCTION){
						test_fun=(Function *)symbol_sfm->ref_ptr;
					}

					if(test_fun!=NULL && (test_fun->properties & FUNCTION_PROPERTY_MEMBER_FUNCTION)){
						fun_obj=test_fun;
					}
				}else{
					ZS_THROW_EXCEPTION_FILE_LINE(
							file
							,line
							,"Cannot bind script function '%s': Cannot find function '%s'"
							,function_access.toConstChar()
							,access_var.get(access_var.length()-1).toConstChar()
					);
				}

			}else{ // some function in main function
				String symbol_to_find=access_var.get(0);
				Vector<Symbol *> *list_functions=main_function->scope->symbol_functions;

				for(int i = 0; i < list_functions->length() && fun_obj==NULL; i++){
					Symbol *symbol=(Symbol *)list_functions->get(i);
					Function *aux_fun_obj=(Function *)symbol->ref_ptr;
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
						,function_access.toConstChar()
						,access_var.get(access_var.length()-1).toConstChar()
				);
			}
			try{
				return_function=bindFunction<F>(fun_obj,calling_obj,file,line);
			}catch(std::exception & ex){
				ZS_THROW_EXCEPTION_FILE_LINE(file,line,"Cannot bind script function '%s': %s",function_access.toConstChar(),ex.what());
			}


			return return_function;

		}
}
