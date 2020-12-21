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
session4_backUp = [C_files_backup '/session_13Nov/'];
% CUL data
data_dir_session4 = dir([session4_backUp 'CUL_filter*.txt']);
[amount dummy] = size(data_dir_session4);
[s4raw_t, s4raw_f] = samples_analysis(data_dir_session4,'C',0,'Recorded raw data');
[s4c_t, s4c_f] = samples_analysis(data_dir_session4,'C',1,'SAS filtered data');

files_dir = [session4_backUp '/CUL'];
for k=1:amount
    name = ['Session 13th Nov, CUL recording nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end

% JW data 
data_dir_session4 = dir([session4_backUp 'JW_filter*.txt']);
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


% Original files?
files_dir = [session4_backUp 'CA'];
k = 3;
    name = ['Session 13th Nov, CA recording nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C'); 
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
% Testing electrodes placement:
session5_backUp = [C_files_backup '/session_16Nov/'];
% all data
data_dir_session5 = dir([session5_backUp 'CA_leg_filter*.txt']);

% Comparing how different the filtering looks on the lower leg
samples_analysis(data_dir_session5([1 3 6]),'C',1,'SAS filtered data');

% Comparing how different the filtering looks on the upper leg
samples_analysis(data_dir_session5([8 11]),'C',1,'SAS filtered data');

files_dir = [session5_backUp 'CA_leg'];
k = 3;
    name = ['Session 13th Nov, CA recording nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C'); 

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


%% Only th for the report
% Session 3
files_dir = [C_files_backup '/test_session_10Nov/CUL'];
pos = [1:3];
set_name = {'Retified-Filtered data and mean values - Recording 1';
            'Retified-Filtered data and mean values - Recording 2';
            'Retified-Filtered data and mean values - Recording 3';
               'Session 10th Nov false end-trajectories'};
data = only_th(files_dir,set_name,pos);

% Session 4
% CUL
session4_backUp = [C_files_backup '/session_13Nov/'];
files_dir = [session4_backUp '/CUL'];
% the complete disaster toguether
pos = [1:7];
set_name = {'Retified-Filtered data and mean values - Recording 1';
            'Retified-Filtered data and mean values - Recording 2';
            'Retified-Filtered data and mean values - Recording 3';
            'Retified-Filtered data and mean values - Recording 4';
            'Retified-Filtered data and mean values - Recording 5';
            'Retified-Filtered data and mean values - Recording 6';
            'Retified-Filtered data and mean values - Recording 7';
               'Session 13th Nov different thresholds approach'};
data = only_th(files_dir,set_name,pos);

% only the relevant ones
pos = [2 4 5];
set_name = {'Recording 2: threshold = mean*2.5';
            'Recording 4: threshold = mean + sd/2';
            'Recording 5: threshold = mean + sd/3';
            'Session 13th Nov different thresholds approach'};
data = only_th(files_dir,set_name,pos);

% JW
session4_backUp = [C_files_backup '/session_13Nov/'];
files_dir = [session4_backUp '/JW'];
% all of it
pos = [1:3];
set_name = {'Recording 8: threshold = mean + sd/4';
            'Recording 9: threshold = mean + sd/4';
            'Recording 10: threshold = mean + sd/4';
            'Session 13th Nov same threshold'};
data = only_th(files_dir,set_name,pos);

%% ===================== Others demo and data =====================
%% Demo 18th November
subject1_name = [data_address '/subject1'];
subject1_dir=dir([subject1_name '_filter*']);
[amount dummy] = size(subject1_dir);
% Only raw and filtered EMG data
[emg_raw_t1, emg_raw_f] = samples_analysis(subject1_dir,'C',0,'Raw data EMG demo 1');
[emg_f_t1, emg_f_f] = samples_analysis(subject1_dir,'C',1,'SAS filtered EMG demo 1');

% - Individually 
% Time domain + Frequency domain + mean in a single figure 
% set1 = upper leg.  set 2 = lower leg
set1 = [1:3];
for k=1:length(set1)
    name = ['Demo 18th Nov, upper leg recording nr.' num2str(set1(k))];
    data = plot_th3(subject1_name,name,set1(k),'C');    
end

set2 = 4;
name = ['Demo 18th Nov, lower leg recording nr.' num2str(set2)];
data = plot_th3(subject1_name,name,set2,'C');  

% - Multiple toguether
set_com = [1:3];
set_com_name = {'Lower leg recording 1';
                'Lower leg recording 2';
                'Lower leg recording 3';
               'Demo 18th Nov, lowe leg recording'};
data = only_th(subject1_name,set_com_name,set_com);

%% Session 23-11-2020
% I do not remember what we wanted to do in this session or why
% Files dir
data_path = genpath('SAS/session_23Nov');
addpath(data_path);
% CA data
data_dir_session6 = dir(['SAS/session_23Nov/CUL_lower_leg_filter*.txt']);
[amount dummy] = size(data_dir_session6);
files_dir = ['SAS/session_23Nov/CUL_lower_leg'];

% individually 
for k=1:amount
    name = ['Session 23rd Nov, CUL lower leg recording nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end

%% Demo 26-11-2020 -> This was with the FOCUS group = clinical people
% Files dir
data_path = genpath([C_files_backup '/demo_26Nov']);
addpath(data_path);
% CA data
data_dir_session6 = dir([C_files_backup '/demo_26Nov/CA_lower_leg1_filter*.txt']);
[amount dummy] = size(data_dir_session6);
files_dir = [C_files_backup '/demo_26Nov/CA_lower_leg1'];

% individually 
for k=1:amount
    name = ['Demo 26th Nov, CA lower leg(1) recording nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end

[s6raw_t, s6raw_f] = samples_analysis(data_dir_session6(end),'C',0,'Recorded raw data');
[s6c_t, s6c_f] = samples_analysis(data_dir_session6(end),'C',1,'SAS filtered data');

% V2 data
data_dir_session6 = dir(['SAS/demo_26Nov/V2_upper_leg_filter*.txt']);
[amount dummy] = size(data_dir_session6);
files_dir = ['SAS/demo_26Nov/V2_upper_leg'];

% individually 
for k=1:amount
    name = ['Demo 26th Nov, Volunteer 2 upper leg recording nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end

%% ===================== Timing perfomance evaluation =====================
% Time analysis perfomance 
% -time 1 = from when threshold has been passed until the stimulator starts (FES_cnt)
% -time 2 = filtering perfomance (EMG_tic)
% -time 3 = thread 1 process perfomance: state machine + recorder + stimulator time
% -time 4 = thread 2 interface perfomance: keyboard + UDP + TCP time

% Session 4 13th Nov
files1=dir([C_files_backup '/session_13Nov/CUL_time1_*']);
[amount1, dummy ] = size(files1);
files2=dir([C_files_backup '/session_13Nov/JW_time1_*']);
[amount2, dummy ] = size(files2);
files3=dir([C_files_backup '/session_13Nov/CA_time1_*']);
[amount3, dummy ] = size(files3);

% times 1 and 2
fprintf('%%MATLAB GENERATED DATA\n');
for k=1:amount1
    fprintf('%%Recording %i\n', k);
    fprintf('%i & ', k);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name], [1 2], [10 10]);
    fprintf('\\hline\n');
end
fprintf('%%MATLAB GENERATED DATA\n');
for k=1:amount2
    fprintf('%%Recording %i\n', (k+amount1));
    fprintf('%i & ', (k+amount1));
    [value_av,value_max,val_samples] = timing_eval([files2(k).folder '\' files2(k).name], [1 2], [10 10]);
    fprintf('\\hline\n');
end
fprintf('%%MATLAB GENERATED DATA\n');
for k=1:amount3
    fprintf('%%Recording %i\n', (k+amount1+amount2));
    fprintf('%i & ', (k+amount1+amount2));
    [value_av,value_max,val_samples] = timing_eval([files3(k).folder '\' files3(k).name], [1 2], [10 10]);
    fprintf('\\hline\n');
end

% times 3 and 4 (thread 1 and thread 2)
fprintf('%%MATLAB GENERATED DATA\n');
for k=1:amount1
    fprintf('%%Recording %i\n', k);
    fprintf('%i & ', k);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name], [3 4], [100 150]);
    fprintf('\\hline\n');
end
fprintf('%%MATLAB GENERATED DATA\n');
for k=1:amount2
    fprintf('%%Recording %i\n', (k+amount1));
    fprintf('%i & ', (k+amount1));
    [value_av,value_max,val_samples] = timing_eval([files2(k).folder '\' files2(k).name], [3 4], [100 150]);
    fprintf('\\hline\n');
end
fprintf('%%MATLAB GENERATED DATA\n');
for k=1:amount3
    fprintf('%%Recording %i\n', (k+amount1+amount2));
    fprintf('%i & ', (k+amount1+amount2));
    [value_av,value_max,val_samples] = timing_eval([files3(k).folder '\' files3(k).name], [3 4], [100 150]);
    fprintf('\\hline\n');
end

% Session 5 16th Nov
files1=dir([C_files_backup '/session_16Nov/CA_leg_time1_*']);
[amount, dummy ] = size(files1);
% recording 10 and 11 not valid
fprintf('%%MATLAB GENERATED DATA\n');
for k=1:amount
    fprintf('%%Recording %i\n', k);
    fprintf('%i & ', k);
    [value_av,value_sd,val_samples] = timing_eval([files1(k).folder '\' files1(k).name], [1 2], [10 10]);
    fprintf('\\hline\n');
end
% times 3 and 4 (thread 1 and thread 2)
fprintf('%%MATLAB GENERATED DATA\n');
for k=1:amount
    fprintf('%%Recording %i\n', k);
    fprintf('%i & ', k);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name], [3 4], [100 150]);
    fprintf('\\hline\n');
end

% Session 6 9th December
files1=dir(['C:\Users\Carolina\Desktop\Internship\Software\C_files_backup\test_09Dec\' 'CA_upper_leg_time1_*.txt']);
[amount, dummy ] = size(files1);

% recording 10 and 11 not valid
fprintf('%%MATLAB GENERATED DATA\n');
for k=1:7
    fprintf('%%Recording %i\n', k);
    fprintf('%i & ', k);
    [value_av,value_sd,val_samples] = timing_eval([files1(k).folder '\' files1(k).name], [1 2], [10 10]);
    fprintf('\\hline\n');
end
% times 3 and 4 (thread 1 and thread 2)
fprintf('%%MATLAB GENERATED DATA\n');
for k=1:9
    fprintf('%%Recording %i\n', k);
    fprintf('%i & ', k);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name], [3 4], [33 125]);
    fprintf('\\hline\n');
end

