/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#ifdef _MSC_VER
#include <conio.h>
#define getch _getch
#endif

namespace zetscript{

	zs_int ConsoleModule_readChar(ZetScript *_zs){
		ZS_UNUSUED_PARAM(_zs);
			char buf=0;
		#ifdef _WIN32
			buf=getch();
		#else
		       /*#include <unistd.h>   //_getch*/
		       /*#include <termios.h>  //_getch*/

		       struct termios old;

		       memset(&old,0,sizeof(old));

		       fflush(stdout);
		       if(tcgetattr(0, &old)<0){
		           perror("tcsetattr()");
		       }
		       old.c_lflag&=~ICANON;
		       old.c_lflag&=~ECHO;
		       old.c_cc[VMIN]=1;
		       old.c_cc[VTIME]=0;
		       if(tcsetattr(0, TCSANOW, &old)<0){
		           perror("tcsetattr ICANON");
		       }
		       if(read(0,&buf,1)<0){
		           perror("read()");
		       }

		       old.c_lflag|=ICANON;
		       old.c_lflag|=ECHO;
		       if(tcsetattr(0, TCSADRAIN, &old)<0){
		           perror ("tcsetattr ~ICANON");
		       }
		       //printf("%c\n",buf);
		#endif
		       return buf;
	}

	void ConsoleModule_outln(ZetScript *zs,StackElement *str, StackElement *args){
		if(str->properties==0){
			fprintf(stdout,"\n");
			return;
		}

		ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stdout,"%s\n",str_out->toString().c_str());
			delete str_out;
		}
	}


	void ConsoleModule_out(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stdout,"%s",str_out->toString().c_str());
			delete str_out;
		}

	}

	void ConsoleModule_errorln(ZetScript *zs,StackElement *str, StackElement *args){
		if(str->properties==0){
			fprintf(stderr,"\n");
			return;
		}

		ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stderr,"%s\n",str_out->toString().c_str());
			delete str_out;
		}
	}


	void ConsoleModule_error(ZetScript *zs,StackElement *str, StackElement *args){
		if(str->properties==0){
			return;
		}

		ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stderr,"%s",str_out->toString().c_str());
			delete str_out;
		}
	}


}

