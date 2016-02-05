/********************************************************************
ugraph.hpp
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#ifndef UGRAPH_HPP
#define UGRAPH_HPP

//C library includes
#include <cstdlib>
#include <cstring>
#include <cassert>

//C++ library includes
#include <map>

//Bitfield defines
#define UGRAPH_SUBGPH_OPT_DEPTH 1
#define UGRAPH_SUBGPH_OPT_THRESHOLD 2
#define UGRAPH_SUBGPH_OPT_DEPTHHOLD 3
#define UGRAPH_SUBGPH_OPT_REMOVE 4

template <class T, class K>
class ugraph{

    public:

        //constructors and destructors
        ugraph();

        ugraph(size_t num_nodes);

        ~ugraph();

        ugraph(const ugraph<T,K> & rhs);

        //overloaded operators
        ugraph<T,K>& operator=(const ugraph<T,K> & rhs);

        //all other methods
        void resize(size_t num_nodes);

        size_t size(){

            return numNodes;

        }

        const K* adjPtr(){

            return adjMat;

        }

        const std::map<T,size_t>& keyIndexRef(){

            return keyIndex;

        }

        void addEdge(const T & node1, const T & node2, const K & link);
        void removeEdge(const T & node1, const T & node2);
        void removeNode(const T & node);

        void getStrongestSubgraph(ugraph<T,K> & subgraph,
                                  const K & threashold,
                                  const size_t depth,
                                  const unsigned char opt);

        //for debug
        std::pair<T,T> strongestCouple();

    private:

        //internal maintenance
        void clean();
        void init(size_t num_nodes);
        void copyMatRows(K* dest, K* source, size_t dest_cols, size_t source_cols);
        void resizeAdjMat(size_t num_nodes);
        void resizeAvailIndices(size_t num_nodes);

        size_t getIndex(const T & node1); //lookup and possibly create node
        typename std::map<T,size_t>::iterator findKey(size_t index); //reverse lookup node

        //for searching
        std::pair<size_t,size_t> findStrongestEdge(); //type K must have comparison operator

        //recursive function that traverses the links
        void findEdges(const T & this_node,
                       const K & threshold,
                       size_t depth,
                       bool* visited_nodes,
                       ugraph<T,K> & subgraph,
                       const unsigned char opt);

        //member data
        size_t numNodes;
		
        std::map<T,size_t> keyIndex; //map node data to index
        bool* availIndices; //array indicating what indices are available for new nodes
        size_t availIndex; //current lowest index for new node

        K* adjMat; //pointer to adjacency matrix

};

//default constructor
template <class T, class K>
ugraph<T,K>::ugraph(){

    init(0);
    
}

//overloaded constructor
template <class T, class K>
ugraph<T,K>::ugraph(size_t num_nodes){

    init(num_nodes);

}

//default destructor
template <class T, class K>
ugraph<T,K>::~ugraph(){

    clean();

}

//default copy constructor
template <class T, class K>
ugraph<T,K>::ugraph(const ugraph<T,K> & rhs){

    init(0);

    *this = rhs;

}

//assignment operator
template <class T, class K>
ugraph<T,K>& ugraph<T,K>::operator=(const ugraph<T,K> & rhs){

    clean();

    this->init(rhs.numNodes);

    size_t numBytes = sizeof(K)*this->numNodes*this->numNodes;

    memcpy(this->adjMat,rhs.adjMat,numBytes);

    this->keyIndex = rhs.keyIndex;

    return *this;

}

//internal maintenance methods

template <class T, class K>
void ugraph<T,K>::clean(){

    if(adjMat){

        free(adjMat);
	adjMat = NULL;

        free(availIndices);
        availIndices = NULL;

        keyIndex.clear();

    }

}

template <class T, class K>
void ugraph<T,K>::init(size_t num_nodes){


    if(num_nodes == 0){

        adjMat = NULL; //indicate memory is unallocated
        numNodes = 0;

        availIndices = NULL; //indicate memory is unallocated
        availIndex = 0;

    }else{

        //There's (num_nodes)^2 possible connections
        size_t numBytes = sizeof(K)*num_nodes*num_nodes;

        adjMat = (K*)malloc(numBytes); //create adjacency matrix
        memset(adjMat,0,numBytes);
        numNodes = num_nodes;

        availIndices = (bool*)malloc(sizeof(bool)*numNodes); //create index list
        memset(availIndices,1,sizeof(bool)*numNodes);
        availIndex = 0;

    }

    keyIndex.clear();

}

template <class T, class K>
void ugraph<T,K>::copyMatRows(K* dest, K* source, size_t dest_cols, size_t source_cols){

    size_t i, num_elements;
    if(dest_cols < source_cols) num_elements = dest_cols;
    else num_elements = source_cols;

    for(i=0;i<num_elements;++i){ //copy row by row

        memcpy(&dest[dest_cols*i],&source[source_cols*i],sizeof(K)*num_elements);

    }

}

template <class T, class K>
void ugraph<T,K>::resizeAdjMat(size_t num_nodes){

    K* newMat = (K*)malloc(sizeof(K)*num_nodes*num_nodes);

    memset(newMat,0,sizeof(K)*num_nodes*num_nodes);

    if(adjMat){

        copyMatRows(newMat,adjMat,num_nodes,numNodes);

        free(adjMat);

    }

    adjMat = newMat;

}

template <class T, class K>
void ugraph<T,K>::resizeAvailIndices(size_t num_nodes){

    size_t num_elements;
    if(num_nodes < numNodes) num_elements = num_nodes;
    else num_elements = numNodes;

    bool* newIndices = (bool*)malloc(sizeof(bool)*num_nodes);

    memset(newIndices,1,sizeof(bool)*num_nodes);

    if(availIndices){

        memcpy(newIndices,availIndices,sizeof(bool)*num_elements);

        free(availIndices);

    }

    availIndices = newIndices;

}

template <class T, class K>
size_t ugraph<T,K>::getIndex(const T & node){

    size_t index;

    typename std::map<T,size_t>::iterator it = keyIndex.find(node);

    if(it == keyIndex.end()){ //if node does not exist
        
        if(availIndex >= numNodes){

            availIndex = numNodes;
            resize(numNodes+1);

        }

        keyIndex[node] = availIndex;
        index = availIndex;
        availIndices[availIndex]= false; //no longer available

        //find next available index
        while(!availIndices[availIndex] && availIndex < numNodes) ++availIndex;

    }else{ //if it exists, grab it's index

        index = it->second;

    }

    return index;

}

template <class T, class K>
typename std::map<T,size_t>::iterator ugraph<T,K>::findKey(size_t index){

    typename std::map<T,size_t>::iterator it;

    for(it=keyIndex.begin();it!=keyIndex.end();++it){

        if(it->second == index) break; //found the index

    }

    return it;

}

//all other public methods

template <class T, class K>
void ugraph<T,K>::resize(size_t num_nodes){

    if(num_nodes == numNodes) return; //if same size, do nothing

    resizeAdjMat(num_nodes); //resize adjacency matrix

    resizeAvailIndices(num_nodes); //resize the list of available indices

    if(num_nodes < numNodes){ //remove entries from the key-index map if necessary

        for(size_t i=num_nodes;i<numNodes;++i){

            typename std::map<T,size_t>::iterator it = findKey(i);
            if(it!=keyIndex.end()) keyIndex.erase(it);

        }

    }

    numNodes = num_nodes;

    //find next available index
    availIndex = 0;
    while(!availIndices[availIndex] && availIndex < numNodes) ++availIndex;

    assert(numNodes >= keyIndex.size());
    assert(availIndex <= numNodes);

}

template <class T, class K>
void ugraph<T,K>::addEdge(const T & node1, const T & node2, const K & edge){

    size_t i = getIndex(node1);

    size_t j = getIndex(node2);

    adjMat[numNodes*i+j] = edge;
    adjMat[numNodes*j+i] = edge;

}

template <class T, class K>
void ugraph<T,K>::removeEdge(const T & node1, const T & node2){

    size_t i = getIndex(node1);

    size_t j = getIndex(node2);

    memset(&adjMat[numNodes*i+j],0,sizeof(K));
    memset(&adjMat[numNodes*j+i],0,sizeof(K));

}

template <class T, class K>
void ugraph<T,K>::removeNode(const T & node){

    size_t i = getIndex(node);
    size_t j;

    memset(&adjMat[numNodes*i],0,sizeof(K)*numNodes); //zero out the row links

    for(j=0;j<numNodes;++j){

        memset(&adjMat[numNodes*j+i],0,sizeof(K)); //zero out the column links

    }

    keyIndex.erase(node);

    availIndices[i] = true;

    if(i<availIndex) availIndex=i; //next available index should be the lowest

}

//for searching
template <class T, class K>
std::pair<size_t,size_t> ugraph<T,K>::findStrongestEdge(){

    size_t i, j;

    //the strongest link at beginning is the first
    K strongestEdge = adjMat[0];
    std::pair<size_t,size_t> edgeIndices(0,0);

    for(i=0;i<numNodes;++i){

        for(j=0;j<numNodes;++j){

            size_t pos = numNodes*i+j;
            if(adjMat[pos] > strongestEdge){ //type K must have comparison defined

                strongestEdge = adjMat[pos]; //keep track of the link's value

                //register the indices
                edgeIndices.first = i;
                edgeIndices.second = j;

            }

        }

    }

    return edgeIndices;

}

template <class T, class K>
void ugraph<T,K>::findEdges(const T & this_node,
                            const K & threshold,
                            size_t depth,
                            bool* visited_nodes,
                            ugraph<T,K> & subgraph,
                            const unsigned char opt)
{

    size_t i, j;
    size_t node_index = getIndex(this_node);
    typename std::map<T,size_t>::iterator it;

    visited_nodes[node_index] = true;

    for(i=0;i<numNodes;++i){

        if(i == node_index) continue; //there should be no node connections to itself

        K edge = adjMat[numNodes*node_index + i];

        bool descend = true;

        if(opt & UGRAPH_SUBGPH_OPT_DEPTH && depth == 0)            descend = false; //if we're deep enough

        if(opt & UGRAPH_SUBGPH_OPT_THRESHOLD && edge <= threshold) descend = false; //if it's not strong enough

        if(descend){ 

            it = findKey(i);
            subgraph.addEdge(this_node,it->first,edge);

            //if the node has not already been visited, go there
            if(!visited_nodes[i]) findEdges(it->first,threshold,depth-1,visited_nodes,subgraph,opt);

        }

    }

    if(opt & UGRAPH_SUBGPH_OPT_REMOVE) removeNode(this_node);

}

template <class T, class K>
void ugraph<T,K>::getStrongestSubgraph(ugraph<T,K> & subgraph,
                                       const K & threshold,
                                       const size_t depth,
                                       const unsigned char opt)
{

    //we use this array to keep track of which nodes have been visted during traversal
    bool* visited_nodes = (bool*)malloc(sizeof(bool)*numNodes);
    memset(visited_nodes,0,sizeof(bool)*numNodes);

    std::pair<size_t,size_t> indices;
    typename std::map<T,size_t>::iterator it1, it2;

    indices = findStrongestEdge(); //first find the "strongest link" to start with

    //retrieve the nodes' data
    it1 = findKey(indices.first);
    it2 = findKey(indices.second);

    //retrieve their edge
    K edge = adjMat[numNodes*it1->second + it2->second];

    bool descend = true;

    if(opt & UGRAPH_SUBGPH_OPT_DEPTH && depth == 0)            descend = false; //if we're deep enough

    if(opt & UGRAPH_SUBGPH_OPT_THRESHOLD && edge <= threshold) descend = false; //if it's not strong enough

    if(descend){ 

        //add them to subgraph
        subgraph.addEdge(it1->first,it2->first,edge);

        //start the traversal
        findEdges(it1->first,threshold,depth-1,visited_nodes,subgraph,opt);

        findEdges(it2->first,threshold,depth-1,visited_nodes,subgraph,opt);

        if(opt & UGRAPH_SUBGPH_OPT_REMOVE){

            removeNode(it1->first);
            removeNode(it2->first);

        }

    }

    free(visited_nodes);

}

template <class T, class K>
std::pair<T,T> ugraph<T,K>::strongestCouple(){

    std::pair<size_t,size_t> indices;
    typename std::map<T,size_t>::iterator it;
    std::pair<T,T> couple;

    indices = findStrongestEdge();

    it = findKey(indices.first);
    couple.first = it->first;

    it = findKey(indices.second);
    couple.second = it->first;

    return couple;

}

#endif
