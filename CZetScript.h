/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */



#pragma once


// MAIN INCLUDES....

#include          <stdlib.h>
#include          <memory.h>

#include          <stdio.h>
#include          <math.h>

#include          <vector>
#include          <stack>
#include 			<regex>
#include                  <new>
#include           <iostream>
#include <functional>
#include 		<sstream>
#include <string>
#include <cstring>
#include <list>
#include <utility>
#include <float.h>
#include <cstdarg>
#include <stdexcept>

#include <typeinfo>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <string>
#include <map>

#include <sstream>
#include <cstddef>

#include <type_traits>

#include <mutex>

#if defined(__GNUC__)
	#include <cxxabi.h>
	#include <dirent.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <memory.h>
	#include <fcntl.h>

	#ifdef _WIN32
		#include <windows.h>
		#include <stdio.h>
		#include <conio.h>
		#include <tchar.h>
	#else
		#include <dlfcn.h>
		#include <sys/ipc.h>
		#include <sys/shm.h>
		#include <sys/ioctl.h>
		#include <termios.h>
	#endif

#endif





using std::string;
using std::vector;
using std::map;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::stack;
using std::ostringstream;

// Prototypes & structs
#ifdef __MEMMANAGER__
#include "MEM_ram.h"
#endif





//#include "CSharedPointerManager.h"
#include "Common.h"
#include "zetscript_exception.h"
#include "CZetScriptUtils.h"
#include "var/CScriptVariable.h"
#include "var/CStringScriptVariable.h"
#include "var/CVectorScriptVariable.h"
#include "var/CFunctorScriptVariable.h"
#include "var/CStructScriptVariable.h"
#include "eval/CEval.h"
#include "CScope.h"
#include "CScopeFactory.h"
#include "CCommonClassFunctionData.h"
#include "CScriptFunction.h"
#include "CScriptFunctionFactory.h"
#include "CScriptClass.h"
#include "CScriptClassFactory.h"
#include "CVirtualMachine.h"

#define ZETSCRIPT_MAJOR_VERSION 2
#define ZETSCRIPT_MINOR_VERSION 0
#define ZETSCRIPT_PATCH_VERSION 0



#define CURRENT_VM	CZetScript::getInstance()->getVirtualMachine()



//#define ZS_WRITE_ERROR_MSG 		writeErrorMsg
//#define ZS_GET_ERROR_MSG		CZetScript::getErrorMsg
//#define ZS_CLEAR_ERROR_MSG		CZetScript::clearErrorMsg

//#define MAX_BUFFER_STR_ERROR	4096
#define MAX_BUFFER_AUX_TMP		4096


namespace zetscript{

	class CScriptVariable;
	class  CZetScript{


		//static char str_error[MAX_BUFFER_STR_ERROR];
		static CZetScript * m_instance;

		// calling C function with differnt parameters...
		CScriptVariable		*		m_mainObject;
		CScriptFunction 	*		m_mainFunction;
		//int						 	idxMainScriptFunctionObject;

		CVirtualMachine *vm;
		bool __init__;
		bool show_filename_on_error;





		void parse_ast(const char *str, int idx_filename=-1);



		//CScriptFunction m_structInfoMain;

		bool init();
		CZetScript();


		void destroyMainFunction();
		~CZetScript();

	public:




		ZETSCRIPT_MODULE_EXPORT static CZetScript * getInstance();
		//ZETSCRIPT_MODULE_EXPORT static void setVectorInfoParsedFiles(vector<tInfoParsedSource> * parsedFiles);




		//static void clearErrorMsg();
		//static void writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);
		//ZETSCRIPT_MODULE_EXPORT static const char * getErrorMsg();

		void	setUserCallbackOnError(tPrintFunctionCallback _fun);

		ZETSCRIPT_MODULE_EXPORT static int eval_int(const string & str_to_eval);
		ZETSCRIPT_MODULE_EXPORT static bool eval_bool(const string & str_to_eval);
		ZETSCRIPT_MODULE_EXPORT static float eval_float(const string & str_to_eval);
		ZETSCRIPT_MODULE_EXPORT static string eval_string(const string & str_to_eval);



		/**
		 * Main bind function
		 */

		ZETSCRIPT_MODULE_EXPORT bool getScriptObjectFromFunctionAccess(const string &function_access_expression
														  ,CScriptVariable **calling_obj
														  ,CScriptFunction **fun_obj);
		//----






		ZETSCRIPT_MODULE_EXPORT CVirtualMachine * getVirtualMachine();
		//ZETSCRIPT_MODULE_EXPORT CScriptVariable * getMainObject(){return m_mainObject;}


		/**
		 * Clear: Clear compile information.
		 */
		ZETSCRIPT_MODULE_EXPORT void clear();
		//ZETSCRIPT_MODULE_EXPORT void parse(const string & string,const char *filename_ref=NULL);
		//ZETSCRIPT_MODULE_EXPORT void parse_file(const char * filename);
		//ZETSCRIPT_MODULE_EXPORT void compile(bool show_bytecode=false);
		ZETSCRIPT_MODULE_EXPORT void execute();

		ZETSCRIPT_MODULE_EXPORT bool evalString(const string & string, bool execute=true, const char *filename_ref=NULL,bool show_bytecode=false);
		ZETSCRIPT_MODULE_EXPORT bool evalFile(const string & filename,bool execute=true,bool show_bytecode=false);

		/*int newGlobalFunction( const string & function_name, vector<tArgumentInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);
		int getGlobalFunction(const string & function_ref,char n_args=0);

		int newGlobalVariable(const string & variable,const string & variable_ref, const string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0);
		int getGlobalVariable(const string & variable_ref);
*/

		ZETSCRIPT_MODULE_EXPORT static void destroy();
	};




}

#include "helper/CppBindingHelper.h"

