/*
 * Author: Spikygeon (2021)
 * Creative Commons.
 */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdlib.h>
#include "variables.h"

#define MAXSTATES_DEF 6
#define MAXLISTENERS_DEF 8

#define INIT_STATE				0
#define SHOW_PATRON_STATE		1
#define INSERT_PATRON_STATE		2
#define PAUSE_STATE				3
#define YOU_WIN_STATE			4
#define YOU_LOSE_STATE			5

#define MAX_GAME_LEVELS			3		//Numero de niveles para ganar (sin contar el cero)

//////////////////////////////////////////////////////////////////////////

//DECLARACIÓN DE CLASES

class threadConf {
private:
	int states[MAXSTATES_DEF];
	int nstates;
	void *arg;

public:
	threadConf() {
		nstates = 0;
		arg = NULL;
	}

	void addState(int s) {
		states[nstates] = s;
		nstates++;
	}

	bool containsState(int s) {
		bool retval = false;
		int i;
		for (i=0; i<getNStates(); i++) {
			if (states[i] == s) {
				retval = true;
			}
		}
		return retval;
	}

	int getNStates() {
		return nstates;
	}

	void setArg(void *pointer) {
		arg = pointer;
	}

	void *getArg() {
		return arg;
	}
};

class StateMonitor {
private:
	static const int MAXSTATES = MAXSTATES_DEF;
	static const int MAXLISTENERS = MAXLISTENERS_DEF;
	int internalState;
	int previousState;
	pthread_mutex_t mutex;
	pthread_cond_t condition;
	void* (*func_table[MAXSTATES][MAXSTATES][MAXLISTENERS])(int stFrom, int stTo);

public:
	StateMonitor() {
		internalState = INIT_STATE;
		previousState = INIT_STATE;
		int i,j,k;
		mutex = PTHREAD_MUTEX_INITIALIZER;
		condition = PTHREAD_COND_INITIALIZER;

		for(i=0;i<MAXSTATES;i++)
			for(j=0;j<MAXSTATES;j++)
				for(k=0;k<MAXLISTENERS;k++) {
					func_table[i][j][k] = NULL;
				}
	}

	int waitState(threadConf *cfg) {
		int auxState;
		pthread_mutex_lock(&mutex);
		while (!cfg->containsState(internalState) ) {
			pthread_cond_wait(&condition, &mutex);
		}
		auxState = internalState;
		pthread_mutex_unlock(&mutex);
		return(auxState);
	}

	int changeState(int st) {
		pthread_mutex_lock(&mutex);
		int i;
		for(i=0;i<MAXLISTENERS;i++) {
			if (func_table[internalState][st][i] != NULL) {
				(*func_table[internalState][st][i])(internalState,st);
			}
		}
		internalState = st;
		pthread_cond_broadcast(&condition);
		pthread_mutex_unlock(&mutex);
		return(0);
	}

	int getState() {
		int aux;
		pthread_mutex_lock(&mutex);
		aux = internalState;
		pthread_mutex_unlock(&mutex);
		return(aux);
	}

	int setPreviousState(int st) {
		pthread_mutex_lock(&mutex);
		previousState = st;
		pthread_mutex_unlock(&mutex);
		return(0);
	}

	int getPreviousState() {
		int aux;
		pthread_mutex_lock(&mutex);
		aux = previousState;
		pthread_mutex_unlock(&mutex);
		return(aux);

	}

	int addStateChangeListener(int fromState, int toState, void* (*handle)(int,int)) {
		pthread_mutex_lock(&mutex);
		int i = 0;
		int retval = 1;
		for(i=0;i<MAXLISTENERS;i++) {
			if (func_table[fromState][toState][i] == NULL) {
				retval = 0;
				func_table[fromState][toState][i] = handle;
				break;
			}
		}
		pthread_mutex_unlock(&mutex);
		return(retval);
	}
};

//////////////////////////////////////////////////////////////////////////

//DECLARACIÓN DE VARIABLES

int game_level;							// Indica el nivel actual
int shown_sequence[MAX_GAME_LEVELS];	// Vector que almacena la secuencia mostrada por la máquina
int myTimeOffset;
int current_match;
int current_show;
int randomNum;

unsigned int sval_min = 350000;
unsigned int sval_max = 2600000;

