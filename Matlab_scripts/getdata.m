function [data] = getdata(file)
%Clean data from file so it does not look so shit
%   Detailed explanation goes here

fid = fopen(file, 'rt') ;              % Open source file.
v = [];
temp = [];
% Filter data from file
while ~feof(fid)
    read_line = fgetl(fid) ;   
    %disp(tline)
    comp = strsplit(read_line, ',');
    [s1, s2] = size( comp );
    if (s2>=3)   % filter not-good lines 
        v = [v; comp];
    end 
end
fclose(fid); % Close original file 

[m n] = size(v);
for i = 1:m
    temp_row = [];
    for j = 1:n
    temp_row=[temp_row str2double(v{i,j})];
    end
    temp=[temp; temp_row];
end

data = temp;
end

