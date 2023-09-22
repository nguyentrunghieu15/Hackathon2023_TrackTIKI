clear
g++ index.cpp -o index
for i in 1 2 3 4 5 6 7 8 9 10
do
    ./index $i
    echo 
done