/*
// experimenting with different ways to control niryo one with ROS
*/

// weird time bug: in catkin_ws
//find . -type f -exec touch {} +



#include "ros/ros.h"

#include "niryo_one_msgs/CloseGripper.h"
#include "niryo_one_msgs/OpenGripper.h"
#include "niryo_one_msgs/ToolActionGoal.h"
#include <niryo_one_msgs/RobotMoveAction.h>
#include <niryo_one_msgs/SetInt.h>

#include <actionlib/server/action_server.h>
#include <actionlib/client/action_client.h>
#include <actionlib/server/simple_action_server.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/simple_client_goal_state.h>
#include <actionlib/client/terminal_state.h>

typedef actionlib::SimpleActionServer<niryo_one_msgs::RobotMoveAction> NiryoServer;
typedef actionlib::SimpleActionClient<niryo_one_msgs::RobotMoveAction> NiryoClient;


int main(int argc, char **argv)
{
    ros::init(argc, argv, "open_gripper_client");
   
    ros::NodeHandle n;
    
    ros::Rate loop_rate(10);
    ros::AsyncSpinner spinner(3);
    spinner.start();

    ROS_INFO("Connecting to niryo");
    NiryoClient ac("/niryo_one/commander/robot_action/",true);
    while (!ac.waitForServer(ros::Duration(3.0))) 
    ROS_INFO("Connection to niryo succesfull");


    // ---------------------------------------
    // Position Control
    // ---------------------------------------

    niryo_one_msgs::RobotMoveActionGoal action;
    niryo_one_msgs::RobotMoveCommand cmd;
   
	ROS_INFO("Send position goal" );
    cmd.cmd_type = 2;
    cmd.position.x = .2; 
    cmd.position.y = .2;
    cmd.position.z = .2;
    cmd.rpy.roll = .5;
    cmd.rpy.pitch = .5;
    cmd.rpy.yaw = .5;
    action.goal.cmd = cmd;
    ac.sendGoal(action.goal);
    bool success = ac.waitForResult(ros::Duration(5.0));
    ROS_INFO("Position reached successfully");



    // ---------------------------------------
    // Gripper Control
    // ---------------------------------------

    ROS_INFO("Connecting to the gripper tool");
    ros::ServiceClient changeToolClient_;
    changeToolClient_ = n.serviceClient<niryo_one_msgs::SetInt>("/niryo_one/change_tool/");
    niryo_one_msgs::SetInt srv;
    srv.request.value = 11;
    while (!changeToolClient_.call(srv))
    ROS_INFO("Gripper connection successfull");

    ROS_INFO("Open Gripper");
    niryo_one_msgs::ToolCommand tcmd;
    tcmd.cmd_type = 1;
    tcmd.gripper_open_speed = 300;
    tcmd.tool_id = 11;
    action.goal.cmd.cmd_type = 6;
    action.goal.cmd.tool_cmd = tcmd;
    ac.sendGoal(action.goal);
    ac.waitForResult(ros::Duration(10.0));
    ROS_INFO("Gripper successfully opened");

    ROS_INFO("Close Gripper");
    tcmd.cmd_type = 2;
    tcmd.gripper_open_speed = 300;
    tcmd.tool_id = 11;
    action.goal.cmd.cmd_type = 6;
    action.goal.cmd.tool_cmd = tcmd;
    ac.sendGoal(action.goal);
    ac.waitForResult(ros::Duration(10.0));
    ROS_INFO("Gripper successfully closed");






    return 0;
}
