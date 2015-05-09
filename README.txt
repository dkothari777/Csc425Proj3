VM46-47

Authors:
-Nikolas Gelo - ngelo@email.arizona.edu
-Darshan Kothari - dkoathari@email.arizona.edu

The main folder of this project, the one this README file is contained, is the farthest were able to get. It successfully implements the following:
-The connections: Telnet Client <-> cproxy <-> sproxy <-> Telnet Daemon.
-Heartbeat and application data packet types
-Heartbeat functionality.

We have a subfolder titled "step2" that successfully implements Part 2 of the project's specifications.

We also have another subfolder titled "DEBUG_LEVEL_1" that successfully implements:
-Telnet Client <-> cproxy <-> Telnet Daemon

When you run our program, you will notice that we have debug logs being printed to standard output. We used these logs to validate and check that packets were correctly being send. If you would like to turn off these debug logs open debug.h, remove the comment on line 15 and comment out line 16. The debug logs can be kept on to observe cproxy & sproxy's activity.

