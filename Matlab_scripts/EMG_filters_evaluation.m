%%%%%%%%%%%%%%%%%%%% FIRST DAY RECORDINGS %%%%%%%%%%%%%%%%%%%
%% Load data files
path = pwd;
% Add source directories
data_path = genpath('SAS');
fcn_path = genpath('Matlab_scripts');
addpath(data_path);
addpath(fcn_path);

%% First time running this: 
% Find and filter files
noise_dir=dir('SAS/files/noise_iir_*.txt');
CA_nomove_dir=dir('SAS/files/CA_nomove_iir_*.txt');
CA_ind_dir=dir('SAS/files/CA_ind_iir_*.txt');
CA_move_dir=dir('SAS/files/CA_move_iir_*.txt');
% All the files are located in the same directory:
folder_dir = noise_dir(1).folder;
% [amount, dummy ] = size(noise_dir);
% 
% % Load containing data into matlab variables/files
% noise_full_name1 = [folder_dir '\' noise_dir(1).name];
% noise_full_name2 = [folder_dir '\' noise_dir(2).name];
% noise_full_name3 = [folder_dir '\' noise_dir(3).name];
% noise_data1 = load(noise_full_name1);
% noise_data2 = load(noise_full_name2);
% noise_data3 = load(noise_full_name3);
% 
% CA_nomove_full_name1 = [folder_dir '\' CA_nomove_dir(1).name];
% CA_nomove_full_name2 = [folder_dir '\' CA_nomove_dir(2).name];
% CA_nomove_full_name3 = [folder_dir '\' CA_nomove_dir(3).name];
% CA_nomove_data1 = load(CA_nomove_full_name1);
% CA_nomove_data2 = load(CA_nomove_full_name2);
% CA_nomove_data3 = load(CA_nomove_full_name3);
% 
% CA_ind_full_name1 = [folder_dir '\' CA_ind_dir(1).name];
% CA_ind_full_name2 = [folder_dir '\' CA_ind_dir(2).name];
% CA_ind_full_name3 = [folder_dir '\' CA_ind_dir(3).name];
% CA_ind_data1 = load(CA_ind_full_name1);
% CA_ind_data2 = load(CA_ind_full_name2);
% CA_ind_data3 = load(CA_ind_full_name3);
% 
% CA_move_full_name1 = [folder_dir '\' CA_move_dir(1).name];
% CA_move_full_name2 = [folder_dir '\' CA_move_dir(2).name];
% CA_move_full_name3 = [folder_dir '\' CA_move_dir(3).name];
% CA_move_data1 = load(CA_move_full_name1);
% CA_move_data2 = load(CA_move_full_name2);
% CA_move_data3 = load(CA_move_full_name3);

% Put all data toguether in a single load statement:
for i=1:4
    switch i
        case 1
            temp_dir = noise_dir;
        case 2
            temp_dir = CA_nomove_dir;
        case 3
            temp_dir = CA_ind_dir;
        case 4
            temp_dir = CA_move_dir;
    end
    % Access files 
    full_name1 = [folder_dir '\' temp_dir(1).name];
    full_name2 = [folder_dir '\' temp_dir(2).name];
    full_name3 = [folder_dir '\' temp_dir(3).name];
    temp1 = load(full_name1);
    temp2 = load(full_name2);
    temp3 = load(full_name3);

    % Put all sample size equal
    sample_size = min([length(temp1) length(temp2) length(temp3)]);
    temp = zeros(sample_size,length(temp1(1,:)),3);
    temp(:,:,1)=temp1(1:sample_size,:);
    temp(:,:,2)=temp2(1:sample_size,:);
    temp(:,:,3)=temp3(1:sample_size,:);
    
    switch i
        case 1
            noise_data = temp;
        case 2
            CA_nomove_data = temp;
        case 3
            CA_ind_data = temp;
        case 4
            CA_move_data = temp;
    end
end

save('Matlab_scripts/EMG_CA_2020_10_23.mat','noise_data','CA_nomove_data','CA_ind_data','CA_move_data');

%% For other times: 
srate = 4000;           % sampling rate
load('Matlab_scripts/EMG_CA_2020_10_23.mat');
% Separate all data from raw and filtered
for j=1:4
    switch j 
        case 1
            temp_data = noise_data;
        case 2
            temp_data = CA_nomove_data;
        case 3
            temp_data = CA_ind_data;
        case 4
            temp_data = CA_move_data;   
    end
    
    [samples col amount] = size(temp_data);
    temp_raw = zeros(samples,amount);
    temp_f1 = zeros(samples,amount);
    temp_f2 = zeros(samples,amount);
    temp_f3 = zeros(samples,amount);
    for i=1:amount
        temp_raw(:,i) = temp_data(:,1,i);
        temp_f1 (:,i) = temp_data(:,2,i);
        temp_f2(:,i) = temp_data(:,3,i);
        temp_f3(:,i) = temp_data(:,4,i);
    end

    switch j 
        case 1
            noise_raw = temp_raw;
            noise_b = temp_f1;
            noise_c50 = temp_f2;
            noise_c100 = temp_f3;
        case 2
            CA_nomove_raw = temp_raw;
            CA_nomove_b = temp_f1;
            CA_nomove_c50 = temp_f2;
            CA_nomove_c100 = temp_f3;
        case 3
            CA_ind_raw = temp_raw;
            CA_ind_b = temp_f1;
            CA_ind_c50 = temp_f2;
            CA_ind_c100 = temp_f3;
        case 4
            CA_move_raw = temp_raw;
            CA_move_b = temp_f1;
            CA_move_c50 = temp_f2;
            CA_move_c100 = temp_f3;     
    end
end


%% Processing data 

t = zeros(1,length(CA_ind_raw));
th_data = [];
for i = 1:length(t)
 t(i) = i/srate;
 if(t(i)<3)
     th_data = [th_data; abs(CA_move_c100(i,:))];
 end
end
%subplot(3,1,1)
figure
plot(t,CA_ind_raw(:,1)')
xlim([0 t(1,length(t)) ])


% Christians threshold:
c_th = mean(th_data)*2.5;
c_th_v = zeros(3,length(CA_move_raw)); c_th_v(1,:)=c_th(1); c_th_v(2,:)=c_th(2); c_th_v(3,:)=c_th(3);

% Girls threshold:
th1 =  mean(th_data)+ std(th_data)/2;
th2 =  mean(th_data)+ std(th_data)/3;
th1_v = zeros(3,length(CA_move_raw)); th1_v(1,:)=th1(1); th1_v(2,:)=th1(2); th1_v(3,:)=th1(3);

figure
subplot(3,1,1)
hold all 
plot(t,abs(CA_move_c100(:,1)'))
plot(t,th1_v(1,:))
plot(t,c_th_v(1,:))
xlim([0 t(1,length(t)) ])

subplot(3,1,2)
hold all 
plot(t,abs(CA_move_c100(:,2))')
plot(t,th1_v(2,:))
plot(t,c_th_v(2,:))
xlim([0 t(1,length(t)) ])

subplot(3,1,3)
hold all 
plot(t,abs(CA_move_c100(:,3))')
plot(t,th1_v(3,:))
plot(t,c_th_v(3,:))
xlim([0 t(1,length(t)) ])



t = zeros(1,length(noise));
th_data = [];
for i = 1:length(t)
 t(i) = i/srate;
 if(t(i)<3)
     th_data = [th_data; noise(i,:)];
 end
end
%subplot(3,1,1)
plot(t,CA_move_c100')
xlim([0 t(1,length(t)) ])

%% Matlab's filters
% Christian's filters
Fn = srate/2;
Hphz = 20/Fn;
Lphz = 300/Fn;
%[Hb,Ha]=butter(4,(Hphz/s.Rate),'high');
%[Lb,La]=butter(4,(Lphz/s.Rate),'low');
[b,a] = butter(4,[Hphz,Lphz],'bandpass');

Cheby50 = designfilt('bandstopiir', 'PassbandFrequency1', 47, 'StopbandFrequency1', 49, 'StopbandFrequency2', 51, 'PassbandFrequency2', 53, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', 4000, 'DesignMethod', 'cheby2');
Cheby100 = designfilt('bandstopiir', 'PassbandFrequency1', 97, 'StopbandFrequency1', 99, 'StopbandFrequency2', 101, 'PassbandFrequency2', 103, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', 4000, 'DesignMethod', 'cheby2');

MatButty =filtfilt(b,a,CA_move_raw(:,1)');
MatCheby50 = filtfilt(Cheby50,MatButty);
MatCheby100 = filtfilt(Cheby100,MatCheby50);
%MatData = [CA_move_raw(:,1)'; MatButty; MatCheby50; MatCheby100];
fftEMG(MatCheby100);
fftEMG(CA_ind_raw(:,1)');
%%
Fs = 4000;            % Sampling frequency                    
T = 1/Fs;             % Sampling period       
L = length(CA_move_raw(:,1)');             % Length of signal
t = (0:L-1)*T;        % Time vector
S = 0.7*sin(2*pi*50*t) + sin(2*pi*120*t);
X = S + 2*randn(size(t));
plot(1000*t(1:50),X(1:50))
title('Signal Corrupted with Zero-Mean Random Noise')
xlabel('t (milliseconds)')
ylabel('X(t)')

X = abs(CA_move_raw(:,1)');
Y = fft(X);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = Fs*(0:(L/2))/L;
plot(f,P1) 
title('Single-Sided Amplitude Spectrum of X(t)')
xlabel('f (Hz)')
ylabel('|P1(f)|')


%% Hasomed example:
Hasomed_addr = 'C:\Users\Carolina\Desktop\Internship\Software\Hasomed_examples\IngestTest\IngestTest';
addpath(genpath(Hasomed_addr));
% Hasomed_dir=dir([Hasomed_addr '\' 'EMG_*.txt']);
Hasomed_dir=dir([Hasomed_addr '\' 'CA_ind1_EMG_*.txt']);
folder_dir = Hasomed_dir(1).folder;
hasomed_full_name1 = [folder_dir '\' Hasomed_dir(1).name];
hasomed_data = load(hasomed_full_name1);

hasomed_bio = hasomed_data(:,1)';
hasomed_emg1 = abs(hasomed_data(:,2)');
hasomed_emg2 = hasomed_data(:,3)';
hasomed_analog = hasomed_data(:,4)';
hasomed_offset = hasomed_data(:,5)';

t = zeros(1, length(hasomed_emg1));
for i=2:length(t)
    t(i) = i/4000;
end
figure('Name', 'Raw data')
plot(t,hasomed_emg1)

fftEMG(hasomed_emg1)
fftEMG(hasomed_emg2)

% Action of the filters:
MatButty1 =filtfilt(b,a,hasomed_emg1);
MatCheby50_1 = filtfilt(Cheby50,MatButty1);
MatCheby100_1 = filtfilt(Cheby100,MatCheby50_1);
figure('Name', 'Filtered data')
plot(t,MatCheby100_1)
fftEMG(MatCheby100_1);
fftEMG(MatCheby50_1);

MatButty2 =filtfilt(b,a,hasomed_emg2);
MatCheby50_2 = filtfilt(Cheby50,MatButty2);
MatCheby100_2 = filtfilt(Cheby100,MatCheby50_2);
fftEMG(MatCheby100_2);


figure('Name','Raw data')
subplot(2,2,1)
t = zeros(1,length(C_raw));
for i = 1:length(t)
 t(i) = i/srate;
end
plot(t,C_raw)
title([files_iir(2).name ' time domain'])
subplot(2,2,2)
fftEMG(C_raw);
subplot(2,2,3)
t = zeros(1, length(hasomed_emg1));
for i=2:length(t)
    t(i) = i/4000;
end
plot(t,hasomed_emg1)
subplot(2,2,4)
fftEMG(hasomed_emg1);
%%
Hasomed_addr = 'C:\Users\Carolina\Desktop\Internship\Software\Hasomed_examples\IngestTest\IngestTest';
Hasomed_robot=dir([Hasomed_addr '\' 'CA_ind*.txt']);
files_robot=dir('SAS/files/CA_ind_iir_20201027_*.txt');

Hasomed_desk=dir([Hasomed_addr '\' 'EMG_desk_*.txt']);
files_desk=dir('SAS/files/CA_desk_iir_20201027_*.txt');

% At the robot
samples_analysis(files_robot,'C',0,'ROBERT-SAS raw data');
samples_analysis(files_robot,'C',1,'ROBERT-SAS filtered data');
samples_analysis(Hasomed_robot,'H',0,'ROBERT-Hasomed raw data');
samples_analysis(Hasomed_robot,'H',1,'ROBERT-Hasomed filtered data');

% At the desk
samples_analysis(files_desk,'C',0,'Desk-SAS raw data');
samples_analysis(files_desk,'C',1,'Desk-SAS filtered data');
samples_analysis(Hasomed_desk,'H',0,'Desk-Hasomed raw data');
