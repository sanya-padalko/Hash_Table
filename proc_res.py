from math import sqrt
from sys import argv

def calc_error(a):
	a = a[3:]
	a.sort()

	b = a[1:6]
	for i in range(5):
		print(f"${b[i]}$, ", end=' ')
	print()

	avg = sum(b) / len(b)

	print(f"Average: ${avg}$")
	sum_d = 0
	for i in range(7):
		print(f"| {i + 1} | {a[i]} |")
		sum_d += (a[i] - avg) ** 2

	std = sqrt(sum_d / 7)
	print(f"Relative error: {std / avg * 100}%")

with open('results/table_results.csv', 'r') as file:
	all_results = [int(line.strip()) for line in file if line.strip()]

group_cnt = int(argv[1])
groups	  = [all_results[i : i + 10] for i in range(0, 10 * group_cnt, 10)]

for i in range(group_cnt):
	print("-" * 70)
	print(f"Group {i + 1}:")
	calc_error(groups[i])
	print("-" * 70 + '\n')