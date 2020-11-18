%% Load data files
path = pwd;
% Add source directories
data_path = genpath('SAS/files');
fcn_path = genpath('Matlab_scripts');
addpath(data_path);
addpath(fcn_path);

C_files_backup = 'C:\Users\Carolina\Desktop\Internship\Software\C_files_backup';
backup_path = genpath(C_files_backup);
addpath(backup_path);

%% Session 0: testing the recorder with electrodes on the arm and at the desk
CUL_dir=dir([C_files_backup '/files/CUL_arm_raw_*']);
[emg_raw_t1, emg_raw_f] = samples_analysis(CUL_dir,'C',0,'Raw data EMG 1');
[emg_raw_t2, emg_raw_f] = samples_analysis(CUL_dir,'C',0,'Raw data EMG 2');

[emg_f_t1, emg_f_f] = samples_analysis(CUL_dir,'C',1,'SAS filtered data EMG 1');
[emg_f_t2, emg_f_f] = samples_analysis(CUL_dir,'C',1,'SAS filtered data EMG 2');


%% Sessions 1-4 (November) -> Directly going to the figures:
fig_path = genpath('C:\Users\Carolina\Desktop\Internship\Software\Matlab_figs_and_data');
addpath(fig_path);

% S1 With Chebyshev II filters: session 04th Nov
open('2020_11_04_record1_stim.fig');
open('2020_11_04_record2_stim.fig');
open('2020_11_04_record3_no_stim.fig');
open('2020_11_04_record4_no_stim.fig');

% S2 With 1st order Butterworth filters instead of Chebyshev II: session 9th Nov
open('2020_11_09_record1.fig');
open('2020_11_09_record2.fig');
open('2020_11_09_record3.fig');
open('2020_11_09_record4.fig');

% S3 Adding the mean-under-the-threshold condition and different thresholds
open('2020_11_10_record1.fig');
open('2020_11_10_record2.fig');
open('2020_11_10_record3.fig');
% Butterworth filters modified to 2 order and a bigger bandwidth
open('2020_11_10_record4.fig');
open('2020_11_10_record5.fig');
open('2020_11_10_record6.fig');

% S4 With new volunteer. Trying out new threshold methods.
% CUL - different thresholds
open('2020_11_13_record1_no_stim_CUL.fig'); % stimulator was not triggered
open('2020_11_13_record2_CUL.fig');
open('2020_11_13_record3_CUL.fig');
open('2020_11_13_record4_CUL.fig');
open('2020_11_13_record5_CUL.fig');
open('2020_11_13_record6_CUL.fig');
open('2020_11_13_record7_CUL.fig');
% JW - new subject. Always same threshold method
open('2020_11_13_record1_JW.fig');
open('2020_11_13_record2_JW.fig');
open('2020_11_13_record3_JW.fig');
% CA - new filters and electrodes placement
open('2020_11_13_record1_CA.fig');
open('2020_11_13_record2_CA.fig');
open('2020_11_13_record3_CA.fig');
open('2020_11_13_record4_CA.fig');
open('2020_11_13_record5_CA.fig');
% Complete EMGS data sets:
open('2020_11_13_CUL_rawdata.fig');
open('2020_11_13_CUL_filterdata.fig');
open('2020_11_13_JW_rawdata.fig');
open('2020_11_13_JW_filterdata.fig');
open('2020_11_13_CA_rawdata.fig');
open('2020_11_13_CA_filterdata.fig');

