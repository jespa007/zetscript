/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

#define ZS_STRING_MAX_ELEMENTS			2000000
#define ZS_STRING_EXPAND_CHAR_ELEMENTS 	16

namespace zetscript{

	//--------
	// STATIC
	int String::npos=-1;

	bool String::contains(const Vector<String> & _strings, const String & _str_containts,StringComparer sc){

		String *_strings_items=_strings.data();
		for(int i = 0; i < _strings.length(); i++){
			if(_strings_items[i].contains(_str_containts,sc)){
				return true;
			}
		}
		return false;
	}

	String  String::format(const  char  *_str_in, ...){

		char  _sformat_buffer[ZS_MAX_STR_BUFFER] = { 0 };
		va_list  ap;
		va_start(ap,  _str_in);
		vsprintf(_sformat_buffer,  _str_in,  ap);
		va_end(ap);

		return String(_sformat_buffer);
	}

	String	String::formatFileLine(const char* _file, int _line, const  char* _str_in, ...) {
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


	//--------
	// MEMBER

	void	String::inc_slots(int _n_slots){
		// condition to increase this->items:
		// last slot exhausted
		if (this->_size <= (this->count+_n_slots)) {
			this->_size = this->count+_n_slots+this->count*1.5;//ZS_STRING_EXPAND_CHAR_ELEMENTS;
			size_t _size_new=sizeof(char) * this->_size + 1;
			buf=(char *)realloc(buf, _size_new); // + 1 for the keeping the null character
			memset(buf+this->count,0,_size_new-this->count);
		}

		this->count+=_n_slots;
	}

	void String::set(const char * _const_char, int _length){

		const char *buffer=_const_char;
		if(buffer==NULL){
			buffer="";
		} // do not create string from NULL pointers

		__cleanup__(); // cleanup any existing data
		count=_length;
		if(count == npos){
			count = (int)strlen(buffer);
		}
		_size=count+ZS_STRING_EXPAND_CHAR_ELEMENTS;
		this->buf = (char *)ZS_MALLOC(sizeof(char) * _size+1); // + 1 for the keeping the null character
		strcpy(buf, buffer); // copy from the incoming buffer to character buffer of the new object
	}


	void String::set(const String & _s){
		set(_s.toConstChar());
	}

	String::String() // default constructor -> empty
	{
		buf=NULL;
		_size=0;
		count=0;
		//set("");
	}

	String::String(const char * _const_char) // constructor
	{
		buf=NULL;
		count=_size=0;
		set(_const_char);
	}

	String::String(const String & _obj) // copy constructor
	{
		buf=NULL;
		count=_size=0;
		set(_obj);
	}

	String::String(String && _str_tmp) // move constructor
	// && is a reference operator defined in the C++11 standard
	// which means "dyingObj" is an r-value reference.
	// Compiler calls this constructor when the object passed in the argument
	// is about to die due to scope end or such
	{
		// Copy data from the incoming object
		_size = _str_tmp._size;
		count = _str_tmp.count;

		// Transfer ownership of underlying char buffer from incoming object to this object
		buf = _str_tmp.buf;
		_str_tmp.buf = nullptr;
	}

	String& String::operator=(const String & _str) {
		set(_str);
		return *this;
	}

	String& String::operator=(String && _str_tmp){ // move assignment

		__cleanup__(); // cleanup any existing data

		// Copy data from the incoming object
		_size = _str_tmp._size;
		count = _str_tmp.count;

		// Transfer ownership of underlying char buffer from incoming object to this object
		buf = _str_tmp.buf;
		_str_tmp.buf = nullptr;

		return *this;
	}


	String& String::operator=(const char *_str) // copy assignment
	{
		set(_str);
		return *this;
	}

	String& String::operator+=(const String& obj){
		append(obj.buf);
	    return *this;
	}

	String& String::operator+=(const char *_str) {
		append(_str);
		return *this;
	}

	String& String::operator+=(char _c){
		append(_c);
	    return *this;
	}

    char& String::operator[] (int _pos){

    	if(_pos < 0 || _pos >= this->count) ZS_THROW_EXCEPTION("String::access out of bounds");

    	return buf[_pos];
    }

    const char& String::operator[] (int _pos) const{
    	if(_pos < 0 || _pos >= this->count) ZS_THROW_EXCEPTION("String::access out of bounds");

    	return buf[_pos];
    }

    char& String::at (int _pos){
    	if(_pos < 0 || _pos >= this->count) ZS_THROW_EXCEPTION("String::access out of bounds");

    	return buf[_pos];
    }

    const char& String::at (int _pos) const{
    	if(_pos < 0 || _pos >= this->count) ZS_THROW_EXCEPTION("String::access out of bounds");

    	return buf[_pos];
    }

    void String::clear(){
    	__cleanup__();
    }

    String String::newFromTwo(const char *_s1, const char *_s2) {
		String s; // create a new string named 's'
		size_t len1=0;
		size_t len2=0;
		char *buf_new_from_two =NULL;
		size_t buf_new_from_two_size = 0;

		if(_s1!=NULL){
			len1=strlen(_s1);
		}

		if(_s2!=NULL){
			len2=strlen(_s2);
		}

		buf_new_from_two_size =len1 + len2;

		if(buf_new_from_two_size>0){
			buf_new_from_two = (char *)ZS_MALLOC(buf_new_from_two_size + 1); // allocate memory to keep the concatenated string

			if(len1!=0) {
				strcpy(buf_new_from_two, _s1); // copy the 1st string
			}

			if(len2!=0) {
				strcpy(buf_new_from_two + len1, _s2);
			}

			s=buf_new_from_two;

			free(buf_new_from_two);
		}
		return s;
    }

    // +
    String  operator+(const String & _s1, const String &_s2) // concatenation
	{
		return String::newFromTwo(_s1.buf,_s2.buf);
	}

	String  operator+(const char * _s1, const String &_s2) // concatenation
	{
		return String::newFromTwo(_s1,_s2.buf);
	}

	String operator+(const String & _s1, const char * _s2) // concatenation
	{
		return String::newFromTwo(_s1.buf,_s2);
	}

	String operator+(const String & _s1, char _s2){
		char tmp[]={_s2,0};
		return String::newFromTwo(_s1.buf,tmp);
	}

	String operator+(char  _s1, const String & _s2){
		char tmp[]={_s1,0};
		return String::newFromTwo(tmp,_s2.buf);
	}

	// ==
	bool operator==(const String & _s1, const String &_s2){
		// both are not empty
		return strcmp(_s1.toConstChar(),_s2.toConstChar())==0;
	}

	bool operator==(const char * _s1, const String & _s2){
		if(_s1==NULL){
			ZS_THROW_RUNTIME_ERRORF("operator '==' : operand 1 == NULL");
		}

		return strcmp(_s1,_s2.toConstChar())==0;
	}

	bool operator==(const String & _s1, const char *_s2){
		if(_s2==NULL){
			ZS_THROW_RUNTIME_ERRORF("operator '==' : operand 2 == NULL");
		}

		return strcmp(_s1.toConstChar(),_s2)==0;
	}

	// !=
	bool operator!=(const String & _s1, const String &_s2){
		return strcmp(_s1.toConstChar(),_s2.toConstChar())!=0;
	}

	bool operator!=(const char * _s1, const String & _s2){
		if(_s1==NULL){
			ZS_THROW_RUNTIME_ERRORF("operator '!=' : operand 1 == NULL");
		}

		return strcmp(_s1,_s2.toConstChar())!=0;
	}

	bool operator!=(const String & _s1, const char *_s2){
		if(_s2==NULL){
			ZS_THROW_RUNTIME_ERRORF("operator '!=' : operand 2 == NULL");
		}

		return strcmp(_s1.toConstChar(),_s2)!=0;
	}


	//------------------------------------------------------------------------------------------------------------------------
	Vector<String> String::split(char _delim) const {
		const String &_s_in=*this;
		Vector<String> elems;
		//String s = s_in;
		char *last_pos=(char *)_s_in.toConstChar();
		char *pos=(char *)_s_in.toConstChar();
		String token;
		while((pos=strchr(pos,_delim))!=NULL) {
			token = _s_in.substr(last_pos-_s_in.toConstChar(), pos-last_pos);
			elems.push(String(token));
			pos++;
			last_pos=pos;
		}

		// push last token
		token = _s_in.substr((last_pos-_s_in.toConstChar()), _s_in.length()-(last_pos-_s_in.toConstChar()));
		elems.push(String(token));

		return elems;
	}

	Vector<String> String::split(const String & _delim) const {
		Vector<String> elems;
		String s = *this;
		int pos = 0;
		String token;
		while ((pos = s.find(_delim)) != String::npos) {
			token = s.substr(0, pos);
			elems.push(String(token));
			s.erase(0, pos + _delim.length());
		}

		elems.push(String(s));
		return elems;
	}


	String String::toLowerCase() const{

		String ret = *this;
		for(int short l = 0; l < ret.length();l++){
			ret[l] = Character::toLowerCase(ret[l]);
		}
		return ret;
	}

	String String::toUpperCase() const{

		String ret = *this;
		for(int short l = 0; l < ret.length();l++){
			ret[l] = Character::toUpperCase(ret[l]);
		}
		return ret;
	}

	bool String::endsWith(const String & ending){
		size_t len_str=this->length();
		size_t len_end_str=ending.length();
		if(len_end_str<=len_str){
			const char *p1=this->toConstChar()+len_str-len_end_str;
			const char *p2=ending.toConstChar();
			return strcmp(p1,p2)==0;
		}

		return false;
	}

	bool String::startsWith(const String & starting){
		if (this->length() >= starting.length()) {
			return strncmp(this->toConstChar(),starting.toConstChar(),starting.length())==0;
		}
		return false;
	}

	/*int String::count(const String & s,char c){
		int n_items=0;

		for(int i=0; i < s.length(); i++)
			if(s[i] == c)
				n_items++;

		return n_items;
	}*/

	bool String::contains(const String & _str_containts,StringComparer sc){

		String s1=*this;
		String s2=_str_containts;

		if(sc==StringComparer::OrdinalIgnoreCase){
			s1=s1.toLowerCase();
			s2=s2.toLowerCase();
		}

		if (s1.find(s2) != String::npos) {
			return true;
		}
		return false;
	}

	String String::substring(int _start_index, int _end_index){
		String out_string="";
		if(_start_index<0){
			ZS_THROW_RUNTIME_ERROR("_start_index negative (_start_index:%i)", _start_index);
		}
		if(_end_index < 0){
			_end_index=this->length()+_end_index;
		}
		if(_end_index>=this->length()){
			_end_index=this->length()-1;
		}

		if(_start_index<=_end_index){
			out_string=this->substr(_start_index,_end_index-_start_index+1);
		}

		return out_string;
	}


	String String::unescape()const{
		String res;
		char *it = (char *)this->toConstChar();
		char *end= (char *)this->toConstChar()+this->length();
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

	String String::replace(const String & str_old, const String & str_new){
		String str = *this;
		int	idx_current_pos=0;
		char *current_pos=NULL;

		while((current_pos = strstr((char*)str.toConstChar()+idx_current_pos,str_old.toConstChar())) != NULL) {
			idx_current_pos=current_pos-str.toConstChar();
			str.replace(idx_current_pos, str_old.length(), str_new);
			idx_current_pos += str_new.length(); // Handles case where 'str_new' is a substring of 'str_old'
		}

		return str;
	}

	String String::substr (int _pos, int _len) const{
		String s;
		if(_len == npos){
			_len=count-_pos;
		}

		if(_len < 0){
			ZS_THROW_RUNTIME_ERRORF("substring: negative _len");
		}

		if((_pos+_len) > count){
			ZS_THROW_RUNTIME_ERROR("substring: pos+len >= size (%i+%i>=%i)",_pos,_len,count);
		}

		char *str_cut=(char *)ZS_MALLOC(_len+1);
		strncpy(str_cut,this->buf+_pos,_len);

		s=str_cut;

		free(str_cut);

		return s;
	}

	String & String::replace(int _pos, int _len, const String & _to_replace){

		if(_pos>=this->count) ZS_THROW_RUNTIME_ERROR("insert(int,const String &): _pos(%i) >= size(%i)",_pos,count);

		int new_size = count + (_to_replace.count-_len);

		if(new_size<=0) ZS_THROW_RUNTIME_ERRORF("replace(int , int , const String & ): new_size <= 0");

		char *new_buf = (char *)ZS_MALLOC(new_size + 1); // allocate memory to keep the concatenated string

		if(_pos > 0){
			strncpy(new_buf, buf, _pos); // copy the 1st string
		}

		strcat(new_buf+_pos,_to_replace.toConstChar());

		strcpy(new_buf+_pos+_to_replace.count, buf+_pos+_len);

		free(buf);

		buf=new_buf;
		count=_size=new_size;

		return *this;
	}

	void String::erase(int _pos){
		erase(_pos,1);
	}

	void String::insert(int _pos, char _c){
		if(_pos>=this->count) ZS_THROW_RUNTIME_ERROR("insert(int,char): _pos(%i) >= size(%i)",_pos,count);

		int new_size = count + 1;
		char *new_buf = (char *)ZS_MALLOC(new_size + 1); // allocate memory to keep the concatenated string

		if(_pos > 0){
			strncpy(new_buf, buf, _pos); // copy the 1st string
		}

		// set char
		*(new_buf+_pos)=_c;

		// copy last string
		strcpy(new_buf+_pos+1, buf+_pos);

		free(buf);

		buf=new_buf;
		count=_size=new_size;
	}

	void String::insert(int _pos, const String & _s1){
		if(_pos>=this->count) ZS_THROW_RUNTIME_ERROR("insert(int,const String &): _pos(%i) >= size(%i)",_pos,count);

		int new_size = count + _s1.count;
		char *new_buf = (char *)ZS_MALLOC(new_size + 1); // allocate memory to keep the concatenated string

		if(_pos > 0){
			strncpy(new_buf, buf, _pos); // copy the 1st string
		}
		strcat(new_buf+_pos,_s1.toConstChar());

		strncpy(new_buf+_pos+_s1.count, buf+_pos, count-_pos);

		free(buf);

		buf=new_buf;
		count=_size=new_size;
	}

	void String::erase(int _pos, int _len){

		if(_pos>=this->count) ZS_THROW_RUNTIME_ERROR("erase: _pos(%i) >= size(%i)",_pos,count);
		if((_pos+_len)>this->count) ZS_THROW_RUNTIME_ERROR("erase: _pos(%i)+_len(%i) >= size(%i)",_pos,_len,count);

		int new_size=count-_len;

		// it erases last element
		char *new_buf=(char *)ZS_MALLOC(new_size*sizeof(char)+1);

		// 1st copy
		if(_pos>0){
			memcpy(new_buf,buf,_pos);
		}

		// copy last span
		memcpy(new_buf+_pos, buf+_pos+_len,count-_len-_pos);

		free(buf);
		_size=count=new_size;
		buf=new_buf;

	}

	int String::find(const char *_s, int pos) const{

		int len=(int)strlen(_s);

		if((pos<count) && (len <= count)){

			int idx=0;

			do{
				bool contains=true;
				for(int i=0; (i < len) && (contains==true); i++){ // search for all chars
					if(_s[i]!=buf[idx+i]){
						contains=false;
					}
				}

				if(contains==true){
					return idx;
				}

				// next span
				idx++;

			}while(idx<count);
		}

		return npos;
	}

	int String::find(const String &_s, int pos) const{
		return find(_s.toConstChar(),pos);
	}

	int String::findLastOf(const char *_s, int pos) const{

		int len=(int)strlen(_s);

		if(pos == npos){
			pos=count-1;
		}

		if(pos < count){
			int idx=ZS_MIN(pos,count-1);

			do{

				char c=buf[idx];
				for(int i=0; i < len; i++){ // search for all chars
					if(_s[i]==c){
						return idx;
					}
				}
			}while(--idx>=0);
		}

		return npos;
	}

	int String::findLastOf(const String & _str, int pos) const{
		return findLastOf(_str.toConstChar(),pos);
	}

	void String::append(const char *_buf, int _len){
		int start=count;
		inc_slots(_len);
		memcpy(buf+start,_buf,_len);
	}

	void String::append(const String &_s){
		String::append(_s.buf);
	}

	void String::append(const char *_s){
		if(_s==NULL){
			ZS_THROW_RUNTIME_ERRORF("input string null");
		}
		String::append(_s,(int)strlen(_s));
	}

	void String::append(char _c){
		inc_slots(1);
		//buf=(char *)rea0loc(buf,size+1);
		buf[count]=0;
		buf[count-1]=_c;
	}

	void String::__cleanup__()
	{
		if (buf != NULL)
		{
			free(buf);
		}
		count = _size = 0;
		buf = NULL;
	}

	String::~String() // destructor
	{
		__cleanup__();
	}



}
