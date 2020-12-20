function data = only_th(files_dir,name,pos)
%- files_dir = location and name of the files.
%- name = tile of each individual plot and figure title.
%- pos = array with the concrete elements to be plot from the directory.
% Example:
% files_dir = [C_files_backup '/test_session_10Nov/CUL'];
% pos = [1 3];  % we specifically wanna see only these 2 recordings
% name = {'Recording nr.1';
%         'Recording nr.3';
%         'Some recordings from session 10th Nov'};
% data = only_th(files_dir,name,pos);

% getting array size
[dummy amount] = size(pos);

% things to do once
figure('Name',char(name(end)))

for i_loop=1:amount
    % getting filtered EMG 
    data_dir=dir([files_dir '_filter_*']);
    full_name = [data_dir(pos(i_loop)).folder '\' data_dir(pos(i_loop)).name]; %Just take the last one
    data = (load(full_name))';
    c_raw = data(1,:);
    c_t = data(end,:);
    srate = 1000;

    t_emg = zeros(1,length(c_t));
    for i=1:length(t_emg)
        t_emg(i) = i/1000;
    end

    % accessing th directory and variables
    full_name = strrep(full_name,'_filter_','_th_'); 
    data = (load(full_name))';
    mean_v = data(1,:);
    sd_v = data(2,:);
    tinit = data(3,:);
    tend = data(4,:);
    toff = data(5,:);

    % finding position where the threshold is set:
    th_pos_discard = 0; th_pos_start = 0;
    for i=2:length(mean_v)
        if ((sd_v(i)==0)&&(sd_v(i-1)~=0))
            th_pos_start = i-1;
        end
        if ((sd_v(i)~=0)&&(sd_v(i-1)==0))
            th_pos_discard = i;
        end
    end
    % resting EMG:
    r_th = [];
    r_th_val = [];
    for i=th_pos_discard:th_pos_start
        r_th = [r_th mean_v(i)*ones(1,tend(i)-tend(i-1))];
        r_th_val = [r_th_val (toff(i)*ones(1,tend(i)-tend(i-1)))/(tend(i)-tend(i-1))];
    end
    r_t_th = zeros(1, length(r_th));
    for i=1:length(r_th)
        r_t_th(i) = (tend(th_pos_discard-1)+i)/1000;
    end
    % threshold values
    y_th = mean(r_th) * ones(1,tend(th_pos_start)-tend(th_pos_discard+1));
    t_th = zeros(1,tend(th_pos_start)-tend(th_pos_discard+1));
    for i=1:length(t_th)
        t_th(i) = (tend(th_pos_discard+1)+i)/1000;
    end
    % mean values
    y_t =[];
    for i=(th_pos_start+1):length(mean_v)
        y_temp = mean_v(1,i) * ones(1,toff(1,i));
        y_t = [y_t y_temp];
    end
    t = zeros(1,length(y_t));
    for i=1:length(t)
        t(i) = t_th(end) + i/1000;
    end

    th_value = mean(r_th_val) * ones(1, length(y_t));


    % accessing log directory and variables
    full_name = strrep(full_name,'_th_','_log_');
    data = (load(full_name))';
    log_type = data(1,:);
    log_val = data(2,:);

    %
    subplot(amount,1,i_loop)
    hold on
    grid on 
    plot1 = plot(t_emg, abs(c_t'));
    plot2 = plot(r_t_th,r_th,'c');
    plot3 = plot(t,abs(y_t),'r');
    plot4 = plot(t, th_value,'y');
    plot_array = [plot1 plot2 plot3 plot4];
    plot_names = {'EMG','Resting mean', 'Activity mean', 'Threshold'};

    %title(['Retified-Filtered data and mean values EMG ' num2str(pos(i_loop))]);
    title(char(name(i_loop)));
    xlim([0 t(end)]);
    %ylim([0 max(c_t(3000:end))+0.2*max(c_t(3000:end))]);
    ylim([0 1.1*0.0001]);
    xlabel('t (s)');
    ylabel('v (V)');
    
    % plotting the logs 
    ma = 0; ka = 0; mc = 0; gx = 0; gt = 0; kc = 0; kt = 0;
    for k=1:length(log_type)
        log_plot = 'b.';
        temp_x_value = t(1)*1000;
        temp_t_value = log_val(k)/1000;
            
        switch log_type(k)
            case 1                  % magenta asterisk: stimulator triggered
                log_plot = 'm*';   
                plot_ma = plot(temp_t_value, y_t(log_val(k)-temp_x_value), log_plot);
                ma = 1;
            case 2
                log_plot = 'k*';    % black asterisk = end of repetition (without triggering stimulator)
                plot_ka = plot(temp_t_value, y_t(log_val(k)-temp_x_value), log_plot);
                ka = 1;
            case 3
                log_plot = 'm-';    % magenta circle = end of repetition (with stimulator)
                y_plot_temp = max(c_t(3000:end))+0.2*max(c_t(3000:end));
                plot_mc = plot(temp_t_value*ones(1,3), [0 y_t(log_val(k)-temp_x_value) y_plot_temp], log_plot);
                mc = 1;
            case 4
                log_plot = '-';    % gx green X = start repetition
                y_plot_temp = max(c_t(3000:end))+0.2*max(c_t(3000:end));
                plot_gx = plot(temp_t_value*ones(1,3), [0 y_t(log_val(k)-temp_x_value) y_plot_temp], log_plot, 'Color', [0.4660 0.6740 0.1880]);
                gx = 1;
            case 5
                log_plot = 'g^';    % Start training pressed
                plot_gt = plot(temp_t_value, y_t(log_val(k)-temp_x_value), log_plot);
                gt = 1;              
%             case 6
%                 log_plot = 'y*';    % Stimulator actually being stopped
%                 plot_kc = plot(temp_t_value, y_t(log_val(k)-temp_x_value), log_plot);
%                 kc = 1;
             case 7
                log_plot = 'k^';    % Stimulator stopped by user
                plot_kt = plot(temp_t_value, y_t(log_val(k)-temp_x_value), log_plot);
                kt = 1;               
        end
    
    end
    % legend and label stuff
    if(gt==1)
        plot_array = [plot_array plot_gt];
        plot_names{end+1} = 'Start training';
    end
    if(gx==1)
        plot_array = [plot_array plot_gx];
        plot_names{end+1} = 'Start rep';
    end    
    if(ma==1)
        plot_array = [plot_array plot_ma];
        plot_names{end+1} = 'Trigger(s)';
    end
    if(mc==1)
        plot_array = [plot_array plot_mc];
        plot_names{end+1} = 'End rep (with stim)';
    end    
    if(kc==1)
        plot_array = [plot_array plot_kc];
        plot_names{end+1} = 'Stop stim';
    end
    if(ka==1)
        plot_array = [plot_array plot_ka];
        plot_names{end+1} = 'End rep (no stim)';
    end
    if(kt==1)
        plot_array = [plot_array plot_kt];
        plot_names{end+1} = 'Stop stim by user';
    end

    legend(plot_array,plot_names);
end % i_loop    
data = c_t;
end

