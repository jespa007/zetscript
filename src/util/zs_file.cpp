/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

namespace zetscript{
	namespace zs_file{

		bool exists(const zs_string & filename){
			FILE  *fp;
			if((fp  =  fopen(filename.c_str(),"r")) != NULL){
				fclose(fp);
				return true;
			}
			return false;
		}

		char * read(const zs_string &  filename, size_t & n_bytes_readed){

			int  length, readed_elements;
			FILE  *fp;

			if((fp  =  fopen(filename.c_str(),"rb"))  !=  NULL)
			{
				if((length = size(filename)) != -1) {

					n_bytes_readed=length+1;
					char *buffer = (char *)ZS_MALLOC(n_bytes_readed);
					readed_elements = fread(buffer, 1, length, fp);

					if(readed_elements != length) {

						free(buffer);
						THROW_RUNTIME_ERROR("number elements doesn't match with length file (%s)",filename.c_str());
					}

					fclose(fp);
					return buffer;
				}
				else  {
					THROW_RUNTIME_ERROR("I can't read file '%s'",filename.c_str());
				}
			}
			else  {
				THROW_RUNTIME_ERROR("I can't open file '%s'",filename.c_str());
			}

			return NULL;
		}

		int  size(const  zs_string & filename)
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
