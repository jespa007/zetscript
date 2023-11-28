/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

namespace zetscript{
	namespace File{

		bool exists(const String & filename){
#ifdef _WIN32
			 DWORD dwAttrib = GetFileAttributes(filename.toConstChar());

			  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
			         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

#else
		struct stat statbuf;
			stat(filename.toConstChar(), &statbuf);
			// test for a regular file
			if (S_ISREG(statbuf.st_mode)){
				return true;
			}


			return false;
#endif
			/*FILE  *fp;
			if((fp  =  fopen(filename.toConstChar(),"r")) != NULL){
				fclose(fp);
				return true;
			}
			return false;*/
		}

		Buffer *readText(const String &  filename){

			int  length=-1;
			size_t readed_elements;
			FILE  *fp;

			if ((fp = fopen(filename.toConstChar(), "rb")) == NULL)
			{
				ZS_THROW_RUNTIME_ERROR("I can't open file '%s'", filename.toConstChar());
			}


			if ((length = size(filename)) == -1) {
				ZS_THROW_RUNTIME_ERROR("I can't read file '%s'", filename.toConstChar());
			}

			int n_bytes_readed=length+1;
			Buffer *buffer= new Buffer((uint8_t *)ZS_MALLOC(n_bytes_readed),n_bytes_readed);
			readed_elements = fread(buffer->ptr, 1, (size_t)length, fp);

			if((int)readed_elements != length) {

				delete buffer;
				ZS_THROW_RUNTIME_ERROR("number elements doesn't match with length file (%s)",filename.toConstChar());
			}

			fclose(fp);

			return buffer;

		}

		int  size(const  String & filename)
		{
			int  ini,  end;
			FILE  *fp;

			if((fp  =  fopen(filename.toConstChar(),"rb"))  !=  NULL)
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
