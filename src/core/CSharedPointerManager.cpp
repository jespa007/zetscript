#include "CZetScript.h"

CSharedPointerManager *CSharedPointerManager::sharedPointerManager=NULL;


CSharedPointerManager * CSharedPointerManager::getInstance(){
	if(sharedPointerManager == NULL){
		sharedPointerManager=new CSharedPointerManager();
	}

	return sharedPointerManager;
}

CSharedPointerManager::CSharedPointerManager(){


	for(int s = 0; s < MAX_STACK; s++){
		n_pointers_with_0_shares[s]=0;
		memset(shared_pointer[s],0,sizeof(shared_pointer[s]));
		n_freeCell[s]=MAX_UNIQUE_OBJECTS_POINTERS-1;
		for(int i = 0; i < MAX_UNIQUE_OBJECTS_POINTERS; i++){
			indexFreeCell[s][i]=i;
		}
	}

	idxCurrentStack=0;

}

int CSharedPointerManager::getFreeCell(){
	if(n_freeCell[idxCurrentStack] >= 0){
		int index= indexFreeCell[idxCurrentStack][n_freeCell[idxCurrentStack]];
		indexFreeCell[idxCurrentStack][n_freeCell[idxCurrentStack]] = -1;
		n_freeCell[idxCurrentStack]--;
		return index;
	}else{
		print_error_cr("Reached max assigment pointers (MAX: %i)",MAX_UNIQUE_OBJECTS_POINTERS);
	}

	return -1;

}

void CSharedPointerManager::setFreeCell(int index_to_free){
	if(n_freeCell[idxCurrentStack] < (MAX_UNIQUE_OBJECTS_POINTERS-1)){

		n_freeCell[idxCurrentStack]++;
		indexFreeCell[idxCurrentStack][n_freeCell[idxCurrentStack]]=index_to_free;

	}else{
		print_error_cr("Reached min unique pointers (internal error)");
	}
}

int CSharedPointerManager::getIdx0Shares(int index){

	if(index < 0){
		print_error_cr("index -1");
		return -1;
	}

	if(index >= MAX_UNIQUE_OBJECTS_POINTERS){
		print_error_cr("bad reference shared pointers");
		return -1;
	}

	return shared_pointer[idxCurrentStack][index].idx_0_shares;
}

int CSharedPointerManager::getNumShares(int index){

	if(index < 0){
		print_error_cr("index -1");
		return -1;
	}

	if(index >= MAX_UNIQUE_OBJECTS_POINTERS){
		print_error_cr("bad reference shared pointers");
		return -1;
	}

	return shared_pointer[idxCurrentStack][index].n_shares;
}


void CSharedPointerManager::remove0Shares(int index_0_share_idx){

	if(index_0_share_idx >=n_pointers_with_0_shares[idxCurrentStack]){
		print_error_cr("index_0_share_idx >=n_pointers_with_0_shares");
		return ;
	}

	if(n_pointers_with_0_shares[idxCurrentStack] > 0){

		// erase element ...
		memmove(pointers_with_0_shares[idxCurrentStack] + index_0_share_idx,
				pointers_with_0_shares[idxCurrentStack] + index_0_share_idx + 1,
				sizeof(pointers_with_0_shares[idxCurrentStack][0]) * (MAX_UNIQUE_OBJECTS_POINTERS - index_0_share_idx - 1));



		n_pointers_with_0_shares[idxCurrentStack]--;

	}
	else{
		print_error_cr("n_pointers_with_0_shares <= 0");
	}

}

int CSharedPointerManager::insert0Shares(int shared_pointer_idx)
{
	if(n_pointers_with_0_shares[idxCurrentStack] >=MAX_UNIQUE_OBJECTS_POINTERS){
		print_error_cr("n_pointers_with_0_shares >=MAX_UNIQUE_OBJECTS_POINTERS");
		return -1;
	}

	pointers_with_0_shares[idxCurrentStack][n_pointers_with_0_shares[idxCurrentStack]] = shared_pointer_idx;



	n_pointers_with_0_shares[idxCurrentStack]++;

	return (n_pointers_with_0_shares[idxCurrentStack]-1);
}


int CSharedPointerManager::newSharedPointer(CScriptVariable *var_ptr){
	int index = CSharedPointerManager::getFreeCell();

	if(index != -1){
		shared_pointer[idxCurrentStack][index].n_shares=0;
		shared_pointer[idxCurrentStack][index].shared_ptr=var_ptr;

		shared_pointer[idxCurrentStack][index].idx_0_shares=insert0Shares(index);

		return index;
	}

	return -1;
}

void CSharedPointerManager::sharePointer(int index){

	if(index < 0){
		print_error_cr("index -1");
		return;
	}

	if(index >= MAX_UNIQUE_OBJECTS_POINTERS){
		print_error_cr("bad reference shared pointers");
		return;
	}

	if(shared_pointer[idxCurrentStack][index].shared_ptr != NULL){
		shared_pointer[idxCurrentStack][index].n_shares++; // remove from 0 shares

		if(shared_pointer[idxCurrentStack][index].idx_0_shares != -1){
			remove0Shares(shared_pointer[idxCurrentStack][index].idx_0_shares);
			shared_pointer[idxCurrentStack][index].idx_0_shares= -1;
		}
	}
}
void CSharedPointerManager::unrefSharedPointer( int index){

	if(index < 0){
		print_error_cr("index -1");
		return;
	}

	if(index >= MAX_UNIQUE_OBJECTS_POINTERS){
		print_error_cr("bad reference shared pointers");
		return;
	}

	if(shared_pointer[idxCurrentStack][index].shared_ptr != NULL){

		if(shared_pointer[idxCurrentStack][index].n_shares > 0){
			shared_pointer[idxCurrentStack][index].n_shares--;
		}

		if(shared_pointer[idxCurrentStack][index].n_shares==0){
			delete shared_pointer[idxCurrentStack][index].shared_ptr;
			shared_pointer[idxCurrentStack][index].shared_ptr = NULL;
			setFreeCell(index);
		}
	}

}

void CSharedPointerManager::gc(){

	for(int i = 0; i < n_pointers_with_0_shares[idxCurrentStack]; i++){

		int index = pointers_with_0_shares[idxCurrentStack][i];

		unrefSharedPointer(index);
	}

	n_pointers_with_0_shares[idxCurrentStack] = 0;

}

bool CSharedPointerManager::push(){ // save current stack
	if(idxCurrentStack < MAX_STACK){
		idxCurrentStack++;
		return true;
	}
	else{
		print_error_cr("Reached max stack");
	}
	return false;
}

bool CSharedPointerManager::pop(){ // restore current stack.
	if(idxCurrentStack > 0){
		idxCurrentStack--;
		return true;
	}
	else{
		print_error_cr("Reached min stack");
	}
	return false;

}

void CSharedPointerManager::destroySingletons(){
	if(sharedPointerManager != NULL){
		delete sharedPointerManager;
	}
	sharedPointerManager = NULL;
}
