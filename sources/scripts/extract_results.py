from __future__ import division
from collections import defaultdict
import argparse
import csv
import os
import subprocess

standard = '../'
fastflow = '../sources_ff'
start = 1
t_par_1 = 0.0

parser = argparse.ArgumentParser(description="This script collects data" +
                                 " about the execution time of the main " +
                                 "program.")
parser.add_argument('-e', '--executable', type=str, choices=['standard',
                    'fastflow'], default='standard',
                    help='Which executable should be tested.')
parser.add_argument('-l', '--loop', type=int,
                    help='Number of iterations the main program' +
                    ' have to be executed.')
parser.add_argument('-n', '--name', type=str,
                    help='Name of the file in which the results will be saved')
args = vars(parser.parse_args())

with open('../../results/' + args['name'] + '.csv', 'w') as csvfile:
    if args['executable'] == 'standard':
        os.chdir(standard)
    else:
        os.chdir(fastflow)
        start += 1

    fieldnames = ['PARALLELISM DEGREE', 'COMPLETION TIME', 'AVERAGE LATENCY',
                  'AVERAGE LOADING TIME', 'AVERAGE SAVING TIME',
                  'AVERAGE CREATION TIME', 'SPEEDUP', 'EFFICIENCY']

    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()

    for pd in range(start, args['loop'] + 1):
        row = defaultdict(float)

        print 'TESTING FOR PARALLELISM DEGREE ' + str(pd)

        cmd = list()

        if args['executable'] == 'standard':
            cmd = ['./main', '../imgs', '../watermark.jpg', str(pd),
                   '../output_dir']
        else:
            cmd = ['./main', '../../imgs', '../../watermark.jpg', str(pd),
                   '../../output_dir']

        out = subprocess.check_output(cmd)
        out = out.split('\n')
        out = [item for item in out if item != '']

        row['pd'] = float(pd)
        row['ct'] = float(out[1].replace(' SECONDS', '').replace(' ', '').
                          split(':')[1])
        row['al'] = float(out[2].replace(' SECONDS', '').replace(' ', '').
                          split(':')[1])
        row['avg_load'] = float(out[3].replace(' SECONDS', '').
                                replace(' ', '').split(':')[1])
        row['avg_save'] = float(out[4].replace(' SECONDS', '').
                                replace(' ', '').split(':')[1])

        if pd != 1:
            row['avg_creation'] = float(out[5].replace(' SECONDS', '').
                                        replace(' ', '').split(':')[1])

        if pd == 1 or (pd == 2 and args['executable'] == 'fastflow'):
            t_par_1 = row['ct']

        row['speedup'] = t_par_1 / row['ct']
        row['efficiency'] = row['speedup'] / row['pd']

        writer.writerow({'PARALLELISM DEGREE': row['pd'],
                         'COMPLETION TIME': row['ct'],
                         'AVERAGE LATENCY': row['al'],
                         'AVERAGE LOADING TIME': row['avg_load'],
                         'AVERAGE SAVING TIME': row['avg_save'],
                         'AVERAGE CREATION TIME': row['avg_creation'],
                         'SPEEDUP': row['speedup'],
                         'EFFICIENCY': row['efficiency']})
