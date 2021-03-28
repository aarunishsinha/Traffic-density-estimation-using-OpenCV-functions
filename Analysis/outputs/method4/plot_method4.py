import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os

def main():
	th = []
	ti = []
	cp = []
	runtime_file = "runtime_method4.csv"
	cpu_file = "cpu_util4.csv"
	runtime = np.array(pd.read_csv(runtime_file,header=None))
	cpu = np.array(pd.read_csv(cpu_file,header=None))
	for i in range(len(cpu)):
		th.append(i)
		ti.append(runtime[i][1])
		cp.append(cpu[i][1])
	
	fig,ax = plt.subplots()
	ax.set_xlabel("Number of threads")
	ax.set_ylabel("Runtime(seconds)")
	ln1=ax.plot(th,ti,label="Runtime(seconds)",color="red",marker='o')
	ax2=ax.twinx()
	ax2.set_ylabel("CPU Utilisation %")
	ln2=ax2.plot(th,cp,label="Percentage of cpu utilised by the program",color="blue",marker='o');
	lns=ln1+ln2
	labs=[l.get_label() for l in lns]
	ax.legend(lns,labs,loc=5)
	plt.grid()
	plt.savefig("plot1.png",dpi=200)
	plt.show()
	
	cell_text=[]
	for i in range(len(th)):
		cell_text.append([th[i],ti[i],cp[i]])
	table=plt.table(cellText=cell_text,colLabels=['Number of threads','Runtime(s)','CPU Utilisation(%)'],loc='center')
	ax=plt.gca()
	ax.get_xaxis().set_visible(False)
	ax.get_yaxis().set_visible(False)
	plt.box(on=None)
	table.scale(1,1.5)
	plt.savefig("table1.png",dpi=200)
	plt.show()
	

if __name__ == "__main__":
	main()
