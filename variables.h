/*
 * variables.h
 *
 *  Created on: May 23, 2021
 *      Author: spikygeon
 */


#include <stdio.h>
#include <unistd.h>
#include "GPIO.h"

using namespace BBB;
using namespace std;

#define MAX_PATRON 10

#define POT_SPEED 	"/sys/bus/iio/devices/iio:device0/in_voltage0" //p9_39
#define POT_TIME 	"/sys/bus/iio/devices/iio:device0/in_voltage2" //p9_37

int eventCounter;	//TODO: Borrar

//Salidas LEDs
GPIO led_RED_GPIO(67);   	//led Rojo 		P8_8
GPIO led_YELLOW_GPIO(68);   //led Amarillo 	P8_10
GPIO led_GREEN_GPIO(44);   	//led Verde 	P8_12
GPIO led_BLUE_GPIO(26);   	//led Azul 		P8_14
GPIO led_START_PAUSE_GPIO(46);   	//led Paua 		P8_16

//Entradas Buttons
GPIO button_RED_GPIO(66);    	//pulsador Rojo 	P8_7
GPIO button_YELLOW_GPIO(69);  	//pulsador Amarillo P8_9
GPIO button_GREEN_GPIO(45);    	//pulsador Verde 	P8_11
GPIO button_BLUE_GPIO(47); 		//pulsador Azul 	P8_15
GPIO button_START_PAUSE_GPIO(61);    	//pulsador Pause	P8_26

int read_Pot_Speed() {
	int number;
	FILE *fptr;
	fptr = fopen(POT_SPEED,"r");
	fscanf(fptr, "%i", &number);
	fclose(fptr);
	return number;
}
int read_Pot_Time() {
	int number;
	FILE *fptr;
	fptr = fopen(POT_TIME,"r");
	fscanf(fptr, "%i", &number);
	fclose(fptr);
	return number;
}


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

	char lcd_SHOW;


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
	}
	///////////////////////////////////////////////////////////////////

	//READ HARDWARE FUNCTIONS
	int read_HW_button_RED() {
		int aux = button_RED_GPIO.getValue();
		return aux;
	}

	int read_HW_button_GREEN() {
		int aux = button_GREEN_GPIO.getValue();
		return aux;
	}

	int read_HW_button_BLUE() {
		int aux = button_BLUE_GPIO.getValue();
		return aux;
	}

	int read_HW_button_YELLOW() {
		int aux = button_YELLOW_GPIO.getValue();
		return aux;
	}

	int read_HW_button_INIT_PAUSE() {
		int aux = button_START_PAUSE_GPIO.getValue();
		return aux;
	}

	int read_HW_pot_TIME() {
		return 0;
	}

	int read_HW_pot_SPEED() {
		return 0;
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

	void write_HW_START_PAUSE(GPIO::VALUE value) {
		led_START_PAUSE_GPIO.setValue(value);
	}

	///////////////////////////////////////////////////////////////////

	//SETER PROTECTED VARIABLES
	void set_button_RED(double value) {
		pthread_mutex_lock(&mutex_variables);
		button_RED = value;
		pthread_mutex_unlock(&mutex_variables);
	}

	void set_button_GREEN(bool value) {
		pthread_mutex_lock(&mutex_variables);
		button_GREEN = value;
		pthread_mutex_unlock(&mutex_variables);
	}

	void set_button_BLUE(double value) {
		pthread_mutex_lock(&mutex_variables);
		button_BLUE = value;
		pthread_mutex_unlock(&mutex_variables);
	}

	void set_button_YELLOW(double value) {
		pthread_mutex_lock(&mutex_variables);
		button_YELLOW = value;
		pthread_mutex_unlock(&mutex_variables);
	}

	void set_button_START_PAUSE(double value) {
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

	void set_lcd_SHOW(char value) {
		pthread_mutex_lock(&mutex_variables);
		lcd_SHOW = value;
		pthread_mutex_unlock(&mutex_variables);
	}

	//SETTER COMMON VARIABLES
	void set_game_speed(int value) {
		game_speed = value;
	}

	void set_game_time(int value) {
		game_time = value;
	}

	///////////////////////////////////////////////////////////////////

	//GETTER PROTECTED VARIABLES
	double get_button_RED() {
		pthread_mutex_lock(&mutex_variables);
		int aux = button_RED;
		pthread_mutex_unlock(&mutex_variables);
		return aux;
	}

	double get_button_GREEN() {
		pthread_mutex_lock(&mutex_variables);
		int aux = button_GREEN;
		pthread_mutex_unlock(&mutex_variables);
		return aux;
	}

	double get_button_BLUE() {
		pthread_mutex_lock(&mutex_variables);
		int aux = button_BLUE;
		pthread_mutex_unlock(&mutex_variables);
		return aux;
	}

	double get_button_YELLOW() {
		pthread_mutex_lock(&mutex_variables);
		int aux = button_YELLOW;
		pthread_mutex_unlock(&mutex_variables);
		return aux;
	}

	double get_button_START_PAUSE() {
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
	int get_game_speed(void) {
		return game_speed;
	}

	int get_game_time(void) {
		return game_time;
	}

	int get_timer_value(void) {
		return timer_value;
	}

	char get_lcd_SHOW(void) {
		return lcd_SHOW;
	}

	///////////////////////////////////////////////////////////////////

	//AUXILIAR FUNCTIONS
	int get_button_pressed() {
		int aux = 0;
		if(get_button_RED()) {
			aux = 1;
			//printf( "getBottonPressed: RED PRESSED\n");
		}
		if(get_button_GREEN()) {
			aux = 2;
			//printf( "getBottonPressed: GREEN PRESSED\n");
		}
		if(get_button_BLUE()) {
			aux = 3;
			//printf( "getBottonPressed: BLUE PRESSED\n");
		}
		if(get_button_YELLOW()) {
			aux = 4;
			//printf( "getBottonPressed: YELLOW PRESSED\n");
		}
		if(get_button_START_PAUSE()) {
			aux = 5;
			//printf( "getBottonPressed: PAUSE PRESSED\n");
		}
		return aux;
	}
};

