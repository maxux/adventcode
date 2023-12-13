import requests
import re

source = ""
with open("puzzle-01.input", "r") as f:
    source = f.read()

total = 0

for line in source.split():
    clean = re.sub("[^0-9]", "", line)
    concat = f"{clean[0]}{clean[-1]}"
    total += int(concat)

print(total)
