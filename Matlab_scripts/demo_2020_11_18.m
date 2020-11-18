%% Add source directories
data_address = 'SAS/demo_18Nov';
data_path = genpath(data_address);
fcn_path = genpath('Matlab_scripts');
addpath(data_path);
addpath(fcn_path);

%% Subject 1 parameters 
subject1_name = [data_address '/subject1'];
subject1_dir=dir([subject1_name '_filter*']);
[amount dummy] = size(subject1_dir);

%% Only raw and filtered EMG data
[emg_raw_t1, emg_raw_f] = samples_analysis(subject1_dir,'C',0,'Raw data EMG demo 1');
[emg_f_t1, emg_f_f] = samples_analysis(subject1_dir,'C',1,'SAS filtered EMG demo 1');

%% Plotting EMGs

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