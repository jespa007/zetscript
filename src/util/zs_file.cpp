/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "../zetscript.h"

namespace zetscript{
	namespace zs_file{

		bool exists(const zs_string & filename){
#ifdef _WIN32
			 DWORD dwAttrib = GetFileAttributes(filename.c_str());

			  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
			         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

#else
		struct stat statbuf;
			stat(filename.c_str(), &statbuf);
			// test for a regular file
			if (S_ISREG(statbuf.st_mode)){
				return true;
			}


			return false;
#endif
			/*FILE  *fp;
			if((fp  =  fopen(filename.c_str(),"r")) != NULL){
				fclose(fp);
				return true;
			}
			return false;*/
		}

		zs_buffer *read_text(const zs_string &  filename){

			int  length=-1;
			size_t readed_elements;
			FILE  *fp;

			if((fp  =  fopen(filename.c_str(),"rb"))  !=  NULL)
			{
				if((length = size(filename)) != -1) {

					size_t n_bytes_readed=(size_t)(length+1);
					zs_buffer *buffer= new zs_buffer((uint8_t *)ZS_MALLOC(n_bytes_readed),n_bytes_readed);
					readed_elements = fread(buffer->ptr, 1, (size_t)length, fp);

					if((int)readed_elements != length) {

						delete buffer;
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
