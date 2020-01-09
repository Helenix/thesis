#include "../headers/scenarioHelper.h"

scenarioHelper::scenarioHelper(): areaWidth(0), areaHeight(0), numOfNodes(0), offset(1) {};

scenarioHelper::scenarioHelper(const char *fileName): areaWidth(0), areaHeight(0), numOfNodes(0), offset(1) {
    groundNodesList = readPointsFromFile(fileName);
};

void scenarioHelper::setNumOfNodes(int numOfNodes) {
    this->numOfNodes = numOfNodes;
};

int scenarioHelper::getNumOfNodes() {
    return this->numOfNodes;
};

void scenarioHelper::setAreaWidth(int areaWidth) {
    this->areaWidth = areaWidth;
};

int scenarioHelper::getAreaWidth() {
    return this->areaWidth;
};

void scenarioHelper::setAreaHeight(int areaHeight) {
    this->areaHeight = areaHeight;
};

int scenarioHelper::getAreaHeight() {
    return this->areaHeight;
};

void scenarioHelper::setOffset(int offset) {
    this->offset = offset;
};

int scenarioHelper::getOffset() {
    return this->offset;
};

void scenarioHelper::deployGroundNodesFile() {
    FILE *fp = NULL;
    char fileName[32];
    int numOfNodes = getNumOfNodes();
    int offset = getOffset();
    vector<Point> tmpList {};

    srand(time(NULL));

    int n = sprintf(fileName, "groundNodes%d.txt", numOfNodes);
    if(n < 0) {
        printf("Error generating the ground node file\n");
        exit(-1);
    }
    
    tmpList.push_back(Point(0, 0, 0));

    for(int i = 0; i < numOfNodes - 1; i++) {
        bool isNewPoint = false;
        int counter = 0;

        while(!isNewPoint) {
            counter++;
            if(counter > 1) {
                printf("colision\n");
            }
            int x = rand() % (getAreaWidth() + 1);
            int y = rand() % (getAreaWidth() + 1);
            int z = 0;

            Point point(x, y , z);

            if(!findPoint(tmpList, point)) {
                tmpList.push_back(point);
                isNewPoint = true;
            }
        } 
    };

    writePointsToFile(fileName, tmpList);
    printf("%lu\n", tmpList.size());
    tmpList.clear();
};