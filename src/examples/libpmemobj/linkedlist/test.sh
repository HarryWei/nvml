#!/bin/bash

echo "32 bytes..."
./flush
#sleep 10
./fifo32 pool_file32 1000000 32
./fifo32 pool_file32 1000000 32

echo "64 bytes..."
./flush
#sleep 10
./fifo64 pool_file64 1000000 64
./fifo64 pool_file64 1000000 64

echo "128 bytes..."
./flush
#sleep 10
./fifo128 pool_file128 1000000 128
./fifo128 pool_file128 1000000 128

echo "256 bytes..."
./flush
#sleep 10
./fifo256 pool_file256 1000000 256
./fifo256 pool_file256 1000000 256

echo "512 bytes..."
./flush
#sleep 10
./fifo512 pool_file512 1000000 512
./fifo512 pool_file512 1000000 512

echo "1024 bytes..."
./flush
#sleep 10
./fifo1024 pool_file1024 1000000 1024
./fifo1024 pool_file1024 1000000 1024
