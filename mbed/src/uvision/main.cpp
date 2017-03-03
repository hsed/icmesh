#include "mbed.h"
#include "nRF24L01P.h"
#include "LSM303DLHC.h"

nRF24L01P my_nrf24l01p(p5, p6, p7, p8, p9, p10);    // mosi, miso, sck, csn, ce, irq

//This uses the new 2013 library (limited function), Result is smaller numbers and more responsive, could be in terms of g??
//Printing float causes problems when compiling makefile.

Serial pc(USBTX,USBRX);

LSM303DLHC compass(p28, p27);
DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
 
int main() {
   pc.format(8,Serial::None,1);
   pc.baud(115200);
   pc.printf("LSM303DLH Test\x0d\x0a");
   //compass.setOffset(29.50, -0.50, 4.00); // example calibration
   //compass.setScale(1.00, 1.03, 1.21);    // example calibration
   float ax, ay, az, mx, my, mz;
   while(1) {
     //hdg = compass.heading();
     compass.read(&ax, &ay, &az, &mx, &my, &mz);
     //debug.printf("Heading: %.2f\n", hdg);
     //debug.printf("A: %.2f", ax);
     pc.printf("A: %4.2f %4.2f %4.2f   ", ax, ay, az);
     pc.printf("M: %4.2f\t%4.2f\t%4.2f\n", mx, my, mz);
     wait(0.1);
     
   }
 }
 
 
void commSample(Serial pc) {

// The nRF24L01+ supports transfers from 1 to 32 bytes, but Sparkfun's
//  "Nordic Serial Interface Board" (http://www.sparkfun.com/products/9019)
//  only handles 4 byte transfers in the ATMega code.
#define TRANSFER_SIZE   1

   char txData[TRANSFER_SIZE], rxData[TRANSFER_SIZE];
   int txDataCnt = 0;
   int rxDataCnt = 0;

   my_nrf24l01p.powerUp();

   // Display the (default) setup of the nRF24L01+ chip
   pc.printf( "nRF24L01+ Frequency    : %d MHz\r\n",  my_nrf24l01p.getRfFrequency() );
   pc.printf( "nRF24L01+ Output power : %d dBm\r\n",  my_nrf24l01p.getRfOutputPower() );
   pc.printf( "nRF24L01+ Data Rate    : %d kbps\r\n", my_nrf24l01p.getAirDataRate() );
   pc.printf( "nRF24L01+ TX Address   : 0x%010llX\r\n", my_nrf24l01p.getTxAddress() );
   pc.printf( "nRF24L01+ RX Address   : 0x%010llX\r\n", my_nrf24l01p.getRxAddress() );

   pc.printf( "Type keys to test transfers:\r\n  (transfers are grouped into %d characters)\r\n", TRANSFER_SIZE );

   my_nrf24l01p.setTransferSize( TRANSFER_SIZE );

   my_nrf24l01p.setReceiveMode();
   my_nrf24l01p.enable();

   while (1) {

       // If we've received anything over the host serial link...
       if ( pc.readable() ) {

           // ...add it to the transmit buffer
           txData[txDataCnt++] = pc.getc();

           // If the transmit buffer is full
           if ( txDataCnt >= sizeof( txData ) ) {

               // Send the transmitbuffer via the nRF24L01+
               my_nrf24l01p.write( NRF24L01P_PIPE_P0, txData, txDataCnt );

               txDataCnt = 0;
           }

           // Toggle LED1 (to help debug Host -> nRF24L01+ communication)
           myled1 = !myled1;
       }

       // If we've received anything in the nRF24L01+...
       if ( my_nrf24l01p.readable() ) {

           // ...read the data into the receive buffer
           rxDataCnt = my_nrf24l01p.read( NRF24L01P_PIPE_P0, rxData, sizeof( rxData ) );

           // Display the receive buffer contents via the host serial link
           for ( int i = 0; rxDataCnt > 0; rxDataCnt--, i++ ) {

               pc.putc( rxData[i] );
           }

           // Toggle LED2 (to help debug nRF24L01+ -> Host communication)
           myled2 = !myled2;
       }
   }
}