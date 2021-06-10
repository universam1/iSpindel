# 数据上传到 FHEM


 iSpindel 可以将数据传输到本地的 [FHEM](http://fhem.de/fhem.html) 服务器上，在那里可以对数据进行可视化和进一步处理 (例如，控制发酵温度)。
FHEM 通过 Http Get 请求获取数据:

    http://server:8083/fhem?cmd.Test=set%20iSpindel%2011.5%2022.3%203.3&XHR=1

目标地址和端口 (服务器: 8083) 以及设备的名称 (这里是 "iSpindel") 必须在 spindel 的 configurationUI 里设置好。所以要传输的数据都通过 Get 在 URL 中进行传输。顺序是 倾斜角度 - 温度 电量。空格必须用 % 20 替代。

在 FHEM (ie fhem.cfg)，设备必须创建一个名称 (这里是："iSpindel")以及一些 Perl logic 运算符，以便将数据记录拆分为各个值。产生的数据结果被分配给设备 iSpindelData 并且从那里进一步处理。


 ```
    define iSpindel dummy
    define iSpindelData dummy
    
    define iSpindel_Notify notify iSpindel {  \
     my @x= split(/\s/,ReadingsVal("iSpindel","state",0));; \
     my $Neigung=$x[0];; \
     my $Temp=$x[1];; \
     my $Battery=$x[2];; \
     my $Plato=CalcPlato($Neigung, $Temp);; \
     fhem "setreading  iSpindelData Neigung $Neigung";;\
     fhem "setreading  iSpindelData Temp $Temp";;\
     fhem "setreading  iSpindelData Battery $Battery";;\
     fhem "setreading  iSpindelData Plato $Plato";;\
    }
    
    define iSpindel_Values readingsGroup iSpindelData:Temp iSpindelData:Neigung iSpindelData:Battery iSpindelData:Plato
    attr iSpindel_Values room Bier
    
    define FileLog_iSpindel FileLog ./log/iSpindel-%Y.log iSpindelData.*
    attr FileLog_iSpindel logtype text
    
    define weblink_iSpindel SVG FileLog_iSpindel:SVG_FileLog_iSpindel_1:CURRENT
    attr weblink_iSpindel room Bier
```

函数 CalcPlato (曲线斜率，温度) 可以存储在 99_MyUtils.pm:

```
    sub CorrectPlato($$)  #(P, T)
    {
      my $mPlato=shift;
      my $Temp=shift;
      my @k=(56.084,   -0.17885,   -0.13063);
      if ($mPlato>=5) { @k=(69.685,    -1.367,      -0.10621);};
      if ($mPlato>=10){  @k=(77.782,    -1.7288,    -0.10822);};
      if ($mPlato>=15){  @k=(87.895,    -2.3601,    -0.10285);};
      if ($mPlato>=20){  @k=(97.052,    -2.7729,    -0.10596);};
    
      my $cPlato=$k[0]+$k[1]*$Temp+$k[2]*$Temp*$Temp;
      return $mPlato-$cPlato/100;
    }
    
    sub CalcPlato($$)   #(Angle, T)
    {
      my $tilt=shift;
      my $Temp=shift;
      # Get this from Excel Calibration at 20 Degrees 
      my $mPlato=0.00438551*($tilt)*($tilt) + 0.13647658*($tilt) - 6.968821422;
      return CorrectPlato($mPlato,$Temp);
    }
```

 $mPlato 的公式必须取自校准样本表，CorrectPlato () 还用于对 Platower 进行温度校正 (需要在 20 度下进行校准)。温度校正在 5 到 30 摄氏度的范围内有效。

最后，只剩下图表中的设备测量数值还需定义，使用 FHEM -GPlot 编辑器很容易做到这一点，看起来像这样：
![Definition iSpindel Diagram](/pics/FHEM-iSpindel-gplot.png)

定义 iSpindel 图表

    attr WEB endPlotNow 1


