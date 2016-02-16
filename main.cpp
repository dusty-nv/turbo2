/*
 * rover
 */

#include "Rover.h"



int main( int argc, char *argv[] )
{
	printf("rover\n");


	Rover* rover = Rover::Create();

	if( !rover )
		return 0;

	printf("[rover]  starting rover main loop...\n");

	/*
	 * main event loop
	 */
	while(true)
	{
		rover->NextEpoch();
	}

	delete rover;
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

