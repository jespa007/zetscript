/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "ZetScript.h"

namespace zetscript {

	int GetErrorLine();
	const char * GetErrorDescription();
	const char * GetErrorSourceFilename();


	namespace exception{


		void  WriteError(const char *filename, int line, const  char  *string_text, ...);
		/*int GetErrorLine();
		const char * GetErrorDescription();
		const char * GetErrorSourceFilename();*/


		ScriptException::ScriptException(const char *  _file, int _line, const char * _error_description, const char *_error_type){
			error_type=_error_type;
			file=_file;
			line=_line;
			error_description=_error_description;


			sprintf(what_msg,"[%s %s:%i] %s",error_type,_file, _line, (char *)error_description.c_str());
		}

	    const char* ScriptException::what() const throw()
		{

	    	return (const char *)what_msg;
	    	//return "["+file+":"+CZetJsonCppUtils::IntToString(line)+"]"+error;
		}



		ScriptExceptionError::ScriptExceptionError(const char *  _file, int _line, const char * _error):ScriptException(_file,  _line, _error,"ERR"){}




		void throw_script_error(){
			throw ScriptExceptionError(GetErrorSourceFilename(),GetErrorLine(),GetErrorDescription());
		}

	}

};
