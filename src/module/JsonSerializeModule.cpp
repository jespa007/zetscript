/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

#define ZS_JSON_SERIALIZE_NEW_LINE(default_str_value,ident) \
	default_str_value.append('\n');\
	for (int k = 0; k < (ident); k++)	default_str_value.append('\t');

	namespace json{

		void serialize_stk(
				zetscript::ScriptEngine *_script_engine
				,ScriptObject *_this_object
				,String & _str_result
				, StackElement *_stk
				, int _ident
				,bool _is_formatted
				, bool _strict_json_format
		);


		void serialize_array(
				zetscript::ScriptEngine *_script_engine
				,ScriptObject *_this_object
				,String & _str_result
				, ArrayScriptObject * _array
				,int _ident
				, bool _is_formatted
				,bool _strict_json_format
				){

			_str_result.append('[');

			for (int i = 0; i < _array->length(); i++) {
				if (i > 0) {
					_str_result.append(',');
				}

				serialize_stk(
					_script_engine
					,_this_object
					,_str_result
					,_array->getStackElementByIndex(i)
					,_ident
					,_is_formatted
					,_strict_json_format
				);
			}
			_str_result.append(']');
		}

		void serialize_object(
			zetscript::ScriptEngine *_script_engine
			,ScriptObject *_this_object
			, String & _str_result
			, ObjectScriptObject *_obj
			, int _ident
			, bool _is_formatted
			,bool _strict_json_format
			){

			_str_result.append('{');

			MapStringIterator fields[2]={
					_obj->getMapStringFields()->begin()
					,_obj->getMapStringBuiltinFields()->begin()
			};

			for(int i=0; i < 2; i++){//MapStringIterator *it=map_iterators;mi!=map_iterators.end();mi++){
				int k=0;
				MapStringIterator *mi=&fields[i];
				for(;!mi->end();mi->next()){

					StackElement stk_se=*((StackElement *)mi->value);

					// depack container slot
					if(stk_se.properties & STACK_ELEMENT_PROPERTY_CONTAINER_SLOT){
						stk_se.value=(zs_int)((ContainerSlot *)(stk_se.value))->getSrcContainerRef();
						stk_se.properties=STACK_ELEMENT_PROPERTY_OBJECT;
					}

					StackElementMemberProperty *smp=NULL;

					if(stk_se.properties & STACK_ELEMENT_PROPERTY_MEMBER_PROPERTY){
						smp=(StackElementMemberProperty *)stk_se.value;
						if(smp->member_property->metamethod_members.getter == NULL){ // ignore value due it doesn't has getter
							continue;
						}
					}

					// Serialize values as primitives or objects and ignore functions or script object memeber functions
					if(
						((stk_se.properties & STACK_ELEMENT_PROPERTY_FUNCTION) == 0)
							&&
						STACK_ELEMENT_IS_MEMBER_FUNCTION_OBJECT(&stk_se) == false
					){
						bool getter_found=false;
						bool value_from_vm_execute=false;
						StackElement ptr_stk_param=k_stk_undefined;
						StackElement stk_getter_result=k_stk_undefined;

						if (_is_formatted){
							_str_result.append('\n');
							for (int j = 0; j <= (_ident); j++){
								_str_result.append('\t');
							}
						}

						if (k>0){
							_str_result.append(',');
						}

						_str_result.append("\"" + String(mi->key)+ "\":");

						// if is member property we have to call _get if exist...
						if(smp!=NULL){

							ScriptFunction *ptr_function=(ScriptFunction *)(smp->member_property->metamethod_members.getter->ref_ptr);

							if(ptr_function!=NULL && _obj->script_type_id > SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT){ // getter found

								getter_found=true;

								if((ptr_function->properties & SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF) == 0){

									value_from_vm_execute=true;

									stk_getter_result=ZS_VM_EXECUTE(
										_script_engine->getVirtualMachine()
										,_obj
										,ptr_function
										,NULL
										,0
									);


								}else{ // expect return an scriptobjectstring

									// Change undefined type as null valid in json scope
									zs_int result= 0;
									String str_aux="";
									void *c_object = ((ClassScriptObject *)_obj)->getNativeObject();

									switch(ptr_function->return_script_type_id){
									case  SCRIPT_TYPE_ID_STRING:
										str_aux=((String (*)(ScriptEngine *,void *))(ptr_function->ref_native_function_ptr))(_script_engine,c_object);
										result = (zs_int)&str_aux;
										break;
									case  SCRIPT_TYPE_ID_FLOAT:
										ZS_WRITE_INTPTR_FLOAT(&result,((zs_float (*)(ScriptEngine *,void *))(ptr_function->ref_native_function_ptr))(_script_engine,c_object));
										break;
									default:
										result=((zs_int (*)(ScriptEngine *,void *))(ptr_function->ref_native_function_ptr))(_script_engine,c_object);
										break;
									}
									stk_getter_result=_script_engine->toStackElement(
										result
										,ptr_function->return_script_type_id
									);
								}

								ptr_stk_param=stk_getter_result;
							}
						}

						if(getter_found == false){
							ptr_stk_param=stk_se;
						}

						serialize_stk(
							_script_engine
							,_this_object
							, _str_result
							, &ptr_stk_param
							, _ident+1
							,_is_formatted
							,_strict_json_format);

						if(stk_getter_result.properties & STACK_ELEMENT_PROPERTY_OBJECT){
							if(value_from_vm_execute==true){
								vm_unref_lifetime_object(
									_script_engine->getVirtualMachine()
									,(ScriptObject *)stk_getter_result.value
								);
							}else{
								delete ((ScriptObject *)stk_getter_result.value);
							}

						}
						k++;
					}
				}
			}

			if(_is_formatted){
				_str_result.append('\n');
				for (int i = 0; i < (_ident); i++){
					_str_result.append('\t');
				}
			}
			_str_result.append('}');
		}

		void serialize_stk(
			zetscript::ScriptEngine *_script_engine
			, ScriptObject *_this_object
			,String & _str_result
			, StackElement *_stk
			, int _ident
			,bool _is_formatted
			,bool _strict_json_format
		){

			ScriptObject *obj=NULL;
			int16_t var_type = 0;
			StackElement stk=*_stk;

			stk=_script_engine->unwrapStackElement(stk);

			var_type = STACK_ELEMENT_TYPE_PROPERTIES(stk.properties);

			switch(var_type){
			default:
				break;
			case STACK_ELEMENT_PROPERTY_FLOAT:
			case STACK_ELEMENT_PROPERTY_BOOL:
			case STACK_ELEMENT_PROPERTY_INT:
				_str_result.append(_script_engine->stackElementValueToString(&stk));
				break;
			case STACK_ELEMENT_PROPERTY_NULL:
				_str_result.append("null");
				break;
			case STACK_ELEMENT_PROPERTY_UNDEFINED:
				if(_strict_json_format==true){
					_str_result.append("null");
				}else{
					_str_result.append("undefined");
				}
				break;
			case STACK_ELEMENT_PROPERTY_OBJECT: // vector or object

				obj=((ScriptObject *)stk.value);
				switch(obj->script_type_id){
				case SCRIPT_TYPE_ID_STRING_SCRIPT_OBJECT:
					_str_result.append(String("\"") + ((StringScriptObject *)obj)->toString() + "\"");
					break;
				case SCRIPT_TYPE_ID_ARRAY_SCRIPT_OBJECT:
					serialize_array(_script_engine, _this_object, _str_result,(ArrayScriptObject *)obj,_ident,_is_formatted,_strict_json_format);
					break;
				default:
					if(
						obj->script_type_id>=SCRIPT_TYPE_ID_OBJECT_SCRIPT_OBJECT
					){
						if(_this_object != obj){ // avoid recursivity
							serialize_object(_script_engine,_this_object,_str_result,(ObjectScriptObject *)obj,_ident,_is_formatted,_strict_json_format);
						}
						else{
							_str_result.append("\"ScriptObject@"+String(_this_object->getScriptTypeName())+"\"");
						}
					}
					break;
				}
				break;
			}
		}

		String serialize(ScriptEngine *_script_engine, StackElement *_stk, bool _is_formatted, bool _strict_json_format){
			String serialized_stk="";

			if(
					(_stk->properties & STACK_ELEMENT_PROPERTY_OBJECT)
				&& (((ScriptObject *)(_stk->value))->script_type_id>=SCRIPT_TYPE_ID_OBJECT_SCRIPT_OBJECT)
			){
				serialize_object(_script_engine,(ScriptObject *)_stk->value,serialized_stk,(ObjectScriptObject *)(_stk->value),0,_is_formatted,_strict_json_format);
			}else{
				serialize_stk(_script_engine,NULL,serialized_stk,_stk,0,_is_formatted,_strict_json_format);
			}

			return serialized_stk;
		}


	}


}
