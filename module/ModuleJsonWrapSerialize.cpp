
namespace zetscript{

#define ZS_JSON_SERIALIZE_NEW_LINE(default_str_value,ident) \
	default_str_value += "\n";\
	for (int k = 0; k < (ident); k++)	default_str_value += "\t";

	namespace json{

		void serialize_stk(std::string & str_result, StackElement *stk, int ident,bool is_formatted);


		void serialize_vector(std::string & str_result, ScriptObjectVector * vector,int ident, bool is_formatted){

			str_result+="[";

			if (is_formatted){
				ZS_JSON_SERIALIZE_NEW_LINE(str_result,ident+1);
			}

			for (unsigned i = 0; i < vector->length(); i++) {
				if (i > 0) {
					str_result += ",";
				}

				serialize_stk(str_result,vector->getUserElementAt(i),ident,is_formatted);
			}
			if (is_formatted){
				ZS_JSON_SERIALIZE_NEW_LINE(str_result,ident);
			}
			str_result+="]";
		}

		void serialize_object(std::string & str_result, ScriptObjectObject *obj, int ident, bool is_formatted){
			int k=0;

			str_result += "{";

			if (is_formatted){
				str_result += "\n";
			}

			for(auto it=obj->begin();!it.end();it.next(),k++){
				if (is_formatted){
					for (int i = 0; i <= (ident); i++){
						str_result += "\t";
					}
				}

				if (k>0){
					str_result += ",";
				}


				str_result += "\"" + std::string(it.getKey())+ "\":";

				serialize_stk(str_result, (StackElement *)it.getValue(), ident,is_formatted);


				if (is_formatted){
					str_result += "\n";
				}

			}

			if (is_formatted){
				for (int i = 0; i < ident; i++){
					str_result += "\t";
				}
			}

			str_result += "}";

			if (is_formatted){
				"\n";
			}
		}

		void serialize_stk(std::string & str_result, StackElement *stk, int ident,bool is_formatted){
			ScriptObject *obj=NULL;
			int16_t var_type = 0;

			if(STK_IS_SCRIPT_OBJECT_VAR_REF(stk)){
				stk=((ScriptObjectVarRef *)stk->value)->getStackElementPtr();
			}

			var_type = GET_MSK_STK_PROPERTY_TYPES(stk->properties);


			switch(var_type){
			default:
				break;
			case MSK_STK_PROPERTY_ZS_FLOAT:
			case MSK_STK_PROPERTY_BOOL:
			case MSK_STK_PROPERTY_ZS_INT:
				str_result+=stk->toString();
				break;
			case MSK_STK_PROPERTY_NULL:
				str_result+="null";
				break;
			case MSK_STK_PROPERTY_SCRIPT_OBJECT: // vector or object

				if (is_formatted){
					str_result += "\n";
					for (int i = 0; i <= ident; i++){
						str_result += "\t";
					}
				}

				obj=((ScriptObject *)stk->value);
				switch(obj->idx_script_class){
				case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING:
					str_result+=std::string("\"") + ((ScriptObjectString *)obj)->toString() + "\"";
					break;
				case IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR:
					serialize_vector(str_result,(ScriptObjectVector *)obj,ident,is_formatted);
					break;
				default:
					if(obj->idx_script_class>=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT){
						serialize_object(str_result,(ScriptObjectObject *)obj,ident,is_formatted);
					}
					break;
				}
				break;
			}
		}

		std::string serialize(StackElement *stk, bool is_formatted){
			std::string serialized_stk="";

			serialize_stk(serialized_stk,stk,0,is_formatted);

			return serialized_stk;
		}


	}


}
