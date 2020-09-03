/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

/*#ifdef __EMSCRIPTEN__
#define THROW_EXCEPTION					cerr <<
#define THROW_RUNTIME_ERROR(s,...)		cerr << std::string(zetscript::zs_strutils::format(s,__VA_ARGS__))
//#define THROW_ERROR 					cerr <<
//#define THROW_RUNTIME_ERROR 			cerr <<
#define THROW_SCRIPT_ERROR 				cerr <<
#else*/
//#define THROW_EXCEPTION					throw
#define THROW_RUNTIME_ERROR(s,...)		zetscript::exception::throw_runtime_error(__FILENAME__,__LINE__,s, ##__VA_ARGS__)
//#define THROW_RUNTIME_ERROR 			throw std::runtime_error
#define THROW_SCRIPT_ERROR 				zetscript::exception::throw_script_error
#define THROW_EXCEPTION(s)				throw std::runtime_error(s)
//#endif

namespace zetscript {

	namespace exception{

		class script_exception: public std::exception{
				const char *error_type;

				std::string file;
				int	   line;
				std::string	error_description;

				char what_msg[4096];
			public:

				script_exception(const char *  _file, int _line, const char * _error_description, const char *_error_type);
				virtual const char* what() const noexcept;

				int getErrorLine();

				const char * getErrorDescription();

				const char * getErrorSourceFilename();

		};

		class script_exception_error: public script_exception{
		public:

			script_exception_error(const char *  _file, int _line, const char * _error);
		};

		void throw_script_error(const char *scrip_filename, int script_line, const char *in_txt,...);
		void throw_runtime_error(const char *filename, int line, const char *in_txt,...);
	}
}
