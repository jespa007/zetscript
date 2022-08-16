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

int main(int argc, char *argv[]){
	std::map<std::string,std::string> std_map;
	zetscript::zs_map	map;

	printf("Inserting elements...");
	int start=zetscript::zs_system::clock();

	for(int i=0; i < 1000000; i++){
		//std_map[std_random_key()]="a";
		map.set(std_random_key().c_str(),(zetscript::zs_int)(new std::string("a")));
	}

	printf("done!\n");

	printf("Elapsed time %i\n",zetscript::zs_system::clock()-start);

	zetscript::zs_io::read_char();

	/*for(int i=0; i < 10000000; i++){
		map[random_key()]="a";
	}*/
	return 0;


}
