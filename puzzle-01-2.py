import requests
import re

source = ""
with open("puzzle1.input", "r") as f:
    source = f.read()

total = 0

a = ["one", "two", "three", "four", "five", "six", "seven", "eight", "nine"]
b = ["1", "2", "3", "4", "5", "6", "7", "8", "9"]

for line in source.strip().split("\n"):
    clean = line

    for idx, x in enumerate(a):
        clean = clean.replace(x, b[idx])

    rclean = re.sub("[^0-9]", "", clean)
    concat = f"{rclean[0]}{rclean[-1]}"

    print(f">> {concat} | {line} - {clean} - {rclean}")
    total += int(concat)

print(total)
