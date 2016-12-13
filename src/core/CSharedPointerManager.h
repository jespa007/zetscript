#pragma once


#define MAX_UNIQUE_OBJECTS_POINTERS 4096

class CSharedPointerManager{

	static CSharedPointerManager *sharedPointerManager;

	typedef struct{
		CScriptVariable *shared_ptr;
		int n_shares;
	}tInfoSharedPointer;


	tInfoSharedPointer shared_pointer[MAX_UNIQUE_OBJECTS_POINTERS];
	int indexFreeCell[MAX_UNIQUE_OBJECTS_POINTERS],
		n_freeCell;

	int getFreeCell();
	void setFreeCell(int index_to_free);
	CSharedPointerManager();
public:
	static CSharedPointerManager * getInstance();
	static void destroySingletons();

	int newSharedPointer(CScriptVariable *var_ptr);
	void sharePointer(unsigned int index);
	void unrefSharedPointer(unsigned int index);

};
