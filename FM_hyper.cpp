#include "hyperGraph.h"
#include <stdio.h>
using namespace std;


int main(int argc, char*argv[])
{	
	clock_t tStart = clock();
	//hyperGraph hg("s13207P.hgr");
	if(argc!=2)
	{
		cout << "\n**** Not a Proper Format ! *****\n";
		return 0;
	}
	
	hyperGraph hg(argv[1]);
	
	cout << endl << "Initial Balance:\t" << hg.balance() << "\nNumber of HyperEdges:\t" << hg.numOfCutEdges << endl;
	hg.FMAlgo();
	hg.outputToFile();
	hg.dumpNodes();
	
	//hg.dump();
    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

	return 0;
}
