"""
Sample client code.

This code essentially acts like a subprocess. Its used to test that arguments were passed correctly
to the process and that the caller responds correctly to the expected behaviour of the subprocess.
"""

import argparse
import sys

parser = argparse.ArgumentParser()
parser.add_argument("--droneAddr", required=True)
parser.add_argument("--maxPitch", nargs="?", default=None)
parser.add_argument("--maxVertical", nargs="?", default=None)

args = vars(parser.parse_args())
print("Received Arguments: {}".format(args), file=sys.stderr)

# test arguments given
if len(args) < 3:
    sys.exit()

if args.get('droneAddr', None) is None:
    sys.exit()

if args.get('maxPitch', None) is None:
    sys.exit()

if args.get('maxVertical', None) is None:
    sys.exit()

connectIndicator = input()

# indicate readiness for commands
print("ok")

while True:
    command = input()
    print("ok")

    print("received: {}".format(command), file=sys.stderr)
    if command == "\tstop\t":
        break