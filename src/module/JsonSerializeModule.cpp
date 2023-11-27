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
				ZetScript *_zs
				,Object *_this_object
				,String & _str_result
				, StackElement *_stk
				, int _ident
				,bool _is_formatted
				, bool _strict_json_format
		);


		void serialize_array(
				ZetScript *_zs
				,Object *_this_object
				,String & _str_result
				, ArrayObject * _array
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
					_zs
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
			ZetScript *_zs
			,Object *_this_object
			, String & _str_result
			, DictionaryObject *_obj
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

							Function *ptr_function=(Function *)(smp->member_property->metamethod_members.getter->ref_ptr);

							if(ptr_function!=NULL && _obj->type_id > TYPE_ID_OBJECT_CLASS){ // getter found

								getter_found=true;

								if((ptr_function->properties & FUNCTION_PROPERTY_C_OBJECT_REF) == 0){

									value_from_vm_execute=true;

									stk_getter_result=ZS_VM_EXECUTE(
										_zs->getVirtualMachine()
										,_obj
										,ptr_function
										,NULL
										,0
									);


								}else{ // expect return an scriptobjectstring

									// Change undefined type as null valid in json scope
									zs_int result= 0;
									String str_aux="";
									void *c_object = ((ClassObject *)_obj)->getNativeObject();

									switch(ptr_function->return_type_id){
									case  TYPE_ID_ZS_STRING_C:
										str_aux=((String (*)(ZetScript *,void *))(ptr_function->ref_native_function_ptr))(_zs,c_object);
										result = (zs_int)&str_aux;
										break;
									case  TYPE_ID_FLOAT_C:
										ZS_WRITE_INTPTR_FLOAT(&result,((zs_float (*)(ZetScript *,void *))(ptr_function->ref_native_function_ptr))(_zs,c_object));
										break;
									default:
										result=((zs_int (*)(ZetScript *,void *))(ptr_function->ref_native_function_ptr))(_zs,c_object);
										break;
									}
									stk_getter_result=_zs->toStackElement(
										result
										,ptr_function->return_type_id
									);
								}

								ptr_stk_param=stk_getter_result;
							}
						}

						if(getter_found == false){
							ptr_stk_param=stk_se;
						}

						serialize_stk(
							_zs
							,_this_object
							, _str_result
							, &ptr_stk_param
							, _ident+1
							,_is_formatted
							,_strict_json_format);

						if(stk_getter_result.properties & STACK_ELEMENT_PROPERTY_OBJECT){
							if(value_from_vm_execute==true){
								vm_unref_lifetime_object(
									_zs->getVirtualMachine()
									,(Object *)stk_getter_result.value
								);
							}else{
								delete ((Object *)stk_getter_result.value);
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
			ZetScript *_zs
			, Object *_this_object
			,String & _str_result
			, StackElement *_stk
			, int _ident
			,bool _is_formatted
			,bool _strict_json_format
		){

			Object *obj=NULL;
			int16_t var_type = 0;
			StackElement stk=*_stk;

			stk=_zs->unwrapStackElement(stk);

			var_type = STACK_ELEMENT_TYPE_PROPERTIES(stk.properties);

			switch(var_type){
			default:
				break;
			case STACK_ELEMENT_PROPERTY_FLOAT:
			case STACK_ELEMENT_PROPERTY_BOOL:
			case STACK_ELEMENT_PROPERTY_INT:
				_str_result.append(_zs->stackElementToString(&stk));
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

				obj=((Object *)stk.value);
				switch(obj->type_id){
				case TYPE_ID_OBJECT_STRING:
					_str_result.append(String("\"") + ((StringObject *)obj)->toString() + "\"");
					break;
				case TYPE_ID_OBJECT_ARRAY:
					serialize_array(_zs, _this_object, _str_result,(ArrayObject *)obj,_ident,_is_formatted,_strict_json_format);
					break;
				default:
					if(
						obj->type_id>=TYPE_ID_DICTIONARY_OBJECT
					){
						if(_this_object != obj){ // avoid recursivity
							serialize_object(_zs,_this_object,_str_result,(DictionaryObject *)obj,_ident,_is_formatted,_strict_json_format);
						}
						else{
							_str_result.append("\"Object@"+String(_this_object->getTypeName())+"\"");
						}
					}
					break;
				}
				break;
			}
		}

		String serialize(ZetScript *_zs, StackElement *_stk, bool _is_formatted, bool _strict_json_format){
			String serialized_stk="";

			if(
					(_stk->properties & STACK_ELEMENT_PROPERTY_OBJECT)
				&& (((Object *)(_stk->value))->type_id>=TYPE_ID_DICTIONARY_OBJECT)
			){
				serialize_object(_zs,(Object *)_stk->value,serialized_stk,(DictionaryObject *)(_stk->value),0,_is_formatted,_strict_json_format);
			}else{
				serialize_stk(_zs,NULL,serialized_stk,_stk,0,_is_formatted,_strict_json_format);
			}

			return serialized_stk;
		}


	}


}
