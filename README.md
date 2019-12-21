# LeapDroneController

A c++ application that uses the Leap Motion to control the Parrot Mambo Fly Drone with hand gestures. 

LeapDroneController was the final project of my Advanced Programming Techniques Honors class. It was  a C++ class, which is why this project was written in mostly C++.

Requirements
============

**Operating System**: The application is tested on Ubuntu 19.04. Other linux versions may also work, but it depends on whatever is supported by the Leap Motion and pyparrot.


**Leap Motion**: Version 2.3 of the Leap Motion SDK is required. You can get it [here](https://developer.leapmotion.com/sdk/v2). 


**Parrot Mambo Drone**: A patched version of Pyparrot is required. Pyparrot needs a Python 3.7 interpreter and its own software requirements. You can find the software requirements [here](https://pyparrot.readthedocs.io/en/latest/installation.html).


**Compilation Requirements**: Tested with G++ v3.9.1 or higher and cmake v3.5 or higher.  


Installation
============

1. Download/Clone the repository. 

2. Download and extract the Leap motion SDK for linux.
    - If running a Debian based distribution, install the .deb file with `dpkg -i /path/to/Leap-2.3.x+xxxx-x86/x64.deb`. If running another distribution, you may find instructions on how to convert `.deb` to the package standard required for your distribution with a search engine (e.g. Google).

3. Copy the LeapSDK folder to the root directory of the repository. 

4. Assuming you have Python 3.7 installed, install pyparrot's dependencies.
   - If you don't have a Python 3, you can install it by following [this tutorial](https://linuxize.com/post/how-to-install-python-3-7-on-ubuntu-18-04/)
   
   - Optionally, you can install virtualenv to keep your system python packages clean. To install virtualenv run `python3 -m pip install virtualenv` from your commandline. To create the virtual environment, run `python3 -m virtualenv myenvironmentname` from your commandline. You should see `(myenvironmentname)` prefixing your command line prompt.
   
   - Install pyparrot's software requirements with `sudo apt-get install bluetooth bluez python-bluez libglib2.0-dev`. Note that these instructions assume that you're running Ubuntu. If you have a different distribution, you may need to search for those packages.
   
   - Install pyparrot's python package requirements with `python3 -m pip install untangle zeroconf bluepy`. If you're using the virtualenv, use `python -m pip install untangle zeroconf bluepy`.
  
5. Install the custom pyparrot.
    - Clone/download the custom pyparrot repository: https://github.com/ajpen/pyparrot
    - From inside the directory of the downloaded repository, run `python3 -m pip install .` (or `python -m pip install .` from within the virtualenv)
    

6. Prepare build requirements.
    - Create a folder called build in the root directory of the repository with `mkdir build`.
    - Generate build requirements with `cd build; cmake ../`.
    - Build application binary using `make LeapDroneController`. If this command runs successfully, you'll see a binary named `LeapDroneController` in the `build` directory. Thats the application binary. 
    
    
Running
=========

**Note** Before running, you need to configure the application. Create a file named `config.txt` in the same directory as the `LeapDroneController` binary, and open the file using your preferred editor. Here's a starting template of the configuration:

```
"command":"/path/to/python3 /path/to/repository/parrot/parrotclient/client.py"
"droneAddr":"'d0:3a:ee:02:e6:22'"
"maxPitch":"40"
"maxVertical":"34"
```

Be sure to change the `droneAddr` to the address of your drone. You can find the address by putting the drone in pairing mode, connecting to it using your laptop, and checking the device settings for the device address. You can also follow the tutorial [here](https://pyparrot.readthedocs.io/en/latest/quickstartminidrone.html#ble-connection).

Also, change the path for the python3 interpreter to the interpreter with pyparrot installed. 
Finally replace `/path/to/repository` with the full correct path to the repository.


**Steps:**

1. Ensure that the Leap Motion Daemon is running. If it isn't, run it. Running it depends on the running linux distribution and may vary. The leap motion documentation may be helpful for this. One can try running `sudo leapd` from a separate terminal to run the daemon. 

2. Connect the leap motion to the computer. Ensure that the leap motion IR lights are on (faint red light from the top of the controller). If not, stop the daemon and run it again. 

3. Put the drone in pairing mode (flashing green light.)

4. Ensure that the configuration file is in the same directory as the application binary.

5. Run the application binary (using `./LeapDroneController`)


Controlling the Drone
======================

To takeoff, point a single finger outwards and rotate it clockwise, creating a full circle ([like this](https://www.youtube.com/watch?v=nHHD996k7G4)).

To land, do the same as takeoff but rotate counterclockwise.

**Flying:** 
The drone mimics the orientation of your outstretched hand, where the direction your fingers point is the same as the front of the drone. 

Tilting your hand downwards makes the drone go forward. 

Tilting it upwards makes the drone go backwards. 

Tilting it left makes the drone swing left. 

Tilting it right makes the drone swing right. 

Twisting your hand left or right (using your wrist as if you're wiping an imaginary table) rotates the drone in the direction that you're twisting.


[Demo](https://youtu.be/HgkMTQy89RE)
==========

