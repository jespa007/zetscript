/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define ZS_STRING_MAX_ELEMENTS			2000000
#define ZS_STRING_EXPAND_CHAR_ELEMENTS 	16

namespace zetscript{

	int zs_string::npos=-1;

	void	zs_string::inc_slots(int _n_slots){
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

	void zs_string::set(const char * _const_char, int _length){

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


	void zs_string::set(const zs_string & _s){
		set(_s.c_str());
	}

	zs_string::zs_string() // default constructor -> empty
	{
		buf=NULL;
		_size=0;
		count=0;
		//set("");
	}

	zs_string::zs_string(const char * _const_char) // constructor
	{
		buf=NULL;
		count=_size=0;
		set(_const_char);
	}

	zs_string::zs_string(const zs_string & _obj) // copy constructor
	{
		buf=NULL;
		count=_size=0;
		set(_obj);
	}

	zs_string::zs_string(zs_string && _str_tmp) // move constructor
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

	zs_string& zs_string::operator=(const zs_string & _str) {
		set(_str);
		return *this;
	}

	zs_string& zs_string::operator=(zs_string && _str_tmp){ // move assignment

		__cleanup__(); // cleanup any existing data

		// Copy data from the incoming object
		_size = _str_tmp._size;
		count = _str_tmp.count;

		// Transfer ownership of underlying char buffer from incoming object to this object
		buf = _str_tmp.buf;
		_str_tmp.buf = nullptr;

		return *this;
	}


	zs_string& zs_string::operator=(const char *_str) // copy assignment
	{
		set(_str);
		return *this;
	}

	zs_string& zs_string::operator+=(const zs_string& obj){
		append(obj.buf);
	    return *this;
	}

	zs_string& zs_string::operator+=(const char *_str) {
		append(_str);
		return *this;
	}

	zs_string& zs_string::operator+=(char _c){
		append(_c);
	    return *this;
	}

    char& zs_string::operator[] (int _pos){

    	if(_pos < 0 || _pos >= this->count) ZS_THROW_EXCEPTION("zs_string::access out of bounds");

    	return buf[_pos];
    }

    const char& zs_string::operator[] (int _pos) const{
    	if(_pos < 0 || _pos >= this->count) ZS_THROW_EXCEPTION("zs_string::access out of bounds");

    	return buf[_pos];
    }

    char& zs_string::at (int _pos){
    	if(_pos < 0 || _pos >= this->count) ZS_THROW_EXCEPTION("zs_string::access out of bounds");

    	return buf[_pos];
    }

    const char& zs_string::at (int _pos) const{
    	if(_pos < 0 || _pos >= this->count) ZS_THROW_EXCEPTION("zs_string::access out of bounds");

    	return buf[_pos];
    }

    void zs_string::clear(){
    	__cleanup__();
    }

    zs_string zs_string::new_from_two(const char *_s1, const char *_s2) {
		zs_string s; // create a new string named 's'
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
    zs_string  operator+(const zs_string & _s1, const zs_string &_s2) // concatenation
	{
		return zs_string::new_from_two(_s1.buf,_s2.buf);
	}

	zs_string  operator+(const char * _s1, const zs_string &_s2) // concatenation
	{
		return zs_string::new_from_two(_s1,_s2.buf);
	}

	zs_string operator+(const zs_string & _s1, const char * _s2) // concatenation
	{
		return zs_string::new_from_two(_s1.buf,_s2);
	}

	zs_string operator+(const zs_string & _s1, char _s2){
		char tmp[]={_s2,0};
		return zs_string::new_from_two(_s1.buf,tmp);
	}

	zs_string operator+(char  _s1, const zs_string & _s2){
		char tmp[]={_s1,0};
		return zs_string::new_from_two(tmp,_s2.buf);
	}

	// ==
	bool operator==(const zs_string & _s1, const zs_string &_s2){
		// both are not empty
		return strcmp(_s1.c_str(),_s2.c_str())==0;
	}

	bool operator==(const char * _s1, const zs_string & _s2){
		if(_s1==NULL){
			ZS_THROW_RUNTIME_ERRORF("operator '==' : operand 1 == NULL");
		}

		return strcmp(_s1,_s2.c_str())==0;
	}

	bool operator==(const zs_string & _s1, const char *_s2){
		if(_s2==NULL){
			ZS_THROW_RUNTIME_ERRORF("operator '==' : operand 2 == NULL");
		}

		return strcmp(_s1.c_str(),_s2)==0;
	}

	// !=
	bool operator!=(const zs_string & _s1, const zs_string &_s2){
		return strcmp(_s1.c_str(),_s2.c_str())!=0;
	}

	bool operator!=(const char * _s1, const zs_string & _s2){
		if(_s1==NULL){
			ZS_THROW_RUNTIME_ERRORF("operator '!=' : operand 1 == NULL");
		}

		return strcmp(_s1,_s2.c_str())!=0;
	}

	bool operator!=(const zs_string & _s1, const char *_s2){
		if(_s2==NULL){
			ZS_THROW_RUNTIME_ERRORF("operator '!=' : operand 2 == NULL");
		}

		return strcmp(_s1.c_str(),_s2)!=0;
	}

	zs_string zs_string::substr (int _pos, int _len) const{
		zs_string s;
		if(_len == npos){
			_len=count-_pos;
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

	zs_string & zs_string::replace(int _pos, int _len, const zs_string & _to_replace){

		if(_pos>=this->count) ZS_THROW_RUNTIME_ERROR("insert(int,const zs_string &): _pos(%i) >= size(%i)",_pos,count);

		int new_size = count + (_to_replace.count-_len);

		if(new_size<=0) ZS_THROW_RUNTIME_ERRORF("replace(int , int , const zs_string & ): new_size <= 0");

		char *new_buf = (char *)ZS_MALLOC(new_size + 1); // allocate memory to keep the concatenated string

		if(_pos > 0){
			strncpy(new_buf, buf, _pos); // copy the 1st string
		}

		strcat(new_buf+_pos,_to_replace.c_str());

		strcpy(new_buf+_pos+_to_replace.count, buf+_pos+_len);

		free(buf);

		buf=new_buf;
		count=_size=new_size;

		return *this;
	}

	void zs_string::erase(int _pos){
		erase(_pos,1);
	}

	void zs_string::insert(int _pos, char _c){
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

	void zs_string::insert(int _pos, const zs_string & _s1){
		if(_pos>=this->count) ZS_THROW_RUNTIME_ERROR("insert(int,const zs_string &): _pos(%i) >= size(%i)",_pos,count);

		int new_size = count + _s1.count;
		char *new_buf = (char *)ZS_MALLOC(new_size + 1); // allocate memory to keep the concatenated string

		if(_pos > 0){
			strncpy(new_buf, buf, _pos); // copy the 1st string
		}
		strcat(new_buf+_pos,_s1.c_str());

		strncpy(new_buf+_pos+_s1.count, buf+_pos, count-_pos);

		free(buf);

		buf=new_buf;
		count=_size=new_size;
	}

	void zs_string::erase(int _pos, int _len){

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

	int zs_string::find(const char *_s, int pos) const{

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

	int zs_string::find(const zs_string &_s, int pos) const{
		return find(_s.c_str(),pos);
	}

	int zs_string::find_last_of(const char *_s, int pos) const{

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

	void zs_string::append(const char *_buf, int _len){
		int start=count;
		inc_slots(_len);
		memcpy(buf+start,_buf,_len);
	}

	void zs_string::append(const zs_string &_s){
		zs_string::append(_s.buf);
	}

	void zs_string::append(const char *_s){
		if(_s==NULL){
			ZS_THROW_RUNTIME_ERRORF("input string null");
		}
		zs_string::append(_s,(int)strlen(_s));
	}


	void zs_string::append(char _c){
		inc_slots(1);
		//buf=(char *)rea0loc(buf,size+1);
		buf[count]=0;
		buf[count-1]=_c;
	}

	bool zs_string::empty() const{
		return count==0;
	}


	const char * zs_string::c_str() const
	{
		return buf==NULL?"":buf;
	}

	void zs_string::__cleanup__()
	{
		if (buf != NULL)
		{
			free(buf);
		}
		count = _size = 0;
		buf = NULL;
	}

	zs_string::~zs_string() // destructor
	{
		__cleanup__();
	}



}
