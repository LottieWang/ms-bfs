import subprocess
import os

CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))



if __name__ == '__main__':
    BFStype = ['naive','noqueue','scbfs','parabfs','8','16','32','64','128','256']
    bWidth = [1,2,4,8,16,32,64]
    # threads=[1,2,4,8,32,64]
    for bfs in BFStype:
        print(f"------------{bfs}---------------")
        for b in bWidth:
            print(f"$$$$$$$$$$$$$ {b} bWidth $$$$$$$$$$$$$$")
            subprocess.call(f"echo '$$$$$$$$$$$$$ {b} bWidth $$$$$$$$$$$$$$' >> a.txt", shell=True)
            mfs_bfs = f"./runBencher test_queries/ldbc10k.txt 1 8 {bfs} {b} >> a.txt"
            subprocess.call(mfs_bfs, shell=True)
    