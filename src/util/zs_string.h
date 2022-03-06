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

		zs_string& operator=(const zs_string & obj);
		zs_string& operator+=(const zs_string& rhs);
		zs_string& operator+=(char _c);

		zs_string(zs_string && dying_obj);

		zs_string& operator=(zs_string && dying_obj);


	    char& operator[] (int pos);
	    const char& operator[] (int pos) const;

	    char& at (int pos);
	    const char& at (int pos) const;

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

		int find(const zs_string &_s, int pos = 0) const;
		int find(const char *_s, int pos = 0) const;
		int find_last_of(const char *_s, int pos = npos) const;

		zs_string replace(int _pos, int _len, const zs_string & to_replace);
		void append(const char *buf, int _len=npos);
		void append(char _c);
		void append(const zs_string & _s);

		void erase(int _pos, int _len);
		void erase(int _pos);
		void insert(int _pos, char _c);

		bool empty() const;

		int length() const;

		const char * c_str() const;

		~zs_string();

	private:
		char * buf = NULL;
		int size = 0;
		static zs_string newFromTwo(const char *_s1,const char *_s2);

		void __cleanup__();
	};

}
