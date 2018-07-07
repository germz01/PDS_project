import csv
import subprocess

out = subprocess.check_output(['./main', '../imgs', '../watermark.jpg', '1',
                               '../output_dir'])

with open('../results/performance.csv', 'w') as csvfile:
    fieldnames = ['PARALLELISM DEGREE', 'COMPLETION TIME', 'OVERHEAD TIME']

    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()

    for pd in range(1, 21):
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