//////////////////////////////////////////////////////////////////////////

//DECLARACIÓN DE ACCESO A CLASES

StateMonitor stateManager;				//Variable para acceder a la clase StateMonitor
Variables variableManagement;			//Variable para acceder a la clase StateMonitor

//////////////////////////////////////////////////////////////////////////

//FUNCIONES PROPIETARIAS
void pulsacion(int value, int time, bool light){
	//100000 medio
	int i = 0;
	while(i<value){

		variableManagement.write_HW_ZUMBADOR(20000);
		if(light == true){
			variableManagement.write_HW_led_ALL_ON();
		}
		usleep(time);
		variableManagement.write_HW_ZUMBADOR(0);
		variableManagement.write_HW_led_ALL_OFF();
		usleep(time);
		i++;
	}
}

void manage_potenciometers_and_motors(void) {

	variableManagement.set_pot_TIME(variableManagement.read_HW_pot_TIME());
	int value = variableManagement.get_pot_TIME();
	double aval_pu = (double)value/4095.0;
	unsigned duty_ref_time = sval_min + aval_pu * (sval_max - sval_min);
	if(duty_ref_time >2100000){

		variableManagement.set_game_time(5);
		duty_ref_time = 2600000;
		variableManagement.write_HW_MOTOR_TIME(duty_ref_time);
	}
	if(duty_ref_time > 1500000 && duty_ref_time<2100000){

		variableManagement.set_game_time(4);
		duty_ref_time = 2100000;
		variableManagement.write_HW_MOTOR_TIME(duty_ref_time);
	}

	if(duty_ref_time > 1120000 && duty_ref_time<1700000){

		variableManagement.set_game_time(3);
		duty_ref_time = 1700000;
		variableManagement.write_HW_MOTOR_TIME(duty_ref_time);
	}

	if(duty_ref_time > 680000 && duty_ref_time<1120000){
		variableManagement.set_game_time(2);

		duty_ref_time = 1020000;
		variableManagement.write_HW_MOTOR_TIME(duty_ref_time);
	}
	if(duty_ref_time < 680000){

		variableManagement.set_game_time(1);
		duty_ref_time = 680000;
		variableManagement.write_HW_MOTOR_TIME(duty_ref_time);
	}

	variableManagement.set_pot_SPEED(variableManagement.read_HW_pot_SPEED());
	int valueSPEE = variableManagement.get_pot_SPEED();
	double aval_pusPEE = (double)valueSPEE/4095.0;
	unsigned duty_ref_sPEE = sval_min + aval_pusPEE * (sval_max - sval_min);
	if(duty_ref_sPEE >2100000){
		variableManagement.set_game_speed(5);
		duty_ref_sPEE = 2600000;
		variableManagement.write_HW_MOTOR_SPEED(duty_ref_sPEE);
	}
	if(duty_ref_sPEE > 1500000 && duty_ref_sPEE<2100000){
		variableManagement.set_game_speed(4);
		duty_ref_sPEE = 2100000;
		variableManagement.write_HW_MOTOR_SPEED(duty_ref_sPEE);
	}
	if(duty_ref_sPEE > 1120000 && duty_ref_sPEE<1700000){
		variableManagement.set_game_speed(3);
		duty_ref_sPEE = 1700000;
		variableManagement.write_HW_MOTOR_SPEED(duty_ref_sPEE);
	}
	if(duty_ref_sPEE > 680000 && duty_ref_sPEE<1120000){
		variableManagement.set_game_speed(2);
		duty_ref_sPEE = 1020000;
		variableManagement.write_HW_MOTOR_SPEED(duty_ref_sPEE);
	}
	if(duty_ref_sPEE < 680000){
		variableManagement.set_game_speed(1);
		duty_ref_sPEE = 680000;
		variableManagement.write_HW_MOTOR_SPEED(duty_ref_sPEE);
	}
}


//////////////////////////////////////////////////////////////////////////

//HILOS

void *pthread_read_input(void *param) {

	printf("pthread_read_input arrancado\n");

	threadConf *cfgPassed = (threadConf*)param;

	for (;;) {
		int state = stateManager.waitState(cfgPassed);

		//Leemos todas las entradas
		variableManagement.read_all_HW_buttons();
		usleep(1000);
	}
}


