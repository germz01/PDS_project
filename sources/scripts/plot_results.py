import csv
import matplotlib.pyplot as plt
from collections import defaultdict

performance = defaultdict(list)

with open('../../results/performance.csv') as csvfile:
    reader = csv.DictReader(csvfile, delimiter=',')

    for row in reader:
        performance['parallelism degree'].append(row['PARALLELISM DEGREE'])
        performance['completion time'].append(row['COMPLETION TIME'])
        performance['overhead time'].append(row['OVERHEAD TIME'])

for t in ['completion', 'overhead']:
    plt.plot([int(pd) for pd in performance['parallelism degree']],
             [float(ct) for ct in performance[t + ' time']], '.-',
             color='black')
    plt.xticks([int(pd) for pd in performance['parallelism degree']],
               performance['parallelism degree'])
    plt.title(t.upper() + ' TIME PER PARALLELISM DEGREE')
    plt.xlabel('PARALLELISM DEGREE')
    plt.ylabel(t.upper() + ' TIME (SECONDS)')
    plt.grid()
    plt.tight_layout()
    plt.savefig('./' + t + '_time.pdf', format='pdf', bbox_inches='tight')
    plt.clf()
