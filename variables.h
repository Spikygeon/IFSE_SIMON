/*
 * variables.h
 *
 *  Created on: May 23, 2021
 *      Author: spikygeon
 */


#include <stdio.h>
#include <unistd.h>
#include "GPIOuniv.h"
#include "PWMuniv.h"
#include <sstream>
#include <fstream>

using namespace BBB;
using namespace std;

#define MAX_PATRON 10
#define POT_SPEED 	"/sys/bus/iio/devices/iio:device0/in_voltage0" //p9_39
#define POT_TIME 	"/sys/bus/iio/devices/iio:device0/in_voltage2" //p9_37

#define RED_BUTTON			1
#define GREEN_BUTTON		2
#define BLUE_BUTTON			3
#define YELLOW_BUTTON		4
#define START_PAUSE_BUTTON	5

//Salidas LEDs
GPIO led_RED_GPIO(GPIO::P8_8);   			//led rouge P8_8
GPIO led_YELLOW_GPIO(GPIO::P8_10);			//led amarillo P8_10
GPIO led_GREEN_GPIO(GPIO::P8_12);   		//led verde P8_12
GPIO led_BLUE_GPIO(GPIO::P8_14);			//led azul P_14
GPIO led_START_PAUSE_GPIO(GPIO::P8_16); 	//led pausa-start P8_16

//Entradas Buttons
GPIO button_RED_GPIO(GPIO::P8_7);    		//pulsador rojo P8_7
GPIO button_YELLOW_GPIO(GPIO::P8_9);    	//Plaster amarillo P8_9
GPIO button_GREEN_GPIO(GPIO::P8_11);		//pulsador verde P8_11
GPIO button_BLUE_GPIO(GPIO::P8_15);			//pulsador azul P8_15
GPIO button_START_PAUSE_GPIO(GPIO::P8_26);	//pulsador inicio/Pause P9_22

PWM Motor_Time(PWM::P9_22); //P9_22 pwm del motor time
PWM Motor_Speed(PWM::P9_21); //P9_21 pwm del motor Speed
PWM Zumbador(PWM::P9_14);

int eventCounter;	//TODO: Borrar

class Variables {
private:

	pthread_mutex_t mutex_variables;//mutex de las variable
	int patron_sequence[MAX_PATRON];
	int pot_SPEED;
	int pot_TIME;
	int button_RED;
	int button_BLUE ;
	int button_GREEN;
	int button_YELLOW;
	int button_START_PAUSE;

	int game_speed;
	int game_time;
	int timer_value;

public:

	///////////////////////////////////////////////////////////////////
	//CONSTRUCTOR
	Variables(){ // @suppress("Class members should be properly initialized")
		led_RED_GPIO.setDirection(GPIO::OUTPUT);
		led_YELLOW_GPIO.setDirection(GPIO::OUTPUT);
		led_GREEN_GPIO.setDirection(GPIO::OUTPUT);
		led_BLUE_GPIO.setDirection(GPIO::OUTPUT);
		led_START_PAUSE_GPIO.setDirection(GPIO::OUTPUT);
		button_RED_GPIO.setDirection(GPIO::INPUT);
		button_YELLOW_GPIO.setDirection(GPIO::INPUT);
		button_GREEN_GPIO.setDirection(GPIO::INPUT);
		button_BLUE_GPIO.setDirection(GPIO::INPUT);
		button_START_PAUSE_GPIO.setDirection(GPIO::INPUT);

		Motor_Speed.setPolarity(PWM::ACTIVE_HIGH);
		Motor_Speed.setPeriod(20000000);  //50Hz
		Motor_Speed.run();

	}
	///////////////////////////////////////////////////////////////////

	//READ HARDWARE FUNCTIONS

	int read_HW_button_RED() {
		int aux = button_RED_GPIO.getValue();
		//printf("read_HW_button_RED:\t%d\n", aux);
		set_button_RED(aux);
		//printf("get_button_RED:\t%d\n", get_button_RED());
		return aux;
	}

	int read_HW_button_GREEN() {
		int aux = button_GREEN_GPIO.getValue();
		set_button_GREEN(aux);
		return aux;
	}

	int read_HW_button_BLUE() {
		int aux = button_BLUE_GPIO.getValue();
		set_button_BLUE(aux);
		return aux;
	}

	int read_HW_button_YELLOW() {
		int aux = button_YELLOW_GPIO.getValue();
		set_button_YELLOW(aux);
		return aux;
	}

