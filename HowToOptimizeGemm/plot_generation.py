import sys
import re
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
        gflops.append(split_array[1])
    return pArray, gflops


def plotData(pArray, gflopsArray):

    # Make a data frame
    df=pd.DataFrame({'p': pArray, 'MMult0': gflopsArray[0],'MMult_1x4_9': gflopsArray[1], 'MMult_4x4_15': gflopsArray[2], })

    # style
    plt.style.use('seaborn-darkgrid')

    # create a color palette
    palette = plt.get_cmap('Set1')

    # multiple line plot
    num = 0
    for column in df.drop('p', axis=1):
        num += 1
        plt.plot(df['p'], df[column], marker='', color=palette(num), linewidth=1, alpha=0.9, label=column)

    # Add legend
    plt.legend(loc=2, ncol=2)

    # Add titles
    plt.title("Matix performance", loc='center', fontsize=12, fontweight=0, color='orange')
    plt.xlabel("P")
    plt.ylabel("Gflops")
    plt.show()


if __name__ == "__main__":
    filenames = ['MMult0.txt','MMult_1x4_9.txt','MMult_4x4_15.txt']
    gflopsArray = []
    for file in filenames:
        generated_array = analysisStdin(file)
        pArray, gflops = analysisArray(generated_array)
        gflopsArray.append(gflops)
    plotData(pArray, gflops)
