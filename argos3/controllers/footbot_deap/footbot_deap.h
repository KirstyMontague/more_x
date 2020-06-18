#ifndef FOOTBOT_DEAP_H
#define FOOTBOT_DEAP_H

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_proximity_sensor.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_gripper_actuator.h>

#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>

#include "node.h"


using namespace argos;

class CFootBotDeap : public CCI_Controller {

public:

   CFootBotDeap();
   virtual ~CFootBotDeap();

   virtual void Init(TConfigurationNode& t_node);
   virtual void ControlStep();
   virtual void Reset() {}
   virtual void Destroy() {}
   
   void buildTree(std::vector<std::string> tokens);
   void setInitialPosition(CVector3 position) {initialPosition = position;}
   void setInitialRotation(CQuaternion rotation) {initialRotation = rotation;}
   
   
private:

	void turnStraight() {m_pcWheels->SetLinearVelocity(m_fWheelVelocity, m_fWheelVelocity);}
	void turnLeft() {m_pcWheels->SetLinearVelocity(0.0f, m_fWheelVelocity);}
	void turnRight() {m_pcWheels->SetLinearVelocity(m_fWheelVelocity, 0.0f);}

	void avoidance();
	void navigation();
	
	
	Node* rootNode;
	int m_count;
	CVector3 initialPosition;
	CQuaternion initialRotation;
   
   // sensors / actuators
   
   CCI_DifferentialSteeringActuator* m_pcWheels;	/* Pointer to the differential steering actuator */
   CCI_FootBotProximitySensor* m_pcProximity;		/* Pointer to the foot-bot proximity sensor */
   CCI_FootBotGripperActuator* m_pcGripper;			/* Pointer to the foot-bot gripper actuator */
   CCI_PositioningSensor* m_pcPosition;				/* Pointer to the foot-bot position sensor */


   /* <parameters> */

   /* Maximum tolerance for the angle between the robot heading direction and the closest obstacle detected. */
   CDegrees m_cAlpha;
   
   /* Maximum tolerance for the proximity reading between the robot and the closest obstacle. The proximity
    * reading is 0 when nothing is detected and grows exponentially to 1 when the obstacle is touching the robot.
    */
   Real m_fDelta;
   
   /* Wheel speed. */
   Real m_fWheelVelocity;
   
   /* Angle tolerance range to go straight. It is set to [-alpha,alpha]. */
   CRange<CRadians> m_cGoStraightAngleRange;

};

#endif
