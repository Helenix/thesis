#ifndef SCENARIOHELPER_H_
#define SCENARIOHELPER_H_

#include "utilityHelper.h"

using namespace std;

class ScenarioHelper {
    public:
        ScenarioHelper();
        ScenarioHelper(const char *fileName);
        // ~ScenarioHelper();

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
        void supplyUavNodes();
        void setGroundNodesDemand(const char *name);
        void initializeNetworkArea(int xMax, int yMax, int offset, const char *convexHullFile, const char *gridFile);
        bool allPointsHaveNeighbor(vector<Point> &gridList);
        
        GroundNode *getGroundNodeByID(int id);
        FlyingNode *getUavNodeById(int id);
        FlyingNode *getAvailableUav();
        int findGroundNodeWithNoParent();
        bool allGNodesConnected();
        void deployUAV();
        void deployUAVrandomly();
        Point *closestUavPos(Point &p);
        int getBestSignal(GroundNode *gn);
        void removeUnusedUavs();
        Point *pointerToPoint(Point &p);
        Point resetPoint();
        void updateAdjacencyMatrix();
        void printAdjacencyMatrix();       
        bool isNodeReachable(int start, int end);
        void setUavPath(int destination);

        vector<Point> groundPointsList {};
        vector<Point> convexHull {};
	    vector<Point> gridInsideConvex {};
        vector<GroundNode *> groundNodesList {};
        vector<FlyingNode *> flyingNodesList {};
        map<int, double> maxFreeDistTableIEEE80211g;
        map<int, vector<int>> adjacencyMatrix;



    protected:
        int areaWidth;
        int areaHeight;
        int offset;
        int numOfNodes;
        static int nodeIdCounter;
        
};

#endif

