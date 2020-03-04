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

    matingPool.resize(popSize, Individual());
    rPopulation.resize(2 * popSize, Individual());
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
            // Doesn't make much sense to have genes.chromosomes! "vector<Allele> genes" could be directly definied in the Invididual class
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

void AlgorithmHelper::runNSGAII(float alpha) {
    int currentGeneration = 0;
    int last = 0;
    int step = 10;
    vector<Individual> tmpPopulation {};

    initializelPopulation();

    do {
        resetFronts();
        if(currentGeneration == 0) {
            for(int i = 0; i < population.size(); i++) {
                matingPool[i] = population[i];
            }
        } else {
            // Tournament
        }

        generateOffspring();

        currentGeneration++;
    } while(stopCriteria > 0.05);
}

void AlgorithmHelper::resetFronts() {
    fronts.erase(fronts.begin(), fronts.end());
    fronts.clear();
    fronts = {};
}

void AlgorithmHelper::generateOffspring() {
    for(int i = 0; i < matingPool.size(); i++) {
      // Crossover
        if(randomProbability() <= getPc()) {
            int randomIndex = rand() % matingPool.size();
            matingPool[i] = crossover(matingPool[i], matingPool[randomIndex] ,scenarioHelper);
            exit(-1);
        }
    } 

    for(int i = 0; i < matingPool.size(); i++) {
        // Mutation
        if(randomProbability() <= getPm()) {
            mutation(matingPool[i], scenarioHelper);
        }
    }
}

void AlgorithmHelper::mutation(Individual &individual, ScenarioHelper *scPointer) {

    printf("Individual size %lu\n", individual.genes.chromosomes.size());

    int index = rand() % individual.genes.chromosomes.size();
    int indexId = individual.genes.chromosomes[index].id;
    FlyingNodeAllele tmpAllele = individual.genes.chromosomes[index];
    individual.genes.chromosomes.erase(individual.genes.chromosomes.begin() + index);

    /* for(auto i: individual.genes.chromosomes) {
        if(i.flying && i.serving) {
            printf("Node %d Serving: \n", i.id);
            for(auto j: i.children) {
            printf("\tChildren %d\n", j);
            }
        } else if(i.flying && i.bridging) {
            printf("Node %d bridging: \n", i.id);;
        }
    } */

    if(randomProbability() <= 0.5) {
        // Type 1 of mutation: remove random Uav
        for(auto i: individual.genes.chromosomes) {
            FlyingNode *uav = scPointer->getUavNodeById(i.id);
            uav->bridging = i.bridging;
            uav->flying = i.flying;
            uav->serving = i.serving;
            uav->height = i.height;
            uav->p = i.p;
        }

        scPointer->updateAdjacencyMatrix();
        connectGnodes(individual.genes.chromosomes, scPointer);

        int uavConnectedToRoot = scPointer->getGroundNodeByID(0)->connectedTo;

        // printf("Uav connected to root node %d\n", uavConnectedToRoot);
        // printf("UAV ID to be removed %d\n", indexId);

        if(!scPointer->allGNodesConnected()) {
            individual.genes.chromosomes.push_back(tmpAllele);
            scPointer->resetNetwork();
            printf("Mutation: Ground nodes are't all connected\n");
            return;
        }

        for(auto i: scPointer->flyingNodesList) {
            // scPointer->isNodeReachable(i->id, 0)) the 0 should be changed to the uav assigned to ground node 0, and what if the uav is randomly removed?
            if(!scPointer->isNodeReachable(i->id, uavConnectedToRoot)) {
                individual.genes.chromosomes.push_back(tmpAllele);
                scPointer->resetNetwork();
                printf("Mutation: Uavs aren't all reachable\n");
                return;
            }
        }

        individual.uavNumber = individual.genes.chromosomes.size();
        individual.maxDissatisfaction = calculateMaxDissatisfaction(individual, scPointer);
        scPointer->resetNetwork();
    } else {
        // Type 2 of mutation: realocated random Uav
        Point backUp = individual.genes.chromosomes[index].p;

        for(auto i: individual.genes.chromosomes) {
            FlyingNode *uav = scPointer->getUavNodeById(i.id);
            uav->bridging = i.bridging;
            uav->flying = i.flying;
            uav->serving = i.serving;
            uav->height = i.height;
            for(auto &gridPoint: scPointer->gridInsideConvex) {
                if(equalPoints(gridPoint, i.p)) {
                    gridPoint.busy = true;
                    gridPoint.z = i.height;
                    uav->p = i.p;
                }
            }
        }

        bool foundAvailableSpot = false;
        Point newPosition;

        while(!foundAvailableSpot) {
            int gridIndex = rand() % scPointer->gridInsideConvex.size();
            if(!scPointer->gridInsideConvex[gridIndex].busy) {
                foundAvailableSpot = true;
                newPosition = scPointer->gridInsideConvex[gridIndex];
                newPosition.z = randomHeight();
            }
        }

        // printf("OLD POSITION: %d %d %d\n", backUp.x, backUp.y, backUp.z);
        // printf("NEW POSITION: %d %d %d\n", newPosition.x, newPosition.y, newPosition.z);

        FlyingNode *reUavPointer = scPointer->getUavNodeById(tmpAllele.id);
        reUavPointer->flying = tmpAllele.flying;
        reUavPointer->bridging = tmpAllele.bridging;
        reUavPointer->serving = tmpAllele.serving;
        reUavPointer->p = newPosition;

        individual.genes.chromosomes[index].p = newPosition;

        connectGnodes(individual.genes.chromosomes, scPointer);
        scPointer->removeUnusedUavs();
        scPointer->updateAdjacencyMatrix();

        int uavConnectedToRoot = scPointer->getGroundNodeByID(0)->connectedTo;

        // printf("Uav connected to root node %d\n", uavConnectedToRoot);
        // printf("UAV ID to be realocated %d\n", indexId);

        if(!scPointer->allGNodesConnected()) {
            individual.genes.chromosomes[index].p = backUp;
            scPointer->resetNetwork();
            printf("Mutation: Ground nodes are't all connected\n");
            return;
        }

        for(auto i: scPointer->flyingNodesList) {
            if(!scPointer->isNodeReachable(i->id, uavConnectedToRoot)) {
                individual.genes.chromosomes[index].p = backUp;
                scPointer->resetNetwork();
                printf("Mutation: Uavs aren't all reachable\n");
                return;
            }
        }


        individual.uavNumber = individual.genes.chromosomes.size();
        individual.maxDissatisfaction = calculateMaxDissatisfaction(individual, scPointer);
        scPointer->resetNetwork();
    }
}

