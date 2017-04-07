clc;
clear all;

distance_to_impact = [584.873;
                    571.102;
                    558.315;
                    543.137;
                    530.76;
                    515.551;
                    502.809;
                    488.632;
                    476.378;
                    461.778;
                    448.467;
                    435.075;
                    422.326;
                    408.151;
                    394.813;
                    381.213;
                    367.98;
                    354.149];

t = 1:1:18;

A = [t', ones(18,1)];

b = distance_to_impact;

x = A\b;

t_fine = 1:0.1:50;

line = x(1)*t_fine + x(2);

figure(1)
plot(t,distance_to_impact, 'o',t_fine,line)
axis([0,50,0,700])
xlabel('Frame Number')
ylabel('Estimated Distance To Impact (mm)')
title('Task 3 Time to Impact')