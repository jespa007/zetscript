#pragma once

namespace zetscript{

	class zs_string
	{
	public:
		zs_string();
		zs_string(const char * const buffer);

		zs_string(const zs_string & obj);

		zs_string& operator=(const zs_string & obj);

		zs_string(zs_string && dyingObj);

		zs_string& operator=(zs_string && dyingObj);

		zs_string operator+(const zs_string & obj);

		unsigned int length();

		const char * c_str() const;

		~zs_string();

	private:
		char * buf = nullptr;
		unsigned int size = 0;

		void __cleanup__();
	};

}
