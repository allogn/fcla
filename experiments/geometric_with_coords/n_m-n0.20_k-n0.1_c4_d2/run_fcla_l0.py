#/usr/local/bin/python
import os
import os.path as path
import subprocess
import datetime

root_path = os.environ['FCLA_ROOT']
data_path = os.environ['DATA_PATH']
expdir = 'n_m-n0.20_k-n0.1_c4_d2'
total = len(os.listdir(path.join(data_path,'geometric_with_coords',expdir)))
count = 0
for f in os.listdir(path.join(data_path,'geometric_with_coords',expdir)):
    if f[-4:] != '.ntw':
        continue
    count += 1
    full_target_path = path.join(data_path,'geometric_with_coords',expdir,f)
    print(" ".join((str(datetime.datetime.now()),full_target_path,"(",str(count),"/",str(total),")")))

    edges = 0
    vertices = 0
    sources = 0

    # get size of a graph and number of customers
    with open(full_target_path) as check_file:
        params = check_file.readline().split(' ')
        id = params[0]
        edges = int(params[2])
        vertices = int(params[1])
        sources = int(params[3])

    facilities = int(sources/2)
    capacity = 4

    p = subprocess.Popen([path.join(root_path,'bin','fcla'),'-i',
			  full_target_path,
			  '-c',str(capacity),
			  '-g','0',
			  '-a','1',
			  '-n',str(facilities),
			  '-o',path.join(data_path,'geometric_with_coords',expdir,'solutions','fcla_l0',id + "fac" + str(facilities) + ".json")],
			  stdout=subprocess.PIPE,
			  stderr=subprocess.PIPE)
    p.wait()
    out, err = p.communicate()
    if len(out) + len(err) > 0:
	print(out, err)
