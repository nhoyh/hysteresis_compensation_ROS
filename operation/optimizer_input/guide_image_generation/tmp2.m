clear all
close all
clc


x = 0.001;
% 2pif = w
f = x * 1000 / 2 / pi;
w = 2*pi*f
T = 1/f


%% sin_magnitude/2 - sin_magnitude*(cos(time))/2;
time = 0:0.001:25;
mag = 60;
sin_magnitude = 60;
a = sin(0.5*time)+sin(0.25*time)+sin(time);

sin_input = (1.2*sin_magnitude/2 - 1.2*sin_magnitude*(cos(time))/2 + sin_magnitude/2 - sin_magnitude*(cos(0.25*time))/2 + sin_magnitude/2 - sin_magnitude*(cos(0.5*time))/2)*60/225;

b = mag/2 - mag/2*cos(w*time);
% b = (mag*1.2 - mag*(cos(time)*1.2) + mag/2 -  mag*(cos(0.5*time))/2 +mag/2 -  mag*(cos(0.25*time))/2)*60/225 ;
plot(sin_input);