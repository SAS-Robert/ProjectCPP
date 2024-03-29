function [outData_t, outData_f] = samples_analysis(directory,type,use_filter,name)
% Plots the time and frequency domain of a EMG recording.
% - directory: dir with the folder and data file name.
% - type:
%        'H' = data file using Hasomed format (from their C++ example apps).
%        'C' = data file using SAS format (from SAS/SASApp C++ solution).
% - use_filter: plot either raw ('0') or filtered ('1') EMG.
% - name: name for the plotting.

[amount, dummy ] = size(directory);
plot_nr = 0;
srate = 1000;
temp_out_t = [];
temp_out_f = [];

if(use_filter==1)
    Fn = srate/2;
    Hphz = 20/Fn;
    Lphz = 300/Fn;
    %[Hb,Ha]=butter(4,(Hphz/s.Rate),'high');
    %[Lb,La]=butter(4,(Lphz/s.Rate),'low');
    [b,a] = butter(4,[Hphz,Lphz],'bandpass');
    Cheby50 = designfilt('bandstopiir', 'PassbandFrequency1', 47, 'StopbandFrequency1', 49, 'StopbandFrequency2', 51, 'PassbandFrequency2', 53, 'PassbandRipple1', 1, 'StopbandAttenuation', 80, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');
    Cheby100 = designfilt('bandstopiir', 'PassbandFrequency1', 80, 'StopbandFrequency1', 89, 'StopbandFrequency2', 110, 'PassbandFrequency2', 120, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');
    Cheby150 = designfilt('bandstopiir', 'PassbandFrequency1', 147, 'StopbandFrequency1', 149, 'StopbandFrequency2', 151, 'PassbandFrequency2', 153, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');
    Cheby200 = designfilt('bandstopiir', 'PassbandFrequency1', 197, 'StopbandFrequency1', 199, 'StopbandFrequency2', 201, 'PassbandFrequency2', 203, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');
end

figure('Name',name)
for i=1:amount
full_name = [directory(i).folder '\' directory(i).name]; %Just take the last one
data = (load(full_name))';

    if(type=='H')
        hasomed_bio = data(1,:);
        hasomed_emg1 = data(2,:);
        hasomed_emg2 = data(3,:);
        hasomed_analog = data(4,:);
        hasomed_offset = data(5,:);

        temp_plot = hasomed_emg1;
        
        if(use_filter==1)
            MatButty =filtfilt(b,a,hasomed_emg1);
            MatCheby50 = filtfilt(Cheby50,MatButty);
            MatCheby100 = filtfilt(Cheby100,MatCheby50);
            MatCheby150 = filtfilt(Cheby150,MatCheby100);
            MatCheby200 = filtfilt(Cheby200,MatCheby150);
            temp_plot = MatCheby200;
        end
    
    else
        C_raw = data(1,:);
        C_Butty = data(2,:);
        C_Cheby50 = data(3,:);
        C_Cheby100 = data(end,:);
        
        if(use_filter==1)
            temp_plot = C_Cheby100;
        else
            temp_plot = C_raw;
        end
    end
        
    t = zeros(1,length(temp_plot));
    for j = 1:length(t)
     t(j) = j/srate;
    end

    plot_nr = plot_nr+1;
    subplot(amount,2,plot_nr)
    hold on
    grid on
    plot(t,temp_plot)
    xlim([0 t(1,length(t))])
    title(['Time domain EMG ' num2str(i)], 'Interpreter','none'); 
    xlabel('t (s)');
    ylabel('v (V)');

    plot_nr = plot_nr+1;
    subplot(amount,2,plot_nr)
    hold on
    grid on
    plot_f = fftEMG(temp_plot,['Frequency domain EMG ' num2str(i)],srate);
    xlim([0 500])
    if(use_filter==1)
     ylim([0 max(plot_f)])
%    ylim([0 0.000004])
    else
        if (max(temp_plot)<0.004)
            ylim([0 max(plot_f)])
        else
            ylim([0 0.004])
        end
    end
    
    % for the output
    temp_out_t = [temp_out_t; (temp_plot')];
    temp_out_f = [temp_out_f; (plot_f')];
end
    outData_t = temp_out_t;
    outData_f = temp_out_f;
end

