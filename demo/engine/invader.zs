// global vars (not initialize here)
var x;
var y;
var image;
var font;
var render;
var sound_player;
var img_var;
var snd;

function init(){ // we initialize here!
	
	image=new CImage();
	render = getRender();
	sound_player=getSoundPlayer();
	
	font=new CFont();
	font.load("font.bmp",16,16);
	
	snd=new CSound();
	snd.load("laser.wav");
	
	x=0;
	y=0;
	
	img_var=0111b; // binary var
	
	print("the value is"+img_var);
}

function update(){
	
	if(TR_LEFT){
		x--;
	}
	
	if(TR_RIGHT){
		x++;
	}
	
	if(T_SPACE){
		print("play");
		sound_player.play(snd);
	}
		
	render.drawText(0,0,font,"Hello world");
	render.drawImage(x,y,image);
}

