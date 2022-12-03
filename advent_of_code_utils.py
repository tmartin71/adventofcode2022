class AdventOfCodeUtils:
    path = '/Users/tylermartin/Documents/adventofcode2022/'

    @classmethod
    def open_input(cls, filename):
        file = open(cls.path + filename, mode = 'r', encoding = 'utf-8-sig')
        lines = file.readlines()
        file.close()
        return lines