/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript {

	zs_exception::zs_exception(const char *  _file, int _line, const char * _error_description, const char *_error_type){
		error_type=_error_type;
		file=_file;
		line=_line;
		error_description=_error_description;
		if(_file==0||*_file==0){
			sprintf(what_msg,"line %i: %s", _line, (char *)error_description.c_str());
		}else{
			sprintf(what_msg,"[%s:%i]: %s",_file, _line, (char *)error_description.c_str());
		}

	}

	const char* zs_exception::what() const noexcept
	{
		return (const char *)what_msg;
	}


	zs_exception_error::zs_exception_error(const char *  _file, int _line, const char * _error):zs_exception(_file,  _line, _error,"ERR"){}


	void throw_script_error(const char *scrip_filename, int script_line, const char *in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);

		throw zs_exception_error(scrip_filename,script_line,out_txt);
	}

	/*void throw_runtime_error(const char *filename, int line, const char *in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);

		throw zs_exception_error(filename,line,out_txt);
	}*/



};
