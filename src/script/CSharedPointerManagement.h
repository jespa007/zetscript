#pragma once


#define MAX_UNIQUE_OBJECTS_POINTERS 4096

class CSharedPointerManagement{

	static CSharedPointerManagement *sharedPointerManagement;

	typedef struct{
		void *shared_ptr;
		int n_shares;
	}tInfoSharedPointer;


	tInfoSharedPointer shared_pointer[MAX_UNIQUE_OBJECTS_POINTERS];
	int indexFreeCell[MAX_UNIQUE_OBJECTS_POINTERS],
		n_freeCell;

	int getFreeCell();
	CSharedPointerManagement();
public:
	static CSharedPointerManagement * getInstance();
	static void destroySingletons();

	int newSharedPointer();

};
