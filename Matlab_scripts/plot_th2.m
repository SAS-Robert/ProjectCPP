function data = plot_th2(files_dir,name,pos,plot_type)
%(--- Second version ---)
% Plots the data from a single EMG recording in three formats:
%1. Top-left: raw vs. filtered data in time domain.
%2. Top-right: raw vs. filtered data in frequency domain.
%3. Bottom: rectified data adn its average.
% Options available:
% - files_dir: string with the address and name of the files.
% - name: title of the figure with the plots.
% - plot_type: what type of filtering is going to be done:
%plot_type = 'C' -> SAS program filters (from the recording).
%plot_type = 'T' -> Test Matlab filters. This is to compare how
%new/modified filters would affect the original data.
%
% NOTE: use only on recorded data from 10th November. 

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
th_dir=dir([files_dir '_th_*']);
full_name = [th_dir(pos).folder '\' th_dir(pos).name]; %Just take the last one
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
    plot(t_emg, abs(c_t'))
    plot(r_t_th,r_th,'g');
    plot(t,y_t,'r')
    plot(t, th_value,'y')
    %plot(t,ny_t,'k');

    title('Retified-Filtered data and mean values');
    xlim([0 t(end)]);
    ylim([0 max(c_t(3000:end))+0.0002]);
    legend('EMG','Resting mean', 'Activity mean', 'Threshold')
    xlabel('t (s)');
    ylabel('v (V)');
    
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