Individual AlgorithmHelper::crossover(Individual &individualA, Individual &individualB, ScenarioHelper *scPointer) {
    Individual newIndvidual {};
    newIndvidual.genes.chromosomes = getSeparationZone(individualA, individualB, scPointer);

    return newIndvidual;
}

vector<FlyingNodeAllele> AlgorithmHelper::getSeparationZone(Individual &individualA, Individual &individualB, ScenarioHelper *scPointer) {
    vector<FlyingNodeAllele> tmpA {individualA.genes.chromosomes};
    vector<FlyingNodeAllele> tmpB {individualB.genes.chromosomes};
    vector<FlyingNodeAllele> result {};
    double localTheta = 45;

    const auto xValues = minmax_element(scPointer->gridInsideConvex.begin(), scPointer->gridInsideConvex.end(), [] (const Point &a,const Point &b) { return minComparator(a.x, b.x); });
    const auto yValues = minmax_element(scPointer->gridInsideConvex.begin(), scPointer->gridInsideConvex.end(), [] (const Point &a,const Point &b) { return minComparator(a.y, b.y); });

    int xMin = xValues.first->x;
    int xMax = xValues.second->x;
    int yMin = yValues.first->y;
    int yMax = yValues.second->y;

    double maxRange = getMaxRange(MIN_RX_SENSITIVITY, getLambda(F), PATH_LOSS_EXPOENT);
    int randomCut = rand() % 4;
    int xMean = round((xMin + xMax) / 2);
    int yMean = round((yMin + yMax) / 2);

    Point pivot(xMean, yMean, 0);
    Point pivotMax;
    Point pivotMin;

    // d = (x − x1) * (y2 − y1) − (y − y1) * (x2 − x1) with pivotMax = (x1, y1), pivotMin = (x2, y2) and point to analyse (x, y)
    // d < 0 down side of the line
    // d > 0 upper side of the line
    // d = 0 in the line
    int d;

    printf("Xmean: %d yMean: %d\n", xMean, yMean);

    auto indivualAIt = tmpA.begin();
    auto indivualBIt = tmpB.begin();
    
    switch (randomCut) {
        // >= <= doesnt make a lot of sense since it removes points that are on the cutting line
        case 0:
            // Vertical cut
            // Left side of individual A
            for(; indivualAIt != tmpA.end(); ) {
                if((*indivualAIt).p.x >= xMean) {
                    indivualAIt = tmpA.erase(indivualAIt);
                } else {
                    indivualAIt++;
                }
            }
            // Right side of individual B
            for(; indivualBIt != tmpB.end(); ) {
                if((*indivualBIt).p.x <= xMean) {
                    indivualBIt = tmpB.erase(indivualBIt);
                } else {
                    indivualBIt++;
                }
            }  
            break;

        case 1:
            // Horizontal cut
            // Down side of individual A
            for(; indivualAIt != tmpA.end(); ) {
                if((*indivualAIt).p.y >= yMean) {
                    indivualAIt = tmpA.erase(indivualAIt);
                } else {
                    indivualAIt++;
                }
            }

            // Up side of individual B
            for(; indivualBIt != tmpB.end(); ) {
                if((*indivualBIt).p.y <= yMean) {
                    indivualBIt = tmpA.erase(indivualBIt);
                } else {
                    indivualBIt++;
                }
            }
            break;

        case 2:
            // 45º cut
            while(pivot.x <= xMax + (int) maxRange) {
                pivot = getNextPointLine(pivot, xMin, xMax, maxRange, true, localTheta);
            }
            pivotMax.x = pivot.x;
            pivotMax.y = pivot.y;

            pivot.x = xMean;
            pivot.y = yMean;

            while(pivot.x >= xMin - (int) maxRange) {
                pivot = getNextPointLine(pivot, xMin, xMax, maxRange, false, localTheta);
            }

            pivotMin.x = pivot.x;
            pivotMin.y = pivot.y;

            for(; indivualAIt != tmpA.end(); ) {
                d = ((*indivualAIt).p.x - pivotMax.x) * (pivotMin.y - pivotMax.y) - ((*indivualAIt).p.y -pivotMax.y) * (pivotMin.x - pivotMax.x);

                if(d <= 0) {
                    indivualAIt = tmpA.erase(indivualAIt);
                } else {
                    indivualAIt++;
                }
            }

            for(; indivualBIt != tmpB.end(); ) {
                d = ((*indivualBIt).p.x - pivotMax.x) * (pivotMin.y - pivotMax.y) - ((*indivualBIt).p.y -pivotMax.y) * (pivotMin.x - pivotMax.x);

                if(d >= 0) {
                    indivualBIt = tmpB.erase(indivualBIt);
                } else {
                    indivualBIt++;
                }
            }
            break;

        case 3:
            // -45º cut
            while(pivot.x <= xMax + (int) maxRange) {
                pivot = getNextPointLine(pivot, xMin, xMax, maxRange, true, -localTheta);
            }
            pivotMax.x = pivot.x;
            pivotMax.y = pivot.y;

            pivot.x = xMean;
            pivot.y = yMean;

            printf("\n");

            while(pivot.x >= xMin - (int) maxRange) {
                pivot = getNextPointLine(pivot, xMin, xMax, maxRange, false, -localTheta);
            }

            pivotMin.x = pivot.x;
            pivotMin.y = pivot.y;
            

            printf("\n");

            for(; indivualAIt != tmpA.end(); ) {
                d = ((*indivualAIt).p.x - pivotMax.x) * (pivotMin.y - pivotMax.y) - ((*indivualAIt).p.y -pivotMax.y) * (pivotMin.x - pivotMax.x);

                if(d >= 0) {
                    indivualAIt = tmpA.erase(indivualAIt);
                } else {
                    indivualAIt++;
                }
            }

            for(; indivualBIt != tmpB.end(); ) {
                d = ((*indivualBIt).p.x - pivotMax.x) * (pivotMin.y - pivotMax.y) - ((*indivualBIt).p.y -pivotMax.y) * (pivotMin.x - pivotMax.x);

                if(d <= 0) {
                    indivualBIt = tmpB.erase(indivualBIt);
                } else {
                    indivualBIt++;
                }
            }
            break;
    
        default:
            printf("AlgorithmsHelper::crossover: Error in random cut\n");
            break;
    }
    repairDuplicateGnodes(tmpA, tmpB, scPointer);

    for(auto i: tmpA) {
        result.push_back(i);
    }

    for(auto i: tmpB) {
        result.push_back(i);
    }

    return result;
}

