import numpy as np

from advent_of_code_utils import AdventOfCodeUtils

input_filename_exercise_1 = 'input_exercise_1.txt'

def get_int_from_line(line):
    return int(line.strip())

def exercise_1():
    lines = AdventOfCodeUtils.open_input(input_filename_exercise_1)

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

if __name__ == "__main__":
    exercise_1()

