namespace zetscript{

	namespace json{

		template<typename _T>
		void serialize_json_var_map(std::string & str_result, _T * json_var_map, int ident, bool minimized) {

			str_result += "{";

			if (minimized==false){
				ZJ_FORMAT_OUTPUT_NEW_LINE(str_result,ident+1);
			}

			int j=0;
			for (auto it=json_var_map->begin();it !=json_var_map->end();it++,j++) {

				if (j > 0){
					if (minimized==false){
						ZJ_FORMAT_OUTPUT_NEW_LINE(str_result,ident+1);
					}
					str_result += ",";
				}

				str_result += "\""+it->first +"\":";
				serialize_json_var(str_result,json_var_map->getJsonVarPtr(it->first),ident+1,minimized);

			}

			if (minimized==false){
				ZJ_FORMAT_OUTPUT_NEW_LINE(str_result,ident);
			}
			str_result += "}";
		}

		template<typename _T>
		void serialize_json_var_vector(std::string & str_result, _T * json_var_vector,int ident, bool minimized){

			str_result+="[";

			if (minimized==false && json_var_vector->getType() !=JsonVarType::JSON_VAR_TYPE_VECTOR_OF_OBJECTS){
				ZJ_FORMAT_OUTPUT_NEW_LINE(str_result,ident+1);
			}
			for (unsigned i = 0; i < json_var_vector->size(); i++) {
				if (i > 0) {
					str_result += ",";
				}

				serialize_json_var(str_result,json_var_vector->getJsonVarPtr(i),ident,minimized);
			}
			if (minimized==false && json_var_vector->getType() !=JsonVarType::JSON_VAR_TYPE_VECTOR_OF_OBJECTS){
				ZJ_FORMAT_OUTPUT_NEW_LINE(str_result,ident);
			}
			str_result+="]";
		}

		void serialize_json_var_object(std::string & str_result, JsonVar *json_var, int ident, bool minimized){
			int k=0;

			if(json_var->getType() != JsonVarType::JSON_VAR_TYPE_OBJECT){
				throw std::runtime_error(zj_strutils::format("Expected json object but it was %s",json_var->getTypeStr()));
			}

			str_result += "{";

			if (minimized == false){
				str_result += "\n";
			}

			char *aux_p = (char *)json_var->getPtrDataStart();
			char *end_p = (char *)json_var->getPtrDataEnd();

			for (; aux_p < end_p; k++) {
				JsonVar * p_sv = (JsonVar *)aux_p;

				if (p_sv != NULL) {

					if (minimized==false){
						for (int i = 0; i <= (ident); i++){
							str_result += "\t";
						}
					}

					str_result += "\"" + p_sv->getVariableName()+ "\":";

					switch (p_sv->getType())// == )
					{
					case JsonVarType::JSON_VAR_TYPE_BOOLEAN:
					case JsonVarType::JSON_VAR_TYPE_NUMBER:
					case JsonVarType::JSON_VAR_TYPE_STRING:
						serialize_json_var(str_result,p_sv,0,minimized);
						break;

					case JsonVarType::JSON_VAR_TYPE_OBJECT:
					case JsonVarType::JSON_VAR_TYPE_VECTOR_OF_BOOLEANS:
					case JsonVarType::JSON_VAR_TYPE_VECTOR_OF_STRINGS:
					case JsonVarType::JSON_VAR_TYPE_VECTOR_OF_NUMBERS:
					case JsonVarType::JSON_VAR_TYPE_VECTOR_OF_OBJECTS:
					case JsonVarType::JSON_VAR_TYPE_MAP_OF_BOOLEANS:
					case JsonVarType::JSON_VAR_TYPE_MAP_OF_STRINGS:
					case JsonVarType::JSON_VAR_TYPE_MAP_OF_NUMBERS:
					case JsonVarType::JSON_VAR_TYPE_MAP_OF_OBJECTS:
						if (minimized==false){
							str_result += "\n";
							for (int i = 0; i <= ident; i++){
								str_result += "\t";
							}
						}
						serialize_json_var(str_result,p_sv,ident+1, minimized);
						break;
					}
				}
				aux_p += p_sv->getSizeData();

				if (aux_p < end_p){
					str_result += ",";
				}

				if (minimized == false){
					str_result += "\n";
				}

			}

			if (minimized == false){
				for (int i = 0; i < ident; i++){
					str_result += "\t";
				}
			}

			str_result += "}";

			if (minimized == false){
				"\n";
			}
		}

		void serialize_json_var(std::string & str_result, JsonVar *json_var, int ident,bool minimized){
			switch(json_var->getType()){
			default:
				break;
			case JSON_VAR_TYPE_BOOLEAN:
				str_result+=(*((JsonVarBoolean<> *)json_var)==true)?"true":"false";
				break;
			case JSON_VAR_TYPE_NUMBER:
				str_result+=zj_strutils::float_to_str(*((JsonVarNumber<> *)json_var));
				break;
			case JSON_VAR_TYPE_STRING:
				str_result+=std::string("\"") + *((JsonVarString<> *)json_var) + "\"";
				break;
			case JSON_VAR_TYPE_OBJECT:
				serialize_json_var_object(str_result, json_var,ident,minimized);
				break;
			case JSON_VAR_TYPE_VECTOR_OF_BOOLEANS:
				serialize_json_var_vector<JsonVarVectorBoolean<>>(str_result, (JsonVarVectorBoolean<> *)json_var,ident,minimized);
				break;
			case JSON_VAR_TYPE_VECTOR_OF_NUMBERS:
				serialize_json_var_vector<JsonVarVectorNumber<>>(str_result,(JsonVarVectorNumber<> *)json_var,ident,minimized);
				break;
			case JSON_VAR_TYPE_VECTOR_OF_STRINGS:
				serialize_json_var_vector<JsonVarVectorString<>>(str_result,(JsonVarVectorString<> *)(json_var),ident,minimized);
				break;
			case JSON_VAR_TYPE_VECTOR_OF_OBJECTS:
				serialize_json_var_vector<JsonVarVectorObject<TestVoid>>(str_result,(JsonVarVectorObject<TestVoid> *)json_var,ident,minimized);
				break;
			case JSON_VAR_TYPE_MAP_OF_BOOLEANS:
				serialize_json_var_map<JsonVarMapBoolean<>>(str_result, (JsonVarMapBoolean<> *)json_var,ident,minimized);
				break;
			case JSON_VAR_TYPE_MAP_OF_NUMBERS:
				serialize_json_var_map<JsonVarMapNumber<>>(str_result, (JsonVarMapNumber<> *)json_var,ident,minimized);
				break;
			case JSON_VAR_TYPE_MAP_OF_STRINGS:
				serialize_json_var_map<JsonVarMapString<>>(str_result, (JsonVarMapString<> *)json_var,ident,minimized);
				break;
			case JSON_VAR_TYPE_MAP_OF_OBJECTS:
				serialize_json_var_map<JsonVarMapObject<TestVoid>>(str_result, (JsonVarMapObject<TestVoid> *)json_var,ident,minimized);
				break;
			}
		}

		std::string serialize(JsonVar *json_var, bool minimized){
			std::string serialized_var="";

			serialize_json_var(serialized_var,json_var,0,minimized);

			return serialized_var;
		}


	}


}
