import argparse
import csv
import subprocess

parser = argparse.ArgumentParser(description="This script collects data" +
                                 " about the execution time of the main " +
                                 "program.")
parser.add_argument('-l', '--loop', type=int,
                    help='Number of iterations the main program' +
                    ' have to be executed.')
args = vars(parser.parse_args())

with open('../results/performance.csv', 'w') as csvfile:
    fieldnames = ['PARALLELISM DEGREE', 'COMPLETION TIME', 'OVERHEAD TIME']

    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()

    for pd in range(1, args['loop'] + 1):
        out = subprocess.check_output(['./main', '../imgs',
                                       '../watermark.jpg', str(pd),
                                       '../output_dir'])
        out = out.split('\n')
        out = [item for item in out if item != '']

        pd = out[0].replace(' ', '').split(':')[1]
        ct = out[1].replace(' SECONDS', '').replace(' ', '').split(':')[1]
        ot = out[2].replace(' SECONDS', '').replace(' ', '').split(':')[1]

        writer.writerow({'PARALLELISM DEGREE': pd, 'COMPLETION TIME': ct,
                         'OVERHEAD TIME': ot})
