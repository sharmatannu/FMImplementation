#include <map>
#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <istream>
#include <ostream>
#include <iterator>
#include <algorithm>
#include <time.h>
using namespace std;

class Edge;
class Node;

class hyperGraph
{
	private :
		map<string, Node*> Nodes; 			//name of the nodes can be a string
		map<int, Edge*> Edges; 				//hyperedges
		int num_Edges, num_Nodes;			//number of nodes and hyperedges
		float r;							//UBFactor, or balance factor
		string fName;						//file name
	map<int,list<string>*> maxLists;		//map
	
	public : 	
		int count[2];
		int numOfCutEdges;
		list<string> gainUpdateList;		//to capture nodes that getUpdates during update cycle
		int prevAssignment;
		
		hyperGraph(string fileName)
		{
			srand (time(NULL));
			fName = fileName;
			ifstream file;
			file.open(fileName.c_str()) ; 
			string line;
			getline(file,line);				//read the first line ;
			std::istringstream iss(line);
			iss >> num_Edges >> num_Nodes;
			int edgeNumber = 1;
			numOfCutEdges = 0;
			prevAssignment = 0;
			r = 0.5;					//UBfactor: This is balance . 
			count[0] = 0;
			count[1] = 0;
			//cout<<num_Edges; 
			while(getline(file,line))		//for all the subsequent lines in the file
			{
				std::istringstream edgeS(line);	
	
				list<string> listOfNodes;		//storing list of nodes for each edge
	
				istream_iterator<std::string> it(edgeS);	//iteration to move to the connecting hyperedge
	
				istream_iterator<std::string> end;
	
				vector<string> result(it, end);	

				for (int i=0 ; i<result.size() ; i++)
				{
					if(result[i]=="")
						continue;
					listOfNodes.push_back(result[i]);
				}	//end for
				processAnInput(edgeNumber++,listOfNodes);
			}		//close while
			//cout<<calculateAllGain();
		} 	//close function hypergraph

		int assignSet()					//for random assignment
		{
			//if(num_Nodes>50)
			return (rand() % 2);
			if(prevAssignment==0)
			{
				prevAssignment = 1;
			}
			else 
			{
				prevAssignment = 0;
			}
			return prevAssignment; 		//its is new assignment now 	
		}		//close assignset

		void processAnInput(int edgeNumber, list<string> listOfNodes);	//create an edge . 
		Node* addNode(string nodeLabel, int edgeNumber);
		void dump();
		void dumpNodes();
		void  calculateAllGain();
		void calculateUpdateGain();
		void  FMAlgo();
		void unlockAll();
		void outputToFile();
		
		bool nodeExist(string n )
		{
			return (Nodes.count(n)>0);		// to get the nodes in the partition
		}		//close bool
		
		Node* getNodeByLabel(string n)
		{		
			if(Nodes.count(n)==0)
				cout<<n;
			return Nodes[n];
		}		//close getNodeByLabel

		Edge* getEdgeByLabel(int n )
		{
			if(Edges.count(n)==0)
				cout << n;
			return Edges[n];
		}		//close getEdgeByLabel

		float balance()					//balance condition
		{		
			return float(count[0])/(count[0]+count[1]);
		}		//close balance
};	//close class hyperedge


class Edge
{
	private : 
		//list<Node* > listOfNodes_ptr;// a cache of pointer that may be needed . 
		list<string>    listOfNodes;
		int label; 
		hyperGraph * hg;
	//		int zeroCount;
	//	int oneCount;
	public:
		int critical;//0 no 1 yes 
		int cutSet ; //0 no 1 yes. 
		int setCount[2];
		//since we are going edge by edge this is where we create an edge in one shot 
		//
		
		Edge()
		{
		}
		
		Edge(int l , list<string> listOfNodes_in,hyperGraph * h)
		{
			list<string>:: iterator it = listOfNodes_in.begin();
			for( ; it!=listOfNodes_in.end() ; it++){
				listOfNodes.push_back(*it);
			}
			//listOfNodes = listOfNodes_in;
			label = l; 
			//cout <<"ADDEd edge"<<l;
			//dumpEdge();
			//cout <<endl;
			hg = h;
			countCut();
		}	//close edge

		void countCut ();
		void updateCutCount(int set);
		void dumpEdge();

};		//close class edge

class Node 
{
	private: 
	//	list <Edge*> ListOfEdges_ptr;
		list<int> listOfEdges;
		string label; 
		int set;//0 or 1 
		hyperGraph * hg;
		bool locked;
	
	public :
		int gain;
		int tempGain;
		Node(string l,hyperGraph * h )
		{
			label = l ;	
			hg=h;
			//set=rand() % 2;
			set=hg->assignSet();
			locked = false;
		}	//close function

