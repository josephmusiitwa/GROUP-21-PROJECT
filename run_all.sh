#!/bin/bash
mkdir -p data/output
for f in data/input/*.csv; do
  n=$(basename "$f" .csv)
  ./build-direct/example1 "$f" "data/output/${n}_results.csv"
done
