#include <Eigen/Geometry>
#include <math.h>

#define sampleFreq	512.0f		// sample frequency in Hz
#define betaDef		0.1f		// 2 * proportional gain

//---------------------------------------------------------------------------------------------------
// Variable definitions

volatile float beta = betaDef;								// 2 * proportional gain (Kp)
volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;	// quaternion of sensor frame relative to auxiliary frame
Quaternionf q;
q=Eigen::Quaternionf(1.0f,0.0f,0.0f,0.0f);
//---------------------------------------------------------------------------------------------------
// Function declarations

double mx,my,mz;

void imumagnetic(const sensor_msgs::Imu::ConstPtr &msg)
{
	mx=msg->MagneticField.x;
	my=msg->MagneticField.y;
	mz=msg->MagneticField.z;
}

void imuCallback(const sensor_msgs::Imu::ConstPtr &msg){

  double x,y,z,w;

  x=msg->orientation.x;
  y=msg->orientation.y;
  z=msg->orientation.z;
  w=msg->orientation.w;
  Quaternionf q;
  q=Eigen::Quaternionf(x,y,z,w);

  double gx,gy,gz,ax,ay,az;
  gx = msg->angular_velocity.x;
  gy = msg->angular_velocity.y;
  gz = msg->angular_velocity.z;

  ax = msg->linear_acceleration.x;
  ay = msg->linear_acceleration.y;
  az = msg->linear_acceleration.z;

  imumagnetic();

  MadgwickAHRSupdate(gx,gy,gz,ax,ay,az,mx,my,mz);
 // R=quat.toRotationMatrix();
}
//====================================================================================================
// Functions

//---------------------------------------------------------------------------------------------------
// AHRS algorithm update

void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) {
	// Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
	Quaternionf qgyro = Eigen::Quaternionf(0.0f,gx,gy,gz);

	Quaternionf qDot = Eigen::Quaternionf(1.0f,0.0f,0.0f,0.0f);

	Quaternionf qh = Eigen::Quaternionf(1.0f,0.0f,0.0f,0.0f);

	Quaternionf qb = Eigen::Quaternionf(1.0f,0.0f,0.0f,0.0f);

	Quaternionf qacc = Eigen::Quaternionf(0.0f,ax,ay,az);

	Quaternionf qmag = Eigen::Quaternionf(0.0f,mx,my,mz);

	Quaternionf delf = Eigen::Quaternionf(1.0f , 0.0f,0.0f,0.0f);

	//
  	qDot=q*qgyro;

	qDot.w()=qDot.w()*0.5;
	qDot.vec()=qDot.vec()*0.5;

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

    qacc.normalise():
    qmag.normalise()
    	//calculating the h quaternion which is the mag data in the earth frame
		qh=((q*qmag)*q.conjugate());
		qb.x()=sqrt(qh.x()*qh.x()+qh.y()*qh.y();
    	qb.z()=qh.z();
    	qb.w()=0;
    	qb.y()=0;

    	qb.normalise();
		// Gradient decent algorithm corrective step
		/*s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q3 + _2bz * q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q2 + _2bz * q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q3 - _4bz * q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * q2 - _2bz * q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q1 + _2bz * q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q0 - _4bz * q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * q3 + _2bz * q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q0 + _2bz * q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
    */

    MatrixXf Fg(3,1);
    MatrixXf Fb(3,1);
   	MatrixXf Jb(3,4);
   	MatrixXf Jg(3,4);

   	//calcuating the acceleration covariance matrix
	Fg<<2*(q.x()*q.z()-q.w()*q.y())-qacc.x(),2*(q.w()*q.x()+q.y()*q.z())-qacc.y(),2*(0.5-q.x()*q.x()-q.y()*q.y())-qacc.z();
   	
   	Fb<<2*b.x()*(0.5-q.y()*q.y()-q.z()*q.z())+2*b.z()*(q.x()*q.z()-q.w()*q.y())-mx,2*b.x()*(q.x()*q.y()-q.w()*q.z())+2*b.z()*(q.w()*q.x()+q.z()*q.y())-my,2*bx*(q.w()*q.y()-q.x()*q.z*())+2*b.z()*(0.5-q.x()*q.x()-q.y()*q.y())-mz;
	
	//this is jacobian of the accelerometer
	Jg<<-2*q.y(),2*q.z(),-2*q.w(),2*q.x(),2*q.x(),2*q.w(),2*q.z(),2*q.y(),0,-4*q.x(),-4*q.y(),0;
	
	Jb<<-2*b.z()*q.y(),2*b.z()*q.z(),-4*b.x()*q.y()-2*b.z()*q.w(),-2*b.x()*q.z()+2*b.z()*q.x(),2*b.x()*q.y()+2*b.z()*q.w(),2*b.x()*q.x()+2*b.z()*q.z(),2*b.x()*q.y(),2*b.x()*q.z()-4*b.z()*q.x(),2*b.x()*q.w()-4*b.z()*q.y(),2*b.x()*q.x();

	MatrixXf Fgb(6,1);
	MatrixXf Jgb(6,4);

	Fgb<<Fg,Fb;
	Jgb<<Jg,Jb;

	delf=Jgb.transpose()*Fgb;
		/*recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;*/
		delf.normalize();

		// Apply feedback step
		/*qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;*/

		qDot.w()=qDot.w()-(beta*delf.w());
		qDot.vec()=qDot.vec()-(beta*delf.vec());

	// Integrate rate of change of quaternion to yield quaternion
	/*q0 += qDot1 * (1.0f / sampleFreq);
	q1 += qDot2 * (1.0f / sampleFreq);
	q2 += qDot3 * (1.0f / sampleFreq);
	q3 += qDot4 * (1.0f / sampleFreq);*/
	q.w()=qDot.w()*(1.0f / sampleFreq);
	q.vec()=qDot.vec()*(1.0f / sampleFreq);
	// Normalise quaternion
	/*recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;*/
	q.normalize()
	}
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "offb_node");
    ros::NodeHandle nh;

    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>
            ("mavros/state", 10, state_cb);
    ros::Subscriber imu_sub = nh.subscribe<sensor_msgs::Imu>
            ("/mavros/imu/data",10,imuCallback);
    ros::Subscriber gps_sub = nh.subscribe<geometry_msgs::PoseStamped>
            ("/mavros/local_position/pose",100,gpsCallback);
    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 10);
    ros::Subscriber sub = nh.subscribe<geometry_msgs::PoseStamped>
            ("aruco_single/pose", 1000, posecallback);
    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>
            ("mavros/cmd/arming");
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>
            ("mavros/set_mode");

    //the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(20.0);

    // wait for FCU connection
    while(ros::ok() && !current_state.connected){
        ros::spinOnce();
        rate.sleep();
    }


    pose.pose.position.x = 0;
    pose.pose.position.y = 0;
    pose.pose.position.z = 2;

    //send a few setpoints before starting
    for(int i = 100; ros::ok() && i > 0; --i){
        local_pos_pub.publish(pose);
        ros::spinOnce();
        rate.sleep();
    }

    mavros_msgs::SetMode offb_set_mode;
    offb_set_mode.request.custom_mode = "OFFBOARD";

    mavros_msgs::CommandBool arm_cmd;
    arm_cmd.request.value = true;

    ros::Time last_request = ros::Time::now();

    while(ros::ok()){
        if( current_state.mode != "OFFBOARD" &&
            (ros::Time::now() - last_request > ros::Duration(5.0))){
            if( set_mode_client.call(offb_set_mode) &&
                offb_set_mode.response.mode_sent){
                ROS_INFO("Offboard enabled");
            }
            last_request = ros::Time::now();
        } else {
            if( !current_state.armed &&
                (ros::Time::now() - last_request > ros::Duration(5.0))){
                if( arming_client.call(arm_cmd) &&
                    arm_cmd.response.success){
                    ROS_INFO("Vehicle armed");
                }
                last_request = ros::Time::now();
            }
        }

        local_pos_pub.publish(pose);

        ros::spinOnce();
        rate.sleep();
    }

    return 0;
}
//---------------------------------------------------------------------------------------------------
// IMU algorithm update

