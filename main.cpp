/*
 * rover
 */

#include "MotorController.h"
#include "UsbManager.h"
#include "evdevController.h"

//#include "spnav.h"


inline float absf( float x )
{
	return (x < 0.0f) ? -x : x;
}


int main( int argc, char *argv[] )
{
	printf("rover\n");

	UsbManager* usb = new UsbManager();

	if( !usb->Init() )
		return 0;

	for( uint n=0; n < usb->GetNumMotorControllers(); n++ )
		printf(" [%u] %s\n", n, usb->GetMotorController(n)->GetSerial());

	
	MotorController* mc[] = { usb->FindBySerial("55FF-7306-7084-5457-2709-0267"),
							  usb->FindBySerial("55FF-7B06-7084-5457-2608-0267") };

	for( uint n=0; n < 2; n++ )
	{
		printf("\n\ncontroller serial:  %s\n\n", mc[n]->GetSerial());

		MotorController::Variables var;
		memset(&var, 0, sizeof(MotorController::Variables));

		if( !mc[n]->ReadVariables(&var) )
		{
			printf("failed to read status of motor %u\n", n);
			return 0;
		}

		// print variables
		printf("errors:       ");

		if( var.errorStatus.safeStart )		printf("safe-start ");
		if( var.errorStatus.lowVIN )		printf("low-VIN ");
		if( var.errorStatus.overheat )		printf("overheat ");

		printf("\n");

		printf("targetSpeed:  %i\n", var.targetSpeed);
		printf("speed:        %i\n", var.speed);
		printf("brake:        %u\n", var.brake);
		printf("temperature:  %f degrees C\n", ((float)var.temperature * 0.1f));
		printf("voltage:      %f V\n", ((float)var.voltage * 0.001f));

		if( var.errorStatus.safeStart )
		{
			if( !mc[n]->ExitSafeStart() )
				printf("\n\nfailed to exit safe-start\n");
		}

		if( !mc[n]->SetSpeed(0) )
			printf("failed to set 0 speed\n");
	}
		
	evdevController* evc = new evdevController();
	
	if( !evc->Open() )
	{
		printf("failed to open bluetooth controller\n");
		return 0;
	}

	
	
#if 1
	float speed[] = { 0.0f, 0.0f };

	//bool speedChanged = true;

	#define MAX_SPEED 1600.0f

	/*
	 * main event loop
	 */
	while(true)
	{
		evc->Poll();

		//if( speedChanged )
		//{
		float speed[] = { evc->Axis[0] * -3200.0f, 
					   evc->Axis[1] * -3200.0f };

		for( int i=0; i < 2; i++ )
		{
			if( speed[i] < -MAX_SPEED )
				speed[i] = -MAX_SPEED;

			if( speed[i] > MAX_SPEED )
				speed[i] = MAX_SPEED;
		}

		mc[0]->SetSpeed(speed[0]);
		mc[1]->SetSpeed(speed[1]);
			//speedChanged = false;
		//}
	}
#endif


	delete usb;
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

