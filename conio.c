#include "conio.h"

#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <asm/ioctl.h>

int getch(void) 
{ 
        struct termios tm, tm_old; 
        int fd = STDIN_FILENO, c; 

        if(tcgetattr(fd, &tm) < 0) 
                return -1; 
        tm_old = tm; 
        cfmakeraw(&tm); 
        if(tcsetattr(fd, TCSANOW, &tm) < 0) 
                return -1; 
        c = fgetc(stdin); 
        if(tcsetattr(fd, TCSANOW, &tm_old) < 0) 
                return -1; 
        return c; 
} 

int kbhit() {
    static const int STDIN = 0;
    static int initialized = FALSE;
    
    if (! initialized) {
        // Use termios to turn off line buffering
        struct termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = TRUE;
    }
    
    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}
