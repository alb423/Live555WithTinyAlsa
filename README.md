
README
===========================
Integrate live555 with tinyalsa to impelment an audio streaming server and an audion streaming client. 

****

|Author|Albert.Liao|
|---|---
|E-mail|alb423@gmail.com

****



## Feature list
1. Revise testRTSPClient.c to render audio by tinyalsa. 
2. Revise wis-streamer as standalone audio streaming server. (change from ALSA to tinyalsa).

## How to install tinyalsa library
* sudo apt-add-repository ppa:taylorcholberton/tinyalsa
* sudo apt-get update
* sudo apt-get install tinyalsa
* sudo apt-get install libtinyalsa-dev

## How to build live555 library
* Download file from http://www.live555.com/liveMedia/public, the latest source I dowload is live.2018.02.12.tar.gz
* tar -xvf live.2018.02.12.tar.gz 
* cd live
* ./genMakefiles linux-with-shared-libraries
* make clean;make
* make install DESTDIR=~/test/x86_live555

## Usage
* git clone https://github.com/alb423/Live555WithTinyAlsa.git
* cd Live555WithTinyAlsa
* make clean;make
* export LD_LIBRARY_PATH=~/test/x86_live555/usr/local/lib
* ./client/testRTSPClient rtsp://192.168.153.3:8554/PCM
* ./server/wis-streamer

## Reference:
* https://github.com/tinyalsa/tinyalsa
* http://albert-oma.blogspot.tw/2018/01/vlc.html