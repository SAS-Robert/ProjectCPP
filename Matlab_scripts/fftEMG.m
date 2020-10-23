function fftEMG(fft)
fs = 10000;             % Sampling frequency
T = 1/fs;             % Sampling period
L = length(ai0);             % Length of signal
t = (0:L-1)*T;        % Time vector
Y = fft(ai0);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = fs*(0:(L/2))/L;
plot(f,P1) 
title('Single-Sided Amplitude Spectrum of X(t)')
xlabel('f (Hz)')
ylabel('|P1(f)|')
end