function data = plot_th3(files_dir,name,pos,plot_type)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

% getting filtered EMG 
data_dir=dir([files_dir '_filter_*']);
%[c_t, c_f] = samples_analysis(data_dir(pos),'C',1,[name ' SAS filtered data spectrum']);
full_name = [data_dir(pos).folder '\' data_dir(pos).name]; %Just take the last one
data = (load(full_name))';
c_raw = data(1,:);
c_t = data(4,:);
srate = 1000;

t_emg = zeros(1,length(c_t));
for i=1:length(t_emg)
    t_emg(i) = i/1000;
end

% accessing th directory and variables
%th_dir=dir([files_dir '_th_*']);
full_name = strrep(full_name,'_filter_','_th_'); % [th_dir(pos).folder '\' th_dir(pos).name]; %Just take the last one
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
%log_dir=dir([files_dir '_log_*']);
full_name = strrep(full_name,'_th_','_log_');%[log_dir(pos).folder '\' log_dir(pos).name]; %Just take the last one
data = (load(full_name))';
log_type = data(1,:);
log_val = data(2,:);

% new resting mean threshold  
temp_value = abs(c_t(tinit(1):tend(1)));
ny_th = mean(temp_value) * ones(1,tend(1)-tinit(1));
%if(sd_type==1)
    nth_value = (mean(temp_value) + std(temp_value)/2) * ones(1,tend(end)-tend(1));
%else
%    nth_value = (mean(temp_value) + std(temp_value)/3) * ones(1,tend(end)-tend(1));
%end

% new mean and sd 
ny_t =[];
for j=(th_pos_start+1):length(mean_v)
    y_temp = mean(abs(c_t(tinit(j):tend(j)))) * ones(1,toff(1,j));
    ny_t = [ny_t y_temp];
end


if(plot_type=='C')
    figure('Name',name)
    %
    subplot(2,2,1)
    hold on
    grid on
    plot(t_emg, c_raw')
    plot(t_emg, c_t')
    legend('Raw EMG','Filtered EMG')
    xlim([0 t_emg(end)]);
    %ylim([0 max(c_t(3000:end))+0.0002]);
    title('Time domain')
    xlabel('t (s)');
    ylabel('v (V)');

    subplot(2,2,2)
    hold on
    plot_r = fftEMG(c_raw,['Frequency domain EMG'],srate);
    plot_f = fftEMG(c_t,['Frequency domain EMG'],srate);
    legend('Raw EMG','Filtered EMG')
    xlim([0 500])
     if (max(plot_f)<0.004)
         ylim([0 max(plot_f)])
     else
         ylim([0 0.004])
     end
    %
    subplot(2,1,2)
    hold on
    grid on 
    plot1 = plot(t_emg, abs(c_t'));
    plot2 = plot(r_t_th,r_th,'g');
    plot3 = plot(t,y_t,'r');
    plot4 = plot(t, th_value,'y');
    plot_array = [plot1 plot2 plot3 plot4];
    plot_names = {'EMG','Resting mean', 'Activity mean', 'Threshold'};
    %plot4(1).LineWidth = 2;
    %plot(t,ny_t,'k');

    title('Retified-Filtered data and mean values');
    xlim([0 t(end)]);
    ylim([0 max(c_t(3000:end))+0.0002]);
    xlabel('t (s)');
    ylabel('v (V)');
    
    % plotting the logs 
    ma = 0; ka = 0; mc = 0; gx = 0; gt = 0; 
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
                log_plot = 'mo';    % magenta circle = end of repetition (with stimulator)
                plot_mc = plot(temp_t_value, y_t(log_val(k)-temp_x_value), log_plot);
                mc = 1;
            case 4
                log_plot = 'gx';    % green X = start repetition
                plot_gx = plot(temp_t_value, y_t(log_val(k)-temp_x_value), log_plot);
                gx = 1;
            case 5
                log_plot = 'g^';    % Start training pressed
                plot_gt = plot(temp_t_value, y_t(log_val(k)-temp_x_value), log_plot);
                gt = 1;
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
    if(ka==1)
        plot_array = [plot_array plot_ka];
        plot_names{end+1} = 'End rep (no stim)';
    end


    legend(plot_array,plot_names);
    
elseif(plot_type=='T')
    figure('Name',name)
    %
    subplot(2,2,1)
    hold on
    grid on
    plot(t_emg, c_raw')
    plot(t_emg, c_t')
    legend('Raw EMG','Filtered EMG')
    xlim([0 t_emg(end)]);
    %ylim([0 max(c_t(3000:end))+0.0002]);
    title('Time domain')
    xlabel('t (s)');
    ylabel('v (V)');

    subplot(2,2,2)
    hold on
    plot_r = fftEMG(c_raw,['Frequency domain EMG'],srate);
    plot_f = fftEMG(c_t,['Frequency domain EMG'],srate);
    legend('Raw EMG','Filtered EMG')
    xlim([0 500])
     if (max(plot_f)<0.004)
         ylim([0 max(plot_f)])
     else
         ylim([0 0.004])
     end
    %
    subplot(2,1,2)
    hold on
    grid on 
    plot(t_th,y_th,'g',t,y_t,'r')
    plot(t, th_value,'b')
    plot(t_th,ny_th,'--k',t,ny_t,'k', t, nth_value, '--m');

    title('Comparsion between old and new mean values');
    xlim([0 t(end)]);
    ylim([0 max(y_t)+0.0002]);
    legend('Old resting mean', 'Old mean', 'Old threshold', 'New resting mean', 'New mean', 'New threshold')
    xlabel('t (s)');
    ylabel('v (V)');
end


data = c_t;
end

