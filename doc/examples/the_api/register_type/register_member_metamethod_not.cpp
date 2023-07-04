
#include "NumberZs.h"


int main(){

	ZetScript zs;

	NumberZs_register(&zs);


	// Evaluates a script that instances "Number" with 10.5 and later it adds 20
	zs.eval(
		var number=new Number()

		if(!number){
			Console::outln("Number is empty")	
		}
	);

	return 0;
};


