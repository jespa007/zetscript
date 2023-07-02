#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;

// A native type `Point` to register
class Point{
public:
	int x,y;

	Point(){
		x=0;
		y=0;
	}
};

Point *_point=NULL;

//------------------------------
// WRAP POINT

// defines new function Point ClassScriptObject
Point *PointZs_new(ZetScript *_zs){
	return new  Point();
}

// defines getter property Point::x ClassScriptObject
zs_int PointZs_get_x(ZetScript *_zs, Point *_this){
	return _this->x;
}

// defines getter property Point::y ClassScriptObject
zs_int PointZs_get_y(ZetScript *_zs, Point *_this){
	return _this->y;
}

// defines delete function Point ClassScriptObject
void PointZs_delete(ZetScript *_zs, Point *_this){
	delete _this;
}

// WRAP POINT
//------------------------------

// C function that returns a Point type pointer
Point *returnPoint(ZetScript *_zs){

	// return global _point
    return _point;
}

int main(){
	ZetScript zs;

	// Creates point and init its fields x,y
	_point=new Point();
	_point->x=10;
	_point->y=10;

	// registers class Point
	zs.registerType<Point>("Point",PointZs_new,PointZs_delete);

	// registers property getter Point::x
	zs.registerMemberPropertyMetamethod<Point>("x","_get",PointZs_get_x);

	// registers property getter Point::y
	zs.registerMemberPropertyMetamethod<Point>("y","_get",PointZs_get_y);

	// registers C function that returns a Point type pointer
    zs.registerFunction("returnPoint",returnPoint);

    // Eval script that C function and prints the result by console
    zs.eval(
        "Console::outln(\"result : \"+returnPoint());"
 	);

	// deletes _point
	delete _point;

    return 0;
}
