#!/bin/bash

openapi_file="/Users/frizer/Documents/Study/YandexBack/enrollment/openapi.json"

# Output directory for schema folders and files
output_dir="schemas"

mkdir -p "$output_dir"

schemas=$(jq -r '.components.schemas | keys[]' "$openapi_file")

for schema in $schemas; do
    schema_dir="$output_dir/$schema"
    mkdir -p "$schema_dir"

    cpp_file="$schema_dir/$schema.cpp"
    echo "#include \"$schema.h\"" > "$cpp_file"
    echo "" >> "$cpp_file"
    echo "class $schema {" >> "$cpp_file"
    echo "    // Add class implementation here" >> "$cpp_file"
    echo "};" >> "$cpp_file"

    h_file="$schema_dir/$schema.h"
    echo "#ifndef ${schema}_H" > "$h_file"
    echo "#define ${schema}_H" >> "$h_file"
    echo "" >> "$h_file"
    echo "class $schema {" >> "$h_file"
    echo "    // Add class declaration here" >> "$h_file"
    echo "};" >> "$h_file"
    echo "" >> "$h_file"
    echo "#endif // ${schema}_H" >> "$h_file"
done

echo "Class templates have been created."
