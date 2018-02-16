import matplotlib.pyplot  as plt
import pandas as pd
import numpy as np
import os.path
import h5py
filename = '../output/data.h5'
if(not os.path.exists(filename)):
    print("File does not exist.")
    exit(1)

store = pd.HDFStore(filename,mode='r')      #Use this to read hdf5 tables. Open in read mode.
h5f   = h5py.File(filename,'r')             #Use this to read hdf5 datasets. Pandas does not like datasets, but h5py does.

thermo_exists    = "thermodynamics" in store
temps_exists     = "temperatures/T" in store

if thermo_exists and temps_exists:
    keys = [s for s in store.keys() if "thermodynamics" in s] # Scan for all datasets in "group"
    # T = h5f["temperatures/T"].value   #Use this to read the temperature dataset instead. This is needed in older versions of the PT program.
    thermo = pd.DataFrame()
    for key in keys:
        headers = list(store[key])
        thermo = pd.concat([thermo, store[key].tail(1)], ignore_index = True) # Take last element in each table
    thermo = thermo.sort_values(by="T")    # Sort by temperature. This this is needed because T10 would come before T2 otherwise
    print(thermo)
    fig, ax = plt.subplots(2, 2, sharex=True)
    ax[0,0].errorbar(thermo["T"], thermo["u"], yerr=thermo["u_std"], capsize=4)
    ax[0,1].errorbar(thermo["T"], thermo["m"], yerr=thermo["m_std"], capsize=4)
    ax[1,0].errorbar(thermo["T"], thermo["c"], yerr=thermo["c_std"], capsize=4)
    ax[1,1].errorbar(thermo["T"], thermo["x"], yerr=thermo["x_std"], capsize=4)

store.close()
plt.show()
exit()