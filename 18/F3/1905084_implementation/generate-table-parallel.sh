g++ max_cut.cpp -O9 -o max_cut
./generate-table-1.sh > table_part1.csv &
./generate-table-2.sh > table_part2.csv &
./generate-table-3.sh > table_part3.csv &
./generate-table-4.sh > table_part4.csv &
