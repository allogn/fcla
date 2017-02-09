//
// Created by allogn on 24.01.17.
//

#ifndef FCLA_SIA_H
#define FCLA_SIA_H

#include <string>
#include <igraph/igraph.h>
#include "nheap.h"
#include "helpers.h"
#include "EdgeGenerator.h"

namespace SIA {

    long INF = LONG_MAX;
    typedef fHeap<long> mmHeap;

    /*
     * Get Cost of an Edge according to potential values of vertices
     *
     * input: vtype is an indicator which subset a vertex belongs to
     */
    inline long edgeCost(long weight,
                         long source_potential,
                         long target_potential)
    {
        //do not invert weight of an edge, because inverted edges are explicitly added to a graph
        return weight - source_potential + target_potential;
    };

    /*
     * Get enheaped cost of an edge
     */
    inline long heapedCost(long edge_w,
                           long source_potential, //potential of source node of an edge
                           long source_mindist) // minimum distance to a source node
    {
        return source_mindist + edge_w - source_potential;
    }

    inline bool ifValidTarget(long distance, mmHeap* gheap) {
        return distance < gheap->getTopValue();
    }

    /*
     * Update vector with minimum distance from a source node (Dijkstra execution)
     */
    inline bool updateMindist(igraph_vector_long_t* mindist,
                              igraph_integer_t v_id,
                              long new_distance)
    {
        long cur_dist = igraph_vector_long_e(mindist, v_id);
        if (cur_dist > new_distance) {
            igraph_vector_long_set(mindist, v_id, new_distance);
            return true;
        }
        return false;
    }

    /*
     * Initialize vectors and variables for Dijkstra
     */
    void init_dijsktra(long vcount,
                       mmHeap* dheap,
                       igraph_vector_long_t* mindist,
                       igraph_vector_t* backtrack,
                       igraph_integer_t source_id)
    {
        //initialize heaps and vectors according to a source node
        igraph_vector_long_init(mindist, vcount);
        igraph_vector_long_fill(mindist, INF);
        VECTOR(*mindist)[source_id] = 0;

        igraph_vector_init(backtrack, vcount);
        igraph_vector_fill(backtrack, -1); //init backtrack with -1 and itself for source
        igraph_vector_set(backtrack, source_id, source_id);

        //enqueue first node into Dijktra heap
        dheap->enqueue(source_id, 0);
    }

    /*
     * Continue running Dijkstra based on existing minimum distances and heap
     * Return the closest non-full service or -1 if there is no path
     */
    igraph_integer_t dijkstra(igraph_t* bigraph,
                              mmHeap* dheap,
                              mmHeap* gheap,
                              newEdges& nearest_edges,
                              igraph_vector_long_t* mindist, //distance from a source node
                              igraph_vector_long_t* edge_excess, //amount of "free" flow = capacity - actual flow
                              igraph_vector_long_t* node_excess,
                              igraph_vector_long_t* potentials,
                              igraph_vector_long_t* weights,
                              igraph_vector_t* backtrack //store ids of an ancestor in a dijkstra tree
    )
    {
        long current_node; //must be of type "long" instead "igraph_integer_t". Otherwise change mmHeap type (idx is always of long type)
        while(dheap->dequeue(current_node) != 0) //heap is not empty
        {
            //if current node is of second type and is not full - return it
            //the degree of outgoing edges is how many customers the service is matched with
            if (VECTOR(*node_excess)[current_node] > 0) {
                //enheap back the last node in order to return the same (best) result
                dheap->enqueue(current_node, VECTOR(*mindist)[current_node]);
                return current_node; //already contains correct path distance in both backtrack and mindist arrays
            }
            //update minimum distances to all neighbors
            igraph_vector_t eids;
            igraph_vector_init(&eids,0);
            igraph_incident(bigraph, &eids, current_node, IGRAPH_OUT);
            for (igraph_integer_t i = 0; i < igraph_vector_size(&eids); i++) {
                igraph_integer_t eid = VECTOR(eids)[i];
                //check if the current edge is full then do not consider it
                if (VECTOR(*edge_excess)[eid] == 0) continue;
                //otherwise update distance to target node
                igraph_integer_t target_node, source_node;
                igraph_edge(bigraph,eid,&source_node,&target_node);
                long new_cost = VECTOR(*mindist)[source_node];
                new_cost += edgeCost(VECTOR(*weights)[eid],
                                     VECTOR(*potentials)[source_node],
                                     VECTOR(*potentials)[target_node]);
                if (updateMindist(mindist, target_node, new_cost)) {
                    //update breadcrumbs
                    igraph_vector_set(backtrack, target_node, source_node);
                    //update or enqueue target node
                    dheap->updateorenqueue(target_node, new_cost);
                    //maintain global heap (value for target_node was changed because of mindist
                    newEdge nearest_edge = nearest_edges[target_node];
                    long new_edge_cost = heapedCost(nearest_edge.weight, VECTOR(*potentials)[target_node], new_cost);
                    if (nearest_edge.exists) gheap->updateorenqueue(target_node, new_edge_cost);
                }
            }
            igraph_vector_destroy(&eids);
        }
        return -1; //no path exist
    }

