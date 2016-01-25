#pragma once

#define REGISTER_FACTORY_FUNCTIONS(_C)\
template<>\
vector<CObject *>			* 	CFactory<_C>::m_vResourceContainer=NULL;\
template<>\
vector<string>			*	CFactory<_C>::m_registeredResourcesTypes=NULL;\
template<>\
string						 	CFactory<_C>::m_type="";\
template<>\
string						 	CFactory<_C>::m_group="";\
template<>\
string						 	CFactory<_C>::m_loadFile="";\
template<>\
map<string,string>  		* 	CFactory<_C>::map_id_prop=NULL;\
template<>\
_C						* 	CFactory<_C>::m_defaultResource=NULL;\
template<>\
unsigned int						CFactory<_C>::time_print=0;\
template<>\
bool						CFactory<_C>::m_factoryLoaded=false;

#define BASE_SCRIPT_FUNCTIONS \
m_chai->add(fun(&intToClassPtr),"intPtrTo"+m_type);\
m_chai->add(fun(&objPtrToClassPtr),"objectPtrTo"+m_type+"Ptr");


#define KEY_ID	"id"
#define KEY_ATTRIBS "properties"


template<class _F>
bool iniFactory(const string & _type){
	return _F::createFactory(_type,_F::registerScriptFunctions);
}

template<class _FS, class _FR>
void REGISTER_PROPERTIES_FACTORY(){
	_FS::addTypeAsLocalElementResourceToLoad(_FR::getType());
}

class CBaseParser;
template<class _tObject>
class CFactory{

	static int n_numeredObject;
	static bool m_isLocal;

protected:

	static map<string,string> *map_id_prop;
	static vector<string>	* m_registeredResourcesTypes;

	static vector<CObject *>								*m_vResourceContainer;
	static string											m_type;
	static string											m_group;
	static string											m_loadFile;
	static bool												m_factoryLoaded;
	//static vector<_tObject *>								*m_vResourceContainerLoadable;

