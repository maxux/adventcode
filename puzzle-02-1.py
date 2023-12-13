import requests
import re

source = ""
with open("puzzle2.input", "r") as f:
    source = f.read()

total = 0

config = {"red": 12, "green": 13, "blue": 14}

def possible(gameid, input):
    for color in config.keys():
        for a in re.findall(f'[0-9]+ {color}', line):
            if int(a.split(" ")[0]) > config[color]:
                return 0

    return gameid

for line in source.strip().split("\n"):
    gameid = int(line[5:].split(":")[0])
    total += possible(gameid, line)

print(total)
