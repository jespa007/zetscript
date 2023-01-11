/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

#ifdef _MSC_VER
#include <conio.h>
#define getch _getch
#endif

#define ZS_IO_GET_LINE_MAX 256

namespace zetscript{

	namespace zs_io{
		int getline(char **lineptr, size_t *n, FILE *stream)
		{
		static char line[ZS_IO_GET_LINE_MAX]={0};
		char *ptr=NULL;
		size_t len=0;

		   if (lineptr == NULL || n == NULL)
		   {
			  errno = EINVAL;
			  return -1;
		   }

		   if (ferror (stream))
			  return -1;

		   if (feof(stream))
			  return -1;

		   if(fgets(line,ZS_IO_GET_LINE_MAX,stream)!=NULL){

			   ptr = strchr(line,'\n');
			   if (ptr)
				   *ptr = '\0';

			   len = strlen(line);

			   if ((len+1) < ZS_IO_GET_LINE_MAX)
			   {
				  ptr = (char *)realloc(*lineptr, ZS_IO_GET_LINE_MAX);
				  if (ptr == NULL)
					 return(-1);
				  *lineptr = ptr;
				  *n = ZS_IO_GET_LINE_MAX;
			   }
		   }

		   strcpy(*lineptr,line);
		   return (int)(len);
		}


		char read_char(){
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
	}
}
