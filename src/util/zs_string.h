#pragma once

namespace zetscript{

	class zs_string
	{
	public:
		static size_t npos=-1;

		zs_string();
		zs_string(const char * const buffer);

		zs_string(const zs_string & obj);

		zs_string& operator=(const zs_string & obj);
		zs_string& operator+=(const zs_string& rhs);
		zs_string& operator+=(char _c);

		zs_string(zs_string && dyingObj);

		zs_string& operator=(zs_string && dyingObj);

		zs_string operator+(const zs_string & obj);

	    char& operator[] (size_t pos);
	    const char& operator[] (size_t pos) const;

	    zs_string substr (size_t pos = 0, size_t len = npos) const;

		bool operator==(const zs_string & _s1, const zs_string &_s2);
		bool operator==(const zs_string & _s1, const char *_s2);
		bool operator==(const char * _s1, const zs_string & _s2);

		size_t find(const char *_s, size_t pos = 0) const;

		void append(const char *buf, size_t len);

		bool empty() const;

		size_t length() const;

		const char * c_str() const;

		~zs_string();

	private:
		char * buf = NULL;
		size_t size = 0;

		void __cleanup__();
	};

}
