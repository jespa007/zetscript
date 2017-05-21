class CVector2{
	var x,y;

	function CVector2(){
		this.x=0;
		this.y=0;
	}
	
	
	function CVector2(_x,_y){
		this.x=_x;
		this.y=_y;
	}

	function _add(v1,v2){
		var v3=new CVector2();
		v3.x=v1.x + v2.x;
		v3.y=v1.y + v2.y;
		return v3;
	}

};


function _add(v1,v2){
	var v3=new CVector2();
	v3.x=v1.x + v2.x;
	v3.y=v1.y + v2.y;
	return v3;
}

var v1=new CVector2(0,2),v2=new CVector2(1,0);
var v3=v1+v2; // add a+b and 

print("v3=("+v3.x+","+v3.y+")");



