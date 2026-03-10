graphics_toolkit('gnuplot');

data = csvread('LOG.CSV', 1, 0);

time = data(:,1) / 1000

figure;
plot(time, data(:,2));
title('Druck');

figure;
plot(time, data(:,3));
title('Höhe');

figure;
plot(time, data(:,4));
title('Temperatur');

figure;
plot(time, data(:,5:end));
title('GX, GY, GZ');
