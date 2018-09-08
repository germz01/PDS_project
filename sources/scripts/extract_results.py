from __future__ import division
from collections import defaultdict
import argparse
import csv
import os
import platform
import subprocess

standard = '../'
fastflow = '../sources_ff'
start = 0
t_par_1 = 0.0
t_seq = 0.0


def check_path(possible_path):
    if os.path.exists(possible_path):
        return possible_path
    else:
        raise argparse.ArgumentTypeError(possible_path + ' IS A NOT VALID')


parser = argparse.ArgumentParser(description="This script collects data" +
                                 " about the execution time of the main " +
                                 "program.",
                                 formatter_class=argparse.
                                 ArgumentDefaultsHelpFormatter)
parser.add_argument('-e', '--executable', type=str, choices=['standard',
                    'fastflow'], default='standard',
                    help='Which executable should be tested. Default is')
parser.add_argument('-i', '--imagedir', type=check_path,
                    default='../imgs', help='The path leading to the '
                    'directory containing the images.')
parser.add_argument('-w', '--watermark', type=check_path,
                    default='../watermark.jpg', help='The path leading to the'
                    ' watermark file.')
parser.add_argument('-o', '--outputdir', type=str,
                    default='../output_dir', help='The path leading to the'
                    ' output directory')
parser.add_argument('-l', '--loop', type=int, default=10,
                    help='Number of iterations the main program' +
                    ' have to be executed.')
parser.add_argument('-d', '--delay', type=int, default=100,
                    help='The delay represents the time that the emitter have'
                    ' to wait before sending a new image into the stream.')
parser.add_argument('-n', '--name', type=str,
                    help='Name of the file in which the results will be '
                    'saved.')
parser.add_argument('-s', '--standard', type=str, default='../../results/',
                    help='The ABSOLUTE PATH leading to the csv file '
                    'containing the informations about the standard execution')
args = vars(parser.parse_args())

to_open = '../../results' + args['name'] + '.csv' if \
    os.path.exists('../../results') else './' + args['name'] + '.csv'

with open(to_open, 'w') as csvfile:
    if args['executable'] == 'standard':
        os.chdir(standard)
    else:
        os.chdir(fastflow)
        start += 1

    fieldnames = ['PARALLELISM DEGREE', 'COMPLETION TIME', 'MEAN LATENCY',
                  'MEAN LOADING TIME', 'MEAN SAVING TIME',
                  'MEAN CREATION TIME', 'SPEEDUP', 'SCALABILITY', 'EFFICIENCY']

    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()

    for pd in range(start, args['loop'] + 1):
        row = defaultdict(float)

        print 'TESTING FOR PARALLELISM DEGREE ' + str(pd)

        cmd = ['./main', args['imagedir'], args['watermark'], str(pd),
               args['outputdir'], str(args['delay'])]

        out = subprocess.check_output(cmd)
        out = out.split('\n')
        out = [item for item in out if item != '']

        row['pd'] = float(pd)
        row['ct'] = float(out[1].split(':')[1].split(' ')[1])
        row['al'] = float(out[2].split(':')[1].split(' ')[1])
        row['avg_load'] = float(out[3].split(':')[1].split(' ')[1])
        row['avg_save'] = float(out[4].split(':')[1].split(' ')[1])

        if pd == 0:
            t_seq = row['ct']
        else:
            if pd == 1:
                t_par_1 = row['ct']

                if args['executable'] == 'fastflow':
                    f = None

                    if os.path.exists('../../results'):
                        if platform.system() == 'Darwin':
                            f = open('../../results/performance_laptop.csv')
                        else:
                            f = open('../../results/performance_xeon.csv')
                    else:
                        f = open(args['standard'])

                    reader = csv.DictReader(f)
                    t_seq = float(reader.next()['COMPLETION TIME'])

            row['avg_creation'] = float(out[5].split(':')[1].split(' ')[1])
            row['scalability'] = t_par_1 / row['ct']
            row['speedup'] = t_seq / row['ct']
            row['efficiency'] = row['speedup'] / row['pd']

        writer.writerow({'PARALLELISM DEGREE': row['pd'],
                         'COMPLETION TIME': row['ct'],
                         'MEAN LATENCY': row['al'],
                         'MEAN LOADING TIME': row['avg_load'],
                         'MEAN SAVING TIME': row['avg_save'],
                         'MEAN CREATION TIME': row['avg_creation'],
                         'SPEEDUP': row['speedup'],
                         'SCALABILITY': row['scalability'],
                         'EFFICIENCY': row['efficiency']})
