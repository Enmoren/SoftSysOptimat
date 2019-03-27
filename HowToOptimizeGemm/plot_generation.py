# Generate performance figure of three different optimization techniques
# python3 plot_generation.py
# Authors: Hwei-Shin Harriman, Enmo Ren, and Cassandra Overney
# Copyright: MIT License

import sys
import re
import ast
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Check if line in data txt line consists of numbers
def hasNumbers(inputString):
    return bool(re.search(r'\d', inputString))

# Read in data txt file and read in data if it consists of has numbers
def analysisStdin(filename):
    # Read in txt file
    with open(filename, 'r') as content_file:
        content = content_file.read()
    array = []
    # Generate data array by analyzing each line of txt file
    for line in content.split('\n'):
        array.append(line) if hasNumbers(line) else print('')
    return array

# Split the data txt file to extract the gflops and leading dimension
def analysisArray(stringArray):
    pArray = []
    gflops = []
    # Split strings into size and gflops
    for str in stringArray:
        split_array = str.split(' ')
        pArray.append(split_array[0])
        gflops.append(ast.literal_eval(split_array[1]))
    return pArray, gflops

# Plot the gflops versu the leading dimension
def plotData(pArray, gflopsArray):
    # Make a data frame
    df=pd.DataFrame({'p': pArray, 'MMult_4x4': gflopsArray[0],'MMult_1x4': gflopsArray[1],'MMult0': gflopsArray[2]})

    # Create a color palette
    palette = ['tomato', 'orange', 'green']

    # Multiple line plot
    num = 0
    for column in df.drop('p', axis=1):
        plt.plot(df['p'], df[column], marker='', color=palette[num], linewidth=1, alpha=0.9, label=column)
        num += 1

    # Add legend
    plt.legend(loc='best')

    # Add titles
    plt.title("Matrix Multiplication Performance", loc='center', fontsize=14, fontweight=0, color='black')
    plt.xlabel("Leading Dimension", fontsize=12, labelpad=10)
    plt.ylabel("Gflop/sec", fontsize=12, labelpad=10)
    plt.show()

# Main function that plots the data in filename array
if __name__ == "__main__":
    filenames = ['Data/MMult_4x4_15.txt','Data/MMult_1x4_9.txt','Data/MMult0.txt']
    gflopsArray = []
    for file in filenames:
        # Get data from txt files
        generated_array = analysisStdin(file)
        pArray, gflops = analysisArray(generated_array)
        gflopsArray.append(gflops)
    # print (gflopsArray)
    plotData(pArray, gflopsArray)
