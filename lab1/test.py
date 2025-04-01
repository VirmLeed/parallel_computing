import subprocess

def get_run_times(thread_amount, matrix_size):
    result = subprocess.run(['./lab', str(thread_amount), str(matrix_size)], capture_output=True, text=True)
    if result.stderr:
        print("Error")
        quit()
    single, multiple = tuple(map(float, result.stdout.split("\n")[:-1]))
    return single, multiple

def print_times(single, multiple):
    print(f"\t\tОднопоточный алгоритм: {single:.10f}; Многопоточный алгоритм: {multiple:.10f}")


output = open("data.csv", "w")
output.write("thread_amount,matrix_side,single_threaded_algo,multithreaded_algo;")
 
print("Для маленьких матриц со стороной 10:")
for thread_exp in range(4):
    print(f"\tКол-во потоков в многопоточном алгоритме: {2**thread_exp}")
    s, m = get_run_times(2**thread_exp, 10)
    print_times(s, m)
    output.write(f"{2**thread_exp},10,{s},{m};")

for matrix_side_exp in range(5):
    print(f"Для матриц со стороной {1000+matrix_side_exp*100} ({(1000+matrix_side_exp*100)**2} эл.):")
    for thread_exp in range(6):
        print(f"\tКол-во потоков в многопоточном алгоритме: {2**thread_exp}")
        s, m = get_run_times(2**thread_exp, 1000+matrix_side_exp*100)
        print_times(s, m)
        output.write(f"{2**thread_exp},{1000+matrix_side_exp*100},{s},{m};")
