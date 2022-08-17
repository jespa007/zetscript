/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

#define ZS_JSON_SERIALIZE_NEW_LINE(default_str_value,ident) \
	default_str_value.append('\n');\
	for (int k = 0; k < (ident); k++)	default_str_value.append('\t');

	namespace json{

		void serialize_stk(ZetScript *_zs,ScriptObject *_this_object,std::string & _str_result, StackElement *_stk, int _ident,bool _is_formatted, bool _strict_json_format);


		void serialize_vector(
				ZetScript *_zs
				,ScriptObject *_this_object
				,std::string & _str_result
				, ScriptObjectVector * _vector
				,int _ident
				, bool _is_formatted
				,bool _strict_json_format
				){

			_str_result.append("[");

			for (int i = 0; i < _vector->length(); i++) {
				if (i > 0) {
					_str_result.append(",");
				}

				serialize_stk(_zs,_this_object,_str_result,_vector->getUserElementAt(i),_ident,_is_formatted,_strict_json_format);
			}
			_str_result.append("]");
		}

		void serialize_object(
			ZetScript *_zs
			,ScriptObject *_this_object
			, std::string & _str_result
			, ScriptObjectObject *_obj
			, int _ident
			, bool _is_formatted
			,bool _strict_json_format
			){

			_str_result.append("{");

			std::map<std::string,StackElement *> *container_properties[2]={
					_obj->getMapUserProperties()
					,_obj->getMapBuiltinProperties()
			};

			for(int i=0; i < 2; i++){//zs_map_iterator *it=map_iterators;mi!=map_iterators.end();mi++){
				int k=0;
				//auto *mi=&maps[i].begin();
				for(auto it=container_properties[i]->begin();it!=container_properties[i]->end();it++){

					StackElement *stk_se=(StackElement *)it->second;
					StackMemberProperty *smp=NULL;

					if(stk_se->properties & STK_PROPERTY_MEMBER_PROPERTY){
						smp=(StackMemberProperty *)stk_se->value;
						if(smp->member_property->metamethod_members.getter == NULL){ // ignore value due it doesn't has getter
							continue;
						}
					}

					// only check if is not function. If is an property an implements get, call
					if(
						((stk_se->properties & STK_PROPERTY_FUNCTION) == 0)
							&&
						STK_IS_SCRIPT_OBJECT_MEMBER_FUNCTION(stk_se) == false
					){
						bool getter_found=false;
						bool value_from_vm_execute=false;
						StackElement *ptr_stk_param=NULL;
						StackElement stk_getter_result=k_stk_undefined;

						if (_is_formatted){
							_str_result.append("\n");
							for (int j = 0; j <= (_ident); j++){
								_str_result.append("\t");
							}
						}

						if (k>0){
							_str_result.append(",");
						}

						_str_result.append("\"" + std::string(it->first)+ "\":");

						// if property we have to call script or native...
						if(smp!=NULL){

							ScriptFunction *ptr_function=(ScriptFunction *)(smp->member_property->metamethod_members.getter->ref_ptr);

							if(ptr_function!=NULL && _obj->idx_script_type>IDX_TYPE_SCRIPT_OBJECT_CLASS){ // getter found

								getter_found=true;

								if((ptr_function->properties & FUNCTION_PROPERTY_C_OBJECT_REF) == 0){

									value_from_vm_execute=true;

									stk_getter_result=VM_EXECUTE(
										_zs->getVirtualMachine()
										,_obj
										,ptr_function
										,NULL
										,0
									);


								}else{ // expect return an scriptobjectstring

									// Change undefined type as null valid in json scope
									zs_int result= 0;
									std::string str_aux="";
									void *c_object = ((ScriptObjectClass *)_obj)->getNativeObject();

									switch(ptr_function->idx_script_type_return){
									case  IDX_TYPE_STRING_C:
										str_aux=((std::string (*)(ZetScript *,void *))(ptr_function->ref_native_function_ptr))(_zs,c_object);
										result = (zs_int)&str_aux;
										break;
									case  IDX_TYPE_ZS_FLOAT_C:
										*((zs_float *)&result)=((zs_float (*)(ZetScript *,void *))(ptr_function->ref_native_function_ptr))(_zs,c_object);
										break;
									default:
										result=((zs_int (*)(ZetScript *,void *))(ptr_function->ref_native_function_ptr))(_zs,c_object);
										break;
									}
									stk_getter_result=to_stk(
										_zs
										,result
										,ptr_function->idx_script_type_return
									);
								}

								ptr_stk_param=&stk_getter_result;
							}
						}

						if(getter_found == false){
							ptr_stk_param=it->second;
						}

						serialize_stk(
							_zs
							,_this_object
							, _str_result
							, ptr_stk_param
							, _ident+1
							,_is_formatted
							,_strict_json_format);

						if(stk_getter_result.properties & STK_PROPERTY_SCRIPT_OBJECT){
							if(value_from_vm_execute==true){
								vm_unref_lifetime_object(
									_zs->getVirtualMachine()
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
				_str_result.append("\n");
				for (int i = 0; i < (_ident); i++){
					_str_result.append("\t");
				}
			}
			_str_result.append("}");
		}

		void serialize_stk(
				ZetScript *_zs
				, ScriptObject *_this_object
				,std::string & _str_result
				, StackElement *_stk
				, int _ident
				,bool _is_formatted
				,bool _strict_json_format){

			ScriptObject *obj=NULL;
			int16_t var_type = 0;

			if(_stk->properties & STK_PROPERTY_PTR_STK){
				_stk=(StackElement *)_stk->value;
			}

			if(STK_IS_SCRIPT_OBJECT_VAR_REF(_stk)){
				_stk=((ScriptObjectVarRef *)_stk->value)->getStackElementPtr();
			}


			var_type = GET_STK_PROPERTY_TYPES(_stk->properties);

			switch(var_type){
			default:
				break;
			case STK_PROPERTY_ZS_FLOAT:
			case STK_PROPERTY_BOOL:
			case STK_PROPERTY_ZS_INT:
				_str_result.append(stk_to_str(_zs,_stk));
				break;
			case STK_PROPERTY_NULL:
				_str_result.append("null");
				break;
			case STK_PROPERTY_UNDEFINED:
				if(_strict_json_format==true){
					_str_result.append("null");
				}else{
					_str_result.append("undefined");
				}
				break;
			case STK_PROPERTY_SCRIPT_OBJECT: // vector or object

				obj=((ScriptObject *)_stk->value);
				switch(obj->idx_script_type){
				case IDX_TYPE_SCRIPT_OBJECT_STRING:
					_str_result.append(std::string("\"") + ((ScriptObjectString *)obj)->toString() + "\"");
					break;
				case IDX_TYPE_SCRIPT_OBJECT_VECTOR:
					serialize_vector(_zs, _this_object, _str_result,(ScriptObjectVector *)obj,_ident,_is_formatted,_strict_json_format);
					break;
				default:
					if(
						obj->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_OBJECT
					){
						if(_this_object != obj){ // avoid recursivity
							serialize_object(_zs,_this_object,_str_result,(ScriptObjectObject *)obj,_ident,_is_formatted,_strict_json_format);
						}
						else{
							_str_result.append("\"Object@"+std::string(_this_object->getTypeName())+"\"");
						}
					}
					break;
				}
				break;
			}
		}

		std::string serialize(ZetScript *_zs, StackElement *_stk, bool _is_formatted, bool _strict_json_format){
			std::string serialized_stk="";

			if(
					(_stk->properties & STK_PROPERTY_SCRIPT_OBJECT)
				&& (((ScriptObject *)(_stk->value))->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_OBJECT)
			){
				serialize_object(_zs,(ScriptObject *)_stk->value,serialized_stk,(ScriptObjectObject *)(_stk->value),0,_is_formatted,_strict_json_format);
			}else{
				serialize_stk(_zs,NULL,serialized_stk,_stk,0,_is_formatted,_strict_json_format);
			}

			return serialized_stk;
		}


	}


}
