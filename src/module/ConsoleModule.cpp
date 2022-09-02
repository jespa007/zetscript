/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"



namespace zetscript{

	zs_int ConsoleModule_readChar(ZetScript *_zs){
		ZS_UNUSUED_PARAM(_zs);
		return zs_io::read_char();
	}

	void ConsoleModule_outln(ZetScript *zs,StackElement *str, StackElement *args){
		StringScriptObject *str_out=StringScriptObject::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stdout,"%s\n",str_out->toString().c_str());
			fflush(stdout);
			delete str_out;
		}
	}


	void ConsoleModule_out(ZetScript *zs,StackElement *str, StackElement *args){
		StringScriptObject *str_out=StringScriptObject::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stdout,"%s",str_out->toString().c_str());
			fflush(stdout);
			delete str_out;
		}

	}

	void ConsoleModule_errorln(ZetScript *zs,StackElement *str, StackElement *args){
		if(str->properties==0){
			fprintf(stderr,"\n");
			fflush(stderr);
			return;
		}

		StringScriptObject *str_out=StringScriptObject::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stderr,"%s\n",str_out->toString().c_str());
			fflush(stderr);
			delete str_out;
		}
	}


	void ConsoleModule_error(ZetScript *zs,StackElement *str, StackElement *args){
		if(str->properties==0){
			return;
		}

		StringScriptObject *str_out=StringScriptObject::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stderr,"%s",str_out->toString().c_str());
			fflush(stderr);
			delete str_out;
		}
	}


}

