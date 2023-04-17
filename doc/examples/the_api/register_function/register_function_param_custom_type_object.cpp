#include "zetscript.h"

using zetscript::ZetScript;
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

// defines setter property interface for Point::x
void PointWrap_set_x(ZetScript *_zs, Point *_this, zs_int _x){
	_this->x=_x;
}

// defines setter property interface for Point::y
void PointWrap_set_y(ZetScript *_zs, Point *_this, zs_int _y){
	_this->y=_y;
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
void mulPoint(ZetScript *_zs, Point *_point, zs_int _mul){
	// define class script object
	
	// initialize x and y
	_point->x*=_mul;
	_point->y*=_mul;

}

int main(){
	ZetScript zs;

	// bind type Point
	zs.registerClass<Point>("Point",PointWrap_new,PointWrap_delete);


	// bind property setter Point::x
	zs.registerMemberPropertySetter<Point>("x",PointWrap_set_x);

	// bind property setter Point::y
	zs.registerMemberPropertySetter<Point>("y",PointWrap_set_y);    

	// bind property getter Point::x
	zs.registerMemberPropertyGetter<Point>("x",PointWrap_get_x);

	// bind property getter Point::y
	zs.registerMemberPropertyGetter<Point>("y",PointWrap_get_y);

	// bind native function mulPoint named as 'mulPoint'
    zs.registerFunction("mulPoint",mulPoint);

    // Eval script that calls native function 'mulPoint'
    zs.eval(
        "var point=new Point();\n"
        "point.x=10;\n"
        "point.y=20;\n"
        "Console::outln(\"before : \"+point);\n"
        "mulPoint(point,5)\n"
        "Console::outln(\"after : \"+point);"
 	);

    return 0;
}
