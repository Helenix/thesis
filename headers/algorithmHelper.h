#ifndef ALGORITHMHELPER_H_
#define ALGORITHMHELPER_H_

#include "scenarioHelper.h"

using namespace std;

class AlgorithmHelper {
    public:
        AlgorithmHelper(int xMax, int yMax, int offset, const char *convexHullFile, const char *gridFile, const char *gpointsFile, const char *demandFile);
        ~AlgorithmHelper();

        void setPopSize(int popSize);
        void setPc(double pc);
        void setPm(double pc);
        void setObjectiveFuncitons(int number);
        void setGenerations(int gens);
        int getPopSize(); 
        double getPc();
        double getPm();
        int getObjectiveFuncitons();
        int getGenerations();
        ScenarioHelper *getScenariohelperPointer();
        void initializelPopulation();
        Individual getIndividual(int pop, ScenarioHelper *scPointer);
        double calculateMaxDissatisfaction(Individual individual, ScenarioHelper *scPointer);
        void runNSGAII(float alpha);
        void resetFronts();
        void generateOffspring();
        void mutation(Individual &individual, ScenarioHelper *scPointer);
        Individual crossover(Individual &individualA, Individual &individualB, ScenarioHelper *scPointer);
        vector<FlyingNodeAllele> getSeparationZone(Individual &individualA, Individual &individualB, ScenarioHelper *scPointer);
        Point getNextPointLine(Point &pivot, int xMin, int xMax, int maxRange, bool rightSide, double theta);
        void repairDuplicateGnodes(vector<FlyingNodeAllele> &tmpA, vector<FlyingNodeAllele> &tmpB, ScenarioHelper *scPointer);
        void connectGnodes(vector<FlyingNodeAllele> &tmpUavs, ScenarioHelper *scPointer);

        vector<Individual> population {};
        vector<Individual> rPopulation {};
        vector<Individual> matingPool {};
        vector<vector<Individual>> fronts {};
        vector<double> newMembers {};
        int generations;
        float stopCriteria;


    protected:
        ScenarioHelper *scenarioHelper;
        int popSize, objectiveFunctions;
        // Probabilty of crossover and mutation
        double pc, pm;
};


#endif