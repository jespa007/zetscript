#include "../zetscript.h"

namespace zetscript{
	namespace zs_strutils{

		std::string  format(const  char  *input_text, ...){
			char  _sformat_buffer[ZS_MAX_STR_BUFFER] = { 0 };
			va_list  ap;
			va_start(ap,  input_text);
			vsprintf(_sformat_buffer,  input_text,  ap);
			va_end(ap);

			return std::string(_sformat_buffer);
		}

		zs_int *  parse_int(const std::string & val){
			zs_int *n=NULL;
			NumberType number_type = is_number(val);
			zs_int value=0;
			if(number_type == NumberType::NUMBER_TYPE_INT){
				value=strtoll (val.c_str(), NULL, 10);
			}else if(number_type == NumberType::NUMBER_TYPE_HEXA){
				value=strtoll (val.c_str(), NULL, 16);
			}else if(is_binary(val)){
				std::string binary = val.substr(0,val.size()-1);
				value=strtoll (binary.c_str(), NULL, 2);
			}
			else{

				//throw std::runtime_error(format("cannot convert \"%s\" to zs_int",val.c_str()));
				return NULL;
			}

			n=new zs_int;
			*n = value;

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

			//throw std::runtime_error(format("cannot convert \"%s\" to bool",s.c_str()));

			// TODO: develop exception handler.
			return NULL;
		}

		zs_float *  parse_float(const std::string & s){

			char *end;
			char *data=(char *)s.c_str();
			float  l;
			errno = 0;
			l = strtof(data, &end);

			if (end == data){
				return NULL;
			}else	if ((errno == ERANGE && l == FLT_MAX) || l > FLT_MAX) {
				return NULL;
			}else if ((errno == ERANGE && l < FLT_MIN) /* || l < FLT_MIN*/) {
				return NULL;
			}
			if (*s.c_str() == '\0' || *end != '\0') {
				return NULL;
			}

			return new zs_float(l);
		}

		std::string zs_int_to_str(zs_int number){

			std::string result="0";
			bool negative=false;

			if(number < 0){
				negative=true;
				number=-number;
			}

			if(number != 0){
				result="";
				while(number != 0){
					result+=((number%10)+'0');
					number*=0.1;
				}
			}

			// is negative ?
			if(negative){
				result+="-";
			}

			// reverse result
			size_t len=result.length();
			for(unsigned i=0; i < (len>>1); i++){
				// do swap
				char aux1=result[len-i-1];
				result[len-i-1]=result[i];
				result[i]=aux1;
			}

		   return result;

		}

		std::string float_to_str(zs_float number){

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

		std::vector<std::string> split(const std::string &s_in, const std::string & delim) {
		    std::vector<std::string> elems;
		    std::string s = s_in;


		    size_t pos = 0;
		    std::string token;
		    while ((pos = s.find(delim)) != std::string::npos) {
		        token = s.substr(0, pos);
		        elems.push_back(token);
		//        std::cout << token << std::endl;
		        s.erase(0, pos + delim.length());
		    }

		    elems.push_back(s);
		    //std::cout << s << std::endl;
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

			char aux_str_copy[ZS_MAX_STR_BUFFER] = {0};

			if(p1 == NULL || p2 == NULL){
				THROW_RUNTIME_ERROR("NULL entry (%p %p)",p1,p2);
			}

			int var_length=p2-p1;

			if(var_length < 0 || var_length >= (ZS_MAX_STR_BUFFER+1)){
				THROW_RUNTIME_ERROR("array out of bounds (Max:%i Min:%i Current:%i)",0,ZS_MAX_STR_BUFFER,var_length);
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

		bool contains(const std::string & input, const std::string & str_containts,StringComparer sc){

			std::string s1=input;
			std::string s2=str_containts;

			if(sc==StringComparer::OrdinalIgnoreCase){
				s1=to_lower(s1);
				s2=to_lower(s2);
			}

			if (s1.find(s2) != std::string::npos) {
				return true;
			}
			return false;
		}

		bool contains(const std::vector<std::string> & input, const std::string & str_containts,StringComparer sc){

			for(unsigned i = 0; i < input.size(); i++){
				if(contains(input,str_containts,sc)){
					return true;
				}
			}
			return false;
		}

		int index_of(std::string& text, std::string& pattern)
		{
			// where appears the pattern in the text?
			std::string::size_type loc = text.find(pattern, 0);
			if (loc != std::string::npos)
			{
				return loc;
			}
			else
			{
				return -1;
			}
		}


		std::string unescape(const std::string & s)	{
			std::string res;
			std::string::const_iterator it = s.begin();
			while (it != s.end()) {
				char c = *it++;
				if (c == '\\' && it != s.end())	{
					switch (*it++) {
						case '\\': c = '\\'; break;
						case 'n': c = '\n'; break;
						case 't': c = '\t'; break;
						case '"': c = '\"'; break;
						case 0x27: c = '\''; break;
						// all other escapes
						default:
						// invalid escape sequence - skip it. alternatively you can copy it as is, throw an exception...
					continue;
				  }
				}
				res += c;
			}

		  return res;
		}

	}
}
