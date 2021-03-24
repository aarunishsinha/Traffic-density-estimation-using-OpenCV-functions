import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os

def main():
	base_f = "1_out_method1.csv"
	baseline = np.array(pd.read_csv(base_f,header = None))
	x_3_f = "3_out_method1.csv"
	x_5_f = "5_out_method1.csv"
	x_6_f = "6_out_method1.csv"
	x_9_f = "9_out_method1.csv"
	x_10_f = "10_out_method1.csv"
	x_12_f = "12_out_method1.csv"
	x_15_f = "15_out_method1.csv"
	x_18_f = "18_out_method1.csv"
	x_21_f = "21_out_method1.csv"
	x_3 = np.array(pd.read_csv(x_3_f,header = None))
	x_5 = np.array(pd.read_csv(x_5_f,header = None))
	x_6 = np.array(pd.read_csv(x_6_f,header = None))	
	x_9 = np.array(pd.read_csv(x_9_f,header = None))
	x_10 = np.array(pd.read_csv(x_10_f,header = None))
	x_12 = np.array(pd.read_csv(x_12_f,header = None))
	x_15 = np.array(pd.read_csv(x_15_f,header = None))
	x_18 = np.array(pd.read_csv(x_18_f,header = None))
	x_21 = np.array(pd.read_csv(x_21_f,header = None))
	print (x_3)

if __name__ == "__main__":
	main()