%% Load data files
path = pwd;
% Add source directories
data_path = genpath('MoveIngest');
fcn_path = genpath('Matlab_scripts');
addpath(data_path);
addpath(fcn_path);
% Find and filter files
files1=dir('MoveIngest/files/file_raw_*.txt');
% -> select somewhere here what files are going to be processed?
% Load containing data into matlab variables/files
[amount, dummy ] = size(files1);
file1_full_name = [files1(amount).folder '\' files1(amount).name];
file1 = getdata(file1_full_name);
%file1 = load(file1_full_name);
file1_bio = file1(:,1)';     %value[0] : channel 1, bioimpedance measurement
file1_emg1 = file1(:,2)';    %value[1] : channel 2, emg 1 measurement
file1_emg2 = file1(:,3)';    %value[2] : channel 3, emg 2
file1_alg = file1(:,4)';     %value[3] : channel 4, analog signal.
file1_t = file1(:,5)';       %value[4] : time_ofset between last sample and actual sample
% -> necessary to add files 2 and 3
files2=dir('MoveIngest/file_filtered_*.txt');
[amount, dummy ] = size(files2);
file2_full_name = [files2(amount).folder '\' files2(amount).name]; %Just take the last one
%file2 = load(file2_full_name);
file2 = getdata(file2_full_name);
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
SubPlotData(file2',t,[f2_1_n; f2_2_n; f2_3_n]);

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
for i = 1:length(t)
 t(i) = i;
end
figure('Name','Yesterday data');
hold on
grid on
plot(t,filet_raw,t,filet_filter5)

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