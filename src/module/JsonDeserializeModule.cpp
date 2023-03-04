/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	namespace json{

		typedef struct{
			const char *filename;
			const char *str_start;
			zs_string str_error;
			StackElement *first_element;
			ZetScript *zs;
		}JsonDeserializeData;


		const char json_deserialize_end_char_values[] = {
				',',
				'}',
				' ',
				'\t',
				'\n',
				'\r', // for make compatible windows...
				']',
				0
		};

		char *deserialize(JsonDeserializeData *data, const char * str_start, int & line,StackElement *stk_json_element);

		bool is_single_comment(char *str){

			if((*str!=0) && *str=='/'){
				return *(str+1)=='/';
			}
			return false;
		}

		bool is_start_comment(char *str){

			if((*str!=0) && *str=='/'){
				return *(str+1)=='*';
			}
			return false;
		}

		bool is_end_comment(char *str){

			if((*str!=0) && *str=='*'){
				return *(str+1)=='/';
			}
			return false;
		}

		char *advance_to_char(char *str,char c) {
			char *aux_p = str;
			// make compatible windows format (\r)...
			while(*aux_p!='\n' && *aux_p!='\r' && *aux_p!=0 && (*aux_p !=(c) )) {
				aux_p++;
			}

			if (*aux_p == '\r') {
				aux_p++;
			}

			return aux_p;
		}

		char *advance_to_end_comment(char *_aux_p, int &_line){

			if(is_start_comment(_aux_p)){
				_aux_p+=2; //advance first
				while(!is_end_comment(_aux_p) && *_aux_p != 0){
					_aux_p = advance_to_char(_aux_p,'*');
					if (*_aux_p == '\n') {
						_aux_p++; // make compatible windows format...
						_line++; // next line
					}
					if(*_aux_p == '\r') _aux_p++;
					if(*(_aux_p+1) != '/') _aux_p++; // not end comment ... advance ...
				}
			}

			return _aux_p;
		}

		char *eval_ignore_blanks(char *str, int &line) {
			char *aux_p = str;
			bool end = false;
			while(!end){
				end = true;
				while(*aux_p!=0 && ((*aux_p==' ')  || (*aux_p=='\t'))) aux_p++;

				if(is_single_comment(aux_p)) {// ignore line
					aux_p = advance_to_char(aux_p,'\n');
				}
				else if(is_start_comment(aux_p)){
					// ignore until get the end of the comment...
					aux_p = advance_to_end_comment(aux_p, line);

					if(is_end_comment(aux_p)){
						aux_p+=2;
					}
					end=false;
				}
				// make compatible windows format...
				if(*aux_p == '\r')
					aux_p++;

				if(*aux_p == '\n') {
					line=line+1;
					end=false;
					aux_p++;
				}
			}
			return aux_p;
		}

		char *advance_to_one_of_collection_of_char(char *_str,char *_collection_of_chars, int &_line) {
			char *aux_p = _str;
			char *chk_char;
			while(*aux_p!=0){
				chk_char = _collection_of_chars;

				// comment blocks also is returned (these lines must be ignored)
				if(is_start_comment(aux_p)) {
					aux_p = advance_to_end_comment(aux_p, _line);
					if(is_end_comment(aux_p))
						aux_p+=2;
				}

				if(is_single_comment(aux_p)) {
					aux_p = advance_to_char(aux_p,'\n');
				}

				while(*chk_char != 0){
					if (*chk_char == *aux_p) {
						return aux_p;
					}
					chk_char++;
				}
				aux_p++;
			}
			return aux_p;
		}

		void json_deserialize_error(JsonDeserializeData *data,const char *str_from,int _line,const char *str,...){
			ZS_UNUSUED_PARAM(str_from);
			char  what_msg[ZS_MAX_STR_BUFFER]={0};
			char  error_description[1024] = { 0 };
			va_list  ap;
			va_start(ap,  str);
			vsprintf(error_description,  str,  ap);
			va_end(ap);
			zs_string filename="";

			if(data->filename!=NULL){
				filename=zs_path::get_filename(data->filename);
			}

			if(data->filename != NULL  && *data->filename != 0){
				sprintf(what_msg,"[file:%s line:%i] %s"
						,data->filename
						, _line
						, (char *)error_description);
			}else{
				sprintf(what_msg,"[line:%i] %s",_line,error_description);
			}

			data->str_error=what_msg;

			//ZS_THROW_EXCEPTION(what_msg);
		}

		char * read_string_between_quotes(JsonDeserializeData *data, const char *str_start,int & line, zs_string * str_out){
			char *str_current = (char *) str_start;

			if(str_out != NULL){
				*str_out="";
			}

			if (*str_current == '\"'){ // try to single quote...
				char *start=++str_current;
				while(*str_current!='\n' && *str_current!='\r' && *str_current!=0 &&  (*str_current=='\"' && *(str_current-1)!='\\')==false){
					str_current++;
				}

				if(str_out != NULL){
					str_out->append(start,str_current-start);
				}
			}else{
				json_deserialize_error(data,str_start,line,"expected string value");
				return NULL;
			}

			if(*str_current != '\"'){
				json_deserialize_error(data,str_start,line,"string value not closed");
				return NULL;
			}

			return eval_ignore_blanks(str_current+1, line);
		}

		char * deserialize_value(
			JsonDeserializeData *data
			,const char *str_start
			, int & line
			, StackElement *stk_json_element
		){
			// ptr_data: can be a bool/string or number in function of type value
			// type_value: defines the value to parse
			// key in case
			char *str_current = (char *)str_start;
			int bytes_readed=0;
			char *str_end=NULL;

			if (*str_current == '\"') {// try string ...
				zs_string str_aux;
				if((str_current=read_string_between_quotes(data,str_current,line,&str_aux))==NULL){
					return NULL;
				}
				ScriptObject *so=StringScriptObject::newStringScriptObject(data->zs, str_aux.c_str());
				if(vm_create_shared_script_object(data->zs->getVirtualMachine(),so)==false){
					json_deserialize_error(data,str_start,line,"cannot create shared pointer for string object");
					return NULL;
				}

				//
				if(stk_json_element != data->first_element){
					if(vm_share_script_object(data->zs->getVirtualMachine(),so) == false){
						json_deserialize_error(data,str_start,line,"cannot share pointer for string object");
						return NULL;
					}
				}
				stk_json_element->value=(intptr_t)so;
				stk_json_element->properties=STK_PROPERTY_SCRIPT_OBJECT;

			}
			else if (strncmp(str_current, "true", 4)==0) { // true detected ...
				stk_json_element->properties=STK_PROPERTY_BOOL;
				stk_json_element->value=true;
				str_current+=4;
			}
			else if (strncmp(str_current, "false", 5) == 0) {// boolean detected
				stk_json_element->properties=STK_PROPERTY_BOOL;
				stk_json_element->value=false;
				str_current+=5;
			}
			else{ // must a number
				// try read until next comma
				char default_str_value[1024]={0};

				str_end = advance_to_one_of_collection_of_char(str_current, (char *)json_deserialize_end_char_values, line);
				bytes_readed = str_end - str_current;
				if (*str_end != 0) {
					str_end--;
				}

				if (bytes_readed > 0) {
					// copy string...
					strncpy(default_str_value,str_current,bytes_readed);
					str_current+=bytes_readed;

					zs_float *number_value = 0;
					if((number_value=zs_strutils::parse_zs_float(default_str_value)) != NULL){
						ZS_FLOAT_COPY(&stk_json_element->value,number_value);
						stk_json_element->properties=STK_PROPERTY_ZS_FLOAT;
						delete number_value;
					}
				}else{
					json_deserialize_error(data,str_start,line,"Cannot deduce json value. Json value can be Number,Boolean,String, Array or Object");
					return NULL;
				}
			}
			return str_current;
		}

		char * deserialize_vector(
				JsonDeserializeData *data
				,const char *str_start
				, int & line
				,StackElement *stk_json_element
				){
			char *str_current = (char *)str_start;
			zs_string error;
			ArrayScriptObject *vo;
			StackElement *stk_element=NULL;


			str_current = eval_ignore_blanks(str_current, line);

			if(*str_current != '['){
				json_deserialize_error(data,str_start,line,"A '[' was expected to parse JsonVarArray type");
				return NULL;
			}

			// ok, we create object
			if(stk_json_element != NULL && stk_json_element->properties==0){
				vo=ArrayScriptObject::newArrayScriptObject(data->zs);
				if(vm_create_shared_script_object(data->zs->getVirtualMachine(),vo) == false){
					json_deserialize_error(data, str_start, line, "Cannot create shared poiner for vector object");
					return NULL;
				}

				if(stk_json_element != data->first_element){

					if(vm_share_script_object(data->zs->getVirtualMachine(),vo) == false){
						json_deserialize_error(data,str_start,line,"cannot share pointer for vector object");
						return NULL;
					}
				}

				stk_json_element->properties=STK_PROPERTY_SCRIPT_OBJECT;
				stk_json_element->value=(intptr_t)vo;
			}else{
				json_deserialize_error(data, str_start, line, "Internal error: A null stackelement expected");
				return NULL;
			}

			str_current = eval_ignore_blanks(str_current+1, line);

			if(*str_current != ']'){ // do parsing primitive...

				do{
					// add new element,
					stk_element=vo->newSlot();

					if((str_current=deserialize(data,str_current,line,stk_element))==NULL){
						return NULL;
					}

					str_current = eval_ignore_blanks(str_current, line);

					if(*str_current==','){
						str_current = eval_ignore_blanks(str_current+1, line);
					}else if(*str_current!=']'){
						json_deserialize_error(data, str_current, line,  "Expected ',' or ']'");
						return NULL;
					}

				}while(*str_current != ']');
			}

			return str_current+1;
		}

		char * deserialize_object(JsonDeserializeData *data, const char * str_start, int & line,StackElement *stk_json_element) {
			char *str_current = (char *)str_start;
			zs_string variable_name,key_id;
			zs_string error;
			ObjectScriptObject *so;
			StackElement *stk_element=NULL;

			str_current = eval_ignore_blanks(str_current, line);

			if(*str_current != '{'){
				json_deserialize_error(data
						, str_start
						, line
						, "A '{' was expected to parse %s type"
						,stk_json_element!=NULL?stk_to_typeof_str(data->zs,stk_json_element).c_str():"");
				return NULL;
			}

			// ok, we create object
			if(stk_json_element != NULL && stk_json_element->properties==0){
				so=ObjectScriptObject::newScriptObjectObject(data->zs);
				if(vm_create_shared_script_object(data->zs->getVirtualMachine(),so) == false){
					json_deserialize_error(data, str_start, line, "Cannot create shared pointer for object");
					return NULL;
				}

				if(stk_json_element != data->first_element){
					if(vm_share_script_object(data->zs->getVirtualMachine(),so) == false){
						json_deserialize_error(data,str_start,line,"cannot share pointer for object");
						return NULL;
					}
				}

				stk_json_element->properties=STK_PROPERTY_SCRIPT_OBJECT;
				stk_json_element->value=(intptr_t)so;
			}else{
				json_deserialize_error(data, str_start, line, "Internal error: A null stackelement expected");
				return NULL;
			}

			str_current = eval_ignore_blanks(str_current+1, line);

			if(*str_current != '}'){ // do parsing object values...
				do{
					str_current =read_string_between_quotes(data, str_current, line, &key_id);
					if (str_current==NULL){
						return NULL;
					}


					if(*str_current != ':') {// ok check value
						json_deserialize_error(data, str_current, line, "json-deserialize: Expected ':' after \"%s\"",key_id.c_str());
						return NULL;
					}

					str_current = eval_ignore_blanks(str_current + 1, line);

					// create property... //get c property
					if((stk_element=so->set(key_id.c_str())) == NULL){
						json_deserialize_error(data, str_current, line, "Cannot set field value '%s'",key_id.c_str());
						return NULL;
					}

					if((str_current=deserialize(data, str_current, line, stk_element))==NULL){
						return NULL;
					}


					str_current = eval_ignore_blanks(str_current, line);

					if(*str_current==','){
						str_current = eval_ignore_blanks(str_current+1, line);
					}else if(*str_current!='}'){
						json_deserialize_error(data, str_current, line, "Expected ',' or '}'");
						return NULL;
					}

				}while(*str_current != '}');
			}

			return str_current+1;
		}

		char * deserialize(JsonDeserializeData *data, const char * str_start, int & line,StackElement *stk_json_element) {
			// PRE: If json_var == NULL it parses but not saves
			char * str_current = (char *)str_start;
			str_current = eval_ignore_blanks(str_current, line);


			//try to deduce ...
			if(*str_current == '['){ // try parse vector
				str_current=deserialize_vector(data, str_current, line,stk_json_element);
			}else if(*str_current == '{') {// try parse object
				str_current=deserialize_object(data, str_current, line,stk_json_element);
			}else{ // try parse value
				str_current=deserialize_value(data, str_current,line,stk_json_element);
			}

			return str_current;
		}
	}

}
