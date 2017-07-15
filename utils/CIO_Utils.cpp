
#include "../CZetScript.h"


namespace zetscript{


	string  CIO_Utils::getFileName(const string & _path) {
	  size_t found;
	  string ss=_path;
	  found=_path.find_last_of("/\\");
	  if((int)found != -1)
		  ss= _path.substr(found+1);
	  return ss;
	}

	string  CIO_Utils::getFileNameWithoutExtension(const string & _path) {

		string file = getFileName(_path);


		std::string fName(file);
			size_t pos = fName.rfind(".");
			if(pos == std::string::npos)  //No extension.
				return fName;

			if(pos == 0)    //. is at the front. Not an extension.
				return fName;

			return fName.substr(0, pos);
	}
	//-----------------------------------------------------------------------------------------------v

	char * CIO_Utils::readFile(const string &  filename, int & n_bytes_readed){


		int  file_length, readed_elements;
		FILE  *fp;

		if((fp  =  fopen(filename.c_str(),"rb"))  !=  NULL)
		{
			if((file_length = getLength(filename)) != -1) {

				n_bytes_readed=file_length+1;
				char *buffer = (char *)malloc(n_bytes_readed);
				memset(buffer,0,file_length+1 );
				readed_elements = fread(buffer, 1, file_length, fp);

				if(readed_elements != file_length) {
					zs_print_error_cr("number elements doesn't match with length file (%s)",filename.c_str());
					free(buffer);
					return NULL;
				}

				//ByteBuffer *ch = new ByteBuffer(buffer, file_length+1);

				//delete [] buffer;

				fclose(fp);

				return buffer;
			}
			else  zs_print_error_cr("I can't read file \"%s\"",filename.c_str());
		}
		else  zs_print_error_cr("I can't open file \"%s\"",filename.c_str());


		return NULL;
	}


	int  CIO_Utils::getLength(const  string & filename)
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

