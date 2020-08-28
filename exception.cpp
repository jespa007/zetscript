/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript {

	int getErrorLine();
	const char * getErrorDescription();
	const char * getErrorSourceFilename();


	namespace exception{

		script_exception::script_exception(const char *  _file, int _line, const char * _error_description, const char *_error_type){
			error_type=_error_type;
			file=_file;
			line=_line;
			error_description=_error_description;

			sprintf(what_msg,"[%s %s:%i] %s",error_type,_file, _line, (char *)error_description.c_str());
		}

	    const char* script_exception::what() const noexcept
		{
	    	return (const char *)what_msg;
		}


		script_exception_error::script_exception_error(const char *  _file, int _line, const char * _error):script_exception(_file,  _line, _error,"ERR"){}


		void throw_script_error(const char *scrip_filename, int script_line, const char *in_txt,...){
			char out_txt[ZS_MAX_VARG_OUTPUT_STRING];
			ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);

			throw script_exception_error(scrip_filename,script_line,out_txt);
		}

		void throw_runtime_error(const char *filename, int line, const char *in_txt,...){
			char out_txt[ZS_MAX_VARG_OUTPUT_STRING];
			ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);

			throw std::runtime_error(zs_strutils::format("[%s:%i] %s",zs_path::get_file_name(filename).c_str(),line,out_txt));
		}

	}

};
