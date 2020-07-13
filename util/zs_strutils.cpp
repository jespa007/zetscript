#include "../zetscript.h"

namespace zetscript{
	namespace zs_strutils{

		enum{
			MAX_STRING_BUFFERS=256,
			MAX_LENGTH_BUFFER=512,
		};


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

		int *  parseInteger(const std::string & val){

			int *n=NULL;
			NumberType number_type = isNumber(val);
			int numberValue=0;
			if(number_type == NumberType::NUMBER_TYPE_INT){
				numberValue=strtol(val.c_str(), NULL, 10);
			}else if(number_type == NumberType::NUMBER_TYPE_HEXA){
				numberValue=strtol(val.c_str(), NULL, 16);
			}else if(isBinary(val)){
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

		bool * parseBoolean(const std::string & s){

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

		float *  parseFloat(const std::string & s){
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

		std::string intToString(int number){

			char int_str[100];

			sprintf(int_str,"%i",number);

		   return std::string(int_str);
		}

		std::string toLower(const std::string & str){

			std::string ret = str;
			for(unsigned short l = 0; l < ret.size();l++)
				ret[l] = tolower(ret[l]);
			return ret;
		}

		std::string toUpper(const std::string & str){

			std::string ret = str;
			for(unsigned short l = 0; l < ret.size();l++)
				ret[l] = toupper(ret[l]);
			return ret;
		}

		std::wstring toWString(const std::string& s)
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

		bool isEmpty(const std::string & str){
			return str.empty();
		}

		bool endsWith(const std::string & fullString, const std::string & ending){
			if (fullString.length() >= ending.length()) {
				return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
			}

			return false;
		}


		bool isDigit(char c){
			return (('0' <= c) && (c<='9'));
		}


		bool isHexaDigit(char c){
			return ((('0' <= c) && (c<='9')) || ('a'<=(tolower(c))&&(tolower(c)<='f')));
		}

		char *advanceDigits(char *aux_p){

			while(isDigit(*aux_p))	aux_p++;
			return aux_p;
		}

		char *advanceHexaDigits(char *aux_p){

			while(isHexaDigit(*aux_p))	aux_p++;
			return aux_p;
		}

		bool isBinary(const std::string & test_str_number){

			char *start_p=(char *)test_str_number.c_str();
			char *aux_p =start_p;

			while(isDigit(*aux_p)){
				if(!(*aux_p=='0' || *aux_p=='1')){
					break;
				}else{
					aux_p++;
				}
			}

			return (*aux_p=='b' || *aux_p=='B') && (aux_p > start_p);
		}

		NumberType isNumber(const std::string & test_str_number){
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
				str = advanceHexaDigits(str);
				if(str == start_str)
					return NumberType::NUMBER_TYPE_INVALID;

				if(*str == ' ' || *str == 0) return NumberType::NUMBER_TYPE_HEXA;
			}else{

				str = advanceDigits(str);
				if(*str=='.') { // is candidate to double

					str++;
					str = advanceDigits(str);

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

					str = advanceDigits(str);
					if(*str == ' ' || *str == 0)
						return NumberType::NUMBER_TYPE_DOUBLE;

					return NumberType::NUMBER_TYPE_INVALID;
				}

				if(*str == ' ' || *str == 0)
					return NumberType::NUMBER_TYPE_INT;

			}

			return NumberType::NUMBER_TYPE_INVALID;


		}

		std::string  remove(std::string & str_old, char ch_to_remove){
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
	}


}