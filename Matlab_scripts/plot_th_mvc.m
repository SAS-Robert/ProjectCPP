function data = plot_th_mean_v(files_dir,name,pos,plot_type)
%(--- First version ---)
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
% NOTE: use only on recorded data from 1st to 9th November.
%
% process_data_iir:  fileVALUES << GL_thMethod << "," << GL_processed << "," << v_size << "," << N_len << "," << mean << "," << THRESHOLD << "," << MEAN << "," << GL_exercise << "," << 3 << "\n";
% process_th_mean: fileVALUES << GL_thMethod << "," << GL_processed << "," << v_size << "," << N_len << "," << mean << "," << TH_DISCARD << "," << MEAN << "," << GL_exercise << "," << 1 << "\n";
%                  fileVALUES << GL_thMethod << "," << GL_processed << "," << v_size << "," << N_len << "," << mean << "," << THRESHOLD << "," << MEAN << "," << GL_exercise << "," << 1 << "\n";
% process_th_mvc: fileVALUES << GL_thMethod << "," << GL_processed << "," << v_size << "," << N_len << "," << MVC << "," << TH_DISCARD << "," << MEAN << "," << GL_exercise << "," << 2 << "\n";
%                 fileVALUES << GL_thMethod << "," << GL_processed << "," << v_size << "," << N_len << "," << MVC << "," << THRESHOLD << "," << MEAN << "," << GL_exercise << "," << 2 << "\n";

