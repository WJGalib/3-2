#!/bin/bash

if [ $# -lt 4 ] ; then
    echo "Usage:"
    echo -e "./organize.sh <submission folder> <target folder> <test folder> <answer folder> [-v] [-noexecute]\n"
    echo "-v: verbose"
    echo "-noexecute: do not execute code files"
    kill -INT $$
fi

submissions="$1"
targets="$2"
tests="$3"
answers="$4"

verbose=false

mkdir -p "$targets/C" "$targets/Java" "$targets/Python"

n_cases=$(ls $answers/ans*.txt | wc -l)

if [ "$5" = "-v" ] ; then
    verbose=true
    echo "Found $n_cases test files"
fi

if [ "$6" != "-noexecute" ] ; then
    results="$targets/result.csv"
    echo "student_id,type,matched,not_matched" > "$results"
fi

for zip_path in "$submissions"/*
do 
    fn=${zip_path%.zip}
    std_id=${fn: -7}

    std_edir="$targets/$std_id"
    mkdir -p "$std_edir"
    unzip "$zip_path" -d "$std_edir" > /dev/null 2>&1

    c_target=$(find "$std_edir" -type f -name "*.c")
    py_target=$(find "$std_edir" -type f -name "*.py")
    j_target=$(find "$std_edir" -type f -name "*.java")

    # c_target=$(ls -R "$std_edir" | grep .*\.c)
    # py_target=$(ls -R "$std_edir" | grep .*\.py)
    # j_target=$(ls -R "$std_edir" | grep .*\.java)
    exec_dir=""
    lang=""

    # TASK  A :

    if [ $verbose = true ] ; then
        echo "Organizing files of $std_id"
    fi

    if [ ! -z "$c_target" ] ; then
        lang="C"
        exec_dir="$targets/$lang/$std_id"
        mkdir -p "$exec_dir"
        mv "$c_target" "$exec_dir/main.c"
    elif [ ! -z "$py_target" ] ; then
        lang="Python"
        exec_dir="$targets/$lang/$std_id"
        mkdir -p "$exec_dir"
        mv "$py_target" "$exec_dir/main.py"
    elif [ ! -z "$j_target" ] ; then
        lang="Java"
        exec_dir="$targets/$lang/$std_id"
        mkdir -p "$exec_dir"
        mv "$j_target" "$exec_dir/Main.java"
    fi
    rm -r "$std_edir"


    #TASK B :

    if [ "$6" = "-noexecute" ] ; then
        continue
    else 
        if [ $verbose = true ] ; then
            echo "Executing files of $std_id"
        fi
    fi

    if [ ! -z "$c_target" ] ; then
        gcc "$exec_dir/main.c" -o "$exec_dir/main.out" > /dev/null 2>&1
    elif [ ! -z "$j_target" ] ; then
        javac "$exec_dir/Main.java" > /dev/null 2>&1
    fi

    matched=0
    unmatched=0

    for ((i=1;i<=n_cases;i++))
    do
        if [ ! -z "$c_target" ] ; then
            "$exec_dir/main.out" < "$tests/test$i.txt" > "$exec_dir/out$i.txt"
        elif [ ! -z "$py_target" ] ; then
            python3 "$exec_dir/main.py" < "$tests/test$i.txt" > "$exec_dir/out$i.txt"
        elif [ ! -z "$j_target" ] ; then
            java -cp "$exec_dir" Main < "$tests/test$i.txt" > "$exec_dir/out$i.txt"
        fi

        if (diff "$answers/ans$i.txt" "$exec_dir/out$i.txt" > /dev/null 2>&1) ; then
            matched=`expr $matched + 1`
        else
            unmatched=`expr $unmatched + 1`
        fi
    done

    echo "$std_id,$lang,$matched,$unmatched" >> "$results"

 done