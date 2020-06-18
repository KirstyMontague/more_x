#include "node.h"

#include <sstream>
#include <bits/stdc++.h> 

#define PI 3.1415926

Node::Node(std::vector<std::string>& chromosome)
{
	if (chromosome.size() > 0)
	{
		std::string word = chromosome.at(0);
		
		int token;
		if (word == "if_up") token = 0;
		if (word == "if_left") token = 1;
		if (word == "left") token = 2;
		if (word == "right") token = 3;
		if (word == "straight") token = 4;
		
		chromosome.erase(chromosome.begin());
		
		switch (token)
		{
			case 0:
				type = nodetype::if_up;
				children.push_back(new Node(chromosome));
				children.push_back(new Node(chromosome));
				break;
			case 1:
				type = nodetype::if_left;
				children.push_back(new Node(chromosome));
				children.push_back(new Node(chromosome));
				break;
			case 2: 
				type = nodetype::left;
				break;
			case 3:
				type = nodetype::right;
				break;
			case 4:
				type = nodetype::straight;
				break;
			default: 
				std::cout << "node error" << std::endl;
		}
	}
}

int Node::evaluate(double z, double w, std::string& output)
{	
	switch (type)
	{
		case Node::nodetype::if_up:
		{
			if (asin(sqrt(z*z)) < PI / 4)
			{
				output += " ifz (y) ";
				return children[0]->evaluate (z, w, output);
			}
			else
			{
				output += "ifz (n) ";
				return children[1]->evaluate (z, w, output);
			}
		}
		
		case Node::nodetype::if_left:
			if (z * w > 0)    
			{
				output += "if_left (y) ";
				return children[0]->evaluate (z, w, output);
			}
			else
			{
				output += "if_leftw (n) ";
				return children[1]->evaluate (z, w, output);
			}

		case Node::nodetype::left: 
		
			output += "left ";
			return -1;

		case Node::nodetype::right:
			output += "right ";
			return 1;

		case Node::nodetype::straight:
			output += "straight ";
			return 0;

		default: 
			std::cout << "evaluation error" << std::endl;
	}

	return 0;
}
