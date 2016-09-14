//
// Copyright (C) 2014 - A. Khorshidi <akhorshidi@live.com>
//
// This file is distributed in the hope that it will be useful; 
// It must be used under the terms of the CeCILL.
// http://www.cecill.info/licences/Licence_CeCILL_V2.1-en.txt
//
//

// The following work provided the inspiration for this challenge. 
// https://www.scilab.org/content/view/full/847
//
// I owe thanks to Bruno Jofret, the author of the original GUI.
// https://fileexchange.scilab.org/toolboxes/270000  
//

ind = x_choose(["RS-232" ;"USB"; "Ethernet" ;"Wireless"],["Please select the type of communication interface: ";"Just double-click on its name. "],"Cancel");
if ind==0 then
    msg=_("ERORR: No types of communication interfaces has been chosen. ");
    messagebox(msg, "ERROR", "error");
    error(msg);
    return;
elseif ind==2
    if (getos() == "Windows") then
        if ~(atomsIsInstalled('serial')) then
            msg=_("ERROR: A serial communication toolbox must be installed.");
            messagebox(msg, "Error", "error");
            error(msg);
            return;
        else
            flag=1;
        end    
    elseif  (getos() == "Linux") then
        if ~(atomsIsInstalled('serialport')) & ~(atomsIsInstalled('serial')) then
            msg=_("ERROR: A serial communication toolbox must be installed.");
            messagebox(msg, "Error", "error");
            error(msg);
            return;
        elseif (atomsIsInstalled('serialport')) & (atomsIsInstalled('serial')) then
            stoolbx = x_choose(['serialport';'serial' ],"Which serial ...
            commiunication toolbox you prefer to use? "," Cancel ")
            if  stoolbx==1 then
                flag=2;
            elseif stoolbx==2 then
                flag=3;
            else
                msg=_("ERROR: No serial toolbox has been chosen. ");
                messagebox(msg, "Error", "error");
                error(msg);
                return;
            end
        elseif (atomsIsInstalled('serialport')) then
            flag=2;
        elseif  (atomsIsInstalled('serial')) then 
            flag=3;     
        end
    else
        msg=_(["WARNING: This program has been tested and works under Gnu/Linux ...
        and Windows."; "On other platforms you may need modify this script. "])
        messagebox(msg, "WARNING", "warning");
        warning(msg);
        return;
    end
else 
    error("Not possible yet.");
    return;
end
//
if (getos() == "Linux") then
    [rep,stat,stderr]=unix_g("ls /dev/ttyACM*");
    if stderr ~= emptystr() then
        msg=_(["No USB device found. ";"Check your USB connection or try ...
            another port. "])
        messagebox(msg, "ERROR", "error");
        error(msg);
        return;
    end
    ind = x_choose(rep,["Please specify which USB port you wanna use for ...
        communication. ";"Just double-click on its name. "],"Cancel");  
    if ind==0 then
        msg=_("ERORR: No serial port has been chosen. ");
        messagebox(msg, "ERROR", "error");
        error(msg);
        return;
    end
    port_name = rep(ind);
end
if (getos() == "Windows") then
    port_name=evstr(x_dialog('Please enter COM port number: ','13'))
    if port_name==[] then
        msg=_("ERORR: No serial port has been chosen. ");
        messagebox(msg, "ERROR", "error");
        error(msg);
        return;
    end
end
//
global %serial_port
if flag==2 then
    %serial_port = serialopen(port_name, 9600, 'N', 8, 1);
    while %serial_port == -1
        btn=messagebox(["Please check your USB connection, and then click on ...
        Try again. "; "To choose another port click on Change. "], "Error", ...
        "error", [" Try again " " Change "], "modal");
        if ~btn==1 then
            [rep,stat,stderr]=unix_g("ls /dev/ttyACM*");
            ind = x_choose(rep,["Please specify which USB port you wanna use...
            for communication. ";"Just double-click on its name. "],"Cancel");
        if ind==0 then
            msg=_("ERORR: No serial port has been chosen. ");
            messagebox(msg, "ERROR", "error");
            error(msg);
            return;
        end
        port_name = rep(ind);    
        end
        %serial_port = serialopen(port_name, 9600, 'N', 8, 1);
    end
elseif flag==1 | flag==3
    %serial_port=openserial(port_name,"9600,n,8,1");
    //error(999)
else
    msg=_("ERROR: Could not specify which serial toolbox to use. ");
    messagebox(msg, "Error", "error");
    error(msg);
    return;
end
//
// * Monitoring Phase:
//
global %MaxTemp
%MaxTemp = 35;
global %MinTemp
%MinTemp = 30;
f=figure("dockable","off");
f.resize="off";
f.menubar_visible="off";
f.toolbar_visible="off";
f.figure_name="Real-time Temperature Monitoring and Control";
f.tag="mainWindow";
bar(.5,0,'blue');
e = gce();
e = e.children(1);
e.tag = "instantSensor";
//
plot([0, 1], [%MinTemp, %MinTemp]);
e = gce();
e = e.children(1);
e.tag = "instantMinTemp";
e.line_style = 5;
e.thickness = 2;
e.foreground = color("orange");
//
plot([0, 3], [%MaxTemp, %MaxTemp]);
e = gce();
e = e.children(1);
e.tag = "instantMaxTemp";
e.line_style = 5;
e.thickness = 2;
e.foreground = color("red");
a = gca();
a.data_bounds = [0, 0; 1, 45];
a.grid = [-1, color("darkgrey")];
a.axes_bounds = [0.1, 0.2, 0.25, 0.85];
a.axes_visible(1) = "off";
a.tag = "liveAxes";
//a.title.text="Current Temperature";
//
f.figure_position = [0 0];
f.figure_size = [1000 700];
f.background = color(246,244,242) //color("darkgrey")
//
minTempSlider = uicontrol("style", "slider", "position", [60 30 30 440], ...
"min", 0, "max", 45, "sliderstep", [1 5], "value" , %MinTemp, ...
"callback", "changeMinTemp", "tag", "minTempSlider");
maxTempSlider = uicontrol("style", "slider", "position", [20 30 30 440], ...
"min", 0, "max", 45, "sliderstep", [1 5], "value" , %MaxTemp, ...
"callback", "changeMaxTemp", "tag", "maxTempSlider");
//
// Functions: 
function changeMinTemp()
    global %MinTemp
    e = findobj("tag", "minTempSlider");
    %MinTemp = e.value //45 - e.value;
    e = findobj("tag", "instantMinTemp");
    e.data(:,2) = %MinTemp;
endfunction
//
function changeMaxTemp()
    global %MaxTemp
    e = findobj("tag", "maxTempSlider");
    %MaxTemp = e.value //45 - e.value;
    e = findobj("tag", "instantMaxTemp");
    e.data(:,2) = %MaxTemp;
endfunction
//
function closeFigure()
    
    stopSensor();
    global %serial_port
    if flag == 2 then
       serialclose(%serial_port);
    elseif flag == 1 | flag == 3 then
        closeserial(%serial_port);
    end
    f = findobj("tag", "mainWindow");
    delete(f);
endfunction
//
function stopSensor()
    global %Acquisition
    %Acquisition = %f;
endfunction
//
function launchSensor()
    global %MaxTemp
    global %serial_port
    global %Acquisition
    %Acquisition = %t;
    global %fanStatus
    %fanStatus = 0;
    // Arduino toolbox
    values=[];
    value=ascii(0);
    while %Acquisition 
       while(value~=ascii(13)) then
           if  flag == 2 then
               
                value=serialread(%serial_port,1);
           elseif flag == 1 | flag == 3 then
                value=readserial(%serial_port,1);
           end
        values=values+value;
        v=strsubst(values,string(ascii(10)),'')
        v=strsubst(v,string(ascii(13)),'')
        data=evstr(v)
        end
    //
    xinfo("Temperature = "+v+"°C");
    values=[]
    value=ascii(0);
    updateSensorValue(data);
    //
     global %RegulationEnable
    if  %RegulationEnable == 1 then
        if data > %MaxTemp then
            enableFan();
        else
            disableFan();
        end
    end
    updateFanValue(%fanStatus);
end
endfunction
//
function updateSensorValue(data)
    global %MaxTemp
    global %MinTemp
    e = findobj("tag", "instantSensor");
    e.data(2) = data;
    if data > %MaxTemp then
        e.background = color("red");
    else
        if data > %MinTemp  then
            e.background = color("orange");
        else
            e.background = color("green");
        end
    end
    //
    e = findobj("tag", "minuteSensor");
    lastPoints = e.data(:, 2);
    e.data(:, 2) = [lastPoints(2:$) ; data];
    e = findobj("tag", "hourSensor");
    lastPoints = e.data(:, 2);
    e.data(:, 2) = [lastPoints(2:$) ; data];
endfunction
//
// * Regulation Phase:
//
global %RegulationEnable
%RegulationEnable = 1;
global %PController
%PController = 0;
global %PIController
%PIController = 0;
global %PIDController
%PIDController = 0;
//
top_axes_bounds = [0.25 0 0.8 0.5];
bottom_axes_bounds = [0.25 0.5 0.8 0.5];
minTempDisplay = 20;
maxTempDisplay = 45;
minRegulationDisplay = -0.2;
maxRegulationDisplay = 1.2;
// Temperature variations in the last 5 minutes
timeBuffer = 300;
subplot(222);
a = gca();
a.axes_bounds = top_axes_bounds;
a.tag = "minuteAxes";
plot2d(0:timeBuffer, zeros(1,timeBuffer + 1), color("red"));
a.title.text="Temperature variations in the last 5 minutes";
a.data_bounds = [0, minTempDisplay; timeBuffer, maxTempDisplay];
e = gce();
e = e.children(1);
e.tag = "minuteSensor";
// adding a second vertical axis on the right side ...
// to show the On/Off status of the DC Fan.
a = newaxes();
a.y_location = "right";
a.filled = "off" 
a.axes_bounds = top_axes_bounds;
plot2d(0:timeBuffer, zeros(1,timeBuffer + 1), color("blue"));
a.data_bounds = [0, minRegulationDisplay; timeBuffer, maxRegulationDisplay];
a.axes_visible(1) = "off";
a.foreground=color("blue");
a.font_color=color("blue");
e = gce();
e = e.children(1);
e.tag = "minuteRegulation";
// Temperature variations in the last hour
timeBuffer = 4000;
subplot(224);
a = gca();
a.axes_bounds = bottom_axes_bounds;
a.tag = "hourAxes";
plot2d(0:timeBuffer, zeros(1,timeBuffer + 1), color("red"));
a.title.text="Temperature variations in the last hour";
a.data_bounds = [0, minTempDisplay; timeBuffer, maxTempDisplay];
e = gce();
e = e.children(1);
e.tag = "hourSensor";
// 2nd vertical axis
a = newaxes();
a.y_location = "right";
a.filled = "off" 
a.axes_bounds = bottom_axes_bounds;
a.axes_visible = "off";
plot2d(0:timeBuffer, zeros(1,timeBuffer + 1), color("blue"));
a.data_bounds = [0, minRegulationDisplay; timeBuffer, maxRegulationDisplay];
a.axes_visible(1) = "off";
a.foreground=color("blue");
a.font_color=color("blue");
e = gce();
e = e.children(1);
e.tag = "hourRegulation";
//
// Functions: 
function resetDisplay()
    e = findobj("tag", "instantSensor");
    e.data(:, 2) = 0;
    e = findobj("tag", "minuteSensor");
    e.data(:, 2) = 0;
    e = findobj("tag", "hourSensor");
    e.data(:, 2) = 0;
    e = findobj("tag", "minuteRegulation");
    e.data(:, 2) = 0;
    e = findobj("tag", "hourRegulation");
    e.data(:, 2) = 0;
endfunction
//
function changeRegulationStatus()
    global %RegulationEnable
    e = findobj("tag", "enableRegulationCBO");
    %RegulationEnable = e.value;
    if %RegulationEnable == 0 then
        disableFan();
    end
endfunction
//
function updateFanValue(data)
    e = findobj("tag", "minuteRegulation");
    lastPoints = e.data(:, 2);
    e.data(:, 2) = [lastPoints(2:$) ; data];
    e = findobj("tag", "hourRegulation");
    lastPoints = e.data(:, 2);
    e.data(:, 2) = [lastPoints(2:$) ; data];
endfunction
//
function enableFan()
    global %serial_port
    if  flag == 2 then
        serialwrite(%serial_port,'H');
    elseif flag == 1 | flag == 3 then
        writeserial(%serial_port,ascii(72));
    end
    global %fanStatus
    %fanStatus = 1;
endfunction
//
function disableFan()
    global %serial_port
    if  flag == 2 then
        serialwrite(%serial_port,ascii(76));
    elseif flag == 1 | flag == 3 then
        writeserial(%serial_port,"L");
    end
    global %fanStatus
    %fanStatus = 0;
endfunction
//
// Buttons: 
// * Main Panel
mainFrame = uicontrol(f, "style", "frame", "position", [15 560 305 80], ...
"tag", "mainFrame", "ForegroundColor", [0/255 0/255 0/255],...
"border", createBorder("titled", createBorder("line", "lightGray", 1)...
, _("Main Panel"), "center", "top", createBorderFont("", 11, "normal"), ...
"black"));
//
startButton = uicontrol(f, "style", "pushbutton", "position", ...
[20 595 145 30], "callback", "launchSensor", "string", "Start Acquisition", ...
"tag", "startButton");
//
stopButton = uicontrol(f, "style", "pushbutton", "position", ...
[170 595 145 30], "callback", "stopSensor", "string", "Stop Acquisition", ...
"tag", "stopButton");
//
resetButton = uicontrol(f, "style", "pushbutton", "position", ...
[20 565 145 30], "callback", "resetDisplay", "string", "Reset", ...
"tag", "resetButton");
//
quitButton = uicontrol(f, "style", "pushbutton", "position", ...
[170 565 145 30], "callback", "closeFigure", "string", "Quit", ...
"tag", "quitButton");
//
RegulationFrame = uicontrol(f, "style", "frame", "position", [15 490 305 65]...
,"tag", "mainFrame", "ForegroundColor", [0/255 0/255 0/255],...
"border", createBorder("titled", createBorder("line", "lightGray", 1), ...
_("Regulation Mode"), "center", "top", createBorderFont("", 11, "normal"),...
 "black"));
//
// * Regulation Mode
enableRegulation = uicontrol(f, "style", "checkbox", "position", ...
[20 520 140 20],"string", "ON/OFF", "value", %RegulationEnable, ...
"callback", "changeRegulationStatus", "tag", "enableRegulationCBO");
//
enableP = uicontrol(f, "style", "checkbox", "position", [20 500 140 20], ...
"string", "P Controller", "value", %PController, ...
"callback", "", "tag", "");
//
enablePI = uicontrol(f, "style", "checkbox", "position", [170 520 140 20], ...
"string", "PI Controller", "value", %PIController, ...
"callback", "", "tag", "");
//
enablePID = uicontrol(f, "style", "checkbox", "position", [170 500 140 20], ...
"string", "PID Controller", "value", %PIDController, ...
"callback", "", "tag", "");
//
