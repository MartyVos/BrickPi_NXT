#include "BrickPi3.cpp" // for BrickPi3
#include <iostream>
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

void Servo(){
    int s = 2;
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    while(true){
        BP.set_motor_power(PORT_B, 25);
        BP.set_motor_power(PORT_C, 25);
        usleep(s*1000000);
        BP.set_motor_power(PORT_B, -25);
        BP.set_motor_power(PORT_C, -25);
        usleep(s*1000000);
      }
}

void IRsensor(){
    sensor_light_t Light1;
    BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_LIGHT_ON);
    while(true){
        if(BP.get_sensor(PORT_1, Light1) == 0){
            BP.get_sensor(PORT_1, Light1);
            cout << Light1.reflected << endl;
        }
        usleep(1*1000000);
    }
}

int main(){
    BP.detect();
    IRsensor();
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}
