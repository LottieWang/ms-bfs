from graph import GRAPH_DIR
# from graph import TR_DIR
from graph import graphs
# from graph import largeD_graphs
import os
import subprocess

import subprocess
import os

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
def test_centrality(type_name, k, g, CURRENT_DIR, LOG_DIR, Closeness_DIR):
	print(f"test on {g}")
	# ground_truth = f"{TR_DIR}/{g}_sym.txt"
	test_file = f"{CURRENT_DIR}/runBencherSimple"
	OUT_DIR = f"{LOG_DIR}/msBFS_{type_name}"
	ANS_DIR = f"{Closeness_DIR}/msBFS_{type_name}"
	os.makedirs(OUT_DIR, exist_ok=True)
	os.makedirs(ANS_DIR, exist_ok=True)
	# repeat = '-t 0'
	repeat = '' 
	numa = 'numactl -i all'
	# numa = ''
	cmd = f"{numa} {test_file} {GRAPH_DIR}/{g}.bin {type_name} -k {k} -f {repeat} -out {ANS_DIR}/{g}_{k}.txt -f >> {OUT_DIR}/{g}_{k}.txt"
	subprocess.call(cmd, shell=True)
def experiment():
	CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
	LOG_DIR= f"{CURRENT_DIR}/log"
	Closeness_DIR = "/data/lwang323/MS_BFS/Closeness"
	#   LOG_DIR=f"/data/lwang323/MS_BFS/Ecc"
	os.makedirs(LOG_DIR, exist_ok=True)
	os.makedirs(Closeness_DIR, exist_ok=True)
	# type_names = ["1Phase", "2Phase", "Exact"]
	type_names = [8]
	batch_sizes = [1024, 4096]
	for k in batch_sizes:
		print(f"--------test on batch_size {k}---------")
		for type_name in type_names:
			print(f"----test on {type_name}----")
			for g in graphs:
				test_centrality(type_name, k, g, CURRENT_DIR, LOG_DIR, Closeness_DIR)

if __name__ == '__main__':
	experiment()

	