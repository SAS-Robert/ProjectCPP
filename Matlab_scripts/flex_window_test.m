%% Testing the flexible window:
path = pwd;
% Add source directories
data_path = genpath('SAS/files');
fcn_path = genpath('Matlab_scripts');
addpath(data_path);
addpath(fcn_path);

C_files_backup = 'C:\Users\Carolina\Desktop\Internship\Software\C_files_backup';
backup_path = genpath(C_files_backup);
addpath(backup_path);

%% Loading files 

mean_dir=dir(['SAS/test_17Dec/' 'flex_window_mean_*']);
[amount dummy] = size(mean_dir);

mean_full_name = [mean_dir(end).folder '\' mean_dir(end).name];
data = load(mean_full_name)';
% data written on the file:
newW = data(1,:);
newW_n = data(2,:);
oldW1 = data(3,:);
oldW1_n = data(4,:);
oldW2 = data(5,:);
oldW2_n = data(6,:);

result = data(7,:);
flex_num = data(8,:);
flex_den = data(9,:);


%% How the flexible window works
% So, if the flexible window is working properly the values must be swipped
% in every iteration:
% newW(1) = oldW1(2) = oldW2(3), etc
newW_s0 = newW(1:end-2);    newW_n_s0 = newW_n(1:end-2);
newW_s1 = oldW1(2:end-1);   newW_n_s1 = oldW1_n(2:end-1);
newW_s2 = oldW2(3:end);     newW_n_s2 = oldW2_n(3:end);

temp_newW_s01 = newW_s0 - newW_s1;
temp_newW_s12 = newW_s1 - newW_s2;

if(temp_newW_s01 == 0 )
    fprintf('Moving new_window -> old_window_1 is correct. \n');
else
    fprintf('Moving new_window -> old_window_1 is wrong. \n');
end

if(temp_newW_s12 == 0 )
    fprintf('Moving old_window_1 -> old_window_2 is correct. \n');
else
    fprintf('Moving old_window_1 -> old_window_2 is wrong. \n');
end

%% How the results are calculated
% flex_num = newW*newW_n + oldW1*oldW1_n + oldW2*oldW2_n
% felx_den = newW_n + oldW1_n + oldW2_n
% result = flex_num/flex_den

error_num = 0; error_num_pos = []; error_num_value = [];
error_den = 0;

for i=3:length(newW)
    temp_num = newW(i)*newW_n(i) + oldW1(i)*oldW1_n(i) + oldW2(i)*oldW2_n(i);
    temp_den = newW_n(i) + oldW1_n(i) + oldW2_n(i);
    
    error_num = error_num + temp_num - flex_num(i);
    if(error_num ~= 0)
        error_num_pos = [error_num_pos i];
        error_num_value = [error_num_value error_num];
    end
    
    error_den = error_den + temp_den - flex_den(i);
end

if(error_num == 0)
    fprintf('Numerator calculation is correct. \n');
else
    fprintf('Numerator calculation is wrong. \n');
end

if(error_den == 0)
    fprintf('Denominator calculation is correct. \n');
else
    fprintf('Denominator calculation is wrong. \n');
end

