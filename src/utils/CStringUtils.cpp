#include "script/zg_script.h"


char CStringUtils::m_buffer[MAX_STRING_BUFFERS][MAX_LENGTH_BUFFER]={{0}};
unsigned char CStringUtils::m_index_buffer=0;

using std::string;
using std::vector;

bool CStringUtils::IS_SINGLE_COMMENT(char *str){

	if((*str!=0) && *str=='/'){
		return *(str+1)=='/';
	}
	return false;
}

bool CStringUtils::IS_START_COMMENT(char *str){

	if((*str!=0) && *str=='/'){
		return *(str+1)=='*';
	}
	return false;
}

bool CStringUtils::IS_END_COMMENT(char *str){

	if((*str!=0) && *str=='*'){
		return *(str+1)=='/';
	}
	return false;
}

char *CStringUtils::ADVANCE_TO_CHAR(char *str,char c, int & m_line) {
	char *aux_p = str;
	// make compatible windows format (\r)...
	while(*aux_p!=0 && (*aux_p !=(c) )) {
		if(*aux_p == '\n') {m_line++;}; // make compatible windows format...
		aux_p++;
	}


	return aux_p;
}

char *CStringUtils::ADVANCE_TO_END_COMMENT(char *aux_p, int &m_line){

	if(IS_START_COMMENT(aux_p)){
		aux_p+=2; //advance first
		while(!IS_END_COMMENT(aux_p) && *aux_p != 0){

			aux_p = ADVANCE_TO_CHAR(aux_p,'*', m_line);
			if(*aux_p == '\n') {aux_p++;m_line++;}; // make compatible windows format...
			if(*aux_p == '\r') aux_p++;
			if(*aux_p == '*' && *(aux_p+1) != '/') aux_p++; // not end comment ... advance ...
		}
	}

	return aux_p;

}

char *CStringUtils::IGNORE_BLANKS(const char *str, int &m_line) {
	char *aux_p = (char *)str;
	bool end = false;
	while(!end){
		end = true;
		while(*aux_p!=0 && ((*aux_p==' ')  || (*aux_p=='\t'))) aux_p++;

		if(IS_SINGLE_COMMENT(aux_p)) // ignore line
			aux_p = ADVANCE_TO_CHAR(aux_p,'\n', m_line);

		else if(IS_START_COMMENT(aux_p)){
			// ignore until get the end of the comment...
			aux_p = ADVANCE_TO_END_COMMENT(aux_p, m_line);

			if(IS_END_COMMENT(aux_p))
				aux_p+=2;

			end=false;
		}
		// make compatible windows format...
		if(*aux_p == '\r')
			aux_p++;

		if(*aux_p == '\n') {
			m_line=m_line+1;
			end=false;
			aux_p++;
		}
	}
	return aux_p;
}

char *CStringUtils::IGNORE_BLANKS_REVERSE(const char *str_begin,const char *str_end, int &m_line) {
	char *aux_p = (char *)str_begin;
	bool end = false;
	while(!end){
		end = true;
		while(aux_p!=str_end && ((*aux_p==' ')  || (*aux_p=='\t'))) aux_p--;

		// make compatible windows format...
		if(*aux_p == '\r')
			aux_p--;

		if(*aux_p == '\n') {
			m_line=m_line+1;
			end=false;
			aux_p--;
		}
	}
	return aux_p;
}


char *CStringUtils::ADVANCE_TO_ONE_OF_COLLECTION_CHAR(char *str,char *end_char_standard_value, int &m_line) {
	char *aux_p = str;
	char *chk_char;
	while(*aux_p!=0){
		chk_char = end_char_standard_value;

		// comment blocks also is returned (these lines must be ignored)
		if(IS_START_COMMENT(aux_p)) {
			aux_p = ADVANCE_TO_END_COMMENT(aux_p, m_line);
			if(IS_END_COMMENT(aux_p))
				aux_p+=2;
		}

		if(IS_SINGLE_COMMENT(aux_p)) {
			aux_p = ADVANCE_TO_CHAR(aux_p,'\n', m_line);
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



const char * CStringUtils::formatString(const char *str, ...) {

	unsigned char  m_current = m_index_buffer;
	m_index_buffer++;

	CAPTURE_VARIABLE_ARGS(CStringUtils::m_buffer[m_current], str);

	//sprintf(CStringUtils::m_buffer[m_current],text);

	return (const char *)CStringUtils::m_buffer[m_current];

};


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
