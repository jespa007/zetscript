
class Test{ // scope 1
	var i;
	
	function Test(){
		this.i=0;
	}
	
	function Test(_i){
		this.i=_i;
	}
	
	function update(){
		print("update");
	}
	
};

var t;

function ini(){
	t=new Test();
}

function update(){
	t.update();
}



ini();
update();