void *pthread_timer(void *param) {

	printf("pthread_timer: arrancado\n");
	threadConf *cfgPassed = (threadConf*)param;
	int counterTimer = 0;
	int mySleepTime = 1000000;	//Incrementa el tiempo cada 1 segundo

	for (;;) {
		int state = stateManager.waitState(cfgPassed);
		counterTimer++;
		variableManagement.set_timer_value(counterTimer);
		//printf("SHOW_TIEMPO: %d\n", variableManagement.get_timer_value());	//Devuelve en segundos
		usleep(mySleepTime);
	}
}


void *pthread_pause(void *param) {

	printf("pthread_pause: arrancado\n");
	threadConf *cfgPassed = (threadConf*)param;

	for (;;) {
		int state = stateManager.waitState(cfgPassed);

		if (variableManagement.get_button_START_PAUSE() == true) {

			printf("pthread_pause: \t PULSADA PAUSA\n");

			if (stateManager.getState() ==  PAUSE_STATE) {
				//Apagamos el led
				variableManagement.write_HW_led_START_PAUSE(GPIO::LOW);


				//Volvemos al último estado activo
				printf("pthread_pause: \t Volvemos al último estado activo: %d\n", stateManager.getPreviousState());
				stateManager.changeState(stateManager.getPreviousState());

			} else if (stateManager.getState() ==  YOU_WIN_STATE) {
				//Reiniciamos juego
				printf("pthread_pause: \t Reiniciamos juego desde Win\n");
				stateManager.changeState(INIT_STATE);

			} else if (stateManager.getState() ==  YOU_LOSE_STATE) {
				//Reiniciamos juego
				printf("pthread_pause: \t Reiniciamos juego desde Lose\n");
				stateManager.changeState(INIT_STATE);

			} else {
				//Almacenamos estado previo (SHOW_PATRON ó INSERT_PATRON)
				stateManager.setPreviousState(stateManager.getState());
				printf("almacenamos estado previo: %d\n",stateManager.getPreviousState());

				//Encendemos el led de pausa
				variableManagement.write_HW_led_colour_ON(START_PAUSE_BUTTON);

				//Vamos al estado de pausa
				printf("pthread_pause: \t Vamos al estado de pausa\n");
				stateManager.changeState(PAUSE_STATE);
			}

			//Hacemos un sleep de medio segundo para evitar duplicidades en la pulsación
			usleep(500000);
		}

		usleep(1000);
	}
}



void *pthread_config_game(void *param) {

	printf("pthread_config_game: arrancado\n");
	threadConf *cfgPassed = (threadConf*)param;

	//Inicializamos los leds a apagado
	led_RED_GPIO.setValue(GPIO::LOW);
	led_GREEN_GPIO.setValue(GPIO::LOW);
	led_BLUE_GPIO.setValue(GPIO::LOW);
	led_YELLOW_GPIO.setValue(GPIO::LOW);
	led_START_PAUSE_GPIO.setValue(GPIO::LOW);

	for (;;) {
		int state = stateManager.waitState(cfgPassed);

		//Controlamos los valores del potenciómetro y ajsutamos la posición de los motores
		manage_potenciometers_and_motors();

		if (variableManagement.get_button_START_PAUSE() == true) {
			printf("\n\n-----------------------------\n\tSTART GAME !!!\n-----------------------------\n\n");
			pulsacion(2,100000,false);
			//Inicializamos todas las variables de la nueva partida
			game_level = 0;
			//randomNum = 0;

			printf("Segundos time %d\n", variableManagement.get_game_time());
			printf("Segundos speed %d\n", variableManagement.get_game_speed());

			variableManagement.set_game_time(5);
			variableManagement.set_game_speed(1);


			//printf("Segundos time %d\n", variableManagement.get_game_time());
			//printf("Segundos speed %d\n", variableManagement.get_game_speed());
			usleep(1000000);
			stateManager.changeState(SHOW_PATRON_STATE);
		}
		usleep(100000);
	}
}


