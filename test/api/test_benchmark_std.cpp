/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"
#include <map>
#include <string>
#include <vector>

//------------------------------------------------------------------------------------

std::string std_random_key(){
	char k[10]={0};
	for(int i=0; i < 10; i++){
		k[i]=49+rand()%10;
	}

	return std::string(k);
}

#define TEST_MAX_KEYS	3


zetscript::String zs_std_random_key(){
	char k[10]={0};
	for(int i=0; i < 10; i++){
		k[i]=49+rand()%10;
	}

	return zetscript::String(k);
}

void test_std_map(){
	std::map<std::string,std::string> std_map;
	std::map<std::string,std::string> std_map1;

	printf("=======================================\n");
	printf("Test std::map\n");
	printf("sizeof(std::map):%i\n",(int)sizeof(std_map));

	printf("Inserting 1000000 elements...");
	int start=zetscript::System::clock();

	for(int i=0; i < 1000000; i++){
		std_map[std_random_key()]="a";
	}

	printf("done!\n");

	printf("Elapsed time %i\n",(int)((zetscript::System::clock()-start)*1000));

	printf("=======================================\n");
	printf("Press any key\n");
	//zetscript::Console::::readChar();



}

void test_MapString(){

	zetscript::MapString	map;
	zetscript::MapString	map1;


	printf("=======================================\n");
	printf("Test MapString\n");
	printf("sizeof(MapString):%i\n",(int)sizeof(map));


    printf("Inserting 1000000 elements...");
	int start=zetscript::System::clock();

	for(int i=0; i < 1000000; i++){
		map.set(std_random_key().toConstChar(),(zetscript::zs_int)(new zetscript::String("a")));
	}

	printf("done!\n");

	printf("Elapsed time %i\n",(int)((zetscript::System::clock()-start)*1000));



	printf("=======================================\n");
	printf("Press any key\n");
	zetscript::Console::::readChar();

}

void test_std_string(){

	std::string str;

	printf("=======================================\n");
	printf("Test std::string\n");
	printf("sizeof(std::std_string):%i\n",(int)sizeof(str));

	printf("Append 1000000 elements...");
	int start=zetscript::System::clock();

	for(int i=0; i < 1000000; i++){
		str+=';';
		if((i%100000)==0){
			printf("%i\n",i);
		}
	}

	printf("done! %i chars\n",(int)str.length());

	printf("Elapsed time %i\n",(int)((zetscript::System::clock()-start)*1000));
	printf("=======================================\n");
	printf("Press any key\n");
	zetscript::Console::::readChar();
}

void test_String(){
	zetscript::String str;
	printf("=======================================\n");
	printf("Test String\n");
	printf("sizeof(String):%i\n",(int)sizeof(str));

	printf("Append 1000000 elements...");
	int start=zetscript::System::clock();

	for(int i=0; i < 1000000; i++){
		str+=';';//.append(zs_std_random_key());
		if((i%100000)==0){
			printf("%i\n",i);
		}
	}

	//printf("done!\n");

	printf("Elapsed time %i\n",(int)((zetscript::System::clock()-start)*1000));
	printf("=======================================\n");
	printf("Press any key\n");
	zetscript::Console::::readChar();

}

void test_std_vector(){
	std::vector<int> vec;
	printf("=======================================\n");
	printf("Test std::vector<int>\n");
	printf("sizeof(std::vector<int>):%i\n",(int)sizeof(std::vector<int>));

	printf("Append 1000000 elements...");
		int start=zetscript::System::clock();

	for(int i=0; i < 1000000; i++){
		vec.append(0);
		if((i%100000)==0){
			printf("%i\n",i);
		}
	}
	printf("Elapsed time %i\n",(int)((zetscript::System::clock()-start)*1000));
	printf("=======================================\n");
	printf("Press any key\n");
	zetscript::Console::::readChar();

}

void test_Vector(){
	zetscript::Vector<int> vec;
	printf("=======================================\n");
	printf("Test Vector<int>\n");
	printf("sizeof(Vector<int>):%i\n",(int)sizeof(zetscript::Vector<int>));

	printf("Append 1000000 elements...");
		int start=zetscript::System::clock();

	for(int i=0; i < 1000000; i++){
		vec.append(0);
		if((i%100000)==0){
			printf("%i\n",i);
		}
	}
	printf("Elapsed time %i\n",(int)((zetscript::System::clock()-start)*1000));
	printf("=======================================\n");
	printf("Press any key\n");
	zetscript::Console::::readChar();

}


//------------------------------------------------------------------------------------

void test_vector(){
	test_std_vector();
	test_Vector();
}


void test_map(){
	test_std_map();
	test_MapString();
}


void test_string(){
	test_std_string();
	test_String();
}

int main(int argc, char *argv[]){
	ZS_UNUSUED_2PARAMS(argc,argv);
	//test_vector();
	test_map();
	test_string();

}
