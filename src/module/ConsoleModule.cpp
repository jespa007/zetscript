/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


namespace zetscript{

	zs_int ConsoleModule_readChar(ScriptEngine *_script_engine){
		ZS_UNUSUED_PARAM(_script_engine);
		return Console::readChar();
	}

	void ConsoleModule_outln(ScriptEngine *_script_engine,StackElement *str, StackElement *args){
		StringScriptObject *str_out=StringScriptObject::format(_script_engine,str,args);
		if(str_out!=NULL){
			fprintf(stdout,"%s\n",str_out->toString().toConstChar());
			fflush(stdout);
			delete str_out;
		}
	}


	void ConsoleModule_out(ScriptEngine *_script_engine,StackElement *str, StackElement *args){
		StringScriptObject *str_out=StringScriptObject::format(_script_engine,str,args);
		if(str_out!=NULL){
			fprintf(stdout,"%s",str_out->toString().toConstChar());
			fflush(stdout);
			delete str_out;
		}
	}

	void ConsoleModule_errorln(ScriptEngine *_script_engine,StackElement *str, StackElement *args){
		if(str->properties==0){
			fprintf(stderr,"\n");
			fflush(stderr);
			return;
		}

		StringScriptObject *str_out=StringScriptObject::format(_script_engine,str,args);
		if(str_out!=NULL){
			fprintf(stderr,"%s\n",str_out->toString().toConstChar());
			fflush(stderr);
			delete str_out;
		}
	}


	void ConsoleModule_error(ScriptEngine *_script_engine,StackElement *str, StackElement *args){
		if(str->properties==0){
			return;
		}

		StringScriptObject *str_out=StringScriptObject::format(_script_engine,str,args);
		if(str_out!=NULL){
			fprintf(stderr,"%s",str_out->toString().toConstChar());
			fflush(stderr);
			delete str_out;
		}
	}


}

