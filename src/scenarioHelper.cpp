#include "../headers/scenarioHelper.h"

ScenarioHelper::ScenarioHelper(): areaWidth(0), areaHeight(0), numOfNodes(0), offset(1) {}

ScenarioHelper::ScenarioHelper(const char *fileName): areaWidth(0), areaHeight(0), numOfNodes(0), offset(1) {
    groundPointsList = readPointsFromFile(fileName);
}

int ScenarioHelper::nodeIdCounter = -1;

void ScenarioHelper::setNumOfNodes(int numOfNodes) {
    this->numOfNodes = numOfNodes;
}

int ScenarioHelper::getNumOfNodes() {
    return this->numOfNodes;
}

void ScenarioHelper::setAreaWidth(int areaWidth) {
    this->areaWidth = areaWidth;
}

int ScenarioHelper::getAreaWidth() {
    return this->areaWidth;
}

void ScenarioHelper::setAreaHeight(int areaHeight) {
    this->areaHeight = areaHeight;
}

int ScenarioHelper::getAreaHeight() {
    return this->areaHeight;
}

void ScenarioHelper::setOffset(int offset) {
    this->offset = offset;
}

int ScenarioHelper::getOffset() {
    return this->offset;
}

void ScenarioHelper::generateGroundNodesFile() {
    FILE *fp = NULL;
    char fileName[32];
    int numOfNodes = getNumOfNodes();
    int offset = getOffset();

    //srand(time(NULL));

    int n = sprintf(fileName, "groundNodes%d.txt", numOfNodes);
    if(n < 0) {
        printf("generateGroundNodesFile: Error generating the ground node file\n");
        exit(-1);
    }

    groundPointsList.push_back(Point(0, 0, 0));

    for(int i = 0; i < numOfNodes - 1; i++) {
        bool isNewPoint = false;

        while(!isNewPoint) {
            int x = rand() % (getAreaWidth() + 1);
            int y = rand() % (getAreaWidth() + 1);
            int z = 0;

            Point point(x, y , z);

            if(!findPoint(groundPointsList, point)) {
                groundPointsList.push_back(point);
                isNewPoint = true;
            }
        } 
    }

    writePointsToFile(fileName, groundPointsList);
    printf("%lu\n", groundPointsList.size());
}

void ScenarioHelper::deployGroundNodes()  {
    for(auto i: groundPointsList) {
        nodeIdCounter++;
        GroundNode *new_node = new GroundNode(nodeIdCounter, i.x, i.y, i.z);
        groundNodesList.push_back(new_node);
    }
}

void ScenarioHelper::supplyUavNodes()  {
    for(auto i: gridInsideConvex) {
        nodeIdCounter++;
        FlyingNode *new_node = new FlyingNode(nodeIdCounter, i.x, i.y, i.z);
        flyingNodesList.push_back(new_node);
    }
}

void ScenarioHelper::generateGroundNodesDemandFile(const char *name) {
    FILE *fp;
    vector<int> demandList {};

    // srand(time(NULL));

    demandList.push_back(6);
    demandList.push_back(9);
    demandList.push_back(12);
    demandList.push_back(18);
    demandList.push_back(24);
    demandList.push_back(36);
    demandList.push_back(48);
    demandList.push_back(54);

    printf("%lu\n", groundNodesList.size());

    fp = fopen(name, "w");
    if(fp) {
        for(unsigned int i = 0; i < groundNodesList.size(); i++) {
            int index = rand() % demandList.size();
            fprintf(fp, "%d %d\n", groundNodesList[i]->id, demandList[index]);
        }
        
    }
    else {
        printf("Cannot create the file!\n");
        exit(-1);   
    }

    fclose(fp);

}

void ScenarioHelper::setGroundNodesDemand(const char *name) {
    FILE *fp;
    char buffer[32];

    fp = fopen(name, "r");

    if(fp) {
        cout << "setGroundNodesDemand: File opened with success!\n";
        while(fgets(buffer, sizeof(buffer), fp)) {
            int id;
            int dataRate;

            sscanf(buffer, "%d %d", &id, &dataRate);
            GroundNode *node = getGroundNodeByID(id);

            if(node != NULL) {
                node->dataRate = dataRate;
            }
            else {
                printf("setGroundNodesDemand: That ID does not exist!");
                exit(-1);
            }
        }
    }
    else {
        cout << "Cannot open the file!\n";
        exit(-1); 
    }

    fclose(fp);
}

void ScenarioHelper::initializeNetworkArea(int xMax, int yMax, int offset, const char *convexHullFile, const char *gridFile) {
    setAreaWidth(xMax);
    setAreaHeight(yMax);
    setOffset(offset);
    setNumOfNodes(groundPointsList.size());
    deployGroundNodes();
    convexHull = readPointsFromFile(convexHullFile);
    gridInsideConvex = readPointsFromFile(gridFile);
    if(allPointsHaveNeighbor(gridInsideConvex)) {
        supplyUavNodes();
    }
}