		void addEdge(int edgeLabel)
		{	
			listOfEdges.push_back(edgeLabel);
		}	//close function addedge
		
		int getSetLabel(){ return set;};
		int calculateGain();
		int calculateTempGain();
		string getLabel(){return label;}
		bool isLocked(){return locked;};
		void setLocked(){ locked=true;};
		void unLock(){locked=false;}
		void updateEdges();
		
		void setFlip()
		{
			if(set==0)
			{
				set = 1;	
				hg->count[0]--;
				hg->count[1]++;
			}
			else
			{ 
				set = 0;
				hg->count[1]--;
				hg->count[0]++;	
			}
		}	//close function setflip

		string dump()
		{
			string s = "";
			ostringstream nameStream;
			nameStream << "Node: " << label << "\tSet Assignment: " << set << "\tLast Gain: " << gain;
			return nameStream.str();
		}	//close dump
};		//close class node

void hyperGraph::processAnInput(int edgeNumber, list<string> listOfNodes)
{	//create an edge . 
	if(edgeNumber>num_Edges)//ERROR
	{
		exit(0);
		cout << edgeNumber;
	}
	list <string>::iterator it = listOfNodes.begin();
	for( ; it != listOfNodes.end() ; it++)
	{	
		addNode(*it,edgeNumber);
	}
	Edges[edgeNumber]= new Edge(edgeNumber,listOfNodes,this);
}	//CLOSE hypergraph::processinput

Node* hyperGraph::addNode(string nodeLabel,int edgeNumber)
{
	/*if(nodeLabel>num_Nodes)//ERROR
		exit(0);*/
	if(nodeExist(nodeLabel))
	{
		Node*  node = getNodeByLabel(nodeLabel);
		node->addEdge(edgeNumber);
		return node;
	}
	else
	{
		Node* node = new  Node(nodeLabel,this);
		node->addEdge(edgeNumber);
		Nodes[nodeLabel] = node;
		count[node->getSetLabel()]++;
		return node;
	}
}	//close hypergraph::addnode

void hyperGraph::dump()
{
	cout << num_Edges << " " << num_Nodes << endl;
	for( int i=1 ; i<=num_Edges ; i++)
	{
		Edge* edge = getEdgeByLabel(i);
		edge->dumpEdge();
		cout << endl;	
		fflush(stdout);
	}	//close for
}	//close hypergraph::dump

void hyperGraph::dumpNodes()
{
	map<string,Node*>::iterator it = Nodes.begin();
	ofstream myfile;
	myfile.open ((fName+".output").c_str());
	myfile << "Number of Nodes: " << num_Nodes << endl;
	for( ; it!=Nodes.end() ; it++)
	{
	//for( int i =1 ; i<= num_Nodes;i++){
		Node* node = it->second;
		myfile << node->dump();
		myfile << endl;	
	}

	myfile.close();
}	//close hypergraph::dumpnotes

void Edge::dumpEdge()
{
	list<string>::iterator it = listOfNodes.begin();
	for( ; it!=listOfNodes.end() ; it++)
	{
		Node * node = hg->getNodeByLabel(*it);
		//cout<<(node->dump());
		cout << *it << " ";
	}
	ostringstream nameStream;
	nameStream << "----" << cutSet << "&&" << critical;
	cout<<nameStream.str();
}	//close edge::dumpedge

void Edge::countCut ()
{	
	list<string>::iterator it = listOfNodes.begin();
	setCount[0] = 0;
	setCount[1] = 0;
	for(;it!=listOfNodes.end();it++)
	{
		Node * node = hg->getNodeByLabel(*it);
		if(node->getSetLabel()<=1)
			setCount[node->getSetLabel()]++;
	}
	if(setCount[0]>0 && setCount[1]>0)
	{	
		cutSet = 1;//is a cut set 
		hg->numOfCutEdges++;
	}
	else 
		cutSet = 0;

	if(setCount[0]==1||setCount[1]==1||cutSet==0)//either exactly one node accross or none. so changing this mighht update the cut set. 
		critical = 1;
	else 
		critical = 0;
}	//close functionedge::countcut

void Edge::updateCutCount(int set)
{
	setCount[set] += 1;
	int otherSet = 1;
	if(set)otherSet = 0;
	setCount[otherSet] -= 1;
	int prevCut = cutSet;
	if(cutSet)
		hg->numOfCutEdges--;
	if(setCount[0]>0 && setCount[1]>0)
	{	
		cutSet = 1;//is a cut set 
		hg->numOfCutEdges++;
	}
	else 
		cutSet = 0;
	if(setCount[0]==1||setCount[1]==1||cutSet==0)//either exactly one node accross or none . so changing this mighht update the cut set . 
		critical = 1;
	else 
		critical = 0;
	
	list<string>::iterator it = listOfNodes.begin();
	for(;it!=listOfNodes.end();it++)
	{
		hg->gainUpdateList.push_back(*it);//affected cells 
	}
	//hg->numOfCutEdges+= (cutSet-prevCut); //1 -0 cut set increased . 
}

