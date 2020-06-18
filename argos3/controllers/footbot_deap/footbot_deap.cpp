#include "footbot_deap.h"
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/core/utility/math/vector2.h>

#include <iostream>

CFootBotDeap::CFootBotDeap() :
   m_pcWheels(NULL),
   m_pcProximity(NULL),
   m_pcGripper(NULL),
   m_pcPosition(NULL),
   m_cAlpha(10.0f),
   m_fDelta(0.5f),
   m_fWheelVelocity(2.5f),
   m_cGoStraightAngleRange(-ToRadians(m_cAlpha),
                           ToRadians(m_cAlpha)) {
	m_count = 0;			   
}

CFootBotDeap::~CFootBotDeap()
{
	// print starting position and final positions on the x axis
	const CCI_PositioningSensor::SReading& pos = m_pcPosition->GetReading();
	std::cout << "sx " << GetId() << " " << initialPosition.GetX() << std::endl;	
	std::cout << "fx " << GetId() << " " << pos.Position.GetX() << std::endl;
}

void CFootBotDeap::Init(TConfigurationNode& t_node) {
	
	m_pcWheels    = GetActuator<CCI_DifferentialSteeringActuator>("differential_steering");
	m_pcProximity = GetSensor  <CCI_FootBotProximitySensor      >("footbot_proximity"    );
	m_pcGripper   = GetActuator<CCI_FootBotGripperActuator      >("footbot_gripper"      );
	m_pcPosition  = GetSensor	<CCI_PositioningSensor    			>("positioning"       	 );

	GetNodeAttributeOrDefault(t_node, "alpha", m_cAlpha, m_cAlpha);
	m_cGoStraightAngleRange.Set(-ToRadians(m_cAlpha), ToRadians(m_cAlpha));
	GetNodeAttributeOrDefault(t_node, "delta", m_fDelta, m_fDelta);
	GetNodeAttributeOrDefault(t_node, "velocity", m_fWheelVelocity, m_fWheelVelocity);
}

void CFootBotDeap::buildTree(std::vector<std::string> tokens)
{
	rootNode = new Node(tokens);	
}

void CFootBotDeap::ControlStep() 
{
	m_count++;
	if (m_count % 5 == 0)
	{
		m_count = 0;		
		
		const CCI_PositioningSensor::SReading& pos = m_pcPosition->GetReading();
		Real z = pos.Orientation.GetZ();
		Real w = pos.Orientation.GetW();
		
		std::string output;
		int result = rootNode->evaluate((double) z, (double) w, output);
		//std::cout << output << std::endl;
		
		if (result == -1)
		{
			turnLeft();
		}
		else if (result == 1)
		{
			turnRight();
		}
		else
		{
			turnStraight();
		}	
	}
}

REGISTER_CONTROLLER(CFootBotDeap, "footbot_deap_controller")
