/********************************************************************
 *   MIT License
 *  
 *   Copyright (c) 2017  Huei-Tzu Tsai, Steven Gambino
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 ********************************************************************/

/** @file testservicebot.cpp
 *  @brief Implementation of unit test for ROS ServiceBot node
 *
 *  This file contains implementation of unit test for ROS ServiceBot node
 *
 *  @author Huei Tzu Tsai \n
 *          Steven Gambino
 *  @date   05/06/2017
*/

#include <ros/ros.h>
#include <ros/package.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Twist.h>
#include <actionlib_msgs/GoalID.h>
#include <gtest/gtest.h>
#include <sstream>
#include "servicebot/commandService.h"
#include "testhelper.hpp"
#include "servicebot.hpp"


/**
 *   @brief  spin thread to process callbacks
 *
 *   @param  continue flag in boolean \n
 *           true to continue, false to terminate thread
 *   @return none
*/
void processThread(bool *cont) {
    ros::Rate loop_rate(10);

    while (ros::ok() && *cont) {
        ros::spinOnce();
        loop_rate.sleep();
    }
}


/**
 *   @brief  Verify initialization in ServiceBot class
 *
 *   @param  none
 *   @return none
*/
TEST(TestServiceBot, testInit) {
    ros::NodeHandle n;
    TestHelper testItem;
    std_msgs::String msg;

    ros::Rate loop_rate(1);

    // register to check number of publishers to /servicebot/command topic
    ros::Subscriber sub = n.subscribe("/servicebot/command", 1000,
                                      &TestHelper::testCommandCallback, &testItem);

    // register to check number of subscribers to /servicebot/command
    ros::Publisher pub = n.advertise<std_msgs::String>("/servicebot/command", 1000);

    loop_rate.sleep();

    EXPECT_NE(0, sub.getNumPublishers());
    EXPECT_NE(0, pub.getNumSubscribers());

    ros::ServiceClient client =
        n.serviceClient<servicebot::commandService>("commandService");

    // Assert service to be ready in 1 second
    EXPECT_TRUE(client.waitForExistence(ros::Duration(1)));
}


/**
 *   @brief  Verify commandService in ServiceBot class
 *
 *   @param  none
 *   @return none
*/
TEST(TestServiceBot, testCommandService) {
    ros::NodeHandle n;
    TestHelper testItem;

    ros::Rate loop_rate(2);

    // Subscribe topic serviceCommand from master to receive messages published on this topic
    ros::Subscriber sub = n.subscribe("/servicebot/command", 1000,
                                      &TestHelper::testCommandCallback, &testItem);

    // allow callback to process
    loop_rate.sleep();

    // Register client with the master
    ros::ServiceClient client =
        n.serviceClient<servicebot::commandService>("commandService");

    // Assert service to be ready in 1 second
    EXPECT_TRUE(client.waitForExistence(ros::Duration(1)));

   // Create request & response objects
    servicebot::commandService::Request req;
    servicebot::commandService::Response resp;

    // Fill in request data
    req.command = "test";
    req.args = "";

    // Call service, expect return TRUE
    EXPECT_TRUE(client.call(req, resp));

    // Expect response is OK
    EXPECT_STREQ("OK", resp.resp.c_str());

    // allow callback to process
    loop_rate.sleep();

    // Expect response is test
    EXPECT_STREQ("test", testItem.cmd.c_str());
}


/**
 *   @brief  Verify name command in ServiceBot class
 *
 *   @param  none
 *   @return none
*/
TEST(TestServiceBot, testNameCommand) {
    ros::NodeHandle n;
    TestHelper testItem;
    std_msgs::String msg;

    ros::Rate loop_rate(2);

    // Register to publish topic 
    ros::Publisher pub = n.advertise<std_msgs::String>("/servicebot/command", 1000);

    // Subscribe topic /robotsound to verify name command
    ros::Subscriber sub = n.subscribe("/robotsound", 1000,
                                      &TestHelper::testRobotSoundCallback, &testItem);
    loop_rate.sleep();

    // test name command
    msg.data = "your name,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    EXPECT_EQ(sound_play::SoundRequest::SAY, testItem.snd);
    EXPECT_EQ(sound_play::SoundRequest::PLAY_ONCE, testItem.sndCmd);
    EXPECT_STREQ("my name is servicebot", testItem.cmd.c_str());
}


