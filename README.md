# About

This is the experimental framework used to evaluate the **MS-BFS** algorithm (proposed in the VLDB 2015 paper [The More the Merrier: Efficient Multi-Source Graph Traversal](http://www.vldb.org/pvldb/vol8/p449-then.pdf)) and its related work.

The code computes the top-k closeness centrality values for the vertices in a given graph using different BFS algorithms.

Following are the instructions to compile and run the source code.

# Build
`./compile`

In order to compare the correctness with my code, use gcc version >10.


Tested on Ubuntu 14.04 using GCC 4.8.2.

# Usage
`./runBencher [nRun] [nThreads] [BFSType] [bWidth] (nSources) (force)`

- `nRun`:     Number of execution
- `nThreads`: Number of threads
- `BFSType`:  Type of BFSs
  - MS-BFS variant values: 16, 32, 64, 128, 256 (e.g. 128 executes MS-BFS using SSE registers)
  - Related work values: naive (textbook BFS), noqueue (textbook BFS based on bit fields), scbfs (direction-optimized BFS), parabfs (parallel BFS)
- `bWidth`:   Number of registers that are used per vertex for MS-BFS, e.g. 4 with the BFSType 128 runs 512 concurrent BFSs
- `nSources`: (optional) Number of source vertices for which the closeness centrality values are computed. If omitted, all vertices are used
- `force`:    (optional) Set to 'f' in order to force the execution even for high parallelism on few source vertices

# Examples
- `./runBencher test_queries/ldbc10k.txt 3 8 naive 1 20 f`
- `./runBencher test_queries/ldbc10k.txt 1 32 256 2` (only works when compiled for the architecture core-avx2)

# Team

- [Manuel Then](http://www-db.in.tum.de/~then/) (Technische Universität München)
- [Moritz Kaufmann](http://www-db.in.tum.de/~kaufmann/) (Technische Universität München)
- [Fernando Chirigati](http://bigdata.poly.edu/~fchirigati/) (New York University)
- [Tuan-Anh Hoang-Vu](http://bigdata.poly.edu/~tuananh/) (New York University)
- [Kien Pham](http://bigdata.poly.edu/~kienpham/) (New York University)
- [Alfons Kemper](http://www3.in.tum.de/~kemper/) (Technische Universität München)
- [Thomas Neumann](http://www-db.in.tum.de/~neumann/) (Technische Universität München)
- [Huy T. Vo](http://serv.cusp.nyu.edu/~hvo/) (New York University)
