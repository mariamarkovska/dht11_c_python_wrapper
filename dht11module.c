/*
 *  dht11module.c:
 *	a python wrapped c function to read temperature and humidity from DHT11 or DHT22 sensor
 *  original code for sensor communication from http://www.uugear.com/portfolio/read-dht1122-temperature-humidity-sensor-from-raspberry-pi/ (2020-01-30)
 */

#include<python2.7/Python.h> // change this to your library path

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
 
#define MAX_TIMINGS	85

// Python exception object
static PyObject *dht11modError;
 
int data[5] = { 0, 0, 0, 0, 0 };

struct Measurement {
    float temp;
    float hum;
};

// C function to read the sensor
struct Measurement * read_dht_data_c(int DHT_PIN, struct Measurement *meas)
{
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j			= 0, i;
 
	data[0] = data[1] = data[2] = data[3] = data[4] = 0;
 
	/* pull pin down for 18 milliseconds */
	pinMode( DHT_PIN, OUTPUT );
	digitalWrite( DHT_PIN, LOW );
	delay( 18 );
 
	/* prepare to read the pin */
	pinMode( DHT_PIN, INPUT );
 
	/* detect change and read data */
	for ( i = 0; i < MAX_TIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHT_PIN ) == laststate )
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHT_PIN );
 
		if ( counter == 255 )
			break;
 
		/* ignore first 3 transitions */
		if ( (i >= 4) && (i % 2 == 0) )
		{
			/* shove each bit into the storage bytes */
			data[j / 8] <<= 1;
			if ( counter > 16 )
				data[j / 8] |= 1;
			j++;
		}
	}
 
	/*
	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	 * print it out if data is good
	 */
	if ( (j >= 40) &&
	     (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) )
	{
		float h = (float)((data[0] << 8) + data[1]) / 10;
		if ( h > 100 )
		{
			h = data[0];	// for DHT11
		}
		float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
		if ( c > 125 )
		{
			c = data[2];	// for DHT22
		}
		if ( data[2] & 0x80 )
		{
			c = -c;
		}
		// float f = c * 1.8f + 32;
	    (*meas).temp = c;
    	(*meas).hum = h;
        return meas;
	}else  {
	    return NULL;
	}

	// Test code for my PC
    // int r = rand() % 20;
    
    // if(r%3 != 0){
    //     float c = 20.3;
    //     float h = 2.0;

	//     (*meas).temp = c;
    // 	(*meas).hum = h;
    //     return meas;	
	// }
    // else {
	// 	// printf( "Data not good, skip\n" );
    //     return NULL;
	// }
}

// Python wrapper function to read the sensor
static PyObject * read_dht_data(PyObject *self, PyObject *args){
	
	// Setup wiringPi pins
	if(wiringPiSetup() == -1){
		exit(1);
	}

	// Parse args from python
	int dht_pin;
    // We expect at least 1 string argument to this function
    if(!PyArg_ParseTuple(args, "i", &dht_pin)){
        return NULL; // return error if no input is found
    }
    
	// Read measurement from sensor
	struct Measurement meas;
    struct Measurement *mptr = read_dht_data_c(dht_pin, &meas);
    if(mptr==NULL){ // if measurement was not valid, throw exception
        PyErr_SetString(dht11modError, "Bad data from sensor"); // set the exception variable
        return NULL;
    }

	// return the valid measurements as a python dictionary object
    return Py_BuildValue("{s:d,s:d}", "temperature", meas.temp, "humidity", meas.hum);
}

// Function table
static PyMethodDef dht11_methods[] = {
    //"python name", c-function name, argument presentation, description
    {"read", read_dht_data, METH_VARARGS, "Read dht11 sensor data"},
    {NULL, NULL, 0, NULL}  /* sentinel */
};

// Initialization function
PyMODINIT_FUNC initdht11(void){
    PyObject *m;
    m = Py_InitModule("dht11", dht11_methods);
    if(m==NULL) return;

    dht11modError = PyErr_NewException("dht11.error",NULL,NULL); // create python error object
    Py_INCREF(dht11modError); // increase reference (since we own this ref)
    PyModule_AddObject(m, "error", dht11modError); // register to python interpreter
}