% S5 Lower leg + Upper leg with different electrodes placement
% Lower leg
open('2020_11_16_record1_set1_CA.fig'); % electrodes between FES
open('2020_11_16_record2_set2_CA.fig');
open('2020_11_16_record3_set1_CA.fig'); % ground on the knee
open('2020_11_16_record4_set2_CA.fig');
open('2020_11_16_record6_set1_CA.fig'); % electrodes outside FES
open('2020_11_16_record7_set2_CA.fig');
% Upper leg
open('2020_11_16_record8_set3_CA.fig'); % electrodes close to the knee
open('2020_11_16_record9_set3_CA.fig');
open('2020_11_16_record10_set3_CA.fig'); % electrodes on the side
open('2020_11_16_record11_set3_CA.fig');
% Comparison between placements
open('2020_11_16_set1_CA.fig'); % lower leg with different placements 
open('2020_11_16_set2_CA.fig');
open('2020_11_16_set3_CA.fig'); % upper leg with different placements

%% Sessions 1-3 ( beginning Nov) -> Loading data to generate the figures 
% Session 1, 4th Nov 2020
% Goal: test complete set up
% Result: Chebyshev II filters too little stable. Stimulator getting always
% triggered because of this issue.
% Note: only plotting the most relevant one.
data_dir_session1 = dir([C_files_backup '/test/after lunch/CUL_leg_filter*.txt']);
[amount dummy] = size(data_dir_session1);
[s1raw_t, s1raw_f] = samples_analysis(data_dir_session1,'C',0,'Recorded raw data');
[s1c_t, s1c_f] = samples_analysis(data_dir_session1,'C',1,'SAS filtered data');

files_dir_s1 = [C_files_backup '/test/after lunch/CUL_leg'];
for k=1:amount
    name = ['Recording 04th Nov, nr.' num2str(k)];
    data = plot_th(files_dir_s1,name,k,'C');    
end

% Session 2, 9th Nov 2020
% Goal: test 1st order Butterworth filters
% Result: filters kinda noisy, stimulator trigger too easily and
% (sometines) unexpectedly.
data_dir_session2 = dir([C_files_backup '/test_session_09Nov/CUL_filter*.txt']);
[amount dummy] = size(data_dir_session2);
[s2raw_t, s2raw_f] = samples_analysis(data_dir_session2,'C',0,'Recorded raw data');
[s2c_t, s2c_f] = samples_analysis(data_dir_session2,'C',1,'SAS filtered data');

files_dir_s2 = [C_files_backup '/test_session_09Nov/CUL'];
for k=1:amount
    name = ['Recording 09th Nov, nr.' num2str(k)];
    data = plot_th2(files_dir_s2,name,k,'C');    
end

% Session 3, 10th Nov 2020
% Goal: test 2nd order Butterworth filters + different thresholds
% Results: filters much better behaviour. Threshold still quite messy.
data_dir_session3 = dir([C_files_backup '/test_session_10Nov/CUL_filter*.txt']);
[amount dummy] = size(data_dir_session3);
[s3raw_t, s3raw_f] = samples_analysis(data_dir_session3,'C',0,'Recorded raw data');
[s3c_t, s3c_f] = samples_analysis(data_dir_session3,'C',1,'SAS filtered data');

