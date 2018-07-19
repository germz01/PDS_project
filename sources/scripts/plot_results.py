import matplotlib.pyplot as plt
import pandas as pd

paths = [['../../results/performance_laptop.csv',
          '../../results/performance_xeon.csv'],
         ['../../results/performance_fastflow_laptop.csv',
          '../../results/performance__fastflow_xeon.csv']]

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
        plt.ylabel(field + ' (SECONDS)')
        plt.grid()
        plt.legend()
        plt.tight_layout()
        plt.savefig('../../report/imgs/' + field.replace(' ', '_').lower() +
                    '_standard.pdf' if 'fastflow' not in l[0] else
                    '../../report/imgs/' + field.replace(' ', '_').lower() +
                    'fastflow.pdf',
                    format='pdf', bbox_inches='tight')
        plt.clf()

# for e in ['standard', 'fastflow']:
#     performance_f = ['performance_laptop.csv', 'performance_xeon.csv'] if \
#         e is 'standard' else ['performance_fastflow_laptop.csv',
#                               'performance_fastflow_xeon.csv']

#     for f in performance_f:
#         with open('../../results/' + f) as csvfile:
#             reader = csv.DictReader(csvfile, delimiter=',')
#             machine = f.split('_')[1].split('.')[0] if e is 'standard' else \
#                 f.split('_')[2].split('.')[0]

#             for row in reader:
#                 if e is 'standard':
#                     performance['parallelism degree ' + machine]. \
#                         append(row['PARALLELISM DEGREE'])
#                     performance['completion time ' + machine]. \
#                         append(row['COMPLETION TIME'])
#                     performance['average latency ' + machine]. \
#                         append(row['AVERAGE LATENCY'])
#                     performance['scalability ' + machine].append(
#                         row['SCALABILITY'])
#                 else:
#                     performance_fastflow['parallelism degree ' + machine]. \
#                         append(row['PARALLELISM DEGREE'])
#                     performance_fastflow['completion time ' + machine]. \
#                         append(row['COMPLETION TIME'])
#                     performance_fastflow['average latency ' + machine]. \
#                         append(row['AVERAGE LATENCY'])
#                     performance_fastflow['scalability ' + machine].append(
#                         row['SCALABILITY'])

# for parameter in ['completion time', 'average latency', 'scalability']:
#     for e in ['standard', 'fastflow']:
#         for machine in ['laptop', 'xeon']:
#             if e is 'standard':
#                 plt.plot([int(pd) for pd in performance['parallelism degree '
#                          + machine]],
#                          [float(p) for p in performance[parameter + ' ' +
#                           machine]], '.-' if machine == 'laptop' else '.--',
#                          color='black', label=machine.upper())
#                 plt.xticks([int(pd) for pd in performance['parallelism degree '
#                             + machine]],
#                            performance['parallelism degree ' + machine])
#             else:
#                 plt.plot([int(pd) for pd in performance_fastflow[
#                          'parallelism degree ' + machine]],
#                          [float(p) for p in performance_fastflow[parameter +
#                           ' ' + machine]],
#                          '.-' if machine == 'laptop' else '.--',
#                          color='black', label=machine.upper())
#                 plt.xticks([int(pd) for pd in performance_fastflow[
#                            'parallelism degree ' + machine]],
#                            performance['parallelism degree ' + machine])
#         plt.title(parameter.upper() + ' PER PARALLELISM DEGREE')
#         plt.xlabel('PARALLELISM DEGREE')
#         plt.ylabel(parameter.upper() + ' (SECONDS)')
#         plt.grid()
#         plt.legend()
#         plt.tight_layout()
#         plt.savefig('../../report/imgs/' + parameter.replace(' ', '_') +
#                     '_' + e + '.pdf',
#                     format='pdf', bbox_inches='tight')
#         plt.clf()