/**
 *   @brief  Verify time command in ServiceBot class
 *
 *   @param  none
 *   @return none
*/
TEST(TestServiceBot, testTimeCommand) {
    ros::NodeHandle n;
    TestHelper testItem;
    std_msgs::String msg;

    ros::Rate loop_rate(2);

    // Register to publish topic 
    ros::Publisher pub = n.advertise<std_msgs::String>("/servicebot/command", 1000);

    // Subscribe topic /robotsound to verify time command
    ros::Subscriber sub = n.subscribe("/robotsound", 1000,
                                      &TestHelper::testRobotSoundCallback, &testItem);
    loop_rate.sleep();

    // test time command
    msg.data = "what time,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    EXPECT_EQ(sound_play::SoundRequest::SAY, testItem.snd);
    EXPECT_EQ(sound_play::SoundRequest::PLAY_ONCE, testItem.sndCmd);
    EXPECT_TRUE(testItem.cmd.find("time now is") != std::string::npos);
}


/**
 *   @brief  Verify play command in ServiceBot class
 *
 *   @param  none
 *   @return none
*/
TEST(TestServiceBot, testPlayCommand) {
    ros::NodeHandle n;
    TestHelper testItem;
    std_msgs::String msg;
    std::string demoMusic = DEMO_MUSIC;

    ros::Rate loop_rate(2);

    // Register to publish topic 
    ros::Publisher pub = n.advertise<std_msgs::String>("/servicebot/command", 1000);

    // Subscribe topic /robotsound to verify play music command
    ros::Subscriber sub = n.subscribe("/robotsound", 1000,
                                      &TestHelper::testRobotSoundCallback, &testItem);

    loop_rate.sleep();

    // play music without arguments (should play default demo music)
    msg.data = "play music";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    EXPECT_EQ(sound_play::SoundRequest::PLAY_FILE, testItem.snd);
    EXPECT_EQ(sound_play::SoundRequest::PLAY_ONCE, testItem.sndCmd);
    EXPECT_STREQ(demoMusic.c_str(), testItem.cmd.c_str());

    // play music by specifying music file path
    std::stringstream musicPath;
    std::stringstream command;
    std::string path = ros::package::getPath("servicebot");
    musicPath << path << "/demo/01.mp3";
    command << "play music," << musicPath.str();

    msg.data = command.str();
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    EXPECT_EQ(sound_play::SoundRequest::PLAY_FILE, testItem.snd);
    EXPECT_EQ(sound_play::SoundRequest::PLAY_ONCE, testItem.sndCmd);
    EXPECT_STRCASEEQ(musicPath.str().c_str(), testItem.cmd.c_str());
}


/**
 *   @brief  Verify stop play command in ServiceBot class
 *
 *   @param  none
 *   @return none
*/
TEST(TestServiceBot, testStopPlayCommand) {
    ros::NodeHandle n;
    TestHelper testItem;
    std_msgs::String msg;

    ros::Rate loop_rate(2);

    // Register to publish topic 
    ros::Publisher pub = n.advertise<std_msgs::String>("/servicebot/command", 1000);

    // Subscribe topic /robotsound to verify stop play music command
    ros::Subscriber sub = n.subscribe("/robotsound", 1000,
                                      &TestHelper::testRobotSoundCallback, &testItem);
    loop_rate.sleep();

    msg.data = "play music";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    msg.data = "stop music";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    EXPECT_EQ(sound_play::SoundRequest::PLAY_STOP, testItem.sndCmd);
}


/**
 *   @brief  Verify move to commands in ServiceBot class
 *
 *   @param  none
 *   @return none
*/
TEST(TestServiceBot, testMoveToCommand) {
    ros::NodeHandle n;
    TestHelper testItem;
    std_msgs::String msg;

    ros::Rate loop_rate(2);

    // Register to publish topic 
    ros::Publisher pub = n.advertise<std_msgs::String>("/servicebot/command", 1000);

    ros::Subscriber subMBGoal = n.subscribe("/move_base/goal", 1000,
                                      &TestHelper::testMoveBaseGoalCallback, &testItem);

    ros::Subscriber subMBCancel = n.subscribe("/move_base/cancel", 1000,
                                      &TestHelper::testMoveBaseCancelCallback, &testItem);

    loop_rate.sleep();

    ROS_DEBUG_STREAM("subMBGoal: number of publisher = " << subMBGoal.getNumPublishers());
    ROS_DEBUG_STREAM("subMBCancel: number of publisher = " << subMBCancel.getNumPublishers());

    // test move to room a
    msg.data = "move to,room a";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    geometry_msgs::Pose expGoal;
    expGoal.position.x = -5;
    expGoal.position.y = -11;
    expGoal.position.z = 0.0;
    expGoal.orientation.x = 0.0;
    expGoal.orientation.y = 0.0;
    expGoal.orientation.z = 0.950;
    expGoal.orientation.w = 0.312;

    geometry_msgs::Pose actGoal = testItem.pos;

    EXPECT_TRUE(0 == std::memcmp(&expGoal, &actGoal, sizeof(expGoal)));

    // test cancel command
    msg.data = "cancel,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    EXPECT_STREQ(testItem.goalID.c_str(), testItem.cancelID.c_str());

    // test move to room b
    msg.data = "move to,room b";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    expGoal.position.x = 7.1;
    expGoal.position.y = -10.7;
    expGoal.position.z = 0.0;
    expGoal.orientation.x = 0.0;
    expGoal.orientation.y = 0.0;
    expGoal.orientation.z = -0.713;
    expGoal.orientation.w = 0.702;

    actGoal = testItem.pos;

    EXPECT_TRUE(0 == std::memcmp(&expGoal, &actGoal, sizeof(expGoal)));

    // test abort command
    msg.data = "abort,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    EXPECT_STREQ(testItem.goalID.c_str(), testItem.cancelID.c_str());

    // test go to room c
    msg.data = "go to,room c";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    expGoal.position.x = 0;
    expGoal.position.y = 0;
    expGoal.position.z = 0;
    expGoal.orientation.x = 0.0;
    expGoal.orientation.y = 0.0;
    expGoal.orientation.z = -0.713;
    expGoal.orientation.w = 0.702;

    actGoal = testItem.pos;

    EXPECT_TRUE(0 == std::memcmp(&expGoal, &actGoal, sizeof(expGoal)));

    msg.data = "stop moving,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    EXPECT_STREQ(testItem.goalID.c_str(), testItem.cancelID.c_str());
}


