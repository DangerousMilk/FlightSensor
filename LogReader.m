close all;
clear all;
clc;

graphics_toolkit('gnuplot');

data = csvread('LOG.CSV', 1, 0);

time = data(:,1) / 1000

smoothing = 200;

figure;
title('Druck');
plot(time, movmean(data(:,2), smoothing));
xlabel('t in s');
ylabel('Druck in hPa');

figure;
title('Hoehe');
plot(time, movmean(data(:,3), smoothing));
xlabel('t in s');
ylabel('Hoehe in m');

figure;
title('Temperatur');
plot(time, movmean(data(:,4), smoothing));
xlabel('t in s');
ylabel('Temp. in C');

#plot(time, data(:,5:end));
#title('GX, GY, GZ');

figure;
title('Beschleunigung X');
plot(time, movmean(data(:,5), smoothing));
xlabel('t in s');
ylabel('Beschleunigung X in m/s^2');

figure;
title('Beschleunigung Y');
plot(time, movmean(data(:,6), smoothing));
xlabel('t in s');
ylabel('Beschleunigung Y in m/s^2');

figure;
title('Beschleunigung Z');
plot(time, movmean(data(:,7), smoothing));
xlabel('t in s');
ylabel('Beschleunigung Z in m/s^2');

