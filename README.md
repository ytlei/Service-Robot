# Service Robot
[![Build Status](https://travis-ci.org/StevieG47/Service-Robot.svg?branch=master)](https://travis-ci.org/StevieG47/Service-Robot)
[![Coverage Status](https://coveralls.io/repos/github/htsai51/Service-Robot/badge.svg?branch=master)](https://coveralls.io/github/htsai51/Service-Robot?branch=master)

## Overview
This is an indoor service robot project which can take simple voice commands from the end user, and navigate autonomously to perform a required task. This indoor service robot is implemented in ROS Gazebo simulation environment using the TurtleBot as the base platform.

## Authors
- Steven Gambino
- Huei-Tzu (Jane) Tsai

## License
MIT open-source license, see LICENSE.TXT

## Link to AIP Sheet
[Team Agile Iterative Process](https://docs.google.com/spreadsheets/d/1iFT3fMAvNtwBAL32Rg6VP-Avbg5zfWhtl9Rwo7dgiHw/edit#gid=1455614482)

## Link to notes
[Spring Planning Notes and Review](https://docs.google.com/document/d/1rKvWalXGmUbCn-tPqvxoK4kGyR2MJIU4lqQ_D-8Y_HA/edit)

## Operation and Demo

## Dependencies
- Ubuntu 14.04
- ROS indigo
- TurtleBot_Gazebo
- Rviz
- Package Dependencies
    - roscpp
    - std_msgs
    - message_generation
    - geometry_msgs
    - move_base_msgs
    - pocketsphinx
    - sound_play
    - actionlib
    - actionlib_msgs
    - nav_msgs
    - nav_core
    - base_local_planner
    - tf


## Known Issues

## Developer Documentation


## How to generate doxygen documentation

- In your . directory

```bash
doxygen ./Doxygen
```

- Doxygen files will be generated to ./docs folder
