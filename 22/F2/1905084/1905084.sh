#!/bin/bash

mobility=("2" "1")
param=("nWifi" "nFlow" "nPkt" "covFac" "speed")

mkdir -p scratch/plots/1
mkdir -p scratch/plots/2
rm scratch/plots/1/*
rm scratch/plots/2/*

gp1="set terminal png size 640,480;"
gp2="using 1:2 with linespoints"

for m in ${mobility[*]}; do
    for p in ${param[*]}; do
        j=0
        if [[ "$p" == "covFac" ]]; then
            if [[ "$m" == "2" ]]; then
                continue
            fi
        fi
        if [[ "$p" == "speed" ]]; then
            if [[ "$m" == "1" ]]; then
                continue
           fi
        fi
        if [[ "$p" == "nWifi" ]]; then
            j=20
        elif [[ "$p" == "nFlow" ]]; then
            j=10
        elif [[ "$p" == "nPkt" ]]; then
            j=100
        elif [[ "$p" == "covFac" ]]; then
            j=1
        elif [[ "$p" == "speed" ]]; then
            j=5
        else
            echo "invalid arguments"; 
        fi
        for i in {1..5}; do
            ./ns3 run "\"scratch/1905084_$m --$p=$((i * j)) --plotX=$p\""
            gnuplot -e "$gp1 set xlabel '$p'; set ylabel 'Packet Delivery Ratio'; plot 'scratch/plots/$m/pdr_v_$p.dat' $gp2" > "scratch/plots/$m/pdr_v_$p.png"
            gnuplot -e "$gp1 set xlabel '$p'; set ylabel 'Throughput (Mbit/s)'; plot 'scratch/plots/$m/throughput_v_$p.dat' $gp2" > "scratch/plots/$m/throughput_v_$p.png"
        done
    done
done

