#ifndef UTILITYHELPER_H_
#define UTILITYHELPER_H_

#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <stack>
#include <algorithm>
#include <list>

#define TX_POWER 23             // UAV Transmitted Power (dBm)  
#define MIN_RX_SENSITIVITY -82  // Ground Nodes Min Sensitivity (dBm)
#define D0 1                    // 1m D0 ref on log path loss model
#define PATH_LOSS_EXPOENT 2.2   // path loss expoent on log path loss model
#define LAMBDA 0.12437810945    // wavelength of uav signal
#define RB 6                    // Data rate (MBit/s)
#define AREA 5000*5000          // Area xMax * yMax (m^2)
#define N_GNS 100               // Number of Ground Nodes  

#define C  300000000            // Speed of Light (m/s)
#define F 2412000000            // UAV Carrier Frequency (Hz)


#define D 1760.93               // Maximum UAV-GN Distance (m)
#define NSGA_ALPHA 0.3                // Alfa
#define NSGA_POPSIZE 80      // NSGA-II Population Size
#define NSGA_PC 0.9             // NSGA-II Pc
#define NSGA_PM 0.6             // NSGA-II Pm
#define NSGIA_GENERATIONS 65      // NSGA-II Number of Iterations
#define NSGA_OBJFUNCT 2
#define INFINITE_DISTANCE 1000000



using namespace std;

inline int randomHeight() {
    vector<int> heightList = {40, 80, 120};
    int index = rand() % heightList.size();
    
    return heightList[index];
}

struct Point {
    int x, y, z;

    // might be better to define busy in Flying Node, gns dont need it?
    bool busy;
    
    // constructor initialization lists
    Point(): x(-1), y(-1), z(-1) , busy(false) {} 
    Point(int x, int y, int z): x(x), y(y), z(z), busy(false) {}
    Point(const Point &p): x(p.x), y(p.y), z(p.z), busy(p.busy) {}
};

struct Node {
    Point p;
    const int id;
    int connectedTo;
    bool isConnected;

    Node(int id): p(), id(id), connectedTo(-1), isConnected(false) {}
    Node(int id, int x, int y, int z): p(x, y, z), id(id), connectedTo(-1), isConnected(false) {}
};

struct GroundNode: Node {
        /* Fixed data rate of 6 MBits/s */
        int dataRate;

        GroundNode(int id): Node(id), dataRate(6) {}
        GroundNode(int id, int x, int y, int z): Node(id, x, y, z), dataRate(6) {}
};

struct FlyingNode: Node {
        int height;
        bool flying;
        bool serving;
        bool bridging;


        FlyingNode(int id): Node(id),  height(0), flying(false), serving(false), bridging(false) {}
        FlyingNode(int id, int x, int y, int z): Node(id, x, y, z), height(z), flying(false), serving(false), bridging(false) {}
};

struct FlyingNodeAllele {
    /* Contains the information of flying nodes to be used in the Genetic Algorithm */
    int id;
    int height;
	bool flying;
	bool serving;
	bool bridging;
    Point p;
    vector<int> children;

    FlyingNodeAllele(): p(), id(-1), height(0), flying(false), serving(false), bridging(false), children {} {}
    FlyingNodeAllele(const FlyingNodeAllele &fly): id(fly.id), height(fly.height), p(fly.p), flying(fly.flying), 
            serving(fly.serving), bridging(fly.bridging), children(fly.children) {}
};

struct Chromosome {
    vector<FlyingNodeAllele> chromosomes;

    Chromosome(): chromosomes {} {}
    Chromosome(const Chromosome &chromosomes): chromosomes(chromosomes.chromosomes) {}  
};

struct Individual {
    /* NSGA2 Sol */
    int rank, np, uavNumber;
	double crowdingDistance, maxDissatisfaction;
	Chromosome genes;
    /* Vector of Individual addresses  */
	vector<Individual *> s;

    Individual(): rank(0), np(0), uavNumber(0), crowdingDistance(0.0), maxDissatisfaction(0.0), genes(), s {} {}
    Individual(int uavNumber, double maxDissatisfaction): rank(0), np(0), uavNumber(uavNumber), 
            crowdingDistance(0.0), maxDissatisfaction(maxDissatisfaction), genes(), s {} {}
    Individual(const Individual &i): rank(i.rank), np(i.np), uavNumber(i.uavNumber), 
            crowdingDistance(i.crowdingDistance), maxDissatisfaction(i.maxDissatisfaction), genes(i.genes), s(i.s) {}
};

int swap(Point &pA, Point &pB);
Point nextToTop(stack<Point> &S);
double distance(Point pA, Point pB);
double orientation(Point pA, Point pB, Point pC);
int compare(const void *vpA, const void *vpB);
vector<Point> findConvexHull(vector<Point> &points);
bool isInsideConvexHull(vector<Point> &convexHull, Point &point);
vector<Point> pointsInsideConvexHull(vector<Point> &convexHull, vector<Point> &points);
void gridInsideConvexHull(int xMax, int yMax, int offset, const char *convexHullFile);

vector<Point> readPointsFromFile(const char *name);
void writePointsToFile(const char *name, vector<Point> points);
void printPoints(vector<Point> &points);
bool findPoint(vector<Point> &points, Point &point);

double getMaxRange(double rxSensitivity, double lambda, double pathLossExpoent);
double dbmTodB(double dbm);
double getLambda(double frequency);
double dist3D(Point &PA, Point &PB);
bool equalPoints(Point &PA, Point &PB);

map<int, double> maxFreeDistanceTableIEEE80211g(double pathLossExpoent);

double randomProbability();
bool minComparator(int a, int b);

#endif