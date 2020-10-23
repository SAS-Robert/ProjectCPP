function [result] = fftPlot(m)
x = m(:,1);
y = m(:,2);
z = m(:,3);
fs = 10000;             % Sampling frequency
T = 1/fs;             % Sampling period
L = length(x);             % Length of signal
t = (0:L-1)*T;        % Time vector
X = fft(x);
Y = fft(y);
Z = fft(z);
P2_x = abs(X/L);
P2_y = abs(Y/L);
P2_z = abs(Z/L);

P1_x = P2_x(1:L/2+1);
P1_x(2:end-1) = 2*P1_x(2:end-1);

P1_y = P2_x(1:L/2+1);
P1_y(2:end-1) = 2*P1_y(2:end-1);

P1_z = P2_x(1:L/2+1);
P1_z(2:end-1) = 2*P1_z(2:end-1);


f = fs*(0:(L/2))/L;
%plot(f,P1_x) 
disp(size(f))
disp(size(P1_x))
disp(size(P1_y))
disp(size(P1_z))

[result] =[f' P1_x P1_y P1_z];