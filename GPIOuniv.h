/*
 * @file GPIO.h
 */

#ifndef GPIOUNIV_H_
#define GPIOUNIV_H_
#include<string>
#include<fstream>
#include <map>
using std::string;
using std::ofstream;

#define GPIO_PATH "/sys/class/gpio/"

namespace BBB {

typedef int (*CallbackType)(int);

/**
 * @class GPIO
 * @brief GPIO class for input and output functionality on a single GPIO pin
 */
class GPIO {
public:
	enum DIRECTION{ INPUT, OUTPUT };
	enum VALUE{ LOW=0, HIGH=1 };
	enum EDGE{ NONE, RISING, FALLING, BOTH };
	enum pinGPIO {	P8_3,	//GPIO_38
					P8_4,	//GPIO_39
					P8_5,	//GPIO_34
					P8_6,	//GPIO_35
					P8_7,	//GPIO_66
					P8_8,	//GPIO_67
					P8_9,	//GPIO_69
					P8_10,	//GPIO_68
					P8_11,	//GPIO_45
					P8_12,	//GPIO_44
					P8_13,	//GPIO_23
					P8_14,	//GPIO_26
					P8_15,	//GPIO_47
					P8_16,	//GPIO_46
					P8_17,	//GPIO_27
					P8_18,	//GPIO_65
					P8_19,	//GPIO_22
					P8_20,	//GPIO_63
					P8_21,	//GPIO_62
					P8_22,	//GPIO_37
					P8_23,	//GPIO_36
					P8_24,	//GPIO_33
					P8_25,	//GPIO_32
					P8_26,	//GPIO_61
					P8_27,	//GPIO_86
					P8_28,	//GPIO_88
					P8_29,	//GPIO_87
					P8_30,	//GPIO_80
					P8_31,	//GPIO_10
					P8_32,	//GPIO_11
					P8_33,	//GPIO_9
					P8_34,	//GPIO_81
					P8_35,	//GPIO_8
					P8_36,	//GPIO_80
					P8_37,	//GPIO_78
					P8_38,	//GPIO_79
					P8_39,	//GPIO_76
					P8_40,	//GPIO_77
					P8_41,	//GPIO_74
					P8_42,	//GPIO_75
					P8_43,	//GPIO_72
					P8_44,	//GPIO_73
					P8_45,	//GPIO_70
					P8_46,	//GPIO_71
					P9_11,	//GPIO_30
					P9_12,	//GPIO_60
					P9_13,	//GPIO_31
					P9_14,	//GPIO_50
					P9_15,	//GPIO_48
					P9_16,	//GPIO_51
					P9_17,	//GPIO_5
					P9_18,	//GPIO_4
					P9_21,	//GPIO_3
					P9_22,	//GPIO_2
					P9_23,	//GPIO_49
					P9_24,	//GPIO_15
					P9_25,	//GPIO_117
					P9_26,	//GPIO_14
					P9_27,	//GPIO_115
					P9_28,	//GPIO_113
					P9_29,	//GPIO_111
					P9_30,	//GPIO_112
					P9_31,	//GPIO_110
					P9_41,	//GPIO_20
					P9_42	//GPIO_7
		};

	const std::map<pinGPIO,string> pinGPIOName = {
				   //Pin	Name
					{P8_3,	"P8.3"},
					{P8_4,	"P8.4"},
					{P8_5,	"P8.5"},
					{P8_6,	"P8.6"},
					{P8_7,  "P8.7"},
					{P8_8,  "P8.8"},
					{P8_9,  "P8.8"},
					{P8_10, "P8.10"},
					{P8_11, "P8.11"},
					{P8_12, "P8.12"},
					{P8_13, "P8.13"},
					{P8_14, "P8.14"},
					{P8_15, "P8.15"},
					{P8_16, "P8.16"},
					{P8_17, "P8.17"},
					{P8_18, "P8.18"},
					{P8_19, "P8.19"},
					{P8_20, "P8.20"},
					{P8_21, "P8.21"},
					{P8_22, "P8.22"},
					{P8_23, "P8.23"},
					{P8_24, "P8.24"},
					{P8_25, "P8.25"},
					{P8_26, "P8.26"},
					{P8_27, "P8.27"},
					{P8_28, "P8.28"},
					{P8_29, "P8.29"},
					{P8_30, "P8.30"},
					{P8_31, "P8.31"},
					{P8_32, "P8.32"},
					{P8_33, "P8.33"},
					{P8_34, "P8.34"},
					{P8_35, "P8.35"},
					{P8_36, "P8.36"},
					{P8_37, "P8.37"},
					{P8_38, "P8.38"},
					{P8_39, "P8.39"},
					{P8_40, "P8.40"},
					{P8_41, "P8.41"},
					{P8_42, "P8.42"},
					{P8_43, "P8.43"},
					{P8_44, "P8.44"},
					{P8_45, "P8.45"},
					{P8_46, "P8.46"},
					{P9_11, "P9.11"},
					{P9_12, "P9.12"},
					{P9_13, "P9.13"},
					{P9_14, "P9.14"},
					{P9_15, "P9.15"},
					{P9_16, "P9.16"},
					{P9_17, "P9.17"},
					{P9_18, "P9.18"},
					{P9_21, "P9.21"},
					{P9_22, "P9.22"},
					{P9_23, "P9.23"},
					{P9_24, "P9.24"},
					{P9_25, "P9.25"},
					{P9_26, "P9.26"},
					{P9_27, "P9.27"},
					{P9_28, "P9.28"},
					{P9_29, "P9.29"},
					{P9_30, "P9.30"},
					{P9_31, "P9.31"},
					{P9_41, "P9.41"},
				    {P9_42,	"P9.42"}
	};

