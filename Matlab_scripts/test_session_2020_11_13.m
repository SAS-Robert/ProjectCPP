%% Session 4, 13th Nov 2020
% Files dir
data_path = genpath('SAS/session_13Nov');
addpath(data_path);

%% Plotting
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


%% Testing new filters
% This filtering is not good enough. Adding more filters
% Using as example Christians arm recording

data_dir_session1 = dir([C_files_backup '/test/after lunch/CUL_leg_filter*.txt']);
[amount dummy] = size(data_dir_session1);
test_filts(data_dir_session1(3),'M',['Filtered data with Butterworth  order = 2, bandwidth = 4.5'],2,4.5);

%% New electrodes placement and filters
% CA data 
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



%% Testing electrodes on Monday 16th
% Files dir
data_path = genpath('SAS/files');
addpath(data_path);
address_test = 'C:\Users\Carolina\Desktop\Internship\Software\ProjectCPP\SAS\files\';

data_dir = dir([address_test 'Test_usual_filter*.txt']);
[amount dummy] = size(data_dir);

files_dir = [address_test 'Test_usual'];
for k=1:amount
    name = ['Session 16th Nov, CA arm recording nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end
[s4raw_t, s4raw_f] = samples_analysis(data_dir_session4,'C',0,'Recorded raw data');
[s4c_t, s4c_f] = samples_analysis(data_dir_session4,'C',1,'SAS filtered data');

files_dir = [address_test 'Test_elbow'];
for k=1:amount
    name = ['Session 16th Nov, CA elbow recording nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end

files_dir = [address_test 'Test_elbow_new'];
for k=1:amount
    name = ['Session 16th Nov, CA elbow new recorder nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end

files_dir = [address_test 'Test_usual_new'];
for k=1:amount
    name = ['Session 16th Nov, CA elbow new recorder nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end

files_dir = [address_test 'Test_sp_new'];
k = 1;
    name = ['Session 16th Nov, CA elbow new recorder nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    