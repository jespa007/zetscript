#include "zg_factory.h"



CFactoryContainer * CFactoryContainer::getInstance(){
	if(m_registerFactory==NULL){
		m_registerFactory = new CFactoryContainer();
	}

	return m_registerFactory;
}

CFactoryContainer * CFactoryContainer::m_registerFactory=NULL;

CFactoryContainer::CFactoryContainer(){
	m_vecTypeContainer=new vector<tInfoFactoryContainer>();

}

CFactoryContainer::tInfoFactoryContainer	* CFactoryContainer::typeObjectFactoryExists(const string & _type){

	for(unsigned i = 0; i < m_vecTypeContainer->size(); i++){
		if(m_vecTypeContainer->at(i).m_type == _type){
			return &m_vecTypeContainer->at(i);
		}
	}

	return NULL;
}

CFactoryContainer::tInfoFactoryContainer	*CFactoryContainer::getFactoryInfo(const string & _type){

	CFactoryContainer::tInfoFactoryContainer * found=NULL;
	if((found = typeObjectFactoryExists(_type))!=NULL){
		//return &(*m_vecTypeContainer)[_type];
		return found;
	}
	else
		print_error_cr("Factory \"%s\" it doesn't exist!",_type.c_str());

	return NULL;
}


bool CFactoryContainer::registerFactory(
		CObject * (* _pfNewObject)(),
		CObject * (* _pfNewObjectWithID)(const string & _id),
		bool (* _pfDeleteObject)(CObject *obj),
		bool (* _pfDeleteObjectByID)(const string & _id),
		CObject * (* _pfGetObject)(const string & _id),
		vector<string> * (* _getLocalResourceTypesToLoad)(),
		void (* _registerScriptFunctions)(),
		void (* unloadResources)(),
		void (* destroyFactory)(),

		const string & _type,
		const string & _typePtr
		){

	if(!typeObjectFactoryExists(_type)){ // can register...
		tInfoFactoryContainer ifc;



		ifc.m_type=_type;
		ifc.m_typePtr=_typePtr;
		ifc.newObject=_pfNewObject;
		ifc.newObjectWithID=_pfNewObjectWithID;
		ifc.deleteObjectByID=_pfDeleteObjectByID;
		ifc.deleteObject=_pfDeleteObject;
		ifc.getObject=_pfGetObject;
		ifc.getLocalResourceTypesToLoad=_getLocalResourceTypesToLoad;
		ifc.registerScriptFunctions=_registerScriptFunctions;
		ifc.unloadResources = unloadResources;
		ifc.destroyFactory = destroyFactory;

		(*m_vecTypeContainer).push_back(ifc);

		// types collection by its group...



		return true;

	}else{
		print_error_cr("* Factory %s type already registered ",_type.c_str());
	}

	return false;
}

bool CFactoryContainer::classPtrIsRegistered(const string & ptr_type){


	for(unsigned int i = 0;i < m_vecTypeContainer->size(); i++){
		if(m_vecTypeContainer->at(i).m_typePtr == ptr_type)
			return true;
	}

	return false;
}

vector<string>		CFactoryContainer::getFactoryTypeList(){

	vector<string> types;

	for(unsigned int i = 0;i < m_vecTypeContainer->size(); i++){
		types.push_back(m_vecTypeContainer->at(i).m_type);
	}

	return types;
}

CObject *intToObjectPtr(int str_ptr){
	void *ptr = (void *)str_ptr;


	CObject *obj = reinterpret_cast<CObject*>(ptr);

	if(!obj){
		print_error_cr("Cannot reinterpret from %i", str_ptr);
	}


	return obj;
}






string CFactoryContainer::getTypeFromInsensitive(const string & insensitive_type){
	for(unsigned i = 0; i < m_vecTypeContainer->size(); i++){
		if(CStringUtils::toLower(m_vecTypeContainer->at(i).m_type) == CStringUtils::toLower(insensitive_type)){
			return m_vecTypeContainer->at(i).m_type;
		}
	}
	return "";
}

CObject *CFactoryContainer::newObject(const string & _type){
	tInfoFactoryContainer *ifc;

	if((ifc=getFactoryInfo(_type))!=NULL){
		return ifc->newObject();
	}

	return NULL;

}

CObject *CFactoryContainer::newObject(const string & _type,const string & _id){
	tInfoFactoryContainer *ifc;

	if((ifc=getFactoryInfo(_type))!=NULL){
		return ifc->newObjectWithID(_id);
	}

	return NULL;
}

CObject *CFactoryContainer::getObject(const string & _type,const string & _id){
	tInfoFactoryContainer *ifc;

	if((ifc=getFactoryInfo(_type))!=NULL){
		return ifc->getObject(_id);
	}

	return NULL;
}


void CFactoryContainer::registerScriptFunctions(){

	for(vector<tInfoFactoryContainer>::const_iterator it = m_vecTypeContainer->begin(); it != m_vecTypeContainer->end(); it++){

		it->registerScriptFunctions();
	}

}


void CFactoryContainer::unLoadResourcesFactories(){
	for(vector<tInfoFactoryContainer>::iterator it=m_vecTypeContainer->begin(); it != m_vecTypeContainer->end(); it++){
		print_info_cr("* unloading resources \"%s\"...",it->m_type.c_str());
		(*it->unloadResources)();
	}



}

void CFactoryContainer::destroyFactories(){
	for(vector<tInfoFactoryContainer>::iterator it=m_vecTypeContainer->begin(); it != m_vecTypeContainer->end(); it++){
		print_info_cr("* removing factory \"%s\"...",it->m_type.c_str());
		(*it->destroyFactory)();
	}
}

void CFactoryContainer::unload(){
	m_vecTypeContainer->clear();
}


bool CFactoryContainer::unRegisterFactory(const string & m_type){
	return false;
}

CFactoryContainer::~CFactoryContainer(){
	destroyFactories();
	delete m_vecTypeContainer;
}

void CFactoryContainer::destroySingletons(){
	delete m_registerFactory;
}
