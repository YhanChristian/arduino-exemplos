//  Implantação de Controle de Temperatura c/ Arduino e Scilab
//  Utilizado: Arduino Uno, LM35, Motor DC
//  Autor: Yhan Christian Souza Silva - Data: 14/09/2016
//  Referências: https://www.scilab.org/community/news/Scilab-Arduino-low-cost-data-acquisition
//	         https://www.scilab.org/content/view/full/847
//	         https://fileexchange.scilab.org/toolboxes/270000  


// Define dispositivo a se comunicar, USB ou RS-232

ind = x_choose(["RS-232" ;"USB"],["Selecione o tipo de comunicação: ";"Apenas de um duplo clique"],"Cancelar");

//Verifica se não houve seleção do tipo de comunicação acima, retornando mensagem de error

if ind == 0 then
    msg=_("ERRO. Nenhum tipo de comunicação foi solecionado ");
    messagebox(msg, "ERRO", "error");
    error(msg);
    return;

//Verifica se o SO é Windows ou Linux e se o serial toolbox não está instalado, retornando mensagem de error

elseif ind == 2
    if (getos() == "Windows") then
        if ~(atomsIsInstalled('serial')) then
            msg=_("ERRO: O serial toolbox necessita ser instalado.");
            messagebox(msg, "ERRO", "error");
            error(msg);
            return;
        else
            flag = 1;
        end    
    elseif  (getos() == "Linux") then
        if ~(atomsIsInstalled('serialport')) & ~(atomsIsInstalled('serial')) then
            msg=_("ERRO: O serial toolbox necessita ser instalado.");
            messagebox(msg, "ERRO", "error");
            error(msg);
            return;
        elseif (atomsIsInstalled('serialport')) & (atomsIsInstalled('serial')) then
            stoolbx = x_choose(['serialport';'serial' ],"Qual ferramenta serial ...
             para comunicação você prefere? "," Cancelar ")
            if  stoolbx == 1 then
                flag = 2;
            elseif stoolbx == 2 then
                flag = 3;
            else
                msg=_("ERRO: Nenhum serial selecionado ");
                messagebox(msg, "ERRO", "error");
                error(msg);
                return;
            end
        elseif (atomsIsInstalled('serialport')) then
            flag = 2;
        elseif  (atomsIsInstalled('serial')) then 
            flag = 3;     
        end 
  else
        msg=_(["ATENÇÃO: Este programa foi testado e funciona no GNU / Linux ...
        e Windows"; "em outras plataformas pode ser necessário modificar este script "])
        messagebox(msg, "ATENÇÃO", "warning");
        warning(msg);
        return;
    end
else 
    error("Não é possível ainda comunicar.");
    return;
end

if (getos() == "Linux") then
    [rep,stat,stderr]=unix_g("ls /dev/ttyACM*");
    if stderr ~= emptystr() then
        msg=_(["Nenhum dispositivo USB encontrado.  ";"Verifique sua conexão USB ou tente ...
            outra porta "])
        messagebox(msg, "ERRO", "error");
        error(msg);
        return;
    end
    ind = x_choose(rep,["Por favor, especifique qual porta USB você quer usar para ...
        comunicação. ";"Apenas de um duplo clique."],"Cancelar");  
    if ind == 0 then
        msg=_("ERRO: Nenhuma porta serial foi escolhida. ");
        messagebox(msg, "ERRO", "error");
        error(msg);
        return;
    end
    porta = rep(ind);
end
if (getos() == "Windows") then
    porta = evstr(x_dialog('Por favor, indique o número da porta COM: ','13'))
    if porta == [] then
        msg=_("ERRO: Nenhuma porta serial foi escolhida.");
        messagebox(msg, "ERRO", "error");
        error(msg);
        return;
    end
end

//Verifica se a comunicação foi realizada com sucesso, enquanto não houver comunicação
//Tentar novamente ou Alterar porta USB.

global %porta_serial
if flag == 2 then
    %porta_serial = serialopen(porta, 9600, 'N', 8, 1);
    while %porta_serial == -1
        btn=messagebox(["Por favor verifique a sua ligação USB e clique em...
        Tentar novamente. "; "Para escolher outra porta clique em Alterar."], "ERRO", ...
        "error", [" Tentar novamente " " Alterar "], "modal");
        if ~btn == 1 then
            [rep,stat,stderr] = unix_g("ls /dev/ttyACM*");
            ind = x_choose(rep,["Por favor, especifique qual porta USB você quer usar ...
            para comunicação ";"Apenas de um duplo clique"],"Cancelar");
        if ind == 0 then
            msg=_("ERRO: Nenhuma porta serial foi escolhida.");
            messagebox(msg, "ERRO", "error");
            error(msg);
            return;
        end
        porta = rep(ind);    
        end
        %porta_serial = serialopen(porta, 9600, 'N', 8, 1);
    end
elseif flag == 1 | flag == 3
    %porta_serial=openserial(porta,"9600,n,8,1");
    //ERRO(999)
else
    msg=_("ERRO: Não foi possível identificar o Serial Toolbox ");
    messagebox(msg, "ERRO", "error");
    error(msg);
    return;
end

// * Etapa de monitoramento!


//Variaveis temperatura máxima e mínima
global %temperatura_maxima
%temperatura_maxima = 35;
global %temperatura_minima
%temperatura_minima = 30;

// *Layout e plotagem do gráfico

f=figure("dockable","off");
f.resize="off";
f.menubar_visible="off";
f.toolbar_visible="off";
f.figure_name="Controle e monitoramento de temperatura em TEMPO REAL - Engenheiro Caiçara";
f.tag="janela_principal";
bar(.5,0,'blue');
e = gce();
e = e.children(1);
e.tag = "sensor_inst";
//
plot([0, 1], [%temperatura_minima, %temperatura_minima]);
e = gce();
e = e.children(1);
e.tag = "mintemp_inst";
e.line_style = 5;
e.thickness = 2;
e.foreground = color("yellow");
//
plot([0, 3], [%temperatura_maxima, %temperatura_maxima]);
e = gce();
e = e.children(1);
e.tag = "maxtemp_inst";
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

//Barra p/ definir temp min e temp max
tempmin_barra = uicontrol("style", "slider", "position", [60 30 30 440], ...
"min", 0, "max", 45, "sliderstep", [1 5], "value" , %temperatura_minima, ...
"callback", "mudaTempMin", "tag", "tempmin_barra");
tempmax_barra = uicontrol("style", "slider", "position", [20 30 30 440], ...
"min", 0, "max", 45, "sliderstep", [1 5], "value" , %temperatura_maxima, ...
"callback", "mudaTempMax", "tag", "tempmax_barra");
//

//*Funcoes:

 
//alterar temperatura minima
function mudaTempMin()
    global %temperatura_minima
    e = findobj("tag", "tempmin_barra");
    %temperatura_minima = e.value //45 - e.value;
    e = findobj("tag", "mintemp_inst");
    e.data(:,2) = %temperatura_minima;
endfunction

//alterar temperatura máxima
function mudaTempMax()
    global %temperatura_maxima
    e = findobj("tag", "tempmax_barra");
    %temperatura_maxima = e.value //45 - e.value;
    e = findobj("tag", "maxtemp_inst");
    e.data(:,2) = %temperatura_maxima;
endfunction

//função p/ fechar janela
function fecharFigura()
    
    paraSensor();
    global %porta_serial
    if flag == 2 then
       serialclose(%porta_serial);
    elseif flag == 1 | flag == 3 then
        closeserial(%porta_serial);
    end
    f = findobj("tag", "janela_principal");
    delete(f);
endfunction

//funcao p/ para leitura sensor
function paraSensor()
    global %aquisicao
    %aquisicao = %f;
endfunction

//Aquisição e exibição da leitura do sensor de temperatura

function lerSensor()
    global %temperatura_maxima
    global %porta_serial
    global %aquisicao
    %aquisicao = %t;
    global %status_carga
    %status_carga = 0;
    values=[];
    value=ascii(0);
    while %aquisicao 
       while(value~=ascii(13)) then
           if  flag == 2 then
               
                value=serialread(%porta_serial,1);
           elseif flag == 1 | flag == 3 then
                value = readserial(%porta_serial,1);
           end
        values=values+value;
        v=strsubst(values,string(ascii(10)),'')
        v=strsubst(v,string(ascii(13)),'')
        data=evstr(v)
        end
    xinfo("Temperatura = "+v+"°C");
    values=[]
    value=ascii(0);
    atualizaValorSensor(data);

//Se temperatura estiver acima da máxima aciono carga, se não mantenho desligada
     global %habilita_regulagem
    if  %habilita_regulagem == 1 then
        if data > %temperatura_maxima then
            ligaCarga();
        else
            desligaCarga();
        end
    end
    atualizaValorCarga(%status_carga);
end
endfunction

// Função para atualização dos dados lidos pelo sensor
// Gráfico vermelho temperatura acima da máxima
// Gráfico amarelo temperatura acima da temperatura mínima, mas abaixo da máxima
// Gráfico verde condições normais de trabalho

function atualizaValorSensor(data)
    global %temperatura_maxima
    global %temperatura_minima
    e = findobj("tag", "sensor_inst");
    e.data(2) = data;
    if data > %temperatura_maxima then
        e.background = color("red");
    else
        if data > %temperatura_minima  then
            e.background = color("yellow");
        else
            e.background = color("green");
        end
    end
    //
    e = findobj("tag", "minutoSensor");
    lastPoints = e.data(:, 2);
    e.data(:, 2) = [lastPoints(2:$) ; data];
    e = findobj("tag", "horaSensor");
    lastPoints = e.data(:, 2);
    e.data(:, 2) = [lastPoints(2:$) ; data];
endfunction

// * Regulagem e controle PID (ProporcionalIntegral Derivativo)


// Variaveis de controle 
global %habilita_regulagem
%habilita_regulagem = 1;
global %P
%P = 0;
global %PI
%PI = 0;
global %PID
%PID = 0
top_axes_bounds = [0.25 0 0.8 0.5];
bottom_axes_bounds = [0.25 0.5 0.8 0.5];
minTempDisplay = 20;
maxTempDisplay = 45;
minRegulationDisplay = -0.1;
maxRegulationDisplay = 1.5;

// Variação nos últimos 5 minutos
timeBuffer = 300;
subplot(222);
a = gca();
a.axes_bounds = top_axes_bounds;
a.tag = "eixosMinuto";
plot2d(0:timeBuffer, zeros(1,timeBuffer + 1), color("orange"));
a.title.text="Variação de temperatura nos ultimos 5 minutos";
a.data_bounds = [0, minTempDisplay; timeBuffer, maxTempDisplay];
e = gce();
e = e.children(1);
e.tag = "minutoSensor";

// adição de um segundo eixo vertical do lado direito ...
// p/ exibir status ON/OFF da carga

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
e.tag = "regulagemMinuto";
// Variação na última hora
timeBuffer = 4000;
subplot(224);
a = gca();
a.axes_bounds = bottom_axes_bounds;
a.tag = "eixosHora";
plot2d(0:timeBuffer, zeros(1,timeBuffer + 1), color("red"));
a.title.text="Variação de temperatura na última hora";
a.data_bounds = [0, minTempDisplay; timeBuffer, maxTempDisplay];
e = gce();
e = e.children(1);
e.tag = "horaSensor";
// Eixo verticual
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
e.tag = "regulacaoHora";

//* Funções

//limpa display e reinicia leitura
function limpaDisplay()
    e = findobj("tag", "sensor_inst");
    e.data(:, 2) = 0;
    e = findobj("tag", "minutoSensor");
    e.data(:, 2) = 0;
    e = findobj("tag", "horaSensor");
    e.data(:, 2) = 0;
    e = findobj("tag", "regulagemMinuto");
    e.data(:, 2) = 0;
    e = findobj("tag", "regulacaoHora");
    e.data(:, 2) = 0;
endfunction

//altera status regulagem
function alteraStatusRegulagem()
    global %habilita_regulagem
    e = findobj("tag", "enableRegulationCBO");
    %habilita_regulagem = e.value;
    if %habilita_regulagem == 0 then
        desligaCarga();
    end
endfunction

//atualiza valor da carga baseado nos dados lidos
function atualizaValorCarga(data)
    e = findobj("tag", "regulagemMinuto");
    lastPoints = e.data(:, 2);
    e.data(:, 2) = [lastPoints(2:$) ; data];
    e = findobj("tag", "regulacaoHora");
    lastPoints = e.data(:, 2);
    e.data(:, 2) = [lastPoints(2:$) ; data];
endfunction

//liga motor DC ou outra carga alimentada no Arduino
function ligaCarga()
    global %porta_serial
    if  flag == 2 then
        serialwrite(%porta_serial,'H');
    elseif flag == 1 | flag == 3 then
        writeserial(%porta_serial,ascii(72));
    end
    global %status_carga
    %status_carga = 1;
endfunction

//desliga motor DC ou outra carga alimentada no Arduino
function desligaCarga()
    global %porta_serial
    if  flag == 2 then
        serialwrite(%porta_serial,ascii(76));
    elseif flag == 1 | flag == 3 then
        writeserial(%porta_serial,"L");
    end
    global %status_carga
    %status_carga = 0;
endfunction

// Botões

mainFrame = uicontrol(f, "style", "frame", "position", [15 560 305 80], ...
"tag", "mainFrame", "ForegroundColor", [0/255 0/255 0/255],...
"border", createBorder("titled", createBorder("line", "lightGray", 1)...
, _("Painel Principal"), "center", "top", createBorderFont("", 11, "normal"), ...
"black"));

startButton = uicontrol(f, "style", "pushbutton", "position", ...
[20 595 145 30], "callback", "lerSensor", "string", "Iniciar Aquisição", ...
"tag", "startButton");

stopButton = uicontrol(f, "style", "pushbutton", "position", ...
[170 595 145 30], "callback", "paraSensor", "string", "Parar Aquisição", ...
"tag", "stopButton");

resetButton = uicontrol(f, "style", "pushbutton", "position", ...
[20 565 145 30], "callback", "limpaDisplay", "string", "Reset", ...
"tag", "resetButton");

quitButton = uicontrol(f, "style", "pushbutton", "position", ...
[170 565 145 30], "callback", "fecharFigura", "string", "Fechar", ...
"tag", "quitButton");

RegulationFrame = uicontrol(f, "style", "frame", "position", [15 490 305 65]...
,"tag", "mainFrame", "ForegroundColor", [0/255 0/255 0/255],...
"border", createBorder("titled", createBorder("line", "lightGray", 1), ...
_("Modo de Regulagem"), "center", "top", createBorderFont("", 11, "normal"),...
 "black"));

// * Regulagem e controle PID (ProporcionalIntegral Derivativo)

enableRegulation = uicontrol(f, "style", "checkbox", "position", ...
[20 520 140 20],"string", "ON/OFF", "value", %habilita_regulagem, ...
"callback", "alteraStatusRegulagem", "tag", "enableRegulationCBO");

enableP = uicontrol(f, "style", "checkbox", "position", [20 500 140 20], ...
"string", "Controle P", "value", %PI, ...
"callback", "", "tag", "");
//
enablePI = uicontrol(f, "style", "checkbox", "position", [170 520 140 20], ...
"string", "Controle PI", "value", %PI, ...
"callback", "", "tag", "");
//
enablePID = uicontrol(f, "style", "checkbox", "position", [170 500 140 20], ...
"string", "Controle PID", "value", %PID, ...
"callback", "", "tag", "");
//
