%% Test session on CUL's leg
path = pwd;
% Add source directories
%data_path = genpath('SAS/files');
data_path = genpath('SAS/test_session_10Nov');
fcn_path = genpath('Matlab_scripts');
addpath(data_path);
addpath(fcn_path);

%%
data_dir = dir('SAS/test_session_10Nov/CUL_filter*.txt');
th_dir = dir('SAS/test_session_10Nov/CUL_th*.txt');
[amount dummy] = size(data_dir);
[raw_t, raw_f] = samples_analysis(data_dir,'C',0,'Recorded raw data');
[c_t, c_f] = samples_analysis(data_dir,'C',1,'SAS filtered data');


files_dir = 'SAS/test_session_10Nov/CUL';
for k=1:amount
    name = ['Recording 10th Nov, nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end

%% Directly going to the figures:
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