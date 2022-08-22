/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"
#include <map>

std::string std_random_key(){
	char k[10]={0};
	for(int i=0; i < 10; i++){
		k[i]=49+rand()%10;
	}

	return std::string(k);
}

zetscript::zs_string zs_std_random_key(){
	char k[10]={0};
	for(int i=0; i < 10; i++){
		k[i]=49+rand()%10;
	}

	return zetscript::zs_string(k);
}

void test_std_map(){
	std::map<std::string,std::string> std_map;

	printf("sizeof(std::map):%lu\n",sizeof(std_map));

	printf("Inserting elements...");
	int start=zetscript::zs_system::clock();

	for(int i=0; i < 100000; i++){
		std_map[std_random_key()]="a";
	}

	printf("done!\n");

	printf("Elapsed time %i\n",zetscript::zs_system::clock()-start);

	zetscript::zs_io::read_char();


}

void test_zs_map(){

	zetscript::zs_map	map;

	printf("sizeof(zs_map):%lu\n",sizeof(map));

	printf("Inserting elements...");
	int start=zetscript::zs_system::clock();

	for(int i=0; i < 100000; i++){
		map.set(std_random_key().c_str(),(zetscript::zs_int)(new zetscript::zs_string("a")));
	}

	printf("done!\n");

	printf("Elapsed time %i\n",zetscript::zs_system::clock()-start);

	zetscript::zs_io::read_char();

	/*for(int i=0; i < 10000000; i++){
		map[random_key()]="a";
	}*/
}

void test_std_string(){

	std::string str;

	printf("=======================================\n");
	printf("Test zs_string\n");
	printf("sizeof(std::std_string):%lu\n",sizeof(str));

	printf("Append elements...");
	int start=zetscript::zs_system::clock();

	for(int i=0; i < 100000; i++){
		str+=';';
		if((i%100000)==0){
			printf("%i\n",i);
		}
	}

	printf("done! %i chars\n",str.length());

	printf("Elapsed time %i\n",zetscript::zs_system::clock()-start);
	printf("=======================================\n");
	//zetscript::zs_io::read_char();
}
void test_zs_string(){
	zetscript::zs_string str;
	printf("=======================================\n");
	printf("Test zs_string\n");
	printf("sizeof(zs_string):%lu\n",sizeof(str));

	printf("Append elements...");
	int start=zetscript::zs_system::clock();

	for(int i=0; i < 100000; i++){
		str+=';';//.append(zs_std_random_key());
		if((i%100000)==0){
			printf("%i\n",i);
		}
	}

	//printf("done!\n");

	printf("Elapsed time %i\n",zetscript::zs_system::clock()-start);
	printf("=======================================\n");
	zetscript::zs_io::read_char();
}

void test_map(){
	test_std_map();
	test_zs_map();
}

void test_string(){
	test_std_string();
	test_zs_string();
}

int main(int argc, char *argv[]){
	test_map();
	test_string();

}
