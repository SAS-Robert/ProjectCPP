function [outData_t, outData_f] = test_filts(directory,type,name,order, bwidth)
% type = '0' -> Raw data, 'C' -> SAS filters, 'M' -> use Matlab filters
[amount, dummy ] = size(directory);

srate = 1000;
temp_out_t = [];
temp_out_f = [];

Fn = srate/2;
Hphz = 20/Fn;
Lphz = 300/Fn;
[b,a] = butter(4,[Hphz,Lphz],'bandpass');

% Matlab Butterworth bandstop example
% [b,a] = butter(3,[0.2 0.6],'stop');
% freqz(b,a)

Sp1Hz = (50-bwidth/2)/Fn;
Sp2Hz = (50+bwidth/2)/Fn;
[b50,a50] = butter(order,[Sp1Hz,Sp2Hz],'stop');

Sp1Hz = (100-bwidth/2)/Fn;
Sp2Hz = (100+bwidth/2)/Fn;
[b100,a100] = butter(order,[Sp1Hz,Sp2Hz],'stop');

Stop50 = designfilt('bandstopfir', 'PassbandFrequency1', 47, 'StopbandFrequency1', 49, 'StopbandFrequency2', 51, 'PassbandFrequency2', 53, 'PassbandRipple1', 1, 'StopbandAttenuation', 80, 'PassbandRipple2', 1, 'SampleRate', srate);
Stop100 = designfilt('bandstopfir', 'PassbandFrequency1', 80, 'StopbandFrequency1', 89, 'StopbandFrequency2', 110, 'PassbandFrequency2', 120, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate);
Cheby50 = designfilt('bandstopiir', 'PassbandFrequency1', 47, 'StopbandFrequency1', 49, 'StopbandFrequency2', 51, 'PassbandFrequency2', 53, 'PassbandRipple1', 1, 'StopbandAttenuation', 80, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');
Cheby100 = designfilt('bandstopiir', 'PassbandFrequency1', 80, 'StopbandFrequency1', 89, 'StopbandFrequency2', 110, 'PassbandFrequency2', 120, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');

% Cheby150 = designfilt('bandstopfir', 'PassbandFrequency1', 147, 'StopbandFrequency1', 149, 'StopbandFrequency2', 151, 'PassbandFrequency2', 153, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');
   % Cheby200 = designfilt('bandstopfir', 'PassbandFrequency1', 197, 'StopbandFrequency1', 199, 'StopbandFrequency2', 201, 'PassbandFrequency2', 203, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');

% More butt filters -> After supervisor meeting
Sp1Hz = (150-bwidth/2)/Fn;
Sp2Hz = (150+bwidth/2)/Fn;
[b150,a150] = butter(order,[Sp1Hz,Sp2Hz],'stop');

Sp1Hz = (200-bwidth/2)/Fn;
Sp2Hz = (200+bwidth/2)/Fn;
[b200,a200] = butter(order,[Sp1Hz,Sp2Hz],'stop');
   
Sp1Hz = (250-bwidth/2)/Fn;
Sp2Hz = (250+bwidth/2)/Fn;
[b250,a250] = butter(order,[Sp1Hz,Sp2Hz],'stop');
   
for i=1:amount
full_name = [directory(i).folder '\' directory(i).name]; %Just take the last one
data = (load(full_name))';
C_raw = data(1,:);
C_Butty = data(2,:);
C_Cheby50 = data(3,:);
C_Cheby100 = data(4,:);

% Current filtering implemented in C:
MatButty =filtfilt(b,a,C_raw);

MatCheby50 = filtfilt(Cheby50,MatButty);
MatCheby100 = filtfilt(Cheby100,MatCheby50);

% Other filters
% 1.
MatStop50 = filtfilt(Stop50,MatButty);
MatStop100 = filtfilt(Stop100,MatStop50);
% 2.
%MatCheby150 = filtfilt(Cheby150,MatStop100);
%MatCheby200 = filtfilt(Cheby200,MatCheby150);
% 3.
BStop50 = filtfilt(b50,a50,MatButty);
BStop100 = filtfilt(b100,a100,BStop50);
BStop150 = filtfilt(b150,a150,BStop100);
BStop200 = filtfilt(b200,a200,BStop150);
BStop250 = filtfilt(b250,a250,BStop200);

temp_plot = BStop250;       
        
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


