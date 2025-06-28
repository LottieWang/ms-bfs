//Copyright (C) 2014 by Manuel Then, Moritz Kaufmann, Fernando Chirigati, Tuan-Anh Hoang-Vu, Kien Pham, Alfons Kemper, Huy T. Vo
//
//Code must not be used, distributed, without written consent by the authors
#include "include/bench.hpp"
#include "include/TraceStats.hpp"
#include "parseCommandLine.h"
#include <fstream>
#include <iomanip>

#define GEN_BENCH_BRANCH(X,CTYPE,WIDTH) \
   X(batchType==sizeof(CTYPE)*8&&batchWidth==WIDTH) { \
    std::cout << "batchType: " << batchType << " CTYPE: " << TypeName<CTYPE>::name() << " sizeof(CTYPE): " <<  sizeof(CTYPE) << " batchWidth: " << WIDTH << std::endl; \
      bencher = new SpecializedBFSBenchmark<Query4::HugeBatchBfs<CTYPE,WIDTH,false>>("BatchBFS "+std::to_string(sizeof(CTYPE)*8)+" ("+std::to_string(WIDTH)+")"); \
      maxBatchSize = sizeof(CTYPE)*8*WIDTH; \
      bfsType = std::to_string(sizeof(CTYPE)*8)+"_"+std::to_string(WIDTH); \
   } \

template <typename T>
struct TypeName {
    static std::string name() { return "Unknown"; }
};

template <>
struct TypeName<uint8_t> {
    static std::string name() { return "uint8_t"; }
};

template <>
struct TypeName<uint16_t> {
    static std::string name() { return "uint16_t"; }
};

template <>
struct TypeName<uint32_t> {
    static std::string name() { return "uint32_t"; }
};

template <>
struct TypeName<uint64_t> {
    static std::string name() { return "uint64_t"; }
};

template <>
struct TypeName<__m128i> {
    static std::string name() { return "__m128i"; }
};

template <>
struct TypeName<__m256i> {
    static std::string name() { return "__m256i"; }
};

template <typename Out>
void split(const std::string& s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
      *result++ = item;
    }
}
  
vector<Query4::PersonId> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    vector<Query4::PersonId> seeds;
    for (size_t i = 0; i < elems.size(); i++) {
      seeds.push_back(stoi(elems[i]));
    }
    return seeds;
}

vector<Query4::PersonId> loadSource(const std::string& file) {
    std::ifstream fin(file);
    // int k=200;
    size_t r;
    string pattern = "seeds:";
    string strLine;
    string line;
    if (!getline(fin, line)){
        cerr<<"Can not read source file " << file << std::endl;
        abort();
    }
    auto seeds = split(line, ' ');
    LOG_PRINT("[LOADING] Loading sources with size "<< seeds.size());
    return seeds;
  }

