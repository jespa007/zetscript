/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class zs_string
	{
	public:
		static int npos;

		zs_string();
		zs_string(const char * buffer);
		zs_string(const zs_string & obj);
		zs_string(zs_string && _str_tmp);

		zs_string& operator=(const zs_string & obj);
		zs_string& operator=(zs_string && _str_tmp);
		zs_string& operator=(const char* buffer);

		zs_string& operator+=(const zs_string& rhs);
		zs_string& operator+=(const char* buffer);
		zs_string& operator+=(char _c);

		


	    char& operator[] (int pos);
	    const char& operator[] (int _pos) const;

	    char& at (int pos);
	    const char& at (int _pos) const;

	    void clear();


	    zs_string  substr (int pos = 0, int len = npos) const;

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

		int find(const zs_string &_s, int _pos = 0) const;
		int find(const char *_s, int _pos = 0) const;
		int find_last_of(const char *_s, int _pos = npos) const;

		zs_string & replace(int _pos, int _len, const zs_string & _to_replace);
		void append(const char *buf, int _len);
		void append(char _c);
		void append(const zs_string & _s);
		void append(const char * _s);

		void erase(int _pos, int _len);
		void erase(int _pos);
		void insert(int _pos, char _c);
		void insert(int _pos, const zs_string & _s1);

		bool empty() const;

		int length() const;

		const char * c_str() const;

		~zs_string();

	private:
		char * buf = NULL;
		int count=0;
		int _size = 0;
		static zs_string new_from_two(const char *_s1,const char *_s2);

		void set(const zs_string & _s);
		void set(const char * _s);
		void inc_slots(int _n_slots);
		void __cleanup__();
	};

}
