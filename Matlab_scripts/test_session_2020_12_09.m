% Test session, 9th Dec 2020
% Goal: test 2nd order Butterworth filters + different thresholds
% Results: filters much better behaviour. Threshold still quite messy.
SASshared = 'C:\Users\Carolina\Desktop\SAS_Shared\files';
SS_path = genpath(SASshared);
addpath(SS_path);

data_dir_ts = dir([SASshared '\CA_lower_leg_filter*.txt']);
[amount dummy] = size(data_dir_ts);
[tsraw_t, tsraw_f] = samples_analysis(data_dir_ts(2:end),'C',0,'Recorded raw data');
[tsc_t, tsc_f] = samples_analysis(data_dir_ts(2:end),'C',1,'SAS filtered data');

files_dir = [SASshared '\CA_lower_leg'];
for k=1:amount
    name = ['Recording 09th Dec nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end

%% Figures
fig_path = genpath('C:\Users\Carolina\Desktop\Internship\Software\Matlab_figs_and_data');
addpath(fig_path);

files_fig = dir(['C:\Users\Carolina\Desktop\Internship\Software\Matlab_figs_and_data' '\2020_12_09_*']);

for k=1:length(files_fig)
    open(files_fig(k).name);
end

%% Timing perfomance:
% -time 1 = from when threshold has been passed until the stimulator starts (FES_cnt)
% -time 2 = filtering perfomance (EMG_tic)
% -time 3 = thread 1 process perfomance: state machine + recorder + stimulator time
% -time 4 = thread 2 interface perfomance: keyboard + UDP + TCP time

files1=dir([SASshared '\CA_lower_leg_time1_*.txt']);
[amount, dummy ] = size(files1);

for k=1:amount
    ['Time perfomance on 9th Dec recording nr' num2str(k)]
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name])
end


files1=dir([C_files_backup '/test_session_09Nov/CUL_time1_*']);
[amount, dummy ] = size(files1);

for k=1:amount
    ['Time perfomance on 9th Nov recording nr' num2str(k)]
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name])
end

%% Other figures for the document
srate = 1000;
testFilt_add = 'C:\Users\Carolina\Desktop\Internship\Software\ProjectCPP\TestFilters\output\';
test_dir = dir([testFilt_add 'out_CUL_leg_*']);

% WITH stimulator
[data1_t data_f] = samples_analysis(test_dir(1),'C',0,'Testing C filters offline');
[data2_t data_f] = samples_analysis(test_dir(1),'C',1,'Testing C filters offline');
t_12 = [0:length(data1_t)-1]*(1/srate);

% plotting all toguether:
figure
subplot(2,2,1)
hold on
title('Raw EMG in time domain')
plot(t_12,data1_t);
xlim([0 t_12(end)]);
subplot(2,2,3)
hold on
title('Filtered EMG in time domain')
plot(t_12,data2_t);
xlim([0 t_12(end)]);

subplot(2,2,2)
grid on
plot_f = fftEMG(data1_t,['Raw EMG in frequency domain'],srate);
xlim([0 300])
subplot(2,2,4)
grid on
plot_f = fftEMG(data2_t,['Filtered EMG in frequency domain'],srate);
xlim([0 300])    


% WITHOUT stimulator
[data1_t data_f] = samples_analysis(test_dir(2),'C',0,'Testing C filters offline');
[data2_t data_f] = samples_analysis(test_dir(2),'C',1,'Testing C filters offline');
t_12 = [0:length(data1_t)-1]*(1/srate);

% plotting all toguether:
figure
subplot(2,2,1)
hold on
title('Raw EMG in time domain')
plot(t_12,data1_t);
xlim([0 t_12(end)]);
subplot(2,2,3)
hold on
title('Filtered EMG in time domain')
plot(t_12,data2_t);
xlim([0 t_12(end)]);

subplot(2,2,2)
grid on
plot_f = fftEMG(data1_t,['Raw EMG in frequency domain'],srate);
xlim([0 300])
ylim([0 0.01])
subplot(2,2,4)
grid on
plot_f = fftEMG(data2_t,['Filtered EMG in frequency domain'],srate);
xlim([0 300])  
ylim([0 0.01])
