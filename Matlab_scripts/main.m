%% Load data files
path = pwd;
% Add source directories
data_path = genpath('SAS');
fcn_path = genpath('Matlab_scripts');
addpath(data_path);
addpath(fcn_path);
% Find and filter files
files1=dir('SAS/files/file_raw_*.txt');
% -> select somewhere here what files are going to be processed?
% Load containing data into matlab variables/files
srate = 4000;           % sampling rate
% File 1: raw data 
[amount, dummy ] = size(files1);
file1_full_name = [files1(amount).folder '\' files1(amount).name];
file1 = getdata(file1_full_name);
file1_bio = file1(:,1)';     %value[0] : channel 1, bioimpedance measurement
file1_emg1 = file1(:,2)';    %value[1] : channel 2, emg 1 measurement
file1_emg2 = file1(:,3)';    %value[2] : channel 3, emg 2
file1_alg = file1(:,4)';     %value[3] : channel 4, analog signal.
file1_t = file1(:,5)';       %value[4] : time_ofset between last sample and actual sample

% File 2: MicroModeler Filter 
files_MM=dir('SAS/files/file_filtered_*.txt');
[amount, dummy ] = size(files_MM);
fileMM_full_name = [files_MM(amount).folder '\' files_MM(amount).name]; %Just take the last one
fileMM = getdata(file2_full_name);
MMbandstop = fileMM(:,1)';
MMnotch50 = fileMM(:,2)';
MMnotch100 = fileMM(:,3)';

% File 3: Hasomed Filters
files_iir=dir('SAS/files/file_iir_*.txt');
[amount, dummy ] = size(files_iir);
file_iir_full_name = [files_iir(amount).folder '\' files_iir(1).name]; %Just take the last one
C_data = (getdata(file_iir_full_name))';
save('Matlab_scripts/C_data_sample.mat','C_data');
C_raw = C_data(1,:);
C_Butty = C_data(2,:);
C_Cheby50 = C_data(3,:);
C_Cheby100 = C_data(4,:);

% File 3: Hasomed Filters with last session
files_iir=dir('SAS/files/CA_ind_iir_20201027_*.txt');
[amount, dummy ] = size(files_iir);
file_iir_full_name = [files_iir(1).folder '\' files_iir(2).name]; %Just take the last one
C_data = (load(file_iir_full_name))';
save('Matlab_scripts/C_data_sample.mat','C_data');
C_raw = C_data(1,:);
C_Butty = C_data(2,:);
C_Cheby50 = C_data(3,:);
C_Cheby100 = C_data(4,:);

%% Plotting data in time 
% Raw data
f1_bio_n = {'Bioimpedance', 'Data [unit]'};
f1_emg1_n = {'EMG 1', 'Data [unit]'};
f1_emg2_n = {'EMG 2', 'Data [unit]'};
f1_alg_n = {'Analog measurement', 'Data [unit]'};
f1_t = {'Time offset', 'Data [unit]'};

%Calculating time values:
t = zeros(size(file1_t));
t(1) = file1_t(1);
for i = 2:length(t)
 t(i) = t(i-1)+file1_t(i);
end
figure('Name','Raw data');
SubPlotData(file1(:,1:4)',t,[f1_bio_n; f1_emg1_n; f1_emg2_n; f1_alg_n]);

f2_1_n = {'Raw data', 'Voltage [V]'};
f2_2_n = {'Bandstop filter', 'Voltage [V]'};
f2_3_n = {'Notch50 filter', 'Voltage [V]'};
f2_4_n = {'Notch100 filter', 'Voltage [V]'};
f2_t = {'Iteration', 'Data [unit]'};
%Calculating time values:
t = zeros(size(file2_bandstop));
for i = 1:length(t)
 t(i) = i;
end
figure('Name','Filtered data');
SubPlotData(fileMM',t,[f2_1_n; f2_2_n; f2_3_n]);


H_1_n = {'Raw data', 'Voltage [V]', 'time [s]'};
H_2_n = {'Butterworth filter', 'Voltage [V]', 'time [s]'};
H_3_n = {'ChebyshevII 50Hz filter', 'Voltage [V]', 'time [s]'};
H_4_n = {'ChebyshevII 100Hz filter', 'Voltage [V]', 'time [s]'};
%H_t = {'Time', 'Data [s]'};
%Calculating time values:
t = zeros(1,length(C_data));
for i = 1:length(t)
 t(i) = i/srate;
end
figure('Name','Raw data');
plot(t,C_raw);
figure('Name','Filtered data');
plot(t,C_Cheby100);

fftEMG(C_raw);
fftEMG(C_Cheby100);
%SubPlotData(C_data,t,[H_1_n; H_2_n; H_3_n; H_4_n]);

%% Testing filters offline:
test_path = genpath('TestFilters');
addpath(test_path);
filesTest=dir('TestFilters/output/output_*.txt');
[amount, dummy ] = size(filesTest);
filet_full_name = [filesTest(amount).folder '\' filesTest(amount).name];

filetest = getdata(filet_full_name);
filet_raw = filetest(:,1)';
filet_filter1 = filetest(:,2)';
filet_filter2  = filetest(:,3)';
filet_filter3  = filetest(:,4)';
filet_filter4  = filetest(:,5)';
filet_filter5  = filetest(:,6)';

t = zeros(1,length(filet_raw));
sr = 4000;                          % Sampling rate = 4000 Hz
for i = 1:length(t)
 t(i) = i/sr;
end
figure('Name','Yesterday data');
hold on
grid on
plot(t,filet_raw,t,filet_filter3)
%plot(t,filet_raw,t,filet_filter5)

figure('Name','Yesterday data filtering process');
hold on
grid on
subplot(5,1,1)
title('Filter 1')
plot(t,filet_raw,t,filet_filter1)
subplot(5,1,2)
title('Filter 2')
plot(t,filet_filter1,t,filet_filter2)
subplot(5,1,3)
title('Filter 3')
plot(t,filet_filter2,t,filet_filter3)
subplot(5,1,4)
title('Filter 4')
plot(t,filet_filter3,t,filet_filter4)
subplot(5,1,5)
title('Filter 5')
plot(t,filet_filter4,t,filet_filter5)


%% From Dirk's example application
data2_path = genpath('sciencemode_stim_trigger');
addpath(data2_path);
directory = pwd;
raw_data=load([directory '\sciencemode_stim_trigger\sciencemode_stim_trigger\example.txt']);
filter_data=load([directory '\sciencemode_stim_trigger\sciencemode_stim_trigger\data.txt']);
[amount types] = size(filter_data);
%Calculating time values:
t = zeros(1,amount);
for i = 1:length(t)
 t(i) = i;
end
figure('Name','Dirks data');
hold on
grid on
plot(t,filter_data(:,1)',t,filter_data(:,2)')
plot(t,filter_data(:,1)',t,filter_data(:,2)')


%% Christian's filters
Fn = srate/2;
Hphz = 20/Fn;
Lphz = 300/Fn;
%[Hb,Ha]=butter(4,(Hphz/s.Rate),'high');
%[Lb,La]=butter(4,(Lphz/s.Rate),'low');
[b,a] = butter(4,[Hphz,Lphz],'bandpass');

Cheby50 = designfilt('bandstopiir', 'PassbandFrequency1', 47, 'StopbandFrequency1', 49, 'StopbandFrequency2', 51, 'PassbandFrequency2', 53, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', 4000, 'DesignMethod', 'cheby2');
Cheby100 = designfilt('bandstopiir', 'PassbandFrequency1', 97, 'StopbandFrequency1', 99, 'StopbandFrequency2', 101, 'PassbandFrequency2', 103, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', 4000, 'DesignMethod', 'cheby2');

% Transform here some data: based on a example from Matlab webpage
x = C_raw;
fs = srate;
y = fft(x);
n = length(x);          % number of samples
f = (0:n-1)*(fs/n);     % frequency range
power = abs(y).^2/n;    % power of the DFT

plot(f,power)
xlabel('Frequency')
ylabel('Power')

% Filtering data
%Filtered = filtfilt(Cheby100,(filtfilt(Cheby50,(filtfilt(b,a,C_raw)))));
MatButty =filtfilt(b,a,C_raw);
MatCheby50 = filtfilt(Cheby50,MatButty);
MatCheby100 = filtfilt(Cheby100,MatCheby50);
MatData = [C_raw; MatButty; MatCheby50; MatCheby100];

t = zeros(1,length(MatData));
for i = 1:length(t)
 t(i) = i/srate;
end
figure('Name','MATLAB Filtered data');
SubPlotData(MatData,t,[H_1_n; H_2_n; H_3_n; H_4_n]);

% Difference between the data filtered in C and the one in Matlab:
d1_n = {'Butterworth difference', 'DeltaV [V]', 'time [s]'};
d2_n = {'ChebyshevII difference', 'DeltaV [V]', 'time [s]'};
d3_n = {'ChebyshevII difference', 'DeltaV [V]', 'time [s]'};

diff = [C_Butty-MatButty; C_Cheby50-MatCheby50; C_Cheby100-MatCheby100];

SubPlotData(diff,t,[d1_n; d2_n; d3_n]);

% Proportional difference respecting to the Matlab Values:
diff_dV = diff;
for i = 1:length(t)
 diff_dV(1,i) = diff(1,i)/MatButty(i); 
 diff_dV(2,i) = diff(2,i)/MatCheby50(i); 
 diff_dV(3,i) = diff(3,i)/MatCheby100(i); 
end
d1_n = {'Butterworth difference', 'DeltaV [%]', 'time [s]'};
d2_n = {'ChebyshevII 50 difference', 'DeltaV [%]', 'time [s]'};
d3_n = {'ChebyshevII 100 difference', 'DeltaV [%]', 'time [s]'};

SubPlotData(diff_dV,t,[d1_n; d2_n; d3_n]);
subplot(2,2,1)
ylim([-100 100])
subplot(2,2,2)
ylim([-100 100])
subplot(2,2,3)
ylim([-100 100])