#include "zg_core.h"

CSharedPointerManager *CSharedPointerManager::sharedPointerManager=NULL;


CSharedPointerManager * CSharedPointerManager::getInstance(){
	if(sharedPointerManager == NULL){
		sharedPointerManager=new CSharedPointerManager();
	}

	return sharedPointerManager;
}

CSharedPointerManager::CSharedPointerManager(){
	n_pointers_with_0_shares=0;
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


void CSharedPointerManager::remove0Shares(int index_0_share_idx){

	if(index_0_share_idx >=n_pointers_with_0_shares){
		print_error_cr("index_0_share_idx >=n_pointers_with_0_shares");
		return;
	}

	if(n_pointers_with_0_shares > 0){

		// erase element ...
		memmove(pointers_with_0_shares + index_0_share_idx,
				pointers_with_0_shares + index_0_share_idx + 1,
				sizeof(pointers_with_0_shares[0]) * (MAX_UNIQUE_OBJECTS_POINTERS - index_0_share_idx - 1));



		n_pointers_with_0_shares--;
	}
	else{
		print_error_cr("n_pointers_with_0_shares <= 0");
	}
}

int CSharedPointerManager::insert0Shares(int shared_pointer_idx)
{
	if(n_pointers_with_0_shares >=MAX_UNIQUE_OBJECTS_POINTERS){
		print_error_cr("n_pointers_with_0_shares >=MAX_UNIQUE_OBJECTS_POINTERS");
		return -1;
	}

	pointers_with_0_shares[n_pointers_with_0_shares] = shared_pointer_idx;



	n_pointers_with_0_shares++;

	return (n_pointers_with_0_shares-1);
}


int CSharedPointerManager::newSharedPointer(CScriptVariable *var_ptr){
	int index = CSharedPointerManager::getFreeCell();

	if(index != -1){
		shared_pointer[index].n_shares=0;
		shared_pointer[index].shared_ptr=var_ptr;

		shared_pointer[index].idx_0_shares=insert0Shares(index);

		return index;
	}

	return -1;
}

void CSharedPointerManager::sharePointer(int index){

	if(index < 0){
		print_error_cr("index -1");
	}

	if(index >= MAX_UNIQUE_OBJECTS_POINTERS){
		print_error_cr("bad reference shared pointers");
		return;
	}

	if(shared_pointer[index].shared_ptr != NULL){
		shared_pointer[index].n_shares++; // remove from 0 shares

		if(shared_pointer[index].idx_0_shares != -1){
			remove0Shares(shared_pointer[index].idx_0_shares);
			shared_pointer[index].idx_0_shares= -1;
		}
	}
}
void CSharedPointerManager::unrefSharedPointer( int index){

	if(index < 0){
		print_error_cr("index -1");
	}

	if(index >= MAX_UNIQUE_OBJECTS_POINTERS){
		print_error_cr("bad reference shared pointers");
		return;
	}

	if(shared_pointer[index].shared_ptr != NULL){

		if(shared_pointer[index].n_shares > 0){
			shared_pointer[index].n_shares--;
		}

		if(shared_pointer[index].n_shares==0){
			delete shared_pointer[index].shared_ptr;
			shared_pointer[index].shared_ptr = NULL;
			setFreeCell(index);

		}
	}

}

void CSharedPointerManager::gc(){

	for(int i = 0; i < n_pointers_with_0_shares; i++){

		int index = pointers_with_0_shares[n_pointers_with_0_shares];

		unrefSharedPointer(index);
	}

	n_pointers_with_0_shares = 0;

}

void CSharedPointerManager::destroySingletons(){
	if(sharedPointerManager != NULL){
		delete sharedPointerManager;
	}
	sharedPointerManager = NULL;
}
