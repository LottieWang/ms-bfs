//Copyright (C) 2014 by Manuel Then, Moritz Kaufmann, Fernando Chirigati, Tuan-Anh Hoang-Vu, Kien Pham, Alfons Kemper, Huy T. Vo
//
//Code must not be used, distributed, without written consent by the authors
#include "tokenizer.hpp"
#include "graph.hpp"
#include "../query4.hpp"
#include "bfs/noqueue.hpp"
#include "bfs/sc2012.hpp"
#include "io.hpp"
#include "worker.hpp"
#include "TraceStats.hpp"
#include "bfs/parabfs.hpp"
#include "bfs/batch64.hpp"

struct Query {
   uint64_t numNodes;
   std::string dataset;
   std::string reference;
};

struct Queries {
   std::vector<Query> queries;

   static Queries loadFromFile(const std::string& queryFile) {
      io::MmapedFile file(queryFile, O_RDONLY);

      Queries queries;

      Tokenizer tokenizer(file.mapping, file.size);
      // Read queries
      while(!tokenizer.isFinished()) {
         Query query;
         query.numNodes = tokenizer.readId(' ');
         query.dataset = tokenizer.readStr(' ');
         query.reference = tokenizer.readStr('\n');
         if(query.reference.size()==0) {
            FATAL_ERROR("[Queries] Could not load reference result, is it specified?");
         }
         queries.queries.push_back(query);
      }

      LOG_PRINT("[Queries] Loaded "<< queries.queries.size()<< " queries.");
      return queries;
   }
};

struct BFSBenchmark {
   const std::string name;
   std::vector<uint32_t> runtimes;

   BFSBenchmark(std::string name)
      : name(name)
   { }
   virtual ~BFSBenchmark() { }

   uint32_t lastRuntime() const {
      return runtimes.back();
   }

   uint32_t avgRuntime() const {
      uint32_t sum=0;
      for(auto t : runtimes) {
         sum += t;
      }
      return sum / runtimes.size();
   }

   virtual void run(const uint32_t k, const Query4::PersonSubgraph& subgraph, const string& referenceResult, Workers& workers, uint64_t maxBfs) = 0;

   virtual void runSimple(const uint32_t k, const Query4::PersonSubgraph& subgraph, std::vector<double>& closeness, Workers& workers)=0;

   virtual size_t batchSize() = 0;

   virtual void initTrace(size_t numVertices, size_t numEdges, size_t numThreads, size_t maxBfs, std::string bfsType) = 0;

   virtual std::string getMinTrace() = 0;
};

template<typename BFSRunnerT>
struct SpecializedBFSBenchmark : public BFSBenchmark {
   #ifdef STATISTICS
   Query4::BatchStatistics statistics;
   #endif

   std::vector<std::string> traces;

   typedef TraceStats<BFSRunnerT::TYPE_BITS*BFSRunnerT::WIDTH> RunnerTraceStats;

   SpecializedBFSBenchmark(std::string name)
      : BFSBenchmark(name)
   { }
   virtual void run(const uint32_t k, const Query4::PersonSubgraph& subgraph, const string& referenceResult, Workers& workers, uint64_t maxBfs) override {
      uint64_t runtime;
      LOG_PRINT("run SpecializedBFSBenchmark ");
      std::string result = runBFS<BFSRunnerT>(k, subgraph, workers, maxBfs, runtime
      #ifdef STATISTICS
         ,statistics
         #endif
         );
      if(maxBfs == std::numeric_limits<uint64_t>::max() && result != referenceResult) {
         cout<<endl;
         FATAL_ERROR("[Query] Wrong result, expected ["<<referenceResult<<"], got ["<<result<<"]");
      }
      runtimes.push_back(runtime);

      RunnerTraceStats& stats = RunnerTraceStats::getStats();
      traces.push_back(stats.print(runtime));
   }
   // k is the number of sources to run BFS on
   virtual void runSimple(uint32_t k, const Query4::PersonSubgraph& subgraph, std::vector<double>& closeness, Workers& workers) override{
      uint64_t runtime;
      LOG_PRINT("run SpecializedBFSBenchmarkSimple ");
      runBFS<BFSRunnerT>(subgraph, k, closeness, workers, runtime);
      runtimes.push_back(runtime);
      RunnerTraceStats& stats = RunnerTraceStats::getStats();
      traces.push_back(stats.print(runtime));
   }

   virtual size_t batchSize() {
      return BFSRunnerT::batchSize();
   }

   virtual void initTrace(size_t numVertices, size_t numEdges, size_t numThreads, size_t maxBfs, std::string bfsType) {
      RunnerTraceStats& stats = RunnerTraceStats::getStats(true);
      // TODO: Account for overriding of batch size by env variale
      stats.init(numVertices, numEdges, batchSize(), BFSRunnerT::TYPE, BFSRunnerT::TYPE_BITS, BFSRunnerT::WIDTH, numThreads, maxBfs, bfsType);
   }

   virtual std::string getMinTrace() {
      size_t minRuntimeIx=0;
      for (int i = 0; i < runtimes.size(); ++i) {
         if(runtimes[i]<runtimes[minRuntimeIx]) {
            minRuntimeIx=i;
         }
      }

      return traces[minRuntimeIx];
   }
};
