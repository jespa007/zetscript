#include "../CZetScript.h"


char CStringUtils::m_buffer[MAX_STRING_BUFFERS][MAX_LENGTH_BUFFER]={{0}};
char CStringUtils::aux_str_copy[MAX_BUFFER_COPY_FROM_INTERVAL] = {0};
unsigned char CStringUtils::m_index_buffer=0;

using std::string;
using std::vector;



int *  CStringUtils::ParseInteger(const string & val){

	int *n=NULL;
	int type_number = CStringUtils::isNumber(val);
	int numberValue=0;
	if(type_number == CStringUtils::STRING_IS_INT){
		numberValue=strtol(val.c_str(), NULL, 10);
	}else if(type_number == CStringUtils::STRING_IS_HEXA){
		numberValue=strtol(val.c_str(), NULL, 16);
	}else{
		return NULL;
	}

    n=new int;
    *n = numberValue;


	// TODO: develop exception handler.
	return n;
}

bool * CStringUtils::ParseBoolean(const string & s){

	if(CStringUtils::toLower(s)=="true"){
		bool *b=new bool;
		*b=true;
		return b;

	}else if(CStringUtils::toLower(s)=="false"){
		bool *b=new bool;
		*b=false;
		return b;
	}

	// TODO: develop exception handler.
	return NULL;
}

float *  CStringUtils::ParseFloat(const string & s){
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



const char * CStringUtils::formatString(const char *str, ...) {

	unsigned char  m_current = m_index_buffer;
	m_index_buffer++;

	CAPTURE_VARIABLE_ARGS(CStringUtils::m_buffer[m_current], str);

	//sprintf(CStringUtils::m_buffer[m_current],text);

	return (const char *)CStringUtils::m_buffer[m_current];

};



char * CStringUtils::copyStringFromInterval(const char *p1, const char *p2){

	if(p1 == NULL || p2 == NULL){
		print_error_cr("NULL entry (%p %p)",p1,p2);
		return NULL;
	}

	int var_length=p2-p1;

	if(var_length < 0 || var_length >= (MAX_BUFFER_COPY_FROM_INTERVAL+1)){
		print_error_cr("array out of bounds (Max:%i Min:%i Current:%i)",0,MAX_BUFFER_COPY_FROM_INTERVAL,var_length);
		return NULL;
	}

	if(p1 == p2){
		print_error_cr("Nothing to copy");
		return NULL;
	}

	memset(aux_str_copy,0,sizeof(aux_str_copy));
	strncpy(aux_str_copy,p1,var_length);

	return aux_str_copy;
}


string CStringUtils::intToString(int number){

   std::stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

string CStringUtils::floatToString(float number){

	char buff[100];
	sprintf(buff, "%f",number);

	string ss = buff;
	ss = CStringUtils::replace(ss,',','.');

   return ss;//return a string with the contents of the stream
}

string CStringUtils::doubleToString(double number){

	char buff[100];
	sprintf(buff, "%f",number);
	string ss=buff;

	ss=CStringUtils::replace(ss,',','.');


   return ss;//return a string with the contents of the stream
}

string CStringUtils::toLower(const string & str){

	string ret = str;
	for(unsigned short l = 0; l < ret.size();l++)
		ret[l] = tolower(ret[l]);
    return ret;
}

string CStringUtils::toUpper(const string & str){

	string ret = str;
	for(unsigned short l = 0; l < ret.size();l++)
		ret[l] = toupper(ret[l]);
    return ret;
}

std::wstring CStringUtils::toWString(const std::string& s)
{
   /* std::string curLocale = setlocale(LC_ALL, "");
    const char* _Source = s.c_str();
    size_t _Dsize = mbstowcs(NULL, _Source, 0) + 1;
    wchar_t *_Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest,_Source,_Dsize);
    std::wstring result = _Dest;
    delete []_Dest;

    setlocale(LC_ALL, curLocale.c_str());*/
	 std::wstring wsTmp(s.begin(), s.end());


    return wsTmp;
   // std::wstring wsTmp(s.begin(), s.end());

   // ws = wsTmp;

   // return 0;
}

//------------------------------------------------------------------------------------------------------------------------

vector<string> & CStringUtils::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> CStringUtils::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

bool CStringUtils::isEmpty(const string & str){
	return str.empty();
}

bool CStringUtils::endsWith(const string & fullString, const string & ending){
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    }

    return false;
}


bool CStringUtils::isDigit(char c){
	return (('0' <= c) && (c<='9'));
}


bool CStringUtils::isHexaDigit(char c){
	return ((('0' <= c) && (c<='9')) || ('a'<=(tolower(c))&&(tolower(c)<='f')));
}


char *ADVANCE_DIGITS(char *aux_p){

	while(CStringUtils::isDigit(*aux_p))	aux_p++;
	return aux_p;
}

char *ADVANCE_HEXADIGITS(char *aux_p){

	while(CStringUtils::isHexaDigit(*aux_p))	aux_p++;
	return aux_p;
}



int CStringUtils::isNumber(const string & test_str_number){
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
		str = ADVANCE_HEXADIGITS(str);
		if(str == start_str)
			return STRING_IS_INVALID_NUMBER;

		if(*str == ' ' || *str == 0) return STRING_IS_HEXA;
	}else{
		str = ADVANCE_DIGITS(str);
		if(*str=='.') { // is candidate to double

			str++;
			str = ADVANCE_DIGITS(str);

			if(*str != 'e'){
				if(*str == ' ' || *str == 0)
					return STRING_IS_DOUBLE;
			}
		}

		if(*str == 'e'){  // is candidate to double

			str++;

			if(*str == '+' ||*str == '-'){
				str++;
			}

			str = ADVANCE_DIGITS(str);
			if(*str == ' ' || *str == 0)
				return STRING_IS_DOUBLE;

			return STRING_IS_INVALID_NUMBER;
		}

		if(*str == ' ' || *str == 0)
			return STRING_IS_INT;

	}

	return STRING_IS_INVALID_NUMBER;


}

string  CStringUtils::replace(const string & str_old, const char old_ch, char new_ch){
	string str = str_old;
	for (unsigned i = 0; i < str.length(); ++i) {
		if (str[i] == old_ch)
		  str[i] = new_ch;
	}

	return str;
}

void CStringUtils::replace(string & str_input, const string & toReplace, const string & replaceWith){

	std::size_t found;;
	while((found = str_input.find(toReplace)) != std::string::npos){
		str_input = (str_input.replace(found, toReplace.length(), replaceWith));
	}


}

string  CStringUtils::remove(string & str_old, char ch_to_remove){
	string str = str_old;
	string str_new="";

	for (unsigned i = 0; i < str_old.length(); ++i) {
		if (str_old[i] != ch_to_remove)
				str_new+=str_old[i];
	}

	return str_new;
}

int CStringUtils::count(const string & s,char c){
	int n_items=0;

	for(unsigned i=0; i < s.size(); i++)
		if(s[i] == c)
			n_items++;

	return n_items;
}