Point AlgorithmHelper::getNextPointLine(Point &pivot, int xMin, int xMax, int maxRange, bool rightSide, double theta) {
    // given that we only want 45 or -45 this function can be simplified since the next point on the line is always pivot plus range for x and y ( because y = (+-1)x + b at pivot)

    theta *= M_PI / 180.0;
    int x = pivot.x;
    int b = pivot.y - round(tan(theta) * pivot.x);

    if(rightSide) {
        // searches right side of pivot
        x = pivot.x + maxRange;
    } else {
        // searches left side of pivot
        x = pivot.x - maxRange;
    }

    int y = round(tan(theta)*x) + b;

    pivot.x=x;
    pivot.y=y;

return pivot;

}

void AlgorithmHelper::repairDuplicateGnodes(vector<FlyingNodeAllele> &tmpA, vector<FlyingNodeAllele> &tmpB, ScenarioHelper *scPointer) {
    vector<int> common {};
    map<int, pair<pair<FlyingNodeAllele, int>, pair<FlyingNodeAllele, int>>> toRepair;

    for(int i = 0; i < tmpA.size(); i++) {
        for(int j = 0; j < tmpB.size(); j++) {
            for(const auto a: tmpA[i].children) {
                for(const auto b: tmpB[j].children) {
                    if(a == b) {
                        common.push_back(a);
                        toRepair.insert(std::make_pair(a, std::make_pair(std::make_pair(tmpA[i], i), std::make_pair(tmpB[j], j))));
                    }
                }
            }
        }
    }

    for(const auto i: common) {
        printf("Common ID %d\n", i);
    }

    auto it = toRepair.begin();

    for(; it != toRepair.end(); it++) {
        printf("GN %d common to Alelle %d (index %d) and  Alelle %d (index %d)\n", (*it).first, (*it).second.first.first.id, (*it).second.first.second, (*it).second.second.first.id, (*it).second.second.second);
        GroundNode *gn = scPointer->getGroundNodeByID((*it).first);
        printf("GN: %d\n", gn->id);
        double dA = dist3D(gn->p, (*it).second.first.first.p);
        double dB = dist3D(gn->p, (*it).second.second.first.p);
        printf("dA %f\n", dA);
        printf("dB %f\n", dB);

        if(dA < dB) {
            printf("Removed from B\n");
            tmpB.erase(tmpB.begin() + (*it).second.second.second);
        } else {
            printf("Removed from A\n");
            tmpA.erase(tmpA.begin() + (*it).second.first.second);
        }
    }
}

void AlgorithmHelper::connectGnodes(vector<FlyingNodeAllele> &tmpUavs, ScenarioHelper *scPointer) {
    for(auto gnode: scPointer->groundNodesList) {
        int uavId = scPointer->getBestSignal(gnode);
        if(uavId >= 0) {
            if(gnode->connectedTo != uavId) {
                int currentUavId = gnode->connectedTo;

                if(currentUavId >= 0) {
                    // to do
                    printf("For now it doens't enter here\n");
                } else {
                    for(auto i: tmpUavs) {
                        if(uavId == i.id ) {
                            gnode->isConnected = true;
                            gnode->connectedTo = uavId;
                            i.children.push_back(gnode->id);
                            i.serving = true;
                            break;
                        }
                    }
                }
            }
        } 
    }
}