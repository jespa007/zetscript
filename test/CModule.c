#include <stdio.h>
#include <stdlib.h>
#include "CModule.h"

typedef struct{
	int x,y;
}CModuleData;

CModule *CModule_New(void){
	CModule *module=malloc(sizeof(CModule));
	CModuleData *data=malloc(sizeof(CModuleData));
	module->data=data;
	return module;
}

void CModule_Delete(CModule * _this){
	if(_this != NULL){
		free(_this);
	}
}
