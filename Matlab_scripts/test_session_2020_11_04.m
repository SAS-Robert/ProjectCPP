%% Test session on CUL's leg
path = pwd;
% Add source directories
%data_path = genpath('SAS/files');
data_path = genpath('SAS/test/after lunch');
fcn_path = genpath('Matlab_scripts');
addpath(data_path);
addpath(fcn_path);
%%
raw_dev_dir = dir('SAS/test/after lunch/CUL_leg_raw*.txt');
data_dir = dir('SAS/test/after lunch/CUL_leg_filter*.txt');
th_dir = dir('SAS/test/after lunch/CUL_leg_th*.txt');

[raw_t, raw_f] = samples_analysis(data_dir,'C',0,'Recorded raw data');
[c_t, c_f] = samples_analysis(data_dir(end),'C',1,'SAS filtered data');
[m_t, m_f] = samples_analysis(raw_dev_dir,'H',1,'Matlab filtered data');


files_dir = 'SAS/test/after lunch/CUL_leg';
[amount dummy] = size(dir([files_dir '_th_*']));

%for k=1:amount
    k = 3
    name = ['Christians leg recording after lunch, nr.' num2str(k)];
    data = plot_th(files_dir,name,k);    
    figure
    spectrogram(data,'yaxis')
%end


files_dir = 'SAS/test/good stuff/CUL_leg';
[amount dummy] = size(dir([files_dir '_th_*']));

for k=1:amount
    name = ['Christians leg 2nd set of recording-nr.' num2str(k)];

    plot_th(files_dir,name,k);    
end

files_dir = 'SAS/test/more good stuff/CUL_leg';
[amount dummy] = size(dir([files_dir '_th_*']));

for k=1:amount
    name = ['Christians 2nd set of recording-nr.' num2str(k)];

    plot_th(files_dir,name,k);    
end


temp=c_t(3000:end);

max_c_t = max(c_t);
max_temp = max(temp);
max_c_t_t = 0;
max_temp_t = 0;

t_emg = zeros(1, length(c_t));

for i=1:length(t_emg)
    t_emg(i) = i/1000;
    if c_t(i)==max_c_t
        max_c_t_t = t_emg(i)
    end
end

t_temp = zeros(1, length(temp));
for i=1:length(t_temp)
    t_temp(i) = i/1000;
    if temp(i)==max_temp
        max_temp_t = t_temp(i)
    end    
end 

plot(t_emg, c_t);
hold on
plot(t_temp, temp);
plot(max_c_t_t, max_c_t, 'bo', max_temp_t, max_temp, 'ro')

%% Working on fir filters

files_dir = dir('SAS/test/after lunch/CUL_leg_filter*.txt');
analysis(files_dir,'0','Raw data');
analysis(files_dir,'C','SAS filtered data');
analysis(files_dir,'M','Filtered data with Matlab');

th_name = strrep(files_dir(1).name,'filter','th');
th_dir = dir(th_dir);
test_filts(files_dir(1:2),'M','Filtered data with Matlab');
