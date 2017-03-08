//#include "mbed.h"
#include "nRF24L01P.h"
#include "LSM303DLHC.h"
//#include <math.h>

const float ERROR_X = -0.01;
const float ERROR_Y = 0.10;
const float ERROR_Z = 0.94;
Timer t;

struct vector
{
  float x, y, z;
	
	// default + parameterized constructor
  vector(float x=0, float y=0, float z=0) : x(x), y(y), z(z){  }
	
	// assignment operator modifies object, therefore non-const
  vector& operator=(const vector& a)
  {
      x=a.x;
      y=a.y;
		  z=a.z;
      return *this;
  }
	
	// addop. doesn't modify object. therefore const.
  vector operator+(const vector& a) const
  {
      return vector(x+a.x, y+a.y, z+a.z);
  }
	
	// subop. doesn't modify object. therefore const.
  vector operator-(const vector& a) const
  {
      return vector(x-a.x, y-a.y, z-a.z);
  }
	
	// divop. doesn't modify object. therefore const.
  vector operator/(const vector& a) const
  {
      return vector(x/a.x, y/a.y, z/a.z);
  }
	
	// divop. doesn't modify object. therefore const. div by const
  vector operator/(const int& a) const
  {
      return vector(x/a, y/a, z/a);
  }
	
	void fixerr(const float& xerr, const float& yerr, const float& zerr){
		x -= xerr;
		y -= yerr;
		z -= zerr;
	}

	float abs(){
		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}
};
 

nRF24L01P my_nrf24l01p(p5, p6, p7, p8, p9, p10); // mosi, miso, sck, csn, ce, irq

//This uses the new 2013 library (limited function), Result is smaller numbers and more responsive, could be in terms of g??
//Printing float causes problems when compiling makefile.

Serial pc(USBTX, USBRX);

LSM303DLHC compass(p28, p27);
DigitalOut myled1(LED1);
DigitalOut myled2(LED2);

int main()
{
    pc.format(8, Serial::None, 1);
    pc.baud(115200);
    pc.printf("LSM303DLH Test\x0d\x0a");
	
	  vector zero(0,0,0);
		//zero.x = zero.y = zero.z = 0; //not needed

    // Define the number of samples to keep track of.  The higher the number,
    // the more the readings will be smoothed, but the slower the output will
    // respond to the input.  Using a constant rather than a normal variable lets
    // use this value to determine the size of the readings array.
    const int numReadings = 10;

    vector readings[numReadings]; // the readings from the acc sensor 3D array
    int readIndex = 0;         // the index of the current reading
    vector total = zero;             // the running total
    vector average = zero;           // the average

	  vector acc, mag, vel;
	  vel = zero;
	  float speed;
	
    const float dt = 0.1;
		const float g = 9.81;

    while (1)
    {
        t.start();
        compass.read(&acc.x, &acc.y, &acc.z, &mag.x, &mag.y, &mag.z);
				acc.fixerr(ERROR_X, ERROR_Y, ERROR_Z); //fixxacc error
				
			
        float heading = atan2(mag.y, mag.x);
        heading /= 2 * 3.14159;
        heading *= 360;
        heading += 180;

        //pc.printf("A: %4.2f %4.2f %4.2f   ", acc.x, acc.y, acc.z);
        pc.printf("M: %4.2f\t%4.2f\t%4.2f\n", mag.x, mag.y, mag.z);
        pc.printf("Heading is %f\n", heading);

				//Simple moving average, from Arduino website
				total = total - readings[readIndex]; 							// subtract the last reading:
				readings[readIndex] = acc; 												// read from the sensor:
				total = total + readings[readIndex]; 							// add the reading to the total:
				readIndex++; 																			// advance to the next position in the array:

				// if we're at the end of the array...
				readIndex = (readIndex >= numReadings) ? 0 : readIndex;
				//if (readIndex >= numReadings) {	readIndex = 0; }  // ...wrap around to the beginning

				average = total / numReadings; 										// calculate the average
				pc.printf("Average acc is %4.2f\t%4.2f\t%4.2f\n", average.x, average.y, average.z); // send it to the computer as ASCII digits
				pc.printf("The abs of avg acc is %f\n", average.abs()); // send it to the computer as ASCII digits
				if ((average.abs() >= 0.1)){
					//acc is greater than threashold
					vel.x += average.x * g * dt;
					vel.y += average.y * g * dt;
					vel.z += average.z * g * dt;
					speed = vel.abs();
					t.reset();
				}
				else {
					if(t >= 5){
					 vector c(vel.x * 0.2, vel.y * 0.2, vel.z * 0.2);
					 vel.x = c.x;
					 t.reset();
					}
				}
        pc.printf("V: %4.2f\t%4.2f\t%4.2f\n", vel.x, vel.y, vel.z);
        //pc.printf("Speed is %f\n", speed);
				
				
				pc.printf("The time taken was %f seconds\n", t.read());
				
				

        wait(dt);
    }
}

/*
void commSample(Serial pc)
{

// The nRF24L01+ supports transfers from 1 to 32 bytes, but Sparkfun's
//  "Nordic Serial Interface Board" (http://www.sparkfun.com/products/9019)
//  only handles 4 byte transfers in the ATMega code.
#define TRANSFER_SIZE 1

    char txData[TRANSFER_SIZE], rxData[TRANSFER_SIZE];
    int txDataCnt = 0;
    int rxDataCnt = 0;

    my_nrf24l01p.powerUp();

    // Display the (default) setup of the nRF24L01+ chip
    pc.printf("nRF24L01+ Frequency    : %d MHz\r\n", my_nrf24l01p.getRfFrequency());
    pc.printf("nRF24L01+ Output power : %d dBm\r\n", my_nrf24l01p.getRfOutputPower());
    pc.printf("nRF24L01+ Data Rate    : %d kbps\r\n", my_nrf24l01p.getAirDataRate());
    pc.printf("nRF24L01+ TX Address   : 0x%010llX\r\n", my_nrf24l01p.getTxAddress());
    pc.printf("nRF24L01+ RX Address   : 0x%010llX\r\n", my_nrf24l01p.getRxAddress());

    pc.printf("Type keys to test transfers:\r\n  (transfers are grouped into %d characters)\r\n", TRANSFER_SIZE);

    my_nrf24l01p.setTransferSize(TRANSFER_SIZE);

    my_nrf24l01p.setReceiveMode();
    my_nrf24l01p.enable();

    while (1)
    {

        // If we've received anything over the host serial link...
        if (pc.readable())
        {

            // ...add it to the transmit buffer
            txData[txDataCnt++] = pc.getc();

            // If the transmit buffer is full
            if (txDataCnt >= sizeof(txData))
            {

                // Send the transmitbuffer via the nRF24L01+
                my_nrf24l01p.write(NRF24L01P_PIPE_P0, txData, txDataCnt);

                txDataCnt = 0;
            }

            // Toggle LED1 (to help debug Host -> nRF24L01+ communication)
            myled1 = !myled1;
        }

        // If we've received anything in the nRF24L01+...
        if (my_nrf24l01p.readable())
        {

            // ...read the data into the receive buffer
            rxDataCnt = my_nrf24l01p.read(NRF24L01P_PIPE_P0, rxData, sizeof(rxData));

            // Display the receive buffer contents via the host serial link
            for (int i = 0; rxDataCnt > 0; rxDataCnt--, i++)
            {

                pc.putc(rxData[i]);
            }

            // Toggle LED2 (to help debug nRF24L01+ -> Host communication)
            myled2 = !myled2;
        }
    }
}*/