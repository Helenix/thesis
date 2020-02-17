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
}

AlgorithmHelper::~AlgorithmHelper() {
    delete scenarioHelper;
    printf("Finished\n");
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
        population.push_back(getIndividual(i, scenarioHelper));
    }
}

Individual AlgorithmHelper::getIndividual(int pop, ScenarioHelper *scPointer) {
    Individual individual {};

    if(pop < 0.2* getPopSize()) {
        scPointer->deployUAV();
    } else {
        scPointer->deployUAVrandomly();
    }

    for(auto i: scPointer->flyingNodesList) {
        if(i->flying) {
            FlyingNodeAllele allele {};

            allele.bridging = i->bridging;
            allele.serving = i->serving;
            allele.flying = i->flying;
            allele.height = i->height;
            allele.id = i->id;
            allele.p = i->p;

            for(auto j: scPointer->groundNodesList) {
                if(j->connectedTo == i->id) {
                    allele.children.push_back(j->id);
                }
            }

            individual.uavNumber++;
            // Doesn't make much sense to have genes.chromosomes. "vector<Allele> genes" could be directly definied in the Invididual class
            individual.genes.chromosomes.push_back(allele);
        }
    }

    individual.maxDissatisfaction = calculateMaxDissatisfaction(individual, scPointer);

    scPointer->resetNetwork();

    return individual;
}

double AlgorithmHelper::calculateMaxDissatisfaction(Individual individual, ScenarioHelper *scPointer) {
    map<int, double> disMap;

    for(auto i: individual.genes.chromosomes) {
        FlyingNodeAllele tmpAllele = i;

        for(auto j: tmpAllele.children) {
            GroundNode *grounNode = scPointer->getGroundNodeByID(j);
            double distance = dist3D(grounNode->p, tmpAllele.p);
            int dataRate = grounNode->dataRate;

            auto it = scPointer->maxFreeDistTableIEEE80211g.begin();
            int maxDataRatePossible = (*it).first;

            for(; it != scPointer->maxFreeDistTableIEEE80211g.end(); it++) {
                if(distance <= (*it).second) {
                    maxDataRatePossible = (*it).first;
                }
            }
            if(maxDataRatePossible - dataRate < 0) {
                disMap.insert(make_pair(grounNode->id, ((double) (-100* (maxDataRatePossible - dataRate)) / (double) dataRate)));
            } else {
                disMap.insert(make_pair(grounNode->id, 0));
            }
        }
    }

    auto it = disMap.begin();
    double maxDiss = 0.0;
    
    for(; it != disMap.end(); it++) {
        if((*it).second >= maxDiss) {
            maxDiss = (*it).second;
        }
    }

    return maxDiss;
}

