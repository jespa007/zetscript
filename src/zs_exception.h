/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "util/zs_buffer.h"
#include "util/zs_string.h"

#define THROW_RUNTIME_ERROR(_str_error, ...)							::zetscript::throw_script_error(__FILENAME__,__LINE__,_str_error, __VA_ARGS__)
#define THROW_RUNTIME_ERRORF(_str_error)								THROW_RUNTIME_ERROR(_str_error,NULL)

#define THROW_SCRIPT_ERROR_FILE_LINE(file,line,_str_error, ...)			::zetscript::throw_script_error(file,line,_str_error, __VA_ARGS__)
#define THROW_SCRIPT_ERROR_FILE_LINEF(file,line,_str_error)				THROW_SCRIPT_ERROR_FILE_LINE(file,line,_str_error,NULL)

#define THROW_SCRIPT_ERROR 												::zetscript::throw_script_error
#define THROW_EXCEPTION_FILE_LINE										::zetscript::throw_exception_file_line
#define THROW_EXCEPTION													::zetscript::throw_exception

namespace zetscript {

	class zs_exception: public std::exception{
			const char *error_type;

			char 	file[512];
			int	   line;
			zs_string	error_description;
			zs_string 	what_msg;
		public:

			zs_exception(const char *   _file, int _line, const zs_string & _error_description, const char *_error_type);
			virtual const char* what() const noexcept;

			int getErrorLine();

			const char * getErrorDescription();

			const char * getErrorSourceFilename();

	};

	class zs_exception_error: public zs_exception{
	public:

		zs_exception_error(const char *  _file, int _line, const zs_string & _error);
	};

	void throw_script_error(const char * scrip_filename, int script_line, const char *in_txt,...);
	void throw_exception_file_line(const char *filename, int line,const char *error,...);
	void throw_exception(const char  * error,...);


}