	int read_HW_button_INIT_PAUSE() {
		int aux = button_START_PAUSE_GPIO.getValue();
		set_button_START_PAUSE(aux);
		return aux;
	}

	int read_HW_pot_TIME() {
		stringstream ss;
		fstream fs;
		ss << POT_TIME << "_raw";
		fs.open(ss.str().c_str(), fstream::in);
		int sample;
		fs >> sample;
		fs.seekg(0);
		return sample;
	}


	int read_HW_pot_SPEED() {
		stringstream ss;
		fstream fs;
		ss << POT_SPEED << "_raw";
		fs.open(ss.str().c_str(), fstream::in);
		int sample;
		fs >> sample;
		fs.seekg(0);
		return sample;
	}


	void read_all_HW_buttons(void){
		int auxValue;
		auxValue = read_HW_button_RED();
		auxValue = read_HW_button_GREEN();
		auxValue = read_HW_button_BLUE();
		auxValue = read_HW_button_YELLOW();
		auxValue = read_HW_button_INIT_PAUSE();
	}

	//WRITE HARDWARE FUNCTIONS
	void write_HW_led_RED(GPIO::VALUE value) {
		led_RED_GPIO.setValue(value);
	}

	void write_HW_led_GREEN(GPIO::VALUE value) {
		led_GREEN_GPIO.setValue(value);
	}

	void write_HW_led_BLUE(GPIO::VALUE value) {
		led_BLUE_GPIO.setValue(value);
	}

	void write_HW_led_YELLOW(GPIO::VALUE value) {
		led_YELLOW_GPIO.setValue(value);
	}

	void write_HW_led_START_PAUSE(GPIO::VALUE value) {
		led_START_PAUSE_GPIO.setValue(value);
	}

	void write_HW_led_colour_ON(int colour) {
		switch(colour) {

		case RED_BUTTON:
			write_HW_led_RED(GPIO::HIGH);
			break;

		case GREEN_BUTTON:
			write_HW_led_GREEN(GPIO::HIGH);
			break;

		case BLUE_BUTTON:
			write_HW_led_BLUE(GPIO::HIGH);
			break;

		case YELLOW_BUTTON:
			write_HW_led_YELLOW(GPIO::HIGH);
			break;

		case START_PAUSE_BUTTON:
			write_HW_led_START_PAUSE(GPIO::HIGH);
			break;

		default:
			//printf("write_HW_led_colour: Nunca debería entrar aqui\n");
			break;
		}
	}
	void write_HW_led_ALL_ON(void) {
		write_HW_led_RED(GPIO::HIGH);
		write_HW_led_GREEN(GPIO::HIGH);
		write_HW_led_BLUE(GPIO::HIGH);
		write_HW_led_YELLOW(GPIO::HIGH);
	}
	void write_HW_led_ALL_OFF(void) {
		write_HW_led_RED(GPIO::LOW);
		write_HW_led_GREEN(GPIO::LOW);
		write_HW_led_BLUE(GPIO::LOW);
		write_HW_led_YELLOW(GPIO::LOW);
	}

	void write_HW_MOTOR_SPEED(unsigned duty_cycle){
		Motor_Speed.setPolarity(PWM::ACTIVE_HIGH);
		Motor_Speed.setPeriod(20000000);  //50Hz
		Motor_Speed.run();
		Motor_Speed.setDutyCycle(duty_cycle);
	}

	void write_HW_MOTOR_TIME(unsigned duty_cycle){
		Motor_Time.setPolarity(PWM::ACTIVE_HIGH);
		Motor_Time.setPeriod(20000000);  //50Hz
		Motor_Time.run();
		Motor_Time.setDutyCycle(duty_cycle);
	}
	void write_HW_ZUMBADOR(int period){
		unsigned int value = period;
		Zumbador.setPolarity(PWM::ACTIVE_HIGH);
		Zumbador.setPeriod(20000);
		Zumbador.setDutyCycle(value);
		Zumbador.run();

	}
	///////////////////////////////////////////////////////////////////

	//SETER PROTECTED VARIABLES
	void set_button_RED(int value) {
		pthread_mutex_lock(&mutex_variables);
		button_RED = value;
		pthread_mutex_unlock(&mutex_variables);
	}

