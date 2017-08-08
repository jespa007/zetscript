#include "CFont.h"

CFont::CFont(){

	char_width=char_height=totalchars=totalchars_x=totalchars_y=0;
}

bool CFont::load(const char * file,int char_width,int char_height){

	if((texture=CImage::SurfaceToTexture(SDL_LoadBMP(file))) !=NULL){
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

void CFont::load(std::string * file,int *char_width,int *char_height){
	CFont::load(file->c_str(),*char_width, *char_height);
}

int CFont::getCharWidth(){
	return char_width;
}
int CFont::getCharHeight(){
	return char_height;
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
