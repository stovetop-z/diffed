# Diffed

Diffed is a lightweight C++ vector database prototype. It stores fixed-size float vectors in memory, assigns them to simple search buckets, and returns nearby vectors using cosine similarity.

The project is intentionally small. It is currently a research and experimentation codebase—not a complete version-control system, persistent database, or production-ready approximate-nearest-neighbor library.

## What it provides

- In-memory storage for vectors and numeric IDs.
- Dimension validation when vectors are inserted.
- A flat contiguous storage layout for vector data.
- Bucketed indexing through `R` and `Centroid`.
- Cosine-similarity queries with a configurable `top_k` result count.
- Vector math helpers for dot products, norms, distances, and angles.
- ARM NEON acceleration for common four-float operations.
- A small embedding example using `embeddings.cpp`.

## Layout

```text
diffed/
├── flatlake.h                 # Basic in-memory vector storage
├── centroid.h                 # A bucket of stored vectors
├── R.h                        # Bucketed vector index and queries
└── utils/calculations.h       # SIMD-enabled vector operations

main.cc                        # Embedding and query example
compile.py                     # Example build/run helper
```

## Basic usage

The core types can be included directly in a C++20 program:

```cpp
#include "diffed/R.h"

#include <vector>

int main() {
    R index(/*dimensions=*/4, /*parsecs=*/1);

    index.add(101, {1.0f, 0.0f, 0.0f, 0.0f}, 4);
    index.add(102, {0.9f, 0.1f, 0.0f, 0.0f}, 4);
    index.add(103, {0.0f, 0.0f, 1.0f, 0.0f}, 4);

    std::vector<float> query = {1.0f, 0.0f, 0.0f, 0.0f};
    auto results = index.query(query, 2);
}
```

Each result contains the stored ID and its cosine similarity:

```cpp
struct QueryResult {
    uint64_t id;
    float similarity;
};
```

`FlatLake` can be used on its own when bucketed search is unnecessary:

```cpp
#include "diffed/flatlake.h"

flatlake::FlatLake store(3);
store.add(1, {0.2f, 0.4f, 0.8f});
```

## Building the example

The headers themselves only require a C++20 compiler and, on the current implementation, ARM NEON support. The example program additionally links against the local `embeddings.cpp` library.

Build the embedding dependency first, then run:

```sh
python3 compile.py
```

To compile and run the example:

```sh
python3 compile.py --run
```

The example loads a BGE embedding model, creates vectors for sample text, inserts two vectors into `R`, and queries the index. A model path and input text can be supplied as arguments:

```sh
python3 compile.py --run \
  path/to/model-q4_0.bin \
  "Text to encode"
```

The embedding model and conversion workflow are maintained by the `embeddings.cpp` dependency. They are not required for using the basic vector-storage headers with your own vectors.

## Notes on limitations

- Data exists only in memory and is lost when the process exits.
- There is no persistence, WAL, transactions, or recovery.
- There is no metadata or document storage beyond numeric IDs.
- The index is experimental and does not yet provide a general ANN algorithm.
- Query results are limited to the selected bucket rather than a guaranteed global nearest-neighbor search.
- The current SIMD implementation targets ARM NEON.
- Thread safety and concurrent reads/writes are not implemented.

## Possible next steps

Potential additions include persistence, metadata filters, deletion and update operations, global or multi-bucket search, better indexing strategies, platform-specific SIMD implementations, and a small public API for inserting and querying records.
