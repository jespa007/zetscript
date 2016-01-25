#pragma once


#define printError(s, ...)         if(timerPrintMsgFinished()){CLog::print(__FILE__,__LINE__,CLog::LOG_ERROR    ,true, s, ##__VA_ARGS__);timerPrintMsgStart(1000);}
#define printWarning(s, ...)       if(timerPrintMsgFinished()){CLog::print(__FILE__,__LINE__,CLog::LOG_WARNING    ,true, s, ##__VA_ARGS__);timerPrintMsgStart(1000);}
#define printInfo(s, ...)           if(timerPrintMsgFinished()){CLog::print(__FILE__,__LINE__,CLog::LOG_INFO    ,true, s, ##__VA_ARGS__);timerPrintMsgStart(1000);}



class CObject{

    void setup();
public:
    // define all tipe objects avaliable in our scope...

    static int n_numeredObject;

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
    const string & getType() {
        return m_objectType;
    }

    void setType(const string & _type) {
        m_objectType=_type;
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
    CObject *getObjectContainer(CObject *_origin_obj);



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

    bool   active;
    unsigned int m_elapsed_ticks_print;
    string m_objectType,m_classStr,m_pointerClassStr;

    bool registered;





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
