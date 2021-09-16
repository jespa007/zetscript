#include "zetscript.h"

namespace zetscript{


	zs_string::zs_string() : buf(nullptr), size(0) // default constructor
	{
	}

	zs_string::zs_string(const char * const buffer) // constructor
	{
		size = strlen(buffer);
		buf = new char[size + 1]; // + 1 for the keeping the null character
		strncpy_s(buf, size + 1, buffer, size); // copy from the incoming buffer to character buffer of the new object
	}

	zs_string::zs_string(const zs_string & obj) // copy constructor
	{
		size = obj.size;
		buf = new char[size + 1]; // + 1 for the keeping the null character
		strncpy_s(buf, size + 1, obj.buf, size); // copy from the incoming buffer to character buffer of the new object
	}

	zs_string& zs_string::operator=(const zs_string & obj) // copy assignment
	{
		__cleanup__(); // cleanup any existing data

		// Copy data from the newly assigned zs_string object
		size = obj.size;
		buf = new char[size + 1]; // + 1 for the keeping the null character
		strncpy_s(buf, size + 1, obj.buf, size); // copy from the incoming buffer to character buffer of the new object
		return *this;
	}

	zs_string::zs_string(zs_string && dyingObj) // move constructor
	// && is a reference operator defined in the C++11 standard
	// which means "dyingObj" is an r-value reference.
	// Compiler calls this constructor when the object passed in the argument
	// is about to die due to scope end or such
	{
		__cleanup__(); // cleanup any existing data

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

	zs_string zs_string::operator+(const zs_string & obj) // concatenation
	{
		zs_string s; // create a new string named 's'
		s.size = this->size + obj.size;
		s.buf = new char[s.size + 1]; // allocate memory to keep the concatenated string
		strncpy_s(s.buf, this->size + 1, this->buf, this->size); // copy the 1st string
		strncpy_s(s.buf + this->size, obj.size + 1, obj.buf, obj.size);

		return s;
	}

	unsigned int zs_string::length()
	{
		return size;
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
		if (buf != nullptr)
		{
			delete[] buf;
		}
		size = 0;
	}

}
