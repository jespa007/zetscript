#include "script/zg_script.h"


char CStringUtils::m_buffer[MAX_STRING_BUFFERS][MAX_LENGTH_BUFFER]={{0}};
unsigned char CStringUtils::m_index_buffer=0;

using std::string;
using std::vector;


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
