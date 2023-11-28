

/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"
		
namespace zetscript{
        String  StringUtils::format(const  char  *_str_in, ...){

			char  _sformat_buffer[ZS_MAX_STR_BUFFER] = { 0 };
			va_list  ap;
			va_start(ap,  _str_in);
			vsprintf(_sformat_buffer,  _str_in,  ap);
			va_end(ap);

			return String(_sformat_buffer);
		}

		String	StringUtils::formatFileLine(const char* _file, int _line, const  char* _str_in, ...) {
			String str_out;
			char  _sformat_buffer[ZS_MAX_STR_BUFFER] = { 0 };
			va_list  ap;
			va_start(ap,  _str_in);
			vsprintf(_sformat_buffer,  _str_in,  ap);
			va_end(ap);

			if((_file == NULL || *_file == 0)){
				str_out=format("[line %i] : %s", _line,_sformat_buffer);
			}else{
				str_out=format(ZS_FORMAT_FILE_LINE" : %s", Path::getFilename(_file).toConstChar(), _line,_sformat_buffer);
			}

			return str_out;

		}

		char *StringUtils::cloneToCharPtr(const String & _str_in){
			if(_str_in.empty()==false){
				char *buf=(char *)ZS_MALLOC(_str_in.length()+1);
				strncpy(buf,_str_in.toConstChar(), _str_in.length());
				return buf;
			}

			return NULL;
		}

		zs_int *  StringUtils::parseInt(const String & val){
			zs_int *n=NULL;
			NumberType number_type = isNumber(val);
			zs_int value=0;
			if(number_type == NumberType::NUMBER_TYPE_INT){
				value=strtoll (val.toConstChar(), NULL, 10);
			}else if(number_type == NumberType::NUMBER_TYPE_HEXA){
				value=strtoll (val.toConstChar(), NULL, 16);
			}else if(isBinary(val)){
				String binary = val.substr(0,val.length()-1);
				value=strtoll (binary.toConstChar(), NULL, 2);
			}
			else{
				return NULL;
			}

			n=new zs_int;
			*n = value;

			// TODO: develop exception handler.
			return n;
		}

		bool * StringUtils::parseBool(const String & s){

			if(toLower(s)=="true"){
				bool *b=new bool;
				*b=true;
				return b;

			}else if(toLower(s)=="false"){
				bool *b=new bool;
				*b=false;
				return b;
			}

			// TODO: develop exception handler.
			return NULL;
		}

		zs_float *  StringUtils::parseFloat(const String & s){

			char *end;
			char *data=(char *)s.toConstChar();
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
			if (*s.toConstChar() == '\0' || *end != '\0') {
				return NULL;
			}

			return new zs_float(l);
		}

