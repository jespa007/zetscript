/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

namespace zetscript {

	Exception::Exception(const char *  _file, int _line, const String & _what){
		error_type="ERR";
		line=_line;
		file="";
		what_msg = _what;

		if(((_file==0 || *_file==0))==false){
			file=Path::getFilename(_file);
		}
	}

	const char* Exception::what() const noexcept{
		return what_msg.toConstChar();
	}

	int Exception::getLine(){
		return line;
	}

	const char * Exception::getFilename(){
		return this->file.toConstChar();
	}

	//ExceptionError::ExceptionError(const char * _file, int _line, const String & _error):Exception(_file,  _line, _error,"ERR"){}

	void throw_exception_file_line(const char *_file, int _line, const char *in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER]={0};
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);
		throw Exception(_file,_line,out_txt);
	}

	void throw_exception(const char * in_txt,...){
		char out_txt[ZS_MAX_STR_BUFFER]={0};
		ZS_CAPTURE_VARIABLE_ARGS(out_txt,in_txt);
		throw Exception("",ZS_UNDEFINED_IDX,out_txt);
	}
}