	static _tObject										*m_defaultResource;


	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static CObject	*newObjectInternal(){
		return newObject();
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static CObject	*newObjectWithIDInternal(const string & _std){
		return newObject(_std);
	}

	static CObject	*getObjectInternal(const string & _std){
		return getObject(_std);
	}

public:

	static unsigned int time_print;

	//static bool (*load)(_tObject *obj, const CParserVarPropertyGroup<_T_OPTIONS,_CONST_CHAR("attribs")> & m_attribs);


	static void createSingletons();

	static bool	createFactory(const string & _type,void (* _registerScriptFunctions)()){

		string m_nameType=_type;




		if(CFactoryContainer::getInstance()->registerFactory(

				newObjectInternal,
				newObjectWithIDInternal,
				deleteObject,
				deleteObjectByID,
				getObjectInternal,
				getLocalResourceTypesToLoad,
				_registerScriptFunctions,
				unloadResources,
				destroyFactory,

				m_nameType,
				typeid(_tObject *).name()
				)){

					if(m_vResourceContainer==NULL){
						m_vResourceContainer = new vector<CObject *>;

					if(m_registeredResourcesTypes==NULL){
						m_registeredResourcesTypes = new vector<string>;
					}

					if(map_id_prop==NULL){
						map_id_prop=new map<string,string>;
					}

					m_type=m_nameType;

					m_factoryLoaded=true;

					//print_info_cr("* Created factory %s",m_type.c_str());

					return true;


					// register load resources if specified...
						//m_vResourceContainerLoadable = new vector<_tObject *>;
				}

		}

		return false;
	}

	static bool addTypeAsLocalElementResourceToLoad(const string & _type){

		if(CFactoryContainer::getInstance()->typeObjectFactoryExists(_type)){ // can register...
			m_registeredResourcesTypes->push_back(_type);
		}
		else{
			print_error_cr("factory type \"%s\" is not registered.",_type.c_str());
		}

		return false;
	}

	static vector<string> * getLocalResourceTypesToLoad(){
		return m_registeredResourcesTypes;
	}

	static bool factoryLoaded(){
		return m_factoryLoaded;
	}

	static void	destroyFactory(){

		unloadResources();

		if(m_vResourceContainer != NULL)
			delete m_vResourceContainer;

		//if(m_vResourceContainerLoadable != NULL)
		//	delete m_vResourceContainerLoadable;



		if(m_defaultResource!= NULL){
			delete m_defaultResource;
		}

		if(map_id_prop!=NULL){
			delete map_id_prop;
			map_id_prop = NULL;
		}

		if(m_registeredResourcesTypes!=NULL)
			delete m_registeredResourcesTypes;


		m_registeredResourcesTypes =NULL;
		m_vResourceContainer=NULL;
		//m_vResourceContainerLoadable=NULL;

		m_defaultResource=NULL;
	}

	static string getType(){
		return m_type;
	}

	static string getLoadFile(){
		return m_loadFile;
	}

	static string getGroup(){
		return m_group;
	}

	static _tObject *makeDefaultResource(){
		if(m_defaultResource == NULL) { m_defaultResource = new _tObject();}
		return m_defaultResource;
	}

	static _tObject												*getObject(const string & _id){
		return getObject(_id,m_vResourceContainer, true);
	}

	static _tObject												*getObject(const string & _id, bool _showError){
		return getObject(_id,m_vResourceContainer,_showError);
	}

	static _tObject * objectExists(const string & _sUUID, const vector<CObject*>  * v) {
		if(v != NULL) {
			for(unsigned i = 0; i < v->size(); i++) {
				if(v->at(i)->getID() == _sUUID)
					return dynamic_cast<_tObject *>(v->at(i));
			}
		}
		return NULL;
	}

	static const char *getTypeStr(){

		char *cc = (char *) typeid(_tObject).name();

		//while((*cc != 0) && CStringUtils::isDigit(*cc)) cc++;

		return cc;

	}

	static const char *getPointerTypeStr(){

		char *cc = (char *) typeid(_tObject *).name();

		//while((*cc != 0) && CStringUtils::isDigit(*cc)) cc++;

		return cc;

	}

	static _tObject *getDefaultResource() {

		if(m_defaultResource == NULL) {
			m_defaultResource = new _tObject();
		}

		return m_defaultResource;
	}

	static bool setNewID(const string & new_id, _tObject *obj){


		if(getObject(new_id,false)==NULL){
			obj->setID(new_id);
			return true;
		}
		return false;
	}

	static vector<CObject *> * getResourceContainer() {
		return m_vResourceContainer;
	}

	static _tObject	*intToClassPtr(int str_ptr){
		void *ptr = (void *)str_ptr;


		_tObject *obj = reinterpret_cast<_tObject*>(ptr);

		if(!obj){
			print_error_cr("Cannot reinterpret from %i", str_ptr);
		}


		return obj;
	}

	static _tObject	*objPtrToClassPtr(CObject * _obj){

		return (_tObject *)(_obj);

	}
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static _tObject	*newObject(){
		_tObject *obj = NULL;
		if(m_vResourceContainer != NULL){

			m_vResourceContainer->push_back(
			obj = new _tObject());

			obj->setType(m_type);
			obj->setClassStr(getTypeStr());
			obj->setPointerClassStr(getPointerTypeStr());

			return obj;
		}
		else{
			print_error_cr("m_vResourceContainer %sFactory NULL (container not initialized on CEngine::CEngine()", getTypeStr());
		}

		return NULL;
	}
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static _tObject	*newObject(const string & m_id){
		_tObject *obj = NULL;

		if((obj = newObject()) != NULL){
			if(!setNewID(m_id, obj)){
				print_error_cr("Error object type \"%s\". Trying to set ID \"%s\" that already exist!",getTypeStr(),m_id.c_str());
			}
			obj->setName(m_id);
		}

		return obj;
	}
	static bool getProperty(const string & _id, string & v){
		// PRE:loadPropertiesFile has been loaded before...

		if(map_id_prop->count(_id)==1){
			v= (*map_id_prop)[_id];
			return true;
		}

		return false;

	}



	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static _tObject * getObject(const string & _sName, const vector< CObject *> * _vec, bool _showErrorMessageNotFound) {

		_tObject *exist_obj = NULL;

		// check if controller exist...
		if(((exist_obj = objectExists(_sName, _vec)) == NULL) && _showErrorMessageNotFound) {
			print_error_cr("%s \"%s\" not found ", getTypeStr(), _sName.c_str());
		}

		return exist_obj;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static _tObject * getObjectResource(const string & _sName) {

		_tObject *exist_obj = getObject(_sName);

		if(exist_obj == NULL){
			// it don't care put a warning message get(_sName) already gives you a error (if not found)
		}

		return exist_obj;
	}
	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static bool deleteObjectByID(const string & _sUUID) {
		CObject *exist_obj = NULL;


		if(m_vResourceContainer == NULL) {
			print_error_cr("I can't delete %s::%s because resource list null!!",STR(_tObject),_sUUID.c_str() );
			return false;
		}
		// check if object exist pop from list and get it ...
		for(unsigned int i = 0; i < m_vResourceContainer->size() && exist_obj == NULL; i++) {
			if(m_vResourceContainer->at(i)!=NULL) {
				if(m_vResourceContainer->at(i)->getID() == _sUUID) {
					//print_info_cr("pim pam");
					exist_obj = m_vResourceContainer->at(i);
					m_vResourceContainer->erase(m_vResourceContainer->begin() + i);
				}
			}
		}

		if(exist_obj == NULL) {
			print_warning_cr(" %s::%s doesn't exist",STR(_tObject), _sUUID.c_str());
			return false;
		}

		delete exist_obj;

		return true;
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static bool deleteObject(CObject *object) {
		CObject *exist_obj = NULL;
		string id=object->getID();


		if(m_vResourceContainer == NULL) {
			print_error_cr("I can't delete %s::%s because resource list null!!",STR(_tObject),id.c_str() );
			return false;
		}
		// check if object exist pop from list and get it ...
		for(unsigned int i = 0; i < m_vResourceContainer->size() && exist_obj == NULL; i++) {

			if(m_vResourceContainer->at(i) == object) {
				exist_obj = m_vResourceContainer->at(i);
				m_vResourceContainer->erase(m_vResourceContainer->begin() + i);
			}

		}

		if(exist_obj == NULL) {
			print_warning_cr(" %s::%s doesn't exist",STR(_tObject), id.c_str());
			return false;
		}

		delete exist_obj;

		return true;
	}

	static void unloadResources(){

		if(m_vResourceContainer != NULL) {
			for(unsigned i = 0; i < m_vResourceContainer->size(); i++)
				delete (m_vResourceContainer)->at(i);
			m_vResourceContainer->clear();
		}
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------
	static void freeRegisteredResources() {

	}
};



