function [outData_t, outData_f] = test_filts(directory,name,order, bwidth)

[amount, dummy ] = size(directory);

srate = 1000;
temp_out_t = [];
temp_out_f = [];

Fn = srate/2;
Hphz = 20/Fn;
%Lphz = 300/Fn;                             % Original
Lphz = 90/Fn;                             % New
[b,a] = butter(4,[Hphz,Lphz],'bandpass');

% Matlab Butterworth bandstop example
% [b,a] = butter(3,[0.2 0.6],'stop');
% freqz(b,a)

Sp1Hz = (50-bwidth/2)/Fn;
Sp2Hz = (50+bwidth/2)/Fn;
[b50,a50] = butter(2,[Sp1Hz,Sp2Hz],'stop');

Fn = srate/2;
Lphz = (100)/Fn;     
[bC100,aC100] = cheby2(8,25,Lphz);

abC100 = designfilt('lowpassiir','FilterOrder',8, 'PassbandFrequency',90,'PassbandRipple',0.5, 'SampleRate',srate);

for i=1:amount
full_name = [directory(i).folder '\' directory(i).name]; %Just take the last one
data = (load(full_name))';
C_raw = data(1,:);
C_Butty = data(2,:);
C_Cheby50 = data(3,:);
C_Cheby100 = data(4,:);

% Current filtering implemented in C:
MatButty =filtfilt(b,a,C_raw);

% Other filters
% 2.
% 3.
BStop50 = filtfilt(b50,a50,MatButty);
%4.
Clow100 = filtfilt(abC100,BStop50);
Cheby_low100 = filtfilt(bC100,aC100,BStop50);


temp_plot = Cheby_low100;       
        
    t = zeros(1,length(temp_plot));
    for j = 1:length(t)
     t(j) = j/srate;
    end

% Adding threshold values
    th_name = strrep(directory(i).name,'filter','th');
    th_full_name = [directory(i).folder '\' th_name]; %Just take the last one
    data = (load(th_full_name))';
    mean_v = data(1,:);
    sd_v = data(2,:);
    tinit = data(3,:);
    tend = data(4,:);
    toff = data(5,:);
    % threshold values
    y_th = mean_v(1,1) * ones(1,tend(1)-tinit(1));
    t_th = zeros(1,tend(1)-tinit(1));
    for j=1:length(t_th)
        t_th(j) = (tinit(1)+j)/1000;
    end
    % mean values
    y_t =[];
    for j=2:length(mean_v)
        y_temp = mean_v(1,j) * ones(1,toff(1,j));
        y_t = [y_t y_temp];
    end
    t_t = zeros(1,tend(end)-tend(1));
    for j=1:length(t_t)
        t_t(j) = t_th(end) + j/1000;
    end
    th_value = toff(1) * ones(1,tend(end)-tend(1));
    % new mean  
    ny_t =[];
    for j=2:length(mean_v)
        y_temp = mean(abs(temp_plot(tinit(j):tend(j)))) * ones(1,toff(1,j));
        ny_t = [ny_t y_temp];
    end
    
    % Plotting
    figure('Name',[name])
    subplot(2,2,1)
    hold on
    grid on
    plot(t,abs(temp_plot))
    xlim([0 t(1,length(t))])
    title(['Time domain retified EMG' num2str(i)], 'Interpreter','none'); 
    xlabel('t (s)');
    ylabel('v (V)');
    %legend('EMG','SAS-resting mean', 'SAS-activity mean', 'SAS-Threshold', 'MatLab new mean')

    subplot(2,2,2)
    plot_f = fftEMG(temp_plot,['Frequency domain EMG ' num2str(i)],srate);
    xlim([0 500])
    %xlim([0 130])       % only for Butterworth testing
   % ylim([0 0.0002])     % only for Butterworth testing
%     if(type=='C' || type=='M')
      ylim([0 max(plot_f)])
%     else
%         if (max(temp_plot)<0.004)
%             ylim([0 max(plot_f)])
%         else
%             ylim([0 0.004])
%         end
%     end
    
    subplot(2,1,2)
    grid on
    hold on
    plot(t_th,y_th,'g',t_t,y_t,'r',t_t, th_value,'y')
    plot(t_t,ny_t,'k');   
    xlim([0 t(1,length(t))])
    title(['Comparison between means EMG ' num2str(i)], 'Interpreter','none'); 
    xlabel('t (s)');
    ylabel('v (V)');
    legend('SAS-resting mean', 'SAS-activity mean', 'SAS-Threshold', 'MatLab new mean')    
    
%    ylim([0 1.5*0.0001])     % only for Butterworth testing
%     figure('Name',[name ' difference'])
%     plot(t,temp_plot,t,abs(MatCheby100)-abs(temp_plot))
%     xlim([0 t(1,length(t))])
%     legend('New EMG','Difference with old one')
%     title('Filtered EMGs difference');

    % for the output
    temp_out_t = [temp_out_t; (temp_plot')];
    temp_out_f = [temp_out_f; (plot_f')];
end
    outData_t = temp_out_t;
    outData_f = temp_out_f;
end


