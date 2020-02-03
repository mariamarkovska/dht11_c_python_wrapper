# dht11_c_python_wrapper
A python wrapper for c code to read from dht11 temperature and humidity sensor.
Created for usage on ASUS TinkerBoard and utilizes TinkerBoard GPIO C library, which is an extention of the wiringPi library for Raspberry Pi.

## Prerequisites
The GPIO C library for Tinker Board: https://github.com/TinkerBoard/gpio_lib_c

Python developer package: ```apt-get install python-dev```

## Install package
Build:
```python setup.py build```

Install:
```sudo python setup.py install```

## Functionality
The module contains only one function named *read* in Python. The function takes a wiringPi pin as input. For example using the wiringPi pin 0 corresponds to the TinkerBoard physical pin 11. To display a GPIO map for TinkerBoard type:

```gpio readall```

The DHT11 sensor reading is timing critical, so if CPU is assigned to other tasks during reading, the timing requirement might not be reached and the reading fails. Currently, the function throws an exception for failed readings. Another possible approach could be to cache and send the last successful reading.

For successful readings the function returns a Python dictionary containing temperature and humidity measurements.

## Example usage

```
import dht11

dht_pin = 0 # define pin number to read

while(1):
    try:
        res = dht11.read(dht_pin) # read sensor data
        print(res)
    except dht11.error as e: # catch exceptions
        print(e)
```