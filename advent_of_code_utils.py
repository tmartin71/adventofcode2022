from re import U


class AdventOfCodeUtils:
    path = '/Users/tylermartin/Documents/adventofcode2022/'

    @classmethod
    def open_input(cls, filename):
        file = open(cls.path + filename, mode = 'r', encoding = 'utf-8-sig')
        lines = file.readlines()
        file.close()
        return lines

    @classmethod
    def compute_exercise_output(cls, lines, exercise_task):
        total_part_1 = 0
        total_part_2 = 0
        for line in lines:
            u, v = exercise_task(line)
            total_part_1 += u
            total_part_2 += v

        print(total_part_1)
        print(total_part_2)