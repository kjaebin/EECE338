import numpy as np
import scipy.io as sio
import scipy.io.wavfile
import matplotlib.pyplot as plt
import argparse 

parser = argparse.ArgumentParser()
parser.add_argument('--stu_id', required=True, default=20232108)
args = parser.parse_args()

last_digit = int(args.stu_id[-1]) 

BW1 = last_digit + 1
BW2 = 10-last_digit

def quantization_error ( BitWidth ) : 
    
    ## generating x from 0 to 4pi and y using numpy library
    x = np.arange(0, 4*np.pi, 0.1) # start, stop, step
    y = np.sin(x)
    data = y 
    print("quantization in : ", BitWidth, "bit")
    ################WRITE YOUR OWN CODE##################
    # 1. make scale fator for quantization 
    #Scale_factor = (max(data)-min(data)) / (expression range)
    #
    # 2. quantized data using scale factor 
    # Quantized value = round(data/Scale_factor) * Scale_factor
    #
    # 3. find error caseud by quantization  
    # Error = SUM( |Quantized_value -data| ) 
    #  
    #####################################################

    ## ploting sine and cosine curve using matplotlb
    print('Error for {0:1d} bit quantization {1:.2f}'.format(last_digit, Error))
    y_q = Quantized_value
    plt.plot(x, y, x, y_q)
    plt.show()


quantization_error(BW1)
quantization_error(BW2)
