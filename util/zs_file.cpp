#include "../zetscript.h"

namespace zetscript{
	namespace zs_file{

		char * read(const std::string &  filename, size_t & n_bytes_readed){

			size_t  length, readed_elements;
			FILE  *fp;

			if((fp  =  fopen(filename.c_str(),"rb"))  !=  NULL)
			{
				if((length = size(filename)) != -1) {

					n_bytes_readed=length+1;
					char *buffer = (char *)malloc(n_bytes_readed);
					memset(buffer,0,length+1 );
					readed_elements = fread(buffer, 1, length, fp);

					if(readed_elements != length) {

						free(buffer);
						THROW_RUNTIME_ERROR(zs_strutils::format("number elements doesn't match with length file (%s)",filename.c_str()));
					}

					fclose(fp);
					return buffer;
				}
				else  THROW_RUNTIME_ERROR(zs_strutils::format("I can't read file \"%s\"",filename.c_str()));
			}
			else  THROW_RUNTIME_ERROR(zs_strutils::format("I can't open file \"%s\"",filename.c_str()));

			return NULL;
		}

		int  size(const  std::string & filename)
		{
			int  ini,  end;
			FILE  *fp;

			if((fp  =  fopen(filename.c_str(),"rb"))  !=  NULL)
			{


				fseek(fp,  0,SEEK_SET);
				ini  =  ftell(fp);
				fseek(fp,  0,SEEK_END);
				end  =  ftell(fp);

				fclose(fp);

				return  (end  -  ini);
			}

			return    -1;
		}
	}
}
