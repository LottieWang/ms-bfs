//Copyright (C) 2014 by Manuel Then, Moritz Kaufmann, Fernando Chirigati, Tuan-Anh Hoang-Vu, Kien Pham, Alfons Kemper, Huy T. Vo
//
//Code must not be used, distributed, without written consent by the authors
#include "include/tokenizer.hpp"
#include "include/graph.hpp"
#include "include/io.hpp"
#include <fstream>

#include <algorithm>

using namespace std;

GraphData GraphData::loadFromPath(const std::string& edgesFile) {
   // Count number of persons (excluding header line)
   size_t numEdges = io::fileLines(edgesFile)-1;
   LOG_PRINT("[LOADING] Number of edges: "<<numEdges);


   // Load edges data from file
   LOG_PRINT("[LOADING] Loading edges from file: "<< edgesFile);
   io::MmapedFile file(edgesFile, O_RDONLY);

   Tokenizer tokenizer(file.mapping, file.size);
   tokenizer.skipLine(); // Skip header line

   vector<NodePair> edges;
   edges.reserve(numEdges);

   std::unordered_map<uint64_t,uint64_t> nodeRenaming;
   std::unordered_map<uint64_t,uint64_t> revNodeRenaming;
   uint64_t nextNodeId=0;

   auto mapExternalNodeId = [&nodeRenaming, &revNodeRenaming, &nextNodeId](uint64_t id) -> uint64_t {
      if(nodeRenaming.find(id)==nodeRenaming.end()) {
         nodeRenaming[id] = nextNodeId;
         revNodeRenaming[nextNodeId] = id;
         return nextNodeId++;
      } else {
         return nodeRenaming[id];
      }
   };

   while(!tokenizer.isFinished()) {
      assert(edges.size()<numEdges*2);
      NodePair pair;
      pair.idA = tokenizer.readId('|');
      pair.idB = tokenizer.readId('\n');
      if(pair.idA == pair.idB) {
         continue; //No self-edges
      }

      //Add undirected
      edges.push_back(pair);
      edges.push_back(NodePair(pair.idB, pair.idA));
   }

   // Reading edges
   LOG_PRINT("[LOADING] Read edges");
   std::sort(edges.begin(), edges.end(), [](const NodePair& a, const NodePair& b) {
      return a.idA<b.idA||(a.idA==b.idA && a.idB<b.idB);
   });

   //Remove duplicates
   vector<NodePair> uniqueEdges(edges.size());
   size_t e=0;
   NodePair last(numeric_limits<uint64_t>::max(), numeric_limits<uint64_t>::max());
   for(const NodePair& a : edges) {
      if(last.idA!=a.idA || last.idB!=a.idB) {
         last = a;
         uniqueEdges[e++] = NodePair(mapExternalNodeId(a.idA), a.idB);
      }
   }
   uniqueEdges.resize(e);
   for(NodePair& a : uniqueEdges) {
      a.idB = mapExternalNodeId(a.idB);
   }
   LOG_PRINT("[LOADING] Sorted edges");

   LOG_PRINT("[LOADING] Number of nodes: "<<nextNodeId);

   return GraphData(nextNodeId, move(uniqueEdges), move(revNodeRenaming));
}


GraphData GraphData::loadBinaryFromPath(const std::string& edgesFile) {
    std::ifstream ifs(edgesFile);
    if (!ifs.is_open()) {
      cerr << "Error: Cannot open file " << edgesFile << '\n';
      abort();
    }
    size_t n, m, sizes;
    ifs.read(reinterpret_cast<char*>(&n), sizeof(size_t));
    ifs.read(reinterpret_cast<char*>(&m), sizeof(size_t));
    ifs.read(reinterpret_cast<char*>(&sizes), sizeof(size_t));
    assert(sizes == (n + 1) * 8 + m * 4 + 3 * 8);

    std::vector<uint64_t> offset(n + 1);
    std::vector<uint32_t> edge(m);
    ifs.read(reinterpret_cast<char*>(offset.data()), (n + 1) * 8);
    ifs.read(reinterpret_cast<char*>(edge.data()), m * 4);
    if (ifs.peek() != EOF) {
      cerr << "Error: Bad data\n";
      abort();
    }
    ifs.close();

    vector<NodePair> edges;
    edges.reserve(m);
 
    
    for (uint64_t i = 0; i< n; i++){
        for (uint64_t j = offset[i]; j<offset[i+1]; j++){
            edges.push_back(NodePair(i,edge[j]));
        }
    }

    std::unordered_map<uint64_t,uint64_t> revNodeRenaming;
    for (uint64_t i = 0; i<n; i++){
        revNodeRenaming[i]=i;
    }

    LOG_PRINT("[LOADING] Number of nodes: "<<n);
    
    return GraphData(n, move(edges), move(revNodeRenaming));
 }
 