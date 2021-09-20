#include "zetscript.h"

namespace zetscript{

	size_t zs_string::npos=-1;


	zs_string::zs_string() : buf(NULL), size(0) // default constructor
	{
	}

	zs_string::zs_string(const char * const buffer) // constructor
	{
		size = strlen(buffer);
		buf = (char *)malloc(size + 1); // + 1 for the keeping the null character
		strncpy_s(buf, size + 1, buffer, size); // copy from the incoming buffer to character buffer of the new object
	}

	zs_string::zs_string(const zs_string & obj) // copy constructor
	{
		size = obj.size;
		buf = (char *)malloc(size + 1); // + 1 for the keeping the null character
		strncpy_s(buf, size + 1, obj.buf, size); // copy from the incoming buffer to character buffer of the new object
	}

	zs_string& zs_string::operator=(const zs_string & obj) // copy assignment
	{
		__cleanup__(); // cleanup any existing data

		// Copy data from the newly assigned zs_string object
		size = obj.size;
		buf = (char *)malloc(size + 1); // + 1 for the keeping the null character
		strncpy_s(buf, size + 1, obj.buf, size); // copy from the incoming buffer to character buffer of the new object
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

    char& zs_string::operator[] (size_t pos){

    	if(pos>=size) THROW_EXCEPTION("zs_string::access out of bounds");

    	return buf[pos];
    }

    const char& zs_string::operator[] (size_t pos) const{
    	if(pos>=size) THROW_EXCEPTION("zs_string::access out of bounds");

    	return buf[pos];
    }

    zs_string zs_string::newFromTwo(const char *_s1, const char *_s2) {
		zs_string s; // create a new string named 's'
		size_t len1=strlen(_s1);
		size_t len2=strlen(_s2);

		s.size = len1 + len2;
		s.buf = (char *)malloc(s.size + 1); // allocate memory to keep the concatenated string
		strncpy(s.buf, _s1, len1); // copy the 1st string
		strncpy(s.buf + len1, _s2, len2);

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

	zs_string zs_string::substr (size_t pos, size_t len) const{

		if(len == npos){
			len=pos+size-len;
		}

		if((pos+len) >= size){
			THROW_RUNTIME_ERROR("substring: pos+len >= size (%i+%i>=%i)",pos,len,size);
		}

		zs_string str;
		str.buf=(char *)malloc(len+1);
		str.size=len;
		strncpy(str.buf,this->buf+pos,len);
		return str;


	}

	zs_string zs_string::replace(size_t _pos, size_t _len, const zs_string & to_replace){
		zs_string str;

		return str;
	}

	void zs_string::erase(size_t _pos, size_t _len){

		/*size_t end_pos=size;

		if(_len != npos){
			end_pos=_pos+_len;
		}

		size_t strlen_end_cpy=size-(_pos+_len);

		if((_pos+_len) >= size){
			THROW_RUNTIME_ERROR("erase: pos+len >= size (%i+%i>=%i)",_pos,_len,size);
		}

		size_t new_size=this->size-_len;
		char *new_buf=(char *)malloc(new_size+1);
		if(_pos > 0){
			strncpy(new_buf,buf,_pos);
		}

		if(end_pos!=this->size-1){
			strncpy(new_buf,buf+end_pos,strlen_end_cpy);
		}

		__cleanup__(); // cleanup any existing data

	    this->buf=new_buf;
	    this->size=new_size;*/
	}

	size_t zs_string::find(const char *_s, size_t pos) const{

		if(pos<size){
			const char *f=strstr(buf+pos,_s);
			if(f!=NULL){
				return f-buf;
			}
		}

		return npos;
	}
	size_t zs_string::find(const zs_string &_s, size_t pos) const{
		return find(_s.c_str(),pos);
	}

	size_t zs_string::find_last_of(const char *_s, size_t pos) const{

		if(pos < size){
			int idx=MIN(pos,size-1);

			do{
				size_t len=strlen(_s);
				char c=buf[idx];
				for(unsigned i=0; i < len; i++){ // search for all chars
					if(_s[i]==c){
						return idx;
					}
				}
			}while(--idx>=0);
		}

		return npos;
	}

	void zs_string::append(const char *_buf, size_t _len){

		size_t len=_len;
		if(len == npos){
			len=strlen(_buf);
		}

		size_t new_size=this->size+len;
		char *new_buf=(char *)malloc(new_size+1);
		sprintf(new_buf,"%s%s",this->buf,_buf);

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

	size_t zs_string::length() const
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
	}

}
