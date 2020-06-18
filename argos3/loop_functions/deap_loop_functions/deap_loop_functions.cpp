#include "deap_loop_functions.h"

#include <argos3/plugins/simulator/entities/box_entity.h>
#include <argos3/plugins/simulator/entities/cylinder_entity.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>

#include <controllers/footbot_deap/footbot_deap.h>

#include <sstream>
#include <list>
#include <algorithm>
#include <string>

CDeapLoopFunctions::CDeapLoopFunctions() {}
CDeapLoopFunctions::~CDeapLoopFunctions() {}

void CDeapLoopFunctions::Init(TConfigurationNode& t_tree) 
{
	CRandom::CRNG* pcRNG = CRandom::CreateRNG("argos");
	
	// make sure robots are placed in positions with room to travel up the x axis	
	CRange<Real> areaX(-2,0);
	CRange<Real> areaY(-2,2);

	// get random seed from file	
   std::ifstream seedFile("../seed.txt");
	std::string seed;
	while( getline(seedFile, seed) )
	{
		pcRNG->SetSeed(std::stoi(seed));
		pcRNG->Reset();
	}
		
	// get the filename for chromosome (best/chromosome)
	std::string filename;
	TConfigurationNodeIterator itDistr;
	for(itDistr = itDistr.begin(&t_tree); itDistr != itDistr.end(); ++itDistr) 
	{
		TConfigurationNode& tDistr = *itDistr;
		GetNodeAttribute(tDistr, "name", filename);
	}

	// read chromosome	
   std::ifstream chromosomeFile("../"+filename);
   
	int numRobots = 0;
	std::string chromosome;
	
	std::string line;
   while( getline(chromosomeFile, line) )
	{
		// number of robots
		if (numRobots == 0)
		{
			numRobots = std::stoi(line);
		}
		
		// get chromosome
		else
		{
			line.erase(std::remove(line.begin(), line.end(), ','), line.end());
			
			std::replace( line.begin(), line.end(), '(', ' ');
			std::replace( line.begin(), line.end(), ')', ' ');
			
			chromosome = line;
		}
	}
	
	// tokenize chromosome
	std::stringstream ss(chromosome); 
	std::string token;
	std::vector<std::string> tokens;
	while (std::getline(ss, token, ' ')) {
		if (token.length() > 0)
		{
			tokens.push_back(token);
		}
	}   
	
	// add robots
	for (int i = 0; i < numRobots; ++i)
	{
		std::cout << std::to_string(i) << std::endl;
		
		CFootBotEntity* pcFB = new CFootBotEntity(std::to_string(i), "fdc");
		AddEntity(*pcFB);
		
		CVector3 cFBPos;
		CQuaternion cFBRot;	
		
		int attempts = 0;
		bool finished = false;
		while (attempts < 20 && !finished)
		{
			attempts++;
		
			// position
			double x = pcRNG->Uniform(areaX);
			double y = pcRNG->Uniform(areaY);
			cFBPos.Set(x, y, 0.0f);	
			
			// rotation
			auto r = pcRNG->Uniform(CRadians::UNSIGNED_RANGE);
			cFBRot.FromAngleAxis(r, CVector3::Z);
			
			// place robot
			finished = MoveEntity(pcFB->GetEmbodiedEntity(), cFBPos, cFBRot);			
			if (attempts == 20 && !finished)
			{
				std::cout << "couldn't place robot " << std::to_string(i) << std::endl;
			}
		}
	
		// create robot
		CFootBotDeap& controller = dynamic_cast<CFootBotDeap&>(pcFB->GetControllableEntity().GetController());
		controller.buildTree(tokens);
		controller.setInitialPosition(cFBPos);
		controller.setInitialRotation(cFBRot);
	}

}

REGISTER_LOOP_FUNCTIONS(CDeapLoopFunctions, "deap_loop_functions");
