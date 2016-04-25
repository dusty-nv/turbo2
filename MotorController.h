/**
 * rover
 */

#ifndef __ROVER_MOTOR_CONTROLLER_H
#define __ROVER_MOTOR_CONTROLLER_H


#include "Config.h"
#include "stdio.h"

#include <string>
#include <libusb-1.0/libusb.h>


/**
 * MotorController
 */
class MotorController
{
public:
	/**
	 * constructor
	 */
	MotorController( libusb_device* device );

	/**
	 * destructor
	 */
	~MotorController();

	/**
	 * Open
	 */
	bool Open();

	/**
	 * Close
	 */
	void Close();

	/**
	 * GetTemperature
	 */
	//float GetTemperature( bool fahrenheit=true );

	/**
	 * GetVoltage
	 */
	//float GetVoltage();

	/**
	 * Usb Vendor Id
	 */
	static const uint16_t VendorId = 0x1ffb;		// Pololu Corporation

	/**
	 * Usb Product Id
	 */
	static const uint16_t ProductId_24v12 = 0x9A;
	static const uint16_t ProductId_18v15 = 0x98;

	/**
	 * ErrorStatus
	 */
	struct ErrorStatus
	{
		uint16_t safeStart 		: 1;
		uint16_t requiredChannel 	: 1;
		uint16_t serial 			: 1;
		uint16_t timeout 			: 1;
		uint16_t killSwitch 		: 1;
		uint16_t lowVIN 			: 1;
		uint16_t highVIN 			: 1;
		uint16_t overheat 		: 1;
		uint16_t motorDriver 		: 1;
		uint16_t errLine 			: 1;
		uint16_t reserved			: 6;
	} __attribute__((packed));

	/**
	 * GetErrorStatus
	 */
	//ErrorStatus GetErrorStatus();


	/**
	 * SerialError
	 */
	struct SerialError
	{
		/**
		 * Reserved0
		 */
		uint16_t reserved0 : 1;

		/**
		 * De-synchronization or excessive noise on the RX line is detected.
		 */
		uint16_t frame : 1;

		/**
		 * Noise is detected on the RX line.
		 */
		uint16_t noise : 1;

		/**
		 * The buffer for storing uint8_ts received on the RX line is full and data was lost as a result.
		 */
		uint16_t rxOverflow : 1;

		/**
		 * The serial uint8_ts received on RX do not obey the protocol.
		 */
		uint16_t format : 1;

		/**
		 * This error occurs if you have enabled cyclic redundancy check (CRC) for serial commands, but the 
		 * CRC uint8_t received was invalid.
		 */
		uint16_t crc : 1;

		/**
		 * Reserved
		 */
		uint16_t reserved	: 10;

	} __attribute__((packed));
	

	/**
	 * LimitStatus
	 */
	struct LimitStatus
	{
		/**
		 * Motor is not allowed to run due to an error or safe-start violation.
		 */
		uint16_t safeStart : 1;

		/**
	 	 * Temperature is active reducing target speed.
		 */
		uint16_t overheat : 1;

		/**
		 * Max speed limit is actively reducing target speed (target speed > max speed).
		 */
		uint16_t maxSpeed : 1;

		/**
		 * Starting speed limit is actively reducing target speed to zero (target speed < starting speed).
		 */
		uint16_t startSpeed : 1;

		/**
		 * Motor speed is not equal to target speed because of acceleration, deceleration, or brake duration limits.
		 */
		uint16_t acceleration : 1;

		/**
		 * RC1 is configured as a limit/kill switch and the switch is active (scaled value ≥ 1600).
		 */
		uint16_t killRC1 : 1;

		/**
		 * RC2 is configured as a limit/kill switch and the switch is active (scaled value ≥ 1600).
		 */
		uint16_t killRC2 : 1;
		
		/**
		 * AN1 is configured as a limit/kill switch and the switch is active (scaled value ≥ 1600).
		 */
		uint16_t killAN1 : 1;

		/**
		 * AN2 is configured as a limit/kill switch and the switch is active (scaled value ≥ 1600).
		 */
		uint16_t killAN2 : 1;

		/**
		 * USB kill switch is configured and active
		 */
		uint16_t killUSB : 1;

		/**
		 * Reserved
		 */
		uint16_t reserved : 6;

	} __attribute__((packed));


    struct ChannelStatus
    {
        /**
		 * The raw value of the channel as read from the pin.
		 * This is mainly useful for the control input setup wizard.
		 * 0xFFFF if disconnected but not affected by absolute max/min limits.
		 * Units of quarter-microseconds if an RC channel.
		 * 12-bit ADC reading (0-4095) if analog.
		 * 0xFFFF if input is disconnected.
		 */
        uint16_t unlimitedRawValue;

		/**
		 * This is just like unlimitedRawValue except that it will be 0xFFFF
		 * if the absolute max/min limits are violated.
		 */
		uint16_t rawValue;

		/**
		 * The result of scaling the rawValue.  This value depends on all the scaling settings for the channel.
         */
		uint16_t scaledValue;

		/**
		 * Reserved
		 */
		uint16_t reserved;

    } __attribute__((packed));


