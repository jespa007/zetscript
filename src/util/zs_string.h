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
		zs_string(const char * buffer);
		zs_string(const zs_string & obj);

		zs_string& operator=(const zs_string & obj);
		zs_string& operator+=(const zs_string& rhs);
		zs_string& operator+=(char _c);

		zs_string(zs_string && dying_obj);

		zs_string& operator=(zs_string && dying_obj);


	    char& operator[] (unsigned pos);
	    const char& operator[] (unsigned _pos) const;

	    char& at (unsigned pos);
	    const char& at (unsigned _pos) const;

	    void clear();


	    zs_string  substr (unsigned pos = 0, size_t len = npos) const;

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

		size_t find(const zs_string &_s, unsigned _pos = 0) const;
		size_t find(const char *_s, unsigned _pos = 0) const;
		size_t find_last_of(const char *_s, unsigned _pos = npos) const;

		zs_string & replace(unsigned _pos, size_t _len, const zs_string & _to_replace);
		void append(const char *buf, size_t _len=npos);
		void append(char _c);
		void append(const zs_string & _s);

		void erase(unsigned _pos, size_t _len);
		void erase(unsigned _pos);
		void insert(unsigned _pos, char _c);
		void insert(unsigned _pos, const zs_string & _s1);

		bool empty() const;

		size_t length() const;

		const char * c_str() const;

		~zs_string();

	private:
		char * buf = NULL;
		size_t count=0;
		size_t _size = 0;
		static zs_string new_from_two(const char *_s1,const char *_s2);

		void set(const zs_string & _s);
		void set(const char * _s);
		void inc_char_slot();
		void __cleanup__();
	};

}
