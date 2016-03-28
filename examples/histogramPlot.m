% Histogram viewer.
% This file plots the histogram data from the file histogram.dat
% See PPM.t.cpp

clear all;
load histogram.dat;
figure(1);
hold on;
i=1:256;
plot(histogram(i),'r');

title ('Image histogram');
ylabel('count');
xlabel('intensity');
grid;
hold off;
