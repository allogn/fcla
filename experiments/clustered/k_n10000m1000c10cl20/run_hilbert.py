#/usr/local/bin/python
import os
import os.path as path
import subprocess
import datetime

root_path = os.environ['FCLA_ROOT']
data_path = os.environ['DATA_PATH']
expdir = 'k_n10000m1000c10cl20'
total = len(os.listdir(path.join(data_path,'clustered',expdir)))
count = 0
for f in os.listdir(path.join(data_path,'clustered',expdir)):
    if f[-4:] != '.ntw':
        continue
    count += 1
    full_target_path = path.join(data_path,'clustered',expdir,f)
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

    capacity = 10

    for fac in range(200,1000,100):    		  
        print(fac)
        p = subprocess.Popen([path.join(root_path,'bin','hilbertsolver'),
	   		  '-c',str(capacity),
			  '-n',str(fac),
			  '-o',path.join(data_path,'clustered',expdir,'solutions','hilbert',id + "_" + str(fac) + ".json"),
                          '-i',full_target_path],
			  stdout=subprocess.PIPE,
			  stderr=subprocess.PIPE)
        p.wait()
        out, err = p.communicate()
        if len(out) + len(err) > 0:
            print(out, err)
