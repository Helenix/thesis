#include "../headers/utilityHelper.h"
#include "../headers/scenarioHelper.h"

enum mode {
    SERVING,
    FLYING,
    BRIDGING
};

int main(int argc, char *argv[]) {
    FILE *fp = NULL;
    char buffer[32];
    vector<Point> gns {};
    vector<Point> stack {};
    vector<Point> convexHull {};    
    scenarioHelper scenario;

    scenario.setAreaHeight(5000);
    scenario.setAreaWidth(5000);
    scenario.setNumOfNodes(90);
    scenario.setOffset(1);

    scenario.deployGroundNodesFile();


    /* gns = readPointsFromFile("../input_files/groundNodes120.txt");
    convexHull = findConvexHull(gns);
    writePointsToFile("../output_files/convexHull.txt", convexHull);
    printPoints(convexHull); */

    // double max_range = getMaxRange(MIN_RX_SENSITIVITY, (double)C/F, PATH_LOSS_EXPOENT);
    // printf("\n\n%f\n\n", max_range);

    /* for(unsigned int i = 0; i < convexHull.size(); i++){
        int r_height = randomHeight();
        printf("%d\n", r_height);
        
        if(isInsideConvexHull(convexHull, convexHull[i])) 
            printf("Point is inside convex hull\n");
    } */

    return 0;
}