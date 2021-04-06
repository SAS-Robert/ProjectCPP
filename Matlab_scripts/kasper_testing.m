%% =============== Example standard code ===============
%% Add source directories
% Functions and scripts
fcn_path = genpath('Matlab_scripts');
addpath(fcn_path);

% Figures directory
% fig_path = genpath('C:\Users\Carolina\Desktop\Internship\Software\Matlab_figs_and_data');
% addpath(fig_path);

% Data directory
folder = 'files\';    % folder name from SAS.cpp on string format
SASshared = 'D:\OneDrive - Aalborg Universitet\GitHub\ProjectCPP\SASApp\';
SASshared_full = [SASshared folder];
SS_path = genpath(SASshared_full);
addpath(SS_path);

%% Variables
name = 'subject';    % files name from SAS.cpp on string format
data_dir = dir([SASshared folder name '_filter*.txt']);
[amount dummy] = size(data_dir);     
% amount = how many recordings are on the folder


%% Plots
% plot all the EMGs in time and frequency domain
%[raw_t, raw_f] = samples_analysis(data_dir,'C',0,'Recorded raw data');
%[c_t, c_f] = samples_analysis(data_dir,'C',1,'SAS filtered data');

% plot all the EMGs individually with raw-vs-filtered on time domain, 
% frequency domain, start-end of repetitions, triggers, etc
files_dir = [SASshared_full  name];
for k=amount
    name = ['Recording XXth January, nr.' num2str(k)];
    data = plot_th_mvc(files_dir,name,k,'C');    
end

% plotting only the filtered-rectified EMGs with the EMG averages and threshold:
select_nr = [1 3 4]; % e.g.: plot only recordings 1, 3 and 4
plot_titles = {'Retified-Filtered data and mean values - Recording 1';
               'Retified-Filtered data and mean values - Recording 3';
               'Retified-Filtered data and mean values - Recording 4';
               'Test Session XXth January different electrodes placement'};
% the last component of plot_titles is the name of the Figure
data = only_th(files_dir, plot_titles, select_nr);