
var x;
var y;
//print("LEFT PRESSED..."+c_var);


//print("CACA");

var image;
var render;

function init(){
	
	// init variables here!
	image=new CImage();
	render = getRender();
	
	x=0;
	y=0;
	
	/*if(TR_LEFT){
		
		print("LEFT PRESSED...");
		
	}*/
}

function update(){
	
	

	if(TR_LEFT){
		
		x--;
		
		
		
		//print("RIGHT PRESSED...");
		
	}
	
	if(TR_RIGHT){
		
		x++;
		//print("LEFT PRESSED...");
		
	}
	
	render.drawImage(x,y,image);
}

