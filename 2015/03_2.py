import sys

def main() -> None:
    presents: dict[tuple[int, int], int] = { (0, 0): 2 }
    input: str = ""
    santa: list[int, int] = [0, 0]
    robosanta: list[int, int] = [0, 0]

    with open("./03_input.txt") as f:
        input = f.read()

    count: int = 0
    for c in input:
        count += 1
        if (count % 2):
            active = santa
        else:
            active = robosanta
        match c:
            case '^':
                active[1] += 1
            case 'v':
                active[1] -= 1
            case '<':
                active[0] -= 1
            case '>':
                active[0] += 1
            case '\n':
                continue
            case _:
                print("Error: unrecognized character")
                sys.exit(1)

        x, y = active[0], active[1]
        presents[(x, y)] = presents.get((x, y), 0) + 1

    print(f'Santa (and robosanta) visited {len(presents.keys())} houses')

if __name__ == "__main__":
    main()
