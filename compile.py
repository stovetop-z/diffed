#!/usr/bin/env python3

import argparse
import platform
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "main.cc"
OUTPUT = ROOT / "main"
LIBRARY_BUILD = ROOT / "embeddings.cpp" / "build"
GGML_BUILD = LIBRARY_BUILD / "ggml" / "src"


def main() -> int:
    parser = argparse.ArgumentParser(description="Compile the embeddings.cpp test program")
    parser.add_argument("--run", action="store_true", help="run the program after compiling")
    parser.add_argument("model", nargs="?", help="optional model path passed to the test")
    parser.add_argument("text", nargs="?", help="optional text passed to the test")
    args = parser.parse_args()

    if not SOURCE.exists():
        parser.error(f"source file not found: {SOURCE}")
    if not (LIBRARY_BUILD / "libbert.dylib").exists() and not (LIBRARY_BUILD / "libbert.so").exists():
        parser.error(
            f"bert library not found in {LIBRARY_BUILD}; build embeddings.cpp first"
        )

    if platform.system() == "Darwin":
        rpath = [
            "-Wl,-rpath,@loader_path/embeddings.cpp/build",
            "-Wl,-rpath,@loader_path/embeddings.cpp/build/ggml/src",
        ]
    else:
        rpath = [
            "-Wl,-rpath,$ORIGIN/embeddings.cpp/build",
            "-Wl,-rpath,$ORIGIN/embeddings.cpp/build/ggml/src",
        ]

    command = [
        "clang++",
        "-std=c++20",
        str(SOURCE),
        f"-L{LIBRARY_BUILD}",
        "-lbert",
        f"-L{GGML_BUILD}",
        "-lggml",
        *rpath,
        "-o",
        str(OUTPUT),
    ]

    print("Compiling:", " ".join(command))
    subprocess.run(command, check=True, cwd=ROOT)
    print(f"Built {OUTPUT}")

    if args.run:
        run_command = [str(OUTPUT)]
        if args.model:
            run_command.append(args.model)
        if args.text:
            run_command.append(args.text)
        subprocess.run(run_command, check=True, cwd=ROOT)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
