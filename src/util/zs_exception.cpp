/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript {

	zs_exception::zs_exception(const char *  _file, int _line, const zs_string & _what, const char *_error_type){
		error_type=_error_type;
		line=_line;
		file="";
		what_msg = _what;

		if(((_file==0 || *_file==0))==false){
			file=zs_path::get_filename(_file);
		}
	}

	const char* zs_exception::what() const noexcept{
		return what_msg.c_str();
	}

	int zs_exception::getLine(){
		return line;
	}

	const char * zs_exception::getFilename(){
		return this->file.c_str();
	}

	zs_exception_error::zs_exception_error(const char * _file, int _line, const zs_string & _error):zs_exception(_file,  _line, _error,"ERR"){}

	void throw_exception_file_line(const char *_file, int _line, const char *in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER]={0};
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);
		throw zs_exception_error(_file,_line,out_txt);
	}

	void throw_exception(const char * in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER]={0};
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);
		throw zs_exception_error("",ZS_UNDEFINED_IDX,out_txt);
	}
}
