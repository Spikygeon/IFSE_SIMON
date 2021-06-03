/**
 *
 * PWMuniv.h
 * Author: Jose Simo (2019)
 * Universitat Politecnica de Valencia. AI2-DISCA
 * Creative Commons.
 *
 */
#ifndef PWMUNIV_H_
#define PWMUNIV_H_

#include<string>

using std::string;

#define PWM_PERIOD "period"
#define PWM_DUTY "duty_cycle"
#define PWM_POLARITY "polarity"
#define PWM_RUN "enable"

namespace BBB {

class PWM {
public:
	enum POLARITY{ ACTIVE_LOW=0, ACTIVE_HIGH=1 };

	enum pinPWM { P9_22, P9_31, //EHRPWM0-A
				  P9_21, P9_29, //EHRPWM0-B
				  P9_42,		//ECAPPWM0
				  P9_14, P8_36, //EHRPWM1-A
				  P9_16, P8_34, //EHRPWM1-B
				  P8_19, P8_45, //EHRPWM2-A
				  P8_13, P8_46, //EHRPWM2-B
				  P9_28 };		//ECAPPWM2

private:
	string pinName;
	string path;
	string chipPath;
	float analogFrequency;  //defaults to 100,000 Hz
	float analogMax;        //defaults to 3.3V

public:
	PWM(pinPWM pinName_p);

	virtual int setPeriod(unsigned int period_ns);
	virtual unsigned int getPeriod();
	virtual int setFrequency(float frequency_hz);
	virtual float getFrequency();
	virtual int setDutyCycle(unsigned int duration_ns);
	virtual int setDutyCycle(float percentage);
	virtual unsigned int getDutyCycle();
	virtual float getDutyCyclePercent();

	virtual int setPolarity(PWM::POLARITY);
	virtual void invertPolarity();
	virtual PWM::POLARITY getPolarity();

	virtual void setAnalogFrequency(float frequency_hz) { this->analogFrequency = frequency_hz; }
	virtual int calibrateAnalogMax(float analogMax); //must be between 3.2 and 3.4
	virtual int analogWrite(float voltage);

	virtual int run();
	virtual bool isRunning();
	virtual int stop();

	virtual ~PWM();
private:
	float period_nsToFrequency(unsigned int);
	unsigned int frequencyToPeriod_ns(float);
};

} /* namespace BBB */

#endif /* PWMUNIV_H_ */
