function [value_av,value_sd,val_samples] = timing_eval(name, pos, goal)
% It evaluates the average, standard deviation of the different time
% measurements of a single recording.
% - name: srting with full name directory + name + '_time1_' + whatever time.
% - pos: select time to give results from (1 to 4).
% - goal: ideal time that the results aim to achieve. If any of the results
% differ too much from the goal, it will be printed with a red/yellow
% color.
%
%e.g.:
% location_name =
% 'C:\Users\Carolina\Desktop\Internship\Software\C_files_backup\test_09Dec\';
% file_name = 'CA_upper_leg_time1_20201209_1700.txt';
% full_name = [location_name file_name];
% timing = [1:4];           % results from all measurements
% goal = [10 10 33 150];    % [ideal_t1 ideal_t2 ideal_t3 ideal_t4];
% [value_av,value_sd,val_samples] = timing_eval(full_name, timing, goal);
%
% NOTE: The results are printed on the command screen in a LaTeX format.

st_th = 1; st_wait = 2; st_running = 3; st_stop = 4; % different states 

% start outputs:
temp_av = [];
temp_sd = [];
temp_samples = [];
    
for k=1:4
    % select type of time measurement
    switch k
        case 1
            file_name = name;
        case 2 
            file_name = strrep(name,'_time1_','_time2_');
        case 3
            file_name = strrep(name,'_time1_','_time3_');
        case 4
            file_name = strrep(name,'_time1_','_time4_');
    end    
    % load data
    t_data = load(file_name)';
    [data_row data_col] = size(t_data);
    t_t = [];

    if (data_row==1)
        t_t = t_data(1,:);
    else
        for i=1:length(t_data)
            if (t_data(2,i)>=st_wait)&&(t_data(2,i)<=st_stop)
            t_t = [t_t t_data(1,i)];
            end
        end
    end
    % calculate values 
    t_m = mean(t_t);
    t_sd = std(t_t);
    
    % accumulate outputs:
    temp_av = [temp_av t_m];
    temp_sd = [temp_sd t_sd];
    temp_samples = [temp_samples length(t_t)];
end
% Conversion to ms
value_av = temp_av*1000;
value_av(1) = value_av(1)/10;
value_sd = temp_sd*1000;       
val_samples = temp_samples; 
% Printing into a format to show on the screen
%fprintf('average = [');
% fprintf(' %i,',value_av);
% %fprintf(']\n');
% 
% %fprintf('max = [');
% fprintf(' %i,',value_max);
% %fprintf(']\n');
% 
% %fprintf('samples = [');
% fprintf(' %i,',temp_samples);
%fprintf(']\n');

% Printing into a format for a .tex table file
for k=1:length(pos)    
    fprintf('%i & ',temp_samples(pos(k)));
    if (abs(value_av(pos(k)))> (abs(goal(k)) + 0.1*abs(goal(k))) )
        fprintf('\\cellcolor{red!25}');
    else
        fprintf('\\cellcolor{green!10}');
    end
    fprintf('%f & ',value_av(pos(k)));
    
    if (abs(value_sd(pos(k)))> (0.3*abs(goal(k))) )
        fprintf('\\cellcolor{yellow!25}');
    end
    if (k==pos(end))||(pos(k)==4)
        fprintf('%f \\\\',value_sd(pos(k)));
    else
        fprintf('%f & ',value_sd(pos(k)));
    end
    fprintf('\n');
end
    %fprintf('----------\n');

end

