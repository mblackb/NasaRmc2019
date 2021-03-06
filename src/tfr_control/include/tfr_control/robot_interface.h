/**
 * controller.h
 * 
 * Provides utilites for the hardware layer of the ros control stack.
 * One of its jobs is to go from commanded values, to hardware control, 
 * with a few safety measures. 
 * 
 * It's other job is to put feedback in the appropriate data structures,
 * that are used by the controllers at the control layer to appropriately
 * command the rover
 *
 * It has an an enum of joints, and a class that are detailed below.
 */
#ifndef ROBOT_INTERFACE_H
#define ROBOT_INTERFACE_H

#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int32.h>
#include <hardware_interface/joint_command_interface.h>
#include <hardware_interface/joint_state_interface.h>
#include <hardware_interface/robot_hw.h>
#include <utility>
#include <algorithm>
#include <tfr_msgs/ArduinoAReading.h>
#include <tfr_msgs/ArduinoBReading.h>
#include <tfr_msgs/PwmCommand.h>
#include <tfr_utilities/control_code.h>
#include <tfr_utilities/joints.h>
#include <vector>
#include <mutex>
#include <limits>
#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <iomanip>
#include <iostream>
#include <cstdio>

namespace tfr_control {

    /**
     * Contains the lower level interface inbetween user commands coming
     * in from the controller layer, and manages the state of all joints,
     * and sends commands to those joints
     * */
    class RobotInterface : public hardware_interface::RobotHW
    {
    public:

        RobotInterface(ros::NodeHandle &n, bool fakes, const double lower_lim[tfr_utilities::Joint::JOINT_COUNT],
                const double upper_lim[tfr_utilities::Joint::JOINT_COUNT]);
	
        
        /*
         * Reads state from hardware (encoders/potentiometers) and writes it to
         * shared memory 
         * */
        void read();
	
        /*
         * Takes commanded states from shared memory, enforces basic safety
         * contraints, and writes them to hardware
         * */
        void write();
	
	
        /*
         * retrieves the state of the bin
         * */
        double getBinState();
	
        /*
         * retrieves the state of the arm
         * */
        void getArmState(std::vector<double>&);
	
        /*
         * Clears all command values being sent and sets them to safe values
         * stops the treads and commands the arm to hold position.
         * */
        void clearCommands();
	
        void setEnabled(bool val);
	
        void zeroTurntable();
        
		
		

    private:
        //joint states for Joint state publisher package
        hardware_interface::JointStateInterface joint_state_interface;
        //cmd states for position driven joints
        hardware_interface::PositionJointInterface joint_position_interface;
        //cmd states for velocity driven joints
        hardware_interface::EffortJointInterface joint_effort_interface;
        
        bool enabled;

        double turntable_offset;

		// Read the relative velocity counters from the brushless motor controller
		ros::Subscriber brushless_right_tread_vel;
		ros::Subscriber brushless_left_tread_vel;
		
		volatile ros::Subscriber turntable_subscriber_encoder;
		volatile ros::Subscriber turntable_subscriber_amps;
		ros::Publisher  turntable_publisher;
		volatile int32_t turntable_encoder = 0;
		volatile double turntable_amps = 0.0;
		std::mutex turntable_mutex;
		
		volatile ros::Subscriber lower_arm_subscriber_encoder;
		volatile ros::Subscriber lower_arm_subscriber_amps;
		ros::Publisher  lower_arm_publisher;
		volatile int32_t lower_arm_encoder = 0;
		volatile double lower_arm_amps = 0.0;
		std::mutex lower_arm_mutex;
		
		volatile ros::Subscriber upper_arm_subscriber_encoder;
		volatile ros::Subscriber upper_arm_subscriber_amps;
		ros::Publisher  upper_arm_publisher;
		volatile int32_t upper_arm_encoder = 0;
		volatile double upper_arm_amps = 0.0;
		std::mutex upper_arm_mutex;
		
		volatile ros::Subscriber scoop_subscriber_encoder;
		volatile ros::Subscriber scoop_subscriber_amps;
		ros::Publisher  scoop_publisher;
		volatile int32_t scoop_encoder = 0;
		volatile double scoop_amps = 0.0;
		std::mutex scoop_mutex;
		
		void readTurntableEncoder(const std_msgs::Int32 &msg);
		void readTurntableAmps(const std_msgs::Float64 &msg);
		
		void readLowerArmEncoder(const std_msgs::Int32 &msg);
		void readLowerArmAmps(const std_msgs::Float64 &msg);
		
		void readUpperArmEncoder(const std_msgs::Int32 &msg);
		void readUpperArmAmps(const std_msgs::Float64 &msg);
		
		void readScoopEncoder(const std_msgs::Int32 &msg);
		void readScoopAmps(const std_msgs::Float64 &msg);
		