/**
 *   @brief  Verify come back commands in ServiceBot class
 *
 *   @param  none
 *   @return none
*/
TEST(TestServiceBot, testComeBackCommand) {
    ros::NodeHandle n;
    TestHelper testItem;
    std_msgs::String msg;

    ros::Rate loop_rate(2);

    // Register to publish topic 
    ros::Publisher pub = n.advertise<std_msgs::String>("/servicebot/command", 1000);

    ros::Subscriber subMBGoal = n.subscribe("/move_base/goal", 1000,
                                      &TestHelper::testMoveBaseGoalCallback, &testItem);

    ros::Subscriber subMBCancel = n.subscribe("/move_base/cancel", 1000,
                                      &TestHelper::testMoveBaseCancelCallback, &testItem);

    loop_rate.sleep();

    ROS_DEBUG_STREAM("subMBGoal: number of publisher = " << subMBGoal.getNumPublishers());
    ROS_DEBUG_STREAM("subMBCancel: number of publisher = " << subMBCancel.getNumPublishers());

    msg.data = "come back,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    geometry_msgs::Pose expGoal;
    expGoal.position.x = 0;
    expGoal.position.y = 0;
    expGoal.position.z = 0.0;
    expGoal.orientation.x = 0.0;
    expGoal.orientation.y = 0.0;
    expGoal.orientation.z = 0.0;
    expGoal.orientation.w = 1.0;

    geometry_msgs::Pose actGoal = testItem.pos;

    EXPECT_TRUE(0 == std::memcmp(&expGoal, &actGoal, sizeof(expGoal)));

    msg.data = "cancel";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    EXPECT_STREQ(testItem.goalID.c_str(), testItem.cancelID.c_str());
}


/**
 *   @brief  Verify forward command in ServiceBot class
 *
 *   @param  none
 *   @return none
*/
TEST(TestServiceBot, testForwardCommand) {
    TestHelper testItem;
    ros::NodeHandle n;
    std_msgs::String msg;

    ros::Rate loop_rate(2);

    // Register to publish topic 
    ros::Publisher pub = n.advertise<std_msgs::String>("/servicebot/command", 1000);

    ros::Subscriber sub = n.subscribe("/mobile_base/commands/velocity", 1000,
                                      &TestHelper::testMBCmdVelocityCallback, &testItem);

    loop_rate.sleep();

    ROS_DEBUG_STREAM("sub: number of publisher = " << sub.getNumPublishers());

    msg.data = "forward,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    geometry_msgs::Twist expTwist;
    expTwist.linear.x = 0.1;
    expTwist.linear.y = 0.0;
    expTwist.linear.z = 0.0;
    expTwist.angular.x = 0.0;
    expTwist.angular.y = 0.0;
    expTwist.angular.z = 0.0;

    geometry_msgs::Twist actTwist = testItem.twist;

    EXPECT_TRUE(0 == std::memcmp(&expTwist, &actTwist, sizeof(expTwist)));

    msg.data = "stop,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    expTwist.linear.x = 0.0;
    actTwist = testItem.twist;

    EXPECT_TRUE(0 == std::memcmp(&expTwist, &actTwist, sizeof(expTwist)));
}


