
var prova;
//var caca_de_vaca=0;
var render = getRender();

/*
class MyClass:CSprite{
	
	var active;
	//var width;
	//var height;
	//var x;
	//var y;
	
	function MyClass(){
		//this.id=_id;

	}
	
	function update(){
		

		//print("update");
	}
};


class Prova{
	var sprite;
	var free_index;
	
	function Prova(max_sprites){
		this.sprite=[];
		this.free_index=[];
		
		var spr=new MyClass();
		//spr.addFrame(image);
		spr.active=false;
		this.sprite.add(spr);
		
		spr=this.sprite[0];
		
		
		
		for(var i=0; i < this.sprite.size(); i++)
		{
			this.sprite[i].update();
			spr=this.sprite[i];
			var x=spr.x;
		}
		
		
	
	}
	

	
	//function update(){}
	
	function update(){

	}
		
	
	
};*/

//global vars
var MAX_ENEMY_BULLETS=20;
var MAX_HERO_BULLETS=10;
var MAX_EXPLOSIONS=20;


var sound_player=getSoundPlayer();

//var render_width=render.getWidth();
//var render_height=render.getHeight();

var image=[];

var hero;
var hero_bullet;
var explosion;

var enemy=[];
var enemy_bullet=[];

var font;
var laser_snd;


var score=0;
var top_score=0;


//heradate CSprite from C++
class CMySprite: CSprite{
	var active;
	var points;
	var color;
	var time_life;
	
	function CMySprite(){
		this.time_life=0;
		this.active=false;
		this.color=0xFFFFFF;
	}
	
	function CMySprite(_points,_color){
		this.active=false;
		this.points=_points;
		this.color=_color;
		this.time_life = 0;
	}
	
	function addFrame(_img){
		super(_img, this.color);
	}
	
	function update(){
		if(this.time_life>0){
			if(this.time_life < currentTime()){
				this.active=false;
			}
		} 
		super(); // it calls CSprite::update (from C++)
	}
};

class CSpriteManager{
	
	var sprite;
	var free_index;
	var max_time_life;

	function CSpriteManager(){
		this.sprite=[];
		this.free_index=[];
		this.max_time_life=0;
	}

	
	function CSpriteManager(max_sprites, image,_max_time_life){
		this.sprite=[];
		this.free_index=[];
		this.max_time_life=_max_time_life;
		
		
		for(var i=0; i < max_sprites; i++){
			var spr=new CMySprite();
			spr.addFrame(image);
			
			this.sprite.add(spr);
			
			this.free_index.add(i);
		}
	}
	
	function create(start_x, start_y, _dx, _dy){
		
		var index;
		
		if(this.free_index.size()>0)
		{
			// pops the last value and set sprite as active ...
			index= this.free_index.pop();
			
			this.sprite[index].x= start_x;
			this.sprite[index].y=start_y;
			this.sprite[index].dy=_dy;
			this.sprite[index].dx=_dx;
			this.sprite[index].active=true;
			
			if(this.max_time_life>0){
				this.sprite[index].time_life=currentTime()+this.max_time_life;
			}
			
		}
	}
	
	function check_collision(spr)
	{
		
	}
	
	function remove(i){
		this.sprite[i].active=false;
		this.free_index.add(i);
	}
	
	function update(){
		for(var i=0; i < this.sprite.size(); i++)
		{
			var spr=this.sprite[i];
			this.check_collision(spr);
			if(spr.active){
				
				spr.update();
				render.drawSprite(spr);
				
				var x=spr.x;
				
				if(
					
					    ((spr.y<(-(spr.height))) || (spr.y>render.getHeight())) // <-- (spr.height must be in parenthesis)
					 || ((spr.time_life>0) && (spr.time_life<currentTime()))
				 	 || (((spr.x)<(-(spr.width)))   || (spr.x> render.getWidth())) // <-- something happens
						){
					this.remove(i);
					
				}

			}
		}
	}
};

class CEnemyManager:CSpriteManager{
	function CEnemyManager(){
		
		super();

		// create mask based sprite 16x16 (total retro!!!)

		var x=20;
		var start_y=20;
		
		// creating the invaders...
		for(var i=0; i < 15; i++){
			var spr=new CMySprite(100,0x00FF00);
			spr.active=true;
			this.sprite.add(spr);
			spr.addFrame(image[0]);		
			spr.addFrame(image[1]);
			spr.setTimeFrame(500);
			spr.x=x;
			spr.y=start_y+18*1;
			
			spr=new CMySprite(100,0x00FFFF);
			spr.active=true;
			this.sprite.add(spr);
			spr.addFrame(image[2]);		
			spr.addFrame(image[3]);
			spr.setTimeFrame(500);
			spr.x=x;
			spr.y=start_y+18*2;
			
			spr=new CMySprite(100,0xFFFF00);
			spr.active=true;
			this.sprite.add(spr);
			spr.addFrame(image[4]);		
			spr.addFrame(image[5]);
			spr.setTimeFrame(500);
			spr.x=x;
			spr.y=start_y+18*3;
			
			x+=16;
		}
		
		
		// bonus sprite
		var spr=new CMySprite(100,0x0000FF);
		this.sprite.add(spr);
		spr.addFrame(image[6]);		
		spr.x=100;
		spr.y=start_y;
	}
	
