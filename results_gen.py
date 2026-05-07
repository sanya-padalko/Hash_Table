from math import sqrt
from sys import argv
import sys

def calc_error(a, warm_cnt, test_cnt):
	a = a[warm_cnt:]
	a.sort()

	b = a[1:test_cnt - 1]

	avg = sum(b) / len(b)
	sum_d = 0
	print("| Номер замера | Среднее количество тиков |")
	print("| :---: | :---: |")
	for i in range(test_cnt):
		print(f"| {i + 1} | {a[i]} |")
		sum_d += (a[i] - avg) ** 2
	print("")

	print(f"Результаты без минимума и максимума:", end=' ')
	for i in range(test_cnt - 2):
		if i != test_cnt - 3:
			print(f"${b[i]}$", end=', ')
		else:
			print(f"${b[i]}$\n")

	std = sqrt(sum_d / test_cnt)
	err = round(std / avg * 100, 2)
	avg = round(avg)
	print(f"""Среднее количество тиков на <i>TableFind</i>: ${round(avg)} 
													\\pm {round(avg * err / 100)}$\n""")
	print(f"Относительная погрешность: ${err:.2f}\\%$\n")

	return [avg, err]


def print_begin(test_ind):
	if (test_ind == 0):
		print("## -O3:\n")

	elif (test_ind == 1):
		print("## Замена strcmp на my_strcmp из ассемблерного файла\n")
		print("```")
		with open('src/my_strcmp.s', 'r') as strcmp:
			for c in strcmp.read():
				print(c, end='') 
		print("\n```\n")

	elif (test_ind == 2):
		print("## Замена crc32 на intrinsic-и\n")
		print("```c")
		with open('src/_opt_crc32.cpp', 'r') as crc:
			for c in crc.read():
				print(c, end='') 
		print("\n```\n")
	
	elif (test_ind == 3):
		print("Рассмотрим ассемблерный вид <i>TableFind</i> c godbolt:\n")
		print("```")
		with open('src/find_godbolt.s', 'r') as table_find:
			for c in table_find.read():
				print(c, end='') 
		print("\n```\n")

		print("## Ассемблерная вставка, проверяющая первые символы строк\n")

		print("```c")
		with open('src/find_opt.cpp', 'r') as opt_find:
			for c in opt_find.read():
				print(c, end='') 
		print("\n```\n")
	
	return


def print_calc(avg, last_avg, err, last_err, add_str="относительно -O3"):
	boost	= round(((last_avg - avg) / last_avg) * 100, 2)
	rel_err = round(sqrt(last_err * last_err + err * err) / 100, 4)
	abs_err = round(boost * rel_err, 2)
	print(f"""на $\\frac{{{last_avg} - {avg}}}{{{last_avg}}} * 100 =
			{boost:.2f}\\% \\pm {abs_err:.2f}$ $\\%$ {add_str}.\n""")
	print(f"""Относительная погрешность ускорения: 
			$\\sqrt{{{last_err:.2f}^2+{err:.2f}^2}} =
			{round(rel_err * 100, 2):.2f}\\%$\n""")
	print(f"""Абсолютная погрешность: ${boost:.2f}\\% * {rel_err:.4f} = 
			{abs_err:.2f}\\%$\n""")

	return


def print_end(test_ind, last_avg, last_err, base_avg, base_err):
	if (test_ind == 0):
		with open('results/_base_res.txt', 'w') as base_res:
			print(f"{avg}\n{err}", file=base_res)

		print("![До оптимизаций](../images/base.png)\n")

	elif (test_ind == 1):
		last_avg = base_avg
		last_err = base_err
		print(f"Получили ускорение", end=' ')
		print_calc(avg, last_avg, err, last_err)

		print("![1-я оптимизация](../images/opt_strcmp.png)\n")

	elif (test_ind == 2):
		print(f"Полученное ускорение:\n")
		print(f"$\\cdot$", end=' ')
		print_calc(avg, last_avg, err, last_err, "относительно предыдущей версии")

		last_avg = base_avg
		last_err = base_err
		print(f"\n$\\cdot$", end=' ')
		print_calc(avg, last_avg, err, last_err)

		print("![2-я оптимизация](../images/opt_crc32.png)\n")

	elif (test_ind == 3):
		print(f"Полученное ускорение:\n")
		print(f"$\\cdot$", end=' ')
		print_calc(avg, last_avg, err, last_err, "относительно предыдущей версии")

		last_avg = base_avg
		last_err = base_err
		print(f"\n$\\cdot$", end=' ')
		print_calc(avg, last_avg, err, last_err)

		print("![3-я оптимизация](../images/opt_assem.png)")
	
	with open('results/_last_res.txt', 'w') as save_res:
		print(f"{avg}\n{err}", file=save_res)
	
	return


test_ind  = int(argv[2])
group_cnt = int(argv[1])
test_cnt  = int(argv[3])
warm_cnt  = int(argv[4])

all_cnt = warm_cnt + test_cnt

with open('results/table_results.csv', 'r') as file:
	all_results = [int(line.strip()) for line in file if line.strip()]

groups	  = [all_results[i : i + all_cnt] for i in range(0, all_cnt * group_cnt, all_cnt)]

last_avg = 0
base_avg = 0
last_err = 0
base_err = 0

if (test_ind > 0):
	with open('results/_base_res.txt', 'r') as base:
		base_avg = int(base.readline())
		base_err = float(base.readline())
if (test_ind > 1):
	with open('results/_last_res.txt', 'r') as last:
		last_avg = int(last.readline())
		last_err = float(last.readline())

with open('results/all_res.md', 'a') as res_file:
	sys.stdout = res_file
	print_begin(test_ind)

	avg = 0
	err = 0
	for i in range(group_cnt):
		res = calc_error(groups[i], warm_cnt, test_cnt)
		avg = res[0]
		err = res[1]
	
	print_end(test_ind, last_avg, last_err, base_avg, base_err)
