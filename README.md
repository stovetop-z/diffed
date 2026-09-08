# Vectorized Version Control System

A prototype version-control system that represents files, commits, and changes as vectors so they can be searched by semantic similarity.

The current prototype uses [`embeddings.cpp`](embeddings.cpp/) with a BGE embedding model. It can load a model, encode text, and print the resulting vector. The version-control and vector-database layers are the next stages of development.

## Current status

Working:

- Build the `bert.cpp` embedding library and its `ggml` dependency.
- Download and convert a Hugging Face embedding model.
- Generate normalized embeddings from C++.
- Compile and run the test program with `compile.py`.

Planned:

- Represent commits, files, and diffs as searchable records.
- Store embeddings in a vector database.
- Search repository history using natural-language queries.
- Combine semantic search with normal hash-based version-control operations.
- Add a command-line interface for indexing, searching, committing, and inspecting history.

## Requirements

- macOS or Linux
- Python 3
- A C++20 compiler (`clang++` or `g++`)
- CMake
- Hugging Face model access

## Setup

Initialize the embedding library's `ggml` dependency:

```sh
cd embeddings.cpp
git submodule update --init --recursive
```

Create or activate a Python environment and install the model-conversion dependencies:

```sh
python3 -m venv .venv
source .venv/bin/activate
pip install -r embeddings.cpp/requirements.txt
```

## Download and convert a model

Build the native conversion tools first:

```sh
cmake -S embeddings.cpp -B embeddings.cpp/build-tools \
  -DBUILD_SHARED_LIBS=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5

cmake --build embeddings.cpp/build-tools --parallel
```

Download the model into `embeddings.cpp/models`:

```sh
cd embeddings.cpp/models
python download-repo.py BAAI/bge-base-en-v1.5
```

Convert it to GGML format and quantize it:

```sh
python convert-to-ggml.py bge-base-en-v1.5 1

../build-tools/bin/quantize \
  bge-base-en-v1.5/ggml-model-f16.bin \
  bge-base-en-v1.5/ggml-model-q4_0.bin 2

cd ../..
```

The quantized model is used by the C++ test program.

## Build the embedding library

```sh
cmake -S embeddings.cpp -B embeddings.cpp/build \
  -DBUILD_SHARED_LIBS=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5

cmake --build embeddings.cpp/build --parallel
```

## Compile and run the test

```sh
python3 compile.py --run
```

The default test uses:

```text
embeddings.cpp/models/bge-base-en-v1.5/ggml-model-q4_0.bin
```

You can provide a different model and input text:

```sh
python3 compile.py --run \
  path/to/model-q4_0.bin \
  "Searchable text from a repository"
```

The program prints the embedding size, vector norm, and first few vector values.

## Direction

The core design is to preserve ordinary version-control identity while adding semantic retrieval:

```text
repository content
        |
        v
  files / diffs / commits
        |
        v
    embeddings
        |
        v
    vector index  <---- natural-language query
```

Hashes remain the source of truth for exact history and reproducibility. Embeddings provide an additional discovery layer for questions such as:

- “When did authentication failure handling change?”
- “Find commits related to database migrations.”
- “Show code similar to this function.”

Embeddings should be treated as derived data: the repository history can always be rebuilt if the model or vector index changes.
