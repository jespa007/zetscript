
namespace zetscript{

#define ZS_JSON_SERIALIZE_NEW_LINE(default_str_value,ident) \
	default_str_value += "\n";\
	for (int k = 0; k < (ident); k++)	default_str_value += "\t";

	namespace json{

		void serialize_stk(ZetScript *zs,std::string & str_result, StackElement *stk, int ident,bool is_formatted);


		void serialize_vector(ZetScript *zs,std::string & str_result, ScriptObjectVector * vector,int ident, bool is_formatted){

			str_result+="[";

			for (unsigned i = 0; i < vector->length(); i++) {
				if (i > 0) {
					str_result += ",";
				}

				serialize_stk(zs,str_result,vector->getUserElementAt(i),ident,is_formatted);
			}
			str_result+="]";
		}

		void serialize_object(ZetScript *zs, std::string & str_result, ScriptObjectObject *obj, int ident, bool is_formatted){


			/*if(is_formatted){
				str_result += "\n";
				for (int i = 0; i < (ident); i++){
					str_result += "\t";
				}
			}*/
			str_result += "{";

			std::vector<zs_map_iterator> map_iterators={
					obj->begin_builtin()
					,obj->begin()
			};

			for(auto mi=map_iterators.begin();mi!=map_iterators.end();mi++){
				int k=0;
				for(;!mi->end();mi->next()){
					StackElement *stk_se=(StackElement *)mi->getValue();
					// only check if is not function. If is an attribute an implements get, call
					if((stk_se->properties & STK_PROPERTY_FUNCTION) == 0){
						if (is_formatted){
							str_result += "\n";
							for (int i = 0; i <= (ident); i++){
								str_result += "\t";
							}
						}

						if (k>0){
							str_result += ",";
						}

						str_result += "\"" + std::string(mi->getKey())+ "\":";

						serialize_stk(zs, str_result, (StackElement *)mi->getValue(), ident+1,is_formatted);
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

		void serialize_stk(ZetScript *zs, std::string & str_result, StackElement *stk, int ident,bool is_formatted){
			ScriptObject *obj=NULL;
			int16_t var_type = 0;

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

				/*if (is_formatted){
					str_result += "\n";
					for (int i = 0; i < ident; i++){
						str_result += "\t";
					}
				}*/

				obj=((ScriptObject *)stk->value);
				switch(obj->idx_script_class){
				case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING:
					str_result+=std::string("\"") + ((ScriptObjectString *)obj)->toString() + "\"";
					break;
				case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR:
					serialize_vector(zs, str_result,(ScriptObjectVector *)obj,ident,is_formatted);
					break;
				default:
					if(
						obj->idx_script_class>=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT
					){
						serialize_object(zs,str_result,(ScriptObjectObject *)obj,ident,is_formatted);
					}
					break;
				}
				break;
			}
		}

		std::string serialize(ZetScript *zs, StackElement *stk, bool is_formatted){
			std::string serialized_stk="";

			serialize_stk(zs,serialized_stk,stk,0,is_formatted);

			return serialized_stk;
		}


	}


}