void Node::updateEdges()
{
	list<int>::iterator it = listOfEdges.begin();
	gain=0;
	hg->gainUpdateList.clear();//new updateList
	for( ; it!=listOfEdges.end() ; it++)
	{
		Edge* edge= hg->getEdgeByLabel(*it);
		edge->updateCutCount(set);//label is node number;
	}
}
int Node::calculateGain()
{
	list<int>::iterator it = listOfEdges.begin();
	gain=0;
	for( ; it!=listOfEdges.end() ; it++)
	{
		Edge* edge= hg->getEdgeByLabel(*it);
		if(!(edge->critical))
			continue;
		if(edge->setCount[set]==1)//F(x)
			gain += 1;
		int otherset = 0;
		if(set==1)
			otherset = 0;
		else 
			otherset = 1;
		if(edge->setCount[otherset]==0)//T(X)
			gain -= 1;
	}
	tempGain=gain;
	return gain;		
}	//close node::calculator

int Node::calculateTempGain()
{
	list<int>::iterator it = listOfEdges.begin();
	//gain=0;
	tempGain = 0;
	for(;it!=listOfEdges.end();it++)
	{
		Edge* edge = hg->getEdgeByLabel(*it);
		if(!(edge->critical))
			continue;
		if(edge->setCount[set]==1)//F(x)
			tempGain += 1;
		int otherset = 0;
		if(set==1)
			otherset = 0;
		else 
			otherset = 1;
		if(edge->setCount[otherset]==0)//T(X)
			tempGain -=1 ;
	}
	return tempGain;
}		//close node::calculator

void hyperGraph::calculateAllGain(){
	map<string,Node*>::iterator it = Nodes.begin();
	//map<int,list<string>> maxList;//map
	maxLists.clear();
	int max = 0;
	int validMove = -1;//0,1 -1 all
	float bal1 = float(count[0]+1)/(count[0]+count[1]);
	float bal2 = float(count[0]-1)/(count[0]+count[1]);
	//cout<<bal1<<"  "<<bal2;
	if(bal1<r+0.056 && bal2>r-0.050)
		validMove = -1;
	else if(bal1 < r+0.056)
		validMove = 1;// from 1 
	else if(bal2 > r-0.05)
		validMove = 0;//from 0;
	else
		return;
	//cout<<"validmove"<<validMove<<endl;
	//for ( int i=1 ; i<num_Nodes;i++){
	for( ; it!=Nodes.end() ; it++)
	{
		string i = it->first;
		//if(!nodeExist(i))
		//	continue;
		Node* n = it->second; //getNodeByLabel(i);
		int g = n->calculateGain();
		
		if(n->isLocked())
			continue;
		if(n->getSetLabel()!=validMove && validMove!=-1)//balance factor destroys
			continue;
		//cout << "being happy "<<n->getSetLabel()<<endl;
		if(maxLists.count(g)==0)
		{
			list<string>* x = new list<string>;
			x->push_back(i);
			maxLists[g] = x;
		}
		else
		{
			maxLists[g]->push_back(i);
		}
	/*	if(g>max){
			maxList.clear();
			max= g;
			maxList.push_back(i);
		}else if(g==max){
			maxList.push_back(i);
		}*/
	}
}	//close hypergraph::calculateAllgain

void hyperGraph::unlockAll()
{
	map<string,Node*>::iterator it = Nodes.begin();
	for(; it!=Nodes.end() ; it++)
	{
		it->second->unLock();
	}
}	//close hypergraph::unlockall

