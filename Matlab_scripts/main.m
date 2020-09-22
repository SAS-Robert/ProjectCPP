%% Load data files
path = pwd;
% Add source directories
data_path = genpath('MoveIngest');
fcn_path = genpath('Matlab_scripts');
addpath(data_path);
addpath(fcn_path);
% Find and filter files
files1=dir('MoveIngest/file1_*.txt');
% -> select somewhere here what files are going to be processed?
% Load containing data into matlab variables/files
[amount, dummy ] = size(files1);
file1_full_name = [files1(amount).folder '\' files1(amount).name];
file1 = load(file1_full_name);
file1_bio = file1(:,1)';     %value[0] : channel 1, bioimpedance measurement
file1_emg1 = file1(:,2)';    %value[1] : channel 2, emg 1 measurement
file1_emg2 = file1(:,3)';    %value[2] : channel 3, emg 2
file1_alg = file1(:,4)';     %value[3] : channel 4, analog signal.
file1_t = file1(:,5)';       %value[4] : time_ofset between last sample and actual sample
% -> necessary to add files 2 and 3
files2=dir('MoveIngest/file2_*.txt');
[amount, dummy ] = size(files2);
file2_full_name = [files2(amount).folder '\' files2(amount).name]; %Just take the last one
file2 = load(file2_full_name);
file2_data = file2';
file2_bandstop = file2(:,1)';
file2_notch50 = file2(:,2)';
file2_notch100 = file2(:,3)';

%% Plotting data
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

f2_1_n = {'Bandstop filter', 'Data [unit]'};
f2_2_n = {'Notch50 filter', 'Data [unit]'};
f2_3_n = {'Notch100 filter', 'Data [unit]'};
f2_t = {'Iteration', 'Data [unit]'};
%Calculating time values:
t = zeros(size(file2_bandstop));
for i = 1:length(t)
 t(i) = i;
end
figure('Name','Filtered data');
SubPlotData(file2',t,[f2_1_n; f2_2_n; f2_3_n]);
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