// global vars (not initialize here)
var x;
var y;
var image;
var font;
var render;
var img_var;

function init(){ // we initialize here!
	
	image=new CImage();
	render = getRender();
	
	font=new CFont();
	font.load("font.bmp",16,16);
	
	x=0;
	y=0;
	
	img_var=0111b;
	
	print("the value is"+img_var);
}

function update(){
	
	if(TR_LEFT){
		x--;
	}
	
	if(TR_RIGHT){
		x++;
	}
	
	render.drawText(0,0,font,"Hello world");
	render.drawImage(x,y,image);
}

