#include "CFont.h"

CFont::CFont(){

	char_width=char_height=totalchars=totalchars_x=totalchars_y=0;
}

bool CFont::load(string * file,int char_width,int char_height){

	if((texture=CImage::SurfaceToTexture(SDL_LoadBMP(file->c_str()))) !=NULL){
		SDL_QueryTexture(texture,
				NULL,
				NULL,
				&this->mWidth,
				&this->mHeight);

		this->char_width=char_width;
		this->char_height=char_height;
		totalchars_x=this->mWidth/char_width;
		totalchars_y=this->mHeight/char_height;
		totalchars=totalchars_x*totalchars_y;

		return true;
	}


	return false;
}

int CFont::getCharWidth(){
	return char_width;
}
int CFont::getCharHeight(){
	return char_height;
}

int CFont::getTextWith(const string & str){

	int total_width=0;

	for(unsigned i=0; i < str.size(); i++){
		total_width+=char_width;
	}
	return total_width;
}

SDL_Rect * CFont::getRectChar(char c){
	m_aux_rect={0,0,char_width,char_height};
	if(c<totalchars){
		int x=c%totalchars_x;
		int y=c/totalchars_x;


		m_aux_rect.x=x*char_width;
		m_aux_rect.y=y*char_height;

	}


	return &m_aux_rect;
}

CFont::~CFont(){

}
