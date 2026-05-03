from math import sqrt
from sys import argv

def calc_error(a, warm_cnt, all_cnt, res_file):
	a = a[warm_cnt:]
	a.sort()

	b = a[1:all_cnt - 1]

	avg = sum(b) / len(b)
	sum_d = 0
	print("| Номер замера | Среднее количество тиков |", file=res_file)
	print("| :---: | :---: |", file=res_file)
	for i in range(all_cnt):
		print(f"| {i + 1} | {a[i]} |", file=res_file)
		sum_d += (a[i] - avg) ** 2
	print("", file=res_file)

	print(f"Результаты без минимума и максимума:", end=' ', file=res_file)
	for i in range(all_cnt - 2):
		if i != all_cnt - 3:
			print(f"${b[i]}$", end=', ', file=res_file)
		else:
			print(f"${b[i]}$\n", file=res_file)

	print(f"Среднее количество тиков на <i>TableFind</i>: ${round(avg)}$\n", file=res_file)

	std = sqrt(sum_d / all_cnt)
	err = round(std / avg * 100, 2)
	avg = round(avg)
	print(f"Относительная погрешность: ${err:.2f}\\%$\n", file=res_file)

	return [avg, err]

with open('results/table_results.csv', 'r') as file:
	all_results = [int(line.strip()) for line in file if line.strip()]

