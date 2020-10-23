% Time analysis perfomance 
% -time 1 = from when threshold has been passed until the stimulator starts (FES_cnt)
% -time 2 = filtering perfomance (EMG_tic)
% -time 3 = process perfomance: state machine + recorder + stimulator time
%% Load data files
path = pwd;
% Add source directories
data_path = genpath('SAS');
fcn_path = genpath('Matlab_scripts');
addpath(data_path);
addpath(fcn_path);
% Find files 
files1=dir('SAS/files/time1_*.txt');
files2=dir('SAS/files/time2_*.txt');
files3=dir('SAS/files/time3_*.txt');

%time 1 data 
[amount, dummy ] = size(files1);
files1_full_name = [files1(amount).folder '\' files1(amount).name]; %Just take the last one
t1_data= load(files1_full_name);

%time 2 data 
[amount, dummy ] = size(files2);
files2_full_name = [files2(amount).folder '\' files2(amount).name]; %Just take the last one
t2_data= load(files2_full_name)';
t2_t = t2_data(1,:);
t2_s = t2_data(2,:);
t2_s0=[]; t2_s1=[]; t2_s2=[]; t2_s3=[]; t2_s4=[]; t2_s5=[];
% classify time samples
for i=1:length(t2_t)
    switch t2_s(i)
        case 0
            t2_s0 = [t2_s0, t2_t(i)];
        case 1
            t2_s1 = [t2_s1, t2_t(i)];
        case 2
            t2_s2 = [t2_s2, t2_t(i)];
        case 3
            t2_s3 = [t2_s3, t2_t(i)];
        case 4
            t2_s4 = [t2_s4, t2_t(i)];
        case 5
            t2_s5 = [t2_s5, t2_t(i)];
    end
end

%time 3 data 
[amount, dummy ] = size(files3);
files3_full_name = [files3(amount).folder '\' files3(amount).name]; %Just take the last one
t3_data= load(files3_full_name)';
t3_t = t3_data(1,:);
t3_s = t3_data(2,:);
t3_s0=[]; t3_s1=[]; t3_s2=[]; t3_s3=[]; t3_s4=[]; t3_s5=[];
% classify time samples
% for i=1:length(t3_t)
%     switch t3_s(i)
%         case 0
%             t3_s0 = [t3_s0, t3_t(i)];
%         case 1
%             t3_s1 = [t3_s1, t3_t(i)];
%         case 2
%             t3_s2 = [t3_s2, t3_t(i)];
%         case 3
%             t3_s3 = [t3_s3, t3_t(i)];
%         case 4
%             t3_s4 = [t3_s4, t3_t(i)];
%         case 5
%             t3_s5 = [t3_s5, t3_t(i)];
%     end
% end
boxplot(t3_t,t3_s)
xlabel('State process')
ylabel('Time [s]') 
ylim([0 1])
