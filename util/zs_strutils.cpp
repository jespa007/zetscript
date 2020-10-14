#include "../zetscript.h"

#define			MAX_BUFFER_COPY_FROM_INTERVAL 4096

namespace zetscript{
	namespace zs_strutils{

		/*enum{
			MAX_STRING_BUFFERS=256,
			MAX_LENGTH_BUFFER=512,
		};*/
		//char m_buffer[MAX_STRING_BUFFERS][MAX_LENGTH_BUFFER]={{0}};
		//char aux_str_copy[MAX_BUFFER_COPY_FROM_INTERVAL] = {0};
		//unsigned char m_index_buffer=0;
		//char  _sformat_buffer[4096] = { 0 };

		std::string  format(const  char  *input_text, ...){
			char  _sformat_buffer[4096] = { 0 };
			va_list  ap;
			va_start(ap,  input_text);
			vsprintf(_sformat_buffer,  input_text,  ap);
			va_end(ap);

			return std::string(_sformat_buffer);
		}

		int *  parse_int(const std::string & val){

			int *n=NULL;
			NumberType number_type = is_number(val);
			int numberValue=0;
			if(number_type == NumberType::NUMBER_TYPE_INT){
				numberValue=strtol(val.c_str(), NULL, 10);
			}else if(number_type == NumberType::NUMBER_TYPE_HEXA){
				numberValue=strtol(val.c_str(), NULL, 16);
			}else if(is_binary(val)){
				std::string binary = val.substr(0,val.size()-1);
				numberValue=strtol(binary.c_str(), NULL, 2);
			}
			else{
				return NULL;
			}

			n=new int;
			*n = numberValue;


			// TODO: develop exception handler.
			return n;
		}

		bool * parse_bool(const std::string & s){

			if(to_lower(s)=="true"){
				bool *b=new bool;
				*b=true;
				return b;

			}else if(to_lower(s)=="false"){
				bool *b=new bool;
				*b=false;
				return b;
			}

			// TODO: develop exception handler.
			return NULL;
		}

		float *  parse_float(const std::string & s){
			char *p;bool ok=true;
			float *n=NULL;

			float numberValue=0;
			if(s!="0") {// trivial case
			  numberValue=strtof((char *)s.c_str(),&p);
			  ok = *p == '\0';
			}

			if(ok){
				n=new float;
				*n = numberValue;
			}

			// TODO: develop exception handler.
			return n;
		}

		std::string int_to_str(int number){

			char int_str[100];

			sprintf(int_str,"%i",number);

		   return std::string(int_str);
		}

		std::string float_to_str(float number){

			char float_str[100];

			sprintf(float_str,"%f",number);

		   return std::string(float_str);
		}

		std::string to_lower(const std::string & str){

			std::string ret = str;
			for(unsigned short l = 0; l < ret.size();l++)
				ret[l] = tolower(ret[l]);
			return ret;
		}

		std::string to_upper(const std::string & str){

			std::string ret = str;
			for(unsigned short l = 0; l < ret.size();l++)
				ret[l] = toupper(ret[l]);
			return ret;
		}

		std::wstring to_wstring(const std::string& s)
		{
			std::wstring wsTmp(s.begin(), s.end());
			return wsTmp;
		}
		//------------------------------------------------------------------------------------------------------------------------

		std::vector<std::string> split(const std::string &s, char delim, std::vector<std::string> &elems) {
			std::stringstream ss(s);
			std::string item;
			while (std::getline(ss, item, delim)) {
				elems.push_back(item);
			}
			return elems;
		}


		std::vector<std::string> split(const std::string &s, char delim) {
			std::vector<std::string> elems;
			split(s, delim, elems);
			return elems;
		}

		bool is_empty(const std::string & str){
			return str.empty();
		}

		bool ends_with(const std::string & str, const std::string & ending){
			if (str.length() >= ending.length()) {
				return (0 == str.compare (str.length() - ending.length(), ending.length(), ending));
			}

			return false;
		}

		bool starts_with(const std::string & str, const std::string & starting){
			if (str.length() >= starting.length()) {
				return (0 == str.compare (0+starting.length(), starting.length(), starting));
			}
			return false;
		}

