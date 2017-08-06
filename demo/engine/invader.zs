
//print("LEFT PRESSED..."+c_var);


//print("CACA");

var image;
var render;

function init(){
	
	image=new CImage();
	render = getRender();
	
	/*if(TR_LEFT){
		
		print("LEFT PRESSED...");
		
	}*/
}

function update(){
	
	

	if(TR_RIGHT){
		
		
		
		
		
		print("RIGHT PRESSED...");
		
	}
	
	if(TR_LEFT){
		
		print("LEFT PRESSED...");
		
	}
	
	render.drawImage(0,0,image);
}

