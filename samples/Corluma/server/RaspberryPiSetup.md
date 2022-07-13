## <a name="setup"></a>Setup on a Raspberry Pi
_This guide shows how to set up a Raspberry Pi to be a dedicated lights server. Setup steps should be similar but not identical in different Raspberry Pi environments._
* To start out, install your favorite OS onto a Raspberry Pi. For this guide, I will be using Raspbian Stretch.
* Next, make sure you are running python3, and [install pip](https://pip.pypa.io/en/stable/installing/). In many distros, you can just run:
```
sudo apt-get install pip
```
* With pip, install PySerial:
```
pip install pyserial
```
* That's it for setting up your python environment! Next we want to set up a local static IP on the Raspberry Pi so that you can easily find it on the network. Your method of doing this will vary from distro to distro and may even depend on your router's configuration. I found [this thread](https://www.raspberrypi.org/forums/viewtopic.php?t=191140) useful for Raspbian Stretch.
* Test that your static IP address is working by rebooting the board and checking that the IP is correct.
* Save the IP address with your preferred method (writing it down, saving it in a .txt on another machine, staring at it for a super long time, etc.). We'll be using it to connect to the board soon.
* Network configuration complete! Now its time to set up the script. First, make a directory for this script:
```
mkdir ~/scripts
```
* Next, pull the script from the web by navigating to the directory and running wget
```
cd ~/scripts
wget https://github.com/timsee/ArduCor/blob/master/samples/Corluma/server/UDPtoSerialAdapter.py
```
* Now that you have the script on the Pi, connect your Arduino running an _ArduCor_ sample to the raspberry pi over USB.
* Find the serial connection's name on the Raspberry Pi. Two common ones would be `/dev/ttyUSB0` and `/dev/ttyACM0` but it may be different for you. You can use the arduino IDE to find and test the serial connection if you prefer GUIs, or you can use command line with  `dmesg | grep tty`.
* Now its time to test the script. Test the script by running this, replacing `$SERIAL` with the serial connection name from the last step. If you want to connect to multiple arduino samples, add them all as arguments to the python script and separate those arguments with spaces:
```
cd ~/scripts
python UDPtoSerial.py $SERIAL
```
* Use whatever method you want to try to connect to the lights over UDP. I tend to use [Corluma](https://github.com/timsee/Corluma).  A good test of communication is to send a UDP packet with the ASCII string `DISCOVERY_PACKET`. You should receive a discovery packet back from _ArduCor_ sample.
* Almost there, Now that you've proven that you can communicate with the Arduino through the Raspberry Pi, the last steps set things ups so the python script runs automatically on bootup. Modify your startup script to call the `UDPtoSerialAdapter.py`. It is recommended to use `systemd` to add a startup service to run the script. Once you've built the systemd service, you can run the script like this, replacing `$USER` with your username (this will typically be _pi_ unless you created a new user) and `$SERIAL` with the serial connection's name:
```
python /home/$USER/scriptsUDPtoSerial.py $SERIAL
```
* Save your modifications and reboot the Raspberry Pi. Congratulations, you should now be able to control the Arduino through the Raspberry Pi using UDP packets!
