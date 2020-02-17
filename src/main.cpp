#include "../headers/utilityHelper.h"
#include "../headers/scenarioHelper.h"
#include "../headers/algorithmHelper.h"

int main(int argc, char *argv[]) {
    int x = 5000;
    int y = 5000;
    double max_range = getMaxRange(MIN_RX_SENSITIVITY, getLambda(F), PATH_LOSS_EXPOENT);
    printf("Max UAV Range: %f\n", max_range);
    int offset = int(max_range) * 0.3;
    const char *convexHullfile = "../input_files/convexHull.txt";
    const char *gridFile = "../input_files/gridFile.txt";
    const char *gpointsFile = "../input_files/groundNodes100.txt";
    const char *demandFile = "../input_files/nodeDemand.txt";

    AlgorithmHelper algHelper(x, y, offset, convexHullfile, gridFile, gpointsFile, demandFile);
    algHelper.initializelPopulation();
    

    printf("\n\n\n DISSATISFACTIONS\n");
    int counter = 0;
    for(auto i: algHelper.population) {
        printf("Individual %d Max dissatisfaction %f\n", counter, i.maxDissatisfaction);
        counter++;
    } 

    /* FILE *fp = NULL;
    char path[64];
    vector<Point> gns {};
    vector<Point> convexHull {};    
    ScenarioHelper scenario("../input_files/groundNodes100.txt");

    // scenario.setAreaHeight(5000);
    // scenario.setAreaWidth(5000);
    scenario.setNumOfNodes(100);
    // scenario.generateGroundNodesFile(); 
    // scenario.deployGroundNodes();
    // scenario.generateGroundNodesDemandFile("../input_files/nodeDemand.txt");
    // scenario.setGroundNodesDemand("../input_files/nodeDemand.txt");

    int n = sprintf(path, "../input_files/groundNodes%d.txt", scenario.getNumOfNodes());
    if(n < 0) {
        printf("Error generating path file");
        exit(-1);
    }
    gns = readPointsFromFile(path);

    convexHull = findConvexHull(scenario.groundPointsList);
    writePointsToFile("../input_files/convexHull.txt", convexHull);

    gridInsideConvexHull(5000, 5000, offset, "../input_files/convexHull.txt"); */

    return 0;
}