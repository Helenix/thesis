#include "../headers/utilityHelper.h"
#include "../headers/scenarioHelper.h"

enum mode {
    SERVING,
    FLYING,
    BRIDGING
};

int main(int argc, char *argv[]) {
    FILE *fp = NULL;
    char path[64];
    vector<Point> gns {};
    vector<Point> stack {};
    vector<Point> convexHull {};    
    scenarioHelper scenario;

    /* scenario.setAreaHeight(5000);
    scenario.setAreaWidth(5000);
    scenario.setNumOfNodes(90);
    scenario.setOffset(1);
    scenario.generateGroundNodesFile();
    scenario.deployGroundNodes();
    scenario.generateGroundNodesDemandFile("../input_files/nodeDemand.txt");
    scenario.setGroundNodesDemand("../input_files/nodeDemand.txt"); */

    /* int n = sprintf(path, "../input_files/groundNodes%d.txt", scenario.getNumOfNodes());
    if(n < 0) {
        printf("Error generating path file");
        exit(-1);
    } */


    /* gns = readPointsFromFile(path);
    convexHull = findConvexHull(gns);
    writePointsToFile("../output_files/convexHull.txt", convexHull);
    printPoints(convexHull);

    double max_range = getMaxRange(MIN_RX_SENSITIVITY, getLambda(F), PATH_LOSS_EXPOENT);
    printf("Max UAV Range: %f\n", max_range); */

    /* for(unsigned int i = 0; i < convexHull.size(); i++){
        int r_height = randomHeight();
        printf("%d\n", r_height);
        
        if(isInsideConvexHull(convexHull, convexHull[i])) 
            printf("Point is inside convex hull\n");
    } */

    //gridInsideConvexHull(4, 4, 3, "gridFile.txt");

    return 0;
}