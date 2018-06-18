/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript {

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



	class script_error_exception: public script_exception{
	public:

		script_error_exception(const char *  _file, int _line, const char * _error);


	};

	void throw_error();

};
