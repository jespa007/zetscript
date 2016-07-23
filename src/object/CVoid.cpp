#include "script/zg_script.h"

CVoid::CVoid(){
    m_classStr=typeid(CVoid).name();
    m_pointerClassStr=typeid(CVoid *).name();

}
