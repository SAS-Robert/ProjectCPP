%% =============== Example standard code ===============
%% Add source directories
% Functions and scripts
fcn_path = genpath('Matlab_scripts');
addpath(fcn_path);

% Figures directory
% fig_path = genpath('C:\Users\Carolina\Desktop\Internship\Software\Matlab_figs_and_data');
% addpath(fig_path);

% Data directory
dataDirectory = 'C:\Users\kkl\OneDrive - Aalborg Universitet\LSR Test - 14-04-2021\';
SS_path = genpath(dataDirectory);
addpath(SS_path);

%% Variables
name = 'subject';    % files name from SAS.cpp on string format
data_dir = dir([dataDirectory name '_filter*.txt']);
[amount, ~] = size(data_dir);
% amount = how many recordings are on the folder


%% Plots
% plot all the EMGs in time and frequency domain
%[raw_t, raw_f] = samples_analysis(data_dir,'C',0,'Recorded raw data');
%[c_t, c_f] = samples_analysis(data_dir,'C',1,'SAS filtered data');

% plot all the EMGs individually with raw-vs-filtered on time domain,
% frequency domain, start-end of repetitions, triggers, etc
files_dir = [dataDirectory  name];
for k=1:amount
    try
        plotName = ['Recording XXth January, nr.' num2str(k)];
        data = plot_th_mvc(files_dir,plotName,k);
    catch ME
        disp(ME.message)
    end
end

% plotting only the filtered-rectified EMGs with the EMG averages and threshold:
% select_nr = [1 3 4]; % e.g.: plot only recordings 1, 3 and 4
% plot_titles = {'Retified-Filtered data and mean values - Recording 1';
%                'Retified-Filtered data and mean values - Recording 3';
%                'Retified-Filtered data and mean values - Recording 4';
%                'Test Session XXth January different electrodes placement'};
% % the last component of plot_titles is the name of the Figure
% data = only_th(files_dir, plot_titles, select_nr);