/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "util/zs_buffer.h"
#include "util/zs_string.h"

#define ZS_THROW_RUNTIME_ERROR(_str_error, ...)							::zetscript::throw_exception_file_line(__FILENAME__,__LINE__,_str_error, __VA_ARGS__)
#define ZS_THROW_RUNTIME_ERRORF(_str_error)								ZS_THROW_RUNTIME_ERROR(_str_error,NULL)

#define ZS_THROW_SCRIPT_ERROR 											::zetscript::throw_exception_file_line
#define ZS_THROW_EXCEPTION_FILE_LINE									::zetscript::throw_exception_file_line
#define ZS_THROW_EXCEPTION												::zetscript::throw_exception

namespace zetscript {

	class zs_exception: public std::exception{
		public:

			zs_exception(const char *   _file, int _line, const zs_string & _error_description, const char *_error_type);
			virtual const char* what() const noexcept;

			int getLine();

			const char * getDescription();

			const char * getFilename();
		private:
			const char *error_type;

			zs_string	file;
			int	   		line;
			zs_string	error_description;
			zs_string 	what_msg;
			zs_string 	stk_trace;
	};

	class zs_exception_error: public zs_exception{
	public:

		zs_exception_error(const char *  _file, int _line, const zs_string & _error);
	};

	void throw_exception_file_line(const char *filename, int line,const char *error,...);
	void throw_exception(const char  * error,...);


}
