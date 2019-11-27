"""
Client for the Leap Drone Controller

Supports the Mambo Fly drone through bluetooth connection
"""

from pyparrot.Minidrone import Mambo
import argparse
from multiprocessing import Queue, Process
import queue
import sys


STOPSIGNAL = "\tstop\t"


"""
Methods for responding to parent
"""


def complain(message):
    """
    Sends a message to the parent, likely one it doesn't expect.
    After this method is called, the child should either exit or
    expect that the parent will signal its end.
    :param message: whatever you'd like the parent to know
    :return:
    """
    print(message)


def acknowledge_command():
    print("ok")


"""
Command execution and drone communication methods
"""


def listen_and_execute(command_queue):
    """
    Reads commands from stdin and passes them to the drone object
    """
    while True:
        message = input()

        try:
            command_queue.put_nowait(message)
        except queue.Full:
            print("Command {} was skipped because child processor is overworked (queue full".format(message),
                  file=sys.stderr)

        acknowledge_command()
        if message == STOPSIGNAL:
            break


class Processor(object):
    """
    consumes commands and executes them
    """

    def __init__(self, command_queue, **kwargs):
        self.queue = command_queue
        self.is_flying = False
        self.connected = False

        self.droneAddr = kwargs.get('droneAddr', None)
        self.maxPitch = int(kwargs.get('maxPitch') or 50)
        self.maxVertical = int(kwargs.get('maxVertical') or 50)

        if self.droneAddr is None:
            sys.exit()

        # always connect with BT
        self.client = Mambo(self.droneAddr, use_wifi=False)

        # # set max horizontal/vertical speed
        # self.client.set_max_tilt(self.maxPitch)
        # self.client.set_max_vertical_speed(self.maxVertical)

    def connect(self):

        if self.client.connect(3):
            self.connected = True
            return True
        
        return False

    def fly(self, roll, pitch, yaw, vertical_movement):
        duration = 1
        
        roll = int(roll)
        pitch = int(pitch)
        yaw = int(yaw)
        vertical_movement = int(vertical_movement)

        if self.is_flying:
            self.client.fly_direct(roll, pitch, yaw, vertical_movement, duration)
            return True
        else:
            print("ERROR: Cannot fly when drone is not in flight mode", file=sys.stderr)
            return False

    def land(self, timeout=10):
        self.client.safe_land(timeout)
        self.is_flying = False
        return True

    def emergency(self):
        self.client.safe_emergency(10)
        self.client.disconnect()
        return False

    def takeoff(self, timeout=10):
        self.client.safe_takeoff(timeout)
        self.is_flying = True
        return True

    def process_commands(self):
        seconds_idle = 0
        while True:
            try:
                message = self.queue.get_nowait()
            except queue.Empty:
                if self.connected:
                    self.client.smart_sleep(1)
                    seconds_idle += 1

                # stop flying if idle for too long
                if seconds_idle >= 10:
                    self.client.safe_land(10)

                continue

            # Ignore blank commands
            if len(message) <= 0:
                continue

            if message == STOPSIGNAL:
                if self.is_flying:
                    self.client.safe_land(10)
                self.client.disconnect()
                break

            command = message.split()
            if not self.__getattribute__(command[0])(*command[1:]):
                self.client.safe_emergency(10)
                self.client.disconnect()
                complain("Command {} failed!".format(message))
                break

            # reset idle time
            seconds_idle = 0


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--droneAddr", required=True)
    parser.add_argument("--maxPitch", nargs="?", default=None)
    parser.add_argument("--maxVertical", nargs="?", default=None)

    args = vars(parser.parse_args())

    pendingCommands = Queue()
    processor = Processor(pendingCommands, **args)
    subprocess_args = (processor,)

    processor_process = Process(target=Processor.process_commands, args=subprocess_args, daemon=True)
    processor_process.start()

    # start reading commands from the parent
    listen_and_execute(pendingCommands)

    # wait on the process_command and then exit
    processor_process.join(timeout=10)