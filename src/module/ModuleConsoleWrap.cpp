#include "zetscript.h"

#ifdef _MSC_VER
#include <conio.h>
#define getch _getch
#endif

namespace zetscript{

	zs_int ModuleConsoleWrap_readChar(){
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

	void ModuleConsoleWrap_outln(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stdout,"%s\n",str_out->toString().c_str());
			delete str_out;
		}
	}


	void ModuleConsoleWrap_out(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stdout,"%s",str_out->toString().c_str());
			delete str_out;
		}

	}

	void ModuleConsoleWrap_errorln(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stderr,"%s\n",str_out->toString().c_str());
			delete str_out;
		}
	}


	void ModuleConsoleWrap_error(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=ScriptObjectString::format(zs,str,args);
		if(str_out!=NULL){
			fprintf(stderr,"%s",str_out->toString().c_str());
			delete str_out;
		}
	}


}

