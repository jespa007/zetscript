/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_THROW_RUNTIME_ERROR(_str_error, ...)							::zetscript::throw_exception_file_line(__FILENAME__,__LINE__,_str_error, __VA_ARGS__)
#define ZS_THROW_RUNTIME_ERRORF(_str_error)								ZS_THROW_RUNTIME_ERROR(_str_error,NULL)

#define ZS_THROW_EXCEPTION_FILE_LINE									::zetscript::throw_exception_file_line
#define ZS_THROW_EXCEPTION												::zetscript::throw_exception

namespace zetscript {
	class Exception: public std::exception{
		public:

			Exception(const char *   _file, int _line, const String & _error_description);
			virtual const char* what() const noexcept;

			int getLine();

			//const char * getDescription();

			const char * getFilename();
		private:
			const char *error_type;

			String	file;
			int	   		line;
			//String	error_description;
			String 	what_msg;
			String 	stk_trace;
	};

	/*class ExceptionError: public Exception{
	public:

		ExceptionError(const char *  _file, int _line, const String & _what);
	};*/

	void throw_exception_file_line(const char *filename, int line,const char *_what,...);
	void throw_exception(const char  * error,...);


}
