use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

fn main() {
    let lines = match read_lines("./input.txt") {
        Ok(lines) => lines,
        Err(why) => {
            eprintln!("Error reading file: {why}");
            return;
        }
    };

    let bad_strings = vec!["ab", "cd", "pq", "xy"];
    let vowels = "aeiou";

    let mut nice_strings_count = 0u64;

    for line in lines.map_while(Result::ok) {
        if bad_strings.iter().any(|&bs| line.contains(bs)) {
            continue;
        }

        let mut vowel_count = 0;
        let mut double_letter = false;
        let mut last_letter: char = '\0';

        for c in line.chars() {
            if c == last_letter {
                double_letter = true;
            }
            if vowels.contains(c) {
                vowel_count += 1;
            }
            last_letter = c;
        }
        if vowel_count >= 3 && double_letter {
            nice_strings_count += 1;
        }
    }
    println!("{nice_strings_count}");
}

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where
    P: AsRef<Path>,
{
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}
