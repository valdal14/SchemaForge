# SchemaForge

![C](https://img.shields.io/badge/language-C-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Memory](https://img.shields.io/badge/memory-Zero_Heap-orange.svg)

SchemaForge is a high-performance, zero-heap streaming CSV validator and type-inference engine written in bare-metal C. It is designed to act as a lightning-fast, invisible pass-through filter between data sources and destination pipelines, ensuring strict schema enforcement before data reaches the target system.

## Features

* **Zero-Heap Architecture:** Utilizes strict stack allocation and Leftover Shift Memory Management to process massive streams with zero `malloc` or `free` calls, preventing memory leaks and guaranteeing predictable CPU performance.
* **Character-Level State Machine:** Infers data types (`INT`, `FLOAT`, `BOOL`, `VARCHAR`) on the fly without heavy Regular Expression libraries.
* **Intelligent Upcasting:** Permits upward type compatibility (e.g., passing `1050` into a `VARCHAR` column) to match standard database ingestion behaviors.
* **Bounds Protection:** Instant, pipeline-severing underflow and overflow protection for corrupted delimiter streams.
* **Log Engine Ready:** Outputs a clean JSON summary payload to `stderr` upon success for seamless integration with enterprise telemetry and log engines.

## Compilation

A `Makefile` is provided for standard GCC compilation.

```bash
# Compile for Production (Silent pass-through)
make

# Compile for Development (Includes token type visualization)
make debug

# Clean build artifacts
make clean
```

Usage
SchemaForge accepts data via standard input (stdin) and requires two arguments: the expected schema (comma-separated or semicolon-separated string of types) and the single-character delimiter.

Syntax:

```bash
cat data.csv | ./sfengine.out "<SCHEMA_STRING>" "<DELIMITER>"
```

Example: The Happy Path
Valid data is streamed silently to stdout to be picked up by your destination pipeline, while a success JSON log is sent to stderr.

```Bash
echo -e "ID;Price;IsActive;Name\n1;14.99;true;Alice\n2;0.00;false;Bob" | ./sfengine.out "INT;FLOAT;BOOL;VARCHAR" ";"
```

Output:

```PlainText
ID;Price;IsActive;Name
1;14.99;true;Alice
2;0.00;false;Bob
{"status": "success", "rows_validated": 2}
```

Example: Schema Mismatch
If a mismatch or structural error occurs, the pipeline is immediately severed (Exit Code 1) and a detailed error is sent to stderr.

```Bash
echo -e "ID;Price;IsActive;Name\n1;FREE;true;Alice" | ./sfengine.out "INT;FLOAT;BOOL;VARCHAR" ";"
```

Output:

```PlainText
ID;Price;IsActive;Name
Schema Mismatch at row 2, column 1: Expected FLOAT but got VARCHAR
```
