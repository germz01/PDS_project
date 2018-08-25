import matplotlib.pyplot as plt
import pandas as pd

paths = [['../../results/performance_laptop.csv',
          '../../results/performance_xeon.csv'],
         ['../../results/performance_fastflow_laptop.csv',
          '../../results/performance_fastflow_xeon.csv']]

for l in paths:
    laptop = pd.read_csv(l[0])
    xeon = pd.read_csv(l[1])
    fieldnames = list(laptop.columns)

    for field in fieldnames[1:]:
        for file in [laptop, xeon]:

            plt.plot(list(file['PARALLELISM DEGREE']),
                     list(file[field]),
                     '.-' if file is laptop else '.--', color='black',
                     label='LAPTOP' if file is laptop else 'XEON')
            plt.xticks(list(file['PARALLELISM DEGREE']),
                       [str(i) for i in list(file['PARALLELISM DEGREE'])])

        plt.title(field + ' PER PARALLELISM DEGREE')
        plt.xlabel('PARALLELISM DEGREE')
        plt.ylabel(field + r' ($\mu$ SECONDS)')
        plt.grid()
        plt.legend()
        plt.tight_layout()
        plt.savefig('../../report/imgs/' + field.replace(' ', '_').lower() +
                    '_standard.pdf' if 'fastflow' not in l[0] else
                    '../../report/imgs/' + field.replace(' ', '_').lower() +
                    '_fastflow.pdf',
                    format='pdf', bbox_inches='tight')
        plt.clf()
