function [data] = getdata(file)
%This function reads the saved data in a file, and ommits the unuseful
%lines (that do not match the file format or is not numeric).
% [data] = getdata(file)
%- file = file's name string. It must include file folder if the file 
%         it is not in Matlab workspace.
%- data = output matrix with the containing data in the file.
% e.g.:     recorded_signal = getdata('folder1/example.txt');

fid = fopen(file, 'rt') ;              % Open source file.
v = [];
temp = [];
% Filter data from file
while ~feof(fid)
    read_line = fgetl(fid) ;   
    %disp(tline)
    comp = strsplit(read_line, ',');
    [s1, s2] = size( comp );
    if (s2>=2)   % filter not-good lines 
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

