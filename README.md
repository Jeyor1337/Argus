# Argus

A simple wordlist generator.

## What is this?

Argus is a command-line tool for generating wordlists. You give it a character set, and it will generate all possible combinations of those characters up to a certain length. It's multi-threaded, so it's pretty fast.

## How to build

You'll need `gcc`, `cmake`, and `git`.

```bash
# Clone the repo
git clone https://github.com/your-username/argus.git
cd argus

# Build it
mkdir build && cd build
cmake ..
make
```

## How to use

```bash
./build/argus -c abcdef -m 1 -M 6 -t 4 -o wordlist.txt
```

This will generate a wordlist with all combinations of "abcdef" from 1 to 6 characters long, using 4 threads, and save it to `wordlist.txt`.

For all the options, run `./build/argus -h`.

## Contributing

Sure, why not. Open an issue or a PR.
