function EMG_analysis_filter(directory,type,name)
% Explanation here 

[amount, dummy ] = size(directory);
plot_nr = 0;
srate = 4000;

if(type=='H')
    Fn = srate/2;
    Hphz = 20/Fn;
    Lphz = 300/Fn;
    %[Hb,Ha]=butter(4,(Hphz/s.Rate),'high');
    %[Lb,La]=butter(4,(Lphz/s.Rate),'low');
    [b,a] = butter(4,[Hphz,Lphz],'bandpass');
    Cheby50 = designfilt('bandstopiir', 'PassbandFrequency1', 47, 'StopbandFrequency1', 49, 'StopbandFrequency2', 51, 'PassbandFrequency2', 53, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', 4000, 'DesignMethod', 'cheby2');
    Cheby100 = designfilt('bandstopiir', 'PassbandFrequency1', 97, 'StopbandFrequency1', 99, 'StopbandFrequency2', 101, 'PassbandFrequency2', 103, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', 4000, 'DesignMethod', 'cheby2');
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
        
        MatButty =filtfilt(b,a,abs(hasomed_emg1));
        MatCheby50 = filtfilt(Cheby50,MatButty);
        temp_filt = filtfilt(Cheby100,MatCheby50);
    
    else
        C_raw = data(1,:);
        C_Butty = data(2,:);
        C_Cheby50 = data(3,:);
        C_Cheby100 = data(4,:);
        
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
    plot(t,temp_plot)
    xlim([0 t(1,length(t))])
    title(['Time domain EMG ' num2str(i)], 'Interpreter','none'); 
    
    plot_nr = plot_nr+1;
    subplot(amount,2,plot_nr)
    fftEMG(temp_plot,['Frequency domain EMG ' num2str(i)])
    xlim([0 500])
    ylim([0 0.004])

end

end

