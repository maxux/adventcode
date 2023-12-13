import requests
import re

source = ""
with open("puzzle-02.input", "r") as f:
    source = f.read()

total = 0

def possible(gameid, input):
    mins = {"blue": 0, "red": 0, "green": 0}

    for color in mins.keys():
        for a in re.findall(f'[0-9]+ {color}', line):
            val = int(a.split(" ")[0])
            if val > mins[color]:
                mins[color] = val

    power = 1
    for color in mins.keys():
        power *= mins[color]

    return power

for line in source.strip().split("\n"):
    gameid = int(line[5:].split(":")[0])
    total += possible(gameid, line)

print(total)
