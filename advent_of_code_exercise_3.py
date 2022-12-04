from mimetypes import init
from advent_of_code_utils import AdventOfCodeUtils

import string
import numpy as np

input_filename_exercise_3 = '/input_exercise_3.txt'

def compute_letter_score(s):
    if s.isupper():
        return string.ascii_uppercase.index(s) + 27
    elif s.islower():
        return string.ascii_lowercase.index(s) + 1

def compute_score(string_collection):
    score = 0
    for s in string_collection:
        score += compute_letter_score(s)
    return score

class Rucksack:
    def __init__(self, init_string):
        sanitized = init_string.strip()
        string_length = len(sanitized)
        assert(np.mod(string_length, 2) == 0)

        compartment_string_length = int(string_length / 2)
        first_substring = sanitized[0:compartment_string_length]
        second_substring = sanitized[-compartment_string_length:]

        self.compartment_0 = self.fill_compartment(first_substring)
        self.compartment_1 = self.fill_compartment(second_substring)
        self.all_items = self.fill_compartment(sanitized)
        
    @classmethod
    def fill_compartment(cls, substring):
        return set(substring)

    def compute_intersection(self):
        return self.compartment_0.intersection(self.compartment_1)

    def get_all_items(self):
        return self.all_items

class ElfGroup:
    def __init__(self, rucksacks):
        self.rucksacks = rucksacks
        self.num_rucksacks = len(rucksacks)

    def compute_intersection(self):
        output = self.rucksacks[0].get_all_items()
        for i in range(self.num_rucksacks - 1):
            output = output.intersection(self.rucksacks[i+1].get_all_items())

        assert(len(output) == 1)
        return output

def exercise_3():
    lines = AdventOfCodeUtils.open_input(input_filename_exercise_3)
    total_part_1 = 0
    total_part_2 = 0
    elf_count = 0
    rucksacks_in_group = []
    for line in lines:
        rucksack = Rucksack(line)
        rucksacks_in_group.append(rucksack)

        total_part_1 += compute_score(rucksack.compute_intersection())
        elf_count += 1
        if elf_count == 3:
            group = ElfGroup(rucksacks_in_group)
            total_part_2 += compute_score(group.compute_intersection())
            elf_count = 0
            rucksacks_in_group = []

    print(total_part_1)
    print(total_part_2)

if __name__ == "__main__":
    exercise_3()