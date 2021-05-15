/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

//#include "zetscript.h"

#define json_deserialize_error

namespace zetscript{

	namespace json{

		typedef struct{
			const char *filename;
			const char *str_start;
			bool error;
			std::string str_error;
		}JsonDeserializeData;


		const char end_char_standard_value[] = {
				',',
				'}',
				' ',
				'\t',
				'\n',
				'\r', // for make compatible windows...
				']',
				0
		};

		char *deserialize_stk_json_element(JsonDeserializeData *deserialize_data, const char * str_start, int & line,StackElement *stk_json_element);

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
			while(*aux_p!='\n' && *aux_p!='\r' && *aux_p!=0 && (*aux_p !=(c) )) aux_p++;

			if(*aux_p=='\r')
				aux_p++;

			return aux_p;
		}

		char *advance_to_end_comment(char *aux_p, int &line){

			if(is_start_comment(aux_p)){
				aux_p+=2; //advance first
				while(!is_end_comment(aux_p) && *aux_p != 0){
					aux_p = advance_to_char(aux_p,'*');
					if(*aux_p == '\n') aux_p++; // make compatible windows format...
					if(*aux_p == '\r') aux_p++;
					if(*(aux_p+1) != '/') aux_p++; // not end comment ... advance ...
				}
			}

			return aux_p;
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

		char *advance_to_one_of_collection_of_char(char *str,char *end_char_standard_value, int &line) {
			char *aux_p = str;
			char *chk_char;
			while(*aux_p!=0){
				chk_char = end_char_standard_value;

				// comment blocks also is returned (these lines must be ignored)
				if(is_start_comment(aux_p)) {
					aux_p = advance_to_end_comment(aux_p, line);
					if(is_end_comment(aux_p))
						aux_p+=2;
				}

				if(is_single_comment(aux_p)) {
					aux_p = advance_to_char(aux_p,'\n');
				}

				while(*chk_char != 0){
					if(*chk_char == *aux_p)
						return aux_p;
					chk_char++;
				}
				aux_p++;
			}
			return aux_p;
		}

		char * read_string_between_quotes(JsonDeserializeData *deserialize_data, const char *str_start,int & line, std::string * str_out){
			char *str_current = (char *) str_start;
			size_t str_size;

			if(str_out != NULL){
				*str_out="";
			}

			if (*str_current == '\"'){ // try to single quote...
				str_current++;
				while(*str_current!='\n' && *str_current!='\r' && *str_current!=0 &&  (*str_current=='\"' && *(str_current-1)!='\\')==false){
					if(str_out != NULL){
						(*str_out)+=*str_current;
					}
					str_current++;
				}
			}else{
				json_deserialize_error(deserialize_data,str_start,line,"expected string value");
			}

			if(*str_current != '\"'){
				json_deserialize_error(deserialize_data,str_start,line,"string value not closed");
			}

			return eval_ignore_blanks(str_current+1, line);
		}

		char * deserialize_stk_json_value(
			JsonDeserializeData *deserialize_data
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
			bool ok=false;
			void *ptr_data=NULL;

			if (*str_current == '\"') {// try string ...
				std::string *str_aux=(std::string *)ptr_data;
				str_current=read_string_between_quotes(deserialize_data,str_current,line,str_aux);
			}
			else if (strncmp(str_current, "true", 4)==0) { // true detected ...
				if(ptr_data != NULL){
					*((bool *) ptr_data)= true;
				}
				str_current+=4;
			}
			else if (strncmp(str_current, "false", 5) == 0) {// boolean detected
				if(ptr_data != NULL){
					*((bool *) ptr_data) = false;
				}
				str_current+=5;
			}
			else{ // must a number
				// try read until next comma
				char default_str_value[1024]={0};

				str_end = advance_to_one_of_collection_of_char(str_current, (char *)end_char_standard_value, line);
				bytes_readed = str_end - str_current;
				if (*str_end != 0) {
					str_end--;
				}

				if (bytes_readed > 0) {
					// copy string...
					strncpy(default_str_value,str_current,bytes_readed);
					str_current+=bytes_readed;

					zs_float *number_value = 0;
					if((number_value=zs_strutils::parse_float(default_str_value)) != NULL){
						*((float *) ptr_data) = *number_value;
						delete number_value;

					}
				}
			}

			// if the parse was ok or json_var was not found we advance anyway
			if(ok || stk_json_element == NULL){
				return str_current;
			}

			return NULL;
		}

		char * deserialize_stk_json_vector(
				JsonDeserializeData *deserialize_data
				,const char *str_start
				, int & line
				,StackElement *stk_json_element
				){
			char *str_current = (char *)str_start;
			std::string error;

			str_current = eval_ignore_blanks(str_current, line);

			if(*str_current != '['){
				json_deserialize_error(deserialize_data,str_start,line,"A '[' was expected to parse JsonVarVector type");
				return 0;
			}

			str_current = eval_ignore_blanks(str_current+1, line);

			if(*str_current != ']'){ // do parsing primitive...

				do{
					str_current=deserialize_stk_json_element(deserialize_data,str_current,line,stk_json_element);

					str_current = eval_ignore_blanks(str_current, line);

					if(*str_current==','){
						str_current = eval_ignore_blanks(str_current+1, line);
					}else if(*str_current!=']'){
						json_deserialize_error(deserialize_data, str_current, line,  "Expected ',' or ']'");
						return 0;
					}

				}while(*str_current != ']');
			}

			return str_current+1;
		}

		char * deserialize_stk_json_object(JsonDeserializeData *deserialize_data, const char * str_start, int & line,StackElement *stk_json_element) {
			char *str_current = (char *)str_start;
			std::string variable_name,key_id;
			std::string error;

			str_current = eval_ignore_blanks(str_current, line);

			if(*str_current != '{'){
				json_deserialize_error(deserialize_data, str_start, line, "A '{' was expected to parse %s type",stk_json_element!=NULL?stk_json_element->typeStr():"");
				return NULL;
			}

			str_current = eval_ignore_blanks(str_current+1, line);

			if(*str_current != '}'){ // do parsing object values...
				do{
					StackElement *stk_json_property=NULL;
					str_current =read_string_between_quotes(deserialize_data, str_current, line, &key_id);
					if (*str_current != ':') {// ok check value
						json_deserialize_error(deserialize_data, str_current, line, "Error ':' expected");
						return NULL;
					}

					str_current = eval_ignore_blanks(str_current + 1, line);

					// get c property


					str_current=deserialize_stk_json_element(deserialize_data, str_current, line, stk_json_property);


					str_current = eval_ignore_blanks(str_current, line);

					if(*str_current==','){
						str_current = eval_ignore_blanks(str_current+1, line);
					}else if(*str_current!='}'){
						json_deserialize_error(deserialize_data, str_current, line, "Expected ',' or '}'");
						return NULL;
					}

				}while(*str_current != '}');
			}

			return str_current+1;
		}

		char * deserialize_stk_json_element(JsonDeserializeData *deserialize_data, const char * str_start, int & line,StackElement *stk_json_element) {
			// PRE: If json_var == NULL it parses but not saves
			char * str_current = (char *)str_start;
			std::string error="";
			str_current = eval_ignore_blanks(str_current, line);


			//try to deduce ...
			if(*str_current == '['){ // try parse vector
				str_current=deserialize_stk_json_vector(deserialize_data, str_current, line,stk_json_element);
			}else if(*str_current == '{') {// try parse object
				str_current=deserialize_stk_json_object(deserialize_data, str_current, line,stk_json_element);
			}else{ // try parse value
				str_current=deserialize_stk_json_value(deserialize_data, str_current,line,stk_json_element);
			}

			return str_current;
		}


	}

}