group_cnt = int(argv[1])
groups	  = [all_results[i : i + 10] for i in range(0, 10 * group_cnt, 10)]
test_ind  = int(argv[2])

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
	if (test_ind == 0):
		print("## -O2:\n", file=res_file)

	elif (test_ind == 1):
		print("## Замена strcmp на my_strcmp из ассемблерного файла\n", file=res_file)
		print("```", file=res_file)
		with open('src/my_strcmp.s', 'r') as strcmp:
			for c in strcmp.read():
				print(c, end='', file=res_file) 
		print("\n```\n", file=res_file)

	elif (test_ind == 2):
		print("## Замена crc32 на intrinsic-и:\n", file=res_file)
		print("```c", file=res_file)
		with open('src/_opt_crc32.cpp', 'r') as crc:
			for c in crc.read():
				print(c, end='', file=res_file) 
		print("\n```\n", file=res_file)
	
	elif (test_ind == 3):
		print("Рассмотрим ассемблерный вид <i>TableFind</i> c godbolt:\n", file=res_file)
		print("```", file=res_file)
		with open('src/find_godbolt.s', 'r') as table_find:
			for c in table_find.read():
				print(c, end='', file=res_file) 
		print("\n```\n", file=res_file)

		print("## Замена TableFind на ассемблерную вставку:\n", file=res_file)

		print("```c", file=res_file)
		with open('src/find_opt.cpp', 'r') as opt_find:
			for c in opt_find.read():
				print(c, end='', file=res_file) 
		print("\n```\n", file=res_file)


	avg = 0
	err = 0
	for i in range(group_cnt):
		res = calc_error(groups[i], 3, 7, res_file)
		avg = res[0]
		err = res[1]


	if (test_ind == 0):
		with open('results/_base_res.txt', 'w') as base_res:
			print(f"{avg}\n{err}", file=base_res)
		print("![До оптимизаций](../images/base.png)\n", file=res_file)

	elif (test_ind == 1):
		last_avg = base_avg
		last_err = base_err

		boost	= round(((last_avg - avg) / last_avg) * 100, 2)
		rel_err = round(sqrt(last_err * last_err + err * err) / 100, 4)
		abs_err = round(boost * rel_err, 2)
		print(f"Получили ускорение на $\\frac{{{last_avg} - {avg}}}{{{last_avg}}} * 100 = {boost:.2f}\\% \\pm {abs_err:.2f}$ $\\%$ относительно -O3.\n", file=res_file)
		print(f"Абсолютная погрешность: ${boost:.2f}\\% * \\frac{{\\sqrt{{{last_err:.2f}^2+{err:.2f}^2}}}}{{100}} = {boost:.2f}\\% * {rel_err:.4f} = {abs_err:.2f}\\%$\n", file=res_file)
		print(f"Относительная погрешность ускорения: ${round(rel_err * 100, 2):.2f}\\%$\n", file=res_file)

		print("![1-я оптимизация](../images/opt_strcmp.png)\n", file=res_file)

	elif (test_ind == 2):
		print(f"Полученное ускорение:\n", file=res_file)
		boost	= round(((last_avg - avg) / last_avg) * 100, 2)
		rel_err = round(sqrt(last_err * last_err + err * err) / 100, 4)
		abs_err = round(boost * rel_err, 2)
		print(f"$\\cdot$ на $\\frac{{{last_avg} - {avg}}}{{{last_avg}}} * 100 = {boost:.2f}\\% \\pm {abs_err:.2f}$ $\\%$ относительно предыдущей оптимизации.\n", file=res_file)
		print(f"Абсолютная погрешность: ${boost:.2f}\\% * \\frac{{\\sqrt{{{last_err:.2f}^2+{err:.2f}^2}}}}{{100}} = {boost:.2f}\\% * {rel_err:.4f} = {abs_err:.2f}\\%$\n", file=res_file)
		print(f"Относительная погрешность ускорения: ${round(rel_err * 100, 2):.2f}\\%$\n", file=res_file)

		last_avg = base_avg
		last_err = base_err
		boost	= round(((last_avg - avg) / last_avg) * 100, 2)
		rel_err = round(sqrt(last_err * last_err + err * err) / 100, 4)
		abs_err = round(boost * rel_err, 2)
		print(f"$\\cdot$ на $\\frac{{{last_avg} - {avg}}}{{{last_avg}}} * 100 = {boost:.2f}\\% \\pm {abs_err:.2f}$ $\\%$ относительно -O3.\n", file=res_file)
		print(f"Абсолютная погрешность: ${boost:.2f}\\% * \\frac{{\\sqrt{{{last_err:.2f}^2+{err:.2f}^2}}}}{{100}} = {boost:.2f}\\% * {rel_err:.4f} = {abs_err:.2f}\\%$\n", file=res_file)
		print(f"Относительная погрешность ускорения: ${round(rel_err * 100, 2):.2f}\\%$\n", file=res_file)

		print("![2-я оптимизация](../images/opt_crc32.png)\n", file=res_file)


	elif (test_ind == 3):
		print(f"Полученное ускорение:\n", file=res_file)
		boost	= round(((last_avg - avg) / last_avg) * 100, 2)
		rel_err = round(sqrt(last_err * last_err + err * err) / 100, 4)
		abs_err = round(boost * rel_err, 2)
		print(f"$\\cdot$ на $\\frac{{{last_avg} - {avg}}}{{{last_avg}}} * 100 = {boost:.2f}\\% \\pm {abs_err:.2f}$ $\\%$ относительно предыдущей оптимизации.\n", file=res_file)
		print(f"Абсолютная погрешность: ${boost:.2f}\\% * \\frac{{\\sqrt{{{last_err:.2f}^2+{err:.2f}^2}}}}{{100}} = {boost:.2f}\\% * {rel_err:.4f} = {abs_err:.2f}\\%$\n", file=res_file)
		print(f"Относительная погрешность ускорения: ${round(rel_err * 100, 2):.2f}\\%$\n", file=res_file)

		last_avg = base_avg
		last_err = base_err
		boost	= round(((last_avg - avg) / last_avg) * 100, 2)
		rel_err = round(sqrt(last_err * last_err + err * err) / 100, 4)
		abs_err = round(boost * rel_err, 2)
		print(f"$\\cdot$ на $\\frac{{{last_avg} - {avg}}}{{{last_avg}}} * 100 = {boost:.2f}\\% \\pm {abs_err:.2f}$ $\\%$ относительно -O3.\n", file=res_file)
		print(f"Абсолютная погрешность: ${boost:.2f}\\% * \\frac{{\\sqrt{{{last_err:.2f}^2+{err:.2f}^2}}}}{{100}} = {boost:.2f}\\% * {rel_err:.4f} = {abs_err:.2f}\\%$\n", file=res_file)
		print(f"Относительная погрешность ускорения: ${round(rel_err * 100, 2):.2f}\\%$\n", file=res_file)

		print("![3-я оптимизация](../images/opt_assem.png)", file=res_file)
	
	with open('results/_last_res.txt', 'w') as save_res:
		print(f"{avg}\n{err}", file=save_res)
