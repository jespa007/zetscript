#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;

// C structure to register
class Point{
public:
	int x,y;

	Point(){
		x=0;
		y=0;
	}
};


class PointManager{
public:
	static PointManager *getInstance(){
		if(instance==NULL){
			instance=new PointManager();
		}
		return instance;
	}

	static void destroy(){
		delete instance;
	}

	Point *newPoint(){
		Point *p=new Point();
		PointManager *manager=getInstance();
		manager->points.push_back(p);
		return p;
	}

	Point *getPoint(int _index){
		PointManager *manager=getInstance();
		return manager->points[_index];
	}

private:
	static PointManager *instance;
	std::vector<Point *> points;

	~PointManager(){
		for(auto point:points){
			delete point;
		}
		points.clear();
	}
};
	
	

PointManager *PointManager::instance=NULL;

//------------------------------
// WRAP POINT

// defines getter property Point::x ClassScriptObject
zs_int PointWrap_get_x(ZetScript *_zs, Point *_this){
	return _this->x;
}

// defines getter property Point::y ClassScriptObject
zs_int PointWrap_get_y(ZetScript *_zs, Point *_this){
	return _this->y;
}

// WRAP POINT
//------------------------------

// C function that returns classScriptObject
Point *returnPoint(ZetScript *_zs){

	// get point at index 0 from our manager
	Point *point=PointManager::getInstance().getPoint(0);

	// initialize x and y
	point->x=10;
	point->y=10;

	// return class script object
    return point;
}

int main(){
	ZetScript zs;

	// Creates point using our manager at index 0
	PointManager::getInstance()->newPoint();

	// register class Point as no instanciable
	zs.registerClass<Point>("Point");

	// register property getter Point::x
	zs.registerMemberPropertyGetter<Point>("x",PointWrap_get_x);

	// register property getter Point::y
	zs.registerMemberPropertyGetter<Point>("y",PointWrap_get_y);

	// register C function that returns Point ScriptClassObject
    zs.registerFunction("returnPoint",returnPoint);

    // Eval script that C function and prints the result by console
    zs.eval(
        "Console::outln(\"result : \"+returnPoint());"
 	);

	// destroy point manager
	PointManager::destroy();

    return 0;
}