% getting filtered EMG
data_dir=dir([files_dir '_filter_*']);
%[c_t, c_f] = samples_analysis(data_dir(pos),'C',1,[name ' SAS filtered data spectrum']);
full_name = [data_dir(pos).folder '\' data_dir(pos).name]; %Just take the last one
data = (load(full_name))';
c_raw = data(1,:);
%c_raw = c_raw(1:63500);
c_t = data(3,:);
%c_t = c_t(1:63500);
srate = 1000;

t_emg = zeros(1,length(c_t));
for i=1:length(t_emg)
    t_emg(i) = i/1000;
end

% accessing th directory and variables
th_dir=dir([files_dir '_th_*']);
full_name = [th_dir(pos).folder '\' th_dir(pos).name]; %Just take the last one
data = (load(full_name))';
meanIdx = find(data(9,:)==1);
mvcIdx = find(data(9,:)==2);
trainIdx = find(data(9,:)==3);

GL_processed = data(2,:);
v_size = data(3,:);
N_len = data(4,:);
mean_v = data(5,:);
threshold = data(6,:);

tinit = threshold(1);

%GL_processed = data(2,:);
% v_size = data(5,:);
% N_len = data(6,:);
% mean_v = data(2,:);
% tinit = data(4,1);
%threshold = data(6,:);

%v_size = data(5,:);
%N_len = data(6,:);

% threshold values
%y_idx_start = find(v_size>1100,1);
%y_idx_stop = find(v_size>1100+3000,1);

%mean_data = abs(c_t(v_size(y_idx_start):v_size(y_idx_stop)));
y_th = mean_v(1,10) * ones(1,v_size(meanIdx(end))-tinit);

%y_th = mean(mean_data) * ones(1,v_size(meanIdx(end))-tinit);


t_th = zeros(1,v_size(meanIdx(end))-tinit);
for i=1:length(t_th)
    t_th(i) = (tinit+i)/1000;
end

if not(isempty(mvcIdx))
    % MVC Data
    y_mvc = [];
    for i = 1:length(mvcIdx)
        y_temp = mean_v(1,mvcIdx(i)) * ones(1,N_len(1,mvcIdx(i)));
        y_mvc = [y_mvc y_temp];
    end
    
    t1 = zeros(1,v_size(mvcIdx(end))-v_size(mvcIdx(1)-1));
    for i=1:(v_size(mvcIdx(end))-v_size(mvcIdx(1)-1))
        t1(i) = v_size(mvcIdx(1)-1)/1000 + i/1000;
    end
end
% mean values
y_t =[];
if isempty(mvcIdx)
    for i=length(meanIdx)+length(mvcIdx)+1:length(data(1,:))
        try    
            y_temp = mean_v(1,i) * ones(1,N_len(1,i));
        catch
            % Do nothing
        end
        y_t = [y_t y_temp];
    end
else
    for i=mvcIdx(end)+1:length(data(1,:))
        try    
            y_temp = mean_v(1,i) * ones(1,N_len(1,i));
        catch
            % Do nothing
        end
        y_t = [y_t y_temp];
    end
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
t = zeros(1,length(y_t));
for i=1:length(t)
    t(i) = t_th(end) + i/1000;
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%if isempty(mvcIdx)
%    t2 = zeros(1,v_size(trainIdx(end))-v_size(meanIdx(end)));
%    for i=1:length(t2)
%        t2(i) = t_th(end) + i/1000;
%    end
%else
%    t2 = zeros(1,v_size(end)-v_size(mvcIdx(end)));
%    for i=1:length(t2)
%        t2(i) = t1(end) + i/1000;
%    end
%end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% if data(1,1) == 0
%     th_value = mean(mean_data)+3*std(mean_data);
% elseif data(1,1) == 1
% elseif data(1,1) == 2
% elseif data(1,1) == 3
% end
%     th_value1 = mean(mean_data)+2*std(mean_data);
%
% else
%     mvc_data=abs(c_t(v_size(mvcIdx(1)):v_size(mvcIdx(end))));
%     th_value1 = mean(mean_data)+0.05*max(mvc_data);
%     th_value2 = mean(mean_data)+0.1*max(mvc_data);
%
% end

log_dir=dir([files_dir '_log_' full_name(end-18:end)]);
full_name = [log_dir.folder '\' log_dir.name]; %Just take the last one
log_data = (load(full_name))';
log_type = log_data(1,:);
log_val = log_data(2,:);

if(plot_type=='C')
    figure('Name',name,'units','normalize','outerposition',[0 0 1 1])
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
    plot2 = plot(t_th,y_th,'g');
    if not(isempty(mvcIdx))
        plot3 = plot(t1,y_mvc,'c');
    end
    plot4 = plot(t,y_t,'r');
    plot5 = plot([t(1) t(end)], [threshold(trainIdx(end)) threshold(trainIdx(end))],'y');
    % plot5 = plot([t2(1) t2(end)], [threshold(1) threshold(1)],'y');
    if not(isempty(mvcIdx))
        plot_array = [plot1 plot2 plot3 plot4 plot5];
    else
        plot_array = [plot1 plot2 plot4 plot5];
    end
    
    %plot(t,ny_t,'k');
    
    title('Retified-Filtered data and mean values');
    xlim([0 t_emg(end)]);
    ylim([0 max(c_t(3000:end))+0.0002]);
    
    if data(1,1) == 0
        threshLabel = 'Mean+3SD';
    elseif data(1,1) == 1
        threshLabel = 'Mean+2SD';
    elseif data(1,1) == 2
        threshLabel = 'Mean+0.05*MVC';
    elseif data(1,1) == 3
        threshLabel = 'Mean+0.10*MVC';
    end
    
    
    if not(isempty(mvcIdx))
        plot_names={'EMG','Resting mean','MVC Data', 'Activity mean', threshLabel};
    else
        plot_names={'EMG','Resting mean','Activity mean', threshLabel};
    end
    
    xlabel('t (s)');
    ylabel('v (V)');
    
    
    % plotting the logs
    ma = 0; ka = 0; mc = 0; gx = 0; gt = 0; kc = 0; kt = 0;
    for k=1:length(log_type)
        log_plot = 'b.';
        temp_x_value = round(t(1)*1000);
        temp_t_value = log_val(k)/1000;
        %tempY= [y_th y_mvc y_t];
        
        switch log_type(k)
            case 1                  % magenta asterisk: stimulator triggered
                log_plot = 'm*';
                plot_ma = plot(temp_t_value, y_t(log_val(k)-temp_x_value), log_plot);
                ma = 1;
                %             case 2
                %                 log_plot = 'k*';    % black asterisk = end of repetition (without triggering stimulator)
                %                 plot_ka = plot(temp_t_value, y_t(log_val(k)-temp_x_value), log_plot);
                %                 ka = 1;
            case 3
                log_plot = 'm-';    % magenta circle = end of repetition (with stimulator)
                plot_mc = plot(temp_t_value*ones(1,3), [0 y_t(log_val(k)-temp_x_value) 0.02], log_plot);
                mc = 1;
            case 4
                log_plot = '-';    % gx green X = start repetition
                plot_gx = plot(temp_t_value*ones(1,3), [0 y_t(log_val(k)-temp_x_value) 0.02], log_plot, 'Color', [0.4660 0.6740 0.1880]);
                gx = 1;
            case 5
                log_plot = 'g^';    % Start training pressed
                plot_gt = plot(temp_t_value, y_t(log_val(k)-temp_x_value), log_plot);
                gt = 1;
            case 6
                log_plot = 'y*';    % Stimulator actually being stopped
                plot_kc = plot(temp_t_value, y_t(log_val(k)-temp_x_value), log_plot);
                kc = 1;
                %              case 7
                %                 log_plot = 'k^';    % Stimulator stopped by user
                %                 plot_kt = plot(temp_t_value, y_t(log_val(k)-temp_x_value), log_plot);
                %                 kt = 1;
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
    
elseif(plot_type=='T')
    
    % new resting mean threshold
    temp_value = abs(c_t(tinit(1):v_size(1)));
    ny_th = mean(temp_value) * ones(1,v_size(1)-tinit(1));
    nth_value = (mean(temp_value) + std(temp_value)/3) * ones(1,v_size(end)-v_size(1));
    
    % new mean and sd
    ny_t =[];
    for j=2:length(mean_v)
        y_temp = mean(abs(c_t(tinit(j):v_size(j)))) * ones(1,N_len(1,j));
        ny_t = [ny_t y_temp];
    end
    
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

