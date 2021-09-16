#include "../zetscript.h"

namespace zetscript{
	namespace zs_strutils{

		zs_string  format(const  char  *input_text, ...){
			char  _sformat_buffer[ZS_MAX_STR_BUFFER] = { 0 };
			va_list  ap;
			va_start(ap,  input_text);
			vsprintf(_sformat_buffer,  input_text,  ap);
			va_end(ap);

			return zs_string(_sformat_buffer);
		}

		zs_int *  parse_zs_int(const zs_string & val){
			zs_int *n=NULL;
			NumberType number_type = is_number(val);
			zs_int value=0;
			if(number_type == NumberType::NUMBER_TYPE_INT){
				value=strtoll (val.c_str(), NULL, 10);
			}else if(number_type == NumberType::NUMBER_TYPE_HEXA){
				value=strtoll (val.c_str(), NULL, 16);
			}else if(is_binary(val)){
				zs_string binary = val.substr(0,val.length()-1);
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

		bool * parse_bool(const zs_string & s){

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

		zs_float *  parse_zs_float(const zs_string & s){

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

		zs_string zs_int_to_str(zs_int _number, const zs_string & _format){

			zs_string result="0";
			bool negative=false;

			if(_number < 0){
				negative=true;
				_number=-_number;
			}

			if(_number != 0){
				result="";
				while(_number != 0){
					result+=((_number%10)+'0');
					_number*=0.1;
				}
			}

			// reverse result
			size_t len=result.length();
			for(unsigned i=0; i < (len>>1); i++){
				// do swap
				char aux1=result[len-i-1];
				result[len-i-1]=result[i];
				result[i]=aux1;
			}

			// check format ...
			if(_format.length()>0){
				zs_string sf=zs_strutils::to_lower(_format);
				char *it_str=(char *)sf.c_str();

				if(*it_str == 'd'){ // decimal
					zs_int *zs_int_aux=NULL;
					zs_int num_zeros=0;
					if((zs_int_aux=parse_zs_int(it_str+1))!=NULL){
						num_zeros=*zs_int_aux;
						delete zs_int_aux;
						zs_int_aux=NULL;
						int len=num_zeros-(int)result.length();

						// append zeros to the beginning
						for(int i=0; i < len; i++){
							result="0"+result;
						}
					}
					it_str++;
				}
			}

			// is negative...
			if(negative){
				result="-"+result;
			}

		   return result;

		}

		zs_string zs_float_to_str(zs_float _number, const zs_string & _format){

			char float_str[100];

			sprintf(float_str,"%f",_number);

		   return zs_string(float_str);
		}

		zs_string to_lower(const zs_string & str){

			zs_string ret = str;
			for(unsigned short l = 0; l < ret.length();l++){
				ret[l] = tolower(ret[l]);
			}
			return ret;
		}

		zs_string to_upper(const zs_string & str){

			zs_string ret = str;
			for(unsigned short l = 0; l < ret.length();l++)
				ret[l] = toupper(ret[l]);
			return ret;
		}

		std::wstring to_wstring(const zs_string& s)
		{
			std::wstring wsTmp(s.begin(), s.end());
			return wsTmp;
		}
		//------------------------------------------------------------------------------------------------------------------------

		std::vector<zs_string> split(const zs_string &s, char delim, std::vector<zs_string> &elems) {
			zs_stringstream ss(s);
			zs_string item;
			while (std::getline(ss, item, delim)) {
				elems.push_back(item);
			}
			return elems;
		}


		std::vector<zs_string> split(const zs_string &s, char delim) {
			std::vector<zs_string> elems;
			split(s, delim, elems);
			return elems;
		}

		std::vector<zs_string> split(const zs_string &s_in, const zs_string & delim) {
		    std::vector<zs_string> elems;
		    zs_string s = s_in;
		    size_t pos = 0;
		    zs_string token;
		    while ((pos = s.find(delim)) != zs_string::npos) {
		        token = s.substr(0, pos);
		        elems.push_back(token);
		//        std::cout << token << std::endl;
		        s.erase(0, pos + delim.length());
		    }

		    elems.push_back(s);
		    //std::cout << s << std::endl;
		    return elems;
		}

		bool is_empty(const zs_string & str){
			return str.empty();
		}

		bool ends_with(const zs_string & str, const zs_string & ending){
			size_t len_str=str.length();
			size_t len_end_str=ending.length();
			if(len_end_str<=len_str){
				const char *p1=str.c_str()+len_str-len_end_str;
				const char *p2=ending.c_str();
				return strcmp(p1,p2)==0;
			}

			return false;
		}

		bool starts_with(const zs_string & str, const zs_string & starting){
			if (str.length() >= starting.length()) {
				return strncmp(str.c_str(),starting.c_str(),starting.length())==0;
			}
			return false;
		}

		zs_string replace(const zs_string & input_str, const zs_string & str_old, const zs_string & str_new){
			zs_string str = input_str;
			char *start_pos=NULL;

			while((start_pos = strstr(start_pos,str_old.c_str())) != NULL) {
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

		bool is_binary(const zs_string & test_str_number){

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

		NumberType is_number(const zs_string & test_str_number){
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

		zs_string  erase(zs_string & str_old, char ch_to_remove){
			zs_string str = str_old;
			zs_string str_new="";

			for (unsigned i = 0; i < str_old.length(); ++i) {
				if (str_old[i] != ch_to_remove)
						str_new+=str_old[i];
			}
			return str_new;
		}

		int count(const zs_string & s,char c){
			int n_items=0;

			for(unsigned i=0; i < s.length(); i++)
				if(s[i] == c)
					n_items++;

			return n_items;
		}

		void copy_from_ptr_diff(zs_string & str_dst,const char *p1, const char *p2){

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

		bool contains(const zs_string & input, const zs_string & str_containts,StringComparer sc){

			zs_string s1=input;
			zs_string s2=str_containts;

			if(sc==StringComparer::OrdinalIgnoreCase){
				s1=to_lower(s1);
				s2=to_lower(s2);
			}

			if (s1.find(s2) != zs_string::npos) {
				return true;
			}
			return false;
		}

		bool contains(const std::vector<zs_string> & input, const zs_string & str_containts,StringComparer sc){

			for(unsigned i = 0; i < input.size(); i++){
				if(contains(input,str_containts,sc)){
					return true;
				}
			}
			return false;
		}

		int index_of(zs_string& text, zs_string& pattern)
		{
			// where appears the pattern in the text?
			zs_string::size_type loc = text.find(pattern, 0);
			if (loc != zs_string::npos)
			{
				return loc;
			}
			else
			{
				return -1;
			}
		}


		zs_string unescape(const zs_string & s)	{
			zs_string res;
			zs_string::const_iterator it = s.begin();
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
