# Arduino-Smart-Alarm

Project of UPV Distributed Systems of the year 2021/2022. </br>

The project is an Arduino application connected through wi-fi to a Java client. </br> </br>

The project consists in an alarm watch with humidity and temperature sensors, built and programmed through Arduino, capable of communicating with a Java client, through which is possible to modify some settings, like the song chosen for the alarm or the time at which the alarm shohuld ring.

Furthermore the Java client sends to the Arduino periodical SET_TIME commands in order to synchronize the Arduino alarm with the correct time (in this case the time is the one of the computer running the Java client, which is supposed to be synchronized), so that the alarm doesn't lose seconds during the excecution.

