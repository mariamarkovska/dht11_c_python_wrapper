from distutils.core import setup, Extension

dht11_module = Extension('dht11',
    library_dirs = ['/usr/local/include/python2.7/'],
    libraries = ['wiringPi'], 
    sources=['dht11module.c']
)

setup (name = 'dht11',
    version = '1.0',
    description = 'This is a package for reading measurements from the dht11 sensor with c code',
    author = 'mariamarkovska',
    url = 'https://github.com/mariamarkovska',
    ext_modules=[dht11_module]
)