// void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az) {
// 	float recipNorm;
// 	float s0, s1, s2, s3;
// 	float qDot1, qDot2, qDot3, qDot4;
// 	float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

// 	// Rate of change of quaternion from gyroscope
// 	qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
// 	qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
// 	qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
// 	qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

// 	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
// 	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

// 		// Normalise accelerometer measurement
// 		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
// 		ax *= recipNorm;
// 		ay *= recipNorm;
// 		az *= recipNorm;

// 		// Auxiliary variables to avoid repeated arithmetic
// 		_2q0 = 2.0f * q0;
// 		_2q1 = 2.0f * q1;
// 		_2q2 = 2.0f * q2;
// 		_2q3 = 2.0f * q3;
// 		_4q0 = 4.0f * q0;
// 		_4q1 = 4.0f * q1;
// 		_4q2 = 4.0f * q2;
// 		_8q1 = 8.0f * q1;
// 		_8q2 = 8.0f * q2;
// 		q0q0 = q0 * q0;
// 		q1q1 = q1 * q1;
// 		q2q2 = q2 * q2;
// 		q3q3 = q3 * q3;

// 		// Gradient decent algorithm corrective step
// 		s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
// 		s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
// 		s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
// 		s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
// 		recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
// 		s0 *= recipNorm;
// 		s1 *= recipNorm;
// 		s2 *= recipNorm;
// 		s3 *= recipNorm;

// 		// Apply feedback step
// 		qDot1 -= beta * s0;
// 		qDot2 -= beta * s1;
// 		qDot3 -= beta * s2;
// 		qDot4 -= beta * s3;
// 	}

// 	// Integrate rate of change of quaternion to yield quaternion
// 	/*q0 += qDot1 * (1.0f / sampleFreq);
// 	q1 += qDot2 * (1.0f / sampleFreq);
// 	q2 += qDot3 * (1.0f / sampleFreq);
// 	q3 += qDot4 * (1.0f / sampleFreq);
// */
//    q.w()=qDot.w()*(1.0f / sampleFreq);
//    q.vec()=qDot.vec()*(1.0f / sampleFreq);

// 	// Normalise quaternion
// 	/*recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
// 	q0 *= recipNorm;
// 	q1 *= recipNorm;
// 	q2 *= recipNorm;
// 	q3 *= recipNorm;*/
// 	q.normalize()
// }

//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root

/*float invSqrt(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}*/