	// check collision for all bullets...
	function check_collision(spr){
		if(spr.active){
			for(var i=0;i < hero_bullet.sprite.size(); i++){
				if(hero_bullet.sprite[i].active){
					if(checkCollision(spr,hero_bullet.sprite[i])){
						//print("collision");
						hero_bullet.remove(i);
						spr.active=false;
						score+=100;
						explosion.create(spr.x,spr.y,0,0);
					}
				}
			}
		}
	}
	
	function update(){
		super();
	}
	
};





function init(){ // we initialize here!

	//prova = new Prova(20);
	
	font=new CFont();
	font.load("font.bmp",8,16);
	
	laser_snd=new CSound();
	laser_snd.load("laser.wav");
	
	// first create images....	
	image=[
		
		//invader 1	(frame 1)	
		new CImage([
			  00100000100b
			 ,00010001000b
			 ,00111111100b
			 ,01101110110b
			 ,11111111111b
			 ,10111111101b
			 ,10100000101b
			 ,00011011000b
				 
		])
		// invader 1 (frame 2)
		,new CImage([
			  00100000100b
			 ,10010001001b
			 ,10111111101b
			 ,11101110111b
			 ,11111111111b
			 ,01111111110b
			 ,00100000100b
			 ,01000000010b
					 
		]),
		//invader 2 (frame 1)
		new CImage([
			  00011000b
			 ,00111100b
			 ,01111110b
			 ,11011011b
			 ,11111111b
			 ,00100100b
			 ,01011010b
			 ,10100101b
			 
		]),
		//invader 2 (frame 2)		
		new CImage([
			  00011000b
			 ,00111100b
			 ,01111110b
			 ,11011011b
			 ,11111111b
			 ,01011010bCEnemyManager
			 ,10000001b
			 ,01100010b
			 
		]),
		//invader 3 (frame 1)
		new CImage([
			  000011110000b
			 ,011111111110b
			 ,111111111111b
			 ,111001100111b
			 ,111111111111b
			 ,000110011000b
			 ,001101101100b
			 ,110000000011b
			 
		]),
		//invader 4 (frame 1)		
		new CImage([
			  000011110000b
			 ,011111111110b
			 ,111111111111b
			 ,111001100111b
			 ,111111111111b
			 ,001110011100b
			 ,011001100110b
			 ,001100001100b
			 
		]),
		
		//enemy bonus
		new CImage([
			  0000011111100000b
			 ,0001111111111000b
			 ,0011111111111100b
			 ,0110110110110110b
			 ,1111111111111111b
			 ,0011100110011100b
			 ,0001000000001000b
				 
		]),
		
		// enemy bullet
		new CImage([
			  010b
			 ,100b
			 ,010b
			 ,001b
			 ,010b
			 ,100b
			 ,010b
				 
		]),
		
		// explosion
		new CImage([
			  10010001001b
			 ,01001010010b
			 ,00100000100b
			 ,11000000011b
			 ,00100000100b
			 ,01001010010b
			 ,10010001001b
				 
		])
		
		// hero bullet
		,new CImage([
					 11b
					,11b
					,11b
					,11b
		])
		
		// hero
		,new CImage([
			 0000110000b
			,0100110010b
			,0101111010b
			,1111111111b
			,1111111111b
		])
	];
	

	// hero sprite
	hero=new CMySprite();//100,0x00FF00);
	hero.addFrame(image[10]);
	hero.x=render.getWidth()/2;
	hero.y=195;
	
	// hero bullets...
	enemy=new CEnemyManager();
	hero_bullet=new CSpriteManager(MAX_ENEMY_BULLETS,image[9],0);
	explosion=new CSpriteManager(MAX_EXPLOSIONS,image[8],200);
	/*for(var i=0; i < MAX_HERO_BULLETS; i++){
		hero_bullet
	}*/

}



function update(){
	
	if(TR_LEFT){
		hero.x--;
	}
	
	if(TR_RIGHT){
		hero.x++;
	}
	
	if(T_SPACE){
		
		sound_player.play(laser_snd);
		
		hero_bullet.create(hero.x, hero.y, 0, -5);
	}
	
		
	render.drawText(render.getWidth()/2,0,font,"Score:"+score+"         Top Score:"+top_score);
	//render.drawImage(x,y,image,0x00FF00);
	
	hero_bullet.update();
	enemy.update();
	explosion.update();
	// it draws our customized sprite automatically according current position + current frame...
	
	render.drawSprite(hero); // <-- .x and lost!

	
	//prova.update();
	

		
}

function unload(){

	delete image;
	
	//delete bullet;	
	delete enemy;
	delete hero;
	
}