		std::string replace(const std::string & input_str, const std::string & str_old, const std::string & str_new){
			std::string str = input_str;
			size_t start_pos = 0;
			while((start_pos = str.find(str_old, start_pos)) != std::string::npos) {
				str.replace(start_pos, str_old.length(), str_new);
				start_pos += str_new.length(); // Handles case where 'str_new' is a substring of 'str_old'
			}

			return str;
		}


		bool is_digit(char c){
			return (('0' <= c) && (c<='9'));
		}


		bool is_hexa_digit(char c){
			return ((('0' <= c) && (c<='9')) || ('a'<=(tolower(c))&&(tolower(c)<='f')));
		}

		char *advance_digits(char *aux_p){

			while(is_digit(*aux_p))	aux_p++;
			return aux_p;
		}

		char *advance_hexa_digits(char *aux_p){

			while(is_hexa_digit(*aux_p))	aux_p++;
			return aux_p;
		}

		bool is_binary(const std::string & test_str_number){

			char *start_p=(char *)test_str_number.c_str();
			char *aux_p =start_p;

			while(is_digit(*aux_p)){
				if(!(*aux_p=='0' || *aux_p=='1')){
					break;
				}else{
					aux_p++;
				}
			}

			return (*aux_p=='b' || *aux_p=='B') && (aux_p > start_p);
		}

		NumberType is_number(const std::string & test_str_number){
			bool isHexa=false;
			char *str = (char *)test_str_number.c_str();

			switch(*str){
			case '-': str++; // negative numbers ...
					   break;
			case '0':
					  if(tolower(*(str+1))=='x')  {
						  isHexa = true;
						  str+=2;
					  }
					  break;
			default:
					break;
			};

			char *start_str = str; // saves position...
			if(isHexa) {
				str = advance_hexa_digits(str);
				if(str == start_str)
					return NumberType::NUMBER_TYPE_INVALID;

				if(*str == ' ' || *str == 0) return NumberType::NUMBER_TYPE_HEXA;
			}else{

				str = advance_digits(str);
				if(*str=='.') { // is candidate to double

					str++;
					str = advance_digits(str);

					if(*str != 'e'){
						if(*str == ' ' || *str == 0)
							return NumberType::NUMBER_TYPE_DOUBLE;
					}
				}

				if(*str == 'e'){  // is candidate to double

					str++;

					if(*str == '+' ||*str == '-'){
						str++;
					}

					str = advance_digits(str);
					if(*str == ' ' || *str == 0)
						return NumberType::NUMBER_TYPE_DOUBLE;

					return NumberType::NUMBER_TYPE_INVALID;
				}

				if(*str == ' ' || *str == 0)
					return NumberType::NUMBER_TYPE_INT;

			}

			return NumberType::NUMBER_TYPE_INVALID;


		}

		std::string  erase(std::string & str_old, char ch_to_remove){
			std::string str = str_old;
			std::string str_new="";

			for (unsigned i = 0; i < str_old.length(); ++i) {
				if (str_old[i] != ch_to_remove)
						str_new+=str_old[i];
			}
			return str_new;
		}

		int count(const std::string & s,char c){
			int n_items=0;

			for(unsigned i=0; i < s.size(); i++)
				if(s[i] == c)
					n_items++;

			return n_items;
		}

		void copy_from_ptr_diff(std::string & str_dst,const char *p1, const char *p2){

			char aux_str_copy[MAX_BUFFER_COPY_FROM_INTERVAL] = {0};

			if(p1 == NULL || p2 == NULL){
				THROW_RUNTIME_ERROR("NULL entry (%p %p)",p1,p2);
			}

			int var_length=p2-p1;

			if(var_length < 0 || var_length >= (MAX_BUFFER_COPY_FROM_INTERVAL+1)){
				THROW_RUNTIME_ERROR("array out of bounds (Max:%i Min:%i Current:%i)",0,MAX_BUFFER_COPY_FROM_INTERVAL,var_length);
			}

			if(p1 == p2){
				THROW_RUNTIME_ERROR("Nothing to copy");
			}

			strncpy(aux_str_copy,p1,var_length);

			str_dst=aux_str_copy;

		}

		std::string substring(const std::string & str, size_t start_idx, size_t end_idx){
			if(end_idx > str.length()){
				THROW_RUNTIME_ERROR("substring: end end_idx out of bounds");
			}

			if(start_idx >= end_idx){
				THROW_RUNTIME_ERROR("substring: start_idx >= end_idx");
			}

			return str.substr(start_idx,end_idx);


		}
	}
}
