/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript {

	zs_exception::zs_exception(const std::string &  _file, int _line, const char * _error_description, const char *_error_type){
		error_type=_error_type;
		file=_file;
		line=_line;
		error_description=_error_description;
		if(zs_strutils::is_empty(_file)){
			sprintf(what_msg,"line %i: %s", _line, (char *)error_description.c_str());
		}else{
			sprintf(what_msg,"[%s:%i] %s",zs_path::get_filename(_file).c_str(), _line, (char *)error_description.c_str());
		}

	}

	const char* zs_exception::what() const noexcept
	{
		return (const char *)what_msg;
	}


	int zs_exception::getErrorLine(){
		return line;
	}

	const std::string & zs_exception::getErrorDescription(){
		return this->error_description;
	}

	const std::string & zs_exception::getErrorSourceFilename(){
		return this->file;
	}


	zs_exception_error::zs_exception_error(const std::string &  _file, int _line, const char * _error):zs_exception(_file,  _line, _error,"ERR"){}


	void throw_script_error(const std::string & script_filename, int script_line, const char *in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);

		throw zs_exception_error(script_filename,script_line,out_txt);
	}

	/*void throw_runtime_error(const char *filename, int line, const char *in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);

		throw zs_exception_error(filename,line,out_txt);
	}*/



};
