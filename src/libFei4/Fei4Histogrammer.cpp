// #################################
// # Author: Timon Heim
// # Email: timon.heim at cern.ch
// # Project: Yarr
// # Description: Histograms Fei4 data
// # Comment: 
// ################################

#include "Fei4Histogrammer.h"

Fei4Histogrammer::Fei4Histogrammer() {

}

Fei4Histogrammer::~Fei4Histogrammer() {
    for (unsigned i=0; i<algorithms.size(); i++)
        delete algorithms[i];
}

void Fei4Histogrammer::init() {
    first = true;
}

void Fei4Histogrammer::process() {
    while (!input->empty()) {
        Fei4Data *data = input->popData();

        if (data != NULL) {
            if (first) {
                for (unsigned i=0; i<algorithms.size(); i++) {
                    curStat = data->lStat;
                    algorithms[i]->create(curStat);
                }
                first = false;
            }
            for (unsigned i=0; i<algorithms.size(); i++) {
                if (!(data->lStat == curStat)) {
                    std::cout << "Split" << std::endl;
                    this->publish();
                    curStat = data->lStat;
                    algorithms[i]->create(curStat);
                }
                algorithms[i]->processEvent(data);
            }
            delete data;
        }
    }
    for (unsigned i=0; i<algorithms.size(); i++) {
        algorithms[i]->end();
    }
}

void Fei4Histogrammer::publish() {
    for (unsigned i=0; i<algorithms.size(); i++) {
        output->pushData(algorithms[i]->getHisto());
    }
}

void Fei4Histogrammer::toFile(std::string basename) {
    for (unsigned i=0; i<algorithms.size(); i++) {
        algorithms[i]->getHisto()->toFile(basename, true);
    }
}

void Fei4Histogrammer::plot(std::string basename) {
    for (std::deque<HistogramBase*>::iterator it = output->begin(); it != output->end(); ++it) {
        std::cout << "Plotting : " << (*it)->getName() << std::endl;
        (*it)->plot(basename);
    }    
}


void OccupancyMap::processEvent(Fei4Data *data) {
    unsigned c1 = 0;
    unsigned c2 = 0;
    unsigned c3 = 0;
    for (std::list<Fei4Event>::iterator eventIt = (data->events).begin(); eventIt!=data->events.end(); ++eventIt) {   
        Fei4Event curEvent = *eventIt;
        ++c1;
        c3 += curEvent.nHits;
        for (std::vector<Fei4Hit>::iterator hitIt = curEvent.hits.begin(); hitIt!=curEvent.hits.end(); ++hitIt) {   
            Fei4Hit curHit = *hitIt;
            ++c2;
            if(curHit.tot > 0)
                h->fill(curHit.col, curHit.row);
        }
    }
    std::cout << __PRETTY_FUNCTION__ << " : " << c1 << " Events with " << c2 << " hits! Should be " << c3 << std::endl;
}

void TotMap::processEvent(Fei4Data *data) {
    for (std::list<Fei4Event>::iterator eventIt = (data->events).begin(); eventIt!=data->events.end(); ++eventIt) {   
        Fei4Event curEvent = *eventIt;
        for (std::vector<Fei4Hit>::iterator hitIt = curEvent.hits.begin(); hitIt!=curEvent.hits.end(); ++hitIt) {   
            Fei4Hit curHit = *hitIt;
            if(curHit.tot > 0)
                h->fill(curHit.col, curHit.row, curHit.tot);
        }
    }
}

void Tot2Map::processEvent(Fei4Data *data) {
    for (std::list<Fei4Event>::iterator eventIt = (data->events).begin(); eventIt!=data->events.end(); ++eventIt) {   
        Fei4Event curEvent = *eventIt;
        for (std::vector<Fei4Hit>::iterator hitIt = curEvent.hits.begin(); hitIt!=curEvent.hits.end(); ++hitIt) {   
            Fei4Hit curHit = *hitIt;
            if(curHit.tot > 0)
                h->fill(curHit.col, curHit.row, curHit.tot*curHit.tot);
        }
    }
}

void L1Dist::processEvent(Fei4Data *data) {
    // Event Loop
    for (std::list<Fei4Event>::iterator eventIt = (data->events).begin(); eventIt!=data->events.end(); ++eventIt) {   
        Fei4Event curEvent = *eventIt;
        if(curEvent.l1id != l1id) {
            l1id = curEvent.l1id;
            bcid_offset = curEvent.bcid;
        }
        int delta_bcid = curEvent.bcid - bcid_offset;
        if (delta_bcid < 0)
            delta_bcid += 0x400;
        h->fill(delta_bcid, curEvent.nHits);
    }
}
    
