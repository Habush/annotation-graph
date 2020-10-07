/**
Created by Abdulrahman Semrie<hsamireh@gmail.com> on 10/2/20
Copyright © 2020 Abdulrahman Semrie
Annotation Graph is free software; you can redistribute
it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.
This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.
You should have received a copy of the GNU General Public License
along with this software.  If not, see
<http://www.gnu.org/licenses/>.
**/

//
#include <ogdf/basic/PreprocessorLayout.h>
#include <ogdf/energybased/FastMultipoleEmbedder.h>
#include <ogdf/energybased/multilevel_mixer/BarycenterPlacer.h>
#include <ogdf/energybased/multilevel_mixer/EdgeCoverMerger.h>
#include <ogdf/energybased/multilevel_mixer/ModularMultilevelMixer.h>
#include <ogdf/energybased/multilevel_mixer/ScalingLayout.h>
#include <ogdf/energybased/multilevel_mixer/SolarMerger.h>
#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/layered/LongestPathRanking.h>
#include <ogdf/layered/BarycenterHeuristic.h>
#include <ogdf/layered/FastHierarchyLayout.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/energybased/multilevel_mixer/LocalBiconnectedMerger.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/packing/ComponentSplitterLayout.h>
#include <ogdf/packing/TileToRowsCCPacker.h>
#include <rapidjson/document.h>

using namespace ogdf;

namespace annotation_graph {
    template<class T>
    static MultilevelBuilder *getDoubleFactoredZeroAdjustedMerger() {
        T *merger = new T();
        merger->setFactor(2.0);
        merger->setEdgeLengthAdjustment(0);
        return merger;
    }

    static InitialPlacer *getBarycenterPlacer() {
        BarycenterPlacer *placer = new BarycenterPlacer();
        placer->weightedPositionPriority(true);
        return placer;
    }

    static void configureNiceLayout(ScalingLayout *sl, MultilevelBuilder *&merger, InitialPlacer *&placer) {
        // The EdgeCoverMerger is used for the coarsening phase.
        merger = getDoubleFactoredZeroAdjustedMerger<EdgeCoverMerger>();
        // The BarycenterPlacer is used for the placement.
        placer = getBarycenterPlacer();

        // Postprocessing is applied at each level after the single level layout.
        // In this example a FastMultipoleEmbedder with zero iterations is used for postprocessing.
        sl->setExtraScalingSteps(0);
        // No scaling is done. It is fixed to factor 1.
        sl->setScalingType(ScalingLayout::ScalingType::RelativeToDrawing);
        sl->setScaling(3.0, 8.0);
    }
    static void configureNoTwistLayout(ScalingLayout *sl, MultilevelBuilder *&merger, InitialPlacer *&placer)
    {
        // The LocalBiconnectedMerger is used for the coarsening phase.
        // It tries to keep biconnectivity to avoid twisted graph layouts.
        merger = getDoubleFactoredZeroAdjustedMerger<LocalBiconnectedMerger>();
        // The BarycenterPlacer is used for the placement.
        placer = getBarycenterPlacer();

        // Postprocessing is applied at each level after the single level layout.
        // It is turned off  in this example.
//        sl->setExtraScalingSteps(1);
        // The ScalingLayout is used to scale with a factor between 5 and 10
        // relative to the edge length.
        sl->setScalingType(ScalingLayout::ScalingType::RelativeToDesiredLength);
        sl->setScaling(5.0, 10.0);
    }

    static void hierarchyLayout(GraphAttributes &GA) {
        // TODO try different layout algorithms
        SugiyamaLayout SL;
        SL.setRanking(new OptimalRanking);
        SL.setCrossMin(new MedianHeuristic);

        OptimalHierarchyLayout *ohl = new OptimalHierarchyLayout;
        ohl->layerDistance(30.0);
        ohl->nodeDistance(80.0);
        ohl->weightBalancing(0.8);
        SL.setLayout(ohl);

        SL.call(GA);
    }

    static void multiLevelLayout(GraphAttributes &GA) {

        // Then we create a MultilevelGraph from the GraphAttributes.
        MultilevelGraph mlg(GA);

        // The FastMultipoleEmbedder is used for the single level layout.
        FastMultipoleEmbedder *fme = new FastMultipoleEmbedder();
        // It will use 1000 iterations at each level.
        fme->setNumIterations(1000);
        fme->setRandomize(true);
        fme->setNumberOfThreads(3);
        fme->setDefaultEdgeLength(25.0);
        // To minimize dispersion of the graph when more nodes are added, a
        // ScalingLayout can be used to scale up the graph on each level.
        ScalingLayout *sl = new ScalingLayout();
        sl->setLayoutRepeats(1);
        // The FastMultipoleEmbedder is nested into this ScalingLayout.
        sl->setSecondaryLayout(fme);

        // Set the merger and placer according to the wanted configuration.
        MultilevelBuilder *merger;
        InitialPlacer *placer;
        configureNiceLayout(sl, merger, placer);

        // Then the ModularMultilevelMixer is created.
        ModularMultilevelMixer *mmm = new ModularMultilevelMixer;
        mmm->setLayoutRepeats(1);
        // The single level layout, the placer and the merger are set.
        mmm->setLevelLayoutModule(sl);
        mmm->setInitialPlacer(placer);
        mmm->setMultilevelBuilder(merger);
        mmm->setAllEdgeLengths(50.0);

        // Since energy-based algorithms are not doing well for disconnected
        // graphs, the ComponentSplitterLayout is used to split the graph and
        // computation is done separately for each connected component.
        ComponentSplitterLayout *csl = new ComponentSplitterLayout;
        // The TileToRowsPacker merges these connected components after computation.
        TileToRowsCCPacker *ttrccp = new TileToRowsCCPacker;
        csl->setPacker(ttrccp);
        csl->setLayoutModule(mmm);

        // At last the PreprocessorLayout removes double edges and loops.
        PreprocessorLayout ppl;
        ppl.setLayoutModule(csl);
        ppl.setRandomizePositions(true);

        ppl.call(mlg);

        // After the computation the MultilevelGraph is exported to the
        // GraphAttributes and written to disk.
        mlg.exportAttributes(GA);
    }

    static bool isGO(const Value& data){
        auto type = (string)data.GetObject()["type"].GetString();
        return type == "biologicalprocess" || type == "cellularcomponent" || type == "molecularfunction";
    }

    static bool isMain(const Value& data){
        const Value& arr =  data.GetObject()["group"];
        assert(arr.IsArray());
        for(auto& v : arr.GetArray()){
            if((string)v.GetString() == "main")
                return true;
        }
        return false;
    }
}

