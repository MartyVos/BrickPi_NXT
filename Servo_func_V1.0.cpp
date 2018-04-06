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

// Toegevoegd door Derk
void SDodge(){
    // Draai 45 graden links
    // Draai afstandssensor 45 graden rechts
    //      Als afstandssensor iets ziet tijdens afrijden S kant, draai robot 45 graden links en blijf rijden totdat waardes toenemen, draai vervolgens weer 45 graden rechts.
    // Na Robot S kant heeft agereden, draai 45 graden rechts
    // draai afstandssensor 45 graden rechts
    //      Als sensor nog iets waarneemt, blijf rechtdoor rijden totdat waarden sensor toenemen
    // Als waarden sensor zijn afgenomen, draai robot 45 graden naar rechts en rij dezelfde S kant afstand + eventueel extra afstand gereden door langwerpig obstakel
    // Nadat robot afstand S (+ E) heeft afgelegd, draai 45 graden links en hervat lijn volgen

}

void follow(sensor_light_t &Light1, sensor_color_t &Color2, sensor_ultrasonic_t &Sonic3){
    int IR_max = 2200;
    int CL_max = 300;


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
}

void Sensor(){
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    sensor_light_t Light1;
    sensor_color_t Color2;
    sensor_ultrasonic_t Sonic3;

    //RGB.set_sens\r_type(PORT_2, SENSOR_TYPE_NXT_COLOR_FULL);
    //BP.get_sensor(PORT_2, Color1);

    BP.get_sensor(PORT_1, Light1);
    BP.get_sensor(PORT_2, Color2);
    BP.get_sensor(PORT_3, Sonic3);

    cout << "======================================"<< endl;
    cout <<  "IR:       " << Light1.reflected       << endl;
    cout <<  "Color:    " << Color2.reflected_red   << endl;
    cout <<  "Sonic:    " << Sonic3.cm              << endl;

    //usleep(1*100000);
    follow(Light1, Color2, Sonic3);
}

void turn(){
    signal(SIGINT, exit_signal_handler);
    BP.set_motor_position(PORT_D, 0);
    usleep(1000000*1);
    BP.set_motor_position_relative(PORT_D, 90);
    usleep(1000000*3);
    BP.set_motor_position_relative(PORT_D, -180);
    usleep(1000000*3);
}


int main(){
    BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_LIGHT_ON);
    BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_COLOR_RED);
    BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_ULTRASONIC);
    while(true){
        Sensor();
        //turn();
    }
}

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_COLOR_OFF);
    exit(-2);
  }
}
