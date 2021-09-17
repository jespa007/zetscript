
namespace zetscript{

#define ZS_JSON_SERIALIZE_NEW_LINE(default_str_value,ident) \
	default_str_value += "\n";\
	for (int k = 0; k < (ident); k++)	default_str_value += "\t";

	namespace json{

		void serialize_stk(ZetScript *zs,ScriptObject *this_object,zs_string & str_result, StackElement *stk, int ident,bool is_formatted);


		void serialize_vector(ZetScript *zs,ScriptObject *this_object,zs_string & str_result, ScriptObjectVector * vector,int ident, bool is_formatted){

			str_result+="[";

			for (unsigned i = 0; i < vector->length(); i++) {
				if (i > 0) {
					str_result += ",";
				}

				serialize_stk(zs,this_object,str_result,vector->getUserElementAt(i),ident,is_formatted);
			}
			str_result+="]";
		}

		void serialize_object(ZetScript *zs,ScriptObject *this_object, zs_string & str_result, ScriptObjectObject *obj, int ident, bool is_formatted){

			str_result += "{";

			zs_vector<zs_map_iterator> map_iterators={
					obj->begin_builtin()
					,obj->begin()
			};

			for(auto mi=map_iterators.begin();mi!=map_iterators.end();mi++){
				int k=0;
				for(;!mi->end();mi->next()){

					StackElement *stk_se=(StackElement *)mi->getValue();
					// only check if is not function. If is an attribute an implements get, call
					if((stk_se->properties & STK_PROPERTY_FUNCTION) == 0){
						bool created_object=false;
						bool getter_found=false;
						StackElement *ptr_stk_param;
						StackElement stk_getter_result=k_stk_undefined;


						if (is_formatted){
							str_result += "\n";
							for (int i = 0; i <= (ident); i++){
								str_result += "\t";
							}
						}

						if (k>0){
							str_result += ",";
						}

						str_result += "\"" + zs_string(mi->getKey())+ "\":";

						// if attribute we have to call script or native...
						if(stk_se->properties & STK_PROPERTY_MEMBER_ATTRIBUTE){

							StackMemberAttribute *sma=(StackMemberAttribute *)stk_se->value;
							ScriptFunction *ptr_function=sma->member_attribute->getter;
							if(ptr_function!=NULL && obj->idx_script_class>IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS){ // getter found

								getter_found=true;

								if((ptr_function->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF) == 0){

									stk_getter_result=VM_EXECUTE(
										zs->getVirtualMachine()
										,obj
										,ptr_function
										,NULL
										,0
									);


								}else{ // expect return an scriptobjectstring
									void *c_object = ((ScriptObjectClass *)obj)->getNativeObject();
									zs_int result=((zs_int (*)(void *))(ptr_function->ref_native_function_ptr))(c_object);
									stk_getter_result=zs->convertVarToStackElement(result,ptr_function->idx_return_type);

								}

								ptr_stk_param=&stk_getter_result;
							}
						}

						if(getter_found == false){
							ptr_stk_param=(StackElement *)mi->getValue();
						}

						serialize_stk(zs,this_object, str_result, ptr_stk_param, ident+1,is_formatted);

						if(stk_getter_result.properties & STK_PROPERTY_SCRIPT_OBJECT){
							vm_unref_lifetime_object(zs->getVirtualMachine(),(ScriptObject *)stk_getter_result.value);

						}
						k++;
					}
				}
			}

			if(is_formatted){
				str_result += "\n";
				for (int i = 0; i < (ident); i++){
					str_result += "\t";
				}
			}
			str_result += "}";
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
				str_result+=stk->toString();
				break;
			case STK_PROPERTY_NULL:
				str_result+="null";
				break;
			case STK_PROPERTY_SCRIPT_OBJECT: // vector or object

				obj=((ScriptObject *)stk->value);
				switch(obj->idx_script_class){
				case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING:
					str_result+=zs_string("\"") + ((ScriptObjectString *)obj)->toString() + "\"";
					break;
				case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR:
					serialize_vector(zs, this_object, str_result,(ScriptObjectVector *)obj,ident,is_formatted);
					break;
				default:
					if(
						obj->idx_script_class>=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT
					){
						if(this_object != obj){ // avoid recursivity
							serialize_object(zs,this_object,str_result,(ScriptObjectObject *)obj,ident,is_formatted);
						}
						else{
							str_result+="\"Object@"+this_object->getClassName()+"\"";
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
				&& (((ScriptObject *)(stk->value))->idx_script_class>=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT)
			){
				serialize_object(zs,(ScriptObject *)stk->value,serialized_stk,(ScriptObjectObject *)(stk->value),0,is_formatted);
			}else{
				serialize_stk(zs,NULL,serialized_stk,stk,0,is_formatted);
			}

			return serialized_stk;
		}


	}


}