void *pthread_show_patron(void *param) {

	printf("pthread_show_patron: arrancado\n");
	threadConf *cfgPassed = (threadConf*)param;

	for(;;) {

		int state = stateManager.waitState(cfgPassed);

		//Si hemos superado todos los niveles
		if (game_level > (MAX_GAME_LEVELS-1)) {
			printf("TODOS LOS NIVELES SUPERADOS !!!\n");
			stateManager.changeState(YOU_WIN_STATE);
		}
		else {
			// Se genera un valor aleatorio del patrón
			randomNum = rand() % 4 + 1;		// Colores del 1 al 4
			//printf("SHOW_PATRON: número aleatorio: %d\n", randomNum);

			// Almacenamos el nuevo valor del patrón en el vector de muestra
			shown_sequence[game_level] = randomNum;

			// Se muestra la secuencia del patron
			for (int i=0; i<=game_level ;i++) {

				//Comprobamos si se ha pulsado pausa
				int state = stateManager.waitState(cfgPassed);

				// Se enciende el LED del patron
				switch (shown_sequence[i]) {

				case 1:	//RED
					//printf("pthread_show_patron: set RED\n");
					variableManagement.write_HW_led_RED(GPIO::HIGH);
					usleep(variableManagement.get_game_speed());
					variableManagement.write_HW_led_RED(GPIO::LOW);
					usleep(variableManagement.get_game_speed()/2);
					break;

				case 2:	//GREEN
					//printf("pthread_show_patron: set GREEN\n");
					variableManagement.write_HW_led_GREEN(GPIO::HIGH);
					usleep(variableManagement.get_game_speed());
					variableManagement.write_HW_led_GREEN(GPIO::LOW);
					usleep(variableManagement.get_game_speed()/2);
					break;

				case 3:	//BLUE
					//printf("pthread_show_patron: set BLUE\n");
					variableManagement.write_HW_led_BLUE(GPIO::HIGH);
					usleep(variableManagement.get_game_speed());
					variableManagement.write_HW_led_BLUE(GPIO::LOW);
					usleep(variableManagement.get_game_speed()/2);
					break;

				case 4:	//YELLOW
					//printf("pthread_show_patron: set YELLOW\n");
					variableManagement.write_HW_led_YELLOW(GPIO::HIGH);
					usleep(variableManagement.get_game_speed());
					variableManagement.write_HW_led_YELLOW(GPIO::LOW);
					usleep(variableManagement.get_game_speed()/2);
					break;

				default:	//Nunca debería entrar aquí
					//printf("pthread_show_patron: NO DEBERIAMOS ENTRAR AQUÍ, case default. \n");
					break;
				}
			}

			//Mostramos secuencia patrón por pantalla
			for (int j=0; j<=game_level; j++) {
				printf("shown_sequence[ %d ]:\t%d\n", j, shown_sequence[j]);
			}

			// Terminada la secuencia damos unos segundos de margen y cambiamos de estado
			usleep(1000000);

			//Iniciamos tiempo de offset antes de pasar al estado de insertar patrón
			myTimeOffset = variableManagement.get_timer_value();
			printf("myTimeOffset (first): \t %d\n", myTimeOffset);
			current_match = 0;
			printf("current_match (first): \t %d\n", current_match);

			//Comprobamos si hemos ido a pausa mientras estamos mostrando la secuencia. En cuyo caso no cambiaríamos de paso
			if (stateManager.getState() != PAUSE_STATE) {
				// Cambiamos de paso
				stateManager.changeState(INSERT_PATRON_STATE);
			}
		}
	}
}


