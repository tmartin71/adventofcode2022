from advent_of_code_utils import AdventOfCodeUtils

input_filename_exercise_3 = '/input_exercise_4.txt'

class Range:
    def __init__(self, range):
        min, max = range.split('-')
        self.min = int(min)
        self.max = int(max)
        assert(self.min <= self.max)
    
    def get_length(self):
        return self.max - self.min

    def contains(self, range):
        if self.min <= range.min and self.max >= range.max:
            return True

        return False

    def contains_value(self, value):
        if self.min <= value and self.max >= value:
            return True

        return False

    def overlaps(self, range):
        if self.contains_value(range.min) or self.contains_value(range.max):
            return True
        
        return False

def exercise_4():
    lines = AdventOfCodeUtils.open_input(input_filename_exercise_3)
    total_part_1 = 0
    total_part_2 = 0

    for line in lines:
        range_0_string, range_1_string = line.split(',')
        range_lhs = Range(range_0_string)
        range_rhs = Range(range_1_string)

        if range_lhs.get_length() <= range_rhs.get_length():
            range_lhs, range_rhs = range_rhs, range_lhs

        if range_lhs.contains(range_rhs):
            total_part_1 += 1

        if range_lhs.overlaps(range_rhs):
            total_part_2 += 1

    print(total_part_1)
    print(total_part_2)

if __name__== "__main__":
    exercise_4()