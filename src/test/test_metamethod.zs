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
	

};


function _add(){
	var v3=new CVector2();
	v3.x=0 + 1;
	v3.y=1 + 0;
	return v3;
}

var v1=new CVector2(0,1),v2=new CVector2(1,0);
var v3=_add(); // add a+b and 



