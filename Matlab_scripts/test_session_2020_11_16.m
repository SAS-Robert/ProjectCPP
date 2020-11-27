%% Session 4, 13th Nov 2020
% Files dir
data_path = genpath('SAS/session_16Nov');
addpath(data_path);
% CA data
data_dir_session5 = dir(['SAS/session_16Nov/CA_leg_filter*.txt']);
[amount dummy] = size(data_dir_session5);
files_dir = ['SAS/session_16Nov/CA_leg'];

%% Plotting
% per types
lower_leg1 = [1:2]; lower_leg2 = [3:4]; lower_leg3 = [6:7];

[s5raw_t, s5raw_f] = samples_analysis(data_dir_session5(lower_leg1),'C',0,'Lower leg - Raw data usual config');
[s5c_t, s5c_f] = samples_analysis(data_dir_session5(lower_leg1),'C',1,'Lower leg - Filtered data usual config');

[s5raw_t, s5raw_f] = samples_analysis(data_dir_session5(lower_leg2),'C',0,'Lower leg - Raw data ground on knee');
[s5c_t, s5c_f] = samples_analysis(data_dir_session5(lower_leg2),'C',1,'Lower leg - Filtered ground on knee');

[s5raw_t, s5raw_f] = samples_analysis(data_dir_session5(lower_leg3),'C',0,'Lower leg - Raw data recording in new placement');
[s5c_t, s5c_f] = samples_analysis(data_dir_session5(lower_leg3),'C',1,'Lower leg - Filtered data recording in new placement');

upper_leg1 = [8:9]; upper_leg2 = [10:11];

[s5raw_t, s5raw_f] = samples_analysis(data_dir_session5(upper_leg1),'C',0,'Upper leg - Raw data ground on knee');
[s5c_t, s5c_f] = samples_analysis(data_dir_session5(upper_leg1),'C',1,'Upper leg - Filtered data ground on knee');

[s5raw_t, s5raw_f] = samples_analysis(data_dir_session5(upper_leg2),'C',0,'Upper leg - Raw data with electrodes on side');
[s5c_t, s5c_f] = samples_analysis(data_dir_session5(upper_leg2),'C',1,'Upper leg - Filtered data with electrodes on side');



% for comparison
set1 = [1 3 6];
set2 = [2 4 7];
set_ll_name = {'Electrodes located in middle of FES';
               'Ground on the knee';
               'Electrodes at the end of muscle + ground on the knee';
               'Session 16th Nov, lower leg'};
data = only_th(files_dir,set_ll_name,set1);
data = only_th(files_dir,set_ll_name,set2);

set_upper_leg = [8:11];
set_ul_name = {'Recording 1 - Electrodes close to the knee';
               'Recording 2 - Electrodes close to the knee';
               'Recording 1 - Electrodes on the side';
               'Recording 2 - Electrodes on the side';
               'Session 16th Nov, upper leg'};
data = only_th(files_dir,set_ul_name,set_upper_leg);


% individually 
for k=1:length(set_upper_leg)
    name = ['Session 16th Nov, CA recording nr.' num2str(set_upper_leg(k))];
    data = plot_th3(files_dir,name,set_upper_leg(k),'C');    
end

%% Applying changes
data_dir_session1 = dir([C_files_backup '/test/after lunch/CUL_leg_filter*.txt']);
[amount dummy] = size(data_dir_session1);
test_filts(data_dir_session1(2),'Chebyshev II as 100Hz-Notch',80,10);

testFilt_add = 'C:\Users\Carolina\Desktop\Internship\Software\ProjectCPP\TestFilters\output\';
test_dir = dir([testFilt_add 'out_CUL_leg_*']);
[data_t data_f] = samples_analysis(test_dir(1),'C',1,'Testing C filters offline');
data_dt = diff(data_t);

t = [0:length(data_t)-1]*0.001;

subplot(2,1,1)
plot(t,data_t)
subplot(2,1,2)
plot(t(2:end),data_dt)

%% Session 23-11-2020
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

%% Demo 26-11-2020
% Files dir
data_path = genpath('SAS/demo_26Nov');
addpath(data_path);
% CA data
data_dir_session6 = dir(['SAS/demo_26Nov/CA_lower_leg1_filter*.txt']);
[amount dummy] = size(data_dir_session6);
files_dir = ['SAS/demo_26Nov/CA_lower_leg1'];

% individually 
for k=1:amount
    name = ['Demo 26th Nov, CA lower leg(1) recording nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end

% V2 data
data_dir_session6 = dir(['SAS/demo_26Nov/V2_upper_leg_filter*.txt']);
[amount dummy] = size(data_dir_session6);
files_dir = ['SAS/demo_26Nov/V2_upper_leg'];

% individually 
for k=1:amount
    name = ['Demo 26th Nov, Volunteer 2 upper leg recording nr.' num2str(k)];
    data = plot_th3(files_dir,name,k,'C');    
end