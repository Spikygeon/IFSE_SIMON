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
#include "GPIO.h"

#define MAXSTATES_DEF 6
#define MAXLISTENERS_DEF 8

#define INIT_STATE				0
#define SHOW_PATRON_STATE		1
#define INSERT_PATRON_STATE		2
#define PAUSE_STATE				3
#define YOU_WIN_STATE			4
#define YOU_LOSE_STATE			5

#define GAME_LEVELS			5
#define PATRON_TIME_OUT		10

enum VALUE{ LOW=0, HIGH=1 };

int game_speed 	= 1 * 1000000;
int game_time	= 3 * 1000000;

//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
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
//Creamos la variable del StateMonitor
StateMonitor stateManager;
//Creamos la variable de la clase variables
Variables variable;
//////////////////////////////////////////////////////////////////////////

void *pthread_read_input(void *param) {

	printf("pthread_read_input arrancado\n");

	//Inicializamos los leds a apagado
	led_RED_GPIO.setValue(GPIO::LOW);
	led_GREEN_GPIO.setValue(GPIO::LOW);
	led_BLUE_GPIO.setValue(GPIO::LOW);
	led_YELLOW_GPIO.setValue(GPIO::LOW);
	led_START_PAUSE_GPIO.setValue(GPIO::LOW);

	for (;;) {

		int pulsado = variable.get_button_pressed();

		switch(pulsado) {

		case 1:
			variable.write_HW_led_RED(GPIO::HIGH);
			printf( "RED HIGH\n");
			usleep(100000);
			variable.write_HW_led_RED(GPIO::LOW);
			break;

		case 2:
			variable.write_HW_led_GREEN(GPIO::HIGH);
			printf( "GREEN HIGH\n");
			usleep(100000);
			variable.write_HW_led_GREEN(GPIO::LOW);
			break;

		case 3:
			variable.write_HW_led_BLUE(GPIO::HIGH);
			printf( "BLUE HIGH\n");
			usleep(100000);
			variable.write_HW_led_BLUE(GPIO::LOW);
			break;

		case 4:
			variable.write_HW_led_YELLOW(GPIO::HIGH);
			printf( "YELLOW HIGH\n");
			usleep(100000);
			variable.write_HW_led_YELLOW(GPIO::LOW);
			break;

		case 5:
			variable.write_HW_START_PAUSE(GPIO::HIGH);
			printf( "PAUSA HIGH\n");
			usleep(100000);
			variable.write_HW_START_PAUSE(GPIO::LOW);
			break;

		default:
			break;
		}

		usleep(100000);
	}
}

