// global vars (not initialize here)
var x;
var y;
var spr;
var font;
var render;
var sound_player;
var img_var;
var snd;

// heradate CSprite from C++
class CMySprite: CSprite{
	
	function update(){
		
		super(); // it calls CSprite::update (from C++)
		
		
	}
};

function init(){ // we initialize here!
	
	
	render = getRender();
	sound_player=getSoundPlayer();
	
	font=new CFont();
	font.load("font.bmp",16,16);
	
	snd=new CSound();
	snd.load("laser.wav");
	
	// create mask based sprite 16x16 (total retro!!!)
	spr=new CMySprite();
	
	// add sprite mask based usign the constructor (CImage::create from C++)
	spr.addFrame(new CImage(
			[
			  1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			 ,1100000000000011b
			]
	));
	
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
	
	spr.update();
		
	render.drawText(0,0,font,"Hello world");
	//render.drawImage(x,y,image);
	
	// it draws our customized sprite automatically according current position + current frame...
	render.drawSprite(spr);
	
	
}