int main(int argc, char** argv) {
   auto usage = "Usage: runBencherSimple <filename> <BFSType> <numSources> -W <bWidth> -t <repeat>  -out <outfile> -f ";
   CommandLine P(argc, argv);

   if (argc < 4) {
    cerr << "Usage: " << usage << std::endl;
    abort();
   }
   
   size_t numThreads = std::thread::hardware_concurrency()/2;
   printf("hardware_concurrency: %d\n",std::thread::hardware_concurrency());
   

   LOG_PRINT("[Main] Using "<< numThreads <<" threads");

    // Load input graphs and sources
    std::string graphFile = argv[1];
    //  std::string sourceFile = argv[3];
    auto personGraph = Graph<Query4::PersonId>::loadFromPath(graphFile);
    //  auto sources = loadSource(sourceFile);

    size_t bfsLimit = atoi(argv[3]);
    int numRuns=P.getOptionInt("-t", 3);
    char* outFile = P.getOptionValue("-out");
    if(bfsLimit>personGraph.size()) {
      bfsLimit=personGraph.size();
    }

   size_t maxBatchSize;
   BFSBenchmark* bencher;
   std::string bfsType;
   Query4::PARABFSRunner::setThreadNum(numThreads);//Using threads inside BFS
   if(std::string(argv[2])=="naive") {
      bencher = new SpecializedBFSBenchmark<Query4::BFSRunner>("BFSRunner");
      maxBatchSize = 1;
      bfsType = "naive";
   } else if(std::string(argv[2])=="noqueue") {
      bencher = new SpecializedBFSBenchmark<Query4::NoQueueBFSRunner>("NoQueueBFSRunner");
      maxBatchSize = 1;
      bfsType = "noqueue";
   } else if(std::string(argv[2])=="scbfs") {
      bencher = new SpecializedBFSBenchmark<Query4::SCBFSRunner>("SCBFSRunner");
      maxBatchSize = 1;
      bfsType = "scbfs";
   } else if(std::string(argv[2])=="parabfs") {
      bencher = new SpecializedBFSBenchmark<Query4::PARABFSRunner>("PARABFSRunner");
      maxBatchSize = 1;
      bfsType = "parabfs";
      numThreads = 1;
   } else {
      const int batchType = std::stoi(std::string(argv[2]));
    //   const int batchWidth = std::stoi(std::string(argv[5]));
      const int batchWidth = P.getOptionInt("-W", 1);
      GEN_BENCH_BRANCH(if,__m128i,8)
      GEN_BENCH_BRANCH(else if,__m128i,4)
      GEN_BENCH_BRANCH(else if,__m128i,1)
      #ifdef AVX2
      GEN_BENCH_BRANCH(else if,__m256i,2)
      GEN_BENCH_BRANCH(else if,__m256i,1)
      #endif
      GEN_BENCH_BRANCH(else if,uint64_t,8)
      GEN_BENCH_BRANCH(else if,uint64_t,1)
      GEN_BENCH_BRANCH(else if,uint32_t,16)
      GEN_BENCH_BRANCH(else if,uint32_t,1)
      GEN_BENCH_BRANCH(else if,uint16_t,32)
      GEN_BENCH_BRANCH(else if,uint16_t,1)
      GEN_BENCH_BRANCH(else if,uint8_t,64)
      GEN_BENCH_BRANCH(else if,uint8_t,1) 
      else {
         exit(-1);
      }

      bool checkNumTasks =! P.getOption("-f");
    
      
      if(checkNumTasks)
      {
         auto ranges = generateTasks(bfsLimit, personGraph.size(), maxBatchSize);
         auto desiredTasks=numThreads*3;
         if(ranges.size()<desiredTasks) {
            FATAL_ERROR("[Main] Not enough tasks! #Threads="<<numThreads<<", #Tasks="<<ranges.size()<<", #DesiredTasks="<<desiredTasks<<", #maxBatchSize="<<maxBatchSize);
         } 
      }
   }

    // Allocate additional worker threads
    Workers workers(numThreads-1);

      // Run benchmark
      std::cout<<"# Benchmarking "<<bencher->name<<" ... "<<std::endl<<"# ";
      LOG_PRINT("[Main] bfsLimit " << bfsLimit);
      vector<double> closeness(bfsLimit,0.0);
      vector<Query4::PersonId> sources(bfsLimit);

      for(int i=0; i<numRuns; i++) {
         bencher->initTrace(personGraph.numVertices, personGraph.numEdges, numThreads, bfsLimit, bfsType);
         bencher->runSimple(bfsLimit, personGraph, closeness, workers, sources);

         std::cout<<bencher->lastRuntime()<<"ms ";
         std::cout.flush();
      }
      std::cout<<std::endl;

      std::cout<<bencher->getMinTrace()<<std::endl;
    
      if (P.getOption("-out")) {
         std::ofstream fout(outFile);
         if (!fout.is_open()) {
            std::cerr << "Error opening output file: " << outFile << std::endl;
            abort();
         }
         for (int i = 0; i < bfsLimit; i++) {
            fout << sources[i] << ": " << std::fixed << std::setprecision(10) << closeness[i] << std::endl;
         }
         fout.close();
      }
    //  Print the sources and corresponding closeness centrality 
      // for (int i = 0;i<bfsLimit; i++){
      //   printf("%u: %lf\n",sources[i], closeness[i]);
      // }

   workers.close();

   if(bfsType=="parabfs") {
     Query4::PARABFSRunner::finish();
   }
   return 0;
}