files_dir = [C_files_backup '/test_session_10Nov/CUL'];
for k=1:amount
    name = ['Recording 10th Nov, nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end

%% Remarks: session 2 peaks
[s2c_t, s2c_f] = samples_analysis(data_dir_session2(2),'C',1,'SAS filtered data');
% peak 1 
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

%% Session 4 - 13th Nov
data_path = genpath('SAS/session_13Nov');
addpath(data_path);
% CUL data
data_dir_session4 = dir(['SAS/session_13Nov/CUL_filter*.txt']);
[amount dummy] = size(data_dir_session4);
[s4raw_t, s4raw_f] = samples_analysis(data_dir_session4,'C',0,'Recorded raw data');
[s4c_t, s4c_f] = samples_analysis(data_dir_session4,'C',1,'SAS filtered data');

files_dir = ['SAS/session_13Nov/CUL'];
for k=1:amount
    name = ['Session 13th Nov, CUL recording nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end

% JW data 
data_dir_session4 = dir(['SAS/session_13Nov/JW_filter*.txt']);
[amount dummy] = size(data_dir_session4);
files_dir = ['SAS/session_13Nov/JW'];
for k=1:amount
    name = ['Session 13th Nov, JW recording nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end
[s4raw_t, s4raw_f] = samples_analysis(data_dir_session4,'C',0,'Recorded raw data');
[s4c_t, s4c_f] = samples_analysis(data_dir_session4,'C',1,'SAS filtered data');

% CA data - New electrodes placement and filters
address_2 = 'C:\Users\Carolina\Desktop\Internship\Software\ProjectCPP\TestFilters\output\';
data_dir_session4 = dir([address_2 'CA_filter*.txt']);
[amount dummy] = size(data_dir_session4);

files_dir = [address_2 'CA'];
for k=1:amount
    name = ['Session 13th Nov, CA recording nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end
[s4raw_t, s4raw_f] = samples_analysis(data_dir_session4,'C',0,'Recorded raw data');
[s4c_t, s4c_f] = samples_analysis(data_dir_session4,'C',1,'SAS filtered data');

%% Remarks: session 4 peaks and transition time
[s4c_t, s4c_f] = samples_analysis(data_dir_session4(end),'C',1,'SAS filtered data');% peak 1 
srate = 1000;
t11 = 117.0; t12 = 123.0;
peak1 = s4c_t(t11*srate:t12*srate);
t_peak1 = [t11:(1/srate):t12];

figure('Name','Analysing peaks')
subplot(1,2,1)
plot(t_peak1, peak1)
xlim([t_peak1(1) t_peak1(end)])
ylim([0 max(peak1)])
subplot(1,2,2)
peak1_f = fftEMG(peak1,['Frequency domain peak 1'],srate);

figure('Name','Spectogram on peak1')
spectogram(peak1)

figure('Name','Spectogram on EMG')
spectrogram(s4c_t)

%% Session 5 - 16th Nov 
% Testing new and old device on the desk (without stimulator)


%% Testing different filters on Matlab
% This is useful to tune filters and choose what would suit better

% select raw data to process 
files_dir = dir([C_files_backup '/test/after lunch/CUL_leg_filter*.txt']);
file_select = 1;                % choose a concrete set of data
% Raw and different types of filters
analysis(files_dir(file_select),'0','Raw data');
analysis(files_dir(file_select),'C','SAS filtered data');
analysis(files_dir(file_select),'M','Filtered data with Matlab');

% Playing with the Butterworth order nr.
bandwidth = 2;
for j=1:5
test_filts(files_dir(file_select),'M',['Filtered data with Butterworth order nr.' num2str(j)],j,bandwidth);
end

% Playing with the Butterworth bandwidth
order = 1;
for j=2:2:10
test_filts(files_dir(file_select),'M',['Filtered data with Butterworth  bandwidth = ' num2str(j)],order,j);
end
% Trying a fixed value on a concrete data set
test_filts(files_dir(file_select),'M',['Filtered data with Butterworth  order = 2, bandwidth = 4.5'],1,4.5);

%% Testing different filters on the TestFilters.cpp program
% This is to check if the set up on the filters on C is good, run different
% filters test on previous sessions raw data and without having to wait for
% a complete prototype set up to analyse the filters results on C++

test_dir = dir([C_files_backup '/output/out_CUL_leg_filter*.txt']);
analysis(test_dir(1),'0','SAS raw data');
analysis(test_dir(1),'C','SAS raw data');

[c_t, c_f] = samples_analysis(test_dir(1),'C',1,'SAS filtered data');


% 'C' = original used filters (when the original data was recorded) vs new 
% filtering done on TestFilters.cpp
% 'T' = original used filters (when the original data was recorded) vs new 
% filtering done on Matlab

test_dir_string = [C_files_backup '/output/out_CUL_leg'];
for k=2:4
    name = ['Christians leg recording after lunch, nr.' num2str(k)];
%    data = plot_th(test_dir_string,name,k,'C');    
    data = plot_th(test_dir_string,name,k,'T');    
end