#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


#define MAX_COLUMN_POOL_V1 10

void img2c(const char *dst_name, const char *src_file)
{

	FILE *dst_c = NULL;
	FILE *dst_h = NULL;
	SDL_Surface *img = IMG_Load(src_file);

	if(img)
	{
		printf("\n Image info: %i x %i xi bpp", img->w, img->h, img->format->BitsPerPixel);

		char filename_c[100], filename_h[100];

		sprintf(filename_c,"%s.c",dst_name);
		sprintf(filename_h,"%s.h",dst_name);

		dst_c = fopen(filename_c,"wt");
		//dst_h = fopen(filename_h,"wt");

		if(dst_c)
		{
				int dimension = img->w*img->h*img->format->BytesPerPixel;
				Uint8 *aux_ptr = img->pixels;
				int j=0;
				int i=0;
				char dst_name_upper[100]={0};

				for(j=0; j < strlen(dst_name); j++)
					dst_name_upper[j] = toupper(dst_name[j]);

				// writes header ...
				fprintf(dst_c,"#define\t%s_WIDTH\t\t%i",dst_name_upper, img->w);
				fprintf(dst_c,"\n#define\t%s_HEIGHT\t\t%i",dst_name_upper, img->h);
				fprintf(dst_c,"\n#define\t%s_BITS_PP\t\t%i",dst_name_upper, img->format->BitsPerPixel);
				fprintf(dst_c,"\n#define\t%s_BYTES_PP\t\t%i",dst_name_upper, img->format->BytesPerPixel);
				//fprintf(dst_h,"\nextern C const char %s[%i*%i*%i];\n",dst_name, img->w, img->h,img->w, img->h,img->format->BytesPerPixel);

				// write .c
				fprintf(dst_c,"\n\nconst unsigned char %s[%i*%i*%i]={",dst_name, img->w, img->h, img->format->BytesPerPixel);
				for( i = 0; i < dimension; i++)
				{
					if((i%MAX_COLUMN_POOL_V1)==0)
						fprintf(dst_c,"\n\t");
					fprintf(dst_c,"0x%02x",*aux_ptr);
					if(i < (dimension+1))
						fprintf(dst_c,",");

					aux_ptr++;
				}

				fprintf(dst_c,"\n};\n");

				fclose(dst_c);
				//fclose(dst_h);

		}
		else
			fprintf(stderr,"\nError, cannot open \"\" or \"\" to write ");



	}
	else
		fprintf(stderr,"\nError cannot load \"%s\" because %s",src_file,SDL_GetError());

}

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		printf("\nInsuficient parameters.\n");
		printf("\nUse:.\n");
		printf("\nconvert name.c image.png\n");
		return -1;
	}

	img2c(argv[1], argv[2]);

	return 0;
}