	const std::map<pinGPIO,int > pinGPIONumber = {
	               //Pin	Number
				    {P8_3,	38},
				    {P8_4,	39},
				    {P8_5,	34},
				    {P8_6,	35},
				    {P8_7,  66},
				    {P8_8,  67},
					{P8_9,  69},
					{P8_10, 68},
					{P8_11, 45},
					{P8_12, 44},
					{P8_13, 23},
					{P8_14, 26},
					{P8_15, 47},
					{P8_16, 46},
					{P8_17, 27},
					{P8_18, 65},
					{P8_19, 22},
					{P8_20, 63},
					{P8_21, 62},
					{P8_22, 37},
					{P8_23, 36},
					{P8_24, 33},
					{P8_25, 32},
					{P8_26, 61},
					{P8_27, 86},
					{P8_28, 88},
					{P8_29, 87},
					{P8_30, 80},
					{P8_31, 10},
					{P8_32, 11},
					{P8_33, 9},
					{P8_34, 81},
					{P8_35, 8},
					{P8_36, 80},
					{P8_37, 78},
					{P8_38, 79},
					{P8_39, 76},
					{P8_40, 77},
					{P8_41, 74},
					{P8_42, 75},
					{P8_43, 72},
					{P8_44, 73},
					{P8_45, 70},
					{P8_46, 71},
					{P9_11, 30},
					{P9_12, 60},
					{P9_13, 31},
					{P9_14, 50},
					{P9_15, 48},
					{P9_16, 51},
					{P9_17, 5},
					{P9_18, 4},
					{P9_21, 3},
					{P9_22, 2},
					{P9_23, 49},
					{P9_24, 15},
					{P9_25, 117},
					{P9_26, 14},
					{P9_27, 115},
					{P9_28, 113},
					{P9_29, 111},
					{P9_30, 112},
					{P9_31, 110},
					{P9_41, 20},
					{P9_42,	7}
	};

private:
	int number;			/**< The GPIO number of the object */
	int debounceTime;   /**< The debounce time in milliseconds */
	string name;		/**< The name of the GPIO e.g. gpio50 */
	string pinName;
	string path;  		/**< The full path to the GPIO e.g. /sys/class/gpio/gpio50/ */
	//string chipPath;
	//string system_export_param = "";

public:
	//GPIO(int number);
	GPIO(pinGPIO pinName_p);
	virtual int getNumber() { return number; } /**< Returns the GPIO number as an int. */

	// General Input and Output Settings
	virtual int setDirection(GPIO::DIRECTION);
	virtual GPIO::DIRECTION getDirection();
	virtual int setValue(GPIO::VALUE);
	virtual int toggleOutput();
	virtual GPIO::VALUE getValue();
	virtual int setActiveLow(bool isLow=true);  //low=1, high=0
	virtual int setActiveHigh(); //default
	//software debounce input (ms) - default 0
	virtual void setDebounceTime(int time) { this->debounceTime = time; }

	// Advanced OUTPUT: Faster write by keeping the stream alive (~20X)
	virtual int streamOpen();
	virtual int streamWrite(GPIO::VALUE);
	virtual int streamClose();

	virtual int toggleOutput(int time); //threaded invert output every X ms.
	virtual int toggleOutput(int numberOfTimes, int time);
	virtual void changeToggleTime(int time) { this->togglePeriod = time; }
	virtual void toggleCancel() { this->threadRunning = false; }

	// Advanced INPUT: Detect input edges; threaded and non-threaded
	virtual int setEdgeType(GPIO::EDGE);
	virtual GPIO::EDGE getEdgeType();
	virtual int waitForEdge(); // waits until button is pressed
	virtual int waitForEdge(CallbackType callback); // threaded with callback
	virtual void waitForEdgeCancel() { this->threadRunning = false; }

	virtual ~GPIO();  //destructor will unexport the pin

private:
	//int write(string path, string filename, string value);
	//int write(string path, string filename, int value);
	//string read(string path, string filename);
	int exportGPIO();
	int unexportGPIO();
	ofstream stream;
	pthread_t thread;
	CallbackType callbackFunction;
	long lastEventTime;
	bool threadRunning;
	int togglePeriod;  //default 100ms
	int toggleNumber;  //default -1 (infinite)
	friend void* threadedPoll(void *value);
	friend void* threadedToggle(void *value);
};

void* threadedPoll(void *value);
void* threadedToggle(void *value);

} /* namespace BBB */

#endif /* GPIOUNIV_H_ */
