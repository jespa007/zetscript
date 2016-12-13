#include "zg_core.h"

CSharedPointerManager *CSharedPointerManager::sharedPointerManager=NULL;


CSharedPointerManager * CSharedPointerManager::getInstance(){
	if(sharedPointerManager == NULL){
		sharedPointerManager=new CSharedPointerManager();
	}

	return sharedPointerManager;
}

CSharedPointerManager::CSharedPointerManager(){
	memset(shared_pointer,0,sizeof(shared_pointer));

	n_freeCell=MAX_UNIQUE_OBJECTS_POINTERS-1;

	for(int i = 0; i < MAX_UNIQUE_OBJECTS_POINTERS; i++){
		indexFreeCell[i]=i;
	}


}

int CSharedPointerManager::getFreeCell(){
	if(n_freeCell > 0){
		int index= indexFreeCell[n_freeCell-1];
		n_freeCell--;
		return index;
	}else{
		print_error_cr("Reached max unique pointers");
	}

	return -1;

}

void CSharedPointerManager::setFreeCell(int index_to_free){
	if(n_freeCell < MAX_UNIQUE_OBJECTS_POINTERS){
		n_freeCell++;
		indexFreeCell[n_freeCell]=index_to_free;
	}
}


int CSharedPointerManager::newSharedPointer(CScriptVariable *var_ptr){
	int index = CSharedPointerManager::getFreeCell();

	if(index != -1){
		shared_pointer[index].n_shares=1;
		shared_pointer[index].shared_ptr=var_ptr;
	}

	return -1;
}

void CSharedPointerManager::sharePointer(unsigned int index){

	if(index >= MAX_UNIQUE_OBJECTS_POINTERS){
		return;
	}

	if(shared_pointer[index].shared_ptr != NULL){
		shared_pointer[index].n_shares++;
	}
}
void CSharedPointerManager::unrefSharedPointer(unsigned int index){

	if(index >= MAX_UNIQUE_OBJECTS_POINTERS){
		return;
	}

	if(shared_pointer[index].shared_ptr != NULL && shared_pointer[index].n_shares > 0){
		shared_pointer[index].n_shares--;
		if(shared_pointer[index].shared_ptr==0){
			delete shared_pointer[index].shared_ptr;
			shared_pointer[index].shared_ptr = NULL;
			setFreeCell(index);

		}
	}

}

void CSharedPointerManager::destroySingletons(){
	if(sharedPointerManager != NULL){
		delete sharedPointerManager;
	}
	sharedPointerManager = NULL;
}
