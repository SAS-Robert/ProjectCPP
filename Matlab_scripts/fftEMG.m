function [outArg] = fftEMG(ai0,name, fs)
% Calculates and plots the frequency domain of a function
% - ai0: input vector with time domain data.
% - name: string title for the plot.
% - fs: sampling frequency. 

T = 1/fs;             % Sampling period
L = length(ai0);             % Length of signal
t = (0:L-1)*T;        % Time vector
Y = fft(ai0);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = fs*(0:(L/2))/L;
%figure
grid on
plot(f,P1) 
title(name)
xlabel('f (Hz)')
ylabel('|P1(f)|')

outArg = P1;
end