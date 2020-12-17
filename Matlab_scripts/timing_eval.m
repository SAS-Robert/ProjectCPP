function [value_av,value_max,val_samples] = timing_eval(name)

%
st_th = 1; st_wait = 2; st_running = 3; st_stop = 4; % different states 

% start outputs:
temp_av = [];
temp_max = [];
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
    t_max = max(t_t);
    
    % accumulate outputs:
    temp_av = [temp_av t_m];
    temp_max = [temp_max t_max];
    temp_samples = [temp_samples length(t_t)];
end
%full_name = strrep(full_name,'_time1_','_time2_'); 
value_av = temp_av;
value_max = temp_max;
val_samples = temp_samples;

fprintf('average = [');
fprintf(' %i,',value_av);
fprintf(']\n');

fprintf('max = [');
fprintf(' %i,',value_max);
fprintf(']\n');

fprintf('samples = [');
fprintf(' %i,',temp_samples);
fprintf(']\n');
end

