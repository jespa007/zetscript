#include "zetscript.h"

#ifdef _MSC_VER
#include <conio.h>
#define getch _getch
#endif

namespace zetscript{

	zs_int ModuleSystem::clock(){
		return std::clock()*(1000.0f/CLOCKS_PER_SEC);
	}

	zs_int ModuleSystem::readChar(){
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

	void ModuleSystem::printlnSf(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=ScriptObjectString::formatSf(zs,str,args);
		printf("%s\n",str_out->toString().c_str());
		delete str_out;
	}


	void ModuleSystem::printSf(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=ScriptObjectString::formatSf(zs,str,args);
		printf("%s",str_out->toString().c_str());
		delete str_out;
	}

	void ModuleSystem::makeReadOnly(StackElement *stk){
		stk->properties|=MSK_STK_PROPERTY_READ_ONLY;
	}


	void ModuleSystem::evalSf(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=ScriptObjectString::formatSf(zs,str,args);
		printf("not implemented");
		delete str_out;
	}
}

