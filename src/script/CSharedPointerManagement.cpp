#include "zg_script.h"

CSharedPointerManagement *CSharedPointerManagement::sharedPointerManagement=NULL;


CSharedPointerManagement * CSharedPointerManagement::getInstance(){
	if(sharedPointerManagement == NULL){
		sharedPointerManagement=new CSharedPointerManagement();
	}

	return sharedPointerManagement;
}

CSharedPointerManagement::CSharedPointerManagement(){
	memset(shared_pointer,0,sizeof(shared_pointer));
	memset(indexFreeCell,-1,sizeof(indexFreeCell));
	n_freeCell=MAX_UNIQUE_OBJECTS_POINTERS;

	for(int i = 0; i < MAX_UNIQUE_OBJECTS_POINTERS; i++){
		indexFreeCell[i]=i;
	}


}

int CSharedPointerManagement::getFreeCell(){
	if(n_freeCell < MAX_UNIQUE_OBJECTS_POINTERS){
		return indexFreeCell[n_freeCell];
	}else{
		print_error_cr("Reached max unique pointers");
	}

	return -1;

}

void CSharedPointerManagement::destroySingletons(){
	if(sharedPointerManagement != NULL){
		delete sharedPointerManagement;
	}
	sharedPointerManagement = NULL;
}
