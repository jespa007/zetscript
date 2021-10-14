/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class zs_string
	{
	public:
		static size_t npos;

		zs_string();
		zs_string(const char * const buffer);
		zs_string(const zs_string & obj);

		zs_string& operator=(const zs_string & obj);
		zs_string& operator+=(const zs_string& rhs);
		zs_string& operator+=(char _c);

		zs_string(zs_string && dyingObj);

		zs_string& operator=(zs_string && dyingObj);


	    char& operator[] (size_t pos);
	    const char& operator[] (size_t pos) const;

	    char& at (size_t pos);
	    const char& at (size_t pos) const;

	    void clear();


	    zs_string  substr (size_t pos = 0, size_t len = npos) const;

	    // +
		friend zs_string operator+(const zs_string & _s1, const zs_string &_s2);
		friend zs_string operator+(const zs_string & _s1, const char *_s2);
		friend zs_string operator+(const char * _s1, const zs_string & _s2);

		friend zs_string operator+(const zs_string & _s1, char _s2);
		friend zs_string operator+(char  _s1, const zs_string & _s2);

		// ==
		friend bool operator==(const zs_string & _s1, const zs_string &_s2);
		friend bool operator==(const zs_string & _s1, const char *_s2);
		friend bool operator==(const char * _s1, const zs_string & _s2);

		// !=
		friend bool operator!=(const zs_string & _s1, const zs_string &_s2);
		friend bool operator!=(const zs_string & _s1, const char *_s2);
		friend bool operator!=(const char * _s1, const zs_string & _s2);

		size_t find(const zs_string &_s, size_t pos = 0) const;
		size_t find(const char *_s, size_t pos = 0) const;
		size_t find_last_of(const char *_s, size_t pos = npos) const;

		zs_string replace(size_t _pos, size_t _len, const zs_string & to_replace);
		void append(const char *buf, size_t _len=npos);
		void append(char _c);
		void append(const zs_string & _s);

		void erase(size_t _pos, size_t _len);
		void erase(size_t _pos);
		void insert(size_t _pos, char _c);

		bool empty() const;

		size_t length() const;

		const char * c_str() const;

		~zs_string();

	private:
		char * buf = NULL;
		size_t size = 0;
		static zs_string newFromTwo(const char *_s1,const char *_s2);

		void __cleanup__();
	};

}