    /*
     * Add a new edge to a residual bipartite graph and update dijkstra heap accordingly
     */
    void addNewEdge(igraph_t* bigraph,
                    mmHeap* dheap,
                    newEdge new_edge,
                    igraph_vector_long_t* weights,
                    igraph_vector_long_t* excess,
                    igraph_vector_long_t* potentials,
                    igraph_vector_long_t* mindist)
    {
        //add a new edge
        igraph_add_edge(bigraph, new_edge.source_node, new_edge.target_node);
        igraph_vector_long_push_back(weights, new_edge.weight);
        igraph_vector_long_push_back(excess, new_edge.capacity);
        //add an inverted edge to a bigraph
        igraph_add_edge(bigraph, new_edge.target_node, new_edge.source_node);
        igraph_vector_long_push_back(weights, -new_edge.weight);
        igraph_vector_long_push_back(excess, 0);

        //updating Dijkstra heap by adding source_node to a heap:
        //note, that we don't have to check all outgoing edges, but consideration of the new edge
        //is nothing but a part of dijkstra execution starting from source node,
        //so we do a bit of overhead in order to minimize amount of code
        if (VECTOR(*mindist)[new_edge.source_node] < INF) //otherwise we will have overflow of long when calculating INF+something as new distance
            dheap->updateorenqueue(new_edge.source_node, VECTOR(*mindist)[new_edge.source_node]);
    }

    /*
     * Try to add edge from a global heap, update gheaps if succeed
     *
     * Return if something was added
     */
    bool addHeapedEdge(igraph_t* bigraph,
                       mmHeap* dheap,
                       mmHeap* gheap,
                       EdgeGenerator* edge_generator,
                       newEdges& new_edges,
                       igraph_vector_long_t* weights,
                       igraph_vector_long_t* excess,
                       igraph_vector_long_t* potentials,
                       igraph_vector_long_t* mindist)
    {
        //get the best edge from a global heap or return if the heap is empty
        long source_node;
        if (!gheap->dequeue(source_node))
            return false;
        addNewEdge(bigraph, dheap, new_edges[source_node], weights, excess, potentials, mindist);

        // update vector with next nearest weights
        newEdge next_new_edge = edge_generator->getEdge(source_node);
        new_edges[source_node] = next_new_edge;
        // enqueue the next new value in gheap
        if (next_new_edge.exists) {
            long new_heaped_value = heapedCost(next_new_edge.weight,
                                               VECTOR(*potentials)[source_node],
                                               VECTOR(*mindist)[source_node]);
            gheap->enqueue(source_node, new_heaped_value);
        }
        return true;
    }

    /*
     * Run dijkstra and enlarge graph until a valid path to non-full vertex to type B appears
     *
     * Return false if no path exists
     */
    bool findAndEnlarge(igraph_t* bigraph,
                        mmHeap* dheap,
                        mmHeap* gheap,
                        EdgeGenerator* edge_generator,
                        newEdges& nearest_edges,
                        igraph_vector_long_t* weights,
                        igraph_vector_long_t* edge_excess,
                        igraph_vector_long_t* potentials,
                        igraph_vector_long_t* node_excess,
                        igraph_vector_long_t* mindist,
                        igraph_vector_t* backtrack,
                        igraph_integer_t* result_vid
    )
    {

        igraph_integer_t target_id = -1;
        while (target_id == -1) {
            //run Dijkstra
            target_id = dijkstra(bigraph,dheap,gheap,nearest_edges,mindist,edge_excess,node_excess,potentials,weights,backtrack);

            //if current residual graph is full, then return target_id or exception
            if (target_id == -1) {
                if (!addHeapedEdge(bigraph, dheap, gheap, edge_generator, nearest_edges,
                                   weights, edge_excess, potentials, mindist)) {
                    return false;
                }
            } else {
                long sp_length = VECTOR(*mindist)[target_id];
                //check threshold
                if (!ifValidTarget(sp_length, gheap) &&
                        addHeapedEdge(bigraph, dheap, gheap, edge_generator, nearest_edges,
                                      weights, edge_excess, potentials, mindist)) {
                    target_id = -1; //invalidate result if new edge is added, otherwise return current result
                }
            }
        }
        *result_vid = target_id;
        return true;
    }

