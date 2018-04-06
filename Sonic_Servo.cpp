#include "BrickPi3.cpp" // for BrickPi3
#include <iostream>
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
using namespace std;

BrickPi3 BP;
uint8_t power = 30;


void exit_signal_handler(int signo);

void core(){
    signal(SIGINT, exit_signal_handler);
    sensor_ultrasonic_t  Sonic3;
    BP.set_motor_position(PORT_D, -45);
    usleep(1000000);
    while(true){
        BP.set_motor_limits(PORT_D, 30,0);
        BP.set_motor_position(PORT_D, -45);
        BP.get_sensor(PORT_3, Sonic3);
        cout << "Sonic:\t\t" << Sonic3.cm <<
        endl << "Position:\t" << BP.get_motor_encoder(PORT_D) <<
        endl << "=================================" << endl;
        usleep(1500000);//sleep 50 ms

        BP.set_motor_position_relative(PORT_D, 45);
        BP.get_sensor(PORT_3, Sonic3);
        cout << "Sonic:\t\t" << Sonic3.cm <<
        endl << "Position:\t" << BP.get_motor_encoder(PORT_D) <<
        endl << "=================================" << endl;
        usleep(1500000);//sleep 50 ms

        BP.set_motor_position_relative(PORT_D, 45);
        BP.get_sensor(PORT_3, Sonic3);
        cout << "Sonic:\t\t" << Sonic3.cm <<
        endl << "Position:\t" << BP.get_motor_encoder(PORT_D) <<
        endl << "=================================" << endl;
        usleep(1500000);

        BP.set_motor_position_relative(PORT_D, -45);
        BP.get_sensor(PORT_3, Sonic3);
        cout << "Sonic:\t\t" << Sonic3.cm <<
        endl << "Position:\t" << BP.get_motor_encoder(PORT_D) <<
        endl << "=================================" << endl;
        usleep(1500000);
    }
}

int main(){
    BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_ULTRASONIC);
    core();
}

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}
