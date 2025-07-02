import os
import sys
import subprocess
import re
import pandas as pd
import numpy as np
from graph import graphs


def collect_data(file_in, key_words):
	f = open(file_in,'r')
	res = f.read()
	f.close()
	data_lines = re.findall(f"{key_words}.*", res)
	data = list(map(lambda x: eval(x.split(" ")[-1]), data_lines))
	return data


def collect_average(file_in, key_words, repeat=4):
	f = open(file_in,'r')
	res = f.read()
	f.close()
	data_lines = re.findall(f"{key_words}.*", res)
	data = list(map(lambda x: eval(x.split(" ")[-1]), data_lines))
	data = data[1:]  # Skip the first column which is usually the graph name or similar
	data = np.mean(data)
	return data

def collect_kExact(k,w):
	print("Collecting data in kBFS 1Phase")
	LOG_DIR= f"{CURRENT_DIR}/log/msBFS_8_{w}"
	# OUT_FILE=f"{CURRENT_DIR}/../result/exp4.csv"
	data=dict()
	data["Data"]=graphs
	key_words = {
		"Select": "inner ms-BFS: selecting sources:",
		"kBFS": "ms-bfs Closeness:",
		"Total": "ms-bfs Closeness: "
	}
	# "Total": "Average Closeness Centrality:"
	for key, words in key_words.items():
		data[f"{key}"]=[]
		for g in graphs:
			log_file = f"{LOG_DIR}/{g}_{k}.txt"
			try:
				data[f"{key}"].append(collect_average(log_file, words))
			except:
				data[f"{key}"].append(np.nan)
	df = pd.DataFrame.from_dict(data)
	print(df.to_csv(index=False))
	
if __name__ == "__main__":
	CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
	w=1
	for k in [8192]:
		collect_kExact(k,w)
