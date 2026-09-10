#include "embeddings.cpp/bert.h"
#include "diffed/flatlake.h"
#include "diffed/utils/calculations.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv) {
    const std::string model_path = argc > 1
        ? argv[1]
        : "embeddings.cpp/models/bge-base-en-v1.5/ggml-model-q4_0.bin";

    const std::string text = argc > 2
        ? argv[2]
        : "This is a test sentence for generating an embedding.";
    
    const std::string dtext = "This is a different sentence for comparison.";

    bert_ctx *ctx = bert_load_from_file(model_path.c_str());
    if (ctx == nullptr) {
        std::cerr << "Failed to load model: " << model_path << '\n';
        return EXIT_FAILURE;
    }

    const int32_t embedding_size = bert_n_embd(ctx);
    flatlake::FlatLake fl(static_cast<size_t>(embedding_size));
    std::vector<float> embedding(embedding_size);
    std::vector<float> dembedding(embedding_size);

    bert_encode(ctx, 4, text.c_str(), embedding.data());
    bert_encode(ctx, 4, dtext.c_str(), dembedding.data());

    fl.add(0, embedding);
    fl.add(1, dembedding);

    float dp = calculations::dot_product(
        embedding,
        dembedding,
        static_cast<size_t>(embedding_size));

    double squared_norm = 0.0;
    for (float value : embedding) {
        squared_norm += static_cast<double>(value) * value;
    }

    std::cout << "Model: " << model_path << '\n'
              << "Text: " << text << '\n'
              << "Embedding dimensions: " << embedding_size << '\n'
              << "Embedding norm: " << std::sqrt(squared_norm) << '\n'
              << "First values: [";

    const int values_to_print = std::min(10, embedding_size);
    for (int i = 0; i < values_to_print; ++i) {
        if (i != 0) {
            std::cout << ", ";
        }
        std::cout << std::fixed << std::setprecision(6) << embedding[i] - dembedding[i];
    }
    std::cout << "]\n";

    std::cout << "dot product:\t" << dp << "\n";

    bert_free(ctx);
    return EXIT_SUCCESS;
}
