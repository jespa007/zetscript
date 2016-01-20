
/* BIN2C V1.0 CODED BY CHRISTIAN PADOVANO ON 17-MAY-1995 
   Modified by JORDI ESPADA BRAU ON 24-JULY-2007

   this little utility translates a binary file in a useful C structure
   that can be included in a C source.

   to contact me write to EMAIL: Christian_Padovano@amp.flashnet.it
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_LEN 1
#define LINE     12

/* Tell u the file size in bytes */

long int filesize( FILE *fp )
  {
    long int save_pos, size_of_file;

    save_pos = ftell( fp );
    fseek( fp, 0L, SEEK_END );
    size_of_file = ftell( fp );
    fseek( fp, save_pos, SEEK_SET );
    return( size_of_file );
  }


/* lower chars --> upper chars */

void Upper_chars(char *buffer)
{
 unsigned int c;

 for (c=0; c <= strlen(buffer)-1; c++) *(buffer+c)=toupper( *(buffer+c) );
}


void main( argc, argv )
int argc;
char *argv[];
{
    FILE *source,*dest_h, *dest_c;
    unsigned char buffer[BUF_LEN], Dummy[20];
    unsigned char defs[100]={"__"}, other_buf[30];
    int c;

    if ( (argc < 4) )
    {

     if (  ( argc == 2 ) && ( strcmp(argv[1],"/?")==0 )  )
     {
      puts(" - <<< BIN2C V1.0 >>> by Christian Padovano - \n");
      puts("USAGE: Bin2C  <BINARY file name> <TARGET file name> <STRUCT name>");
      puts("\n <STRUCT > = name of the C structure in the destination file name.\n");
      puts("  <TARGET > = without extension '.h' it will be added by program.");
      exit(0L);
     }
     else
     {
      puts("Bad arguments !!! You must give me all the parameters !!!!\n"
           "Type 'BIN2C /?' to read the help !!!! ");
      exit(0L);
     }

    }

    if( (source=fopen( argv[1], "rb" )) == NULL )
    {
      printf("ERROR : I can't find source file   %s\n",argv[1]);
      exit(20L);
    }

    strcpy(Dummy,argv[2]);
    strcat(Dummy,".h");               /* add suffix .h to target name */

    if( (dest_h=fopen( Dummy, "wb+" )) == NULL )
    {
      printf("ERROR : I can't open destination file   %s\n",Dummy);
      (void)_fcloseall();
      exit(20L);
    }

    strcpy(Dummy,argv[2]);
    strcat(Dummy,".c");               /* add suffix .h to target name */

    if( (dest_c=fopen( Dummy, "wb+" )) == NULL )
    {
      printf("ERROR : I can't open destination file   %s\n",Dummy);
      (void)_fcloseall();
      exit(20L);
    }


    strcpy(Dummy,argv[3]);

    strcpy(other_buf,argv[3]);
    Upper_chars(other_buf);  /* lower to upper chars */
    strcat(other_buf,"_H__");  /* add the suffix _LEN to the struct name */
                           /* for the #define stantment              */

    strcat(defs,other_buf);


    /* It writes the header information */
    fprintf( dest_h, "\n#ifndef %s",defs);
    fprintf( dest_h, "\n#define %s\n\n",defs);

    fprintf( dest_h, "\n#ifdef __cplusplus");
    fprintf( dest_h, "\nextern \"C\"{");
    fprintf( dest_h, "\n#endif\n\n");



    fprintf( dest_h, "\nextern unsigned int %s_size;\n\n", Dummy, filesize(source) );
    fprintf( dest_c, "\nunsigned int %s_size=%i;\n\n", Dummy, filesize(source) );
    fprintf( dest_h, " extern const unsigned char %s[];\n  ", argv[3] );
    fprintf( dest_c, " const unsigned char %s[]=\n {\n  ", argv[3] );

    fprintf( dest_h, "\n#ifdef __cplusplus");
    fprintf( dest_h, "\n};");
    fprintf( dest_h, "\n#endif\n");
    fprintf( dest_h, "\n#endif\n");


    if( ferror( dest_c ) || ferror( dest_h ))
    {
     printf( "ERROR writing on target file:  %s\n",argv[2] );
     (void)_fcloseall();
     exit(20L);
    }

    do
    {
     for ( c=0; ((c <= LINE) && (! feof( source ) )) ; c++)
     {
      fread( buffer, 1, 1, source );
      if (! feof( source ) ) fprintf(dest_c,"0x%02x,",*buffer);
      else fprintf(dest_c,"0x%02x",*buffer);
     }
     fprintf(dest_c,"\n  ");
    }
    while( ! feof( source ) );

    fprintf(dest_c,"};\n\n");

    (void)_fcloseall();

}



