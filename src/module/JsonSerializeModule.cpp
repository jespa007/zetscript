/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

#define ZS_JSON_SERIALIZE_NEW_LINE(default_str_value,ident) \
	default_str_value.append('\n');\
	for (int k = 0; k < (ident); k++)	default_str_value.append('\t');

	namespace json{

		void serialize_stk(ZetScript *zs,ScriptObject *this_object,zs_string & str_result, StackElement *stk, int ident,bool is_formatted);


		void serialize_vector(ZetScript *zs,ScriptObject *this_object,zs_string & str_result, ScriptObjectVector * vector,int ident, bool is_formatted){

			str_result.append('[');

			for (unsigned i = 0; i < vector->length(); i++) {
				if (i > 0) {
					str_result.append(',');
				}

				serialize_stk(zs,this_object,str_result,vector->getUserElementAt(i),ident,is_formatted);
			}
			str_result.append(']');
		}

		void serialize_object(
			ZetScript *_zs
			,ScriptObject *_this_object
			, zs_string & _str_result
			, ScriptObjectObject *_obj
			, int _ident
			, bool _is_formatted){

			_str_result.append('{');

			zs_map_iterator map_iterators[2]={
					_obj->getMapUserProperties()->begin()
					,_obj->getMapBuiltinProperties()->begin()
			};

			for(int i=0; i < 2; i++){//zs_map_iterator *it=map_iterators;mi!=map_iterators.end();mi++){
				int k=0;
				zs_map_iterator *mi=&map_iterators[i];
				for(;!mi->end();mi->next()){

					StackElement *stk_se=(StackElement *)mi->value;
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
							_str_result.append('\n');
							for (int j = 0; j <= (_ident); j++){
								_str_result.append('\t');
							}
						}

						if (k>0){
							_str_result.append(',');
						}

						_str_result.append("\"" + zs_string(mi->key)+ "\":");

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
									void *c_object = ((ScriptObjectClass *)_obj)->getNativeObject();
									zs_int result=((zs_int (*)(ZetScript *,void *))(ptr_function->ref_native_function_ptr))(_zs,c_object);
									stk_getter_result=_zs->convertVarToStackElement(
										result
										,ptr_function->idx_script_type_return
									);

								}

								ptr_stk_param=&stk_getter_result;
							}
						}

						if(getter_found == false){
							ptr_stk_param=(StackElement *)(mi->value);
						}

						serialize_stk(
							_zs
							,_this_object
							, _str_result
							, ptr_stk_param
							, _ident+1
							,_is_formatted);

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
				_str_result.append('\n');
				for (int i = 0; i < (_ident); i++){
					_str_result.append('\t');
				}
			}
			_str_result.append('}');
		}

		void serialize_stk(ZetScript *zs, ScriptObject *this_object,zs_string & str_result, StackElement *stk, int ident,bool is_formatted){
			ScriptObject *obj=NULL;
			int16_t var_type = 0;

			if(stk->properties & STK_PROPERTY_PTR_STK){
				stk=(StackElement *)stk->value;
			}

			if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk)){
				stk=((ScriptObjectVarRef *)stk->value)->getStackElementPtr();
			}


			var_type = GET_STK_PROPERTY_TYPES(stk->properties);

			switch(var_type){
			default:
				break;
			case STK_PROPERTY_ZS_FLOAT:
			case STK_PROPERTY_BOOL:
			case STK_PROPERTY_ZS_INT:
				str_result.append(stk_to_str(zs,stk));
				break;
			case STK_PROPERTY_UNDEFINED:
				str_result.append("undefined");
				break;
			case STK_PROPERTY_NULL:
				str_result.append("null");
				break;
			case STK_PROPERTY_SCRIPT_OBJECT: // vector or object

				obj=((ScriptObject *)stk->value);
				switch(obj->idx_script_type){
				case IDX_TYPE_SCRIPT_OBJECT_STRING:
					str_result.append(zs_string("\"") + ((ScriptObjectString *)obj)->toString() + "\"");
					break;
				case IDX_TYPE_SCRIPT_OBJECT_VECTOR:
					serialize_vector(zs, this_object, str_result,(ScriptObjectVector *)obj,ident,is_formatted);
					break;
				default:
					if(
						obj->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_OBJECT
					){
						if(this_object != obj){ // avoid recursivity
							serialize_object(zs,this_object,str_result,(ScriptObjectObject *)obj,ident,is_formatted);
						}
						else{
							str_result.append("\"Object@"+this_object->getTypeName()+"\"");
						}
					}
					break;
				}
				break;
			}
		}

		zs_string serialize(ZetScript *zs, StackElement *stk, bool is_formatted){
			zs_string serialized_stk="";

			if(
					(stk->properties & STK_PROPERTY_SCRIPT_OBJECT)
				&& (((ScriptObject *)(stk->value))->idx_script_type>=IDX_TYPE_SCRIPT_OBJECT_OBJECT)
			){
				serialize_object(zs,(ScriptObject *)stk->value,serialized_stk,(ScriptObjectObject *)(stk->value),0,is_formatted);
			}else{
				serialize_stk(zs,NULL,serialized_stk,stk,0,is_formatted);
			}

			return serialized_stk;
		}


	}


}
