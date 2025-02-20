# Chess Engine 

A high-performance chess engine implemented in C++ with advanced search algorithms and parallel computing capabilities. Designed for integration with chess interfaces via UCI protocol and optimized for modern multi-core processors.

## Features

### Core Engine Architecture
- **Bitboard Representation** utilizing 64-bit unsigned integers for efficient board state management.
- **Magic Bitboards** for accelerated sliding piece move generation (rook, bishop, queen).
- **Iterative Deepening** with aspiration windows for dynamic depth adjustment.
- **Parallel Search** implementing Lazy SMP for efficient multi-threaded computation.

### Search Algorithms
- **Principal Variation Search** with enhanced null-move pruning.
- **Quiescence Search** for stable position evaluation in tactical situations.
- **History Heuristic** with killer moves and countermove prioritization.
- **Transposition Table** using Zobrist hashing for position caching.

### Evaluation System
- **Tapered Evaluation** blending midgame and endgame weights.
- **Piece-Square Tables** with mobility bonuses.
- **Pawn Structure Analysis** including isolated, doubled, and passed pawns.
- **King Safety Evaluation** considering pawn shields and attack patterns.

## Installation

### Requirements
- C++17 compatible compiler (GCC 11+/Clang 14+).
- CMake 3.20+.
- Python 3.11+ (for UCI validation).

To install the chess engine, clone the repository and build it:

```sh
git clone https://github.com/Arsh-2811/chess-engine
cd chess-engine
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_SMP=ON ..
make -j$(nproc)
```

## Lichess Integration

To deploy as a Lichess bot, follow the official [lichess-bot documentation](https://github.com/lichess-bot-devs/lichess-bot/wiki) for:
- Bot account creation.
- API token generation.
- Engine configuration.

Configure the `config.yml` file to set up your bot's parameters.

## Acknowledgements

This project builds upon foundational work from:
- **Deepov** by Romain Goussault ([GitHub](https://github.com/RomainGoussault/Deepov)) - Neural network evaluation architecture reference.
- Stockfish Development Team - Search algorithm optimizations.
- Python-Chess Maintainers - UCI protocol validation suite.
- Lichess Bot Developers - API integration patterns.
