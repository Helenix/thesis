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

        vector<Individual> population;
        vector<Individual> Rpopulation;
        vector<Individual> matingPool;

    protected:
        ScenarioHelper *scenarioHelper;
        int popSize, objectiveFunctions;
        int generations;
        // Probabilty of crossover and mutation
        double pc, pm;
};


#endif