	void set_button_GREEN(int value) {
		pthread_mutex_lock(&mutex_variables);
		button_GREEN = value;
		pthread_mutex_unlock(&mutex_variables);
	}

	void set_button_BLUE(int value) {
		pthread_mutex_lock(&mutex_variables);
		button_BLUE = value;
		pthread_mutex_unlock(&mutex_variables);
	}

	void set_button_YELLOW(int value) {
		pthread_mutex_lock(&mutex_variables);
		button_YELLOW = value;
		pthread_mutex_unlock(&mutex_variables);
	}

	void set_button_START_PAUSE(int value) {
		pthread_mutex_lock(&mutex_variables);
		button_START_PAUSE = value;
		pthread_mutex_unlock(&mutex_variables);
	}

	void set_pot_SPEED(int value) {
		pthread_mutex_lock(&mutex_variables);
		pot_SPEED = value;
		pthread_mutex_unlock(&mutex_variables);
	}

	void set_pot_TIME(int value) {
		pthread_mutex_lock(&mutex_variables);
		pot_TIME = value;
		pthread_mutex_unlock(&mutex_variables);
	}

	void set_timer_value(int value) {
		pthread_mutex_lock(&mutex_variables);
		timer_value = value;
		pthread_mutex_unlock(&mutex_variables);
	}


	//SETTER COMMON VARIABLES
	void set_game_speed(int value) {
		game_speed = value*1000000;
	}

	void set_game_time(int value) {
		game_time = value;
	}

	///////////////////////////////////////////////////////////////////

	//GETTER PROTECTED VARIABLES
	int get_button_RED() {
		pthread_mutex_lock(&mutex_variables);
		int aux = button_RED;
		pthread_mutex_unlock(&mutex_variables);
		return aux;
	}

	int get_button_GREEN() {
		pthread_mutex_lock(&mutex_variables);
		int aux = button_GREEN;
		pthread_mutex_unlock(&mutex_variables);
		return aux;
	}

	int get_button_BLUE() {
		pthread_mutex_lock(&mutex_variables);
		int aux = button_BLUE;
		pthread_mutex_unlock(&mutex_variables);
		return aux;
	}

	int get_button_YELLOW() {
		pthread_mutex_lock(&mutex_variables);
		int aux = button_YELLOW;
		pthread_mutex_unlock(&mutex_variables);
		return aux;
	}

	int get_button_START_PAUSE() {
		pthread_mutex_lock(&mutex_variables);
		int aux = button_START_PAUSE;
		pthread_mutex_unlock(&mutex_variables);
		return aux;
	}

	int get_pot_SPEED() {
		pthread_mutex_lock(&mutex_variables);
		int aux = pot_SPEED;
		pthread_mutex_unlock(&mutex_variables);
		return aux;
	}
	int get_pot_TIME() {
		pthread_mutex_lock(&mutex_variables);
		int aux = pot_TIME;
		pthread_mutex_unlock(&mutex_variables);
		return aux;
	}

	//GETTER COMMON VARIABLES
	int get_timer_value(void) {
		pthread_mutex_lock(&mutex_variables);
		int aux = timer_value;
		pthread_mutex_unlock(&mutex_variables);
		return aux;
	}

	int get_timer_value_millis(void) {
		pthread_mutex_lock(&mutex_variables);
		int aux = timer_value*1000;
		pthread_mutex_unlock(&mutex_variables);
		return aux;
	}
	int get_game_speed(void) {
		return game_speed;
	}

	int get_game_time(void) {
		return game_time;
	}

	///////////////////////////////////////////////////////////////////

	//AUXILIAR FUNCTIONS
	int get_button_pressed() {
		int aux = 0;
		if(get_button_RED()) {
			aux = RED_BUTTON;
			//printf( "getBottonPressed: RED PRESSED\n");
		}
		if(get_button_GREEN()) {
			aux = GREEN_BUTTON;
			//printf( "getBottonPressed: GREEN PRESSED\n");
		}
		if(get_button_BLUE()) {
			aux = BLUE_BUTTON;
			//printf( "getBottonPressed: BLUE PRESSED\n");
		}
		if(get_button_YELLOW()) {
			aux = YELLOW_BUTTON;
			//printf( "getBottonPressed: YELLOW PRESSED\n");
		}
		if(get_button_START_PAUSE()) {
			aux = START_PAUSE_BUTTON;
			//printf( "getBottonPressed: PAUSE PRESSED\n");
		}
		return aux;
	}
};
