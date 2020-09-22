%% Live session EMG data
% This section is on pause until new stuff 
%Create socket
pause on
PORT = 30002;
SERVER = '127.0.0.1';       %LocalIP address
echoudp("on",PORT)          %Start server
%SAS = udp(SERVER,PORT);
%set(SAS,'TimeOut',2);
fopen(SAS);
while(true)
%for i=1:1000
    %Send a request message:
   % msg = 'Req';
   % fwrite(SAS,msg);
    %Read a message
    data = char(fread(SAS));
    disp(data);
    pause(0.1);
end

echoudp("off");             %Finish server