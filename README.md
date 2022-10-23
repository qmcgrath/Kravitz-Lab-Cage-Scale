# Kravitz-Lab-Cage-Scale
Quinlan McGrath, Robbie Schaefer, Lex Kravitz, Gunwoo Lee

  The use of food and water restriction, drugs, surgery, and disease models in live mouse studies require continuous monitoring of body weight to surveill mouse health and determine experiment endpoints. Resolution of weight fluctuation over time can offer researchers a more complete picture of mouse health. Traditional manual weighing induces stress in mice and disrupts circadian rhythms.Existing commercial and open source autonomous mouse weight systems are available. However, commercial systems are not cost effective, cannot be used in colonial housing cages, or required rFID tags. 
 
Accurate, non-invasive recording of mouse weight allows for increased volume of body weight data while decreasing animal stress and human interaction. The developed scale can be constructed for $150 and operates for ~3.5 days on one charge. It has a small footprint of 7.6x8.6x8 cm and can be used in singley housed mice in traditional vivarium cages without modification. An Hx711 and proximity sensor work in tandem to determine mouse weight readings. The scale is operates on open source arduino code and data is anlysised with provided python code. 
  
  It was demonstrated that the scale can acheived and estimation of mouse weight with an average of 1.55% error with 50% hourly resolution (22 instances). Hourly resolution can be increased to 100% at the expense of an increased average error of 1.66% (33 instances). Accuracy was determined via comparision of maunaul weighing and reported weights for the same hour. Twenty two instances were used. It was demonstrated that this scale has potential appications in circadian rhythm monitoring, energy expendature evaluation, and group housed mice.  
  
 Assemebly, hardware and software overviews, quick start instructions, and code documentation are available in the wiki. The Jupyter notebooks used for data anlysis are provided. 

A graphical overview of embedded logic is shown below. 

<img src="https://github.com/qmcgrath/Kravitz-Lab-Cage-Scale/blob/main/Images/stateValueDiagram.png" width="500">

## Results
Three scales (labeled A, B, C) were constructed and included in testing procedures. Individually house mice were monitored continuously for an average of 5 days. Manual weights were recorded at 33 instances.  

Code and data used are available above. The manual recorded weights are available in 'example data'.

Figure 1: Visualization of the basic scale operations. 

<img src="https://github.com/qmcgrath/Kravitz-Lab-Cage-Scale/blob/main/Images/rangeandCgramsSA.png" width="300">

Figure 2: Reported hourly weight (g) vs Time (hour)

<img src="https://github.com/qmcgrath/Kravitz-Lab-Cage-Scale/blob/main/Images/weightVsRecordSA.png" width="300">


The methodology for calcuting the hourly weight from the scale data was simple. First, any report cgrams < 15 g were dropped, as it can be assumed these were recorded when the mouse was only partially on the scale. Next, the data was binned into hours and the median was found. Median weights were found to be more accurate than mode data. For further evaluation, a 'minimum number of cgram recordings per hour' was selected in which a reported weight could be deemed reliable or well informed. An array of minimum number counts, the resultant accuracy and resolution, and instances count (# of hours for which there was a manual weight available) are reported in Table 1. As shown, the average error is a minimum at a 150 instance thresehold. However, 100 data points was choosen as the optimal value as it has a higher resolution (nearly 50%) for minimum accuracy gain. Thus, it was determined that the scale can determine mouse weight with an accuracy of 1.55% with 50% resolution.


Figure 3: Weight error percent distribution. 

<img src="https://github.com/qmcgrath/Kravitz-Lab-Cage-Scale/blob/main/Images/histrogamErrorNoScaleSEp.png" width="300">


Table 1 was recreated for data in which hourly median weight was interpolated across hours with insufficient cgrams were reported. This increased resolution to 100%, will only minorly increasing overall error to 1.66%. 

However, it can be seen in the color coded histogram that not all scales performed equally. In fact, Table 3 demonstrates the distribution of error across the scales - with scale A outperforming the other two substaintially.


### Next Steps 
- More user friendly 3d design 
- improved calibration code 