void hyperGraph::calculateUpdateGain()
{//from updateList
	list<string>::iterator it = gainUpdateList.begin();
	int validMove= -1;//0,1 -1 all
	float bal1= float(count[0]+1)/(count[0]+count[1]);
	float bal2= float(count[0]-1)/(count[0]+count[1]);
	//cout<<bal1<<"  "<<bal2;
	if(bal1<r+0.056 && bal2>r-0.050)
		validMove = -1;
	else if(bal1<r+0.056)
		validMove = 1;// from 1 
	else if(bal2>r-0.05)
		validMove = 0;//from 0;
	else 
		return;
	for (; it != gainUpdateList.end() ; it++)
	{	
		Node* node = getNodeByLabel(*it);
		int gt = node->tempGain;//now remove from the maxList
		if(node->isLocked())continue;
		if(node->getSetLabel()!=validMove && validMove!=-1)//balance factor destroys
			continue;
		int g = node->calculateTempGain();
		if(maxLists.count(gt)==0)
		{
			cout << "\nsomething wrong";
			continue;
		}
		else
		{
			list<string> * a = maxLists[gt];
			a->remove(*it); //remove this node as it will be re enterred with a new Gain l;
		}
		
		if(maxLists.count(g)==0)
		{
			list<string>* x = new list<string>;
			x->push_back(*it);
			maxLists[g] = x;
		}
		else
		{
			maxLists[g]->push_back(*it);
		}
	}	//close for
}	//close hypergraph::calculateupdategain

 void hyperGraph::FMAlgo()
 {	
	int i = 0;
	vector<int> numCutsInPass; //to store number of cuts for breaking . 
	int prevNumOfCutEdges = 0;
	int convergenceCount = 0;
	do{ //this loop when all cells are freee begin pass 
		 //cout <<"pass "<<i <<"<<<<<<<<<<<<<<<<<<<<"<<endl;
		 vector<string> moveList;
		 //Loop will start here 
		 calculateAllGain();
		if(maxLists.empty())
			break;//nothing much happend;
		 //moving baseCell
		 list<string>* baseCell = (maxLists.rbegin())->second;//max gain item 
	
		 do{
			 Node * node = getNodeByLabel(baseCell->front());//arbitrarily getting the first improve this later. remove from sorted list as it is locked now
			 baseCell->pop_front();
			 node->setFlip();
			 node->updateEdges();
			 node->setLocked();
			 moveList.push_back(node->getLabel());//=node->gain;
			 calculateUpdateGain();
			 baseCell = (maxLists.rbegin())->second;//max gain item ;
		} while(!baseCell->empty());

		 vector<int> seqGain;
		 int maxTotalGain = -num_Nodes;
		 int totalGain = 0;
		 int maxK = 0;
		 for ( int k = 0 ; k<moveList.size() ; k++)
		 {
			 //cout<<moveList[k]<<"  "<<getNodeByLabel(moveList[k])->tempGain<<endl;
			 totalGain += getNodeByLabel(moveList[k])->tempGain;
			 if(totalGain>maxTotalGain)
			 {
				 maxTotalGain = totalGain;
				 maxK = k;	
			 }
		 }
		 //tofinalize gain i will change gain = temp gain for index <= maxk and for index >maxK i will flip to nullify the temp move and we are done 
		 for ( int index=0 ; index <moveList.size();index++){
			 Node * node = getNodeByLabel(moveList[index]);
			 if(index<=maxK)
			 {
				 node->gain = node->tempGain;
			 }
			 else 
			 {
				 node->setFlip();//flip as we are not moving this . 
			 }
		 }
		 // we have made one pass and now i will unlock all nodes 
		 //cout <<"max k is "<<maxK<<"max total gain is "<<maxTotalGain<<endl;
		 //cout <<"balance " <<balance()<<" number og cut " <<numOfCutEdges<<endl;
		if(prevNumOfCutEdges==numOfCutEdges)
		{
			convergenceCount++;
		}
		else
		{
			convergenceCount = 0;
		}	
		if(convergenceCount>10)
		{
			cout << "\nCoverged FM is fully tuned!" << endl; 
			cout << "\nMax k:\t" << maxK << "\tmax total gain:\t" << maxTotalGain << endl;
			cout <<"Balance:\t" << balance() << "\tNumber of cut:\t " << numOfCutEdges << endl;
			break;//algorithm has converged . 
		}
		unlockAll();
		i++;
		
		prevNumOfCutEdges = numOfCutEdges;
	  //The Second break condition is if totall gain has not changed for some pass and numberof Cut Edges have reached . 

	} while(!maxLists.empty());//the whole heuristic loop runs till calculateAllGain keeps returning some base cells in max list which is a sorted list as per gains .
	cout << "Pass: "<< i <<endl;
 }	//close hypergraph::FMAlgo

void hyperGraph::outputToFile()
{
	//ofstream myfile;
	//myfile.open ((fName+".output").c_str());
	cout << "**** Input Parameters ****\n" << endl;
	cout <<"The Number of Nodes:\t"<< num_Nodes << "\nThe number of Edges:\t" << num_Edges << endl;
	cout << "***** My Results *****" << endl;
	cout << "Number of Minimum Cutset:\t" << numOfCutEdges << endl;
	cout << "The number of element in each cut:" << endl;
	cout << "Set 0:\t" << count[0] << "\tSet1:\t" << count[1] << endl;
	cout << "Balance:\t" << balance() << endl;
	cout << "******************************************" << endl;
	//myfile.close();
}		//close function hypergraph output file