void *pthread_insert_patron(void *param) {

	printf("pthread_insert_patron: arrancado\n");
	threadConf *cfgPassed = (threadConf*)param;
	int myButtonPressed;

	for (;;) {

		int state = stateManager.waitState(cfgPassed);

		//////////////////////////////////////////////////////////////////////////////////////////

		//Si no hemos llegado al final del patrón
		if (current_match <= game_level){

			//Si estamos dentro del tiempo establecido (5 segundos)
			if(variableManagement.get_timer_value() < (myTimeOffset + variableManagement.get_game_time())) {

				printf("tiempo:\t %d\n", variableManagement.get_timer_value());
				printf("time out:\t %d\n", (myTimeOffset + variableManagement.get_game_time()));

				//Si pulsamos cualquier botón
				myButtonPressed = variableManagement.get_button_pressed();
				variableManagement.write_HW_led_colour_ON(myButtonPressed);
				usleep(500000);	//Pequeño sleep para evitar duplicidades
				variableManagement.write_HW_led_ALL_OFF();
				if ( (myButtonPressed != 0) && (myButtonPressed != START_PAUSE_BUTTON) )
				{
					//Si la tecla coincide con el patrón
					printf(">>> posición del vector real: shown_sequence[ %d ] = %d\n", current_match, shown_sequence[current_match]);
					printf(">>> botos pulsado:            nmyButtonPressed:\t%d\n", myButtonPressed);
					if(myButtonPressed == shown_sequence[current_match]) {

						current_match++;
						printf("MATCH posicion, current_match: %d\n", current_match);
					}
					else {

						printf("BOTON INCORRECTO, current_match: %d\n", current_match);;
						stateManager.changeState(YOU_LOSE_STATE);
					}

					//Iniciamos tiempo de offset antes de pasar al siguiente botón
					myTimeOffset = variableManagement.get_timer_value();
					printf("tiempo:\t\t %d\n", variableManagement.get_timer_value());
					printf("time out:\t %d\n", (myTimeOffset + variableManagement.get_game_time()));
				}
			}
			else {

				printf("pthread_insert_patron: \t TIMEOUT SUPERADO!!!\n");
				stateManager.changeState(YOU_LOSE_STATE);
			}
		}
		else {

			game_level++;
			printf("NEXT LEVEL !!! %d\n", game_level+1);
			usleep(2000000);
			stateManager.changeState(SHOW_PATRON_STATE);
		}

		usleep(1000);

		//////////////////////////////////////////////////////////////////////////////////////////
	}
}


void *pthread_end_game(void *param) {
	printf("pthread_end_game: arrancado\n");
	threadConf *cfgPassed = (threadConf*)param;

	for (;;) {
		int state = stateManager.waitState(cfgPassed);

		if (variableManagement.get_button_START_PAUSE() == true) {
			printf("\n\n-----------------------------\n\tGG !!!\n-----------------------------\n\n");
			usleep(1000000);
			stateManager.changeState(INIT_STATE);
		}

		usleep(100000);
	}
}


//////////////////////////////////////////////////////////////////////////

//MANEJADORES DE CAMBIO DE ESTADO
void *changeStateHandler(int stFrom, int stTo) {
	printf("********************** Cambio de estado: desde %d a %d.\n",stFrom,stTo);
	if(stFrom == SHOW_PATRON_STATE && stTo == INSERT_PATRON_STATE){
		pulsacion(1,100000,false);
	}
	if(stTo == PAUSE_STATE || stFrom == PAUSE_STATE ){
		pulsacion(3,100000,false);
	}

	return(NULL);
}

void *changeStateHandlerEndGame(int stFrom, int stTo) {
	printf("********************** Cambio de estado: desde %d a %d.\n",stFrom,stTo);
	if (stTo == YOU_WIN_STATE) {
		printf("YOU WIN !!!\n");
		pulsacion(2,400000,true);
		pulsacion(3,200000,true);
		usleep(300000);
		pulsacion(4,200000,true);
		usleep(400000);
		pulsacion(1,200000,true);
		pulsacion(1,400000,true);

	} else if (stTo == YOU_LOSE_STATE){
		printf("YOU LOSE !!!\n");
		pulsacion(2,100000,true);
		pulsacion(1,700000,true);


	}
	return(NULL);
}
//////////////////////////////////////////////////////////////////////////