		ros::Publisher brushless_right_tread_vel_publisher;
		ros::Publisher brushless_left_tread_vel_publisher;
		
		
		std::mutex brushless_right_tread_mutex;
		int32_t accumulated_brushless_right_tread_vel = 0;
		int32_t accumulated_brushless_right_tread_vel_num_updates = 0;
		ros::Time accumulated_brushless_right_tread_vel_start_time;
		ros::Time accumulated_brushless_right_tread_vel_end_time;
		
		
		
		void setBrushlessLeftEncoder(const std_msgs::Int32 &msg);
		void setBrushlessRightEncoder(const std_msgs::Int32 &msg);
		
		int32_t left_tread_absolute_encoder_previous = 0;
		int32_t left_tread_absolute_encoder_current = 0;
		ros::Time left_tread_time_previous;
		ros::Time left_tread_time_current;
		
		int32_t right_tread_absolute_encoder_previous = 0;
		int32_t right_tread_absolute_encoder_current = 0;
		ros::Time right_tread_time_previous;
		ros::Time right_tread_time_current;
		
		const double pi = 3.14159265358979;
		
		std::mutex brushless_left_tread_mutex;
		int32_t accumulated_brushless_left_tread_vel = 0;
		int32_t accumulated_brushless_left_tread_vel_num_updates = 0;
		ros::Time accumulated_brushless_left_tread_vel_start_time;
		ros::Time accumulated_brushless_left_tread_vel_end_time;
		
		
		void accumulateBrushlessRightVel(const std_msgs::Int32 &msg);
		void accumulateBrushlessLeftVel(const std_msgs::Int32 &msg);
		
		
		double readBrushlessRightVel();
		double readBrushlessLeftVel();
		
		const bool enable_left_tread_pid_debug_output = true;
		
		ros::Publisher left_tread_publisher_pid_debug_setpoint;
		ros::Publisher left_tread_publisher_pid_debug_state;
		ros::Publisher left_tread_publisher_pid_debug_command;
		
		
		const int32_t brushless_encoder_count_per_revolution = 5120;
		double brushlessEncoderCountToRadians(int32_t encoder_count);
		double brushlessEncoderCountToRevolutions(int32_t encoder_count);
		double encoderDeltaToLinearSpeed(int32_t encoder_delta, ros::Duration time_delta);
		
        int32_t bin_encoder_min = 0;
        int32_t bin_encoder_max = 1000;
        double bin_joint_min = 0.0;
        double bin_joint_max = 0.0;

        int32_t turntable_encoder_min = -25760;
        int32_t turntable_encoder_max = 25760;
        double turntable_joint_min = -2 * 3.14159265358979;
        double turntable_joint_max = 2 * 3.14159265358979;

		
		/* 
			Arm all the way up (actutator extended):
				encoder: 0
				joint position: 0.1
			Arm all the way down (actuator retracted):
				encoder: 149
				joint position: 1.55
		*/
		 int32_t arm_lower_encoder_min = 888;
		 int32_t arm_lower_encoder_max = 0;
		 double arm_lower_joint_min = 0.104; // lower arm UP
		 double arm_lower_joint_max = 1.55;
		
		 int32_t arm_upper_encoder_min = 836; // arm UP
		 int32_t arm_upper_encoder_max = 0;
		 double arm_upper_joint_min = 0.98; // arm UP
		 double arm_upper_joint_max = 2.4; // arm DOWN, actuator EXTENDED
		
		 int32_t arm_end_encoder_min = 1721;
		 int32_t arm_end_encoder_max = 0;
		 double arm_end_joint_min = -1.16614; // scoop OPEN
		 double arm_end_joint_max = 1.62; // actuator EXTENDED, scoop CLOSED
		
		const int32_t get_arm_lower_min_int();
		const int32_t get_arm_lower_max_int();

        // Populated by controller layer for us to use
        double command_values[tfr_utilities::Joint::JOINT_COUNT]{};

        // Populated by us for controller layer to use
        double position_values[tfr_utilities::Joint::JOINT_COUNT]{};
        // Populated by us for controller layer to use
        double velocity_values[tfr_utilities::Joint::JOINT_COUNT]{};
        // Populated by us for controller layer to use
        double effort_values[tfr_utilities::Joint::JOINT_COUNT]{};
        //used to limit acceleration pull on the drivebase
        std::pair<double, double> drivebase_v0;
        ros::Time last_update;

        template <typename T>
    	T linear_interp(T x, T x1, T y1, T x2, T y2);

        template <typename T>
        T clamp(const T input, const T bound_1, const T bound_2);
        
        void registerJointEffortInterface(std::string name, tfr_utilities::Joint joint);
        void registerJointPositionInterface(std::string name, tfr_utilities::Joint joint);
        //void registerBinJoint(std::string name, Joint joint);

        void adjustFakeJoint(const tfr_utilities::Joint &joint);

        // THESE DATA MEMBERS ARE FOR SIMULATION ONLY
        // Holds the lower and upper limits of the URDF model joint
        bool use_fake_values = false;
        const double *lower_limits, *upper_limits;

    };
}

#endif // CONTROLLER_H
