#include "script/zg_script.h"




int CObject::n_numeredObject = 0;




void CObject::setup() {

    m_elapsed_ticks_print = 0;
    active = false;
    m_objectType = "unknow";


    m_objectContainer=NULL;

    // generate a uuid by default ...
    m_id = CUUID::newUUID();
    m_localId = m_id;
    m_classStr="unknown";
    m_pointerClassStr="unknown";
}
//---------------------------------------------------------------------

CObject::CObject() {
    setup();



    // to avoid collisions with unamed objects...
    //m_name = CStringUtils::formatString("%s_%i",getObjectTypeStr(),n_numeredObject++);
}
//---------------------------------------------------------------------

void CObject::ini() {



}

//---------------------------------------------------------------------
bool CObject::load(const string & m_filename) {

    return false;
}


void CObject::setObjectContainer(CObject *_origin_obj){
    m_objectContainer = _origin_obj;
}

CObject *CObject::getObjectContainer(CObject *_origin_obj){
    return m_objectContainer;
}


//---------------------------------------------------------------------
void CObject::setID(const string & _p_m_id) {
    this->m_id = _p_m_id;
}
//---------------------------------------------------------------------
const string & CObject::getID() const {
    return m_id;
}
//---------------------------------------------------------------------
const string & CObject::getName() const {
    return m_name;
}
//---------------------------------------------------------------------
void CObject::setName(const string & _name) {
    m_name = _name;
}
//---------------------------------------------------------------------
string CObject::toString() {
    string m_strValue = "";

    return m_strValue;
}


void CObject::update(){


}

void CObject::reload() {

}

void CObject::unload() {



}
//---------------------------------------------------------------------
CObject::~CObject() {
    unload();
}
