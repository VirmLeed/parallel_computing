from string import ascii_letters
from random import randint
from tqdm import tqdm

LINES = 10000
LENGTH = 100000

output = ""
letters = len(ascii_letters)

with open("./input.txt", "w") as f:
    for line in tqdm(range(LINES)):
        [f.write(ascii_letters[randint(0, letters-1)]) for letter in range(LENGTH)]
        f.write("\n")
