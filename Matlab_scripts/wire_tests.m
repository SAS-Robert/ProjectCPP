%% =============== Wire tests ===============
%% Add source directories
% Functions and scripts
fcn_path = genpath('Matlab_scripts');
addpath(fcn_path);

% Data directory
wirefolder = 'C:\Users\Carolina\LSR Dropbox\LSR Quality Management System\SAS\ROBERT - SAS Shared\6. Measurements\wire test'; 
wirepath = genpath(wirefolder);
addpath(wirepath);


%% Set up internal var
% Getting folders' names
wiredir = dir(wirefolder);
% Extract only those that are directories
[folderNr aa] = size(wiredir);
% the first 2 are empty
empty = 2;
subFolders = cell(folderNr-empty,1);  
subFolder_name = cell(folderNr-empty,1);  
for k=(empty+1):folderNr
    subFolder_name{k-empty,1} = [wiredir(k).name];
    subFolders{k-empty,1} = [wirefolder '\' wiredir(k).name];
end
% Note: this is on cell format, to convert to char: 
% subF_char = char(subFolders(1))

%% Figures
figures_wires = 'C:\Users\Carolina\LSR Dropbox\LSR Quality Management System\SAS\ROBERT - SAS Shared\6. Measurements\wire test\test_figures';
figpath = genpath(figures_wires);
addpath(figpath);
fig_dir = dir(figures_wires);

for k=(empty+1):folderNr
    open(fig_dir(k).name);
end

%% Plot variables
name = 'subject';
amount = zeros(1, folderNr-empty);

% amount(k) represents how many recording are in each (k) folder
for k=1:folderNr-empty
    data_dir = dir([char(subFolders(k)) '\' name '_filter*.txt']);
    [amount(k) dummy] = size(data_dir); 
end

%% Plots
% Plot individually with all the logs and events

for j=1:folderNr-empty
    % Select subfolder
    files_dir = [char(subFolders(j)) '\' name ];
    
    % Plot all the recordings within that folder
    for k=1:amount(j)
        title_plot = ['Setup: ' char(subFolder_name(j))];
        data = plot_th3(files_dir,title_plot,k,'C');    
    end
end