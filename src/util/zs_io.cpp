#include "../zetscript.h"

namespace zetscript{
	namespace zs_io{
		int getline(char **lineptr, size_t *n, FILE *stream)
		{
		static char line[256];
		char *ptr;
		unsigned int len;
		char *c;

		   if (lineptr == NULL || n == NULL)
		   {
			  errno = EINVAL;
			  return -1;
		   }

		   if (ferror (stream))
			  return -1;

		   if (feof(stream))
			  return -1;

		   c=fgets(line,256,stream);

		   ptr = strchr(line,'\n');
		   if (ptr)
			  *ptr = '\0';

		   len = strlen(line);

		   if ((len+1) < 256)
		   {
			  ptr = (char *)realloc(*lineptr, 256);
			  if (ptr == NULL)
				 return(-1);
			  *lineptr = ptr;
			  *n = 256;
		   }

		   strcpy(*lineptr,line);
		   return(len);
		}
	}
}
