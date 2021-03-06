//
// Created by Alvis Logins on 17/02/2017.
//

#ifndef FCLA_NETWORK_H
#define FCLA_NETWORK_H

#include <unistd.h>
#include <string>
#include <igraph/igraph.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <time.h>
#include "exceptions.h"

class Network {
public:
    igraph_t graph;
    std::string id;
    std::vector<long> weights;
    std::vector<long> source_indexes;
    std::vector<long> target_indexes;
    std::vector<long> target_capacities;
    std::vector<std::pair<double,double>> coords; //in case there are coordinates

    static std::string generate_id() {
        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);
        srand (spec.tv_nsec);
        std::string strtime = std::to_string(spec.tv_sec) + std::to_string(spec.tv_nsec);
        return strtime.substr(0, strtime.size()-3) + std::to_string(rand() % 1000);
    }

    Network(std::string filename, std::string facilityfilename = "") {
        this->load(filename, facilityfilename);
    }
    Network(igraph_t* g,
            std::vector<long>& weights,
            std::vector<long>& source_indexes,
            std::vector<Coords>& coords) {
        igraph_copy(&this->graph, g);
        this->weights = weights;
        this->source_indexes = source_indexes;
        this->coords = coords;

        //generate unique id
        this->id = this->generate_id();
    }
    Network(igraph_t* g,
            std::vector<long>& weights,
            std::vector<long>& source_indexes) {
        igraph_copy(&this->graph, g);
        this->weights = weights;
        this->source_indexes = source_indexes;
        std::vector<std::pair<double, double>> v(weights.size());
        this->coords = v;

        //generate unique id
        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);
        srand (spec.tv_nsec);
        std::string strtime = std::to_string(spec.tv_sec) + std::to_string(spec.tv_nsec);
        this->id = strtime.substr(0, strtime.size()-3) + std::to_string(rand() % 1000);
    }
    ~Network() {
        igraph_destroy(&this->graph);
    }

    long graph_size() {
        igraph_integer_t vcount = igraph_vcount(&this->graph);
        return static_cast<long>(vcount);
    }

    long number_of_customers() {
        return source_indexes.size();
    }

    void save(std::string dir, std::string filename) {
        std::ofstream outf(filename,std::ios::out);
        outf << this->id << " "
             << igraph_vcount(&graph) << " "
             << igraph_ecount(&graph) << " "
             << source_indexes.size() << "\n";
        for (long i = 0; i < igraph_ecount(&graph); i++) {
            igraph_integer_t from, to;
            igraph_edge(&graph, i, &from, &to);
            outf << from << " " << to << " " << weights[i] << "\n";
        }
        for (long i = 0; i < source_indexes.size(); i++) {
            outf << source_indexes[i] << "\n";
        }
        for (long i = 0; i < igraph_vcount(&graph); i++) {
            outf << coords[i].first << " " << coords[i].second << "\n";
        }
        outf.close();
    }

    void save(std::string dir) {
        std::string filename = dir + '/' + this->id + ".ntw";
        this->save(dir, filename);
    }

    void load(std::string filename, std::string target_list_filename = "") {
        std::ifstream infile(filename, std::ios::in);
        if (!infile) {
            throw std::invalid_argument("Input file does not exist");
        }
        long vcount, ecount, source_num;
        infile >> this->id >> vcount >> ecount >> source_num;
        igraph_empty(&graph, vcount, false);
        igraph_vector_t edges;
        igraph_vector_init(&edges, ecount*2);
        weights.clear();
        weights.reserve(ecount);
        for (long i = 0; i < ecount; i++) {
            long from, to, weight, capacity;
            infile >> from >> to >> weight;
            VECTOR(edges)[2*i] = from;
            VECTOR(edges)[2*i + 1] = to;
            weights.push_back(weight);
        }
        igraph_add_edges(&this->graph, &edges, 0);

        igraph_bool_t check_multiple;
        igraph_has_multiple(&this->graph, &check_multiple);
        if (check_multiple) {
            std::cout << "Graph has multiple edges" << std::endl; //@todo move this
            //is not allowed because in facility choser when covering is checked
            //we assume that every outgoing edge from a facility covers one unique new customer
            exit(1);
        }

        source_indexes.clear();
        source_indexes.reserve(source_num);
        for (long i = 0; i < source_num; i++) {
            long source_id;
            infile >> source_id;
            source_indexes.push_back(source_id);
        }
        for (long i = 0; i < vcount; i++) {
            double x,y;
            infile >> x >> y;
            coords.push_back(std::make_pair(x,y));
        }

        //check if consistent
//        for (long i = 0; i < ecount; i++) {
//            igraph_integer_t from;
//            igraph_integer_t to;
//            igraph_edge(&graph, i, &from, &to);
//            if ((from != VECTOR(edges)[2*i]) || (to != VECTOR(edges)[2*i+1])) {
//                throw "not valid";
//            }
//        }
        igraph_vector_destroy(&edges);
        target_indexes.clear();
        target_capacities.clear();
        if (target_list_filename != "") {
	        std::ifstream target_list_file(target_list_filename.c_str());
           //@todo wtf check for a file does not work 
            long nodeid, capacity;
            while (target_list_file >> nodeid >> capacity) {
                target_indexes.push_back(nodeid);
                target_capacities.push_back(capacity);
            }
            if (target_capacities.size() == 0) {
                std::cout << "Error file with potential facilities is empty" << std::endl;
                //throw std::string("File with potential facilities is empty");
	        }
        } else {
            target_indexes.clear();
            for (long i = 0; i < vcount; i++) {
                target_indexes.push_back(i);
            }
            target_capacities.clear();
        }
    }

    void set_target_indexes(std::vector<long> node_indexes, std::vector<long> capacities) {
        if (capacities.size() != node_indexes.size()) {
            std::length_error("Capacities for nodes should have the same length as potential facility locations.");
        }
        this->target_capacities = capacities;
        this->target_indexes = node_indexes;
    }

    void set_target_indexes(std::vector<long> node_indexes, long capacities) {
        this->target_capacities = std::vector<long>(node_indexes.size(), capacities);
        this->target_indexes = node_indexes;
    }
};

#endif //FCLA_NETWORK_H
