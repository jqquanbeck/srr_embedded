#include "ros/ros.h"
#include <pigpiod_if2.h>
#include <sstream>
#include <cmath>

#include "std_msgs/Float64.h" // left_speed, right_speed


/* Motor labeling convention:
	Front
	0	1

	2	3
	
	4	5
*/

const int MOTOR_QUANTITY = 6;
const int PWM_PINS[MOTOR_QUANTITY] = { 26, 18, 20, 21, 22, 23 };
const int DIR_PINS[MOTOR_QUANTITY]={ 5, 6, 16, 19, 12, 13 };

class Motor
{
public:
	void init(int pi, int pwm, int dir);
	void setSpeed(double speed);
	
private:
	int pi_;
	int pwm_;
	int dir_;
};

void Motor::init(int pi, int pwm, int dir)
{
	pi_ = pi;
	pwm_ = pwm;
	dir_ = dir;
}

void Motor::setSpeed(double speed) // speed -1 to +1
{
	// set direction output
	if (speed > 0)
		gpio_write(pi_, dir_, 1);
	else 
	{
		gpio_write(pi_, dir_, 0);
		speed = -speed;
	}

	// set magnitude
	set_PWM_dutycycle(pi_, pwm_, speed * 255.0);
	 ROS_INFO("PWM Val: %lf\n",speed * 255.0);
}

class Listener
{
public:
	Listener(int pi);
	
	void setM0speed(const std_msgs::Float64 msg) { motors[0].setSpeed(msg.data); }
	void setM1speed(const std_msgs::Float64 msg) { motors[1].setSpeed(msg.data); }
	void setM2speed(const std_msgs::Float64 msg) { motors[2].setSpeed(msg.data); }
	void setM3speed(const std_msgs::Float64 msg) { motors[3].setSpeed(msg.data); }
	void setM4speed(const std_msgs::Float64 msg) { motors[4].setSpeed(msg.data); }
	void setM5speed(const std_msgs::Float64 msg) { motors[5].setSpeed(msg.data); }

private:	
	Motor motors[MOTOR_QUANTITY];
};

Listener::Listener(int pi)
{
	// initialize motors
	for (int i = 0; i < MOTOR_QUANTITY; ++i)
	{
		motors[i].init(pi, PWM_PINS[i], DIR_PINS[i]);
	}
}


int main (int argc, char **argv)
{
	ros::init(argc, argv, "motor_control");
	
	ros::NodeHandle n;
	
	// initialize pigpio
	int pi = pigpio_start(NULL, NULL);	
	if (pi < 0)
		ROS_INFO("Error: %s\n", pigpio_error(pi));
	
	Listener listener(pi);
	
	ros::Subscriber M0Sub = n.subscribe("m0_speed_control_effort", 1000, &Listener::setM0speed, &listener);
	ros::Subscriber M1Sub = n.subscribe("m1_speed_control_effort", 1000, &Listener::setM1speed, &listener);
	ros::Subscriber M2Sub = n.subscribe("m2_speed_control_effort", 1000, &Listener::setM2speed, &listener);
	ros::Subscriber M3Sub = n.subscribe("m3_speed_control_effort", 1000, &Listener::setM3speed, &listener);
	ros::Subscriber M4Sub = n.subscribe("m4_speed_control_effort", 1000, &Listener::setM4speed, &listener);
	ros::Subscriber M5Sub = n.subscribe("m5_speed_control_effort", 1000, &Listener::setM5speed, &listener);
	
	ros::spin();
	
	return 0;
}
