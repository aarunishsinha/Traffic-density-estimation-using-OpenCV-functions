import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os

def compKey(elem):
	return elem[2]

def main():
	base_f = "base_out.csv"
	baseline = np.array(pd.read_csv(base_f,header = None))
	q = []
	d = []
	t = []
	runtime_file = "runtime_method2.csv"
	runtime = np.array(pd.read_csv(runtime_file,header=None))
	runtime = sorted(runtime,key=compKey)
	for i in range(len(runtime)):
		filename = str(runtime[i][0])+"x"+str(runtime[i][1])+"_out_method2.csv"
		output = np.array(pd.read_csv(filename,header = None))
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
		q.append(queue_error)
		d.append(dynamic_error)
		t.append(runtime[i][2])
		
	plt.figure()
	plt.xlabel("Runtime(seconds)")
	plt.ylabel("Avgerage squared error")
	plt.plot(t,q,label="Queue Density error",marker='o')
	plt.legend()
	for i in range(len(t)):
		plt.annotate("("+str(runtime[i][0])+"x"+str(runtime[i][1])+")",(t[i],q[i]));
	plt.grid()
	plt.savefig("plot1.png",dpi=200)
	plt.show()
	
	plt.figure()
	plt.xlabel("Runtime(seconds)")
	plt.ylabel("Avgerage squared error")
	plt.plot(t,d,label="Dynamic Density error",marker='o')
	plt.legend()
	for i in range(len(t)):
		plt.annotate("("+str(runtime[i][0])+"x"+str(runtime[i][1])+")",(t[i],d[i]));
	plt.grid()
	plt.savefig("plot2.png",dpi=200)
	plt.show()
	
	cell_text=[]
	for i in range(len(t)):
		cell_text.append([str(runtime[i][0])+" x "+str(runtime[i][1]),t[i],q[i],d[i]])
	table=plt.table(cellText=cell_text,colLabels=['Resolution','Runtime(sec)','Queue Density Error','Dynamic Density Error'],loc='center')
	ax=plt.gca()
	ax.get_xaxis().set_visible(False)
	ax.get_yaxis().set_visible(False)
	plt.box(on=None)
	table.scale(1,1.5)
	fig=plt.gcf();
	fig.set_size_inches(10,7)
	plt.savefig("table1.png",dpi=200)
	plt.show()
	
	q = list(map(lambda x:100.0/(1.0+x),q))
	d = list(map(lambda x:100.0/(1.0+x),d))
	
	plt.figure()
	plt.xlabel("Runtime(seconds)")
	plt.ylabel("Utility Percentage")
	plt.plot(t,q,label="Queue Density utility percentage",marker='o')
	plt.legend()
	for i in range(len(t)):
		plt.annotate("("+str(runtime[i][0])+"x"+str(runtime[i][1])+")",(t[i],q[i]));
	plt.grid()
	plt.savefig("plot3.png",dpi=200)
	plt.show()
	
	plt.figure()
	plt.xlabel("Runtime(seconds)")
	plt.ylabel("Utility percentage")
	plt.plot(t,d,label="Dynamic Density utility percentage",marker='o')
	plt.legend()
	for i in range(len(t)):
		plt.annotate("("+str(runtime[i][0])+"x"+str(runtime[i][1])+")",(t[i],d[i]));
	plt.grid()
	plt.savefig("plot4.png",dpi=200)
	plt.show()
	
	cell_text=[]
	for i in range(len(t)):
		cell_text.append([str(runtime[i][0])+" x "+str(runtime[i][1]),t[i],q[i],d[i]])
	table = plt.table(cellText=cell_text,colLabels=['Resolution','Runtime(sec)','Queue Density Utility(%)','Dynamic Density Utility(%)'],loc='center')
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
