from advent_of_code_utils import AdventOfCodeUtils
from enum import Enum

input_filename_exercise_2 = "input_exercise_2.txt"

class Outcome(Enum):
    LOSE = 0
    DRAW = 3
    WIN = 6

player_1_string_map = {
    "A" : "Rock",
    "B" : "Paper",
    "C" : "Scissors"
    }

player_2_string_map = {
    "X" : "Rock",
    "Y" : "Paper",
    "Z" : "Scissors"
    }

player_2_outcome_map = {
    "X" : Outcome.LOSE,
    "Y" : Outcome.DRAW,
    "Z" : Outcome.WIN
}

class RockPaperScissorsFactory:
    def make(input_string):
        if input_string == "Rock":
            return Rock()
        elif input_string == "Scissors":
            return Scissors()
        elif input_string == "Paper":
            return Paper()

    def get_all_shapes():
        return [Rock(), Paper(), Scissors()]

class RPSBase:
    @classmethod
    def GetShapeScore(cls):
        return 0

    @classmethod
    def IsRock(cls):
        return False

    @classmethod
    def IsScissors(cls):
        return False

    @classmethod
    def IsPaper(cls):
        return False

    @classmethod
    def CompareShape(cls, input):
        return Outcome.DRAW

class Rock(RPSBase):
    @classmethod
    def GetShapeScore(cls):
        return 1

    @classmethod
    def IsRock(cls):
        return True

    @classmethod
    def CompareShape(cls, input):
        if input.IsPaper():
            return Outcome.LOSE
        elif input.IsScissors():
            return Outcome.WIN
        else:
            return RPSBase.CompareShape(input)

class Paper(RPSBase):
    @classmethod
    def GetShapeScore(cls):
        return 2

    @classmethod
    def IsPaper(cls):
        return True

    @classmethod
    def CompareShape(cls, input):
        if input.IsScissors():
            return Outcome.LOSE
        elif input.IsRock():
            return Outcome.WIN
        else:
            return RPSBase.CompareShape(input)  

class Scissors(RPSBase):
    @classmethod
    def GetShapeScore(cls):
        return 3

    @classmethod
    def IsScissors(cls):
        return True

    @classmethod
    def CompareShape(cls, input):
        if input.IsRock():
            return Outcome.LOSE
        elif input.IsPaper():
            return Outcome.WIN
        else:
            return RPSBase.CompareShape(input)

def make_map(string_map):
    output = dict()
    for key in string_map:
        output[key] = RockPaperScissorsFactory.make(string_map[key])

    return output

def compute_score(player_1_choice, player_1_map, player_2_choice, player_2_map):
    opponent_choice = player_1_map[player_1_choice]
    your_choice = player_2_map[player_2_choice]
    result = your_choice.CompareShape(opponent_choice)

    return your_choice.GetShapeScore() + result.value

def compute_score_from_desired_outcome(player_1_choice, player_1_map, player_2_choice, player_2_outcome_map):
    player_1_shape = player_1_map[player_1_choice]
    desired_outcome = player_2_outcome_map[player_2_choice]
    score = 0
    all_shapes = RockPaperScissorsFactory.get_all_shapes()
    for shape in all_shapes:
        if shape.CompareShape(player_1_shape) == desired_outcome:
            score += shape.GetShapeScore()

    score += desired_outcome.value
    return score

def exercise_2():
    lines = AdventOfCodeUtils.open_input(input_filename_exercise_2)
    player_1_map = make_map(player_1_string_map)
    player_2_map = make_map(player_2_string_map)
    total_score = 0
    total_score_part_2 = 0
    for line in lines:
        total_score += compute_score(line[0], player_1_map, line[2], player_2_map)
        total_score_part_2 += compute_score_from_desired_outcome(line[0], player_1_map, line[2], player_2_outcome_map)

    print(total_score)
    print(total_score_part_2)

if __name__ == "__main__":
    exercise_2()