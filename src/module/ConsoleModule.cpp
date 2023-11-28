/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"


namespace zetscript{

	zs_int ConsoleModule_readChar(ZetScript *_zs){
		ZS_UNUSUED_PARAM(_zs);
		return Console::readChar();
	}

	void ConsoleModule_outln(ZetScript *zs,StackElement *str, StackElement *args){
		StringObject *str_out=StringObject::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stdout,"%s\n",str_out->toString().toConstChar());
			fflush(stdout);
			delete str_out;
		}
	}


	void ConsoleModule_out(ZetScript *zs,StackElement *str, StackElement *args){
		StringObject *str_out=StringObject::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stdout,"%s",str_out->toString().toConstChar());
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

		StringObject *str_out=StringObject::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stderr,"%s\n",str_out->toString().toConstChar());
			fflush(stderr);
			delete str_out;
		}
	}


	void ConsoleModule_error(ZetScript *zs,StackElement *str, StackElement *args){
		if(str->properties==0){
			return;
		}

		StringObject *str_out=StringObject::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stderr,"%s",str_out->toString().toConstChar());
			fflush(stderr);
			delete str_out;
		}
	}


}

