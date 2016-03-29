
#ifndef __CONSOLE_LINUX_H__
#define __CONSOLE_LINUX_H__

#include <stdio.h>
#include <termios.h>
#include <unistd.h>


/* 
 * See http://ascii-table.com/ansi-escape-sequences.php for escape sequences
 */

#define ANSI_COLOR_NORMAL  "\x1b[0m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
 

/**
 * ANSI/VT100 helper class
 */
class console
{
public:
	enum consoleColor {
		NORMAL=0,
		RED,
		GREEN,
		YELLOW,
		BLUE,
		MAGENTA,
		CYAN,
	};
	
	static inline const char* codeToColor(consoleColor col) {
		switch(col) {
			default:
			case NORMAL:	return ANSI_COLOR_NORMAL;
			case RED: 		return ANSI_COLOR_RED;
			case GREEN: 	return ANSI_COLOR_GREEN;
			case YELLOW: 	return ANSI_COLOR_YELLOW;
			case BLUE: 		return ANSI_COLOR_BLUE;
			case MAGENTA: 	return ANSI_COLOR_MAGENTA;
			case CYAN: 		return ANSI_COLOR_CYAN;
		}
	}
	
	static inline void setCanonical( bool enabled ) {
		struct termios ttystate;
		tcgetattr(STDIN_FILENO, &ttystate);
	 
		if( !enabled )
		{
			ttystate.c_lflag &= ~ICANON;	// turn off canonical mode
			ttystate.c_cc[VMIN] = 1;		// minimum of number input read.
		}
		else
		{
			ttystate.c_lflag |= ICANON;		// turn on canonical mode
		}

		tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
	}
	
	static inline void setEcho( bool enabled ) {
		struct termios ttystate;
		tcgetattr(STDIN_FILENO, &ttystate);
	 
		if( !enabled )
			ttystate.c_lflag &= ~ECHO;
		else
			ttystate.c_lflag |= ECHO;

		tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
	}
	
	static inline void noEcho() {
		system("stty -echo");
	}
	
	static inline void echo() {
		system("stty echo");
	}
	
	static inline void setColor(consoleColor col) {
		printf("%s", codeToColor(col));
	}
	
	static inline void flush() {
		fflush(stdout);
	}
	
	static inline void resetScrollingRegion() {
		printf("\033[r"); flush();
	}
	
	static inline void setScrollingRegion(int top, int bottom) {
		printf("\033[%i;%ir", top, bottom); flush();
	}
	
	static inline void setCursorPosition(int col, int row) {
		printf("\033[%d;%dH", row, col);
	}
	
	static inline void clearScreen() {
		printf("\033[2J");
		fflush(stdout);
	}
	
	static inline void clearLine() {
		printf("\033[K");
		fflush(stdout);
	}
	
	// move cursor to uppper left corner
	static inline void updateScreen(void) {
		printf("\033[H");
		fflush(stdout);
	}
};

#endif 