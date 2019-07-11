/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript {
	namespace exception{

		void  write_error(const char *filename, int line, const  char  *string_text, ...);
		int getErrorLine();
		const char * getErrorDescription();
		const char * getErrorFilename();


		script_exception::script_exception(const char *  _file, int _line, const char * _error_description, const char *_error_type){
			error_type=_error_type;
			file=_file;
			line=_line;
			error_description=_error_description;


			sprintf(what_msg,"[%s %s:%i] %s",error_type,_file, _line, (char *)error_description.c_str());
		}

	    const char* script_exception::what() const throw()
		{

	    	return (const char *)what_msg;
	    	//return "["+file+":"+CZetJsonCppUtils::int_2_string(line)+"]"+error;
		}



		script_error::script_error(const char *  _file, int _line, const char * _error):script_exception(_file,  _line, _error,"ERR"){}




		void throw_script_error(){
			throw script_error(getErrorFilename(),getErrorLine(),getErrorDescription());
		}

	}

};