/**
 *   @brief  Verify backward commands in ServiceBot class
 *
 *   @param  none
 *   @return none
*/
TEST(TestServiceBot, testBackwardCommand) {
    ros::NodeHandle n;
    TestHelper testItem;
    std_msgs::String msg;

    ros::Rate loop_rate(2);

    // Register to publish topic 
    ros::Publisher pub = n.advertise<std_msgs::String>("/servicebot/command", 1000);

    ros::Subscriber sub = n.subscribe("/mobile_base/commands/velocity", 1000,
                                      &TestHelper::testMBCmdVelocityCallback, &testItem);

    loop_rate.sleep();

    ROS_DEBUG_STREAM("sub: number of publisher = " << sub.getNumPublishers());

    msg.data = "backward,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    geometry_msgs::Twist expTwist;
    expTwist.linear.x = -0.1;
    expTwist.linear.y = 0.0;
    expTwist.linear.z = 0.0;
    expTwist.angular.x = 0.0;
    expTwist.angular.y = 0.0;
    expTwist.angular.z = 0.0;

    geometry_msgs::Twist actTwist = testItem.twist;

    EXPECT_TRUE(0 == std::memcmp(&expTwist, &actTwist, sizeof(expTwist)));

    msg.data = "stop,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    expTwist.linear.x = 0.0;
    actTwist = testItem.twist;

    EXPECT_TRUE(0 == std::memcmp(&expTwist, &actTwist, sizeof(expTwist)));
}


/**
 *   @brief  Verify turn left commands in ServiceBot class
 *
 *   @param  none
 *   @return none
*/
TEST(TestServiceBot, testTurnLeftCommand) {
    ros::NodeHandle n;
    TestHelper testItem;
    std_msgs::String msg;

    ros::Rate loop_rate(2);

    // Register to publish topic 
    ros::Publisher pub = n.advertise<std_msgs::String>("/servicebot/command", 1000);

    ros::Subscriber sub = n.subscribe("/mobile_base/commands/velocity", 1000,
                                      &TestHelper::testMBCmdVelocityCallback, &testItem);

    loop_rate.sleep();

    ROS_DEBUG_STREAM("sub: number of publisher = " << sub.getNumPublishers());

    msg.data = "turn left,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    geometry_msgs::Twist expTwist;
    expTwist.linear.x = 0.0;
    expTwist.linear.y = 0.0;
    expTwist.linear.z = 0.0;
    expTwist.angular.x = 0.0;
    expTwist.angular.y = 0.0;
    expTwist.angular.z = 0.1;

    geometry_msgs::Twist actTwist = testItem.twist;

    EXPECT_TRUE(0 == std::memcmp(&expTwist, &actTwist, sizeof(expTwist)));

    msg.data = "stop,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    expTwist.angular.z = 0.0;
    actTwist = testItem.twist;

    EXPECT_TRUE(0 == std::memcmp(&expTwist, &actTwist, sizeof(expTwist)));
}


/**
 *   @brief  Verify turn right commands in ServiceBot class
 *
 *   @param  none
 *   @return none
*/
TEST(TestServiceBot, testTurnRightCommand) {
    ros::NodeHandle n;
    TestHelper testItem;
    std_msgs::String msg;

    ros::Rate loop_rate(2);

    // Register to publish topic 
    ros::Publisher pub = n.advertise<std_msgs::String>("/servicebot/command", 1000);

    ros::Subscriber sub = n.subscribe("/mobile_base/commands/velocity", 1000,
                                      &TestHelper::testMBCmdVelocityCallback, &testItem);

    loop_rate.sleep();

    ROS_DEBUG_STREAM("sub: number of publisher = " << sub.getNumPublishers());

    msg.data = "turn right,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    geometry_msgs::Twist expTwist;
    expTwist.linear.x = 0.0;
    expTwist.linear.y = 0.0;
    expTwist.linear.z = 0.0;
    expTwist.angular.x = 0.0;
    expTwist.angular.y = 0.0;
    expTwist.angular.z = -0.1;

    geometry_msgs::Twist actTwist = testItem.twist;

    EXPECT_TRUE(0 == std::memcmp(&expTwist, &actTwist, sizeof(expTwist)));

    msg.data = "stop,";
    pub.publish(msg);

    // allow process to message publish
    loop_rate.sleep();

    expTwist.angular.z = 0.0;
    actTwist = testItem.twist;

    EXPECT_TRUE(0 == std::memcmp(&expTwist, &actTwist, sizeof(expTwist)));
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ros::init(argc, argv, "testservicebot");
    ros::NodeHandle nh;

    bool cont = true;

    // spawn another thread
    boost::thread th(processThread, &cont);

    int ret = RUN_ALL_TESTS();

    cont = false;

    // wait the second thread to finish
    th.join();

    return ret;
}