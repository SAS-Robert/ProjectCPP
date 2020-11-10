%% Test session on CUL's leg
path = pwd;
% Add source directories
%data_path = genpath('SAS/files');
data_path = genpath('SAS/test_session_09Nov');
fcn_path = genpath('Matlab_scripts');
addpath(data_path);
addpath(fcn_path);

%%
data_dir = dir('SAS/test_session_09Nov/CUL_filter*.txt');
th_dir = dir('SAS/test_session_09Nov/CUL_th*.txt');
[amount dummy] = size(data_dir);
[raw_t, raw_f] = samples_analysis(data_dir,'C',0,'Recorded raw data');
[c_t, c_f] = samples_analysis(data_dir,'C',1,'SAS filtered data');


files_dir = 'SAS/test_session_09Nov/CUL';
for k=1:amount
    name = ['Recording 9th Nov, nr.' num2str(k)];
    data = plot_th2(files_dir,name,k,'C');    
end


[c_t, c_f] = samples_analysis(data_dir(2),'C',1,'SAS filtered data');

% peak
srate = 1000;

t11 = 34.04; t12 = 34.4;
peak1 = c_t(t11*srate:t12*srate);
t_peak1 = [t11:(1/srate):t12];

t21 = 34.4; t22 = 37;
peak2 = c_t(t21*srate:t22*srate);
t_peak2 = [t21:(1/srate):t22];

figure('Name','Analysing peaks')
subplot(2,2,1)
plot(t_peak1, peak1)
xlim([t_peak1(1) t_peak1(end)])
ylim([0 max(peak1)])
subplot(2,2,2)
peak1_f = fftEMG(peak1,['Frequency domain EMG 1'],srate);

subplot(2,2,3)
plot(t_peak2, peak2)
xlim([t_peak2(1) t_peak2(end)])
ylim([0 max(peak2)])

subplot(2,2,4)
peak2_f = fftEMG(peak2,['Frequency domain EMG 2'],srate);

% peak (part 2)
t11 = 42.55; t12 = 42.73;
peak1 = c_t(t11*srate:t12*srate);
t_peak1 = [t11:(1/srate):t12];

t21 = t12; t22 = 45.3;
peak2 = c_t(t21*srate:t22*srate);
t_peak2 = [t21:(1/srate):t22];

figure('Name','Analysing peaks')
subplot(2,2,1)
plot(t_peak1, peak1)
xlim([t_peak1(1) t_peak1(end)])
ylim([0 max(peak1)])
subplot(2,2,2)
peak1_f = fftEMG(peak1,['Frequency domain EMG 1'],srate);

subplot(2,2,3)
plot(t_peak2, peak2)
xlim([t_peak2(1) t_peak2(end)])
ylim([0 max(peak2)])

subplot(2,2,4)
peak2_f = fftEMG(peak2,['Frequency domain EMG 2'],srate);

% peak (part 3)
t31 = 58.87; t32 = 61.17;
peak3 = c_t(t31*srate:t32*srate);
t_peak3 = [t31:(1/srate):t32];

figure('Name','Analysing peaks (3)')
subplot(1,2,1)
plot(t_peak3, abs(peak3))
xlim([t_peak3(1) t_peak3(end)])
ylim([0 max(peak1)])
subplot(1,2,2)
peak1_f = fftEMG(peak3,['Frequency domain EMG 1'],srate);