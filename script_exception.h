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


		script_exception(const char *  _file, int _line, const char * _error_description, const char *_error_type){
			error_type=_error_type;
			file=_file;
			line=_line;
			error_description=_error_description;

			sprintf(what_msg,"[%s %s:%i] %s",error_type,_file, _line, (char *)error_description.c_str());
		}

	    virtual const char* what() const throw()
		{

	    	return (const char *)what_msg;
	    	//return "["+file+":"+CZetJsonCppUtils::intToString(line)+"]"+error;
		}
	};


	class script_warning_exception: public script_exception{
		public:
		script_warning_exception(const char *  _file, int _line, const char * _error):script_exception(_file,  _line, _error,"WRN"){}
	};

	class script_error_exception: public script_exception{
	public:



		script_error_exception(const char *  _file, int _line, const char * _error):script_exception(_file,  _line, _error,"ERR"){}


	};
};
