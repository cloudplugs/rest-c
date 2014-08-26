CloudPlugs REST Client for ANSI C
=================================

CloudPlugs REST Client for ANSI C is a library to perform REST requests to the CloudPlugs server.

Official repository https://github.com/cloudplugs/rest-c

Documentation at https://docs.cloudplugs.com/rest-c/html

See the LICENSE file for more details.

Copyright 2014 CloudPlugs Inc. http://www.cloudplugs.com

Required libraries
----

Libcurl 7.0+: http://curl.haxx.se/


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
wget http://www.digip.org/jansson/releases/jansson-2.6.tar.gz
tar xvfz jansson-2.6.tar.gz
cd jansson-2.6/
./configure
make
sudo make install
sudo ldconfig
```

Finally download, compile and install the CloudPlugs REST library:
```
git clone git://github.com/cloudplugs/repo-c
cd repo-c
autoreconf -i
./configure
make
sudo make install
```
