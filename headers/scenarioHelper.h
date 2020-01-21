#ifndef SCENARIOHELPER_H_
#define SCENARIOHELPER_H_
#include "utilityHelper.h"
using namespace std;

class scenarioHelper {
    public:
        scenarioHelper();
        scenarioHelper(const char *fileName);
        // ~scenarioHelper();

        void setNumOfNodes(int numOfNodes);
        int getNumOfNodes();
        void setAreaWidth(int areaWidth);
        int getAreaWidth();
        void setAreaHeight(int areaHeight);
        int getAreaHeight();
        void setOffset(int offset);
        int getOffset();

        void generateGroundNodesFile();
        void generateGroundNodesDemandFile(const char *name);
        void deployGroundNodes();
        void setGroundNodesDemand(const char *name);
        
        GroundNode *getGroundNodeByID(int id);

        vector<Point> groundPointsList {};
        vector<GroundNode *> groundNodesList {};
        vector<FlyingNode *> flyingNodesList {};

    protected:
        int areaWidth;
        int areaHeight;
        int offset;
        int numOfNodes;
        static int nodeIdCounter;
        
};

#endif

