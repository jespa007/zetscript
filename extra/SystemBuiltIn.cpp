#include "zetscript.h"

#ifdef _MSC_VER
#include <conio.h>
#define getch _getch
#endif

namespace zetscript{

	zs_int SystemBuiltIn::clock(){
		return std::clock()*(1000.0f/CLOCKS_PER_SEC);
	}

	zs_int SystemBuiltIn::readChar(){
			char buf=0;
		#ifdef _WIN32
			buf=getch();
		#else
		       /*#include <unistd.h>   //_getch*/
		       /*#include <termios.h>  //_getch*/

		       struct termios old={0};
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

	void SystemBuiltIn::printlnSf(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=StringBuiltIn::formatSf(zs,str,args);
		printf("%s\n",str_out->toString().c_str());
		delete str_out;
	}


	void SystemBuiltIn::printSf(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=StringBuiltIn::formatSf(zs,str,args);
		printf("%s",str_out->toString().c_str());
		delete str_out;
	}

	void SystemBuiltIn::makeReadOnly(StackElement *stk){
		stk->properties|=MSK_STK_PROPERTY_READ_ONLY;
	}


	void SystemBuiltIn::evalSf(ZetScript *zs,StackElement *str, StackElement *args){
		ScriptObjectString *str_out=StringBuiltIn::formatSf(zs,str,args);
		printf("not implemented");
		delete str_out;
	}
}

