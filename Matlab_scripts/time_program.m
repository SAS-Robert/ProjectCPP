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

%% ======================== Evaluating time per session ================
%% Session 9th Nov
files1=dir([C_files_backup '/test_session_09Nov/CUL_time1_*']);
[amount, dummy ] = size(files1);

for k=1:amount
    fprintf(['Time perfomance on 9th Nov recording nr' num2str(k) '\n']);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name], [1 2]);
    fprintf('\n');
end

%% Session 10th Nov
files1=dir([C_files_backup '/test_session_10Nov/CUL_time1_*']);
[amount, dummy ] = size(files1);

for k=1:amount
    fprintf(['Time perfomance on 10th Nov recording nr' num2str(k) '\n']);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name]);
    fprintf('\n');
end

%% Session 13th Nov
% CUL
files1=dir([C_files_backup '/session_13Nov/CUL_time1_*']);
[amount, dummy ] = size(files1);
for k=1:amount
    %fprintf(['CUL Time perfomance on 13th Nov recording nr' num2str(k) '\n']);
    fprintf('%%Recording %i\n', k);
    fprintf('%i & ', k);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name], [1 2]);
    fprintf('\\hline\n');
end
% CA
files1=dir([C_files_backup '/session_13Nov/CA_time1_*']);
[amount, dummy ] = size(files1);
for k=1:amount
    fprintf(['CA Time perfomance on 13th Nov recording nr' num2str(k) '\n']);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name]);
    fprintf('\n');
end
% JW
files1=dir([C_files_backup '/session_13Nov/JW_time1_*']);
[amount, dummy ] = size(files1);
for k=1:amount
    fprintf(['JW Time perfomance on 13th Nov recording nr' num2str(k) '\n']);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name]);
    fprintf('\n');
end

%% Session 16th Nov
% CA
files1=dir([C_files_backup '/session_16Nov/CA_leg_time1_*']);
[amount, dummy ] = size(files1);
% times 1 and 2 (triggering and processing time)
for k=1:amount
    fprintf('%%Recording %i\n', k);
    fprintf('%i & ', k);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name], [1 2], [10 10]);
    fprintf('\\hline\n');
end
% times 3 and 4 (thread 1 and thread 2)
for k=1:amount
    fprintf('%%Recording %i\n', k);
    fprintf('%i & ', k);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name], [3 4], [100 150]);
    fprintf('\\hline\n');
end
%% Session 18th Nov
% CA (well, technically unknown)
files1=dir([C_files_backup '/demo_18Nov/subject1_time1_*']);
[amount, dummy ] = size(files1);
for k=1:amount
    fprintf(['Time perfomance on 18th Nov recording nr' num2str(k) '\n']);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name]);
    fprintf('\n');
end

%% Session 23 Nov
% CUL
files1=dir([C_files_backup '/session_23Nov/CUL_lower_leg_time1_*']);
[amount, dummy ] = size(files1);
for k=1:amount
    fprintf(['CUL Time perfomance on 23rd Nov recording nr' num2str(k) '\n']);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name]);
    fprintf('\n');
end

%% Session 26th Nov
% CA 
files1=dir([C_files_backup '/demo_26Nov/CA_lower_leg1_time1_*']);
[amount, dummy ] = size(files1);
for k=1:amount
    fprintf(['Lower leg -> Time perfomance on 26th Nov recording nr' num2str(k) '\n']);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name]);
    fprintf('\n');
end

files1=dir([C_files_backup '/demo_26Nov/CA_upper_leg_time1_*']);
[amount, dummy ] = size(files1);
for k=1:amount
    fprintf(['Upper leg -> Time perfomance on 26th Nov recording nr' num2str(k) '\n']);
    [value_av,value_max,val_samples] = timing_eval([files1(k).folder '\' files1(k).name]);
    fprintf('\n');
end

%% For the report
% Session 3 10th Nov

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
