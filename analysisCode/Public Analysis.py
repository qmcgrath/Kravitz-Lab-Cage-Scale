#!/usr/bin/env python
# coding: utf-8

# Provided analysis code for Kravitz scale. 

# In[1]:


import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import math as math
import numpy as np
import itertools
from scipy import stats
import seaborn as sns
from scipy.stats import mode
import matplotlib.patches as patches


# <font size=5>Section 1: Initial Visualization</font>

# Initial visualization of scale performance directly from the data frame, with no processing. 
# Current max his give a primary visualition of the . The histogram processing data on the arduino is not as powerful and space on the board is limited, thus it uses only about 50 data points. This column should not be used for overall mouse weight. Instead, refer to further in the notebook.

# In[30]:


#add own file location 
df = pd.read_csv("C:/Users/qmcgr/OneDrive/Documents/Creed Labs/3scale files/SA030804.CSV")
df["hours"] = np.floor(df[' Seconds']/3600)


# In[32]:


#C grams refers to corrected grams, or the scale reading - zero points 
plt.plot(df[' Seconds']/3600, df[' cgrams'],  marker=None, color='blue', alpha=0.5, label = 'cgrams')
plt.plot(df[' Seconds']/3600,df[' range'], marker=None, color='grey', alpha=0.5, label = 'prox')
plt.xlabel("Time (Hours)")
plt.ylim(0,40)
plt.legend()
plt.title("Mouse CGrams Vs. Time:" + df[" filename"][1])
plt.show()



plt.plot(df[' Seconds']/3600, df[' zeroAvg'],  marker=None, color='blue', alpha=0.5)
plt.xlabel("Time (Hours)")
plt.title("Zero Point Vs. Time:" + df[" filename"][1])
plt.show()


# <font size=5>Section 2: Determine Weight Per Hour</font>

# In[33]:


import statistics
def findHourlyMedians(dataFrame, interpolate):
    dfFilter = dataFrame[dataFrame[' cgrams'] > 15]  
    df_hourToWeight = dfFilter.groupby('hours')[' cgrams'].apply(list).reset_index()
    df_hourToWeight['median'] = 0.0
    df_hourToWeight['count'] = 0
    hoursUnfilled = 0 
    for i in range(len(df_hourToWeight['median'])):
        df_hourToWeight['median'][i] = statistics.median(df_hourToWeight[' cgrams'][i])
        df_hourToWeight['count'][i] = len(df_hourToWeight[' cgrams'][i])
        #play with this limit
        if df_hourToWeight['count'][i]  <= 100:
            df_hourToWeight['median'][i] =  np.nan 
            df_hourToWeight['count'][i] =  np.nan 
            hoursUnfilled = hoursUnfilled + 1
    #plt.scatter(df_hourToWeight['hours'],df_hourToWeight['median'])
    #plt.show()       
    
    #!!!!!!!!!! change iter to see that interpolation works well. Adds about .1 error for 50% of points

    if interpolate: 
        hoursUnfilled = 0 
        df_hourToWeight['median'] =  df_hourToWeight['median'].interpolate(method='linear',limit_direction='both')
    #count hours filled 
    resolution =100 - ( 100* (hoursUnfilled/ len(df_hourToWeight['hours']) ) ) 
    
    return df_hourToWeight


# In[36]:


resultantHours = findHourlyMedians(df, True)
print(resultantHours)
plt.scatter(resultantHours['hours'],resultantHours['median'])
plt.xlabel("Time (Hours)")
plt.ylim(0,40)
plt.legend()
plt.title("Calculated Weight (g) Vs. Time" )
plt.ylim([24,27])
plt.show()


# <font size=5>Section 3: Track Circadian Patterns</font>

# In[37]:


#get datetime hour, this may take awhile
df['DateTime'] = 0

for i in range(len(df)):
    try: 
        df['DateTime'][i] = pd.to_datetime(df["MM: DD: YYYY hh: mm: ss"][i], format='%m/%d/%Y %H:%M')
        
    except ValueError: 
        df.drop(index=i,inplace=True)

df['hourCirc'] = df['DateTime'].dt.hour
df['dayCirc'] = df['DateTime'].dt.day


# In[38]:


#create array of reasonable c grams
reasonableGrams = df[(df[" cgrams"] > 15) & (df[" range"] < 40) ][' cgrams'] 
hoursGrams = df[(df[" cgrams"] > 15) & (df[" range"] < 40) ]['hourCirc'] 
axes = sns.lineplot(x = hoursGrams, y = reasonableGrams)
axes.set_ylabel ("Grams")
axes.set_xlabel ("24 hour time (hours)")
#set axis limits - here can cut off coldroom period
axes.set_xlim(0, 24)
axes.set_xticks([0, 6, 12, 18, 24])
#nighttime patch
axes.add_patch(
            patches.Rectangle(
                (6,0),   # (x,y)
                12,          # width
                300,        # height
                fill=True,
                zorder=0,
                alpha = 0.2,
                color = "gray"
                )
            )

#recorded weights if applicable 
recordedA = [25.8, 25.43, 25.26, 25.6, 25.1, 25.3, 25]; 
hoursRecorded = [0 , 3, 5 ,1 ,5 ,2 ,4];
plt.scatter(hoursRecorded,recordedA,color='r',label='recorded')
plt.ylim([24,26])
plt.show()


# 
