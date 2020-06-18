#ifndef NODE
#define NODE

#include <iostream>
#include <vector>

class Node
{
	public:
	
		enum nodetype : int
		{
			if_up = 0,
			if_left = 1,
			left = 2,
			right = 3,
			straight = 4
		};

		Node(std::vector<std::string>& chromosome);
		
		std::vector<Node*> children;
		nodetype type;
		int evaluate(double z, double w, std::string& output);
	
};

#endif
