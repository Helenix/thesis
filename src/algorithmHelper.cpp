#include "../headers/algorithmHelper.h"

AlgorithmHelper::AlgorithmHelper(int xMax, int yMax, int offset, const char *convexHullFile, const char *gridFile, const char *gpointsFile, const char *demandFile) {
    setPopSize(NSGA_POPSIZE);
    setPm(NSGA_PM);
    setPc(NSGA_PC);
    setObjectiveFuncitons(NSGA_OBJFUNCT);
    setGenerations(NSGIA_GENERATIONS);
    setPopSize(NSGA_POPSIZE);

    scenarioHelper = new ScenarioHelper(gpointsFile);
    scenarioHelper->maxFreeDistTableIEEE80211g = maxFreeDistanceTableIEEE80211g(PATH_LOSS_EXPOENT);
    scenarioHelper->initializeNetworkArea(xMax, yMax, offset, convexHullFile, gridFile);
    scenarioHelper->setGroundNodesDemand(demandFile);
    scenarioHelper->deployUAVrandomly();
}

void AlgorithmHelper::setPopSize(int popSize) {
    this->popSize = popSize;
}

void AlgorithmHelper::setPc(double pc) {
    this->pc = pc;
}

void AlgorithmHelper::setPm(double pm) {
    this->pm = pm;
}

void AlgorithmHelper::setObjectiveFuncitons(int number) {
    this->objectiveFunctions = number;
}

void AlgorithmHelper::setGenerations(int gens) {
    this->generations = gens;
}

int AlgorithmHelper::getPopSize() {
    return this->popSize;
}

double AlgorithmHelper::getPc() {
    return this->pc;
}

double AlgorithmHelper::getPm() {
    return this->pm;
}

int AlgorithmHelper::getObjectiveFuncitons() {
    return this->objectiveFunctions;
}

int AlgorithmHelper::getGenerations() {
    return this->generations;
}

ScenarioHelper* AlgorithmHelper::getScenariohelperPointer() {
    return this->scenarioHelper;
}

void AlgorithmHelper::initializelPopulation() {
    for(int i = 0; i < getPopSize(); i++) {
        population.push_back(Individual());
    }
}

Individual AlgorithmHelper::getIndividual(int pop, ScenarioHelper *scPointer) {
    Individual individual {};

    return individual;
}