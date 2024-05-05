#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

// 定义一个结构体，用于存储字符和其频率
struct Node {
    unsigned char symbol;
    unsigned int freq;
    struct Node *left, *right;
};

// 创建一个函数，用于读取文件并统计每个字符的频率
void count_freq(char *filename, unsigned int *freq){
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("无法打开文件 %s\n", filename);
        return;
    }

    for (int i = 0; i < 256; i++) {
        freq[i] = 0;
    }

    unsigned char symbol;
    while (fread(&symbol, sizeof(unsigned char), 1, file)) {
        freq[symbol]++;
    }

    fclose(file);
}

// 创建一个函数，用于构建 Huffman 树
struct Node* build_huffman_tree(unsigned int *freq) {
    struct Node* nodes[256];
    unsigned int freqs[256];
    int size = 0;

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            nodes[size] = (struct Node*)malloc(sizeof(struct Node));
            nodes[size]->symbol = i;
            nodes[size]->freq = freq[i];
            nodes[size]->left = NULL;
            nodes[size]->right = NULL;
            freqs[size] = freq[i];
            size++;
        }
    }

    while (size > 1) {
        // 找到频率最小的两个节点
        int min1 = 0, min2 = 1;
        if (freqs[min1] > freqs[min2]) {
            int temp = min1;
            min1 = min2;
            min2 = temp;
        }
        for (int i = 2; i < size; i++) {
            if (freqs[i] < freqs[min1]) {
                min2 = min1;
                min1 = i;
            } else if (freqs[i] < freqs[min2]) {
                min2 = i;
            }
        }

        // 创建新的节点
        struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
        new_node->symbol = 0;
        new_node->freq = freqs[min1] + freqs[min2];
        new_node->left = nodes[min1];
        new_node->right = nodes[min2];

        // 删除旧的节点，添加新的节点
        if (min1 < min2) {
            for (int i = min1; i < size - 1; i++) {
                nodes[i] = nodes[i + 1];
                freqs[i] = freqs[i + 1];
            }
            for (int i = min2 - 1; i < size - 2; i++) {
                nodes[i] = nodes[i + 1];
                freqs[i] = freqs[i + 1];
            }
        } else {
            for (int i = min2; i < size - 1; i++) {
                nodes[i] = nodes[i + 1];
                freqs[i] = freqs[i + 1];
            }
            for (int i = min1 - 1; i < size - 2; i++) {
                nodes[i] = nodes[i + 1];
                freqs[i] = freqs[i + 1];
            }
        }
        nodes[size - 2] = new_node;
        freqs[size - 2] = new_node->freq;
        size--;
    }

    return nodes[0];
}

// 创建一个函数，用于生成 Huffman 编码表
void build_huffman_table(struct Node* node, char *table[], char *code, int depth) {
    if (node->left == NULL && node->right == NULL) {
        code[depth] = '\0';
        table[node->symbol] = strdup(code);
    } else {
        if (node->left != NULL) {
            code[depth] = '0';
            build_huffman_table(node->left, table, code, depth + 1);
        }
        if (node->right != NULL) {
            code[depth] = '1';
            build_huffman_table(node->right, table, code, depth + 1);
        }
    }
}

// 创建一个函数，用于将原文件转化为 Huffman 编码，并将编码表和编码后的内容写入到目标文件中
void compress_file(char *input_filename, char *output_filename, char *table[]) {
    FILE *input = fopen(input_filename, "rb");
    if (input == NULL) {
        printf("无法打开文件 %s\n", input_filename);
        return;
    }

    FILE *output = fopen(output_filename, "wb");
    if (output == NULL) {
        printf("无法打开文件 %s\n", output_filename);
        fclose(input);
        return;
    }

    // 写入 Huffman 编码表
    for (int i = 0; i < 256; i++) {
        if (table[i] != NULL) {
            fprintf(output, "%d %s\n", i, table[i]);
        }
    }

    // 写入一个分隔行
    fputs("---\n", output);

    unsigned char symbol;
    while (fread(&symbol, sizeof(unsigned char), 1, input)) {
        fputs(table[symbol], output);
    }

    fclose(input);
    fclose(output);
}

// 创建一个函数，用于从压缩文件中读取 Huffman 编码表和编码后的内容，并利用编码表解码输出
void decompress_file(char *input_filename, char *output_filename, struct Node* root) {
    FILE *input = fopen(input_filename, "rb");
    if (input == NULL) {
        printf("无法打开文件 %s\n", input_filename);
        return;
    }

    FILE *output = fopen(output_filename, "wb");
    if (output == NULL) {
        printf("无法打开文件 %s\n", output_filename);
        fclose(input);
        return;
    }

    // 读取 Huffman 编码表
    char *table[256];
    for (int i = 0; i < 256; i++) {
        table[i] = NULL;
    }
    int symbol;
    char code[256];
    char line[256];
    while (fgets(line, sizeof(line), input)) {
        if (strcmp(line, "---\n") == 0) {
            break;
        }
        sscanf(line, "%d %s", &symbol, code);
        table[symbol] = strdup(code);
    }

    // 重建 Huffman 树
    //struct Node* root = build_huffman_tree_from_table(table);

    // 读取编码后的内容并解码
    // struct Node* node = root;
    // int c;
    // while ((c = fgetc(input)) != EOF) {
    //     if (c == '0') {
    //         node = node->left;
    //     } else if (c == '1') {
    //         node = node->right;
    //     }
    //     if (node->left == NULL && node->right == NULL) {
    //         fputc(node->symbol, output);
    //         node = root;
    //     }
    // }

    // 读取编码后的内容并解码
    char buffer[256];
    int index = 0;
    int c;
    while ((c = fgetc(input)) != EOF) {
        buffer[index++] = c;
        buffer[index] = '\0';
        for (int i = 0; i < 256; i++) {
            if (table[i] != NULL && strcmp(buffer, table[i]) == 0) {
                fputc(i, output);
                index = 0;
                break;
            }
        }
    }

    fclose(input);
    fclose(output);
}

int main(int argc, char *argv[]) {
    int compress = -1;//-1表示未选择，0表示解压，1表示压缩
    char *input_filename = NULL;
    char *output_filename = NULL;

    struct option long_options[] = {
        {"compress", no_argument, NULL, 'c'},
        {"extract", no_argument, NULL, 'x'},
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {NULL, 0, NULL, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "cxi:o:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'c':
                compress = 1;
                break;
            case 'x':
                compress = 0;
                break;
            case 'i':
                input_filename = optarg;
                break;
            case 'o':
                output_filename = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-c|-x] -i input -o output\n", argv[0]);
                return 1;
        }
    }

    if (compress == -1 || input_filename == NULL || output_filename == NULL) {
        fprintf(stderr, "Usage: %s [-c|-x] -i input -o output\n", argv[0]);
        return 1;
    }

    // 读取原始文件并统计每个字符的频率
    unsigned int freq[256];
    count_freq(input_filename, freq);

    // 使用这些频率构建 Huffman 树
    struct Node* root = build_huffman_tree(freq);

    // 使用 Huffman 树生成 Huffman 编码表
    char *table[256];
    char code[256];
    for (int i = 0; i < 256; i++) {
        table[i] = NULL;
    }
    build_huffman_table(root, table, code, 0);

    // 根据用户的选择进行压缩或解压
    if (compress) {
        compress_file(input_filename, output_filename, table);
    } else {
        decompress_file(input_filename, output_filename, root);
    }

    return 0;
}