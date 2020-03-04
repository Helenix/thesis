#include "../headers/utilityHelper.h"

// Convex Hull utility functions
Point P0;

int swap(Point &pA, Point &pB) {
    Point tmp = pA;
    pA = pB;
    pB = tmp;
}

Point nextToTop(stack<Point> &S) 
{ 
    Point p = S.top(); 
    S.pop(); 
    Point res = S.top(); 
    S.push(p); 
    return res; 
} 

double distance(Point pA, Point pB) {
    return pow(pA.x - pB.x, 2) + pow(pA.y - pB.y, 2);
}

double orientation(Point pA, Point pB, Point pC) {
    // 0 Colinear
    // 1 Clock wise ( > 0)
    // 2 Counter clock wise ( < 0)
     int val = (pB.y - pA.y) * (pC.x - pA.x) - (pB.x - pA.x) * (pC.y - pA.y);
    
    if(val == 0) return 0;

    return (val > 0) ? 1:2;
}

int compare(const void *vpA, const void *vpB) {
    Point *pA = (Point *) vpA; 
    Point *pB = (Point *) vpB;
    
    int o = orientation(P0, *pA, *pB);

    if(o == 0) {
        return (distance(P0, *pB) >= distance(P0, *pA)) ?  -1 : 1;
    } 

    return (o == 2) ? -1 : 1;
}

// Graham Scan Algorithm
vector<Point> findConvexHull(vector<Point> &points) {
    vector<Point> convexHull {};

    int ymin = points[0].y;
    int min = 0;

    for(unsigned int i = 0; i < points.size(); i++) {
        int y = points[i].y;
        if(y < ymin || (y == ymin && points[i].x < points[min].x)) {
            ymin = points[i].y;
        }
    }

    swap(points[0], points[min]); 

    P0 = points[0];

    // 2nd step:
    qsort(&points[1], points.size()-1, sizeof(Point), compare);

    int m = 1;
    for(unsigned int i = 0; i < points.size(); i++) {
        while (i < points.size()-1 && orientation(P0, points[i], points[i+1]) == 0) 
            i++; 
    
        points[m] = points[i]; 
        m++; 
    }

    if(m < 3) return convexHull;

    stack<Point> S;
    S.push(points[0]);
    S.push(points[1]);
    S.push(points[2]);

    for(unsigned int i = 3; i < points.size(); i++) {
        while (orientation(nextToTop(S), S.top(), points[i]) != 2) 
            S.pop(); 
        S.push(points[i]); 
    }

    while (!S.empty()) { 
        Point p = S.top(); 
        convexHull.push_back(p);
        S.pop(); 
    }
    
    reverse(convexHull.begin(), convexHull.end());

    return convexHull;
}

bool isInsideConvexHull(vector<Point> &convexHull, Point &point) {
    // a point is inside a convex hull if it makes a counter clock direction with the convexl hull points
    int size = convexHull.size();
    int o;
    
    if(size >= 3) {
        for(unsigned int i = 0; i < size; i++) {
            if(i != size - 1)
                o = orientation(point, convexHull[i], convexHull[i + 1]);
            else 
                o = orientation(point, convexHull[i], convexHull[0]);
            if(o == 1)
                return false;
        }
    } else return false;
    
    return true;
}

vector<Point> pointsInsideConvexHull(vector<Point> &convexHull, vector<Point> &points) {
    // a point is inside a convex hull if it makes a counter clock direction with the convexl hull points
    int cSize = convexHull.size();
    int pSize = points.size();
    int o;
    bool inside = false;
    vector<Point> pointsInside {};
    
    if(cSize >= 3) {
        for(unsigned int n = 0; n < pSize; n++) {
            inside = true; 

            for(unsigned int i = 0; i < cSize; i++) {
                if(i != cSize - 1)
                    o = orientation(points[n], convexHull[i], convexHull[i + 1]);
                else 
                    o = orientation(points[n], convexHull[i], convexHull[0]);
                

                // if the orientation is either, CC or linear, keep testing it for the other convex hull points, otherwise the point is outside
                if(o == 1) {
                    inside = false;
                    break;
                }
            }
            if(inside) {
                pointsInside.push_back(points[n]);
            }
        }
    }
    
    return pointsInside;
}