int main (void) {

	pthread_t my_pthread_read_input;
	pthread_t my_pthread_timer;
	pthread_t my_pthread_config_game;
	pthread_t my_pthread_show_patron;
	pthread_t my_pthread_insert_patron;
	pthread_t my_pthread_pause;
	pthread_t my_pthread_end_game;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	// Configuramos el manejador de cambio de estado para las diferentes transiciones
	stateManager.addStateChangeListener(INIT_STATE,				SHOW_PATRON_STATE,		changeStateHandler);
	stateManager.addStateChangeListener(SHOW_PATRON_STATE,		INSERT_PATRON_STATE,	changeStateHandler);
	stateManager.addStateChangeListener(INSERT_PATRON_STATE,	SHOW_PATRON_STATE,		changeStateHandler);
	stateManager.addStateChangeListener(SHOW_PATRON_STATE,		YOU_WIN_STATE,			changeStateHandlerEndGame);
	stateManager.addStateChangeListener(INSERT_PATRON_STATE,	YOU_LOSE_STATE,			changeStateHandlerEndGame);
	stateManager.addStateChangeListener(YOU_WIN_STATE,			INIT_STATE,				changeStateHandler);
	stateManager.addStateChangeListener(YOU_LOSE_STATE,			INIT_STATE,				changeStateHandler);

	//El estado pausa de poder accederse y retornar desde y hacia cualquier estado
	int i;
	for (i=0; i<MAXSTATES_DEF; i++) {
		stateManager.addStateChangeListener(i,				PAUSE_STATE,	changeStateHandler);
		stateManager.addStateChangeListener(PAUSE_STATE,	i,				changeStateHandler);
	}

	//my_pthread_read_input. El hilo de lectura de entradas se ejecuta en todos los estados
	threadConf read_input_cfg;
	read_input_cfg.addState(INIT_STATE);
	read_input_cfg.addState(SHOW_PATRON_STATE);
	read_input_cfg.addState(INSERT_PATRON_STATE);
	read_input_cfg.addState(YOU_WIN_STATE);
	read_input_cfg.addState(YOU_LOSE_STATE);
	read_input_cfg.addState(PAUSE_STATE);

	//my_pthread_pause. Hilo de pausa
	threadConf pause_cfg;
	pause_cfg.addState(SHOW_PATRON_STATE);
	pause_cfg.addState(INSERT_PATRON_STATE);
	pause_cfg.addState(PAUSE_STATE);

	//my_pthread_timer. El hilo del timer se ejecuta en los estados en los que tenemos que trabajar con lecturas de tiempo
	threadConf timer_cfg;
	timer_cfg.addState(SHOW_PATRON_STATE);
	timer_cfg.addState(INSERT_PATRON_STATE);

	//my_pthread_config_game. Hilo de configuración del juego
	threadConf config_game_cfg;
	config_game_cfg.addState(INIT_STATE);

	//my_pthread_show_patron. Hilo donde se muestran el patrón de luces a recordar
	threadConf show_patron_cfg;
	show_patron_cfg.addState(SHOW_PATRON_STATE);

	//my_pthread_insert_patron. Hilo para introducir el patrón por el usuario
	threadConf insert_patron_cfg;
	insert_patron_cfg.addState(INSERT_PATRON_STATE);

	//my_pthread_insert_patron. Hilo de fin del juego
	threadConf end_game_cfg;
	end_game_cfg.addState(YOU_WIN_STATE);
	end_game_cfg.addState(YOU_LOSE_STATE);


	//Creamos los hilos
	pthread_create(&my_pthread_read_input,  	&attr,  pthread_read_input,  	(void*)&read_input_cfg);
	pthread_create(&my_pthread_timer,  			&attr,  pthread_timer,  		(void*)&timer_cfg);
	pthread_create(&my_pthread_config_game,  	&attr,  pthread_config_game,  	(void*)&config_game_cfg);
	pthread_create(&my_pthread_show_patron,  	&attr,  pthread_show_patron,  	(void*)&show_patron_cfg);
	pthread_create(&my_pthread_insert_patron,  	&attr,  pthread_insert_patron,  (void*)&insert_patron_cfg);
	pthread_create(&my_pthread_pause,  			&attr,  pthread_pause,  		(void*)&pause_cfg);
	pthread_create(&my_pthread_end_game,		&attr,  pthread_end_game,  		(void*)&end_game_cfg);

	// Permite la generación de números aleatorios distintos en cada ejecución
	srand (time(NULL));

	// Inicializamos la máquina de estados
	stateManager.changeState(INIT_STATE);

	for(;;) {
		//Estado de espera
		usleep(1000);
	}

	printf("main:\t\t OJO NO DEBERÍAMOS ESTAR AQUI !!!\n");
	pthread_join(my_pthread_read_input,		NULL);
	pthread_join(my_pthread_timer,			NULL);
	pthread_join(my_pthread_config_game,	NULL);
	pthread_join(my_pthread_show_patron,	NULL);
	pthread_join(my_pthread_insert_patron,	NULL);
	pthread_join(my_pthread_pause,			NULL);
	pthread_join(my_pthread_end_game,		NULL);
}

