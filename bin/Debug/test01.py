#!/usr/bin/python3.5
import  time
import csv
import random


train_file_name = 'train.csv'

class Hello:
    def __init__(self, x):
        self.a = x
    def print(self, x=None):
        print(x)


def xprint():
    print("read file!")
    fptr = open('testfile.csv', 'r')
    print(fptr.readlines())
    return 666



def readcsv(filename):
    data_inedx = []
    data_name = []
    print("filenameis:")
    print(filename)

    fptr = open(filename, 'r')
    lines_list = fptr.readline()
    #print(lines_list)

    print("processing!")

    data_name.append('P')
    data_name.append('I')
    data_name.append('D')
    data_inedx.append(random.uniform(0, 100))
    data_inedx.append(random.uniform(5, 10))
    data_inedx.append(random.uniform(1, 5))



    datas = [data_name,data_inedx]
    with open('train.csv','w+') as f:
        writer = csv.writer(f)
        for row in datas:
            writer.writerow(row)

    return 0





if __name__ == "__main__":
    #xprint()
    ##h = Hello(5)
    ##print()
    readcsv('testfile.csv')



