/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript {

	zs_exception::zs_exception(const char *  _file, int _line, const zs_string & _error_description, const char *_error_type){
		error_type=_error_type;
		strcpy(file,_file);
		line=_line;
		error_description = _error_description;
		if((_file==0 || *_file==0) && _line == ZS_IDX_UNDEFINED){
			what_msg=zs_strutils::format("%s", _error_description.c_str());
		}else if((_file==0 || *_file==0)){
			what_msg=zs_strutils::format("line %i: %s", _line, _error_description.c_str());
		}else{
			what_msg=zs_strutils::format("[%s:%i] %s",_file, _line, _error_description.c_str());
		}

	}

	const char* zs_exception::what() const noexcept
	{
		return what_msg.c_str();
	}


	int zs_exception::getErrorLine(){
		return line;
	}

	const char * zs_exception::getErrorDescription(){
		return this->error_description.c_str();
	}

	const char * zs_exception::getErrorSourceFilename(){
		return this->file;
	}

	zs_exception_error::zs_exception_error(const char * _file, int _line, const zs_string & _error):zs_exception(_file,  _line, _error,"ERR"){}


	void throw_script_error(const char * _file, int _line, const char *_str_in,...){
		char str_out[ZS_MAX_STR_BUFFER]={0};
		ZS_CAPTURE_VARIABLE_ARGS(str_out,_str_in);

		throw zs_exception_error(_file,_line,str_out);
	}

	void throw_runtime_error(const char *in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER]={0};
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);
		throw zs_exception_error("",-1,out_txt);
	}

	void throw_exception_file_line(const char *_file, int _line, const char *in_txt,...){
		//char aux[ZS_MAX_STR_BUFFER];
		//sprintf(aux,"[%s:%i] ",_file,_line);
		zs_string aux=zs_strutils::format("[%s:%i] ",_file,_line);

		char out_txt[ZS_MAX_STR_BUFFER]={0};
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);

		aux+=out_txt;

		throw zs_exception_error(_file,_line,aux);
	}

	void throw_exception(const char * in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER]={0};
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);
		throw zs_exception_error("",ZS_IDX_UNDEFINED,out_txt);
	}


}
