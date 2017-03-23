clc;
clear;
close all

%copy the data from the text file
ballLeft = [20.998272, -70.10556, 451.49707; 20.895863, -69.732849, 441.85989; 20.858398, -69.365768, 430.45032; 20.807816, -69.005615, 419.39008; 20.811077, -68.606094, 407.59512; 20.780218, -67.992706, 395.02051; 20.74416, -67.291489, 382.91241; 20.760542, -66.649918, 371.66586; 20.726521, -65.863373, 359.93286; 20.71143, -65.004196, 348.45673; 20.711967, -64.097397, 337.2431; 20.692575, -63.17981, 326.09348; 20.716663, -62.19228, 315.45691; 20.732117, -61.079086, 304.05185; 20.733126, -59.907082, 292.73041; 20.751659, -58.746742, 282.06287; 20.778126, -57.484322, 271.06976; 20.76363, -56.181557, 260.20047; 20.781929, -54.846069, 250.00055; 20.785576, -53.369766, 239.17293; 20.804867, -51.879154, 228.42996; 20.813486, -50.286671, 218.02133; 20.821774, -48.705929, 207.63229];
ballRight = [22.754044, -70.557632, 451.78183; 22.614429, -70.192627, 442.14465; 22.532917, -69.903008, 430.73508; 22.439636, -69.531128, 419.67484; 22.39736, -69.06324, 407.87988; 22.317957, -68.373146, 395.30527; 22.235153, -67.755196, 383.19717; 22.208117, -67.110977, 371.95062; 22.128799, -66.313057, 360.21762; 22.069403, -65.502815, 348.74149; 22.026649, -64.646713, 337.52786; 21.964214, -63.714439, 326.37823; 21.947235, -62.72588, 315.74167; 21.918659, -61.640541, 304.33661; 21.875961, -60.488579, 293.01517; 21.853313, -59.337975, 282.34763; 21.837337, -58.059456, 271.35452; 21.78088, -56.728172, 260.48523; 21.7598, -55.395535, 250.28532; 21.721647, -53.953285, 239.4577; 21.699465, -52.417751, 228.71474; 21.6679, -50.870342, 218.30611; 21.636078, -49.240108, 207.91707];

ballLeft_x = ballLeft(:,1);
ballLeft_y = -ballLeft(:,2);
ballLeft_z = ballLeft(:,3);

ballRight_x = ballRight(:,1);
ballRight_y = -ballRight(:,2);
ballRight_z = ballRight(:,3);

ballAll_x = [ballLeft_x; ballRight_x];
ballAll_y = [ballLeft_y; ballRight_y];
ballAll_z = [ballLeft_z; ballRight_z];
%use least squares to fit a parabola to the Y Z data

%y = ax^2 + bx + c;

A_left = [ballLeft_z.^3, ballLeft_z.^2, ballLeft_z, ones(23,1)];
b_left = [ballLeft_y];

A_right = [ballRight_z.^3, ballRight_z.^2, ballRight_z, ones(23,1)];
b_right = [ballRight_y];

A_all = [ballAll_z.^3, ballAll_z.^2, ballAll_z, ones(46,1)]
b_all = [ballAll_y];

%least squares pseudo inverse
x_left = (A_left'*A_left)\A_left'*b_left
x_right = (A_right'*A_right)\A_right'*b_right
x_all = (A_all'*A_all)\A_all'*b_all
x_all_cpp = [-1.76712e-07;
            0.000425296;
            -0.316199;
            0.026123]

t = 0:1:500;
for i=1:length(t)
    y_left(i) = x_left(1,1)*t(i)^3 + x_left(2,1)*t(i)^2 + x_left(3,1)*t(i) + x_left(4,1);
    y_right(i) = x_right(1,1)*t(i)^3 + x_right(2,1)*t(i)^2 + x_right(3,1)*t(i) + x_right(4,1);
    y_all(i) = x_all(1,1)*t(i)^3 + x_all(2,1)*t(i)^2 + x_all(3,1)*t(i) + x_all(4,1);
    y_all_cpp(i) = x_all_cpp(1,1)*t(i)^3 + x_all_cpp(2,1)*t(i)^2 + x_all_cpp(3,1)*t(i) + x_all_cpp(4,1);
end


%use least squares to fit a parabola to the X Z data

%y = ax^2 + bx + c;

A_left_x = [ballLeft_z.^2, ballLeft_z, ones(23,1)];
b_left_x = [ballLeft_x];

A_right_x = [ballRight_z.^2, ballRight_z, ones(23,1)];
b_right_x = [ballRight_x];

%least squares pseudo inverse
x_left_x = (A_left_x'*A_left_x)\A_left_x'*b_left_x;
x_right_x = (A_right_x'*A_right_x)\A_right_x'*b_right_x;

for i=1:length(t)
    y_left_x(i) = x_left_x(1,1)*t(i)^2 + x_left_x(2,1)*t(i) + x_left_x(3,1);
    y_right_x(i) = x_right_x(1,1)*t(i)^2 + x_right_x(2,1)*t(i) + x_right_x(3,1);
end


figure(1)
plot(ballLeft_z,ballLeft_y,'ro',ballRight_z,ballRight_y,'bo',t,y_left,'r',t,y_right,'b',t,y_all,'g')
axis([0,500,0,80])
xlabel('Z (inches)')
ylabel('Y (inches)')
title('Baseball Catcher Ball Position Y vs Z')
legend('L measured','R measured', 'L Least-Squares', 'R Least-Squares', 'Location','northwest')

figure(2)
plot(ballLeft_z,ballLeft_x,'ro',ballRight_z,ballRight_x,'bo',t,y_left_x,'r',t,y_right_x,'b')
axis([0,500,0,40])
xlabel('Z (inches)')
ylabel('X (inches)')
title('Baseball Catcher Ball Position X vs Z')
legend('L measured','R measured', 'L Least-Squares', 'R Least-Squares','Location','northwest')


figure(3)
plot(t,y_all_cpp)

figure(4)


