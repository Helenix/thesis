#include "../headers/utilityHelper.h"

int main(int argc, char *argv[]) {
    FILE *fp = NULL;
    char buffer[32];
    vector<Point> gns {};
    vector<Point> stack {};
    vector<Point> convexHull {};    

    gns = readPointsFromFile("../input_files/groundNodes120.txt");
    convexHull = findConvexHull(gns);
    writePointsToFile("../output_files/convexHull.txt", convexHull);

    printPoints(convexHull);
    
    for(unsigned int i = 0; i < convexHull.size(); i++){
        int r_height = randomHeight();
        printf("%d\n", r_height);
        
        if(isInsideConvexHull(convexHull, convexHull[i])) 
            printf("Point is inside convex hull\n");
    } 

    return 0;
}