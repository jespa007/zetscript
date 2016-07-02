#pragma once


#define printError(s, ...)         if(timerPrintMsgFinished()){CLog::print(__FILE__,__LINE__,CLog::LOG_ERROR    ,true, s, ##__VA_ARGS__);timerPrintMsgStart(1000);}
#define printWarning(s, ...)       if(timerPrintMsgFinished()){CLog::print(__FILE__,__LINE__,CLog::LOG_WARNING    ,true, s, ##__VA_ARGS__);timerPrintMsgStart(1000);}
#define printInfo(s, ...)           if(timerPrintMsgFinished()){CLog::print(__FILE__,__LINE__,CLog::LOG_INFO    ,true, s, ##__VA_ARGS__);timerPrintMsgStart(1000);}



class CObject{

    void setup();
public:

	enum OBJECT_TYPE{
		NOT_DEFINED=0,
		OBJECT, // general object not defined
		VARIABLE,
		MAX_OBJECT_TYPES
	};

    // define all tipe objects avaliable in our scope...


    //------------------------------------------
    CObject();




    //JEMEObjectType _sType
    void setName(const string & name);
    const string & getName() const;



    void setID(const string & _sUUID);
    const     string &  getID() const;





    //------------------------------------------------------------------------------------------------------------------
    virtual string toString();
    //------------------------------------------------------------------------------------------------------------------
    bool isRegistered();
    void setRegistered();

    //------------------------------------------------------------------------------------
    const string & getUserType() {
        return m_userObjectType;
    }

    void setUserType(const string & _type) {
        m_userObjectType=_type;
    }

    void setClassStr(const string & _class) {
        m_classStr=_class;
    }

    void setPointerClassStr(const string & _class) {
        m_pointerClassStr=_class;
    }

    const string &  getClassStr() {
        return m_classStr;
    }

    const string &  getPointerClassStr() {
        return m_pointerClassStr;
    }

    virtual void ini();
    virtual bool load(const string & m_filename);

    void     setObjectContainer(CObject *_origin_obj);
    CObject *getObjectContainer();


    OBJECT_TYPE getObjectType();

    virtual void update();
    /**
     * unload allocated any member. The destructor call it.
     */
    virtual void unload();
    void reload();

    // more virtual functions ...
    virtual ~CObject();



protected:

    //CInput     *input;
    string     m_name;

    string     m_id,m_localId;

    CObject *m_objectContainer;


    string m_userObjectType,m_classStr,m_pointerClassStr;

    OBJECT_TYPE m_type;





};


class CFakeObject{
public:
	CFakeObject(){}

	~CFakeObject(){}
};

class CFakeObject2:public CFakeObject{
public:
	CFakeObject2(){}

	virtual ~CFakeObject2(){}
};