bool ScenarioHelper::allPointsHaveNeighbor(vector<Point> &gridList) {
    double maxRadius = getMaxRange(MIN_RX_SENSITIVITY, getLambda(F), PATH_LOSS_EXPOENT);

    for(int i = 0; i < gridList.size(); i++) {
        int counter = 0;
        for(int j = 0; j < gridList.size(); j++) {
            if(i != j) {
                if(dist3D(gridList[i], gridList[j]) <= maxRadius)
                    counter++;
            }
        }
        if(counter <= 0) {
            printf("scenarioHelper::allPointsHaveNeighbor Some points doesnt have neighbour");
            exit(-1);
        }
    }
    return true;
}

GroundNode *ScenarioHelper::getGroundNodeByID(int id) {
    for(auto i: groundNodesList) {
        if(i->id == id)
            return i;
    }
    return NULL;
}

FlyingNode *ScenarioHelper::getUavNodeById(int id) {
    for(auto i: flyingNodesList) {
        if(i->id == id)
            return i;
    }
    return NULL;
}


int ScenarioHelper::findGroundNodeWithNoParent() {
    int id = -1;
    vector<int> solutionList {};

    for(auto i: groundNodesList) {
        if(!(i->isConnected)) {
            solutionList.push_back(i->id);
        }
    }

    if(!solutionList.empty()) {
        srand(time(NULL));
        int randomIndex = rand() % solutionList.size();
        id = solutionList[randomIndex];
    }

    return id;
}

bool ScenarioHelper::allGNodesConnected() {
    for(auto i: groundNodesList) {
        if(!(i->isConnected))
            return false;
    }
    return true;
}

void ScenarioHelper::deployUAV() {
    int counter = 0;

    while(!allGNodesConnected()) {
        int height = randomHeight();
        int gNodeID = findGroundNodeWithNoParent();

        if(gNodeID < 0) {
            printf("ScenarioHelper::deployUAV() all ground noded are assigned\n");
            return;
        }

        GroundNode *gn = getGroundNodeByID(gNodeID);

        if(gn) {
            Point *uavPoint = closestUavPos(gn->p);
            printf("Closest UAV: %d %d %d\n", uavPoint->x, uavPoint->y, uavPoint->z);

            if(uavPoint) {
                for(auto i: flyingNodesList) {
                    if(!i->flying) {
                        i->flying = true;
                        i->serving = true;
                        // Might be wrong the bridgind
                        i->bridging = false;
                        i->height = height;
                        uavPoint->busy = true;
                        uavPoint->z = height;
                        i->p = *uavPoint;
                        gn->isConnected = true;
                        gn->connectedTo = i->id;

                        for (auto j: groundNodesList) {
						    int newUavID = getBestSignal(j);
						    if(newUavID >= 0) {
							    FlyingNode *tmpUAV = getUavNodeById(newUavID);
							    tmpUAV->serving = true;
							    tmpUAV->bridging = false;
							    j->connectedTo = newUavID;
							    j->isConnected = true;
						    }
                        }
                        break;
                    }
                }
            } else {
                int uavID = getBestSignal(gn);
                if(uavID < 0) {
                    printf("ScenarioHelper::deployUAV() there's no UAV available for that node\n");
                    exit(-1);
                }
                FlyingNode *tmpUAV = getUavNodeById(uavID);
                tmpUAV->serving = true;
                tmpUAV->bridging = false;
                gn->connectedTo = uavID;
                gn->isConnected = true;
            }
        } else {
            printf("ScenarioHelper::deployUAV() there's no node with that ID\n");
            exit(-1);
        }

        if(counter >= 10000) {
            printf("ScenarioHelper::deployUAV() counter exceeded\n");
            break;
        }
        counter++;
    }

    printf("Counter: %d\n", counter);
}

Point *ScenarioHelper::closestUavPos(Point &p) {
    double distance = INFINITE_DISTANCE;
    Point *closestUav = NULL;
    
    for(int i = 0; i < gridInsideConvex.size(); i++) {
        if(!(gridInsideConvex[i].busy) && !equalPoints(p, gridInsideConvex[i])) {
            double currentDist = dist3D(p, gridInsideConvex[i]);
            if(currentDist < distance) {
                closestUav = &gridInsideConvex[i];
                distance = currentDist;
            }
        }
    }

    return closestUav;
}

int ScenarioHelper::getBestSignal(GroundNode *gn) {
    int bestSignal = -1;
    double bestRange = INFINITE_DISTANCE;

    for(auto i: flyingNodesList) {
        if(i->flying) {
            double range = dist3D(gn->p, i->p);

            if(range <= bestRange && range <= getMaxRange(MIN_RX_SENSITIVITY, getLambda(F), PATH_LOSS_EXPOENT)) {
                bestRange = range;
                bestSignal = i->id;
            }
        }
    }
    return bestSignal;
}
