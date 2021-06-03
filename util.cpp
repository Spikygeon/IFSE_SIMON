/*
 *
 */

#include "util.h"
#include<iostream>
#include<fstream>
#include<sstream>
using namespace std;

namespace BBB {

bool fileExist (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}
/**
 * Helper write function that writes a single string value to a file in the path provided
 * @param path The sysfs path of the file to be modified
 * @param filename The file to be written to in that path
 * @param value The value to be written to the file
 * @return
 */
int write(string path, string filename, string value){
   ofstream fs;
   fs.open((path + filename).c_str());
   if (!fs.is_open()){
	   perror("GPIO: write failed to open file ");
	   cout << "Filename of error: " << (path + filename) << endl;
	   return -1;
   }
   fs << value;
   fs.close();
   return 0;
}
/**
 * Helper read function that reads a single string value to a file from the path provided
 * @param path The sysfs path of the file to be read
 * @param filename Filename The file to be written to in that path
 * @return
 */
string read(string path, string filename){
   ifstream fs;
   fs.open((path + filename).c_str());
   if (!fs.is_open()){
	   perror("GPIO: read failed to open file ");
	   cout << "Filename of error: " << (path + filename) << endl;
    }
   string input;
   getline(fs,input);
   fs.close();
   return input;
}

/**
 * Private write method that writes a single int value to a file in the path provided
 * @param path The sysfs path of the file to be modified
 * @param filename The file to be written to in that path
 * @param value The int value to be written to the file
 * @return
 */
int write(string path, string filename, int value){
   stringstream s;
   s << value;
   return write(path,filename,s.str());
}

////// Time management useful routines /////////////
/**
*
*/
long toMicroseconds(struct timespec *ts)
{
	return ((ts->tv_sec)*1000000 + (ts->tv_nsec) / 1000);
}

/**
* Adds "delay" microseconds to timespecs and sleeps until that new time
* This function is intended to implement periodic processes with absolute
* activation times.
*/
void sleep_until(struct timespec *ts, int delay)
{
	long oneSecond = 1000*1000*1000;
	ts->tv_nsec += delay * 1000;
	if(ts->tv_nsec >= oneSecond){
		ts->tv_nsec -= oneSecond;
		ts->tv_sec++;
	}
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, ts,  NULL);
}

/**
* Sleeps "delay" microseconds.
* This function is intended to implement periodic processes with relative
* activation times.
*/
void sleep_lapse(int delay)
{
	long oneSecond = 1000*1000; //in microseconds
	struct timespec ts;
	ts.tv_sec = delay / oneSecond;
	ts.tv_nsec = (delay % oneSecond) * 1000;
	clock_nanosleep(CLOCK_MONOTONIC, 0, &ts,  NULL);
}


long getCurrentMicroseconds() {
	struct timespec currentTime;
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	return (currentTime.tv_sec)*1000000 + (currentTime.tv_nsec) / 1000;
}
//////////////////////////////////////////////////


} /* namespace BBB */
