CloudPlugs REST Client for ANSI C
=================================

CloudPlugs REST Client for ANSI C is a library to perform REST requests to the CloudPlugs server.

Official repository https://github.com/cloudplugs/rest-c

Documentation at https://docs.cloudplugs.com/rest-c/html

See the LICENSE file for more details.

Copyright 2015 CloudPlugs Inc. http://www.cloudplugs.com

Required libraries
----

Libcurl 7.0+: http://curl.haxx.se/
(for SSL support it is mandatory to use libcurl compiled with built-in SSL support)

Jansson 2.4+: https://github.com/akheron/jansson
(optional)


Compile on Debian/Ubuntu
----

Install required packages:
```
sudo apt-get install build-essential autoconf libtool pkg-config
```

Install curl library:
```
sudo apt-get install libcurl4-openssl-dev
```

If available, you can directly install the package:
```
sudo apt-get install libjansson-dev
```

If not, you can download, compile and install Jansson library:
```
wget http://www.digip.org/jansson/releases/jansson-2.7.tar.gz
tar xvfz jansson-2.7.tar.gz
cd jansson-2.7/
./configure
make
sudo make install
sudo ldconfig
```

Finally download, compile and install the CloudPlugs REST library:
```
git clone git://github.com/cloudplugs/rest-c
cd rest-c
autoreconf -i
./configure
make
sudo make install
```
