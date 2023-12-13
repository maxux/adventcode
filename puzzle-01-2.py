import requests
import re

source = ""
with open("puzzle-01.input", "r") as f:
    source = f.read()

total = 0

a = ["one", "two", "three", "four", "five", "six", "seven", "eight", "nine"]
b = ["o1e", "t2o", "t3ree", "f4ur", "f5ve", "s6x", "s7ven", "e8ght", "n9ne"]

for line in source.strip().split("\n"):
    clean = line

    for idx, x in enumerate(a):
        clean = clean.replace(x, b[idx])

    rclean = re.sub("[^0-9]", "", clean)
    concat = f"{rclean[0]}{rclean[-1]}"

    print(f">> {concat} | {line} - {clean} - {rclean}")
    total += int(concat)

print(total)
