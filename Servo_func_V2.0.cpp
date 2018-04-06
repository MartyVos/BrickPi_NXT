#include "BrickPi3.cpp" // for BrickPi3
#include <iostream>
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
using namespace std;

BrickPi3 BP;
int power = 50;


void exit_signal_handler(int signo);

void SStop(){
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
}

void SForwards(){
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    BP.set_motor_power(PORT_B, power);
    BP.set_motor_power(PORT_C, power);
}

void SBackwards(){
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    BP.set_motor_power(PORT_B, -power);
    BP.set_motor_power(PORT_C, -power);
}

void SLeft(){
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    BP.set_motor_power(PORT_B, power);
    BP.set_motor_power(PORT_C, -power);
}

void SRight(){
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    BP.set_motor_power(PORT_B, -power);
    BP.set_motor_power(PORT_C, power);
}

bool detect(){

}

void turn(int degree){          //Positief: Rechts, Negatief: Links

}

void sensorTurn(int degree){   //Positief: Rechts, Negatief: Links

}

void follow(sensor_light_t &Light1, sensor_color_t &Color2, sensor_ultrasonic_t &Sonic3){
    int STATE = 0;
    int IR_max = 2200;
    int CL_max = 300;

    while(true){
        switch(STATE){
            case 0:
                if(Sonic3.cm <= 25){      //ZZ
                    cout << "STOP" << endl;
                    SStop();

                }else if(Light1.reflected >= IR_max && Color2.reflected_red <= CL_max){
                    SLeft();
                }else if(Light1.reflected >= IR_max && Color2.reflected_red > CL_max){                     //ZW
                    cout << "LEFT" << endl;
                    SLeft();
                }else if(Light1.reflected < IR_max && Color2.reflected_red <= CL_max){                     //WZ
                    cout << "RIGHT" << endl;
                    SRight();
                }else if(Light1.reflected < IR_max && Color2.reflected_red > CL_max){                      //WW
                    cout << "FORWARD" << endl;
                    SForwards();
                }else{
                    cout << "EXCEPTION" << endl;
                }
                STATE = 1;

            case 1:
                if(detect()){
                    STATE = 2;
                }else{
                    STATE = 0;
                }

            case 2:
                turn(45);       //45 naar graden rechts
                STATE = 3;

            case 3:
                sensorTurn(90); //90 graden naar rechts
                STATE = 4;

            case 4:
                if(detect()){
                    STATE = 6;
                }else{
                    STATE = 5;
                }

            case 5:
                turn(-90);      //90 graden naar links
                STATE = 0;

            case 6:
                turn(-45);      //45 graden naar links
                STATE = 7;

            case 7:
                if(detect()){
                    STATE = 8;
                }else{
                    STATE = 9;
                }

            case 8:
                SForwards();
                STATE = 7;

            case 9:
                turn(-45);
                STATE = 0;
        }
    }
}

void Sensor(){
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    sensor_light_t Light1;
    sensor_color_t Color2;
    sensor_ultrasonic_t Sonic3;

    BP.get_sensor(PORT_1, Light1);
    BP.get_sensor(PORT_2, Color2);
    BP.get_sensor(PORT_3, Sonic3);

    cout << "======================================"<< endl;
    cout <<  "IR:       " << Light1.reflected       << endl;
    cout <<  "Color:    " << Color2.reflected_red   << endl;
    cout <<  "Sonic:    " << Sonic3.cm              << endl;

    follow(Light1, Color2, Sonic3);
}

int main(){
    BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_LIGHT_ON);
    BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_COLOR_RED);
    BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_ULTRASONIC);

}

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_COLOR_OFF);
    exit(-2);
  }
}
