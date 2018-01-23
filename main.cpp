#include <ompl/geometric/planners/bitstar/BITstar.h>
#include <ompl/geometric/planners/cforest/CForest.h>
#include <ompl/geometric/planners/fmt/FMT.h>
#include <ompl/geometric/planners/fmt/BFMT.h>
#include <ompl/geometric/planners/prm/PRMstar.h>
#include <ompl/geometric/planners/rrt/InformedRRTstar.h>
#include <ompl/geometric/planners/rrt/RRT.h>
#include <ompl/geometric/planners/rrt/RRTsharp.h>
#include <ompl/geometric/planners/rrt/RRTstar.h>
#include <ompl/geometric/planners/est/EST.h>
#include <ompl/geometric/planners/sbl/SBL.h>
#include <ompl/geometric/planners/sst/SST.h>
#include <ompl/geometric/planners/kpiece/KPIECE1.h>
#include <ompl/geometric/planners/stride/STRIDE.h>
#include <ompl/geometric/planners/rrt/SORRTstar.h>

#include "steer_functions/POSQ/POSQSteering.h"
#include "steer_functions/Linear/LinearSteering.h"
#include "steer_functions/ReedsShepp/ReedsSheppSteering.h"

#include "base/Environment.h"
#include "base/gnode.h"

#include "metrics/PathLengthMetric.h"

#include "gui/PathEvaluation.h"
#include "gui/QtVisualizer.h"


#include "planners/AStar.hpp"
#include "planners/ThetaStar.h"
#include "planners/OMPLPlanner.hpp"

#include "PostSmoothing.h"


namespace og = ompl::geometric;

int main(int argc, char **argv)
{
    PathEvaluation::initialize();

    std::vector<unsigned int> environments;
    std::vector<double> jerkMetrics;
    std::vector<double> costMetrics;
    std::vector<double> pathLengthMetrics;
    std::vector<double> speedArcLengthMetrics;
    std::vector<double> peaksMetrics;

    PlannerSettings::initializeSteering();

    const unsigned int RUNS = 20;
    unsigned int totalRun = 0;
    PathStatisticsAggregator statsAggregator;
    for (int run = 1; run <= RUNS; ++run, ++totalRun)
    {
        OMPL_INFORM("#####################################################");
        OMPL_INFORM("#                    RUN %i                         #", run);
        OMPL_INFORM("#####################################################");


        QtVisualizer::initialize();

//        PlannerSettings::environment = Environment::createRandomCorridor(150, 150, 6, 300, //1502484532); //1502407983); //1502323408); //1502316103); //1502231684); //1502227898); //1501893283); //1501892155);//1501089540); //1501089410 );//1500660612);// 1500551721);// 1500550472);
//                                                                         (unsigned int) (time(nullptr) + totalRun));

//        PlannerSettings::environment = Environment::createRandomCorridor(50, 50, 5, 40, //1502484532); //1502407983); //1502323408); //1502316103); //1502231684); //1502227898); //1501893283); //1501892155);//1501089540); //1501089410 );//1500660612);// 1500551721);// 1500550472);
//                                                                         (unsigned int) (time(nullptr) + totalRun));

        PlannerSettings::environment = Environment::createRandom(50, 50,
                                                                 .05);
//        PlannerSettings::environment = Environment::createSimple();

        QtVisualizer::visualize(*PlannerSettings::environment, run);

        RRTPlanner rrtPlanner;
        if (rrtPlanner.run())
        {
            auto p = rrtPlanner.solutionPath();
            auto t = rrtPlanner.solutionTrajectory();
            PostSmoothing::smooth(t, p);
//            PathEvaluation::add(rrtPlanner.solutionPath(), rrtPlanner.solutionTrajectory(),
//                                "RRT", rrtPlanner.planningTime(), Qt::red,
//                                rrtPlanner.smoothBSpline(),
//                                rrtPlanner.simplifyMax());
        }


        Log::instantiateRun();
        statsAggregator.add(PathEvaluation::add(new RRTPlanner, "RRT", Qt::red));
        statsAggregator.add(PathEvaluation::add(new RRTstarPlanner, "RRT*", QColor(180, 140, 0)));
        statsAggregator.add(PathEvaluation::add(new SORRTstarPlanner, "SORRT*", Qt::darkCyan));
        statsAggregator.add(PathEvaluation::add(new RRTsharpPlanner, "RRT#", Qt::darkGreen));
        statsAggregator.add(PathEvaluation::add(new PRMPlanner, "PRM", Qt::blue));

        statsAggregator.add(PathEvaluation::add(new ThetaStar, "Theta*", Qt::black));
//        statsAggregator.add(PathEvaluation::add(new AStar, "A*", Qt::gray));

        Log::storeRun();

//        QtVisualizer::show(); // todo reactivate

    }
    Log::save();

    statsAggregator.showSummary();

    return QtVisualizer::exec();
}