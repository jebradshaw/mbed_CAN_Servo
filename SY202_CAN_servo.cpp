// Program to test the CAN bus using pins 29 and 30 on the mbed connected
// to an MCP2551 CAN transceiver bus IC
//  Note that this program will only continue to transmit while the TX message is <= 8 bytes long

#include "mbed.h"
#include "ServoOut.h"

#define THIS_CAN_ID     1   //Address of this CAN device

Serial pc(USBTX, USBRX);    //tx, and rx for tera term
DigitalOut led1(LED1);      //heartbeat
DigitalOut led2(LED2);      //CAN read activity
DigitalOut led3(LED3);      //CAN write activity

CAN can(p30,p29);      //CAN interface
ServoOut servo(p15);    //servo output

Ticker pulse;

void copyCANdataStr(unsigned char* canData, char* str){
    for(int i=0;i<8;i++){
        str[i] = canData[i];    
    }    
}

void alive(void){
    led1 = !led1;
    if(led1)
        pulse.attach(&alive, .2); // the address of the function to be attached (flip) and the interval (2 seconds)     
    else
        pulse.attach(&alive, 1.3); // the address of the function to be attached (flip) and the interval (2 seconds)
}

void testServo(void){
    servo = 1500;
    wait(.2);
    for(int i=1000;i<2000;i++){
        servo = i;
        wait(.001);
    }
    servo = 1500;
    wait(.2);    
}

int main() {
    char datastr[20];
    CANMessage msg_read;
    int servoPos;
    wait(.2);
    
    pulse.attach(&alive, 2.0); // the address of the function to be attached (alive) and the interval (2 seconds)
    can.frequency(500000);
    pc.baud(115200);                
    
    pc.printf("%s\r\n", __FILE__);
    
    testServo();
        
    while(1) {        
        while(can.read(msg_read)){ //if message is available, read into msg
            if(msg_read.id == THIS_CAN_ID){                
                if(msg_read.data[0] == 's' || msg_read.data[0] == 'S'){
                    //strcpy(datastr, msg_read.data);
                    copyCANdataStr(msg_read.data, datastr);
                    int err = sscanf(datastr, "s%d", &servoPos);
                    servo = servoPos;
                    pc.printf("\r\n%d Message Received: %s", msg_read.id, msg_read.data);
                    led2 = !led2;
                }//if the first letter was an 's', it is a servo command
            }   //was the message to this device?
        }//while a CAN message has been read                         
    }//while(1)
}//main