void gridInsideConvexHull(int xMax, int yMax, int offset, const char *convexHullFile) {
    int xPoints = xMax/offset + 1;
    int yPoints = yMax/offset + 1;
    vector<Point> gridList {};
    vector<Point> convexHull {};
    vector<Point> gridInsideConvex {};

    for(int x = 0; x < xPoints; x++) {
        for(int y = 0; y < yPoints; y++) {
            gridList.push_back(Point(x * offset, y * offset, 0));
        }
    }

    convexHull = readPointsFromFile(convexHullFile);
    gridInsideConvex = pointsInsideConvexHull(convexHull, gridList);

    for(auto i: convexHull) {
        if(!findPoint(gridInsideConvex, i)) {
            gridInsideConvex.push_back(i);
        }
    }

    writePointsToFile("../input_files/gridFile.txt", gridInsideConvex);
}

vector<Point> readPointsFromFile(const char *name) {
    FILE *fp = NULL;
    vector<Point> points {};
    char buffer[32];

    fp = fopen(name, "r");

    if(fp) {
        cout << "File opened with success!\n";
        int x, y, z;
        while(fgets(buffer, sizeof(buffer), fp)) {
            sscanf(buffer, "%d %d %d", &x, &y, &z);
            points.push_back(Point(x, y, z));
        }
    }
    else {
        cout << "Cannot open the file!\n";
        exit(-1); 
    }

    fclose(fp);

    return points; 
}

void writePointsToFile(const char *name, vector<Point> points) {
    FILE *fp = NULL;

    char path[64];
    int n = sprintf(path, "../input_files/%s", name);
    if(n < 0) {
        printf("Error generating the file\n");
        exit(-1);
    }

    fp = fopen(path, "w");
    if(fp) {
        printf("File created with success!\n");
        for(auto i: points) {
            fprintf(fp, "%d %d %d\n", i.x, i.y, i.z);
        }
    }
    else {
        printf("Cannot create the file!\n");
        exit(-1);   
    }

    fclose(fp);
}

void printPoints(vector<Point> &points) {
    for(unsigned int i = 0; i < points.size() ; i++) {
        printf("Point %d: (%d, %d, %d)\n", i + 1, points[i].x, points[i].y, points[i].z);
    }
    printf("\n");
}

bool findPoint(vector<Point> &points, Point &point) {
    for(auto i: points) {
        if(point.x == i.x && point.y == i.y && point.z == i.z) {
            return true;
        }
    } 
    return false;
}

double getMaxRange(double rxSensitivity, double lambda, double pathLossExpoent) {
    return D0*pow(10, ((dbmTodB(TX_POWER) - dbmTodB(rxSensitivity) - 20 * log10(4 * M_PI * D0 / lambda)) / (10 * pathLossExpoent)));
};

double dbmTodB(double dbm) {
    return dbm - 30;
};

double getLambda(double frequency) {
    return (long) C / frequency;
};

double dist3D(Point &PA, Point &PB) {
    return sqrt(pow(PA.x - PB.x, 2) + pow(PA.y - PB.y, 2) + pow(PA.z - PB.z, 2));
}

bool equalPoints(Point &PA, Point &PB) {
    if(PA.x == PB.x && PA.y == PB.y)
        return true;
    return false;
}

map<int, double> maxFreeDistanceTableIEEE80211g(double pathLossExpoent) {
    map<int,double> tableMaxDist;

	tableMaxDist.insert(std::make_pair(6,  getMaxRange((double) -82, getLambda(F), pathLossExpoent)));
	tableMaxDist.insert(std::make_pair(9,  getMaxRange((double) -81, getLambda(F), pathLossExpoent)));
	tableMaxDist.insert(std::make_pair(12, getMaxRange((double) -79, getLambda(F), pathLossExpoent)));
	tableMaxDist.insert(std::make_pair(18, getMaxRange((double) -77, getLambda(F), pathLossExpoent)));
	tableMaxDist.insert(std::make_pair(24, getMaxRange((double) -74, getLambda(F), pathLossExpoent)));
	tableMaxDist.insert(std::make_pair(36, getMaxRange((double) -70, getLambda(F), pathLossExpoent)));
	tableMaxDist.insert(std::make_pair(48, getMaxRange((double) -66, getLambda(F), pathLossExpoent)));
	tableMaxDist.insert(std::make_pair(54, getMaxRange((double) -65, getLambda(F), pathLossExpoent)));

    return tableMaxDist;
}

double randomProbability(){
    return (double)rand()/RAND_MAX;
}

bool minComparator(int a, int b) {
    return (a < b);
}