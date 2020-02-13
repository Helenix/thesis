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

FlyingNode *ScenarioHelper::getAvailableUav() {
    for(int i = 0; i < flyingNodesList.size(); i++) {
        if(!flyingNodesList[i]->flying) {
            return flyingNodesList[i];
        }
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
                // TODO: getAvailableUAV() instead of the for
                for(auto i: flyingNodesList) {
                    if(!i->flying) {
                        i->flying = true;
                        i->serving = true;
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

        if(counter >= 100000) {
            printf("ScenarioHelper::deployUAV() counter exceeded\n");
            exit(-1);
        }
        counter++;
    }
    
    removeUnusedUavs();
    updateAdjacencyMatrix();
    setUavPath(0);
}

void ScenarioHelper::deployUAVrandomly() {
    int counter = 0;

    while(!allGNodesConnected()) {
        int height = randomHeight();
        int foundUav = false;
        int index;

        while(!foundUav) {
            index = rand() % gridInsideConvex.size();
            if(!gridInsideConvex[index].busy) {
                foundUav = true;
            }
        }

        for(auto i: flyingNodesList) {
            if(!i->flying) {
                i->flying = true;
                i->serving = true;
                i->height = height;
                gridInsideConvex[index].busy = true;
                gridInsideConvex[index].z = height;
                i->p = gridInsideConvex[index];

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

        if(counter >= 10000) {
            printf("ScenarioHelper::deployUAV() counter exceeded\n");
            exit(-1);
        }
        counter++;
    }

    removeUnusedUavs();
    updateAdjacencyMatrix();
    setUavPath(0);
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

void ScenarioHelper::removeUnusedUavs() {
    map<int, int> uavConnectionsCounter;

    for(auto i: flyingNodesList) {
        if(i->flying) {
            uavConnectionsCounter.insert(make_pair(i->id, 0));
        }
    }

    for(auto i: groundNodesList) {
        uavConnectionsCounter[i->connectedTo] += 1;
    }

    auto it = uavConnectionsCounter.begin();
    for(;it != uavConnectionsCounter.end(); it++) {
        if((*it).second <= 0) {
            FlyingNode *toRemove = getUavNodeById((*it).first);
            if(!toRemove->bridging) {
                toRemove->flying = false;
                toRemove->serving = false;
                toRemove->height = 0;
                toRemove->isConnected = false;
                toRemove->connectedTo = -1;
                Point *pointPtr = pointerToPoint(toRemove->p);
                if(!pointPtr) {
                    printf("ScenarioHelper::removeUnusedUavs() couldn't release point");
                    exit(-1);
                } else {
                    pointPtr->busy = false;
                }
                toRemove->p = resetPoint();
            }
        }
    }
}

Point *ScenarioHelper::pointerToPoint(Point &p) {
    Point *pointer = NULL;
    for(int i = 0; i < gridInsideConvex.size(); i++) {
        if(equalPoints(p, gridInsideConvex[i])) {
            pointer = &gridInsideConvex[i];
            break;
        }
    }

    return pointer;
}

Point ScenarioHelper::resetPoint() {
    return Point(-1, -1, -1);
}

void ScenarioHelper::updateAdjacencyMatrix() {
    /* for(auto i: flyingNodesList) {
        if(i->flying)
            printf("uav with height %d: %d %d %d\n", i->id, i->p.x, i->p.y, i->p.z);
    } */

    adjacencyMatrix = {};

    for(int i = 0; i < flyingNodesList.size(); i++) {
        if(flyingNodesList[i]->flying) {
            adjacencyMatrix.insert(make_pair(flyingNodesList[i]->id, vector<int> {}));
        }
    }

    double maxRadius = getMaxRange(MIN_RX_SENSITIVITY, getLambda(F), PATH_LOSS_EXPOENT);

    for(int i = 0; i < flyingNodesList.size(); i++) {
        for(int j = 0; j < flyingNodesList.size(); j++) {
            if(i != j) {
                if(flyingNodesList[i]->flying && flyingNodesList[j]->flying) {
                    double dist = dist3D(flyingNodesList[i]->p, flyingNodesList[j]->p);
                    if(dist <= maxRadius) {
                        adjacencyMatrix[flyingNodesList[i]->id].push_back(flyingNodesList[j]->id);
                    }
                }
            }
        }
    }
}

void  ScenarioHelper::printAdjacencyMatrix() {
    printf("\n\nAdjacencyMatrix: \n\n");

    auto adjIt = adjacencyMatrix.begin();

    for(; adjIt != adjacencyMatrix.end(); adjIt++) {
        printf("%d: ", (*adjIt).first);
        for(int i = 0; i < (*adjIt).second.size(); i++) {
            printf("%d ", (*adjIt).second[i]);
        }
        printf("\n");
    }
    printf("Adj size = %lu\n\n", adjacencyMatrix.size());
}

bool ScenarioHelper::isNodeReachable(int start, int destination) {
    list<int> queue {};
    map<int, bool> visited;

    if(start == destination) {
        return true;
    }

    for(int i = 0; i < flyingNodesList.size(); i++) {
        visited.insert(make_pair(flyingNodesList[i]->id, false));
    }

    visited[start] = true;
    queue.push_back(start);

    vector<int>::iterator it;

    printf("OLA %lu\n", adjacencyMatrix.size());

    while(!queue.empty()) {
        start = queue.front();
        queue.pop_front();

        for(it = adjacencyMatrix[start].begin(); it != adjacencyMatrix[start].end(); it++) {
            if((*it) == destination) {
                return true;
            }

            if(!visited[*it]) {
                visited[*it] = true;
                queue.push_back(*it);
            }
        }
    }
    return false;
}

void ScenarioHelper::setUavPath(int destination) {
    double maxRadius = getMaxRange(MIN_RX_SENSITIVITY, getLambda(F), PATH_LOSS_EXPOENT);

    int flyingNodeId;

    for(auto i: groundNodesList) {
        if(i->id == destination) {
            flyingNodeId = i->connectedTo;
            break;
        }
    }

    FlyingNode *flyingNode = getUavNodeById(flyingNodeId);
    printf("Master node %d\n", flyingNodeId);

    auto flyingNodeIt = flyingNodesList.begin();

    for(; flyingNodeIt != flyingNodesList.end(); flyingNodeIt++) {
        if((*flyingNodeIt)->flying && (*flyingNodeIt)->id != flyingNodeId) {
            Point *currentPoint = pointerToPoint((*flyingNodeIt)->p);
            while(!isNodeReachable((*flyingNodeIt)->id, flyingNodeId)) {
                printf("One node is not reachable: id %d (%d %d %d)\n", (*flyingNodeIt)->id, (*flyingNodeIt)->p.x, (*flyingNodeIt)->p.y, (*flyingNodeIt)->p.z);
                vector<Point> tempIds {};

                for(int i = 0; i < gridInsideConvex.size(); i++) {
                    if(!gridInsideConvex[i].busy) {
                        Point possiblePoint(gridInsideConvex[i].x, gridInsideConvex[i].y, randomHeight());
                        if(dist3D(possiblePoint, *currentPoint) <= maxRadius && !equalPoints(possiblePoint, *currentPoint)) {
                            tempIds.push_back(possiblePoint);
                        }
                    }
                }

                if(!tempIds.empty()) {
                    double bestDistance = INFINITE_DISTANCE;
                    Point bestPoint;

                    for(int i = 0; i < tempIds.size(); i++) {
                        double currentDistance = dist3D(tempIds[i], flyingNode->p);
                        if(currentDistance < bestDistance) {
                            bestDistance = currentDistance;
                            bestPoint = tempIds[i];
                        }
                    }
                    
                    currentPoint = pointerToPoint(bestPoint);
                    currentPoint->z = bestPoint.z;
                

                } else {
                    printf("ScenarioHelper::setUavPath: Uav has no neighbour");
                    exit(-1);
                }

                if(!currentPoint->busy) {
                    FlyingNode *availableUav = getAvailableUav();

                    if(availableUav) {
                        printf("New uav Id: %d, P = (%d, %d, %d)\n", availableUav->id, currentPoint->x, currentPoint->y, currentPoint->z);
                        availableUav->flying = true;
                        availableUav->bridging = true;
                        availableUav->height = (*currentPoint).z;
                        availableUav->p = *currentPoint;
                        currentPoint->busy;
                        updateAdjacencyMatrix();
                        
                    } else {
                        printf("ScenarioHelper::setUavPath: There is no available uav");
                        exit(-1);
                    }

                } else {
                    printf("ScenarioHelper::setUavPath: Error in uav candidate point");
                    exit(-1);
                }
            }
        }
    }
}