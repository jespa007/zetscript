/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript {



#ifdef __EMSCRIPTEN__
#define THROW_RUNTIME_ERROR cerr <<
#define THROW_SCRIPT_ERROR cerr <<
#else
#define THROW_RUNTIME_ERROR std::runtime_error
#define THROW_SCRIPT_ERROR throw_script_error
#endif

class script_exception: public std::exception
	{
		const char *error_type;

		string file;
		int	   line;
		string	error_description;

		char what_msg[4096];
	public:

		script_exception(const char *  _file, int _line, const char * _error_description, const char *_error_type);

	    virtual const char* what() const throw();

	};



	class script_error: public script_exception{
	public:

		script_error(const char *  _file, int _line, const char * _error);


	};

	void throw_script_error();

};
