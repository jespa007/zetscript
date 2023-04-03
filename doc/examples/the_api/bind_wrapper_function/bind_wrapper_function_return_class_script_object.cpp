#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ClassScriptObject;
using zetscript::zs_int;

// C structure to be binded
struct Point{
	int x,y;

	Point(){
		x=0;
		y=0;
	}
};

//------------------------------
// WRAP POINT

// defines new function for Point object
Point *PointWrap_new(ZetScript *_zs){
	return new  Point();
}

// defines getter property interface for Point::x
zs_int PointWrap_get_x(ZetScript *_zs, Point *_this){
	return _this->x;
}

// defines getter property interface for Point::y
zs_int PointWrap_get_y(ZetScript *_zs, Point *_this){
	return _this->y;
}

// defines delete function for Point object
void PointWrap_delete(ZetScript *_zs, Point *_this){
	delete _this;
}

// WRAP POINT
//------------------------------

// Definition of the native function interface returnPoint
ClassScriptObject *returnPoint(ZetScript *_zs){
	// define class script object
	ClassScriptObject *class_script_object=NULL;

	// instance point
	Point *point=new Point();

	// initialize x and y
	point->x=10;
	point->y=10;

	// instance new ClassScriptObject using ZetScript context and passing point instance
	class_script_object=_zs->newClassScriptObject(point);

	// return class script object
    return class_script_object;
}

int main(){
	ZetScript zs;

	// bind type Point
	zs.bindType<Point>("Point",PointWrap_new,PointWrap_delete);

	// bind property getter Point::x
	zs.bindMemberPropertyGetter<Point>("x",PointWrap_get_x);

	// bind property getter Point::y
	zs.bindMemberPropertyGetter<Point>("y",PointWrap_get_y);

	// bind native function returnString named as 'returnString'
    zs.bindFunction("returnPoint",returnPoint);

    // Eval script that calls native function 'returnPoint'
    zs.eval(
        "Console::outln(\"result : \"+returnPoint());"
 	);

    return 0;
}
