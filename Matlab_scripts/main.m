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


%% Sessions 1-3 ( beginning Nov) -> Directly going to the figures:
fig_path = genpath('C:\Users\Carolina\Desktop\Internship\Software\Matlab_figs_and_data');
addpath(fig_path);

% With Chebyshev II filters: session 04th Nov
open('2020_11_04_record1_stim.fig');
open('2020_11_04_record2_stim.fig');
%open('2020_11_04_record3_no_stim.fig');
%open('2020_11_04_record4_no_stim.fig');

% With 1st order Butterworth filters instead of Chebyshev II: session 9th Nov
open('2020_11_09_record1.fig');
open('2020_11_09_record2.fig');
open('2020_11_09_record3.fig');
open('2020_11_09_record4.fig');

% Adding the mean-under-the-threshold condition and different thresholds
open('2020_11_10_record1.fig');
open('2020_11_10_record2.fig');
open('2020_11_10_record3.fig');
% Butterworth filters modified to 2 order and a bigger bandwidth
open('2020_11_10_record4.fig');
open('2020_11_10_record5.fig');
open('2020_11_10_record6.fig');

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
test_filts(files_dir(file_select),'M',['Filtered data with Butterworth  order = 2, bandwidth = 4.5'],2,4.5);

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