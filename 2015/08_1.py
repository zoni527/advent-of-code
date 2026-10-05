#!/bin/env python3

import re

"""
Objective: go through the whole input, count how many characters there are in
total, and then count how many characters would get removed if escapes were
only one character

Approaches:
1. go character by character, check for specific cases and move
index forward
2. Substitute matching patterns, calculate length

Observation: each line is double quoted, so they can just be removed
Go for a more Pythonic approach
"""


def main():
    with open('./08_input.txt', mode='r', encoding='utf-8') as f:
        input_raw = f.read()

    total_characters_raw = 0
    total_characters_str = 0
    for line in input_raw.split('\n'):
        if line == '\n':
            continue
        print(line)
        total_characters_raw += len(line)
        line = line[1:-1]
        line = line.replace('\\"', 'Q').replace('\\\\', 'S')
        line = re.sub(r'\\x[0-9a-f]{2}', 'X', line)
        print(line)

        total_characters_str += len(line)

    res = total_characters_raw - total_characters_str
    print(f'{total_characters_raw} - {total_characters_str} = {res}')


if __name__ == '__main__':
    main()