    /*
     * Update potentials for all visited nodes
     *
     * Maintain potentials so that there are no negative weights: new = old + (dist[target] - dist[current])
     * Do it for all nodes where dist < dist[target] (mindist), finding nodes by BFS
     */
    void updatePotentials(igraph_vector_long_t* mindist,
                          igraph_vector_long_t* potentials,
                          igraph_integer_t target)
    {
        long target_distance = VECTOR(*mindist)[target];
        for (long i = 0; i < igraph_vector_long_size(mindist); i++) {
            if (VECTOR(*mindist)[i] < target_distance) {
                VECTOR(*potentials)[i] = VECTOR(*potentials)[i] + target_distance - VECTOR(*mindist)[i];
            }
        }
    }

    /*
     * Change a flow (excess) in the graph from a source to a target (flip edges)
     * A path is reconstructed based on backtrack array, a source node is one that points to itself
     * - Inverted edges already must exist in the graph
     * - Ignore heap values as they will not be used anymore
     *
     */
    long augmentFlow(igraph_t* bigraph,
                     igraph_vector_t* backtrack,
                     igraph_vector_long_t* edge_excess,
                     igraph_vector_long_t* node_excess,
                     igraph_vector_long_t* potentials,
                     igraph_integer_t target)
    {
        igraph_integer_t current_node = target;
        //find minimum excess on the way and make it maximum flow increase
        long min_excess = INF;

        //path selectors
        igraph_vector_t forward_vids;
        igraph_vector_t backward_vids;
        igraph_vector_init(&backward_vids, 0);

        while (VECTOR(*backtrack)[current_node] != current_node) {
            igraph_vector_push_back(&backward_vids, current_node);
            igraph_vector_push_back(&backward_vids, VECTOR(*backtrack)[current_node]);
            current_node = VECTOR(*backtrack)[current_node];
        }
        igraph_vector_copy(&forward_vids, &backward_vids);
        igraph_vector_reverse(&forward_vids);

        igraph_es_t forward_es;
        igraph_es_t backward_es;
        igraph_es_pairs(&forward_es, &forward_vids, true);//directed graph
        igraph_es_pairs(&backward_es, &backward_vids, true);

        //iterate through forward path and find maximum flow
        igraph_eit_t eit;
        igraph_eit_create(bigraph, forward_es, &eit);
        while (!IGRAPH_EIT_END(eit)) {
            igraph_integer_t eid = IGRAPH_EIT_GET(eit);
            min_excess = min(min_excess, VECTOR(*edge_excess)[eid]);
            IGRAPH_EIT_NEXT(eit);
        }
        min_excess = min(min_excess, VECTOR(*node_excess)[target]);
        min_excess = min(min_excess, -VECTOR(*node_excess)[current_node]);

        //iterate throw both paths and change excess
        IGRAPH_EIT_RESET(eit);
        while (!IGRAPH_EIT_END(eit)) {
            igraph_integer_t eid = IGRAPH_EIT_GET(eit);
            VECTOR(*edge_excess)[eid] = VECTOR(*edge_excess)[eid] - min_excess;
            IGRAPH_EIT_NEXT(eit);
        }
        igraph_eit_destroy(&eit);
        igraph_eit_create(bigraph, backward_es, &eit);
        while (!IGRAPH_EIT_END(eit)) {
            igraph_integer_t eid = IGRAPH_EIT_GET(eit);
            VECTOR(*edge_excess)[eid] = VECTOR(*edge_excess)[eid] + min_excess;
            IGRAPH_EIT_NEXT(eit);
        }
        igraph_eit_destroy(&eit);

        //current node contains the source node after while loop, so we change node_excess
        VECTOR(*node_excess)[target] -= min_excess;
        VECTOR(*node_excess)[current_node] += min_excess;

        igraph_vector_destroy(&backward_vids);
        igraph_vector_destroy(&forward_vids);
        return min_excess;
    }


