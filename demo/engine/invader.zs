// global vars (not initialize here)
var x;
var y;
var image;
var render;

function init(){ // we initialize here!
	
	image=new CImage();
	render = getRender();
	
	x=0;
	y=0;
}

function update(){
	
	if(TR_LEFT){
		x--;
	}
	
	if(TR_RIGHT){
		x++;
	}
	
	render.drawImage(x,y,image);
}