		String StringUtils::intToString(zs_int _number, const String & _format){

			String result="0";
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
			int reverse_len=result.length();
			for(int i=0; i < (reverse_len >>1); i++){
				// do swap
				char aux1=result[reverse_len -i-1];
				result[reverse_len -i-1]=result[i];
				result[i]=aux1;
			}

			// check format ...

			if(!_format.empty()){
				String sf=String::toLower(_format);
				char *it_str=(char *)sf.toConstChar();

				if(*it_str == 'd'){ // decimal
					zs_int *zs_int_aux=NULL;
					zs_int num_zeros=0;
					if((zs_int_aux=parseInt(it_str+1))!=NULL){
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

		String StringUtils::floatToString(zs_float _number){

			char float_str[100000];

			sprintf(float_str,"%f",_number);

		   return String(float_str);
		}

		char  StringUtils::toLower(char _ch){
			if('A' <= _ch && _ch <= 'Z'){
				 //Only if it's a upper letter
				return 'a'+(_ch-'A');
			}
			return _ch;
		}

		char  StringUtils::toUpper(char _ch){
			if('a' <= _ch && _ch <= 'z'){
				 //Only if it's a upper letter
				return 'A'+(_ch-'a');
			}
			return _ch;
		}

		String StringUtils::toLower(const String & str){

			String ret = str;
			for(int short l = 0; l < ret.length();l++){
				ret[l] = toLower(ret[l]);
			}
			return ret;
		}

		String StringUtils::toUpper(const String & str){

			String ret = str;
			for(int short l = 0; l < ret.length();l++){
				ret[l] = toUpper(ret[l]);
			}
			return ret;
		}

		//------------------------------------------------------------------------------------------------------------------------
		Vector<String> StringUtils::split(const String &_s_in, char _delim) {
		    Vector<String> elems;
		    //String s = s_in;
		    char *last_pos=(char *)_s_in.toConstChar();
		    char *pos=(char *)_s_in.toConstChar();
		    String token;
		    while((pos=strchr(pos,_delim))!=NULL) {
		        token = _s_in.substr(last_pos-_s_in.toConstChar(), pos-last_pos);
		        elems.append(String(token));
		        pos++;
		        last_pos=pos;
		    }

		    // push last token
		    token = _s_in.substr((last_pos-_s_in.toConstChar()), _s_in.length()-(last_pos-_s_in.toConstChar()));
	    	elems.append(String(token));

		    return elems;
		}

		Vector<String> StringUtils::split(const String &_s_in, const String & _delim) {
		    Vector<String> elems;
		    String s = _s_in;
		    int pos = 0;
		    String token;
		    while ((pos = s.find(_delim)) != String::npos) {
		        token = s.substr(0, pos);
		        elems.append(String(token));
		        s.erase(0, pos + _delim.length());
		    }

		    elems.append(String(s));
		    return elems;
		}

		bool StringUtils::isEmpty(const String & str){
			return str.empty();
		}

		bool StringUtils::endsWith(const String & str, const String & ending){
			size_t len_str=str.length();
			size_t len_end_str=ending.length();
			if(len_end_str<=len_str){
				const char *p1=str.toConstChar()+len_str-len_end_str;
				const char *p2=ending.toConstChar();
				return strcmp(p1,p2)==0;
			}

			return false;
		}

		bool StringUtils::startsWith(const String & str, const String & starting){
			if (str.length() >= starting.length()) {
				return strncmp(str.toConstChar(),starting.toConstChar(),starting.length())==0;
			}
			return false;
		}

		String StringUtils::replace(const String & input_str, const String & str_old, const String & str_new){
			String str = input_str;
			int	idx_current_pos=0;
			char *current_pos=NULL;

			while((current_pos = strstr((char*)str.toConstChar()+idx_current_pos,str_old.toConstChar())) != NULL) {
				idx_current_pos=current_pos-str.toConstChar();
				str.replace(idx_current_pos, str_old.length(), str_new);
				idx_current_pos += str_new.length(); // Handles case where 'str_new' is a substring of 'str_old'
			}

			return str;
		}


		bool is_digit(char c){
			return (('0' <= c) && (c<='9'));
		}


		bool is_hexa_digit(char c){
			return ((('0' <= c) && (c<='9')) || ('a'<=(toLower(c))&&(toLower(c)<='f')));
		}

		char *advance_digits(char *aux_p){

			while(is_digit(*aux_p))	aux_p++;
			return aux_p;
		}

		char *advance_hexa_digits(char *aux_p){

			while(is_hexa_digit(*aux_p))	aux_p++;
			return aux_p;
		}

		bool StringUtils::isBinary(const String & test_str_number){

			char *start_p=(char *)test_str_number.toConstChar();
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

		NumberType StringUtils::isNumber(const String & test_str_number){
			bool isHexa=false;
			char *str = (char *)test_str_number.toConstChar();

			switch(*str){
			case '-': str++; // negative numbers ...
					   break;
			case '0':
					  if(toLower(*(str+1))=='x')  {
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

		String  StringUtils::erase(const String & str_old, char ch_to_remove){
			String str = str_old;
			String str_new="";

			for(int  i = 0; i < str_old.length(); ++i) {
				if (str_old[i] != ch_to_remove)
						str_new+=str_old[i];
			}
			return str_new;
		}

		int StringUtils::count(const String & s,char c){
			int n_items=0;

			for(int i=0; i < s.length(); i++)
				if(s[i] == c)
					n_items++;

			return n_items;
		}

		String StringUtils::copyFromPtrDiff(const char *p1, const char *p2){

			char aux_str_copy[ZS_MAX_STR_BUFFER] = {0};

			if(p1 == NULL || p2 == NULL){
				ZS_THROW_RUNTIME_ERROR("NULL entry (%p %p)",p1,p2);
			}

			int var_length=p2-p1;

			if(var_length < 0 || var_length >= (ZS_MAX_STR_BUFFER+1)){
				ZS_THROW_RUNTIME_ERROR("array out of bounds (Max:%i Min:%i Current:%i)",0,ZS_MAX_STR_BUFFER,var_length);
			}

			if(p1 == p2){
				ZS_THROW_RUNTIME_ERRORF("Nothing to copy");
			}

			strncpy(aux_str_copy,p1,var_length);

			return aux_str_copy;

		}

		bool StringUtils::contains(const String & _str_input, const String & _str_containts,StringComparer sc){

			String s1=_str_input;
			String s2=_str_containts;

			if(sc==StringComparer::OrdinalIgnoreCase){
				s1=toLower(s1);
				s2=toLower(s2);
			}

			if (s1.find(s2) != String::npos) {
				return true;
			}
			return false;
		}

		bool StringUtils::contains(const Vector<String> & _strings, const String & _str_containts,StringComparer sc){

			String *_strings_items=_strings.data();
			for(int i = 0; i < _strings.size(); i++){
				if(contains(_strings_items[i],_str_containts,sc)){
					return true;
				}
			}
			return false;
		}

		int StringUtils::indexOf(const String& text, const String& pattern)
		{
			// where appears the pattern in the text?
			int loc = text.find(pattern, 0);
			if (loc != String::npos)
			{
				return loc;
			}
			else
			{
				return -1;
			}
		}

		String StringUtils::substring(const String & _str_in, int _start_index, int _end_index){
			String out_string="";
			if(_start_index<0){
				ZS_THROW_RUNTIME_ERROR("_start_index negative (_start_index:%i)", _start_index);
			}
			if(_end_index < 0){
				_end_index=_str_in.length()+_end_index;
			}
			if(_end_index>=_str_in.length()){
				_end_index=_str_in.length()-1;
			}

			if(_start_index<=_end_index){
				out_string=_str_in.substr(_start_index,_end_index-_start_index+1);
			}

			return out_string;
		}


		String StringUtils::unescape(const String & s)	{
			String res;
			char *it = (char *)s.toConstChar();
			char *end= (char *)s.toConstChar()+s.length();
			while (it != end) {
				char c = *it++;
				if (c == '\\' && it != end)	{
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