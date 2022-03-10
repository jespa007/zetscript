/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	int zs_string::npos=-1;

	void zs_string::set(const char * _buffer){

		if(_buffer==NULL){return;} // do not create string from NULL pointers

		__cleanup__(); // cleanup any existing data
		size = strlen(_buffer);
		buf = (char *)ZS_MALLOC(size + 1); // + 1 for the keeping the null character
		strncpy(buf, _buffer, size); // copy from the incoming buffer to character buffer of the new object
	}


	void zs_string::set(const zs_string & _s){
		set(_s.c_str());
	}

	zs_string::zs_string() // default constructor -> empty
	{
		buf=NULL;
		size=0;
		set("");
	}

	zs_string::zs_string(const char * buffer) // constructor
	{
		buf=NULL;
		size=0;
		set(buffer);
	}

	zs_string::zs_string(const zs_string & obj) // copy constructor
	{
		buf=NULL;
		size=0;
		set(obj);
	}

	zs_string& zs_string::operator=(const zs_string & obj) // copy assignment
	{
		set(obj);
		return *this;
	}

	zs_string& zs_string::operator+=(const zs_string& obj){
		append(obj.buf);
	    return *this;
	}

	zs_string& zs_string::operator+=(char _c){
		append(_c);
	    return *this;
	}

	zs_string::zs_string(zs_string && dyingObj) // move constructor
	// && is a reference operator defined in the C++11 standard
	// which means "dyingObj" is an r-value reference.
	// Compiler calls this constructor when the object passed in the argument
	// is about to die due to scope end or such
	{
		// Copy data from the incoming object
		size = dyingObj.size;

		// Transfer ownership of underlying char buffer from incoming object to this object
		buf = dyingObj.buf;
		dyingObj.buf = nullptr;
	}

	zs_string& zs_string::operator=(zs_string && dyingObj) // move assignment
	{
		__cleanup__(); // cleanup any existing data

		// Copy data from the incoming object
		size = dyingObj.size;

		// Transfer ownership of underlying char buffer from incoming object to this object
		buf = dyingObj.buf;
		dyingObj.buf = nullptr;

		return *this;
	}

    char& zs_string::operator[] (int pos){

    	if(pos>=size) THROW_EXCEPTION("zs_string::access out of bounds");

    	return buf[pos];
    }

    const char& zs_string::operator[] (int pos) const{
    	if(pos>=size) THROW_EXCEPTION("zs_string::access out of bounds");

    	return buf[pos];
    }

    char& zs_string::at (int pos){
    	if(pos>=size) THROW_EXCEPTION("zs_string::at out of bounds");

    	return buf[pos];
    }

    const char& zs_string::at (int pos) const{
    	if(pos>=size) THROW_EXCEPTION("zs_string::at out of bounds");

    	return buf[pos];
    }

    void zs_string::clear(){
    	this->__cleanup__();
    }

    zs_string zs_string::newFromTwo(const char *_s1, const char *_s2) {
		zs_string s; // create a new string named 's'
		int len1=0;
		int len2=0;

		if(_s1!=NULL){
			len1=strlen(_s1);
		}

		if(_s2!=NULL){
			len2=strlen(_s2);
		}

		int buf_new_from_two_size = len1 + len2;
		char *buf_new_from_two = (char *)ZS_MALLOC(buf_new_from_two_size + 1); // allocate memory to keep the concatenated string

		if(_s1!=NULL) strncpy(buf_new_from_two, _s1, len1); // copy the 1st string
		if(_s2!=NULL) strncpy(buf_new_from_two + len1, _s2, len2);

		s=buf_new_from_two;

		free(buf_new_from_two);

		return s;


    }

    // +
    zs_string  operator+(const zs_string & _s1, const zs_string &_s2) // concatenation
	{
		return zs_string::newFromTwo(_s1.buf,_s2.buf);
	}

	zs_string  operator+(const char * _s1, const zs_string &_s2) // concatenation
	{
		return zs_string::newFromTwo(_s1,_s2.buf);
	}

	zs_string operator+(const zs_string & _s1, const char * _s2) // concatenation
	{
		return zs_string::newFromTwo(_s1.buf,_s2);
	}

	zs_string operator+(const zs_string & _s1, char _s2){
		char tmp[]={_s2,0};
		return zs_string::newFromTwo(_s1.buf,tmp);
	}

	zs_string operator+(char  _s1, const zs_string & _s2){
		char tmp[]={_s1,0};
		return zs_string::newFromTwo(tmp,_s2.buf);
	}

	// ==
	bool operator==(const zs_string & _s1, const zs_string &_s2){
		return strcmp(_s1.c_str(),_s2.c_str())==0;
	}

	bool operator==(const zs_string & _s1, const char *_s2){
		return strcmp(_s1.c_str(),_s2)==0;
	}

	bool operator==(const char * _s1, const zs_string & _s2){
		return strcmp(_s1,_s2.c_str())==0;
	}

	// !=
	bool operator!=(const zs_string & _s1, const zs_string &_s2){
		return strcmp(_s1.c_str(),_s2.c_str())!=0;
	}

	bool operator!=(const zs_string & _s1, const char *_s2){
		return strcmp(_s1.c_str(),_s2)!=0;
	}

	bool operator!=(const char * _s1, const zs_string & _s2){
		return strcmp(_s1,_s2.c_str())!=0;
	}

	zs_string zs_string::substr (int pos, int len) const{
		zs_string s;
		if(len == npos){
			len=size-pos;
		}

		if((pos+len) > size){
			THROW_RUNTIME_ERROR("substring: pos+len >= size (%i+%i>=%i)",pos,len,size);
		}

		char *str_cut=(char *)ZS_MALLOC(len+1);
		strncpy(str_cut,this->buf+pos,len);

		s=str_cut;

		free(str_cut);

		return s;


	}

	zs_string zs_string::replace(int _pos, int _len, const zs_string & _to_replace){
		ZS_UNUSUED_3PARAMS(_pos, _len, _to_replace);
		zs_string str;

		return str;
	}

	void zs_string::erase(int _pos){
		erase(_pos,1);
	}

	void zs_string::insert(int _pos, char _c){
		if(_pos>=this->size) THROW_RUNTIME_ERROR("erase: _pos(%i) >= size(%i)",_pos,size);

		int new_size = size + 1;
		char *new_buf = (char *)ZS_MALLOC(new_size + 1); // allocate memory to keep the concatenated string

		if(_pos > 0){
			strncpy(new_buf, buf+_pos, _pos); // copy the 1st string
		}
		*(new_buf+_pos)=_c;

		strncpy(new_buf+_pos+1, buf+_pos, size-_pos);

		free(buf);

		buf=new_buf;
		size=new_size;

	}

	void zs_string::erase(int _pos, int _len){

		if(_pos>=this->size) THROW_RUNTIME_ERROR("erase: _pos(%i) >= size(%i)",_pos,size);
		if((_pos+_len)>this->size) THROW_RUNTIME_ERROR("erase: _pos(%i)+_len(%i) >= size(%i)",_pos,_len,size);

		int new_size=size-_len;
		char *new_buf=(char *)ZS_MALLOC(new_size*sizeof(char)+1);

		// 1st copy
		if(_pos>0){
			memcpy(new_buf,buf,_pos);
		}

		// last
		memcpy(new_buf, buf+_pos+_len,size-_len-_pos);

		free(buf);

		size=new_size;
		buf=new_buf;


	}

	int zs_string::find(const char *_s, int pos) const{

		if(pos<size){
			const char *f=strstr(buf+pos,_s);
			if(f!=NULL){
				return f-buf;
			}
		}

		return npos;
	}
	int zs_string::find(const zs_string &_s, int pos) const{
		return find(_s.c_str(),pos);
	}

	int zs_string::find_last_of(const char *_s, int pos) const{

		if(pos == npos){
			pos=size-1;
		}

		if(pos < size){
			int idx=ZS_MIN(pos,size-1);

			do{
				int len=strlen(_s);
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

		int len=_len;
		if(len == npos){
			len=strlen(_buf);
		}

		size_t new_size=this->size+len;
		char *new_buf=(char *)ZS_MALLOC(new_size+1);

		strncpy(new_buf,this->buf,this->size);
		strncpy(new_buf+this->size,_buf,len);

		__cleanup__(); // cleanup any existing data

	    this->buf=new_buf;
	    this->size=new_size;
	}

	void zs_string::append(const zs_string &_s){
		zs_string::append(_s.buf);
	}

	void zs_string::append(char _c){
		size++;
		buf=(char *)realloc(buf,size+1);
		buf[size]=0;
		buf[size-1]=_c;
	}

	int zs_string::length() const
	{
		return size;
	}

	bool zs_string::empty() const{
		return size==0;
	}


	const char * zs_string::c_str() const
	{
		return buf;
	}

	zs_string::~zs_string() // destructor
	{
		__cleanup__();
	}

	void zs_string::__cleanup__()
	{
		if (buf != NULL)
		{
			free(buf);
		}
		size = 0;
		buf=NULL;
	}

}
