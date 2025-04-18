#!/bin/bash

# use samples
# ./encrypt_file.sh 689bc3e3bdf1c5f2cff81725011ba7d3c0089b25 models/ configs/models

CRYPTOR_EXEC="/home/sinter/workspace/sk_breast/install/tools/cryptor"

if [ "$#" -ne 3 ]; then
    echo "Error: 参数数量不正确。"
    echo "Usage: $0 <commit> <input_dir> <output_dir>"
    echo "Sample: $0 689bc3e3bdf1c5f2cff81725011ba7d3c0089b25 ./input_models ./encrypted_models"
    exit 1
fi

COMMIT="$1"
INPUT_DIR="$2"
OUTPUT_DIR="$3"

if [ ! -x "$CRYPTOR_EXEC" ]; then
    echo "Error:加密程序 '$CRYPTOR_EXEC' 不存在或没有执行权限。" >&2
    echo "请确保 '$CRYPTOR_EXEC' 在当前目录并且是可执行的。" >&2
    exit 1
fi

if [ ! -d "$INPUT_DIR" ]; then
    echo "Error:输入目录 '$INPUT_DIR' 不存在或不是一个目录。" >&2
    exit 1
fi

mkdir -p "$OUTPUT_DIR"
if [ $? -ne 0 ]; then
    echo "Error:无法创建输出目录 '$OUTPUT_DIR'。" >&2
    exit 1
fi

echo "开始处理目录: '$INPUT_DIR'"
echo "Commit ID: $COMMIT"
echo "输出到目录: '$OUTPUT_DIR'"
echo "-------------------------------------"

find "$INPUT_DIR" -maxdepth 1 -type f | while IFS= read -r input_file; do
    filename=$(basename "$input_file")

    if [[ "$filename" =~ ^([^.]+)(\..*)?$ ]]; then
        first_part="${BASH_REMATCH[1]}"
        rest_part="${BASH_REMATCH[2]}"

        if [[ -z "$rest_part" ]]; then
            output_filename="${first_part}.enc"
        elif [[ -z "$first_part" ]]; then
            output_filename="${filename}.enc"
        else
            output_filename="${first_part}.enc${rest_part}"
        fi
    else
        echo "警告: 文件名 '$filename' 格式特殊，将在末尾添加 .enc" >&2
        output_filename="${filename}.enc"
    fi

    output_file="$OUTPUT_DIR/$output_filename"

    echo "正在处理: '$filename' -> '$output_filename'"

    "$CRYPTOR_EXEC" --mode "encrypt" --input "$input_file" --output "$output_file" --commit "$COMMIT"

    if [ $? -ne 0 ]; then
        echo "Error:加密文件 '$filename' 失败。" >&2
    fi
done

echo "-------------------------------------"
echo "Done"
exit 0
