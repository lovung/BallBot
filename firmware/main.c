#include "include.h"
#include "stm32f4xx_it.h"


volatile uint8_t PWM_motorADutyCycle = 30;
volatile uint8_t PWM_motorBDutyCycle = 30;
volatile uint8_t PWM_motorCDutyCycle = 30;
volatile uint8_t PWM_motorDDutyCycle = 30;

volatile bool enableIMUInterrupt = false;
volatile bool enableEncoderInterrupt = false;
volatile bool enablePIDInterrupt = false;
volatile bool enableLargerPIDInterrupt = false;
volatile bool enableLQGInterrupt = false;

extern double globalTheta[3];
extern double globalThetaDot[3];

volatile double globalRoll, globalPitch;
volatile double gyroXrate, accXangle, gyroYrate, accYangle, gyroZrate, accZangle, roll, pitch;
extern char str_main[50];
//Quaternion q;           // [w, x, y, z]         quaternion container
extern float euler[3];         // [psi, theta, phi]    Euler angle container


void GPIOConfig(void);
uint32_t milis(void);
void _delay_ms(uint32_t ms);

int main(void)
{
	SystemCoreClockUpdate(); 
	SysTick_Config(SystemCoreClock/1000);
	GPIOConfig();
	initButtonOnDiscoveryBoard();
	IIC_Init();
	MPU6050(0xD0);
	MPUinitialize();
	IMUGetValueGyroOffset();
	
#ifdef TESTMOTOR
	PWMConfig();
  	motorDirConfig();
#endif

#ifdef TESTUART
	TM_USART_Init(UART_CHANNEL, UART_PINSPACK, UART_BAUDRATE);
#endif 

#ifdef TESTIMU
	IIC_Init();
	MPU6050(0xD0);
	MPUinitialize();
	IMUGetValueGyroOffset();
	if(MPUtestConnection() == SUCCESS )
	{
		GPIO_SetBits(LEDA_BASE, LEDA_PIN);
	}
	else
	{
		GPIO_SetBits(LEDA_BASE, LEDB_PIN );
	} 
#endif

#ifdef USED_PID

#endif

#ifdef USED_LQR

#endif

	while (1)
	{
		
#ifdef TESTMOTOR
		PWMControl(MOTOR_A, CLOCKWISE, PWM_motorADutyCycle);
		PWMControl(MOTOR_B, COUNTER_CLOCKWISE, PWM_motorBDutyCycle);
#endif

#ifdef TESTENCODER
		if (enableEncoderInterrupt)
		{
			encoderInterrupt();
		}
#endif

#ifdef TESTUART
		TM_USART_Puts(UART_CHANNEL, "Hello\n");	
#endif

#ifdef TESTIMU
		if (enableIMUInterrupt)
		{
				enableIMUInterrupt = false;
				feedbackIMUProcess();
				IMUGetAngle();
				roll = kalmanCalculate(globalTheta[AXIS_X],globalGyroAngle[AXIS_X], SAMPLE_TIME_IMU);
				IMUFilter();
		}

#endif

#ifdef USED_PID

#endif

#ifdef USED_LQR

#endif
	}

}

void GPIOConfig(void)
{
	



  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = LEDA_PIN|LEDB_PIN|LEDC_PIN|LEDD_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(LEDA_BASE, &GPIO_InitStructure);
}

