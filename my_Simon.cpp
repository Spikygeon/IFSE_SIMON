/*
 * Author: Spikygeon (2020)
 * Creative Commons.
 */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

#define MAXSTATES_DEF 6
#define MAXLISTENERS_DEF 8

#define INIT_STATE				0
#define SHOW_PATRON_STATE		1
#define INSERT_PATRON_STATE		2
#define PAUSE_STATE				3
#define YOU_WIN_STATE			4
#define YOU_LOSE_STATE			5


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
	pthread_mutex_t mutex;
	pthread_cond_t condition;
	void* (*func_table[MAXSTATES][MAXSTATES][MAXLISTENERS])(int stFrom, int stTo);

public:
	StateMonitor() {
		internalState = 0;
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
//////////////////////////////////////////////////////////////////////////

void *pthread_read_input(void *param) {
	threadConf *cfgPassed = (threadConf*)param;
	long longPassed = (long) cfgPassed->getArg();
	int iter = 0;
	for (;;) {
		int state = stateManager.waitState(cfgPassed);
		iter++;
		printf("pthread_read_input:\t operando con %ld (iter. %d) en el estado %d.\n",longPassed, iter, state);
		usleep(500000);
	}
}

void *pthread_config_game(void *param) {
	threadConf *cfgPassed = (threadConf*)param;
	long longPassed = (long) cfgPassed->getArg();
	int iter = 0;
	for (;;) {
		int state = stateManager.waitState(cfgPassed);
		iter++;
		printf("pthread_config_game:\t operando con %ld (iter. %d) en el estado %d.\n",longPassed, iter, state);
		usleep(500000);
	}
}

void *pthread_show_patron(void *param) {
	threadConf *cfgPassed = (threadConf*)param;
	long longPassed = (long) cfgPassed->getArg();
	int iter = 0;
	for (;;) {
		int state = stateManager.waitState(cfgPassed);
		iter++;
		printf("pthread_show_patron:\t operando con %ld (iter. %d) en el estado %d.\n",longPassed, iter, state);
		usleep(500000);
	}
}

void *pthread_insert_patron(void *param) {
	threadConf *cfgPassed = (threadConf*)param;
	long longPassed = (long) cfgPassed->getArg();
	int iter = 0;
	for (;;) {
		int state = stateManager.waitState(cfgPassed);
		iter++;
		printf("pthread_insert_patron:\t operando con %ld (iter. %d) en el estado %d.\n",longPassed, iter, state);
		usleep(500000);
	}
}

void *pthread_timer(void *param) {
	threadConf *cfgPassed = (threadConf*)param;
	long longPassed = (long) cfgPassed->getArg();
	int iter = 0;
	for (;;) {
		int state = stateManager.waitState(cfgPassed);
		iter++;
		printf("pthread_timer:\t\t operando con %ld (iter. %d) en el estado %d.\n",longPassed, iter, state);
		usleep(500000);
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
	threadConf *cfgPassed = (threadConf*)param;
	long longPassed = (long) cfgPassed->getArg();
	int iter = 0;
	for (;;) {
		int state = stateManager.waitState(cfgPassed);
		iter++;
		printf("pthread_pause:\t operando con %ld (iter. %d) en el estado %d.\n",longPassed, iter, state);
		usleep(500000);
	}
}

void *pthread_state_monitor(void *param) {
	threadConf *cfgPassed = (threadConf*)param;
	long longPassed = (long) cfgPassed->getArg();
	int iter = 0;

	/////// Ejemplo de control de gestor de transición de estados
	int myState = 0;
	for(;;) {
		int state = stateManager.waitState(cfgPassed);
		iter++;
		printf("pthread_state_monitor:\t operando con %ld (iter. %d) en el estado %d.\n",longPassed, iter, state);

		usleep(5000000);
		myState = (stateManager.getState() + 1) % 4;
		stateManager.changeState(myState);
	}
}


//////////////////////////////////////////////////////////////////////////
void *changeStateHandler(int stFrom, int stTo) {
	printf("********************** Cambio de estado: desde %d a %d.\n",stFrom,stTo);
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
	stateManager.addStateChangeListener(INSERT_PATRON_STATE,	YOU_WIN_STATE,			changeStateHandler);
	stateManager.addStateChangeListener(INSERT_PATRON_STATE,	YOU_LOSE_STATE,			changeStateHandler);
	stateManager.addStateChangeListener(YOU_WIN_STATE,			INIT_STATE,				changeStateHandler);
	stateManager.addStateChangeListener(YOU_LOSE_STATE,			INIT_STATE,				changeStateHandler);

	//El estado pausa de poder accederse y retornar desde y hacia cualquier estado
	int i;
	for (i=0; i<MAXSTATES_DEF; i++) {
		stateManager.addStateChangeListener(i,				PAUSE_STATE,	changeStateHandler);
		stateManager.addStateChangeListener(PAUSE_STATE,	i,				changeStateHandler);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	//my_pthread_state_monitor. El hilo de la máquina de estados se ejecuta en todos los estados
	//	threadConf state_monitor_cfg;
	//	state_monitor_cfg.addState(0);
	//	state_monitor_cfg.addState(1);
	//	state_monitor_cfg.addState(2);
	//	state_monitor_cfg.addState(3);
	//	state_monitor_cfg.setArg((void*)100);
	//
	//	//my_pthread_read_input. El hilo de lectura de entradas se ejecuta en todos los estados
	//	threadConf read_input_cfg;
	//	read_input_cfg.addState(1);
	//	read_input_cfg.addState(2);
	//	read_input_cfg.addState(3);
	//	read_input_cfg.setArg((void*)100);
	//
	//	//my_pthread_lcd. El hilo de la máquina de estados se ejecuta en todos los estados
	//	threadConf lcd_cfg;
	//	lcd_cfg.addState(2);
	//	lcd_cfg.addState(3);
	//	lcd_cfg.setArg((void*)100);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	//my_pthread_pause. Hilo de pausa
	threadConf pause_cfg;
	pause_cfg.addState(PAUSE_STATE);
	pause_cfg.setArg((void*)100);

	//Creamos los hilos
	pthread_create(&my_pthread_state_monitor,  	&attr,  pthread_state_monitor,  (void*)&state_monitor_cfg);
	pthread_create(&my_pthread_read_input,  	&attr,  pthread_read_input,  	(void*)&read_input_cfg);
	pthread_create(&my_pthread_lcd,  			&attr,  pthread_lcd,  			(void*)&lcd_cfg);
	pthread_create(&my_pthread_timer,  			&attr,  pthread_timer,  		(void*)&timer_cfg);
	pthread_create(&my_pthread_config_game,  	&attr,  pthread_config_game,  	(void*)&config_game_cfg);
	pthread_create(&my_pthread_show_patron,  	&attr,  pthread_show_patron,  	(void*)&show_patron_cfg);
	pthread_create(&my_pthread_insert_patron,  	&attr,  pthread_insert_patron,  (void*)&insert_patron_cfg);
	pthread_create(&my_pthread_pause,  			&attr,  pthread_pause,  		(void*)&pause_cfg);


	//Ponemos en marcha los hilos
	pthread_join(my_pthread_state_monitor,	NULL);
	pthread_join(my_pthread_read_input,		NULL);
	pthread_join(my_pthread_lcd,			NULL);
	pthread_join(my_pthread_timer,			NULL);
	pthread_join(my_pthread_config_game,	NULL);
	pthread_join(my_pthread_show_patron,	NULL);
	pthread_join(my_pthread_insert_patron,	NULL);
	pthread_join(my_pthread_pause,			NULL);

}


