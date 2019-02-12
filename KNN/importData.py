import numpy as np

def importTrainingData(path):
    dataPoints = np.genfromtxt(
        path,
        delimiter = ';',
        usecols = [1, 2, 3, 4, 5, 6, 7],
        converters = {
            5: lambda s: 0 if s == b"-1" else float(s),
            7: lambda s: 0 if s == b"-1" else float(s)
        }
    )

    dates = np.genfromtxt(path, delimiter=';', usecols=[0])
    labels = []
    for label in dates:
        if label < 20000301:
            labels.append('winter')
        elif 20000301 <= label < 20000601:
            labels.append('lente') 
        elif 20000601 <= label < 20000901:
            labels.append('zomer') 
        elif 20000901 <= label < 20001201: 
            labels.append('herfst')
        else: # from 01-12 to end of year 
            labels.append('winter')
    
    return np.array(labels), dataPoints

def importValidationData(path):
    dataPoints = np.genfromtxt(
        path,
        delimiter = ';',
        usecols = [1, 2, 3, 4, 5, 6, 7],
        converters = {
            5: lambda s: 0 if s == b"-1" else float(s),
            7: lambda s: 0 if s == b"-1" else float(s)
        }
    )

    dates = np.genfromtxt(path, delimiter=';', usecols=[0])
    labels = []
    for label in dates:
        if label < 20010301:
            labels.append('winter')
        elif 20010301 <= label < 20010601:
            labels.append('lente') 
        elif 20010601 <= label < 20010901:
            labels.append('zomer') 
        elif 20010901 <= label < 20011201: 
            labels.append('herfst')
        else: # from 01-12 to end of year 
            labels.append('winter')

    return np.array(labels), dataPoints

def importUnlabeled(path):
    dataPoints = np.genfromtxt(
        path,
        delimiter = ';',
        usecols = [1, 2, 3, 4, 5, 6, 7],
        converters = {
            5: lambda s: 0 if s == b"-1" else float(s),
            7: lambda s: 0 if s == b"-1" else float(s)
        }
    )

    return dataPoints