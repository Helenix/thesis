#include "../headers/scenarioHelper.h"

scenarioHelper::scenarioHelper(): areaWidth(0), areaHeight(0), numOfNodes(0), offset(1) {}

scenarioHelper::scenarioHelper(const char *fileName): areaWidth(0), areaHeight(0), numOfNodes(0), offset(1) {
    groundPointsList = readPointsFromFile(fileName);
}

int scenarioHelper::nodeIdCounter = -1;

void scenarioHelper::setNumOfNodes(int numOfNodes) {
    this->numOfNodes = numOfNodes;
}

int scenarioHelper::getNumOfNodes() {
    return this->numOfNodes;
}

void scenarioHelper::setAreaWidth(int areaWidth) {
    this->areaWidth = areaWidth;
}

int scenarioHelper::getAreaWidth() {
    return this->areaWidth;
}

void scenarioHelper::setAreaHeight(int areaHeight) {
    this->areaHeight = areaHeight;
}

int scenarioHelper::getAreaHeight() {
    return this->areaHeight;
}

void scenarioHelper::setOffset(int offset) {
    this->offset = offset;
}

int scenarioHelper::getOffset() {
    return this->offset;
}

void scenarioHelper::generateGroundNodesFile() {
    FILE *fp = NULL;
    char fileName[32];
    int numOfNodes = getNumOfNodes();
    int offset = getOffset();

    srand(time(NULL));

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

void scenarioHelper::deployGroundNodes()  {
    for(auto i: groundPointsList) {
        nodeIdCounter++;
        GroundNode *new_node = new GroundNode(nodeIdCounter, i.x, i.y, i.z);
        groundNodesList.push_back(new_node);
    }
}

void scenarioHelper::generateGroundNodesDemandFile(const char *name) {
    FILE *fp;
    vector<int> demandList {};

    srand(time(NULL));

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

void scenarioHelper::setGroundNodesDemand(const char *name) {
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

GroundNode *scenarioHelper::getGroundNodeByID(int id) {
    for(auto i: groundNodesList) {
        if(i->id == id)
            return i;
    }
    return NULL;
}

