clc;
clear all;

frames_to_impact = [44.8668;
                    47.539;
                    41.2463;
                    48.704;
                    38.2521;
                    40.1262;
                    37.0678;
                    38.7235;
                    33.529;
                    36.8131;
                    33.4559;
                    34.9768;
                    31.3895;
                    31.8186;
                    29.6752;
                    30.5671;
                    27.2262];
t = 1:1:17;

A = [t', ones(17,1)];

b = frames_to_impact;

x = A\b;

t_fine = 0:0.1:50;

line = x(1)*t_fine + x(2); 


plot(t,frames_to_impact, 'o',t_fine,line)
axis([0,50,0,50])
                
      