    /*
     * Find a matching for a vertex in a bipartite graph
     *
     * In fact there is one continuous Dijkstra execution that is iteratively terminated
     * or started depending on current results and a stream of new edges
     */
    long matchVertex(igraph_t* bigraph, //residual bipartite graph
                     igraph_vector_long_t* node_excess,
                     igraph_vector_long_t* edge_excess,
                     igraph_vector_long_t* weights,
                     igraph_vector_long_t* potentials,
                     igraph_integer_t source_id,
                     newEdges& nearest_edges, // global cache for storing vertex ids that correspond to nearest_weights
                     EdgeGenerator* edge_generator
    )
    {
        if (VECTOR(*node_excess)[source_id] >= 0)
            throw "Only nodes with negative excess can be matched";

//        print_graph(bigraph, weights, edge_excess);
        //init heaps
        mmHeap gheap; //global heap stores information of the next not-added neighbor of vertices
        mmHeap dheap; //heap used in Dijkstra

        igraph_vector_long_t mindist; //minimum distance in Dijstra execution
        igraph_vector_t backtrack; //ids of ancestor in Dijkstra tree
        init_dijsktra(igraph_vcount(bigraph), &dheap, &mindist, &backtrack, source_id);

        //nearest_edges array is global, but gheap is local. In order to descrease heap size we enheap
        //only those nodes which were visited by the algorithm (relevant)
        //if a node was not yet visited, then we should enheap the value from nearest_edges
        //this is done in dijkstra, because gheap is updated by the current value from nearest_edges if mindist is updated
        if (nearest_edges[source_id].exists) //otherwise all edges were already added, but everything is still fine
            gheap.enqueue(source_id, heapedCost(nearest_edges[source_id].weight, 0, 0));

        //enlarge graph until valid path is found, or throw an exception
        igraph_integer_t result_vid;
        if (!findAndEnlarge(bigraph,&dheap,&gheap,edge_generator,nearest_edges,
                            weights,edge_excess,potentials,node_excess,&mindist,&backtrack,&result_vid))
        {
            throw "No valid matching in the graph, out of additional nodes"; //no path in complete graph
        }

        long flowChange = augmentFlow(bigraph, &backtrack, edge_excess, node_excess, potentials, result_vid);
        updatePotentials(&mindist, potentials, result_vid);


//        print_graph(bigraph, weights, edge_excess);
        //free memory
        igraph_vector_long_destroy(&mindist);
        igraph_vector_destroy(&backtrack);
        return flowChange;
    }

    /*
     * Run matching algorithm on a bipartite graph with vcountA,vcountB number of vertices of two types
     *
     * No not pass bipartite graph, but a function that provides incremental edges
     */
    void minMatch(igraph_vector_long_t* node_excess,
                  EdgeGenerator* edge_generator,
                  long* result_weight,
                  std::vector<std::vector<long>>& result_matching)
    {
        long size = igraph_vector_long_size(node_excess);
        igraph_t bigraph;
        igraph_empty(&bigraph, size, true); //create directed graph

        igraph_vector_long_t potentials;
        igraph_vector_long_t weights;
        igraph_vector_long_t excess;

        igraph_vector_long_init(&potentials,size);
        igraph_vector_long_init(&weights, 0);
        igraph_vector_long_init(&excess, 0);

        //init with a first nearest neighbor for all vertices
        newEdges nearest_edges;
        for (long i = 0; i < size; i++){
            newEdge e = edge_generator->getEdge(i);
            nearest_edges.push_back(e);
        }

        long source_count;
        for (long i = 0; i < size; i++) {
            if (VECTOR(*node_excess)[i] >= 0) {
                source_count = i;
                break;
            }
        }

        long source_id = 0;
        long prev_id = -1;
        //round-robin
        while (source_id != prev_id) {
            if (VECTOR(*node_excess)[source_id] < 0) {
                matchVertex(&bigraph, node_excess, &excess, &weights, &potentials,
                            source_id, nearest_edges, edge_generator);
                prev_id = source_id;
            }
            source_id = (source_id+1) % source_count;
        }

        //arrange an answer
        *result_weight = 0;
        if (result_matching.size() != size) {
            throw "Result matching vector must be initialized with empty vectors for each vertex";
        }
        for (long i = 0; i < source_count; i++) {
            igraph_vector_t neis;
            igraph_vector_init(&neis,0);
            igraph_neighbors(&bigraph, &neis, i, IGRAPH_IN);
            igraph_integer_t matched_id;
            //there can be many matched vertices because of capacities
            for (long j = 0; j < igraph_vector_size(&neis); j++) {
                igraph_integer_t eid;
                igraph_get_eid(&bigraph, &eid, VECTOR(neis)[j], i, true, true);
                if (VECTOR(excess)[eid]>0) {
                    matched_id = VECTOR(neis)[j];
                    result_matching[i].push_back(matched_id);
                    result_matching[matched_id].push_back(i);
                    *result_weight -= VECTOR(weights)[eid];
                }
            }
            igraph_vector_destroy(&neis);
        }

        igraph_vector_long_destroy(&potentials);
        igraph_vector_long_destroy(&weights);
        igraph_vector_long_destroy(&excess);
    }

}

#endif //FCLA_SIA_H
