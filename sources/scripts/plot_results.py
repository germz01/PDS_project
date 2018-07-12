import csv
import matplotlib.pyplot as plt
from collections import defaultdict

performance = defaultdict(list)

for performance_f in ['performance_laptop.csv', 'performance_xeon.csv']:
    with open('../../results/' + performance_f) as csvfile:
        reader = csv.DictReader(csvfile, delimiter=',')
        machine = performance_f.split('_')[1].split('.')[0]

        for row in reader:
            performance['parallelism degree ' + machine]. \
                append(row['PARALLELISM DEGREE'])
            performance['completion time ' + machine]. \
                append(row['COMPLETION TIME'])
            performance['overhead time ' + machine]. \
                append(row['OVERHEAD TIME'])

for t in ['completion', 'overhead']:
    for machine in ['laptop', 'xeon']:
        plt.plot([int(pd) for pd in performance['parallelism degree ' +
                 machine]],
                 [float(ct) for ct in performance[t + ' time ' + machine]],
                 '.-' if machine == 'laptop' else '.--',
                 color='black',
                 label=machine.upper())
    plt.xticks([int(pd) for pd in performance['parallelism degree ' +
               machine]],
               performance['parallelism degree ' + machine])
    plt.title(t.upper() + ' TIME PER PARALLELISM DEGREE')
    plt.xlabel('PARALLELISM DEGREE')
    plt.ylabel(t.upper() + ' TIME (SECONDS)')
    plt.grid()
    plt.legend()
    plt.tight_layout()
    plt.savefig('../../report/imgs/' + t + '_time.pdf', format='pdf',
                bbox_inches='tight')
    plt.clf()
