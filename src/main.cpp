#include "script/zg_script.h"

int main(int argc, char * argv[]){
	

	if(argc < 2){
		printf("Put expression to evaluate.\n");
		printf("\n");
		printf("Example:\n");
		printf("\n");
		printf("5*6+3");
		printf("\n");
		printf("\n");
		return 0;
	}
	
	PASTOperator op=generateAST(argv[1]);
	
	if(op==NULL){ // some error happend!
		return -1;		
	}
	else{
		int numreg=0;
		
		
		if(generateAsmCode(op,numreg)<0){
			printf("Error generating code\n");
		}
	}

	return 0;
}
