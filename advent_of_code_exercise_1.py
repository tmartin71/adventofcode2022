import numpy as np

input_path_exercise_1 = '/Users/tylermartin/Documents/adventofcode2022/input.txt'

def open_input(path):
    file = open(path, mode = 'r', encoding = 'utf-8-sig')
    lines = file.readlines()
    file.close()
    return lines

def get_int_from_line(line):
    return int(line.strip())

def exercise_1():
    lines = open_input(input_path_exercise_1)

    elves = []
    elf = []

    for line in lines:
        if line != "\n":
            elf.append(get_int_from_line(line))
        
        if line == "\n":
            total_cal = np.sum(elf)
            elves.append(total_cal)
            elf.clear()

    elves.sort()
    top_3 = elves[-3:]

    print(top_3[2])
    print(np.sum(top_3))
    return

if __name__=="__main__":
    exercise_1()

