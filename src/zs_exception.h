/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define THROW_RUNTIME_ERROR(s,...)							::zetscript::throw_script_error(__FILENAME__,__LINE__,s, ##__VA_ARGS__)
#define THROW_SCRIPT_ERROR_FILE_LINE(file,line,s,...)		::zetscript::throw_script_error(file,line,s, ##__VA_ARGS__)
#define THROW_SCRIPT_ERROR 									::zetscript::zs_exception::throw_script_error
#define THROW_EXCEPTION										::zetscript::throw_exception

namespace zetscript {

	class zs_exception: public std::exception{
			const char *error_type;

			zs_string file;
			int	   line;
			zs_string	error_description;

			char what_msg[ZS_MAX_STR_BUFFER];
		public:

			zs_exception(const zs_string &   _file, int _line, const char * _error_description, const char *_error_type);
			virtual const char* what() const noexcept;

			int getErrorLine();

			const zs_string & getErrorDescription();

			const zs_string & getErrorSourceFilename();

	};

	class zs_exception_error: public zs_exception{
	public:

		zs_exception_error(const zs_string &   _file, int _line, const char * _error);
	};

	void throw_script_error(const zs_string & scrip_filename, int script_line, const char *in_txt,...);
	void throw_exception(const char *in_txt,...);

}
