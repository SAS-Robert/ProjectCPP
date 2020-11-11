% Time analysis perfomance 
% -time 1 = from when threshold has been passed until the stimulator starts (FES_cnt)
% -time 2 = filtering perfomance (EMG_tic)
% -time 3 = thread 1 process perfomance: state machine + recorder + stimulator time
% -time 4 = thread 2 interface perfomance: keyboard + UDP + TCP time
%% Load data files
path = pwd;
% Add source directories
data_path = genpath('SAS/files');
fcn_path = genpath('Matlab_scripts');
addpath(data_path);
addpath(fcn_path);
% Find files 
files1=dir('SAS/files/time1_*.txt');
files2=dir('SAS/files/test_time2_*.txt');
files3=dir('SAS/files/test_time3_*.txt');
files4=dir('SAS/files/test_time4_*.txt');

%time 1 data 
[amount, dummy ] = size(files1);
files1_full_name = [files1(amount).folder '\' files1(amount).name]; %Just take the last one
t1_t= load(files1_full_name);
t1_m = mean(t1_t);
t1_max = max(t1_t);

%time 2 data 
[amount, dummy ] = size(files2);
files2_full_name = [files2(amount).folder '\' files2(amount).name]; %Just take the last one
t2_t= load(files2_full_name)';
t2_m = mean(t2_t);
t2_max = max(t2_t);

%time 3 data 
[amount, dummy ] = size(files3);
files3_full_name = [files3(end).folder '\' files3(end).name]; %Just take the last one
t3_data= load(files3_full_name)';
t3_t = t3_data(1,:);
t3_s = t3_data(2,:);
t3_s0=[]; t3_s1=[]; t3_s2=[]; t3_s3=[]; t3_s4=[]; t3_s5=[];
%classify time samples
for i=1:length(t3_t)
     switch t3_s(i)
        case 0
            t3_s0 = [t3_s0 t3_t(i)];
        case 1
            t3_s1 = [t3_s1 t3_t(i)];
        case 2
            t3_s2 = [t3_s2 t3_t(i)];
        case 3
            t3_s3 = [t3_s3 t3_t(i)];
        case 4
            t3_s4 = [t3_s4 t3_t(i)];
        case 5
            t3_s5 = [t3_s5 t3_t(i)];
    end
end

t3_s2_m = mean(t3_s2);
t3_s2_max = max(t3_s2);

boxplot(t3_t,t3_s)
xlabel('State process')
ylabel('Time [s]') 
ylim([0 1])


%time 4 data 
files4_full_name = [files4(end).folder '\' files4(end).name]; %Just take the last one
t4_t = load(files4_full_name)';
t4_m = mean(t4_t);
t4_max = max(t4_t);

% total time:
t_total_mean = t3_s2_m + t4_m
t_total_max = t3_s2_max + t4_max
