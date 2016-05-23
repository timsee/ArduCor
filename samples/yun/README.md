# Arduino Yun Sample Sketches

Sample Sketches for the Arduino Yun. These implement the same packet protocol as the standard samples and have all the same functionalities, but instead of serial they can use either HTTP or UDP. 

## <a name="toc"></a>Table of Contents

* [HTTP](#http)
* [UDP](#udp)

#### <a name="http"></a>HTTP


HTTP uses an arduino yun to provide an interface similar to an IP Camera to an LED array. Commands can be sent through a web browser in the format of `http://IP_ADRESS/arduino/MESSAGE` where `IP_ADDRESS` is the current IP address of the arduino and `MESSAGE` is an ASCII message packet formatted in the same way as the [other samples](../). For example, to turn off the lights on an Arduino Yun on *192.168.0.101* you would enter this link into your web browser:

```
http://192.168.0.101/arduino/0,0
```

This method is very convenient since it can be controlled via web browser, but its major drawback is the speed that the Arduino Yun can handle HTTP requests. On the tested network, it took up to a second for the Yun to update the LEDs after sending a command.


#### <a name="udp"></a>UDP

UDP provides a much faster data stream than HTTP, but the increased speed comes at the cost of more configuration. Arduino does not provide a standard UDP library, so the onboard Linux processor is set up to load a UDP server at boot. This is a simple server: all packets received over UDP are immediately put on the Arduino's Bridge without buffering. The sketch running on the microcontroller polls the bridge on a loop to receive the new packets, where its sent through the same parsing as the serial and HTTP samples. 


To get this sample running, you'll need the IP address of your arduino and the password to ssh into it. 
For our sample commands, we will be using the IP address of `192.168.0.102` but this will likely be different in your environment. 

* ssh into your arduino yun. To do so, enter your terminal and run this command:

```
ssh root@192.168.0.102
```
* It will ask you if you are sure you trust your host (choose yes) and then it will ask for the password that you entered during the initial setup of the arduino yun. 

* Now that you're in the arduino's terminal, type this command to make a directory for your sample code:

```
mkdir scripts
```
* Now that we have a place to put the script, we're going to modify the arduino yun's bootup script so that it runs the server automatically. The arduino yun has a very basic version of linux installed, which leads to limited options for text editors. I recommend using vi, which may require you to look up keybindings if you are not familiar. Load `/etc/rc.local` into the text editor, and add this line above the `exit 0` line:

```
python /root/scripts/yun-udp-server.py
```
* Great, now everything that required ssh should be all set up. The next step is to open a new terminal
and navigate to the `samples/yun/udp-server` directory in this git repo. Then, secure copy the server's python script to the arduino yun. To do so, run this line: 

```
scp yun-udp-server.py root@192.168.0.102:/root/scripts

```
* Almost there! Upload your desired UDP sample sketch to the arduino yun's microcontroller. Do this through the arduino application or however else you normally upload new sketches.

* Restart your arduino yun. If everything is set up correctly, the server should start up at boot and after 30-60 seconds, you'll be able to control the arduino over UDP.







