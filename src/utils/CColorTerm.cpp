#include "core/zg_core.h"



void CColorTerm::setColor(FILE *std_type, int attr, int fg, int bg)
{	
  
        char command[13];

	/* Command is the control command to the terminal */
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	fprintf(std_type,"%s", command);
}

void CColorTerm::resetColor(FILE *std_type)
{
    int attr = CColorTerm::RESET;
    int fg = CColorTerm::BLACK;
    int bg= CColorTerm::WHITE;
  
        char command[13];

	/* Command is the control command to the terminal */
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	fprintf(std_type, "%s", command);
}
