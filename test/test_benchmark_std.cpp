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


zetscript::zs_string zs_std_random_key(){
	char k[10]={0};
	for(int i=0; i < 10; i++){
		k[i]=49+rand()%10;
	}

	return zetscript::zs_string(k);
}

void test_std_map(){
	std::map<std::string,std::string> std_map;
	std::map<std::string,std::string> std_map1;

	printf("sizeof(std::map):%i\n",(int)sizeof(std_map));

	printf("Inserting elements...");
	int start=zetscript::zs_system::clock();

	for(int i=0; i < 100000; i++){
		std_map[std_random_key()]="a";
	}

	printf("done!\n");

	printf("Elapsed time %i\n",(int)((zetscript::zs_system::clock()-start)*1000));

	printf("=======================================\n");
	printf("Press any key\n");
	//zetscript::zs_io::read_char();



}

void test_map(){

	zetscript::zs_map	map;
	zetscript::zs_map	map1;



	printf("sizeof(zs_map):%i\n",(int)sizeof(map));


    printf("Inserting elements...");
	int start=zetscript::zs_system::clock();

	for(int i=0; i < 100000; i++){
		map.set(std_random_key().c_str(),(zetscript::zs_int)(new zetscript::zs_string("a")));
	}

	printf("done!\n");

	printf("Elapsed time %i\n",(int)((zetscript::zs_system::clock()-start)*1000));



	printf("=======================================\n");
	printf("Press any key\n");
	zetscript::zs_io::read_char();

}

void test_std_string(){

	std::string str;

	printf("=======================================\n");
	printf("Test zs_string\n");
	printf("sizeof(std::std_string):%i\n",(int)sizeof(str));

	printf("Append elements...");
	int start=zetscript::zs_system::clock();

	for(int i=0; i < 100000; i++){
		str+=';';
		if((i%100000)==0){
			printf("%i\n",i);
		}
	}

	printf("done! %i chars\n",(int)str.length());

	printf("Elapsed time %i\n",(int)((zetscript::zs_system::clock()-start)*1000));
	printf("=======================================\n");
	printf("Press any key\n");
	zetscript::zs_io::read_char();
}

void test_string(){
	zetscript::zs_string str;
	printf("=======================================\n");
	printf("Test zs_string\n");
	printf("sizeof(zs_string):%i\n",(int)sizeof(str));

	printf("Append elements...");
	int start=zetscript::zs_system::clock();

	for(int i=0; i < 100000; i++){
		str+=';';//.append(zs_std_random_key());
		if((i%100000)==0){
			printf("%i\n",i);
		}
	}

	//printf("done!\n");

	printf("Elapsed time %i\n",(int)((zetscript::zs_system::clock()-start)*1000));
	printf("=======================================\n");
	printf("Press any key\n");
	zetscript::zs_io::read_char();

}

void test_std_vector(){
	std::vector<int> vec;
	printf("=======================================\n");
	printf("Test std::vector<int>\n");
	printf("sizeof(std::vector<int>):%i\n",(int)sizeof(std::vector<int>));

	printf("Append elements...");
		int start=zetscript::zs_system::clock();

	for(int i=0; i < 999999; i++){
		vec.push_back(0);
		if((i%100000)==0){
			printf("%i\n",i);
		}
	}
	printf("Elapsed time %i\n",(int)((zetscript::zs_system::clock()-start)*1000));
	printf("=======================================\n");
	printf("Press any key\n");
	zetscript::zs_io::read_char();

}

void test_vector(){
	zetscript::zs_vector<int> vec;
	printf("=======================================\n");
	printf("Test zs_vector<int>\n");
	printf("sizeof(zs_vector<int>):%i\n",(int)sizeof(zetscript::zs_vector<int>));

	printf("Append elements...");
		int start=zetscript::zs_system::clock();

	for(int i=0; i < 999999; i++){
		vec.push_back(0);
		if((i%100000)==0){
			printf("%i\n",i);
		}
	}
	printf("Elapsed time %i\n",(int)((zetscript::zs_system::clock()-start)*1000));
	printf("=======================================\n");
	printf("Press any key\n");
	zetscript::zs_io::read_char();

}


//------------------------------------------------------------------------------------

void test_vector(){
	test_std_vector();
	test_vector();
}


void test_map(){
	test_std_map();
	test_map();
}

void test_string(){
	test_std_string();
	test_string();
}

int main(int argc, char *argv[]){
	ZS_UNUSUED_2PARAMS(argc,argv);
	//test_vector();
	test_map();
	//test_string();

}
