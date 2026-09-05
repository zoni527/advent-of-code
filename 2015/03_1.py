import sys

def main() -> None:
    presents: dict[tuple[int, int], int] = { (0, 0): 1 }
    input: str = ""
    current_location: tuple(int, int) = (0, 0)

    with open("./03_input.txt") as f:
        input = f.read()

    for c in input:
        x, y = current_location
        match c:
            case '^':
                current_location = (x, y + 1)
            case 'v':
                current_location = (x, y - 1)
            case '<':
                current_location = (x - 1, y)
            case '>':
                current_location = (x + 1, y)
            case '\n':
                continue
            case _:
                print("Error: unrecognized character")
                sys.exit(1)

        presents[current_location] = presents.get(current_location, 0) + 1

    print(f'Santa visited {len(presents.keys())} houses')

if __name__ == "__main__":
    main()
