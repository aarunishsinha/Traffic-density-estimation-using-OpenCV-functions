import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os

def main():
	base_f = "base_out.csv"
	baseline = np.array(pd.read_csv(base_f,header = None))
	q = []
	d = []
	th = []
	ti = []
	cp = []
	for i in range(1, 9):
		out_file = str(i) + "_out_method3.csv"
		output = np.array(pd.read_csv(out_file,header = None))
		queue_error = 0.0
		dynamic_error = 0.0
		for j in range(len(output)):
			queue = baseline[j][1] - output[j][1]
			queue=queue*queue
			dynamic = baseline[j][2] - output[j][2]
			dynamic=dynamic*dynamic
			queue_error = queue_error+queue
			dynamic_error = dynamic_error+dynamic
		queue_error = queue_error/len(output)
		dynamic_error = dynamic_error/len(output)
		th.append(i)
		q.append(queue_error)
		d.append(dynamic_error)
	runtime_file = "runtime_method3.csv"
	cpu_file = "cpuUtilisation_method3.csv"
	runtime = np.array(pd.read_csv(runtime_file,header=None))
	cpu = np.array(pd.read_csv(cpu_file,header=None))
	for i in range(len(cpu)):
		ti.append(runtime[i][1])
		cp.append(cpu[i][1])
	plt.figure()
	plt.xlabel("Number of threads")
	plt.ylabel("Avgerage squared error")
	plt.plot(th,q,label="Queue Density error",marker='o')
	plt.plot(th,d,label="Dynamic Density error",marker='o')
	plt.legend()
	plt.grid()
	plt.savefig("plot1.png",dpi=200)
	plt.show()
	
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
	plt.savefig("plot2.png",dpi=200)
	plt.show()
	
	cell_text=[]
	for i in range(len(th)):
		cell_text.append([th[i],ti[i],cp[i],q[i],d[i]])
	table=plt.table(cellText=cell_text,colLabels=['Number of threads','Runtime(s)','CPU Utilisation(%)','Queue Density Error','Dynamic Density Error'],loc='center')
	ax=plt.gca()
	ax.get_xaxis().set_visible(False)
	ax.get_yaxis().set_visible(False)
	plt.box(on=None)
	table.scale(1,1.5)
	fig=plt.gcf();
	fig.set_size_inches(11,7)
	plt.savefig("table1.png",dpi=200)
	plt.show()
	
	
	q = list(map(lambda x:100.0/(1.0+x),q))
	d = list(map(lambda x:100.0/(1.0+x),d))
	plt.figure()
	plt.xlabel("Number of threads")
	plt.ylabel("Utility Percentage")
	plt.plot(th,q,label="Queue Density utility percentage",marker='o')
	plt.plot(th,d,label="Dynamic Density utility percentage",marker='o')
	plt.legend()
	plt.grid()
	fig=plt.gcf();
	fig.set_size_inches(8,6)
	plt.savefig("plot3.png",dpi=200)
	plt.show()
	
	plt.figure()
	plt.xlabel("Runtime (seconds)")
	plt.ylabel("Utility Percentage")
	plt.plot(ti,q,label="Queue Density utility percentage",marker='o')
	plt.plot(ti,d,label="Dynamic Density utility percentage",marker='o')
	plt.legend()
	plt.grid()
	fig=plt.gcf();
	fig.set_size_inches(8,6)
	plt.savefig("plot4.png",dpi=200)
	plt.show()
	
	cell_text=[]
	for i in range(len(th)):
		cell_text.append([th[i],ti[i],q[i],d[i]])
	table = plt.table(cellText=cell_text,colLabels=['Number of threads','Runtime(sec)','Queue Density Utility(%)','Dynamic Density Utility(%)'],loc='center')
	ax=plt.gca()
	ax.get_xaxis().set_visible(False)
	ax.get_yaxis().set_visible(False)
	plt.box(on=None)
	table.scale(1,1.5)
	fig=plt.gcf();
	fig.set_size_inches(10,7)
	plt.savefig("table2.png",dpi=200)
	plt.show()
	

if __name__ == "__main__":
	main()
