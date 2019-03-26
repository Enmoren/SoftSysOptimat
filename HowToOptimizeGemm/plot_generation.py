import sys
import re
import ast
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def hasNumbers(inputString):
    return bool(re.search(r'\d', inputString))

def analysisStdin(filename):
    with open(filename, 'r') as content_file:
        content = content_file.read()
    array = []
    for line in content.split('\n'):
        array.append(line) if hasNumbers(line) else print('')
    return array

def analysisArray(stringArray):
    pArray = []
    gflops = []
    for str in stringArray:
        split_array = str.split(' ')
        pArray.append(split_array[0])
        gflops.append(ast.literal_eval(split_array[1]))
    return pArray, gflops


def plotData(pArray, gflopsArray):

    # Make a data frame
    df=pd.DataFrame({'p': pArray, 'MMult_4x4_15': gflopsArray[0],'MMult_1x4_9': gflopsArray[1],'MMult0': gflopsArray[2]})

    # style
    # plt.style.use('seaborn-darkgrid')

    # create a color palette
    palette = ['tomato', 'orange', 'green']

    # multiple line plot
    num = 0
    for column in df.drop('p', axis=1):
        plt.plot(df['p'], df[column], marker='', color=palette[num], linewidth=1, alpha=0.9, label=column)
        num += 1

    # Add legend
    # plt.legend(loc=5, ncol=1)
    plt.legend(loc='best')

    # Add titles
    plt.title("Matrix Multiplication Performance", loc='center', fontsize=14, fontweight=0, color='black')
    plt.xlabel("Leading Dimension", fontsize=12, labelpad=10)
    plt.ylabel("Gflop/sec", fontsize=12, labelpad=10)
    plt.show()


if __name__ == "__main__":
    filenames = ['MMult_4x4_15.txt','MMult_1x4_9.txt','MMult0.txt']
    gflopsArray = []
    for file in filenames:
        generated_array = analysisStdin(file)
        pArray, gflops = analysisArray(generated_array)
        gflopsArray.append(gflops)
    print (gflopsArray)
    plotData(pArray, gflopsArray)
