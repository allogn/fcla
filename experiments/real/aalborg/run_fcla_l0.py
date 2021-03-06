#/usr/local/bin/python
import os
import os.path as path
import subprocess
import datetime

root_path = os.environ['FCLA_ROOT']
data_path = os.environ['DATA_PATH']
expdir = 'pure_aalborg'
total = len(os.listdir(path.join(data_path,'real',expdir)))
count = 0
for f in os.listdir(path.join(data_path,'real',expdir)):
    if f[-4:] != '.ntw':
        continue
    count += 1
    full_target_path = path.join(data_path,'real',expdir,f)
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
    #if (sources != 16384 and sources != 2048):
     #   continue
    facilities = int(sources*0.1)
    capacity = 20

    p = subprocess.Popen([path.join(root_path,'bin','fcla'),'-i',
			  full_target_path,
			  '-c',str(capacity),
			  '-n',str(facilities),
			  '-o',path.join(data_path,'real',expdir,'solutions','fcla_l0',id + "fac" + str(facilities) + ".json")],
			  stdout=subprocess.PIPE,
			  stderr=subprocess.PIPE)
    p.wait()
    out, err = p.communicate()
    if len(out) + len(err) > 0:
	print(out, err)
