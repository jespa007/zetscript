/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript {

	zs_exception::zs_exception(const char *  _file, int _line, const char * _error_description, const char *_error_type){
		error_type=_error_type;
		strcpy(file,_file);
		line=_line;
		strcpy(error_description,_error_description);
		if((_file==0 || *_file==0) && _line == ZS_IDX_UNDEFINED){
			sprintf(what_msg,"%s", error_description);
		}else if((_file==0 || *_file==0)){
			sprintf(what_msg,"line %i: %s", _line, error_description);
		}else{
			sprintf(what_msg,"[%s:%i] %s",_file, _line, error_description);
		}

	}

	const char* zs_exception::what() const noexcept
	{
		return (const char *)what_msg;
	}


	int zs_exception::getErrorLine(){
		return line;
	}

	const char * zs_exception::getErrorDescription(){
		return this->error_description;
	}

	const char * zs_exception::getErrorSourceFilename(){
		return this->file;
	}


	zs_exception_error::zs_exception_error(const char * _file, int _line, const char * _error):zs_exception(_file,  _line, _error,"ERR"){}


	void throw_script_error(const char * script_filename, int script_line, const char *in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);

		throw zs_exception_error(script_filename,script_line,out_txt);
	}

	void throw_runtime_error(const char *in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);
		throw zs_exception_error("",-1,out_txt);
	}

	void throw_exception_file_line(const char *filename, int line, const char *in_txt,...){
		char aux[ZS_MAX_STR_BUFFER];
		sprintf(aux,"[%s:%i] ",filename,line);

		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);

		strcat(aux,out_txt);

		throw zs_exception_error(filename,line,out_txt);
	}

	void throw_exception(const char * in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER];
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);
		throw zs_exception_error("",ZS_IDX_UNDEFINED,out_txt);
	}


}
