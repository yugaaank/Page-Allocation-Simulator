<div align="center">

# Page-Allocation-Simulator

[![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)](https://isocpp.org)
[![Python](https://img.shields.io/badge/GUI-tkinter-3776AB?logo=python&logoColor=white)](#cli--gui)
[![License](https://img.shields.io/badge/license-MIT-8b5cf6)](#license)

</div>

`Page-Allocation-Simulator` is an interactive teaching tool for page-replacement
algorithms. It runs FIFO, LRU, and Optimal on a reference string and shows,
step by step, how the frame set evolves and where page faults occur. A C++
terminal simulator does the work; a tkinter GUI wraps the same algorithms for
a visual walkthrough.

## Why

Page replacement is easiest to understand by watching frames fill and evict.
The simulator prints each step with ANSI coloring (faults vs. hits) and the GUI
animates the same trace, so the difference between FIFO, LRU, and Optimal is
immediately visible rather than derived from a formula.

## What it implements

- **FIFO** — evicts the oldest-loaded frame (cyclic pointer).
- **LRU** — evicts the least-recently-used frame.
- **Optimal** — evicts the frame whose next use is farthest away (needs the
  full reference string; the theoretical best).

Each algorithm returns `(faults, hits, steps)` where every `step` records the
page, the resulting frame set, and its status — the same structure feeds both
the CLI trace and the GUI table.

## CLI / GUI

`simulator.cpp` — interactive, colored terminal simulator:

```bash
g++ -std=c++17 -Wall -o simulator simulator.cpp
./simulator
```

`simulator_gui.py` — tkinter front end over `solve_fifo` / `solve_lru` /
`solve_optimal` (deque-based frame simulation), with a scrolled step log:

```bash
python simulator_gui.py
```

## Project structure

```
Page-Allocation-Simulator/
├── simulator.cpp        # C++ terminal simulator (FIFO/LRU/Optimal)
├── simulator_gui.py     # tkinter GUI over the same algorithms
└── simulator/           # build / support
```

## License

MIT