    struct MotorLimits
    {
		/**
		 * Max Speed is a number between 0 and 3200 that specifies the maximum speed at which the motor controller 
		 * will ever drive the motor.
		 */
        uint16_t maxSpeed;

		/**
		 * Max Acceleration is a number between 0 and 3200 that specifies how much the magnitude (absolute value) 
		 * of the motor speed is allowed to increase every speed update period.  0 means no limit.
		 */
		uint16_t maxAcceleration;

		/**
		 * Max Deceleration is a number between 0 and 3200 that specifies how much the magnitude (absolute value) 
		 * of the motor speed is allowed to decrease every speed update period.  0 means no limit.
		 */
		uint16_t maxDeceleration;

		/**
		 * Brake duration is the time, in milliseconds, that the motor controller will spend braking the motor (Current Speed = 0) 
		 * before allowing the Current Speed to change signs.
		 */
		uint16_t brakeDuration;

		/**
		 * Minimum non-zero speed (1-3200).  In RC or Analog mode, a scaled value of 1 maps to this speed.
		 * 0 means no effect.
		 */
        uint16_t startingSpeed;

		/**
		 * Reserved
		 */
		uint16_t reserved;

    } __attribute__((packed));

	struct Variables
    {
		ErrorStatus errorStatus;
		ErrorStatus errorOccurred;

		SerialError serialError;

		LimitStatus limitStatus;

		ChannelStatus rc1;
		ChannelStatus rc2;
		ChannelStatus analog1;
		ChannelStatus analog2;

		int16_t targetSpeed;			// Target speed of motor from -3200 to 3200.
		int16_t speed;				
		uint16_t brake;				// Current braking amount, from 0 to 32. This value is only relevant when speed==0, otherwise it will be 0xFF.

		uint16_t voltage;
		uint16_t temperature;

		uint16_t reserved;

		uint16_t rcPeriod;
		uint16_t baudRate;
		
		uint32_t uptime;

        MotorLimits forwardLimits;
		MotorLimits reverseLimits;

    } __attribute__((packed));

	enum Direction
    {
        DIRECTION_FORWARD = 0,
        DIRECTION_REVERSE = 1,
        DIRECTION_BRAKE   = 2
    };

	bool SetSpeed( int value );
	bool SetSpeed( Direction dir, int value );

	bool ReadVariables( Variables* var );
	void PrintVariables();
	
	bool ExitSafeStart();

	inline const char* GetSerial() const								{ return mSerial.c_str(); }

protected:
	

	/*bool Send( void* data, uint size );
	bool Senduint8_t( uint8_t value );
	uint Recieve( void* buffer, uint size );
	bool GetVariable( Variable var, void* buffer, uint size );*/

	bool ControlTransfer( uint8_t requestType, uint8_t request, void* data, uint size );
	bool ControlTransfer( uint8_t requestType, uint8_t request, uint value, uint index, void* data, uint size );
	bool ControlTransfer( uint8_t requestType, uint8_t request, uint value, uint index );

	Variables mVariables;

	libusb_device* mDevice;
	libusb_device_handle* mDeviceCtx;

	std::string mSerial;
};



#if 0
	enum Variable
	{
		/**
		 * The set bits of this variable indicate the errors that are currently stopping the motor. 
		 * The motor can only be driven when this register has a value of 0.
		 * @see ErrorStatus
		 */
		VAR_ERROR_STATUS = 0,

		/**
		 * The set bits of this register indicate the errors that have occurred since this register was last cleared. 
		 * This status register has the same bit assignments as the Error Status register documented above. 
		 * Reading this variable clears all of the bits.
		 * @see ErrorStatus
		 */
		VAR_ERROR_OCCURRED = 1,

		/**
		 * The set bits of this variable indicate the serial errors that have occurred since this variable was last cleared. 
		 * Reading this variable clears all of the bits.
		 * @see SerialError
		 */
		VAR_ERROR_SERIAL = 2,

		/**
		 * The set bits of this variable indicate things that are currently limiting the motor controller.
		 */
		VAR_LIMIT_STATUS   = 3,
		
		/**
		 * Motor target speed (-3200 to +3200) requested by the controlling interface.
		 * @see LimitStatus
		 */
		VAR_TARGET_SPEED = 20,

		/**
		 * Current speed of the motor (-3200 to +3200).
		 */
		VAR_SPEED = 21,

		/**
		 * When Speed=0, this variable indicates how hard the controller is braking with a value from 0 (full coast) 
		 * to 32 (full brake). Otherwise, it has a value of 0xFF (255). The high uint8_t of this variable is always zero.
		 */
		VAR_BRAKE = 22,

		/**
		 * Measured voltage on the VIN pin, in mV.
		 */
		VAR_VOLTAGE = 23,

		/**
		 * Board temperature as measured by a temperature sensor near the motor driver, in 0.1 degrees Celcius. 
		 * Temperatures below freezing are reported as 0.
		 */
		VAR_TEMPERATURE = 24,

		/**
		 * Two lower uint8_ts of the number of milliseconds that have elapsed since the controller was last reset or powered up.
		 */
		VAR_UPTIME_LOW = 28,

		/**
		 * Two upper uint8_ts of the number of milliseconds that have elapsed since the controller was last reset or powered up.
		 */
		VAR_UPTIME_HIGH = 29
	};
#endif

#endif
