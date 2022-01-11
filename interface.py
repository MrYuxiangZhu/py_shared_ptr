# -*-coding:utf-8 -*-
import numpy as np #matplotlib
def add_numbers(xold, yold):
    res = [xold[i] + yold[i] for i in range(len(xold))]
    return res
