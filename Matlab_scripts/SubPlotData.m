function SubPlotData(dataY,dataX,name)
%This functions plots data in one single figure, but divided in subplots
% SubPlotData(dataY,dataX,name)
%-dataY = Nxn matrix data on the Y axis.
%           N: amount of variables = how many plots there will be.
%           n: amount of samples.
%-dataX = data on the X axis. Size: 1xn vector (usually just time).
%-name = string. Size: Nx2 matrix. 3 strings per variable, 1 for the title
            % and 1 to show the units on the Y axis.
% e.g.:  SubPlotData(matrix_1xn,time_1xn,['Recording 1' 'Signal 1 [V]' 'time [s]']);
            
% Calculate total data sizes:
[sizeplot sizedata] = size(dataY);
sizesub = round(sizeplot/2);
% Plot stuff
%figure
    for i = 1:sizeplot
       subplot(sizesub, sizesub, i)
       grid on
       plot(dataX,dataY(i,:));
       title(name(i,1));
       ylabel(name(i,2));
       xlabel(name(i,3));
       xlim([0 dataX(1,length(dataX))])
    end
end

