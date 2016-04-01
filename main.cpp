/*
 * rover
 */

#include "Rover.h"
#include "consoleLinux.h"

#include <signal.h>


bool quit_flag = false;

void sig_handler(int s)
{
	printf("caught signal %i\n", s);
	quit_flag = true;
	printf("signalling quit flag...\n");
	//exit(0);
}


int main( int argc, char *argv[] )
{
	signal(SIGINT, sig_handler);

	// print the CLI
	for( int n=0; n < argc; n++ )
		printf("%s ", argv[n]);
		
	printf("\n");

	
	// get the terminal size, if provided
	int termCols = 80;
	int termRows = 24;
	
	if( argc == 3 )
	{
		termCols = atoi(argv[1]);
		termRows = atoi(argv[2]);
		
		printf("[rover]  terminal size:  %i x %i\n", termCols, termRows);
	}
	
	//const int consoleBottom = termRows/2;
	//console::setScrollingRegion(1, termRows/2);
	//console::clearScreen();
	//console::setEcho(false);

	
	// create the rover object
	Rover* rover = Rover::Create();

	if( !rover )
		return 0;

	printf("[rover]  starting rover main loop...\n");

	
	// main event loop
	while(!quit_flag)
	{
		//printf("quit flag: %i\n", (int)quit_flag);
		/*if( kbhit() )
		{
			printf("keyboard hit\n");
		}*/
		
		rover->NextEpoch();
	}

	printf("closing Rover\n");
	delete rover;
	printf("exiting %s process.\n", argv[0]);
	//console::setEcho(true);
	//console::resetScrollingRegion();
	return 0;
}


/*
 * else
		{
			printf("failed to read variables\n");
		}

		printf("\n\nenter desired speed (-3200, 3200)\n");
		int speed = 0;
		scanf("%i", &speed);

		const bool res = mc->SetSpeed(speed);

		printf("%s\n\n", res ? "success" : "failed");

	} while(true);
 */

/*
float fwd  = (evt.motion.rx / 350.0f) * -1.0f;
				float turn = evt.motion.ry / 350.0f;


				
				if( absf(turn) > 0.1f )
				{
					float aturn = absf(turn);

					if( turn > 0.0f )
					{
						speed[0] = -aturn;
						speed[1] = aturn;
					}
					else if( turn < 0.0f )
					{
						speed[0] = aturn;
						speed[1] = -aturn;
					}

					speedChanged = true;
				}
				else
				{
					if( absf(fwd) < 0.1f )
						fwd = 0.0f;

					speed[0] = fwd;
					speed[1] = fwd;

					speedChanged = true;
				}
			}
*/