void *pthread_config_game(void *param) {

	printf("pthread_config_game: arrancado\n");

	for (;;) {
		//		int state = stateManager.waitState(cfgPassed);
		//		iter++;
		//		printf("pthread_config_game:\t operando con %ld (iter. %d) en el estado %d.\n",longPassed, iter, state);
		usleep(500000);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int shown_sequence[GAME_LEVELS];	// Vector que almacena la secuencia mostrada por la máquina
int game_level = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void *pthread_show_patron(void *param) {

	int randomNum;

	printf("pthread_show_patron: arrancado\n");

	for(;;) {

		// Se genera un valor aleatorio del patrón
		randomNum = rand() % 4 + 1;		// Colores del 1 al 4
		printf("SHOW_PATRON: número aleatorio: %d\n", randomNum);

		// Almacenamos el nuevo valor del patrón en el vector de muestra
		shown_sequence[game_level] = randomNum;

		// Se muestra la secuencia del patron
		for (int i=0; i<=game_level ;i++) {
			// Se enciende el LED del patron
			switch (shown_sequence[i]) {

			case 1:	//RED
				printf("pthread_show_patron: set RED\n");
				variable.write_HW_led_RED(GPIO::HIGH);
				usleep(game_speed);
				variable.write_HW_led_RED(GPIO::LOW);
				break;

			case 2:	//GREEN
				printf("pthread_show_patron: set GREEN\n");
				variable.write_HW_led_GREEN(GPIO::HIGH);
				usleep(game_speed);
				variable.write_HW_led_GREEN(GPIO::LOW);
				break;

			case 3:	//BLUE
				printf("pthread_show_patron: set BLUE\n");
				variable.write_HW_led_BLUE(GPIO::HIGH);
				usleep(game_speed);
				variable.write_HW_led_BLUE(GPIO::LOW);
				break;

			case 4:	//YELLOW
				printf("pthread_show_patron: set YELLOW\n");
				variable.write_HW_led_YELLOW(GPIO::HIGH);
				usleep(game_speed);
				variable.write_HW_led_YELLOW(GPIO::LOW);
				break;

			case 5:	//START-PAUSE
				printf("pthread_show_patron: set START PAUSE\n");
				variable.write_HW_START_PAUSE(GPIO::HIGH);
				usleep(game_speed);
				variable.write_HW_START_PAUSE(GPIO::LOW);
				break;

			default:	//Nunca debería entrar aquí
				printf("pthread_show_patron: NO DEBERIAMOS ENTRAR AQUÍ, case default. \n");
				break;
			}
		}

		int j;
		for (j=0; j<=game_level; j++) {
			printf("shown_sequence[ %d ]:\t%d\n", j, shown_sequence[j]);
		}

		// Terminada la secuencia damos unos segundos de margen y cambiamos de estado
		usleep(5000000);

		game_level++;

		// Cambiamos de paso
		//stateManager.changeState(INSERT_PATRON_STATE);

		printf("pthread_show_patron: NIVEL AUMENTADO\n");
	}
}

void *pthread_insert_patron(void *param) {

	int i;
	int buttonPressed;

	for (;;) {

		for (i=0; i<game_level; i++) {

			//myTimeOffset = getTimer();

			do {
				//leo pulsación
				buttonPressed = 1;
				printf("buttonPressed = %d\n",buttonPressed);

			}while(false);	//((getTimer() < myTimeOffset + game_time) || pulsacion_detectada)

			if(true) {		//((getTimer() >= myTimeOffset + game_time)

				//Timeout superado!
				stateManager.changeState(YOU_LOSE_STATE);

			} else {

				//comprobamos el botón pulsado
				if (buttonPressed != shown_sequence[ i ]) {

					//Botón erróneo!
					stateManager.changeState(YOU_LOSE_STATE);
				}
			}
		}

		if (false) {		//TODO: Comprobamos si estamos en el último nivel
			//Hemos ganado!!!
			stateManager.changeState(YOU_WIN_STATE);
		}
		else {
			//Pasamos al siguiente nivel
			game_level++;
			stateManager.changeState(SHOW_PATRON_STATE);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


void *pthread_timer(void *param) {

	int counterTimer = 0;
	int mySleepTime = 1000000;

	printf("pthread_timer: arrancado\n");

	for (;;) {

		printf("SHOW_TIEMPO: %d\n", counterTimer * mySleepTime /1000000);	//Devuelve en segundos
		counterTimer++;
		usleep(mySleepTime);
	}
}

void *pthread_lcd(void *param) {
	threadConf *cfgPassed = (threadConf*)param;
	long longPassed = (long) cfgPassed->getArg();
	int iter = 0;
	for (;;) {
		int state = stateManager.waitState(cfgPassed);
		iter++;
		printf("pthread_lcd:\t\t operando con %ld (iter. %d) en el estado %d.\n",longPassed, iter, state);
		usleep(500000);
	}
}

void *pthread_pause(void *param) {

	printf("pthread_pause: arrancado\n");

	for (;;) {

		if (variable.get_button_START_PAUSE() == true) {

			if (stateManager.getState() ==  PAUSE_STATE) {
				//Volvemos al último estado activo
				stateManager.changeState(stateManager.getPreviousState());

			} else if (stateManager.getState() ==  YOU_WIN_STATE) {
				//Reiniciamos juego
				stateManager.changeState(INIT_STATE);

			} else if (stateManager.getState() ==  YOU_LOSE_STATE) {
				//Reiniciamos juego
				stateManager.changeState(INIT_STATE);
			} else {
				//Vamos al estado de pausa
				stateManager.changeState(PAUSE_STATE);
			}
		}

		usleep(100000);
	}
}

void *pthread_state_monitor(void *param) {


	printf("pthread_state_monitor: arrancado\n");

	for (;;) {

		switch(stateManager.getState()) {

		case INIT_STATE:
			break;

		case SHOW_PATRON_STATE:
			break;

		case INSERT_PATRON_STATE:
			break;

		case PAUSE_STATE:
			break;

		case YOU_WIN_STATE:
			break;

		case YOU_LOSE_STATE:
			break;

		default:
			break;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
void *changeStateHandler(int stFrom, int stTo) {
	printf("********************** Cambio de estado: desde %d a %d.\n",stFrom,stTo);
	return(NULL);
}

void *changeStateHandlerEndGame(int stFrom, int stTo) {
	printf("********************** Cambio de estado: desde %d a %d.\n",stFrom,stTo);
	if (stTo == YOU_WIN_STATE) {
		printf("YOU WIN !!!\n");
	} else if (stTo == YOU_LOSE_STATE){
		printf("YOU LOSE !!!\n");
	}
	return(NULL);
}
//////////////////////////////////////////////////////////////////////////

int main (void) {

	pthread_t my_pthread_state_monitor;
	pthread_t my_pthread_read_input;
	pthread_t my_pthread_lcd;
	pthread_t my_pthread_timer;
	pthread_t my_pthread_config_game;
	pthread_t my_pthread_show_patron;
	pthread_t my_pthread_insert_patron;
	pthread_t my_pthread_pause;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	// Configuramos el manejador de cambio de estado para las diferentes transiciones
	stateManager.addStateChangeListener(INIT_STATE,				SHOW_PATRON_STATE,		changeStateHandler);
	stateManager.addStateChangeListener(SHOW_PATRON_STATE,		INSERT_PATRON_STATE,	changeStateHandler);
	stateManager.addStateChangeListener(INSERT_PATRON_STATE,	SHOW_PATRON_STATE,		changeStateHandler);
	stateManager.addStateChangeListener(INSERT_PATRON_STATE,	YOU_WIN_STATE,			changeStateHandlerEndGame);
	stateManager.addStateChangeListener(INSERT_PATRON_STATE,	YOU_LOSE_STATE,			changeStateHandlerEndGame);
	stateManager.addStateChangeListener(YOU_WIN_STATE,			INIT_STATE,				changeStateHandler);
	stateManager.addStateChangeListener(YOU_LOSE_STATE,			INIT_STATE,				changeStateHandler);

	//El estado pausa de poder accederse y retornar desde y hacia cualquier estado
	int i;
	for (i=0; i<MAXSTATES_DEF; i++) {
		stateManager.addStateChangeListener(i,				PAUSE_STATE,	changeStateHandler);
		stateManager.addStateChangeListener(PAUSE_STATE,	i,				changeStateHandler);
	}

	//my_pthread_state_monitor. El hilo de la máquina de estados se ejecuta en todos los estados
	threadConf state_monitor_cfg;
	state_monitor_cfg.addState(INIT_STATE);
	state_monitor_cfg.addState(SHOW_PATRON_STATE);
	state_monitor_cfg.addState(INSERT_PATRON_STATE);
	state_monitor_cfg.addState(YOU_WIN_STATE);
	state_monitor_cfg.addState(YOU_LOSE_STATE);
	state_monitor_cfg.addState(PAUSE_STATE);
	state_monitor_cfg.setArg((void*)100);

	//my_pthread_read_input. El hilo de lectura de entradas se ejecuta en todos los estados
	threadConf read_input_cfg;
	read_input_cfg.addState(INIT_STATE);
	read_input_cfg.addState(SHOW_PATRON_STATE);
	read_input_cfg.addState(INSERT_PATRON_STATE);
	read_input_cfg.addState(YOU_WIN_STATE);
	read_input_cfg.addState(YOU_LOSE_STATE);
	read_input_cfg.addState(PAUSE_STATE);
	read_input_cfg.setArg((void*)100);

	//my_pthread_lcd. El hilo de la máquina de estados se ejecuta en todos los estados
	threadConf lcd_cfg;
	lcd_cfg.addState(INIT_STATE);
	lcd_cfg.addState(SHOW_PATRON_STATE);
	lcd_cfg.addState(INSERT_PATRON_STATE);
	lcd_cfg.addState(YOU_WIN_STATE);
	lcd_cfg.addState(YOU_LOSE_STATE);
	lcd_cfg.addState(PAUSE_STATE);
	lcd_cfg.setArg((void*)100);

	//my_pthread_pause. Hilo de pausa
	threadConf pause_cfg;
	pause_cfg.addState(INIT_STATE);
	pause_cfg.addState(SHOW_PATRON_STATE);
	pause_cfg.addState(INSERT_PATRON_STATE);
	pause_cfg.addState(YOU_WIN_STATE);
	pause_cfg.addState(YOU_LOSE_STATE);
	pause_cfg.addState(PAUSE_STATE);
	pause_cfg.setArg((void*)100);

	//my_pthread_timer. El hilo del timer se ejecuta en los estados en los que tenemos que trabajar con lecturas de tiempo
	threadConf timer_cfg;
	timer_cfg.addState(SHOW_PATRON_STATE);
	timer_cfg.addState(INSERT_PATRON_STATE);
	timer_cfg.setArg((void*)100);

	//my_pthread_config_game. Hilo de configuración del juego
	threadConf config_game_cfg;
	config_game_cfg.addState(INIT_STATE);
	config_game_cfg.setArg((void*)100);

	//my_pthread_show_patron. Hilo donde se muestran el patrón de luces a recordar
	threadConf show_patron_cfg;
	show_patron_cfg.addState(SHOW_PATRON_STATE);
	show_patron_cfg.setArg((void*)100);

	//my_pthread_insert_patron. Hilo para introducir el patrón por el usuario
	threadConf insert_patron_cfg;
	insert_patron_cfg.addState(INSERT_PATRON_STATE);
	insert_patron_cfg.setArg((void*)100);


	//Creamos los hilos
	pthread_create(&my_pthread_state_monitor,  	&attr,  pthread_state_monitor,  (void*)&state_monitor_cfg);
	pthread_create(&my_pthread_read_input,  	&attr,  pthread_read_input,  	(void*)&read_input_cfg);
	//	pthread_create(&my_pthread_lcd,  			&attr,  pthread_lcd,  			(void*)&lcd_cfg);
	pthread_create(&my_pthread_timer,  			&attr,  pthread_timer,  		(void*)&timer_cfg);
	//	pthread_create(&my_pthread_config_game,  	&attr,  pthread_config_game,  	(void*)&config_game_cfg);
	pthread_create(&my_pthread_show_patron,  	&attr,  pthread_show_patron,  	(void*)&show_patron_cfg);
	//pthread_create(&my_pthread_insert_patron,  	&attr,  pthread_insert_patron,  (void*)&insert_patron_cfg);
	//	pthread_create(&my_pthread_pause,  			&attr,  pthread_pause,  		(void*)&pause_cfg);


	// Permite la generación de números aleatorios distintos en cada ejecución
	srand (time(NULL));

	//TODO: estado hardcodeado, borrar luego
	stateManager.changeState(SHOW_PATRON_STATE);

	for(;;) {
		//Estado de espera
		usleep(5000000);
		//		myState = (stateManager.getState() + 1) % 4;
		//		stateManager.changeState(myState);
	}

	pthread_join(my_pthread_state_monitor,	NULL);
	pthread_join(my_pthread_read_input,		NULL);
	pthread_join(my_pthread_lcd,			NULL);
	pthread_join(my_pthread_timer,			NULL);
	pthread_join(my_pthread_config_game,	NULL);
	pthread_join(my_pthread_show_patron,	NULL);
	pthread_join(my_pthread_insert_patron,	NULL);
	pthread_join(my_pthread_pause,			NULL);
}

