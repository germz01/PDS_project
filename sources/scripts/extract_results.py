import argparse
import csv
import os
import subprocess

standard = '../'
fastflow = '../sources_ff'
start = 1

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

    fieldnames = ['PARALLELISM DEGREE', 'COMPLETION TIME']

    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()

    for pd in range(start, args['loop'] + 1):
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

        pd = out[0].replace(' ', '').split(':')[1]
        ct = out[1].replace(' SECONDS', '').replace(' ', '').split(':')[1]
        # ot = out[2].replace(' SECONDS', '').replace(' ', '').split(':')[1]

        writer.writerow({'PARALLELISM DEGREE': pd, 'COMPLETION TIME': ct})
