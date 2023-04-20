#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;

// C structure to register
struct Point{
	int x,y;
	static Point *getInstance(){
		if(_instance==NULL){
			_instance=new Point();
		}
		return 
	}
private:
	static Point *_instance;
	
	Point(){
		x=0;
		y=0;
	}

};

Point *Point::_instance=NULL;

//------------------------------
// WRAP POINT

// defines new function Point ClassScriptObject

// defines getter property Point::x ClassScriptObject
zs_int PointWrap_get_x(ZetScript *_zs, Point *_this){
	return _this->x;
}

// defines getter property Point::y ClassScriptObject
zs_int PointWrap_get_y(ZetScript *_zs, Point *_this){
	return _this->y;
}

Point *getPointInstance(){
	return 
}

// WRAP POINT
//------------------------------

// C function that returns classScriptObject
Point *returnPoint(ZetScript *_zs){

	// get singleton point
	Point *point=Point::getInstance();

	// initialize x and y
	point->x=10;
	point->y=10;

	// return class script object
    return point;
}

int main(){
	ZetScript zs;

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

    return 0;
}
