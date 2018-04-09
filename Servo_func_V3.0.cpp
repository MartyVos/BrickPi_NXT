#include "BrickPi3.cpp" // for BrickPi3
#include <iostream>
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <cmath>
using namespace std;

BrickPi3 BP;
int power = 50;
int STATE = 0;


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

bool detect(sensor_ultrasonic_t &Sonic3){
    return (Sonic3.cm <= 25);
}

float degreesTimeC(float degree){
    return ((2.6/180) * abs(degree));
}

void turn(float degree){          //Positief: Rechts, Negatief: Links
    if(degree == 0){
        cout << "Degree:    " << degree << endl;
    }else if(degree < 0){
        float t = degreesTimeC(degree);
        SLeft();
        usleep(t*1000000);
    }else{
        float t = degreesTimeC(degree);
        SRight();
        usleep(t*1000000);
    }
}

void sensorTurn(int degree){   //Positief: Rechts, Negatief: Links
    BP.set_motor_position(PORT_D, degree);
}

void follow(sensor_light_t &Light1, sensor_color_t &Color2, sensor_ultrasonic_t &Sonic3){
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    int IR_max = 2200;
    int CL_max = 300;

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
            cout << "STATE 0" << endl;
            STATE = 1;

        case 1:
            cout << "STATE 1" << endl;
            if(detect(Sonic3)){
                STATE = 2;
                break;
            }else{
                STATE = 0;
                break;
            }

        case 2:
            turn(45);       //45 naar graden rechts
            SForwards();
            cout << "STATE 2" << endl;
            STATE = 3;
            break;

        case 3:
            usleep(1.5*1000000);
            cout << "STATE 3" << endl;
            turn(-45);
            SForwards();
            STATE = 4;
            break;

        case 4:
            cout << "STATE 4" << endl;
            sensorTurn(-90);
            STATE = 5;
            break;

        case 5:
            cout << "STATE 5" << endl;
            if(detect(Sonic3)){
                SForwards();
                break;
            }else{
                STATE = 6;
                break;
            }

        case 6:
            usleep(1.5*1000000);
            cout << "STATE 6" << endl;
            turn(-50);      //50  graden naar links
            SForwards();
            STATE = 7;
            break;

        case 7:
            usleep(1*1000000);
            cout << "STATE 7" << endl;
            sensorTurn(0);
            STATE = 0;

        default:
            STATE = 0;
            break;
    }
}

void Sensor(){
    usleep(1*100000);
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
    while(true){
        Sensor();
    }


}

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_COLOR_OFF);
    exit(-2);
  }
}
