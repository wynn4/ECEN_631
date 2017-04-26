clc;
close all;

true_data = importdata('VO Practice Sequence R and T.txt');
my_data = importdata('my_vo_data_practice.txt');
my_hall_data = importdata('my_vo_data_hall.txt');

Tx_true = true_data(:,4);
Ty_true = true_data(:,8);
Tz_true = true_data(:,12);

Tx_est = my_data(:,4);
Ty_est = my_data(:,8);
Tz_est = my_data(:,12);

Tx_hall = my_hall_data(:,4);
Ty_hall = my_hall_data(:,8);
Tz_hall = my_hall_data(:,12);

figure(1)
plot(Tx_true,Tz_true)
hold on
plot(Tx_est, Tz_est, '-.')
xlabel('x-translation')
ylabel('z-translation')
title('Practice VO Sequence')
legend('Truth', 'VO Estimate')
axis equal

figure(2)
plot(Tx_hall, Tz_hall, '-.')
xlabel('x-translation')
ylabel('z-translation')
title('BYU Hallway VO Sequence')
legend('VO Estimate')
axis([-50 1250 -250 550])
axis equal
