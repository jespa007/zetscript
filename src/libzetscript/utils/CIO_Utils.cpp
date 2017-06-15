
#include "../CZetScript.h"




string  CIO_Utils::getFolder(const string & _filename) {
	size_t found;
	string ss=".";
	found=_filename.find_last_of("/\\");

	if((int)found != -1)
		ss= _filename.substr(0,found);
	return ss;


}

string  CIO_Utils::getFileName(const string & _path) {
  size_t found=0;
  string aux=_path;
  string ss=_path;
  found=aux.find_last_of("/\\");
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

ByteBuffer * CIO_Utils::readFile(const string & filename){


	int  file_length, readed_elements;
	FILE  *fp;

	if((fp  =  fopen(filename.c_str(),"rb"))  !=  NULL)
	{
		if((file_length = getLength(filename)) != -1) {


			unsigned char *buffer = new unsigned char [file_length+1];
			memset(buffer,0,file_length+1 );
			readed_elements = fread(buffer, 1, file_length, fp);

			if(readed_elements != file_length) {
				print_warning_cr("number elements doesn't match with length file (%s)",filename.c_str());
				delete [] buffer;
				return NULL;
			}

			ByteBuffer *ch = new ByteBuffer(buffer, file_length+1);

			delete [] buffer;

			fclose(fp);

			return ch;
		}
		else  print_error_cr("I can't read file \"%s\"",filename.c_str());
	}
	else  print_error_cr("I can't open file \"%s\"",filename.c_str());


	return NULL;
}

//-----------------------------------------------------------------------------------------------v
// Normal file ops.

/*bool CIO_Utils::isDirectory(const string & filename){
	int status;
	struct stat st_buf;
	//int ret_stat;

	status = stat (filename.c_str(), &st_buf);
	    if (status != 0) {
	        //print_error_cr ("Error, errno = %d\n", errno);
	        return false;
	    }


	return S_ISDIR (st_buf.st_mode) != 0;


}*/


bool CIO_Utils::fileExists(const string & m_file) {

	if(m_file == "") return false;

#ifdef _WIN32
	FILE *fp = fopen(m_file.c_str(),"r");

	if(fp == NULL) return false;

	fclose(fp);

	return true;
#else

	if(isDirectory(m_file)) return false;


	struct stat buffer;
	  return (stat (m_file.c_str(), &buffer) == 0);
#endif


}




void CIO_Utils::writeFile(const string & filename, string & data){


	FILE  *fp;

	if((fp  =  fopen(filename.c_str(),"w"))  !=  NULL)
	{

			unsigned n = fwrite((char *)data.c_str(),1, data.size(),fp);

			if(n != data.size()) {
				print_warning_cr("number elements doesn't match with length file (%s)",filename.c_str());
			}

			fclose(fp);

	}
	else  print_error_cr("I can't open file \"%s\"",filename.c_str());
}


int  CIO_Utils::getLength(const  string  & file)
{

	int  ini,  end;
	FILE  *fp;

	if((fp  =  fopen(file.c_str(),"rb"))  !=  NULL)
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

int  CIO_Utils::getLength(FILE  * file)
{

	int  ini,  end;
	FILE  *fp = file;

	if(fp != NULL)
	{
		unsigned int current = ftell(fp);

		fseek(fp,  0,SEEK_SET);
		ini  =  ftell(fp);
		fseek(fp,  0,SEEK_END);
		end  =  ftell(fp);

		fseek(fp,  current,SEEK_SET);
		//fclose(fp);

		return  (end  -  ini);
	}

	return    -1;
}





