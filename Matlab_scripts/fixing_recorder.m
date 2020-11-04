%% Fixing recorder:
path = pwd;
% Add source directories
data_path = genpath('SAS');
fcn_path = genpath('Matlab_scripts');
addpath(data_path);
addpath(fcn_path);
%% 

% recorder_complete=dir('SAS/Ingest_all_*');
% recorder_EMG1=dir('SAS/Ingest_EMG1_*');
% recorder_filt=dir('SAS/files/Ingest_iir_*');

% samples_analysis(recorder_filt,'C',0,'Test');
% samples_analysis(recorder_complete,'H',1,'Test');
Hasomed_addr = 'C:\Users\Carolina\Desktop\Internship\Software\Hasomed_examples\IngestTest\IngestTest';
Hasomed_robot=dir([Hasomed_addr '\' 'CA_ind*.txt']);
Hasomed_desk=dir([Hasomed_addr '\' 'EMG_desk_*.txt']);

C_filters_robot=dir('Matlab_scripts/CA_ind*.txt');
C_filters_desk=dir('Matlab_scripts/desk*.txt');

[Crob_t, Crob_f] = samples_analysis(C_filters_robot,'C',1,'ROBERT-SAS filtered data with Hasomed raw data');
[Hrob_t, Hrob_f] = samples_analysis(Hasomed_robot,'H',1,'ROBERT-Hasomed filtered data');
[Cdesk_t, Cdesk_f] = samples_analysis(C_filters_desk,'C',1,'Desk-SAS filtered data with Hasomed raw data');
[Hdesk_t, Hdesk_f] = samples_analysis(Hasomed_desk,'H',1,'Desk-Hasomed filtered data');

% Absolute and proportional difference
% diff_t=zeros(1,min([length(Crob_t) length(Hrob_t)]));
% diff_dV = diff_t;
% 
% for i = 1:length(diff_t)
%     diff_t(i) = Crob_t(i,1) - Hrob_t(i,1);
%     diff_dV(i) = diff_t(i)/Hrob_t(i,1); 
% end
% 
% plot([1:length(diff_dV)],abs(diff_dV))
% xlim([0 max(abs(diff_dV))])

% Looks quite fixed
C_ingest_test_raw=dir('SAS/Ingest_all_20201028_*');
C_ingest_test_filtered=dir('SAS/files/file_iir_20201028_*');

[Ctest_t, Ctest_f] = samples_analysis(C_ingest_test_filtered,'C',0,'Collected raw data');
[Craw_t, Craw_f] = samples_analysis(C_ingest_test_raw,'H',0,'Amount of processed raw data');

[ca1 cb1] = size(Ctest_t);
[cc1 cd1] = size(Ctest_f);
[ca2 cb2] = size(Craw_t);
[cc2 cd2] = size(Craw_f);

diff_size_t = [ca1 cb1] - [ca2 cb2]
diff_size_f = [cc1 cd1] - [cc2 cd2]

% checking if it's the same data:
diff_v = (abs(Ctest_t - Craw_t)');
diff_sum = sum(diff_v);
amount_diff = 0;


%% Christians arm
H_CU=dir('Matlab_scripts/CU_20201028_*.txt');
C_CU=dir('Matlab_scripts/CU_arm_20201028_*.txt');

[C_CU_t, C_CU_f] = samples_analysis(C_CU,'C',0,'Christians arm raw data - SAS');
[H_CU_t, H_CU_f] = samples_analysis(H_CU,'H',0,'Christians arm raw data - Hasomed');

%% Testing recorder per parts 
stage_1st=dir('SAS/files/LiveData*');
stage_2nd=dir('SAS/files/filehandle*');
stage_3rd=dir('SAS/device_all_*');
stage_4th=dir('SAS/files/test_iir*');

[stage_1st_t, stage_1st_f] = samples_analysis(stage_1st,'H',0,'Raw data on LiveData function');
[stage_2nd_t, stage_2nd_f] = samples_analysis(stage_2nd,'H',0,'Raw data on Handle packet function');
[stage_3rd_t, stage_3rd_f] = samples_analysis(stage_3rd,'H',0,'Raw data on recorder structure');
[stage_4th_t, stage_4th_f] = samples_analysis(stage_4th,'C',0,'Raw data on process function');

stage_unknown=dir('SAS/files/SAStest_20201029*');
%[stageX_t, stageX_f] = samples_analysis(stage_unknown(end),'H',0,'Raw data on LiveData function (last)');
[stageX_t, stageX_f] = samples_analysis(stage_unknown(end),'H',0,'Raw data on LiveData function (last)');
% subplot(3,2,1)
% title('Recording 1')
% subplot(3,2,3)
% title('Recording a few moments later')
% subplot(3,2,5)
% title('Recording after restarting')

% This stage was fixed
stage_unknown2=dir('SAS/files/SAStest2_*');
[stageX2_t, stageX2_f] = samples_analysis(stage_unknown2(end),'H',0,'Raw data on handle global (last)');

% This stage was fixed
stage_unknown3=dir('SAS/files/SAStest3_*');
[stageX3_t, stageX3_f] = samples_analysis(stage_unknown3(end),'H',0,'Raw data on recording thread');

stage_unknown4=dir('SAS/files/SAStest4_*');
[stageX4_t, stageX4_f] = samples_analysis(stage_unknown4(end),'H',0,'Raw data from device');
[stageX4_t, stageX4_f] = samples_analysis(stage_unknown4(end),'H',1,'Filtered raw data from device');

stage_unknown5=dir('SAS/files/SAStest5_*');
[stageX5_t, stageX5_f] = samples_analysis(stage_unknown5(end),'C',0,'Raw data on processing');
[stageX5_t, stageX5_f] = samples_analysis(stage_unknown5(end-1),'C',1,'Filtered data on processing');

% Calculate the error between phases: both have to be 0
size(stageX2_t) - size(stageX3_t)
sum(stageX2_t - stageX3_t)

size(stageX3_t) - size(stageX4_t)
sum(stageX3_t - stageX4_t)

size(stageX4_t) - size(stageX5_t)
sum(abs(stageX4_t(1:length(stageX5_t)) - stageX5_t))
sum(stageX4_f(1:length(stageX5_f)) - stageX5_f)
diff = zeros(length(stageX5_t),1);
for j=1:length(stageX5_t)
    if(stageX4_t(i,1)==stageX5_t(i,1))
    end
end
%% Test on RehaIngest programe
IngeTest=dir('Matlab_scripts/Test_20201029_*');
Hasomed_addr = 'C:\Users\Carolina\Desktop\Internship\Software\Hasomed_examples\IngestTest\IngestTest';
Hasomed_test=dir([Hasomed_addr '\' 'Test_20201029_13*.txt']);

[HTest_t, HTest_f] = samples_analysis(Hasomed_test(end-2:end),'H',0,'Raw data on Hasomeds programme');
subplot(3,2,1)
title('Recording 1')
subplot(3,2,3)
title('Recording a few moments later')
subplot(3,2,5)
title('Recording another moments later')

%% Calculating thresholds:
th_dir=dir('SAS/files/CUL_arm_th_*');
full_name = [th_dir(end).folder '\' th_dir(end).name]; %Just take the last one
data = (load(full_name))';
mean_v = data(1,:);
sd_v = data(2,:);
tinit = data(3,:);
tend = data(4,:);
toff = data(5,:);
% threshold values:
y_th = mean_v(1,1) * ones(1,tend(1)-tinit(1));
t_th = zeros(1,tend(1)-tinit(1));
for i=1:length(t_th)
    t_th(i) = (tinit(1)+i)/1000;
end

y_t =[];
for i=2:length(mean_v)
    y_temp = mean_v(1,i) * ones(1,toff(1,i));
    y_t = [y_t y_temp];
end
t = zeros(1,tend(end)-tend(1));
for i=1:length(t)
    t(i) = t_th(end) + i/1000;
end

th_value = toff(1) * ones(1,tend(end)-tend(1));

t_emg1 = zeros(1,length(emg_f_t1));
for i=1:length(t_emg1)
    t_emg1(i) = i/1000;
end
t_emg2 = zeros(1,length(emg_f_t2));
for i=1:length(t_emg2)
    t_emg2(i) = i/1000;
end


figure('Name','Filtered data and mean values')
%subplot(2,1,1)
hold on
plot(t_emg, emg_f_t')
plot(t_th,y_th,'g',t,y_t,'r')
plot(t, th_value,'y')
xlim([0 t(end)]);
legend('EMG','Resting mean', 'Activity mean', 'Threshold')

%% Christians arm cw45
CUL_dir=dir('SAS/files/CUL_arm_raw_*');
[emg_raw_t1, emg_raw_f] = samples_analysis(CUL_dir(end-1),'C',0,'Raw data EMG 1');
[emg_raw_t2, emg_raw_f] = samples_analysis(CUL_dir(end),'C',0,'Raw data EMG 2');

[emg_f_t1, emg_f_f] = samples_analysis(CUL_dir(end-1),'C',1,'SAS filtered data EMG 1');
[emg_f_t2, emg_f_f] = samples_analysis(CUL_dir(end),'C',1,'SAS filtered data EMG 2');

srate = 1000;
Fn = srate/2;
Hphz = 20/Fn;
Lphz = 300/Fn;
[b,a] = butter(4,[Hphz,Lphz],'bandpass');
Cheby50 = designfilt('bandstopiir', 'PassbandFrequency1', 47, 'StopbandFrequency1', 49, 'StopbandFrequency2', 51, 'PassbandFrequency2', 53, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');
Cheby100 = designfilt('bandstopiir', 'PassbandFrequency1', 97, 'StopbandFrequency1', 99, 'StopbandFrequency2', 101, 'PassbandFrequency2', 103, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');
MatButty =filtfilt(b,a,emg_raw_t1);
MatCheby50 = filtfilt(Cheby50,MatButty);
MatCheby100 = filtfilt(Cheby100,MatCheby50);
emg_m_t1 = MatCheby100;
    t_m1 = zeros(1,length(emg_m_t1));
    for j = 1:length(t_m1)
     t_m1(j) = j/srate;
    end
    figure('Name', 'Matlab filters on EMG1')
    subplot(1,2,1)
    plot(t_m1,emg_m_t1)
    xlim([0 t_m1(end)])
    title(['Time domain EMG 1'], 'Interpreter','none'); 
    xlabel('t (s)');
    ylabel('v (V)');

    subplot(1,2,2)
    emg_m_f1 = fftEMG(emg_m_t1,['Frequency domain EMG 1'],srate);
    xlim([0 500])
    ylim([0 max(emg_m_f1)])

srate = 1000;
Fn = srate/2;
Hphz = 20/Fn;
Lphz = 300/Fn;
[b,a] = butter(4,[Hphz,Lphz],'bandpass');
Cheby50 = designfilt('bandstopiir', 'PassbandFrequency1', 47, 'StopbandFrequency1', 49, 'StopbandFrequency2', 51, 'PassbandFrequency2', 53, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');
Cheby100 = designfilt('bandstopiir', 'PassbandFrequency1', 97, 'StopbandFrequency1', 99, 'StopbandFrequency2', 101, 'PassbandFrequency2', 103, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');

MatButty =filtfilt(b,a,emg_raw_t2);
MatCheby50 = filtfilt(Cheby50,MatButty);
MatCheby100 = filtfilt(Cheby100,MatCheby50);
emg_m_t2 = MatCheby100;
    t_m2 = zeros(1,length(emg_m_t2));
    for j = 1:length(t_m2)
     t_m2(j) = j/srate;
    end
    figure('Name', 'Matlab filters on EMG2')
    subplot(1,2,1)
    plot(t_m2,emg_m_t2)
    xlim([0 t_m2(end)])
    title(['Time domain EMG 2'], 'Interpreter','none'); 
    xlabel('t (s)');
    ylabel('v (V)');

    subplot(1,2,2)
    emg_m_f2 = fftEMG(emg_m_t2,['Frequency domain EMG 2'],srate);
    xlim([0 500])
    ylim([0 max(emg_m_f2)])    
    
save('CUL_arm.mat', 'emg_raw_t1', 'emg_raw_t2', 'emg_f_t1', 'emg_f_t2', 'emg_m_t1', 'emg_m_t2', 't_emg1', 't_emg2');
    
%% Reviewing the little peak
load('CUL_arm.mat');
srate = 1000;

figure('Name','EMG 1')
% Time domain
subplot(3,2,1)
plot(t_emg1, emg_raw_t1)
xlim([0 t_emg1(end)])
title('Raw EMG 1')
subplot(3,2,3)
plot(t_emg1, emg_f_t1)
xlim([0 t_emg1(end)])
title('SAS Filters on EMG 1')
subplot(3,2,5)
plot(t_emg1, emg_m_t1)
xlim([0 t_emg1(end)])
title('Matlab filters on EMG 1')
% Frequency domain
subplot(3,2,2)
emg_raw_f1 = fftEMG(emg_raw_t1,['Frequency domain raw EMG 1'],srate);
xlim([0 500])
ylim([0 max(emg_raw_f1)])
subplot(3,2,4)
emg_f_f1 = fftEMG(emg_f_t1,['Frequency domain SAS EMG 1'],srate);
xlim([0 500])
ylim([0 max(emg_raw_f1)])
subplot(3,2,6)
emg_m_f1 = fftEMG(emg_m_t1,['Frequency domain Matlab EMG 1'],srate);
xlim([0 500])
ylim([0 max(emg_raw_f1)])

%% Cutting of peaks
load('CUL_arm.mat');
srate = 1000;
% peak 1 = t[7.5 - 10.5]
peak1 = []; % column 1 = raw, 2 = C-filtered, 3 = Matlab-filter
t_peak1 = [];
 for j = 1:length(t_emg1)
   if((t_emg1(j)>=7.5)&(t_emg1(j)<=10.5))
       t_peak1 = [t_peak1 t_emg1(j)];
       peak1 = [peak1; emg_raw_t1(j) emg_f_t1(j) emg_m_t1(j)];
   end
 end
  
figure('Name', 'Peak on EMG 1')

subplot(3,2,1)
plot(t_peak1, peak1(:,1))
xlim([t_peak1(1) t_peak1(end)])
title('Raw data on time domain')
subplot(3,2,2)
peak1_rf = fftEMG(peak1(:,1),['Raw data on frequency domain'],srate);
xlim([0 500])
ylim([0 max(peak1_rf(:,1))])

subplot(3,2,3)
plot(t_peak1, peak1(:,2))
xlim([t_peak1(1) t_peak1(end)])
title('SAS data on time domain')
subplot(3,2,4)
peak1_cf = fftEMG(peak1(:,2),['SAS data on frequency domain'],srate);
xlim([0 500])
ylim([0 max(peak1_cf(:,1))])

subplot(3,2,5)
plot(t_peak1, peak1(:,3))
xlim([t_peak1(1) t_peak1(end)])
title('Matlab data on time domain')
subplot(3,2,6)
peak1_mf = fftEMG(peak1(:,3),['Matlab data on frequency domain'],srate);
xlim([0 500])
ylim([0 max(peak1_mf(:,1))])

% peak 2 = t[1.5 - 4.0]
peak2 = []; % column 1 = raw, 2 = C-filtered, 3 = Matlab-filter
t_peak2 = [];
 for j = 1:length(t_emg2)
   if((t_emg2(j)>=1.5)&(t_emg1(j)<=4))
       t_peak2 = [t_peak2 t_emg1(j)];
       peak2 = [peak2; emg_raw_t2(j) emg_f_t2(j) emg_m_t2(j)];
   end
 end
  
figure('Name', 'Peak on EMG 2')

subplot(3,2,1)
plot(t_peak2, peak2(:,1))
xlim([t_peak2(1) t_peak2(end)])
title('Raw data on time domain')
subplot(3,2,2)
peak2_rf = fftEMG(peak2(:,1),['Raw data on frequency domain'],srate);
xlim([0 500])
ylim([0 max(peak2_rf(:,1))])

subplot(3,2,3)
plot(t_peak2, peak2(:,2))
xlim([t_peak2(1) t_peak2(end)])
title('SAS data on time domain')
subplot(3,2,4)
peak2_cf = fftEMG(peak2(:,2),['SAS data on frequency domain'],srate);
xlim([0 500])
ylim([0 max(peak2_cf(:,1))])

subplot(3,2,5)
plot(t_peak2, peak2(:,3))
xlim([t_peak2(1) t_peak2(end)])
title('Matlab data on time domain')
subplot(3,2,6)
peak2_mf = fftEMG(peak2(:,3),['Matlab data on frequency domain'],srate);
xlim([0 500])
ylim([0 max(peak2_mf(:,1))])

%%

srate = 1000;

%if(use_filter==1)
    Fn = srate/2;
    Hphz = 20/Fn;
    Lphz = 300/Fn;
    %[Hb,Ha]=butter(4,(Hphz/s.Rate),'high');
    %[Lb,La]=butter(4,(Lphz/s.Rate),'low');
    [b,a] = butter(4,[Hphz,Lphz],'bandpass');
    Cheby50 = designfilt('bandstopiir', 'PassbandFrequency1', 47, 'StopbandFrequency1', 48.5, 'StopbandFrequency2', 51.5, 'PassbandFrequency2', 53, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');
    Cheby100 = designfilt('bandstopiir', 'PassbandFrequency1', 97, 'StopbandFrequency1', 99, 'StopbandFrequency2', 101, 'PassbandFrequency2', 103, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');
    Cheby150 = designfilt('bandstopiir', 'PassbandFrequency1', 147, 'StopbandFrequency1', 149, 'StopbandFrequency2', 151, 'PassbandFrequency2', 153, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');
    Cheby200 = designfilt('bandstopiir', 'PassbandFrequency1', 197, 'StopbandFrequency1', 199, 'StopbandFrequency2', 201, 'PassbandFrequency2', 203, 'PassbandRipple1', 1, 'StopbandAttenuation', 60, 'PassbandRipple2', 1, 'SampleRate', srate, 'DesignMethod', 'cheby2');

    MatButty =filtfilt(b,a,emg_raw_t1');
    MatCheby50 = filtfilt(Cheby50,MatButty);
            MatCheby100 = filtfilt(Cheby100,MatCheby50);
            MatCheby150 = filtfilt(Cheby150,MatCheby100);
            MatCheby200 = filtfilt(Cheby200,MatCheby150);

            figure
subplot(2,2,1)
plot(t_emg1, emg_raw_t1)
xlim([t_emg1(1) t_emg1(end)])
title('Raw data on time domain')
subplot(2,2,2)
peak2_rf = fftEMG(emg_raw_t1,['Raw data on frequency domain'],srate);
xlim([0 500])
ylim([0 max(peak2_rf(:,1))])     

subplot(2,2,3)
plot(t_emg1, MatCheby200)
xlim([t_emg1(1) t_emg1(end)])
title('Matlab filtered data on time domain')
subplot(2,2,4)
peak2_rf = fftEMG(MatCheby200,['Matlab filtered data on frequency domain'],srate);
xlim([0 500])
ylim([0 max(peak2_rf)])

%% CA's arm 3rd November 2020
CA_Hasomed=dir('SAS/files/CA_arm_raw*');
CA_Cfilter=dir('SAS/files/CA_arm_filter*');
CA_th=dir('SAS/files/CA_arm_th*');

[caraw_t, caraw_f] = samples_analysis(CA_Hasomed(end),'H',0,'Raw recorded data');
[cam_t, cam_f] = samples_analysis(CA_Hasomed(end),'H',1,'Matlab filters on recorded data');

[caCraw_t, caCraw_f] = samples_analysis(CA_Cfilter(end),'C',0,'Raw recorded data');
[caCf_t, caCf_f] = samples_analysis(CA_Cfilter(end),'C',1,'SAS Filter recorded data');

size(caraw_t) - size(caCraw_t)
sum(caraw_t(1:length(caCraw_t)) - caCraw_t)

t_emg = zeros(1,length(caCf_t));
for i=1:length(t_emg)
    t_emg(i) = i/1000;
end

th_dir=dir('SAS/files/CA_arm_th_*');
full_name = [th_dir(end).folder '\' th_dir(end).name]; %Just take the last one
data = (load(full_name))';
mean_v = data(1,:);
sd_v = data(2,:);
tinit = data(3,:);
tend = data(4,:);
toff = data(5,:);
% threshold values:
y_th = mean_v(1,1) * ones(1,tend(1)-tinit(1));
t_th = zeros(1,tend(1)-tinit(1));
for i=1:length(t_th)
    t_th(i) = (tinit(1)+i)/1000;
end

y_t =[];
for i=2:length(mean_v)
    y_temp = mean_v(1,i) * ones(1,toff(1,i));
    y_t = [y_t y_temp];
end
t = zeros(1,tend(end)-tend(1));
for i=1:length(t)
    t(i) = t_th(end) + i/1000;
end

th_value = toff(1) * ones(1,tend(end)-tend(1));


figure('Name','Filtered data and mean values')
%subplot(2,1,1)
hold on
plot(t_emg, abs(caCf_t'))
plot(t_th,y_th,'g',t,y_t,'r')
plot(t, th_value,'y')
xlim([0 t(end)]);
legend('EMG','Resting mean', 'Activity mean', 'Threshold')

%% For dirk
Hasomed_arm = dir('Matlab_scripts/Test_20201103*');
[data_t, data_f] = samples_analysis(Hasomed_arm,'H',0,'RehaIngest raw data');
[data_t, data_f] = samples_analysis(Hasomed_arm,'H',1,'RehaIngest filtered data with Matlab');